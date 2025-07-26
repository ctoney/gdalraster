/* R interface to GDALAlgorithm and related classes
   Chris Toney <jctoney at gmail.com>
   Copyright (c) 2023-2025 gdalraster authors
*/

#if __has_include("gdalalgorithm.h")

#include "gdalalgorithm.h"
#include "gdal.h"
#include "cpl_port.h"
#include "cpl_string.h"

#include "gdalalg.h"


void append_subalg_names_desc_(const GDALAlgorithmH alg,
                               const std::string &cmd_str,
                               std::vector<std::string> *names,
                               std::vector<std::string> *desc,
                               std::vector<std::string> *urls,
                               const std::string &starts_with,
                               bool cout) {

    char **subnames = GDALAlgorithmGetSubAlgorithmNames(alg);
    int num_subnames = CSLCount(subnames);

    for (int i = 0; i < num_subnames; ++i) {
        auto subalg = GDALAlgorithmInstantiateSubAlgorithm(alg, subnames[i]);
        if (subalg == nullptr)
            continue;

        std::string this_cmd_str = cmd_str + " " + GDALAlgorithmGetName(subalg);
        bool cout_this = true;
        if (starts_with == "" ||
                STARTS_WITH_CI(this_cmd_str.c_str(), starts_with.c_str())) {

            names->push_back(this_cmd_str);
            desc->push_back(GDALAlgorithmGetDescription(subalg));
            urls->push_back(GDALAlgorithmGetHelpFullURL(subalg));
        }
        else {
            cout_this = false;
        }

        if (cout && cout_this) {
            Rcpp::Rcout << this_cmd_str.c_str() << ":" << std::endl;
            Rcpp::Rcout << GDALAlgorithmGetDescription(subalg) << std::endl;
            Rcpp::Rcout << GDALAlgorithmGetHelpFullURL(subalg) << std::endl;
            Rcpp::Rcout << std::endl;
        }

        if (GDALAlgorithmHasSubAlgorithms(subalg)) {
            append_subalg_names_desc_(subalg, this_cmd_str, names, desc, urls,
                                      starts_with, cout);
        }

        GDALAlgorithmRelease(subalg);
    }

    CSLDestroy(subnames);
}

// [[Rcpp::export(invisible = true)]]
Rcpp::DataFrame gdal_commands(const std::string &starts_with = "",
                              bool cout = true) {

    auto reg = GDALGetGlobalAlgorithmRegistry();
    if (reg == nullptr) {
        Rcpp::stop("failed to obtain global algorithm registry");
    }

    char **names = GDALAlgorithmRegistryGetAlgNames(reg);
    int num_names = CSLCount(names);
    if (num_names <= 0) {
        Rcpp::stop("failed to obtain global algorithm names");
    }

    std::vector<std::string> cmd_names = {};
    std::vector<std::string> cmd_descriptions = {};
    std::vector<std::string> cmd_urls = {};

    for (int i = 0; i < num_names; ++i) {
        auto alg = GDALAlgorithmRegistryInstantiateAlg(reg, names[i]);
        if (alg == nullptr)
            continue;

        bool cout_this = true;
        if (starts_with == "" ||
                STARTS_WITH_CI(names[i], starts_with.c_str())) {

            cmd_names.push_back(names[i]);
            cmd_descriptions.push_back(GDALAlgorithmGetDescription(alg));
            cmd_urls.push_back(GDALAlgorithmGetHelpFullURL(alg));
        }
        else {
            cout_this = false;
        }

        if (cout && cout_this) {
            Rcpp::Rcout << names[i] << ":" << std::endl;
            Rcpp::Rcout << GDALAlgorithmGetDescription(alg) << std::endl;
            Rcpp::Rcout << GDALAlgorithmGetHelpFullURL(alg) << std::endl;
            Rcpp::Rcout << std::endl;
        }

        if (GDALAlgorithmHasSubAlgorithms(alg)) {
            append_subalg_names_desc_(alg, std::string(names[i]),
                                      &cmd_names, &cmd_descriptions,
                                      &cmd_urls, starts_with, cout);
        }

        GDALAlgorithmRelease(alg);
    }

    CSLDestroy(names);
    GDALAlgorithmRegistryRelease(reg);

    Rcpp::DataFrame df = Rcpp::DataFrame::create(
        Rcpp::Named("command_string") = Rcpp::wrap(cmd_names),
        Rcpp::Named("description") = Rcpp::wrap(cmd_descriptions),
        Rcpp::Named("URL") = Rcpp::wrap(cmd_urls));

    return df;
}

#endif  // __has_include("gdalalgorithm.h")
