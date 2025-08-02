/* R interface to GDALAlgorithm and related classes
   Chris Toney <jctoney at gmail.com>
   Copyright (c) 2023-2025 gdalraster authors
*/

#include <cpl_port.h>
#include <cpl_string.h>

#include <cctype>
#include <sstream>

#include "gdalalg.h"
#include "gdalraster.h"
#include "gdalvector.h"

constexpr R_xlen_t CMD_TOKENS_MAX = 5;

#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 11, 0)
// internal helper to get subalgorithm names, descriptions and URLs,
// potentially filtering on 'starts_with'
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
#endif  // GDAL >= 3.11

// [[Rcpp::export(invisible = true)]]
Rcpp::DataFrame gdal_commands(const std::string &starts_with = "",
                              bool cout = true) {

#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::stop("gdal_commands() requires GDAL >= 3.11");

#else
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
#endif  // GDAL >= 3.11
}

/*
    Implementation of exposed class GDALAlg, which wraps GDALAlgorithm and
    its related classes GDALAlgorithmArg and GDALArgDatasetValue
*/

GDALAlg::GDALAlg() : m_cmd(Rcpp::CharacterVector::create()),
                     m_cmd_str(""),
                     m_args(Rcpp::CharacterVector::create()) {
    // undocumented default constructor with no arguments
    // currently not intended for user code
}

GDALAlg::GDALAlg(const Rcpp::CharacterVector &cmd) :
            GDALAlg(cmd, Rcpp::CharacterVector::create()) {}

GDALAlg::GDALAlg(const Rcpp::CharacterVector &cmd, const Rcpp::RObject &args) {

#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::stop("class GDALAlg requires GDAL >= 3.11");
#else

    if (cmd.size() == 0 ||
        (cmd.size() == 1 && EQUAL(cmd[0], "")) ||
        (cmd.size() == 1 && Rcpp::String(cmd[0]) == NA_STRING)) {

        Rcpp::stop("'cmd' is empty");
    }
    else if (Rcpp::is_true(Rcpp::any(Rcpp::is_na(cmd)))) {
        Rcpp::stop("'cmd' contains one or more NA values");
    }
    else if (cmd.size() > CMD_TOKENS_MAX) {
        Rcpp::stop("number of elements in 'cmd' is out of range");
    }

    m_cmd_str = "";
    for (R_xlen_t i = 0; i < cmd.size(); ++i) {
        m_cmd_str += Rcpp::as<std::string>(cmd[i]);
        if (i < cmd.size() - 1) {
            m_cmd_str += " ";
        }
    }

    bool has_tokens = false;
    for (char c : m_cmd_str) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            has_tokens = true;
            break;
        }
    }

    if (has_tokens) {
        m_cmd = Rcpp::CharacterVector::create();
        std::stringstream ss(m_cmd_str);
        std::string token;
        while (ss >> token) {
            m_cmd.push_back(token);
        }
    }
    else {
        m_cmd = Rcpp::clone(cmd);
    }

    if (!args.isNULL()) {
        if (Rcpp::is<Rcpp::CharacterVector>(args)) {
            m_args = Rcpp::clone(args);
        }
        else if (Rcpp::is<Rcpp::List>(args)) {
            m_args = listArgsToVector_(Rcpp::as<Rcpp::List>(args));
        }
        else {
            Rcpp::stop("'args' must be a character vector or named list");
        }
    }
    else {
        m_args = Rcpp::CharacterVector::create();
    }

    instantiateAlg_();
#endif  // GDAL >= 3.11
}

GDALAlg::~GDALAlg() {
#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 11, 0)
    if (m_hActualAlg != nullptr) {
        if (m_hasRun && !m_hasFinalized)
            GDALAlgorithmFinalize(m_hActualAlg);
        GDALAlgorithmRelease(m_hActualAlg);
    }

    if (m_hAlg != nullptr) {
        GDALAlgorithmRelease(m_hAlg);
    }
#endif  // GDAL >= 3.11
}

