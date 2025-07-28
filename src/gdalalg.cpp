/* R interface to GDALAlgorithm and related classes
   Chris Toney <jctoney at gmail.com>
   Copyright (c) 2023-2025 gdalraster authors
*/

#if __has_include("gdalalgorithm.h")

#include <cctype>
#include <sstream>

#include "gdalalgorithm.h"
#include "gdal.h"
#include "cpl_port.h"
#include "cpl_string.h"

#include "gdalalg.h"
#include "gdalraster.h"

constexpr R_xlen_t CMD_TOKENS_MAX = 5;

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

//  Implementation of exposed class GDALAlg, which wraps GDALAlgorithm and
//  its related classes GDALAlgorithmArg and GDALArgDatasetValue

GDALAlg::GDALAlg() : m_cmd(Rcpp::CharacterVector::create()),
                     m_cmd_str(""),
                     m_args(Rcpp::CharacterVector::create()) {
    // undocumented default constructor with no arguments
    // currently not intended for user code
}

GDALAlg::GDALAlg(const Rcpp::CharacterVector &cmd) :
            GDALAlg(cmd, Rcpp::CharacterVector::create()) {}

GDALAlg::GDALAlg(const Rcpp::CharacterVector &cmd,
                 const Rcpp::Nullable<Rcpp::CharacterVector> &args) {

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

    if (args.isNotNull()) {
        m_args = Rcpp::clone(args);
    }
    else {
        m_args = Rcpp::CharacterVector::create();
    }

    instantiateAlg_();
}

GDALAlg::~GDALAlg() {
    if (m_hActualAlg != nullptr) {
        if (m_hasRun && !m_hasFinalized)
            GDALAlgorithmFinalize(m_hActualAlg);
        GDALAlgorithmRelease(m_hActualAlg);
    }

    if (m_hAlg != nullptr) {
        GDALAlgorithmRelease(m_hAlg);
    }
}

Rcpp::List GDALAlg::info() const {
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
}

Rcpp::List GDALAlg::argInfo(const Rcpp::String &arg_name) const {
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
    std::string arg_type = "";
    if (eType == GAAT_BOOLEAN)
        arg_type = "BOOLEAN";
    else if (eType == GAAT_STRING)
        arg_type = "STRING";
    else if (eType == GAAT_INTEGER)
        arg_type = "INTEGER";
    else if (eType == GAAT_REAL)
        arg_type = "REAL";
    else if (eType == GAAT_DATASET)
        arg_type = "DATASET";
    else if (eType == GAAT_STRING_LIST)
        arg_type = "STRING_LIST";
    else if (eType == GAAT_INTEGER_LIST)
        arg_type = "INTEGER_LIST";
    else if (eType == GAAT_REAL_LIST)
        arg_type = "REAL_LIST";
    else if (eType == GAAT_DATASET_LIST)
        arg_type = "DATASET_LIST";
    else
        arg_type = "unrecognized";

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

    if (arg_type == "STRING" || arg_type == "STRING_LIST") {
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
    arg_info.push_back(GDALAlgorithmArgGetMutualExclusionGroup(hArg),
                       "mutual_exclusion_group");

    return arg_info;
}

Rcpp::String GDALAlg::usageAsJSON() const {
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
}

bool GDALAlg::parseCommandLineArgs() {
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

    bool res = GDALAlgorithmParseCommandLineArguments(m_hAlg, arg_list.data());
    if (res) {
        m_haveParsedCmdLineArgs = true;
        if (m_hActualAlg == nullptr) {
            m_hActualAlg = GDALAlgorithmGetActualAlgorithm(m_hAlg);
        }
    }

    return res;
}

bool GDALAlg::run() {
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
}

SEXP GDALAlg::output() const {
    if (m_hAlg == nullptr)
        Rcpp::stop("algorithm not instantiated");

    if (!m_hasRun || m_hActualAlg == nullptr)
        Rcpp::stop("algorithm has not run");

    Rcpp::List out = Rcpp::List::create();

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
                Rcpp::String s(arg_name);
                s.replace_all("-", "_");
                out.push_back(getOutputArgTypeValue_(hArg), s);
            }

            GDALAlgorithmArgRelease(hArg);
        }
    }
    else {
        Rcpp::Rcout << "no arg names found" << std::endl;
        return R_NilValue;
    }
    CSLDestroy(papszArgNames);

    return out;
}

bool GDALAlg::finalize() {
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
}

void GDALAlg::show() const {
    if (m_hAlg == nullptr)
        Rcpp::stop("algorithm not instantiated");

    GDALAlgorithmH alg = m_hActualAlg ? m_hActualAlg : m_hAlg;

    Rcpp::Rcout << "C++ object of class GDALAlg" << std::endl;
    Rcpp::Rcout << " Name        : " << GDALAlgorithmGetName(alg) << std::endl;
    Rcpp::Rcout << " Description : " << GDALAlgorithmGetDescription(alg) <<
        std::endl;
    Rcpp::Rcout << " Help URL    : " << GDALAlgorithmGetHelpFullURL(alg) <<
        std::endl;
}

