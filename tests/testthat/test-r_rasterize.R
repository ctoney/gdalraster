test_that("rasterize_polygon identifies correct pixels", {
    # polygon with irregular lobes and one interior ring (hole)
    poly <- "POLYGON ((2 4, 2 15, 8 17, 11 9, 5 14, 6 7, 19 5, 16 17, 21 18, 22 1, 2 4), (19 10, 20 10, 20 14, 18 14, 19 10))"

    nodes <- vector("list", 20)
    burn_values <- c()
    attr_values <- c()
    accumulator <- function(yoff, xoff1, xoff2, burn_val, attr_val) {
        nodes[[yoff]] <<- c(nodes[[yoff]], xoff1, xoff2)
        burn_values <<- c(burn_values, rep(burn_val, xoff2 - xoff1 + 1))
        attr_values <<- c(attr_values, attr_val)
    }

    coords <- g_coords(poly)

    ret <- .rasterize_polygon(
        24, 20, tabulate(coords$ring_id), coords$x, coords$y, accumulator, 1)

    expect_equal(ret, 0)

    # row 0 has no intersections
    expect_equal(nodes[[1]], c(19, 21))
    expect_equal(nodes[[2]], c(12, 21))
    expect_equal(nodes[[3]], c(5, 21))
    expect_equal(nodes[[4]], c(2, 21))
    expect_equal(nodes[[5]], c(2, 15, 19, 21))
    expect_equal(nodes[[6]], c(2, 8, 19, 21))
    expect_equal(nodes[[7]], c(2, 5, 18, 21))
    expect_equal(nodes[[8]], c(2, 5, 18, 21))
    expect_equal(nodes[[9]], c(2, 5, 10, 10, 18, 21))
    expect_equal(nodes[[10]], c(2, 5, 9, 9, 18, 18, 20, 20))
    expect_equal(nodes[[11]], c(2, 4, 8, 9, 17, 18, 20, 20))
    expect_equal(nodes[[12]], c(2, 4, 7, 9, 17, 17, 20, 20))
    expect_equal(nodes[[13]], c(2, 4, 6, 8, 17, 17, 20, 20))
    expect_equal(nodes[[14]], c(2, 8, 17, 20))
    expect_equal(nodes[[15]], c(4, 8, 16, 20))
    expect_equal(nodes[[16]], c(7, 7, 16, 20))
    expect_equal(nodes[[17]], c(19, 20))
    expect_true(is.null(nodes[[18]]))
    expect_true(is.null(nodes[[19]]))
    expect_true(all(burn_values == 1))
    expect_equal(length(burn_values), 162)  # g_area(poly) = 162.5
    expect_true(all(is.na(attr_values)))
})