Rcpp::List GDALAlg::info() const {
#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::stop("class GDALAlg requires GDAL >= 3.11");
#else

    if (m_hAlg == nullptr)
        Rcpp::stop("algorithm not instantiated");

    Rcpp::List alg_info = Rcpp::List::create();
    GDALAlgorithmH alg = m_hActualAlg ? m_hActualAlg : m_hAlg;

    alg_info.push_back(GDALAlgorithmGetName(alg), "name");
    alg_info.push_back(GDALAlgorithmGetDescription(alg), "description");
    alg_info.push_back(GDALAlgorithmGetLongDescription(alg),
                       "long_description");
    alg_info.push_back(GDALAlgorithmGetHelpFullURL(alg), "URL");
    alg_info.push_back(GDALAlgorithmHasSubAlgorithms(alg),
                       "has_subalgorithms");

    if (GDALAlgorithmHasSubAlgorithms(alg)) {
        char **papszNames = GDALAlgorithmGetSubAlgorithmNames(alg);
        int nCount = 0;
        nCount = CSLCount(papszNames);
        if (nCount > 0) {
            std::vector<std::string> names(papszNames, papszNames + nCount);
            alg_info.push_back(Rcpp::wrap(names), "subalgorithm_names");
        }
        else {
            alg_info.push_back(Rcpp::CharacterVector::create(),
                               "subalgorithm_names");
        }
        CSLDestroy(papszNames);
    }
    else {
        alg_info.push_back(Rcpp::CharacterVector::create(),
                           "subalgorithm_names");
    }

    char **papszArgNames = GDALAlgorithmGetArgNames(alg);
    int nCount = 0;
    nCount = CSLCount(papszArgNames);
    if (nCount > 0) {
        std::vector<std::string> names(papszArgNames, papszArgNames + nCount);
        alg_info.push_back(Rcpp::wrap(names), "arg_names");
    }
    else {
        alg_info.push_back(Rcpp::CharacterVector::create(), "arg_names");
    }
    CSLDestroy(papszArgNames);

    return alg_info;
#endif  // GDAL >= 3.11
}

