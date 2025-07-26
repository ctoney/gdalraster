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
            const Rcpp::Nullable<Rcpp::CharacterVector> &cl_arg);
    ~GDALAlg();

    // undocumented, exposed read-only fields for internal use
    bool m_haveParsedCmdLineArgs {false};

    // exposed read/write fields
    bool quiet {false};
    std::string outputLayerNameForOpen {""};

    // exposed methods
    Rcpp::List getAlgInfo() const;
    Rcpp::List getArgInfo(std::string arg_name) const;

    bool run();
    Rcpp::List output() const;

    void show() const;

    // methods for internal use not exposed to R


 private:
    std::string m_cmd_str {""};
    GDALAlgorithmH m_hAlg {nullptr};
    GDALAlgorithmH m_hActualAlg {nullptr};
    Rcpp::List m_output {};
};

// cppcheck-suppress unknownMacro
RCPP_EXPOSED_CLASS(GDALAlg)

#endif  // SRC_GDALALG_H_
