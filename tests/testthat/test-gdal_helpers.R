# apply_geotransform() and get_pixel_line() tests are in test-gdal_exp.R

test_that("gdal_compute_version works", {
    expect_equal(gdal_compute_version(3, 7, 0), 3070000L)
    expect_error(gdal_compute_version("3", 7, 0))
    expect_error(gdal_compute_version(3, "7", 0))
    expect_error(gdal_compute_version(3, 7, NULL))
})

test_that("addFilesInZip works", {
    # requires GDAL >= 3.7
    skip_if(as.integer(gdal_version()[2]) < 3070000)

    evt_file <- system.file("extdata/storml_evt.tif", package="gdalraster")
    evc_file <- system.file("extdata/storml_evc.tif", package="gdalraster")
    evh_file <- system.file("extdata/storml_evh.tif", package="gdalraster")
    files_to_add <- c(evt_file, evc_file, evh_file)
    zip_file <- paste0(tempdir(), "/", "storml.zip")
    addFilesInZip(
            zip_file,
            files_to_add,
            full_paths=FALSE,
            sozip_enabled="YES",
            sozip_chunk_size=16384,
            sozip_min_file_size=1024,
            content_type="TEST",
            num_threads=1)
    d <- unzip(zip_file, list=TRUE)
    expect_equal(nrow(d), 3)
    unlink(zip_file)
})

test_that("getCreationOptions works", {
    opt <- getCreationOptions("GTiff", "COMPRESS")
    expect_true(is.list(opt))
    expect_equal(names(opt), "COMPRESS")
    expect_equal(opt$COMPRESS$type, "string-select")
    expect_vector(opt$COMPRESS$values, ptype = character())
    all_opt <- getCreationOptions("GTiff")
    expect_true(is.list(all_opt))
    expect_true(length(names(all_opt)) > 10)
    expect_true(is.list(all_opt$TILED))
    expect_error(getCreationOptions("invalid format name"))
    expect_error(getCreationOptions(NA))

})

test_that("dump_open_datasets works", {
    elev_file <- system.file("extdata/storml_elev_orig.tif", package="gdalraster")
    ds <- new(GDALRaster, elev_file)
    expect_output(dump_open_datasets())
    expect_true(dump_open_datasets() > 0)
    ds$close()
})

test_that("inspectDataset works", {
    # GPKG with subdatasets
    # https://download.osgeo.org/gdal/data/geopackage/small_world_and_byte.gpkg
    src <-  system.file("extdata/small_world_and_byte.gpkg", package="gdalraster")
    dsinfo <- inspectDataset(src)
    expect_equal(dsinfo$format, "GPKG")
    expect_true(dsinfo$supports_raster)
    expect_true(dsinfo$contains_raster)
    expect_true(dsinfo$supports_subdatasets)
    expect_true(dsinfo$contains_subdatasets)
    expect_no_error(ds <- new(GDALRaster, dsinfo$subdataset_names[1]))
    expect_no_error(ds$close())
    expect_no_error(ds <- new(GDALRaster, dsinfo$subdataset_names[2]))
    expect_no_error(ds$close())
    expect_true(dsinfo$supports_vector)
    expect_false(dsinfo$contains_vector)
    expect_vector(dsinfo$layer_names, ptype = character(), size = 0)

    # GPKG with vector
    src <- system.file("extdata/ynp_fires_1984_2022.gpkg", package="gdalraster")
    dsinfo <- inspectDataset(src)
    expect_false(dsinfo$contains_raster)
    expect_false(dsinfo$contains_subdatasets)
    expect_vector(dsinfo$subdataset_names, ptype = character(), size = 0)
    expect_true(dsinfo$contains_vector)
    expect_equal(dsinfo$layer_names, "mtbs_perims")

    # shapefile
    src <- system.file("extdata/poly_multipoly.shp", package="gdalraster")
    dsinfo <- inspectDataset(src)
    expect_equal(dsinfo$format, "ESRI Shapefile")
    expect_false(dsinfo$supports_raster)
    expect_false(dsinfo$contains_raster)
    expect_false(dsinfo$supports_subdatasets)
    expect_false(dsinfo$contains_subdatasets)
    expect_true(dsinfo$supports_vector)
    expect_true(dsinfo$contains_vector)
    expect_vector(dsinfo$layer_names, ptype = character(), size = 1)

    # GTiff
    src <- system.file("extdata/storml_elev_orig.tif", package="gdalraster")
    dsinfo <- inspectDataset(src)
    expect_equal(dsinfo$format, "GTiff")
    expect_true(dsinfo$supports_raster)
    expect_true(dsinfo$contains_raster)
    expect_true(dsinfo$supports_subdatasets)
    expect_false(dsinfo$contains_subdatasets)
    expect_false(dsinfo$supports_vector)
    expect_false(dsinfo$contains_vector)
    expect_vector(dsinfo$layer_names, ptype = character(), size = 0)

    # PostGISRaster / PostgreSQL
    skip_if(nrow(gdal_formats("PostgreSQL")) == 0 ||
            nrow(gdal_formats("PostGISRaster")) == 0)

    dsn <- "PG:dbname='testdb', host='127.0.0.1' port='5444' user='user'
            password='pwd'"
    dsinfo <- inspectDataset(dsn)
    expect_equal(dsinfo$format, "PostgreSQL")
    expect_false(dsinfo$supports_raster)
    expect_false(dsinfo$contains_raster)
    expect_false(dsinfo$supports_subdatasets)
    expect_false(dsinfo$contains_subdatasets)
    expect_true(dsinfo$supports_vector)
    expect_false(dsinfo$contains_vector)
    expect_vector(dsinfo$layer_names, ptype = character(), size = 0)

    dsn <- "PG:dbname='testdb', host='127.0.0.1' port='5444' table='raster_tbl'
            column='raster_col' user='user' password='pwd'"
    dsinfo <- inspectDataset(dsn, vector = FALSE)
    expect_equal(dsinfo$format, "PostGISRaster")
    expect_true(dsinfo$supports_raster)
    expect_false(dsinfo$contains_raster)
    expect_true(dsinfo$supports_subdatasets)
    expect_false(dsinfo$contains_subdatasets)
    expect_false(dsinfo$supports_vector)
    expect_false(dsinfo$contains_vector)
    expect_vector(dsinfo$layer_names, ptype = character(), size = 0)
})