Rcpp::List GDALAlg::argInfo(const Rcpp::String &arg_name) const {
#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::stop("class GDALAlg requires GDAL >= 3.11");
#else

    if (m_hAlg == nullptr)
        Rcpp::stop("algorithm not instantiated");

    if (arg_name == "" || arg_name == NA_STRING)
        Rcpp::stop("'arg_name' is required");

    Rcpp::List arg_info = Rcpp::List::create();

    GDALAlgorithmArgH hArg = nullptr;
    hArg = GDALAlgorithmGetArg(m_hActualAlg ? m_hActualAlg : m_hAlg,
                               arg_name.get_cstring());

    if (hArg == nullptr)
        Rcpp::stop("failed to obtain GDALAlgorithmArg object for 'arg_name'");

    arg_info.push_back(GDALAlgorithmArgGetName(hArg), "name");

    GDALAlgorithmArgType eType = GDALAlgorithmArgGetType(hArg);
    std::string arg_type = str_toupper_(GDALAlgorithmArgTypeName(eType));
    arg_info.push_back(arg_type, "type");

    arg_info.push_back(GDALAlgorithmArgGetDescription(hArg), "description");
    arg_info.push_back(GDALAlgorithmArgGetShortName(hArg), "short_name");

    char **papszAliases = GDALAlgorithmArgGetAliases(hArg);
    int nCount = 0;
    nCount = CSLCount(papszAliases);
    if (papszAliases && nCount > 0) {
        std::vector<std::string> v(papszAliases, papszAliases + nCount);
        arg_info.push_back(Rcpp::wrap(v), "aliases");
    }
    else {
        Rcpp::CharacterVector v =
            Rcpp::CharacterVector::create(NA_STRING);
        arg_info.push_back(v, "aliases");
    }
    CSLDestroy(papszAliases);

    arg_info.push_back(GDALAlgorithmArgGetMetaVar(hArg), "meta_var");
    arg_info.push_back(GDALAlgorithmArgGetCategory(hArg), "category");
    arg_info.push_back(GDALAlgorithmArgIsPositional(hArg), "is_positional");
    arg_info.push_back(GDALAlgorithmArgIsRequired(hArg), "is_required");
    arg_info.push_back(GDALAlgorithmArgGetMinCount(hArg), "min_count");
    arg_info.push_back(GDALAlgorithmArgGetMaxCount(hArg), "max_count");
    arg_info.push_back(GDALAlgorithmArgGetPackedValuesAllowed(hArg),
                       "packed_values_allowed");
    arg_info.push_back(GDALAlgorithmArgGetRepeatedArgAllowed(hArg),
                       "repeated_arg_allowed");

    if (eType == GAAT_STRING || eType == GAAT_STRING_LIST) {
        char **papszChoices = GDALAlgorithmArgGetChoices(hArg);
        int nCount = 0;
        nCount = CSLCount(papszChoices);
        if (papszChoices && nCount > 0) {
            std::vector<std::string> v(papszChoices, papszChoices + nCount);
            arg_info.push_back(Rcpp::wrap(v), "choices");
        }
        else {
            Rcpp::CharacterVector v =
                Rcpp::CharacterVector::create(NA_STRING);
            arg_info.push_back(v, "choices");
        }
        CSLDestroy(papszChoices);
    }
    else {
        arg_info.push_back(R_NilValue, "choices");
    }

    arg_info.push_back(GDALAlgorithmArgIsExplicitlySet(hArg),
                       "is_explicitly_set");
    arg_info.push_back(GDALAlgorithmArgHasDefaultValue(hArg),
                       "has_default_value");
    arg_info.push_back(GDALAlgorithmArgIsHiddenForCLI(hArg),
                       "is_hidden_for_cli");
    arg_info.push_back(GDALAlgorithmArgIsOnlyForCLI(hArg),
                       "is_only_for_cli");
    arg_info.push_back(GDALAlgorithmArgIsInput(hArg), "is_input");
    arg_info.push_back(GDALAlgorithmArgIsOutput(hArg), "is_output");

    if (eType == GAAT_DATASET || eType == GAAT_DATASET_LIST) {

        //  type flags
        GDALArgDatasetType ds_type = GDALAlgorithmArgGetDatasetType(hArg);
        Rcpp::CharacterVector ds_type_flags = Rcpp::CharacterVector::create();
        if (ds_type & GDAL_OF_RASTER)
            ds_type_flags.push_back("RASTER");
        if (ds_type & GDAL_OF_VECTOR)
            ds_type_flags.push_back("VECTOR");
        if (ds_type & GDAL_OF_MULTIDIM_RASTER)
            ds_type_flags.push_back("MULTIDIM_RASTER");
        if (ds_type & GDAL_OF_UPDATE)
            ds_type_flags.push_back("UPDATE");

        arg_info.push_back(ds_type_flags, "dataset_type_flags");

        // input flags
        int ds_input_flags = GDALAlgorithmArgGetDatasetInputFlags(hArg);
        Rcpp::CharacterVector ds_input_flags_out =
            Rcpp::CharacterVector::create();

        if (ds_input_flags & GADV_NAME)
            ds_input_flags_out.push_back("NAME");
        if (ds_input_flags & GADV_OBJECT)
            ds_input_flags_out.push_back("OBJECT");

        arg_info.push_back(ds_input_flags_out, "dataset_input_flags");

        // output flags
        int ds_output_flags = GDALAlgorithmArgGetDatasetOutputFlags(hArg);
        Rcpp::CharacterVector ds_output_flags_out =
            Rcpp::CharacterVector::create();

        if (ds_output_flags & GADV_NAME)
            ds_output_flags_out.push_back("NAME");
        if (ds_output_flags & GADV_OBJECT)
            ds_output_flags_out.push_back("OBJECT");

        arg_info.push_back(ds_output_flags_out, "dataset_output_flags");
    }
    else {
        arg_info.push_back(R_NilValue, "dataset_type_flags");
        arg_info.push_back(R_NilValue, "dataset_input_flags");
        arg_info.push_back(R_NilValue, "dataset_output_flags");
    }

    arg_info.push_back(GDALAlgorithmArgGetMutualExclusionGroup(hArg),
                       "mutual_exclusion_group");

    return arg_info;
#endif  // GDAL >= 3.11
}

