# skip on CRAN while dev status of CLI bindings is "experimental"
skip_on_cran()
skip_if(gdal_version_num() < gdal_compute_version(3, 11, 3))

test_that("gdal_commands works", {
    expect_output(cmds <- gdal_commands())
    expect_true(is.data.frame(cmds))
    expect_true(nrow(cmds) > 10)
    expect_equal(colnames(cmds), c("command", "description", "URL"))

    expect_invisible(gdal_commands("raster info", cout = FALSE))
    expect_silent(cmds <- gdal_commands("raster info", cout = FALSE))
    expect_true(is.data.frame(cmds))
    expect_equal(nrow(cmds), 1)

    expect_no_error(cmds <- gdal_commands(recurse = FALSE, cout = FALSE))
    expect_equal(nrow(cmds[cmds$command_string == "raster info", ]), 0)

    expect_no_error(gdal_commands(NULL))
    expect_no_error(gdal_commands(c("raster", "info")))
    expect_error(gdal_commands(0))
    expect_error(gdal_commands(recurse = "invalid"))
    expect_error(gdal_commands(cout = "invalid"))
})

test_that("gdal_run works", {
    ## raster output
    f <- system.file("extdata/ynp_fires_1984_2022.gpkg", package = "gdalraster")
    f_out = file.path(tempdir(), "ynp_fire_year.tif")
    on.exit(deleteDataset(f_out), add = TRUE)

    args <- list()
    args$input <- f
    args$sql <- "SELECT * FROM mtbs_perims ORDER BY mtbs_perims.ig_year"
    args$attribute_name <- "ig_year"
    args$output <- f_out
    args$overwrite <- TRUE
    args$creation_option <- c("TILED=YES", "COMPRESS=DEFLATE")
    args$resolution <- c(90, 90)
    args$output_data_type <- "Int16"
    args$init <- -32767
    args$nodata <- -32767

    expect_no_error(alg <- gdal_run("vector rasterize", args))

    ds <- alg$output()
    expect_true(is(ds, "Rcpp_GDALRaster"))
    expect_equal(ds$res(), c(90, 90))
    expect_equal(ds$getDataTypeName(band = 1), "Int16")
    expect_equal(ds$getNoDataValue(band = 1), -32767)

    ds$close()
    expect_true(alg$close())
    alg$release()

    # with close
    deleteDataset(f_out)

    expect_no_error(
        gdal_run("vector rasterize", args, close = TRUE, quiet = TRUE))

    expect_true(vsi_stat_size(f_out) > 0)
    expect_no_error(ds <- GDALRaster$new(f_out))
    expect_equal(ds$res(), c(90, 90))
    expect_equal(ds$getDataTypeName(band = 1), "Int16")
    expect_equal(ds$getNoDataValue(band = 1), -32767)

    ds$close()

    ## vector output
    f_shp <- system.file("extdata/poly_multipoly.shp", package="gdalraster")
    f_gpkg <- file.path(tempdir(), "polygons_test.gpkg")
    on.exit(deleteDataset(f_gpkg), add = TRUE)

    args <- c("--input", f_shp, "--output", f_gpkg, "--overwrite")

    expect_no_error(alg <- gdal_run("vector convert", args))

    lyr <- alg$output()
    expect_true(is(lyr, "Rcpp_GDALVector"))
    expect_equal(lyr$getDriverShortName(), "GPKG")

    lyr$close()
    expect_true(alg$close())
    alg$release()

    # errors
    expect_error(gdal_run(NULL))
    expect_error(gdal_run(0))
    expect_error(gdal_run("raster info"))  # no args so alg$run() fails
    expect_error(gdal_run("raster info", 0))
    expect_error(gdal_run("raster info", "--invalid=0"))  # parse fails
    # invalid input for setVectorArgsFromObject:
    args <- c("--input", f_shp, "--output", f_gpkg, "--overwrite")
    expect_error(gdal_run("vector convert", args, "invalid"))

})

