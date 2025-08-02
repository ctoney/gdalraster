/* R interface to GDALAlgorithm and related classes that implement GDAL CLI
   GDAL >= 3.11
   Chris Toney <jctoney at gmail.com>
   Copyright (c) 2023-2025 gdalraster authors
*/

#ifndef SRC_GDALALG_H_
#define SRC_GDALALG_H_

#include <string>
#include <vector>

#include <gdal.h>

#if __has_include("gdalalgorithm.h")
    #include "gdalalgorithm.h"
#endif

#include "rcpp_util.h"

Rcpp::DataFrame gdal_commands(std::string starts_with, bool cout);

class GDALAlg {
 public:
    GDALAlg();
    explicit GDALAlg(const Rcpp::CharacterVector &cmd);
    GDALAlg(const Rcpp::CharacterVector &cmd,
            const Rcpp::RObject &args);
    ~GDALAlg();

    // undocumented, exposed read-only fields for internal use
    bool m_haveParsedCmdLineArgs {false};
    bool m_hasRun {false};
    bool m_hasFinalized {false};

    // exposed read/write fields
    Rcpp::String outputLayerNameForOpen {""};
    bool quiet {false};

    // exposed methods
    Rcpp::List info() const;
    Rcpp::List argInfo(const Rcpp::String &arg_name) const;
    Rcpp::String usageAsJSON() const;

    // Rcpp::List getExplicitlySetArgs() const;
    // void setArg(const Rcpp::String &arg_name, const SEXP &arg_value);

    bool parseCommandLineArgs();
    bool run();
    SEXP output() const;
    Rcpp::List outputs() const;
    bool finalize();
    void release();

    void show() const;

    // methods for internal use not exposed to R
    Rcpp::CharacterVector listArgsToVector_(const Rcpp::List &list_args);
    void instantiateAlg_();
    std::vector<std::string> getOutputArgNames_() const;
#if __has_include("gdalalgorithm.h")
    SEXP getOutputArgValue_(const GDALAlgorithmArgH hArg) const;
#endif

 private:
    Rcpp::CharacterVector m_cmd {};
    std::string m_cmd_str {""};
    Rcpp::CharacterVector m_args {};
    GDALDatasetH m_input_hDS {nullptr};
#if __has_include("gdalalgorithm.h")
    GDALAlgorithmH m_hAlg {nullptr};
    GDALAlgorithmH m_hActualAlg {nullptr};
#endif
};

// cppcheck-suppress unknownMacro
RCPP_EXPOSED_CLASS(GDALAlg)

#endif  // SRC_GDALALG_H_
