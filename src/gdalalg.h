/* R interface to GDALAlgorithm and related classes that implement GDAL CLI
   GDAL >= 3.11
   Chris Toney <jctoney at gmail.com>
   Copyright (c) 2023-2025 gdalraster authors
*/

#ifndef SRC_GDALALG_H_
#define SRC_GDALALG_H_

#include <string>
#include <vector>

#include "rcpp_util.h"

Rcpp::DataFrame gdal_commands(std::string starts_with, bool cout);

class GDALAlg {
 public:
    GDALAlg();
    explicit GDALAlg(const Rcpp::CharacterVector &cmd);
    GDALAlg(const Rcpp::CharacterVector &cmd,
            const Rcpp::Nullable<Rcpp::CharacterVector> &args);
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

    // Rcpp::CharacterVector getArgVector() const;
    // void setArgVector(const Rcpp::CharacterVector &args);

    bool parseCommandLineArgs();
    bool run();
    SEXP output() const;
    bool finalize();

    // void reset();

    void show() const;

    // methods for internal use not exposed to R
    void instantiateAlg_();
    Rcpp::List getOutputArgTypeValue_(const GDALAlgorithmArgH hArg) const;

 private:
    Rcpp::CharacterVector m_cmd {};
    std::string m_cmd_str {""};
    Rcpp::CharacterVector m_args {};
    GDALAlgorithmH m_hAlg {nullptr};
    GDALAlgorithmH m_hActualAlg {nullptr};
    Rcpp::List m_output {};
};

// cppcheck-suppress unknownMacro
RCPP_EXPOSED_CLASS(GDALAlg)

#endif  // SRC_GDALALG_H_
