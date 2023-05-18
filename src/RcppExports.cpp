// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include "gdalraster_types.h"
#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// gdal_version
Rcpp::CharacterVector gdal_version();
RcppExport SEXP _gdalraster_gdal_version() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(gdal_version());
    return rcpp_result_gen;
END_RCPP
}
// get_config_option
std::string get_config_option(std::string key);
RcppExport SEXP _gdalraster_get_config_option(SEXP keySEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type key(keySEXP);
    rcpp_result_gen = Rcpp::wrap(get_config_option(key));
    return rcpp_result_gen;
END_RCPP
}
// set_config_option
void set_config_option(std::string key, std::string value);
RcppExport SEXP _gdalraster_set_config_option(SEXP keySEXP, SEXP valueSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type key(keySEXP);
    Rcpp::traits::input_parameter< std::string >::type value(valueSEXP);
    set_config_option(key, value);
    return R_NilValue;
END_RCPP
}
// create
bool create(std::string format, std::string dst_filename, int xsize, int ysize, int nbands, std::string dataType, Rcpp::Nullable<Rcpp::CharacterVector> options);
RcppExport SEXP _gdalraster_create(SEXP formatSEXP, SEXP dst_filenameSEXP, SEXP xsizeSEXP, SEXP ysizeSEXP, SEXP nbandsSEXP, SEXP dataTypeSEXP, SEXP optionsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type format(formatSEXP);
    Rcpp::traits::input_parameter< std::string >::type dst_filename(dst_filenameSEXP);
    Rcpp::traits::input_parameter< int >::type xsize(xsizeSEXP);
    Rcpp::traits::input_parameter< int >::type ysize(ysizeSEXP);
    Rcpp::traits::input_parameter< int >::type nbands(nbandsSEXP);
    Rcpp::traits::input_parameter< std::string >::type dataType(dataTypeSEXP);
    Rcpp::traits::input_parameter< Rcpp::Nullable<Rcpp::CharacterVector> >::type options(optionsSEXP);
    rcpp_result_gen = Rcpp::wrap(create(format, dst_filename, xsize, ysize, nbands, dataType, options));
    return rcpp_result_gen;
END_RCPP
}
// createCopy
bool createCopy(std::string format, std::string dst_filename, std::string src_filename, bool strict, Rcpp::Nullable<Rcpp::CharacterVector> options);
RcppExport SEXP _gdalraster_createCopy(SEXP formatSEXP, SEXP dst_filenameSEXP, SEXP src_filenameSEXP, SEXP strictSEXP, SEXP optionsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type format(formatSEXP);
    Rcpp::traits::input_parameter< std::string >::type dst_filename(dst_filenameSEXP);
    Rcpp::traits::input_parameter< std::string >::type src_filename(src_filenameSEXP);
    Rcpp::traits::input_parameter< bool >::type strict(strictSEXP);
    Rcpp::traits::input_parameter< Rcpp::Nullable<Rcpp::CharacterVector> >::type options(optionsSEXP);
    rcpp_result_gen = Rcpp::wrap(createCopy(format, dst_filename, src_filename, strict, options));
    return rcpp_result_gen;
END_RCPP
}
// inv_geotransform
Rcpp::NumericVector inv_geotransform(const std::vector<double> gt);
RcppExport SEXP _gdalraster_inv_geotransform(SEXP gtSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::vector<double> >::type gt(gtSEXP);
    rcpp_result_gen = Rcpp::wrap(inv_geotransform(gt));
    return rcpp_result_gen;
END_RCPP
}
// get_pixel_line
Rcpp::IntegerMatrix get_pixel_line(const Rcpp::NumericMatrix xy, const std::vector<double> gt);
RcppExport SEXP _gdalraster_get_pixel_line(SEXP xySEXP, SEXP gtSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::NumericMatrix >::type xy(xySEXP);
    Rcpp::traits::input_parameter< const std::vector<double> >::type gt(gtSEXP);
    rcpp_result_gen = Rcpp::wrap(get_pixel_line(xy, gt));
    return rcpp_result_gen;
END_RCPP
}
// warp
bool warp(std::vector<std::string> src_files, std::string dst_filename, Rcpp::CharacterVector t_srs, Rcpp::Nullable<Rcpp::CharacterVector> cl_arg);
RcppExport SEXP _gdalraster_warp(SEXP src_filesSEXP, SEXP dst_filenameSEXP, SEXP t_srsSEXP, SEXP cl_argSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::vector<std::string> >::type src_files(src_filesSEXP);
    Rcpp::traits::input_parameter< std::string >::type dst_filename(dst_filenameSEXP);
    Rcpp::traits::input_parameter< Rcpp::CharacterVector >::type t_srs(t_srsSEXP);
    Rcpp::traits::input_parameter< Rcpp::Nullable<Rcpp::CharacterVector> >::type cl_arg(cl_argSEXP);
    rcpp_result_gen = Rcpp::wrap(warp(src_files, dst_filename, t_srs, cl_arg));
    return rcpp_result_gen;
END_RCPP
}
// _combine
Rcpp::DataFrame _combine(Rcpp::CharacterVector src_files, Rcpp::CharacterVector var_names, std::vector<int> bands, std::string dst_filename, std::string fmt, std::string dataType, Rcpp::Nullable<Rcpp::CharacterVector> options);
RcppExport SEXP _gdalraster__combine(SEXP src_filesSEXP, SEXP var_namesSEXP, SEXP bandsSEXP, SEXP dst_filenameSEXP, SEXP fmtSEXP, SEXP dataTypeSEXP, SEXP optionsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::CharacterVector >::type src_files(src_filesSEXP);
    Rcpp::traits::input_parameter< Rcpp::CharacterVector >::type var_names(var_namesSEXP);
    Rcpp::traits::input_parameter< std::vector<int> >::type bands(bandsSEXP);
    Rcpp::traits::input_parameter< std::string >::type dst_filename(dst_filenameSEXP);
    Rcpp::traits::input_parameter< std::string >::type fmt(fmtSEXP);
    Rcpp::traits::input_parameter< std::string >::type dataType(dataTypeSEXP);
    Rcpp::traits::input_parameter< Rcpp::Nullable<Rcpp::CharacterVector> >::type options(optionsSEXP);
    rcpp_result_gen = Rcpp::wrap(_combine(src_files, var_names, bands, dst_filename, fmt, dataType, options));
    return rcpp_result_gen;
END_RCPP
}
// _has_geos
bool _has_geos();
RcppExport SEXP _gdalraster__has_geos() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(_has_geos());
    return rcpp_result_gen;
