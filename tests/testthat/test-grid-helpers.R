test_that("gt_from_dim_bbox produces correct geotransform", {
  # global extent
  
  gt <- gt_from_dim_bbox(c(360, 180), c(-180, -90, 180, 90))
  expect_equal(gt[1], -180)
  expect_equal(gt[2], 1)
  expect_equal(gt[3], 0)
  expect_equal(gt[4], 90)
  expect_equal(gt[5], 0)
  expect_equal(gt[6], -1)
  
  
  # arbitrary case
  gt <- gt_from_dim_bbox(c(143, 107), c(0, 1, 5, 10))
  expect_equal(gt[1], 0)
  expect_equal(gt[2], 5/143)
  expect_equal(gt[4], 10)
  expect_equal(gt[6], -9/107)
})

test_that("gt_from_dim_bbox rejects invalid bbox",
          {
            expect_error(gt_from_dim_bbox(c(100, 100), c(10, 0, 5, 10)),
                         "xmax must be greater than xmin")
            expect_error(gt_from_dim_bbox(c(100, 100), c(0, 10, 5, 5)),
                         "ymax must be greater than ymin")
          })

test_that("bbox_from_dim_gt produces correct bbox", {
  gt <- c(-180, 1, 0, 90, 0, -1)
  bbox <- bbox_from_dim_gt(c(360, 180), gt)
  expect_equal(bbox, c(-180, -90, 180, 90))
})

test_that("bbox_from_dim_gt is correct from rotated geotransform", {
  gt_rotated <- c(0, 1, 0.5, 0, 0.5, -1)
  expect_equal(bbox_from_dim_gt(c(100, 100), gt_rotated),
               c(0, -100,  150,   50))

})

test_that("gt and bbox round-trip correctly", {
  dim <- c(500, 300)
  bbox <- c(100, 200, 600, 800)
  
  gt <- gt_from_dim_bbox(dim, bbox)
  bbox_back <- bbox_from_dim_gt(dim, gt)
  expect_equal(bbox_back, bbox)
})


test_that("bbox_from_dim_gt and gt_from_dim_bbox correct and consistent", {
  elev_file <- system.file("extdata/storml_elev_orig.tif", package="gdalraster")
  ds <- new(GDALRaster, elev_file, read_only=TRUE)
  gt <- ds$getGeoTransform()
  bb <- ds$bbox()
  dm <- ds$dim()
  expect_error(ds$setBbox(c(0, 0, 1, 1)), "dataset is read-only")
  ds$close()
  expect_equal(bbox_from_dim_gt(dm, gt), bb)
  expect_equal(gt_from_dim_bbox(dm, bb), gt)
})

test_that("$setBbox() is correct and consistent", {
  lcp_file <- system.file("extdata/storm_lake.lcp", package="gdalraster")
  tif_file <- paste0(tempdir(), "/", "storml_lndscp.tif")
  createCopy(format="GTiff", dst_filename=tif_file, src_filename=lcp_file)
  ds <- new(GDALRaster, tif_file, read_only=FALSE)
  on.exit(deleteDataset(tif_file))
  expect_error(ds$setBbox(c(5, 0, 1, 1)), "invalid bbox: xmax must be greater than xmin")
  expect_error(ds$setBbox(c(0, 5, 1, 1)), "invalid bbox: ymax must be greater than ymin")
  expect_true(ds$setBbox(c(0, 0, 1, 1)))
  ds$close()
})