// ****************************************************************************
// class methods for internal use not exposed in R
// ****************************************************************************

void GDALAlg::instantiateAlg_() {
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
}

Rcpp::List GDALAlg::getOutputArgTypeValue_(const GDALAlgorithmArgH hArg) const {
    // Returns a named list of $type, $value for an output algorith argument.
    // $type is the R type (`typeof()`, e.g., "character", "integer", etc.), or
    // `class()` for objects, e.g., "Rcpp_GDALRaster" or "Rcpp_GDALVector".

    if (hArg == nullptr)
        Rcpp::stop("got nullptr for GDALAlgorithmArgH hArg");

    Rcpp::List out = Rcpp::List::create();

    switch (GDALAlgorithmArgGetType(hArg)) {
        case GAAT_BOOLEAN:
        {
            out.push_back("logical", "type");
            Rcpp::LogicalVector v =
                Rcpp::LogicalVector::create(GDALAlgorithmArgGetAsBoolean(hArg));
            out.push_back(v, "value");
        }
        break;

        case GAAT_STRING:
        {
            out.push_back("character", "type");
            Rcpp::String s = Rcpp::String(GDALAlgorithmArgGetAsString(hArg));
            out.push_back(s, "value");
        }
        break;

        case GAAT_INTEGER:
        {
            out.push_back("integer", "type");
            Rcpp::IntegerVector v =
                Rcpp::IntegerVector::create(GDALAlgorithmArgGetAsInteger(hArg));
            out.push_back(v, "value");
        }
        break;

        case GAAT_REAL:
        {
            out.push_back("double", "type");
            Rcpp::NumericVector v =
                Rcpp::NumericVector::create(GDALAlgorithmArgGetAsDouble(hArg));
            out.push_back(v, "value");
        }
        break;

        case GAAT_STRING_LIST:
        {
            out.push_back("character", "type");
            char **papszValue = GDALAlgorithmArgGetAsStringList(hArg);
            int nCount = 0;
            nCount = CSLCount(papszValue);
            if (papszValue && nCount > 0) {
                std::vector<std::string> v(papszValue, papszValue + nCount);
                out.push_back(Rcpp::wrap(v), "value");
            }
            else {
                Rcpp::CharacterVector v =
                    Rcpp::CharacterVector::create(NA_STRING);
                out.push_back(v, "value");
            }
            CSLDestroy(papszValue);
        }
        break;

        case GAAT_INTEGER_LIST:
        {
            out.push_back("integer", "type");
            size_t nCount = 0;
            const int *panValue = GDALAlgorithmArgGetAsIntegerList(hArg, &nCount);
            if (panValue && nCount > 0) {
                std::vector<int> v(panValue, panValue + nCount);
                out.push_back(Rcpp::wrap(v), "value");
            }
            else {
                Rcpp::IntegerVector v =
                    Rcpp::IntegerVector::create(NA_INTEGER);
                out.push_back(v, "value");
            }
        }
        break;

        case GAAT_REAL_LIST:
        {
            out.push_back("double", "type");
            size_t nCount = 0;
            const double *padfValue = GDALAlgorithmArgGetAsDoubleList(hArg, &nCount);
            if (padfValue && nCount > 0) {
                std::vector<double> v(padfValue, padfValue + nCount);
                out.push_back(Rcpp::wrap(v), "value");
            }
            else {
                Rcpp::NumericVector v =
                    Rcpp::NumericVector::create(NA_REAL);
                out.push_back(v, "value");
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

            if (ds_type & GDAL_OF_RASTER) {
                GDALDatasetH hDS =
                    GDALArgDatasetValueGetDatasetIncreaseRefCount(hArgDSValue);
                std::string ds_name(GDALArgDatasetValueGetName(hArgDSValue));

                GDALRaster *ds = new GDALRaster();
                ds->setFilename(ds_name);
                ds->setGDALDatasetH_(hDS, with_update);

                out.push_back("Rcpp_GDALRaster", "type");
                GDALRaster& ds_ref = *ds;
                out.push_back(ds_ref, "value");
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
            // TODO
        }
        break;
    }

    return out;
}

// ****************************************************************************

RCPP_MODULE(mod_GDALAlg) {
    Rcpp::class_<GDALAlg>("GDALAlg")

    .constructor
        ("Default constructor")
    .constructor<Rcpp::CharacterVector>
        ("Usage: new(GDALAlg, cmd)")
    .constructor<Rcpp::CharacterVector, Rcpp::Nullable<Rcpp::CharacterVector>>
        ("Usage: new(GDALAlg, cmd, cl_arg)")

    // undocumented read-only fields for internal use
    .field_readonly("m_haveParsedCmdLineArgs",
                    &GDALAlg::m_haveParsedCmdLineArgs)
    .field_readonly("m_hasRun", &GDALAlg::m_hasRun)

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
        "Return a named list of output value(s) (as list of $type, $value)")
    .method("finalize", &GDALAlg::finalize,
        "Complete any pending actions, and return the final status")
    .const_method("show", &GDALAlg::show,
        "S4 show()")

    ;
}


#endif  // __has_include("gdalalgorithm.h")