test_that("gdal_run_piped works", {
    # "size" is passed to "raster resize" here since the "resolution" argument
    # was added later in GDAL 3.12, so this test will work with GDAL >= 3.11.
    # This will generate an output raster with _roughly_ 90-m pixel resolution.
    ds_mem <- system.file("extdata/storml_elev.tif", package="gdalraster") |>
        gdal_run_piped("raster resize", "", "MEM", list(
            size = c(48, 36), resampling = "bilinear")
        ) |>
        gdal_run_piped("raster tpi", "", "MEM")

    expect_true(is(ds_mem, "Rcpp_GDALRaster"))
    expect_equal(ds_mem$dim(), c(48, 36, 1))
    expect_true(all(ds_mem$res() >= 89))
    ds_mem$close()

    f_zip <- system.file("extdata/ynp_features.zip", package = "gdalraster")

    # output_index
    out_str <- file.path("/vsizip", f_zip, "ynp_features.gpkg") |>
	    gdal_run_piped("vector info", output_format = "text",
                       other_args = list(layer = "ynp_bnd"),
                       output_index = "output_string")

    expect_true(is.character(out_str))
    expect_true(grepl("Layer name: ynp_bnd", out_str, ignore.case = TRUE))

    # input validation error: output_index < 1
    out_str <- ""
    expect_error(
        out_str <- file.path("/vsizip", f_zip, "ynp_features.gpkg") |>
            gdal_run_piped("vector info", output_format = "text",
                           other_args = list(layer = "ynp_bnd"),
                           output_index = 0)
    )

    # output_index > the number of outputs
    out_str <- ""
    expect_no_error(
        out_str <- file.path("/vsizip", f_zip, "ynp_features.gpkg") |>
            gdal_run_piped("vector info", output_format = "text",
                           other_args = list(layer = "ynp_bnd"),
                           output_index = 2)
    )
    expect_false(out_str)

    # output_index not a valid list element name
    out_str <- ""
    expect_no_error(
        out_str <- file.path("/vsizip", f_zip, "ynp_features.gpkg") |>
            gdal_run_piped("vector info", output_format = "text",
                           other_args = list(layer = "ynp_bnd"),
                           output_index = "invalid")
    )
    expect_false(out_str)

    # outputLayerNameForOpen
    lyr_mem <- file.path("/vsizip", f_zip, "ynp_features.gpkg") |>
        gdal_run_piped("vector reproject", "", "MEM", c("-d", "EPSG:5070"),
            outputLayerNameForOpen = "points_of_interest")

    expect_true(is(lyr_mem, "Rcpp_GDALVector"))
    expect_equal(lyr_mem$getName(), "points_of_interest")
    expect_equal(lyr_mem$getFeatureCount(), 1399)
    lyr_mem$close()
})

test_that("gdal_alg works", {
    expect_no_error(alg <- gdal_alg())
    expect_true(is(alg, "Rcpp_GDALAlg"))
    expect_equal(alg$info()$name, "gdal")
    alg$release()

    expect_no_error(alg <- gdal_alg("raster"))
    expect_true(is(alg, "Rcpp_GDALAlg"))
    alg$release()

    expect_no_error(alg <- gdal_alg("vector"))
    expect_true(is(alg, "Rcpp_GDALAlg"))
    alg$release()

    f <- system.file("extdata/storml_elev.tif", package="gdalraster")

    # character vector args
    args <- c("--format=text", f)
    expect_no_error(alg <- gdal_alg("raster info", args))
    expect_error(alg$output())
    expect_true(alg$run())
    expect_true(nchar(alg$output()) > 1000)
    alg$release()

    # parse = FALSE
    args <- c("--format=text", f)
    expect_no_error(alg <- gdal_alg("raster info", args, FALSE))
    expect_error(alg$output())
    expect_true(alg$parseCommandLineArgs())
    expect_true(alg$run())
    expect_true(nchar(alg$output()) > 1000)
    alg$release()

    # list args
    args <- list()
    args$input <- f
    args$output_format <- "text"
    expect_no_error(alg <- gdal_alg("raster info", args))
    expect_true(alg$run())
    expect_true(nchar(alg$output()) > 1000)
    alg$release()

    # input as object
    ds <- new(GDALRaster, f)
    args <- list()
    args$input <- ds
    args$output_format <- "text"
    expect_no_error(alg <- gdal_alg("raster info", args))
    expect_true(alg$run())
    expect_true(nchar(alg$output()) > 1000)
    expect_true(alg$close())
    alg$release()

    # errors
    expect_error(gdal_alg(0))
    expect_error(gdal_alg("raster info", FALSE))
    expect_error(gdal_alg("raster info", args, parse = "invalid"))
    args$invalid <- "invalid arg name"
    expect_error(gdal_alg("raster info", args))

    ds$close()
})

test_that("gdal_usage works", {
    cmd <- "raster reproject"
    expect_output(gdal_usage(cmd), "Usage:")
    expect_output(gdal_usage(cmd), "Positional arguments:")
    expect_output(gdal_usage(cmd), "Options:")
    expect_output(gdal_usage(cmd), "Advanced options:")
    expect_output(gdal_usage(cmd), "For more details:")

    expect_no_error(gdal_usage("pipeline"))
})

test_that("gdal_global_reg_names returns a character vector", {
    expect_vector(gdal_global_reg_names(), character())
})

test_that("raster pipeline works", {
    ## test raster pipeline algorithms
    skip_if(gdal_version_num() < gdal_compute_version(3, 12, 1))

    ## with a nested input pipeline
    f <- system.file("extdata/storml_elev.tif", package="gdalraster")
    f_elev <- tempfile(fileext = ".tif")
    file.copy(f, f_elev)
    on.exit(deleteDataset(f_elev), add = TRUE)
    f_pal <- system.file("extdata/storml_elev_pal.txt", package="gdalraster")
    f_out <- file.path(tempdir(), "storml_col_relief.tif")
    on.exit(deleteDataset(f_out), add = TRUE)

    args <- paste(
        "read --input", f_elev,
        "! color-map --color-map", f_pal,
        "! blend --overlay [ read --input", f_elev, "! hillshade -z 1.5 ]",
            "--operator=hsv-value",
        "! write --output", f_out, "--overwrite")

    expect_no_error(alg <- gdal_run("raster pipeline", args))
    expect_true(is.list(alg$outputs()))
    ds <- alg$outputs()$output
    expect_true(is(ds, "Rcpp_GDALRaster"))
    expect_equal(ds$res(), c(30, 30))
    expect_equal(ds$dim(), c(143, 107, 3))

    ds$close()
    expect_true(alg$close())
    alg$release()
})
