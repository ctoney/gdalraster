/* R interface to GDALAlgorithm and related classes
   Chris Toney <chris.toney at usda.gov>
   Copyright (c) 2023-2025 gdalraster authors
*/

#ifndef SRC_GDALALG_H_
#define SRC_GDALALG_H_

#include <string>
#include <vector>

#include "rcpp_util.h"

Rcpp::DataFrame gdal_commands(std::string starts_with, bool cout);


#endif  // SRC_GDALALG_H_
