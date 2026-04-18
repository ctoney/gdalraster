#' Convenience functions for simple DEM derivatives
#'
#' Functions to calculate simple DEM derivatives, currently `northness()` and
#' `eastness()` for transforming aspect degrees into the range `-1:1`.
#'
#' @name dem_derivatives
#' @details
#'
#' `northness()` is a cosine transform of aspect degrees, with any flat aspect
#' values (`-1` if present) set to `90` degrees (east) as a neutral value:
#' ```
#' northness <- cos(asp_deg * pi / 180)
#' ```
#' 
#' `eastness()` is a sine transform of aspect degrees, with any flat aspect
#' values (`-1` if present) set to `0` degrees (north) as a neutral value:
#' ```
#' eastness <- sin(asp_deg * pi / 180)
#' ```
#'
#' @param asp_deg Numeric vector of aspect values in degrees (`0:360`).
#' @return
#' Numeric vector of transformed values.
#'
#' @note
#' No validation is done on the input values. The caller is responsible for
#' ensuring input has valid type and range.
#'
#' @seealso
#' [calc()], [dem_proc()]
#'
#' @examples
#' ## plot northness from slope-masked aspect for the Storm Lake AOI
#' f_dem <- system.file("extdata/storml_elev.tif", package="gdalraster")
#'
#' # slope degrees
#' f_slp <- basename(tempfile(pattern = "storml_slp", fileext = ".tif"))
#' f_slp <- file.path("/vsimem", f_slp)
#' dem_proc("slope", f_dem, f_slp)
#'
#' # aspect
#' f_asp <- basename(tempfile(pattern = "storml_asp", fileext = ".tif"))
#' f_asp <- file.path("/vsimem", f_asp)
#' dem_proc("aspect", f_dem, f_asp)
#'
#' # compute masked aspect as an in-memory raster
#' expr <- "ifelse(SLOPE >= 2, ASPECT, -9999)"
#' (ds_masked_asp <- calc(expr = expr,
#'                        rasterfiles = c(f_slp, f_asp),
#'                        var.names = c("SLOPE", "ASPECT"),
#'                        fmt = "MEM",
#'                        dtName = "Float64",
#'                        nodata_value = -9999,
#'                        setRasterNodataValue = TRUE,
#'                        return_obj = TRUE)
#'
#' # diverging palette for northness
#' # adapted from "heatmap3" in {ltc-color-palettes}
#' # https://github.com/loukesio/ltc-color-palettes
#' # Copyright 2021 ltc authors
#' pal <- c("#d7191c", "#fdae61", "#ffffbf", "#abd9e9")
#'
#' plot_raster(ds_masked_asp, legend = TRUE, col_map_fn = pal,
#'             pixel_fn = northness, na_col = "#2c7bb6",
#'             main = "Storm Lake AOI northness")
#'
#' # clean up
#' ds$close()
#' deleteDataset(f_slp)
#' deleteDataset(f_asp)
#'
#' @export
northness <- function(asp_deg) {
    # transform aspect degrees to northness
    # set flat to east (90 degrees) for neutral value
    asp_deg[asp_deg == -1] <- 90
    cos(asp_deg * pi / 180)
}

#' @name dem_derivatives
#' @export
eastness <- function(asp_deg) {
    # transform aspect degrees to eastness
    # set flat to north (0 degrees) for neutral value
    asp_deg[asp_deg == -1] <- 0
    sin(asp_deg * pi / 180)
}
