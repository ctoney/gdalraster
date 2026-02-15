#' Bounding box from dimensions and geotransform
#'
#' `bbox_from_dim_gt()` computes the bounding box from raster dimensions and
#' geotransform coefficients. For rotated rasters, returns the axis-aligned
#' envelope of the four corner coordinates.
#'
#' @param dm Integer vector of length two containing the raster dimensions
#' (xsize, ysize) in pixels.
#' @param gt Numeric vector of length six containing the geotransform
#' coefficients.
#' @returns Numeric vector of length four containing the bounding box
#' (xmin, ymin, xmax, ymax) in georeferenced coordinates.
#' @seealso [`GDALRaster$getGeoTransform()`][GDALRaster],
#' [`GDALRaster$bbox()`][GDALRaster], [gt_from_dim_bbox()],
#' [inv_geotransform()]
#' @examples
#' elev_file <- system.file("extdata/storml_elev.tif", package="gdalraster")
#' ds <- new(GDALRaster, elev_file)
#' gt <- ds$getGeoTransform()
#' dm <- ds$dim()
#'
#' # should match ds$bbox()
#' bbox_from_dim_gt(dm, gt)
#' ds$bbox()
#'
#' ds$close()
#' @export
bbox_from_dim_gt <- function(dm, gt) {
    .bbox_grid_to_geo(gt, 0, dm[1L], 0, dm[2L])
}

#' Geotransform from dimensions and bounding box
#'
#' `gt_from_dim_bbox()` computes geotransform coefficients from raster
#' dimensions and a bounding box. The resulting geotransform will always be
#' north-up (no rotation).
#'
#' @param dm Integer vector of length two containing the raster dimensions
#' (xsize, ysize) in pixels.
#' @param bbox Numeric vector of length four containing the bounding box
#' (xmin, ymin, xmax, ymax) in georeferenced coordinates.
#' @returns Numeric vector of length six containing the geotransform
#' coefficients.
#' @seealso [`GDALRaster$getGeoTransform()`][GDALRaster],
#' [`GDALRaster$setGeoTransform()`][GDALRaster],
#' [`GDALRaster$setBbox()`][GDALRaster], [bbox_from_dim_gt()],
#' [inv_geotransform()]
#' @examples
#' # 1-degree global grid
#' (gt <- gt_from_dim_bbox(c(360, 180), c(-180, -90, 180, 90)))
#'
#' # verify round-trip
#' bbox_from_dim_gt(c(360, 180), gt)
#' @export
gt_from_dim_bbox <- function(dm, bbox) {
    .gt_from_dim_bbox(dm, bbox)
}
