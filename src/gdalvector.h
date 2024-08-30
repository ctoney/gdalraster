/* R interface to a subset of the GDAL C API for vector. A class for OGRLayer,
   a layer of features in a GDALDataset. https://gdal.org/api/vector_c_api.html

   Chris Toney <chris.toney at usda.gov>
   Copyright (c) 2023-2024 gdalraster authors
*/

#ifndef SRC_GDALVECTOR_H_
#define SRC_GDALVECTOR_H_

#include <string>
#include <vector>

#include "rcpp_util.h"

// Predeclare some GDAL types until the public header is included
#ifndef GDAL_H_INCLUDED
typedef void *GDALDatasetH;
typedef void *OGRLayerH;
typedef enum {GA_ReadOnly = 0, GA_Update = 1} GDALAccess;
#endif

class GDALVector {
 public:
    GDALVector();
    explicit GDALVector(Rcpp::CharacterVector dsn);
    GDALVector(Rcpp::CharacterVector dsn, std::string layer);
    GDALVector(Rcpp::CharacterVector dsn, std::string layer, bool read_only);
    GDALVector(Rcpp::CharacterVector dsn, std::string layer, bool read_only,
               Rcpp::CharacterVector open_options);
    GDALVector(Rcpp::CharacterVector dsn, std::string layer, bool read_only,
               Rcpp::Nullable<Rcpp::CharacterVector> open_options,
               std::string spatial_filter, std::string dialect);

    // undocumented exposed read-only fields for internal use
    std::string m_layer_name;  // layer name or sql statement
    bool m_is_sql;
    std::string m_dialect;

    // exposed read-only fields
    Rcpp::List featureTemplate;

    // exposed read/write fields
    std::string defaultGeomFldName = "geometry";
    std::string returnGeomAs = "NONE";
    std::string wkbByteOrder = "LSB";

    // exposed methods
    void open(bool read_only);
    bool isOpen() const;
    std::string getDsn() const;
    Rcpp::CharacterVector getFileList() const;
    std::string getDriverShortName() const;
    std::string getDriverLongName() const;

    std::string getName() const;
    Rcpp::List testCapability() const;
    std::string getFIDColumn() const;
    std::string getGeomType() const;
    std::string getGeometryColumn() const;
    std::string getSpatialRef() const;
    Rcpp::NumericVector bbox();
    Rcpp::List getLayerDefn() const;

    void setAttributeFilter(std::string query);
    std::string getAttributeFilter() const;
    void setIgnoredFields(Rcpp::CharacterVector fields);

    void setSpatialFilter(std::string wkt);
    void setSpatialFilterRect(Rcpp::NumericVector bbox);
    std::string getSpatialFilter() const;
    void clearSpatialFilter();

    double getFeatureCount();
    SEXP getNextFeature();
    void setNextByIndex(double i);
    // fid must be a length-1 numeric vector, since numeric vector can carry
    // the class attribute for integer64:
    SEXP getFeature(Rcpp::NumericVector fid);
    void resetReading();

    Rcpp::DataFrame fetch(double n);

    bool deleteFeature(Rcpp::NumericVector fid);

    bool startTransaction(bool force);
    bool commitTransaction();
    bool rollbackTransaction();

    bool layerIntersection(
            GDALVector method_layer,
            GDALVector result_layer,
            bool quiet,
            Rcpp::Nullable<Rcpp::CharacterVector> options);
    bool layerUnion(
            GDALVector method_layer,
            GDALVector result_layer,
            bool quiet,
            Rcpp::Nullable<Rcpp::CharacterVector> options);
    bool layerSymDifference(
            GDALVector method_layer,
            GDALVector result_layer,
            bool quiet,
            Rcpp::Nullable<Rcpp::CharacterVector> options);
    bool layerIdentity(
            GDALVector method_layer,
            GDALVector result_layer,
            bool quiet,
            Rcpp::Nullable<Rcpp::CharacterVector> options);
    bool layerUpdate(
            GDALVector method_layer,
            GDALVector result_layer,
            bool quiet,
            Rcpp::Nullable<Rcpp::CharacterVector> options);
    bool layerClip(
            GDALVector method_layer,
            GDALVector result_layer,
            bool quiet,
            Rcpp::Nullable<Rcpp::CharacterVector> options);
    bool layerErase(
            GDALVector method_layer,
            GDALVector result_layer,
            bool quiet,
            Rcpp::Nullable<Rcpp::CharacterVector> options);

    void close();

    // methods for internal use not exposed to R
    void checkAccess_(GDALAccess access_needed) const;
    GDALDatasetH getGDALDatasetH_() const;
    void setGDALDatasetH_(GDALDatasetH hDs, bool with_update);
    OGRLayerH getOGRLayerH_() const;
    void setOGRLayerH_(OGRLayerH hLyr, std::string lyr_name);
    void setFeatureTemplate_();
    SEXP initDF_(R_xlen_t nrow) const;

 private:
    std::string m_dsn;
    Rcpp::CharacterVector m_open_options;
    std::string m_attr_filter = "";
    std::string m_spatial_filter;
    GDALDatasetH m_hDataset;
    GDALAccess m_eAccess;
    OGRLayerH m_hLayer;
};

RCPP_EXPOSED_CLASS(GDALVector)

#endif  // SRC_GDALVECTOR_H_