END_RCPP
}
// _g_create
std::string _g_create(Rcpp::NumericMatrix xy, std::string geom_type);
RcppExport SEXP _gdalraster__g_create(SEXP xySEXP, SEXP geom_typeSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type xy(xySEXP);
    Rcpp::traits::input_parameter< std::string >::type geom_type(geom_typeSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_create(xy, geom_type));
    return rcpp_result_gen;
END_RCPP
}
// _g_is_valid
bool _g_is_valid(std::string geom);
RcppExport SEXP _gdalraster__g_is_valid(SEXP geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type geom(geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_is_valid(geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_intersects
bool _g_intersects(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_intersects(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_intersects(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_equals
bool _g_equals(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_equals(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_equals(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_disjoint
bool _g_disjoint(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_disjoint(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_disjoint(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_touches
bool _g_touches(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_touches(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_touches(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_contains
bool _g_contains(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_contains(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_contains(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_within
bool _g_within(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_within(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_within(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_crosses
bool _g_crosses(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_crosses(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_crosses(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_overlaps
bool _g_overlaps(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_overlaps(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_overlaps(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_buffer
std::string _g_buffer(std::string geom, double dist, int quad_segs);
RcppExport SEXP _gdalraster__g_buffer(SEXP geomSEXP, SEXP distSEXP, SEXP quad_segsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type geom(geomSEXP);
    Rcpp::traits::input_parameter< double >::type dist(distSEXP);
    Rcpp::traits::input_parameter< int >::type quad_segs(quad_segsSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_buffer(geom, dist, quad_segs));
    return rcpp_result_gen;
END_RCPP
}
// _g_intersection
std::string _g_intersection(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_intersection(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_intersection(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_union
std::string _g_union(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_union(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_union(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_difference
std::string _g_difference(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_difference(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_difference(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_sym_difference
std::string _g_sym_difference(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_sym_difference(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_sym_difference(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_distance
double _g_distance(std::string this_geom, std::string other_geom);
RcppExport SEXP _gdalraster__g_distance(SEXP this_geomSEXP, SEXP other_geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type this_geom(this_geomSEXP);
    Rcpp::traits::input_parameter< std::string >::type other_geom(other_geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_distance(this_geom, other_geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_length
double _g_length(std::string geom);
RcppExport SEXP _gdalraster__g_length(SEXP geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type geom(geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_length(geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_area
double _g_area(std::string geom);
RcppExport SEXP _gdalraster__g_area(SEXP geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type geom(geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_area(geom));
    return rcpp_result_gen;
END_RCPP
}
// _g_centroid
Rcpp::NumericVector _g_centroid(std::string geom);
RcppExport SEXP _gdalraster__g_centroid(SEXP geomSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type geom(geomSEXP);
    rcpp_result_gen = Rcpp::wrap(_g_centroid(geom));
    return rcpp_result_gen;
END_RCPP
}
// inv_project
Rcpp::NumericMatrix inv_project(Rcpp::NumericMatrix& pts, std::string srs, std::string well_known_gcs);
RcppExport SEXP _gdalraster_inv_project(SEXP ptsSEXP, SEXP srsSEXP, SEXP well_known_gcsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix& >::type pts(ptsSEXP);
    Rcpp::traits::input_parameter< std::string >::type srs(srsSEXP);
    Rcpp::traits::input_parameter< std::string >::type well_known_gcs(well_known_gcsSEXP);
    rcpp_result_gen = Rcpp::wrap(inv_project(pts, srs, well_known_gcs));
    return rcpp_result_gen;
END_RCPP
}
// transform_xy
Rcpp::NumericMatrix transform_xy(Rcpp::NumericMatrix& pts, std::string srs_from, std::string srs_to);
RcppExport SEXP _gdalraster_transform_xy(SEXP ptsSEXP, SEXP srs_fromSEXP, SEXP srs_toSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix& >::type pts(ptsSEXP);
    Rcpp::traits::input_parameter< std::string >::type srs_from(srs_fromSEXP);
    Rcpp::traits::input_parameter< std::string >::type srs_to(srs_toSEXP);
    rcpp_result_gen = Rcpp::wrap(transform_xy(pts, srs_from, srs_to));
    return rcpp_result_gen;
END_RCPP
}
// epsg_to_wkt
std::string epsg_to_wkt(int epsg, bool pretty);
RcppExport SEXP _gdalraster_epsg_to_wkt(SEXP epsgSEXP, SEXP prettySEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< int >::type epsg(epsgSEXP);
    Rcpp::traits::input_parameter< bool >::type pretty(prettySEXP);
    rcpp_result_gen = Rcpp::wrap(epsg_to_wkt(epsg, pretty));
    return rcpp_result_gen;
END_RCPP
}
// srs_to_wkt
std::string srs_to_wkt(std::string srs, bool pretty);
RcppExport SEXP _gdalraster_srs_to_wkt(SEXP srsSEXP, SEXP prettySEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type srs(srsSEXP);
    Rcpp::traits::input_parameter< bool >::type pretty(prettySEXP);
    rcpp_result_gen = Rcpp::wrap(srs_to_wkt(srs, pretty));
    return rcpp_result_gen;
END_RCPP
}
// srs_is_geographic
bool srs_is_geographic(std::string srs);
RcppExport SEXP _gdalraster_srs_is_geographic(SEXP srsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type srs(srsSEXP);
    rcpp_result_gen = Rcpp::wrap(srs_is_geographic(srs));
    return rcpp_result_gen;
END_RCPP
}
// srs_is_projected
bool srs_is_projected(std::string srs);
RcppExport SEXP _gdalraster_srs_is_projected(SEXP srsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type srs(srsSEXP);
    rcpp_result_gen = Rcpp::wrap(srs_is_projected(srs));
    return rcpp_result_gen;
END_RCPP
}
// bbox_from_wkt
Rcpp::NumericVector bbox_from_wkt(std::string wkt);
RcppExport SEXP _gdalraster_bbox_from_wkt(SEXP wktSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type wkt(wktSEXP);
    rcpp_result_gen = Rcpp::wrap(bbox_from_wkt(wkt));
    return rcpp_result_gen;
END_RCPP
}
// bbox_to_wkt
std::string bbox_to_wkt(Rcpp::NumericVector bbox);
RcppExport SEXP _gdalraster_bbox_to_wkt(SEXP bboxSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::NumericVector >::type bbox(bboxSEXP);
    rcpp_result_gen = Rcpp::wrap(bbox_to_wkt(bbox));
    return rcpp_result_gen;
END_RCPP
}

RcppExport SEXP _rcpp_module_boot_mod_cmb_table();
RcppExport SEXP _rcpp_module_boot_mod_GDALRaster();
RcppExport SEXP _rcpp_module_boot_mod_running_stats();

static const R_CallMethodDef CallEntries[] = {
    {"_gdalraster_gdal_version", (DL_FUNC) &_gdalraster_gdal_version, 0},
    {"_gdalraster_get_config_option", (DL_FUNC) &_gdalraster_get_config_option, 1},
    {"_gdalraster_set_config_option", (DL_FUNC) &_gdalraster_set_config_option, 2},
    {"_gdalraster_create", (DL_FUNC) &_gdalraster_create, 7},
    {"_gdalraster_createCopy", (DL_FUNC) &_gdalraster_createCopy, 5},
    {"_gdalraster_inv_geotransform", (DL_FUNC) &_gdalraster_inv_geotransform, 1},
    {"_gdalraster_get_pixel_line", (DL_FUNC) &_gdalraster_get_pixel_line, 2},
    {"_gdalraster_warp", (DL_FUNC) &_gdalraster_warp, 4},
    {"_gdalraster__combine", (DL_FUNC) &_gdalraster__combine, 7},
    {"_gdalraster__has_geos", (DL_FUNC) &_gdalraster__has_geos, 0},
    {"_gdalraster__g_create", (DL_FUNC) &_gdalraster__g_create, 2},
    {"_gdalraster__g_is_valid", (DL_FUNC) &_gdalraster__g_is_valid, 1},
    {"_gdalraster__g_intersects", (DL_FUNC) &_gdalraster__g_intersects, 2},
    {"_gdalraster__g_equals", (DL_FUNC) &_gdalraster__g_equals, 2},
    {"_gdalraster__g_disjoint", (DL_FUNC) &_gdalraster__g_disjoint, 2},
    {"_gdalraster__g_touches", (DL_FUNC) &_gdalraster__g_touches, 2},
    {"_gdalraster__g_contains", (DL_FUNC) &_gdalraster__g_contains, 2},
    {"_gdalraster__g_within", (DL_FUNC) &_gdalraster__g_within, 2},
    {"_gdalraster__g_crosses", (DL_FUNC) &_gdalraster__g_crosses, 2},
    {"_gdalraster__g_overlaps", (DL_FUNC) &_gdalraster__g_overlaps, 2},
    {"_gdalraster__g_buffer", (DL_FUNC) &_gdalraster__g_buffer, 3},
    {"_gdalraster__g_intersection", (DL_FUNC) &_gdalraster__g_intersection, 2},
    {"_gdalraster__g_union", (DL_FUNC) &_gdalraster__g_union, 2},
    {"_gdalraster__g_difference", (DL_FUNC) &_gdalraster__g_difference, 2},
    {"_gdalraster__g_sym_difference", (DL_FUNC) &_gdalraster__g_sym_difference, 2},
    {"_gdalraster__g_distance", (DL_FUNC) &_gdalraster__g_distance, 2},
    {"_gdalraster__g_length", (DL_FUNC) &_gdalraster__g_length, 1},
    {"_gdalraster__g_area", (DL_FUNC) &_gdalraster__g_area, 1},
    {"_gdalraster__g_centroid", (DL_FUNC) &_gdalraster__g_centroid, 1},
    {"_gdalraster_inv_project", (DL_FUNC) &_gdalraster_inv_project, 3},
    {"_gdalraster_transform_xy", (DL_FUNC) &_gdalraster_transform_xy, 3},
    {"_gdalraster_epsg_to_wkt", (DL_FUNC) &_gdalraster_epsg_to_wkt, 2},
    {"_gdalraster_srs_to_wkt", (DL_FUNC) &_gdalraster_srs_to_wkt, 2},
    {"_gdalraster_srs_is_geographic", (DL_FUNC) &_gdalraster_srs_is_geographic, 1},
    {"_gdalraster_srs_is_projected", (DL_FUNC) &_gdalraster_srs_is_projected, 1},
    {"_gdalraster_bbox_from_wkt", (DL_FUNC) &_gdalraster_bbox_from_wkt, 1},
    {"_gdalraster_bbox_to_wkt", (DL_FUNC) &_gdalraster_bbox_to_wkt, 1},
    {"_rcpp_module_boot_mod_cmb_table", (DL_FUNC) &_rcpp_module_boot_mod_cmb_table, 0},
    {"_rcpp_module_boot_mod_GDALRaster", (DL_FUNC) &_rcpp_module_boot_mod_GDALRaster, 0},
    {"_rcpp_module_boot_mod_running_stats", (DL_FUNC) &_rcpp_module_boot_mod_running_stats, 0},
    {NULL, NULL, 0}
};

void _gdal_init(DllInfo *dll);
RcppExport void R_init_gdalraster(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
    _gdal_init(dll);
}