test_that("make_chunk_index works", {
    # the internal C++ function being called is tested in
    # tests/testthat/test-GDALRaster-class.R

    chunks <- make_chunk_index(raster_xsize = 156335, raster_ysize = 101538,
                               block_xsize = 256, block_ysize = 256,
                               gt = c(-2362395, 30, 0, 3267405, 0, -30),
                               max_pixels = 256 * 256 * 16)

    expect_equal(nrow(chunks), 15483)
})

test_that("vector_to_MEM basic functionality works", {
    xsize <- 10L
    ysize <- 10L

    ## raw -> Byte / UInt8
    v <- sample(0:255, xsize * ysize, replace = TRUE)
    v_raw <- as.raw(v)
    expect_no_error(ds_mem <- vector_to_MEM(v_raw, xsize, ysize))
    dt <- ds_mem$getDataTypeName(1)
    expect_true(dt == "Byte" || dt == "UInt8")
    res <- ds_mem$read(1, 0, 0, xsize, ysize, xsize, ysize)
    expect_equal(res, v)
    ds_mem$readByteAsRaw <- TRUE
    res <- ds_mem$read(1, 0, 0, xsize, ysize, xsize, ysize)
    expect_equal(res, v_raw)
    ds_mem$close()
    # multi-band
    expect_no_error(ds_mem <- vector_to_MEM(v_raw, 5, 10, nbands = 2))
    res1 <- ds_mem$read(1, 0, 0, 5, 10, 5, 10)
    expect_equal(res1, v[1:50])
    res2 <- ds_mem$read(2, 0, 0, 5, 10, 5, 10)
    expect_equal(res2, v[51:100])
    ds_mem$close()

    ## integer -> Int32
    v_int <- sample(-32767:32767, xsize * ysize, replace = TRUE)
    expect_no_error(ds_mem <- vector_to_MEM(v_int, xsize, ysize))
    dt <- ds_mem$getDataTypeName(1)
    expect_true(dt == "Int32")
    res <- ds_mem$read(1, 0, 0, xsize, ysize, xsize, ysize)
    expect_equal(res, v_int)
    ds_mem$close()

    ## double -> Float64
    v_dbl <- v_int + 0.5
    expect_no_error(ds_mem <- vector_to_MEM(v_dbl, xsize, ysize))
    dt <- ds_mem$getDataTypeName(1)
    expect_true(dt == "Float64")
    res <- ds_mem$read(1, 0, 0, xsize, ysize, xsize, ysize)
    expect_equal(res, v_dbl, tolerance = 1e4)
    ds_mem$close()

    ## complex -> CFloat64
    z <- complex(real = stats::rnorm(100), imaginary = stats::rnorm(100))
    ds_mem <- vector_to_MEM(z, xsize, ysize)
    dt <- ds_mem$getDataTypeName(1)
    expect_true(dt == "CFloat64")
    res <- ds_mem$read(1, 0, 0, xsize, ysize, xsize, ysize)
    expect_equal(res, z, tolerance = 1e4)
    ds_mem$close()

    ## with arguments for gt, bbox, srs
    f <- system.file("extdata/storml_elev.tif", package = "gdalraster")
    ds <- new(GDALRaster, f)
    dm <- ds$dim()
    xsize = as.integer(dm[1])
    ysize = as.integer(dm[2])
    v_elev <- ds$read(1, 0, 0, xsize, ysize, xsize, ysize)

    expect_no_error(ds_mem <- vector_to_MEM(v_elev, xsize, ysize,
                                            gt = ds$getGeoTransform(),
                                            srs = ds$getProjection()))

    expect_equal(ds_mem$bbox(), ds$bbox(), tolerance = 0.1)
    expect_true(srs_is_same(ds_mem$getProjection(), ds$getProjection()))
    ds_mem$close()

    expect_no_error(ds_mem <- vector_to_MEM(v_elev, xsize, ysize,
                                            bbox = ds$bbox(),
                                            srs = ds$getProjection()))

    expect_equal(ds_mem$getGeoTransform(), ds$getGeoTransform(),
                 tolerance = 0.1)
    expect_true(srs_is_same(ds_mem$getProjection(), ds$getProjection()))
    ds_mem$close()

    expect_warning(ds_mem <- vector_to_MEM(v_elev, xsize, ysize,
                                           gt = ds$getGeoTransform(),
                                           srs = "invalid"))

    ## write to the MEM dataset
    expect_true(ds_mem$setProjection(ds$getProjection()))
    expect_true(srs_is_same(ds_mem$getProjection(), ds$getProjection()))
    v_seq <- seq_len(xsize * ysize)
    expect_no_error(ds_mem$write(1, 0, 0, xsize, ysize, v_seq))
    # original vector v_elev is modified in place
    expect_equal(v_elev, v_seq)

    ds_mem$close()
    ds$close()

    ## errors / input validation
    expect_error(ds_mem <- vector_to_MEM(rep("1", 100), xsize, ysize))
    expect_error(ds_mem <- vector_to_MEM(v_elev, c(10, 10), ysize))
    expect_error(ds_mem <- vector_to_MEM(v_elev, xsize, c(10, 10)))
    expect_error(ds_mem <- vector_to_MEM(v_elev, xsize, ysize, nbands = c(1,2)))
    expect_error(ds_mem <- vector_to_MEM(v_elev, xsize, ysize, nbands = 2))
    expect_error(ds_mem <- vector_to_MEM(v_elev, xsize, ysize,
                 gt = c(1, 2, 3, 4, 5)))
    expect_error(ds_mem <- vector_to_MEM(v_elev, xsize, ysize,
                 bbox = c(1, 2, 3)))
    expect_error(ds_mem <- vector_to_MEM(v_elev, xsize, ysize,
                 srs = 4326))

})