Rcpp::String GDALAlg::usageAsJSON() const {
#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::stop("class GDALAlg requires GDAL >= 3.11");
#else

    if (m_hAlg == nullptr)
        Rcpp::stop("algorithm not instantiated");

    char *pszUsage = nullptr;
    pszUsage = GDALAlgorithmGetUsageAsJSON(
                    m_hActualAlg ? m_hActualAlg : m_hAlg);

    Rcpp::String json = "";
    if (pszUsage)
        json = Rcpp::String(pszUsage);
    CPLFree(pszUsage);

    return json;
#endif  // GDAL >= 3.11
}

bool GDALAlg::parseCommandLineArgs() {
#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::stop("class GDALAlg requires GDAL >= 3.11");
#else

    // parses cl args which sets the values, and also instantiates m_hActualAlg

    if (m_hAlg == nullptr)
        Rcpp::stop("algorithm not instantiated");

    if (m_haveParsedCmdLineArgs) {
        Rcpp::stop(
            "parseCommandLineArgs() can only be called once per instance");
    }

    std::vector<const char*> arg_list = {};
    if (m_args.size() > 0) {
        for (Rcpp::String arg : m_args) {
            arg_list.push_back(arg.get_cstring());
        }
    }
    arg_list.push_back(nullptr);

    bool res = true;
    if (!m_map_in_hDS.empty()) {
        for (auto it = m_map_in_hDS.begin(); it != m_map_in_hDS.end(); ++it) {
            GDALAlgorithmArgH hArg = GDALAlgorithmGetArg(m_hAlg,
                                                         it->first.c_str());
            if (hArg) {
                if (GDALAlgorithmArgGetType(hArg) == GAAT_DATASET) {
                    res = GDALAlgorithmArgSetDataset(hArg, it->second[0]);
                }
                else if (GDALAlgorithmArgGetType(hArg) == GAAT_DATASET_LIST) {
                    res = GDALAlgorithmArgSetDatasets(hArg, it->second.size(),
                                                      it->second.data());
                }
                else {
                    res = false;
                }

                GDALAlgorithmArgRelease(hArg);
                if (!res) break;
            }
        }
    }

    if (res && arg_list.size() > 1) {
        res = GDALAlgorithmParseCommandLineArguments(m_hAlg, arg_list.data());
    }

    if (res) {
        m_haveParsedCmdLineArgs = true;
        if (m_hActualAlg == nullptr) {
            m_hActualAlg = GDALAlgorithmGetActualAlgorithm(m_hAlg);
        }
    }

    return res;
#endif  // GDAL >= 3.11
}

bool GDALAlg::run() {
#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::stop("class GDALAlg requires GDAL >= 3.11");
#else

    if (m_hAlg == nullptr)
        Rcpp::stop("algorithm not instantiated");

    if (m_hasRun)
        Rcpp::stop("algorithm has already run");

    if (!m_haveParsedCmdLineArgs) {
        if (!parseCommandLineArgs()) {
            if (!quiet) {
                Rcpp::Rcout << "parse command line arguments failed" <<
                    std::endl;
            }
            return false;
        }
    }

    if (m_hActualAlg == nullptr) {
        if (!quiet) {
            Rcpp::Rcout << "actual algorithm handle is nullptr" << std::endl;
        }
        return false;
    }

    bool res = GDALAlgorithmRun(m_hActualAlg,
                                quiet ? nullptr : GDALTermProgressR,
                                nullptr);

    if (res)
        m_hasRun = true;

    return res;
#endif  // GDAL >= 3.11
}

SEXP GDALAlg::output() const {
#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::stop("class GDALAlg requires GDAL >= 3.11");
#else

    if (m_hAlg == nullptr)
        Rcpp::stop("algorithm not instantiated");

    if (!m_hasRun || m_hActualAlg == nullptr)
        Rcpp::stop("algorithm has not run");

    std::vector<std::string> out_arg_names = getOutputArgNames_();
    if (out_arg_names.size() == 0) {
        Rcpp::stop("no output arg names found");
    }
    if (out_arg_names.size() > 1) {
        Rcpp::stop(
            "algorithm has multiple outputs, use method `outputs()` instead");
    }

    Rcpp::List out = outputs();
    return out[0];
#endif  // GDAL >= 3.11
}

