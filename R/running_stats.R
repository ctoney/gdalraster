#' @name RunningStats-class
#'
#' @aliases
#' Rcpp_RunningStats Rcpp_RunningStats-class RunningStats
#'
#' @title Class to calculate mean and variance in one pass
#'
#' @description
#' `RunningStats` computes summary statistics on a data stream efficiently.
#' Mean and variance are calculated with Welford's online algorithm
#' (\url{https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance}).
#' The min, max, sum and count are also tracked. The input data values are not
#' stored in memory, so this class can be used to compute statistics for very
#' large data streams.
#'
#' `RunningStats` is a C++ class exposed directly to \R (via
#' `RCPP_EXPOSED_CLASS`). Fields and methods and of the class are accessed
#' using the `$` operator.
#'
#' @param na_rm Logical scalar. `TRUE` to remove `NA` from the input data (the
#' default) or `FALSE` to retain `NA`.
#' @returns An object of class `RunningStats`. A `RunningStats` object
#' maintains the current minimum, maximum, mean, variance, sum and count of
#' values that have been read from the stream. It can be updated repeatedly
#' with new values (i.e., chunks of data read from the input stream), but its
#' memory footprint is negligible. Class methods for updating with new values,
#' and retrieving the current values of statistics, are described in Details.
#'
#' @note
#' The intended use is computing summary statistics for specific subsets or
#' zones of a raster that could be defined in various ways and are generally
#' not contiguous. The algorithm as implemented here incurs the cost of
#' floating point division for each new value updated (i.e., per pixel), but is
#' reasonably efficient for the use case. Note that GDAL internally uses an
#' optimized version of Welford's algorithm to compute raster statistics as
#' described in detail by Rouault, 2016
#' (\url{https://github.com/OSGeo/gdal/blob/master/gcore/statistics.txt}).
#' The class method `GDALRaster$getStatistics()` is a GDAL API wrapper that
#' computes statistics for a whole raster band.
#'
#' @section Usage (see Details):
#' ```
#' ## Constructor
#' rs <- new(RunningStats, na_rm)
#'
#' ## Read/write fields (per-object settings)
#' rs$returnCountAsInteger64
#'
#' ## Methods
#' rs$update(newvalues)
#' rs$get_count()
#' rs$get_mean()
#' rs$get_min()
#' rs$get_max()
#' rs$get_sum()
#' rs$get_var()
#' rs$get_sd()
#' rs$reset()
#' ```
#'
#' @section Details:
#' ## Constructor
#'
#' \code{new(RunningStats, na_rm)}\cr
#' Returns an object of class \code{RunningStats}. The `na_rm` argument
#' defaults to `TRUE` if omitted.
#'
#' ## Read/write fields (per-object settings)
#'
#' \code{$returnCountAsInteger64}
#' A logical value specifying whether to return the count of values currently
#' in the data stream as `bit64::integer64` type. The default is `FALSE` in
#' which case the count is returned as R `numeric` (i.e., `double`). Can be set
#' to `TRUE` to support very large counts without loss of precision (returning
#' the internal `int64_t` counter without a cast to `double`).
#'
#' ## Methods
#'
#' \code{$update(newvalues)}\cr
#' Updates the `RunningStats` object with a numeric vector of `newvalues`
#' (i.e., a chunk of values from the data stream). No return value, called
#' for side effects.
#'
#' \code{$get_count()}\cr
#' Returns the count of values received from the data stream. Returns a
#' `numeric` value (i.e., `double`) unless `returnCountAsInteger64 = TRUE` in
#' which case the count is returned as `bit64::integer64` (see above).
#'
#' \code{$get_mean()}\cr
#' Returns the mean of values received from the data stream.
#'
#' \code{$get_min()}\cr
#' Returns the minimum value received from the data stream.
#'
#' \code{$get_max()}\cr
#' Returns the maximum value received from the data stream.
#'
#' \code{$get_sum()}\cr
#' Returns the sum of values received from the data stream.
#'
#' \code{$get_var()}\cr
#' Returns the variance of values from the data stream
#' (denominator n - 1).
#'
#' \code{$get_sd()}\cr
#' Returns the standard deviation of values from the data stream
#' (denominator n - 1).
#'
#' \code{$reset()}\cr
#' Clears the \code{RunningStats} object to its initialized state (count = 0).
#' No return value, called for side effects.
#'
#' @examples
#' (rs <- new(RunningStats, na_rm = TRUE))
#'
#' chunk <- runif(1000)
#' rs$update(chunk)
#' object.size(rs)
#'
#' rs$get_count()
#' length(chunk)
#'
#' rs$get_mean()
#' mean(chunk)
#'
#' rs$get_min()
#' min(chunk)
#'
#' rs$get_max()
#' max(chunk)
#'
#' rs$get_var()
#' var(chunk)
#'
#' rs$get_sd()
#' sd(chunk)
#'
#' # not needed to count this number of values, but for demonstration:
#' rs$returnCountAsInteger64 <- TRUE
#'
#' \donttest{
#' ## 10^9 values read in 10,000 chunks
#' ## should take under 1 minute on typical hardware
#' for (i in 1:1e4) {
#'   chunk <- runif(1e5)
#'   rs$update(chunk)
#' }
#' rs$get_count()
#' rs$get_mean()
#' rs$get_var()
#'
#' object.size(rs)
#' }
#'
#' ## large numbers with small differences
#' rs$reset()
#' rs$get_count()
#'
#' values <- runif(100000L, min = 100000000, max = 100000000.06)
#' rs$update(values)
#'
#' rs$get_count()
#'
#' rs$get_mean() |> format(nsmall = 3, scientific = FALSE)
#'
#' rs$get_var()
#' var(values)
NULL

Rcpp::loadModule("mod_running_stats", TRUE)