test_that("vector_to_MEM works with object dereference and garbage collect", {
    # test with
    #   - R object dereferenced and garbage collected
    #   - GDALRaster object garbage collected without explicit close()

    xsize <- 10L
    ysize <- 10L

    ## R object dereferenced and garbage collected
    v <- sample(-32767:32767, xsize * ysize, replace = TRUE)
    expect_no_error(ds_mem <- vector_to_MEM(v, xsize, ysize))
    res <- ds_mem$read(1, 0, 0, xsize, ysize, xsize, ysize)
    expect_equal(res, v)
    rm(v)
    gc()
    expect_equal(ds_mem$read(1, 0, 0, xsize, ysize, xsize, ysize), res)
    ds_mem$close()
    rm(ds_mem)
    rm(res)
    gc()

    v2 <- stats::rnorm(100)
    expect_no_error(ds_mem <- vector_to_MEM(v2, xsize, ysize))
    res <- ds_mem$read(1, 0, 0, xsize, ysize, xsize, ysize)
    expect_equal(res, v2)
    rm(v2)
    gc()
    expect_equal(ds_mem$read(1, 0, 0, xsize, ysize, xsize, ysize), res)
    ds_mem$close()
    rm(ds_mem)
    rm(res)
    gc()

    ## GDALRaster object garbage collected without explicit close()
    v3 <- sample(0:255, xsize * ysize, replace = TRUE)
    expect_no_error(ds_mem <- vector_to_MEM(v3, xsize, ysize))
    res <- ds_mem$read(1, 0, 0, xsize, ysize, xsize, ysize)
    expect_equal(res, v3)
    rm(v3)
    rm(ds_mem)
    gc()
})