Rcpp::List GDALAlg::outputs() const {
#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::stop("class GDALAlg requires GDAL >= 3.11");
#else

    if (m_hAlg == nullptr)
        Rcpp::stop("algorithm not instantiated");

    if (!m_hasRun || m_hActualAlg == nullptr)
        Rcpp::stop("algorithm has not run");

    std::vector<std::string> out_arg_names = getOutputArgNames_();
    if (out_arg_names.size() == 0)
        Rcpp::stop("no output arg names found");

    Rcpp::List out = Rcpp::List::create();

    for (std::string arg_name : out_arg_names) {
        GDALAlgorithmArgH hArg = nullptr;
        hArg = GDALAlgorithmGetArg(m_hActualAlg, arg_name.c_str());
        if (hArg == nullptr) {
            if (!quiet) {
                Rcpp::Rcout << "got nullptr for arg: " << arg_name.c_str()
                    << std::endl;
            }
            continue;
        }

        if (GDALAlgorithmArgIsOutput(hArg)) {
            Rcpp::String s(arg_name);
            s.replace_all("-", "_");
            out.push_back(getOutputArgValue_(hArg), s);
        }

        GDALAlgorithmArgRelease(hArg);
    }

    return out;
#endif  // GDAL >= 3.11
}

bool GDALAlg::close() {
#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::stop("class GDALAlg requires GDAL >= 3.11");
#else

    if (m_hAlg == nullptr)
        Rcpp::stop("algorithm not instantiated");

    if (!m_hasRun) {
        if (!quiet)
            Rcpp::Rcout << "algorithm has not run" << std::endl;
        return false;
    }

    if (m_hasFinalized) {
        if (!quiet)
            Rcpp::Rcout << "algorithm has already been finalized" << std::endl;
        return false;
    }

    if (m_hActualAlg != nullptr) {
        return GDALAlgorithmFinalize(m_hActualAlg);
    }
    else {
        if (!quiet)
            Rcpp::Rcout << "actual algorithm handle is nullptr" << std::endl;
        return false;
    }
#endif  // GDAL >= 3.11
}

void GDALAlg::release() {
#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::stop("class GDALAlg requires GDAL >= 3.11");
#else

    if (m_hActualAlg != nullptr) {
        if (m_hasRun && !m_hasFinalized)
            GDALAlgorithmFinalize(m_hActualAlg);
        GDALAlgorithmRelease(m_hActualAlg);
        m_hActualAlg = nullptr;
    }

    if (m_hAlg != nullptr) {
        GDALAlgorithmRelease(m_hAlg);
        m_hAlg = nullptr;
    }

#endif  // GDAL >= 3.11
}

void GDALAlg::show() const {
#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::Rcout << "class GDALAlg requires GDAL >= 3.11" << std::endl;
#else

    Rcpp::Rcout << "C++ object of class GDALAlg" << std::endl;

    if (m_hAlg == nullptr) {
        Rcpp::Rcout << " algorithm not instantiated" << std::endl;
    }
    else {
        GDALAlgorithmH alg = m_hActualAlg ? m_hActualAlg : m_hAlg;

        Rcpp::Rcout << " Name        : " << GDALAlgorithmGetName(alg) << std::endl;
        Rcpp::Rcout << " Description : " << GDALAlgorithmGetDescription(alg) <<
            std::endl;
        Rcpp::Rcout << " Help URL    : " << GDALAlgorithmGetHelpFullURL(alg) <<
            std::endl;
    }
#endif  // GDAL >= 3.11
}

// ****************************************************************************
// class methods for internal use not exposed in R
// ****************************************************************************

