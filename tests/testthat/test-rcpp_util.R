test_that("equal_within_ulps_ behaves as expected", {
    expect_true(.equal_within_ulps(0, 0));
    expect_false(.equal_within_ulps(0, 0.00001));
    expect_false(.equal_within_ulps(0.00001, 0));
    expect_true(.equal_within_ulps(1.0, 1.0));
    expect_false(.equal_within_ulps(1.0, 0.99999));

    expect_false(.equal_within_ulps(NaN, NaN))
    expect_false(.equal_within_ulps(1, NaN))
    expect_false(.equal_within_ulps(NaN, 1))

    expect_false(.equal_within_ulps(NA, NA))
    expect_false(.equal_within_ulps(1, NA))
    expect_false(.equal_within_ulps(NA, 1))

    expect_true(.equal_within_ulps(Inf, Inf))
    expect_true(.equal_within_ulps(-Inf, -Inf))

    expect_true(.equal_within_ulps(.Machine$double.xmax,
                                   .Machine$double.xmax))
    expect_true(.equal_within_ulps(.Machine$double.xmin,
                                   .Machine$double.xmin))
    expect_false(.equal_within_ulps(.Machine$double.xmax, Inf))

    skip_on_cran()
    expect_true(.equal_within_ulps(1.0, 0.99999999999999999));
})
