test_that("dem_derivatives work", {
    f_dem <- system.file("extdata/storml_elev.tif", package="gdalraster")

    f_asp <- basename(tempfile(pattern = "storml_asp", fileext = ".tif"))
    f_asp <- file.path("/vsimem", f_asp)
    on.exit(deleteDataset(f_asp), add = TRUE)
    dem_proc("aspect", f_dem, f_asp)

    ds_asp <- new(GDALRaster, f_asp)
    dm <- ds_asp$dim()

    # northness
    r <- ds_asp$read(1, 0, 0, dm[1], dm[2], dm[1], dm[2])
    expected <- r[r == -1] <- 90
    expected <- cos(r * pi / 180)
    expect_equal(northness(r), expected)

    # eastness
    r <- ds_asp$read(1, 0, 0, dm[1], dm[2], dm[1], dm[2])
    expected <- r[r == -1] <- 0
    expected <- sin(r * pi / 180)
    expect_equal(eastness(r), expected)

    ds_asp$close()
})