Rcpp::CharacterVector GDALAlg::listArgsToVector_(
        const Rcpp::List &list_args) {

    // convert arguments in a named list to a character vector
    // arguments in list form must use arg long names

    Rcpp::CharacterVector arg_vec = Rcpp::CharacterVector::create();

    R_xlen_t num_args = 0;
    if (list_args.size() == 0)
        return arg_vec;
    else
        num_args = list_args.size();

    Rcpp::CharacterVector arg_names = list_args.names();
    if (arg_names.size() == 0 || arg_names.size() != num_args)
        Rcpp::stop("arg list must have named elements");

    for (R_xlen_t i = 0; i < num_args; ++i) {
        Rcpp::String nm = arg_names[i];
        nm.replace_all("_", "-");
        nm.push_front("--");
        if (Rcpp::is<Rcpp::LogicalVector>(list_args[i]) &&
                list_args[i] == TRUE) {

            arg_vec.push_back(nm);
            continue;
        }
        const Rcpp::RObject &val = list_args[i];
        if (!val.isNULL() && val.isObject()) {
            Rcpp::String cls = val.attr("class");
            if (cls == "Rcpp_GDALRaster") {
                const GDALRaster* const &ds = list_args[i];
                std::vector<GDALDatasetH> ds_list = {};
                ds_list.push_back(ds->getGDALDatasetH_());
                m_map_in_hDS[Rcpp::as<std::string>(arg_names[i])] = ds_list;
                continue;
            }
            if (cls == "Rcpp_GDALVector") {
                const GDALVector* const &ds = list_args[i];
                std::vector<GDALDatasetH> ds_list = {};
                ds_list.push_back(ds->getGDALDatasetH_());
                m_map_in_hDS[Rcpp::as<std::string>(arg_names[i])] = ds_list;
                continue;
            }
            // TODO: accept a list of datasets
        }
        nm += "=";
        nm += paste_collapse_(list_args[i], ",");
        arg_vec.push_back(nm);
    }

    return arg_vec;
}

void GDALAlg::instantiateAlg_() {
#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    Rcpp::stop("class GDALAlg requires GDAL >= 3.11");
#else

    // instantiate m_hAlg
    if (m_hAlg != nullptr || m_hActualAlg != nullptr) {
        Rcpp::stop(
            "instantiateAlg_(): algorithm object appears already instantiated");
    }

    auto reg = GDALGetGlobalAlgorithmRegistry();
    if (reg == nullptr) {
        Rcpp::stop("failed to obtain global algorithm registry");
    }

    if (m_cmd.size() == 1) {
        m_hAlg = GDALAlgorithmRegistryInstantiateAlg(reg, m_cmd[0]);
        if (m_hAlg == nullptr) {
            GDALAlgorithmRegistryRelease(reg);
            Rcpp::stop("failed to instantiate CLI algorithm from 'cmd'");
        }
    }
    else {
        std::vector<GDALAlgorithmH> alg_tmp;
        alg_tmp.push_back(GDALAlgorithmRegistryInstantiateAlg(reg, m_cmd[0]));
        if (alg_tmp[0] == nullptr) {
            GDALAlgorithmRegistryRelease(reg);
            Rcpp::stop("failed to instantiate CLI algorithm from 'cmd'");
        }
        for (R_xlen_t i = 1; i < m_cmd.size(); ++i) {
            if (i == (m_cmd.size() - 1)) {
                m_hAlg = GDALAlgorithmInstantiateSubAlgorithm(alg_tmp[i - 1],
                                                              m_cmd[i]);
                if (m_hAlg == nullptr) {
                    for (GDALAlgorithmH alg : alg_tmp) {
                        if (alg)
                            GDALAlgorithmRelease(alg);
                    }
                    GDALAlgorithmRegistryRelease(reg);
                    Rcpp::stop(
                        "failed to instantiate CLI algorithm from 'cmd'");
                }
            }
            else {
                alg_tmp.push_back(
                    GDALAlgorithmInstantiateSubAlgorithm(alg_tmp[i - 1],
                                                         m_cmd[i]));

                if (alg_tmp.back() == nullptr) {
                    for (GDALAlgorithmH alg : alg_tmp) {
                        if (alg)
                            GDALAlgorithmRelease(alg);
                    }
                    GDALAlgorithmRegistryRelease(reg);
                    Rcpp::stop(
                        "failed to instantiate CLI algorithm from 'cmd'");
                }
            }
        }

        for (GDALAlgorithmH alg : alg_tmp) {
            if (alg)
                GDALAlgorithmRelease(alg);
        }
    }
#endif  // GDAL >= 3.11
}

std::vector<std::string> GDALAlg::getOutputArgNames_() const {
    std::vector<std::string> names_out = {};

#if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3, 11, 0)
    return names_out;
#else

    char **papszArgNames = GDALAlgorithmGetArgNames(m_hActualAlg);
    int nCount = 0;
    nCount = CSLCount(papszArgNames);
    if (papszArgNames && nCount > 0) {
        std::vector<std::string> names(papszArgNames, papszArgNames + nCount);
        for (std::string arg_name : names) {
            GDALAlgorithmArgH hArg = nullptr;
            hArg = GDALAlgorithmGetArg(m_hActualAlg, arg_name.c_str());
            if (hArg == nullptr) {
                if (!quiet) {
                    Rcpp::Rcout << "got nullptr for arg: " << arg_name.c_str()
                        << std::endl;
                }
                continue;
            }
            if (GDALAlgorithmArgIsOutput(hArg)) {
                names_out.push_back(arg_name);
            }
            GDALAlgorithmArgRelease(hArg);
        }
    }
    CSLDestroy(papszArgNames);

    return names_out;
#endif  // GDAL >= 3.11
}

#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 11, 0)
SEXP GDALAlg::getOutputArgValue_(const GDALAlgorithmArgH hArg) const {
    if (hArg == nullptr)
        Rcpp::stop("got nullptr for GDALAlgorithmArgH hArg");

    SEXP out = R_NilValue;

    switch (GDALAlgorithmArgGetType(hArg)) {
        case GAAT_BOOLEAN:
        {
            Rcpp::LogicalVector v =
                Rcpp::LogicalVector::create(GDALAlgorithmArgGetAsBoolean(hArg));
            out = v;
        }
        break;

        case GAAT_STRING:
        {
            out = Rcpp::wrap(GDALAlgorithmArgGetAsString(hArg));
        }
        break;

        case GAAT_INTEGER:
        {
            Rcpp::IntegerVector v =
                Rcpp::IntegerVector::create(GDALAlgorithmArgGetAsInteger(hArg));
            out = v;
        }
        break;

        case GAAT_REAL:
        {
            Rcpp::NumericVector v =
                Rcpp::NumericVector::create(GDALAlgorithmArgGetAsDouble(hArg));
            out = v;
        }
        break;

        case GAAT_STRING_LIST:
        {
            char **papszValue = GDALAlgorithmArgGetAsStringList(hArg);
            int nCount = 0;
            nCount = CSLCount(papszValue);
            if (papszValue && nCount > 0) {
                std::vector<std::string> v(papszValue, papszValue + nCount);
                out = Rcpp::wrap(v);
            }
            else {
                Rcpp::CharacterVector v =
                    Rcpp::CharacterVector::create(NA_STRING);
                out = v;
            }
            CSLDestroy(papszValue);
        }
        break;

        case GAAT_INTEGER_LIST:
        {
            size_t nCount = 0;
            const int *panValue =
                GDALAlgorithmArgGetAsIntegerList(hArg, &nCount);

            if (panValue && nCount > 0) {
                std::vector<int> v(panValue, panValue + nCount);
                out = Rcpp::wrap(v);
            }
            else {
                Rcpp::IntegerVector v =
                    Rcpp::IntegerVector::create(NA_INTEGER);
                out = v;
            }
        }
        break;

        case GAAT_REAL_LIST:
        {
            size_t nCount = 0;
            const double *padfValue =
                GDALAlgorithmArgGetAsDoubleList(hArg, &nCount);

            if (padfValue && nCount > 0) {
                std::vector<double> v(padfValue, padfValue + nCount);
                out = Rcpp::wrap(v);
            }
            else {
                Rcpp::NumericVector v =
                    Rcpp::NumericVector::create(NA_REAL);
                out = v;
            }
        }
        break;

        case GAAT_DATASET:
        {
            GDALArgDatasetValueH hArgDSValue =
                    GDALAlgorithmArgGetAsDatasetValue(hArg);

            GDALArgDatasetType ds_type = GDALAlgorithmArgGetDatasetType(hArg);
            bool with_update = false;
            if (ds_type & GDAL_OF_UPDATE)
                with_update = true;

            // raster
            if (ds_type & GDAL_OF_RASTER) {
                GDALDatasetH hDS =
                    GDALArgDatasetValueGetDatasetIncreaseRefCount(hArgDSValue);

                std::string ds_name(GDALArgDatasetValueGetName(hArgDSValue));

                GDALRaster *ds = new GDALRaster();
                ds->setFilename(ds_name);
                ds->setGDALDatasetH_(hDS, with_update);
                const GDALRaster& ds_ref = *ds;
                out = Rcpp::wrap(ds_ref);
            }
            // vector
            else if (ds_type & GDAL_OF_VECTOR) {
                GDALDatasetH hDS =
                    GDALArgDatasetValueGetDatasetIncreaseRefCount(hArgDSValue);
                if (hDS == nullptr) {
                    GDALArgDatasetValueRelease(hArgDSValue);
                    Rcpp::stop("GDAL dataset object is NULL");
                }

                std::string ds_name(GDALArgDatasetValueGetName(hArgDSValue));

                OGRLayerH hLayer = nullptr;
                std::string layer_name = "";
                if (this->outputLayerNameForOpen == "" ||
                    this->outputLayerNameForOpen == NA_STRING) {

                    hLayer = GDALDatasetGetLayer(hDS, 0);
                }
                else {
                    layer_name = this->outputLayerNameForOpen;
                    hLayer = GDALDatasetGetLayerByName(hDS, layer_name.c_str());
                }

                if (layer_name == "") {
                    // default layer first by index was opened
                    if (hLayer != nullptr)
                        layer_name = OGR_L_GetName(hLayer);
                }

                GDALVector *lyr = new GDALVector();
                lyr->setDsn_(ds_name);
                lyr->setGDALDatasetH_(hDS, true);
                lyr->setOGRLayerH_(hLayer, layer_name);
                if (hLayer != nullptr)
                    lyr->setFieldNames_();

                const GDALVector& lyr_ref = *lyr;
                out = Rcpp::wrap(lyr_ref);
            }
            // multidim raster - currently only as dataset name
            else if (ds_type & GDAL_OF_MULTIDIM_RASTER) {

                out = Rcpp::wrap(GDALArgDatasetValueGetName(hArgDSValue));
            }
            // unrecognized dataset type - should not occur
            else {
                out = Rcpp::wrap("unrecognized dataset type");
            }

            GDALArgDatasetValueRelease(hArgDSValue);
        }
        break;

        case GAAT_DATASET_LIST:
        {
            // TODO
        }
        break;

        default:
        {
            out = Rcpp::wrap("unrecognized arg type");
        }
        break;
    }

    return out;
}
#endif  // GDAL >= 3.11

// ****************************************************************************

RCPP_MODULE(mod_GDALAlg) {
    Rcpp::class_<GDALAlg>("GDALAlg")

    .constructor
        ("Default constructor")
    .constructor<Rcpp::CharacterVector>
        ("Usage: new(GDALAlg, cmd)")
    .constructor<Rcpp::CharacterVector, Rcpp::RObject>
        ("Usage: new(GDALAlg, cmd, cl_arg)")

    // undocumented read-only fields for internal use
    .field_readonly("m_haveParsedCmdLineArgs",
                    &GDALAlg::m_haveParsedCmdLineArgs)
    .field_readonly("m_hasRun", &GDALAlg::m_hasRun)
    .field_readonly("m_hasFinalized", &GDALAlg::m_hasFinalized)

    // read/write fields
    .field("outputLayerNameForOpen", &GDALAlg::outputLayerNameForOpen)
    .field("quiet", &GDALAlg::quiet)

    // methods
    .const_method("info", &GDALAlg::info,
        "Return a list of algorithm information")
    .const_method("argInfo", &GDALAlg::argInfo,
        "Return a list of information for an algorithm argument")
    .const_method("usageAsJSON", &GDALAlg::usageAsJSON,
        "Return a list of algorithm information")
    .method("parseCommandLineArgs", &GDALAlg::parseCommandLineArgs,
        "Parse command line arguments")
    .method("run", &GDALAlg::run,
        "Execute the algorithm")
    .const_method("output", &GDALAlg::output,
        "Return the single output value of this algorithm")
    .const_method("outputs", &GDALAlg::outputs,
        "Return the output value(s) of this algorithm as a named list")
    .method("close", &GDALAlg::close,
        "Complete any pending actions, and return the final status")
    .method("release", &GDALAlg::release,
        "Release memory associated with the algorithm (potentially finalize)")
    .const_method("show", &GDALAlg::show,
        "S4 show()")

    ;
}
