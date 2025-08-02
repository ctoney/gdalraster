#' @name GDALAlg-class
#'
#' @aliases
#' Rcpp_GDALAlg Rcpp_GDALAlg-class GDALAlg
#'
#' @title Class encapsulating a GDAL CLI algorithm
#'
#' @description
#' `GDALAlg` provides bindings to `GDALAlgorithm` and related classes
#' that implement the "gdal" command line interface (CLI) in the GDAL API.
#' An object of class `GDALAlg` represents an instance of a CLI algorithm with
#' methods to obtain algorithm information and argument information, run the
#' algorithm, and access its output. **Requires GDAL >= 3.11**.
#'
#' `GDALALg` is a C++ class exposed directly to R (via `RCPP_EXPOSED_CLASS`).
#' Fields and methods of the class are accessed using the `$` operator. **Note
#' that all arguments to class methods are required and must be given in the
#' order documented.**
#'
#' @param cmd A character string or character vector containing the path to the
#' algorithm, e.g., `"raster info"` or `c("raster", "info")`.
#' @param args Either a character vector or a named list containing input
#' arguments of the algorithm (see Details).
#' @returns An object of class `GDALAlg`, which contains a pointer to the
#' algorithm instance. Class methods are described in Details, along with a set
#' of writable fields for per-object settings. Values may be assigned to the
#' class fields by regular \code{<-} or `=` assignment.
#'
#' @section Usage (see Details):
#' \preformatted{
#' ## Constructors
#' alg <- new(GDALAlg, cmd)
#' # or, with arguments
#' alg <- new(GDALAlg, cmd, args)
#'
#' ## Read/write fields (per-object settings)
#' alg$outputLayerNameForOpen
#' alg$quiet
#'
#' ## Methods
#' alg$info()
#' alg$argInfo(arg_name)
#' alg$usageAsJSON()
#'
#' alg$parseCommandLineArgs()
#' alg$run()
#' alg$output()
#' alg$outputs()
#'
#' alg$close()
#' alg$release()
#' }
#' @section Details:
#' ## Constructors
#'
#' \code{new(GDALAlg, cmd)}\cr
#' Instantiate an algorithm without specifying input arguments.
#'
#' \code{new(GDALAlg, cmd, args)}\cr
#' Instantiate an algorithm giving input arguments as a character vector or
#' named list. *** TODO: add syntax info ***
#'
#' ## Read/write fields (per-object settings)
#'
#' \code{$outputLayerNameForOpen}\cr
#' A character string specifying a layer name to open when obtaining algorithm
#' output as an object of class `GDALVector`. See method \code{$output()} below.
#' The default value is empty string (`""`) in which case the first layer by
#' index is opened. Ignored if output is not a vector dataset.
#'
#' \code{$quiet}\cr
#' A logical value, `FALSE` by default. Set to `TRUE` to suppress progress
#' reporting along with various messages and warnings.
#'
#' ## Methods
#'
#' \code{$info()}\cr
#' Returns a named list of algorithm information with the following elements:
#' * `name`: character string, the algorithm name
#' * `description`: character string, the algorithm (short) description
#' * `long_description`: character string, the algorithm longer description
#' * `URL`: character string, the algorithm help URL
#' * `has_subalgorithms`: logical, `TRUE` if the algorithm has sub-algorithms
#' * `subalgorithm_names`: character vector of sub-algorithm names (may be
#' empty)
#' * `arg_names`: character vector of available argument names
#'
#' \code{$argInfo(arg_name)}\cr
#' Returns a named list of information for an algorithm argument given as a
#' character string, with the following elements:
#' * `name`: character string, the name of the argument
#' * `type`: character string, the argument type as one of `"BOOLEAN"`,
#' `"STRING"`, `"INTEGER"`, `"REAL"`, `"DATASET"`, `"STRING_LIST"`,
#' `"INTEGER_LIST"`, `"REAL_LIST"`, `"DATASET_LIST"`
#' * `description`: character string, the argument description
#' * `short_name`: character string, the short name or empty string if there
#' is none
#' * `aliases`: character vector of aliases (empty if none)
#' * `meta_var`: character string, the "meta-var" hint (by default, the
#' meta-var value is the long name of the argument in upper case)
#' * `category`: character string, the argument category
#' * `is_positional`: logical, `TRUE` if the argument is a positional one
#' * `is_required`: logical, `TRUE` if the argument is required
#' * `min_count`: integer, the minimum number of values for the argument (only
#' applies to list type of arguments)
#' * `max_count`: integer, the maximum number of values for the argument (only
#' applies to list type of arguments)
#' * `packed_values_allowed`: logical, `TRUE` if, for list type of arguments,
#' several comma-separated values may be specified (i.e., `"--foo=bar,baz"`)
#' * `repeated_arg_allowed`: logical, `TRUE` if, for list type of arguments,
#' the argument may be repeated (i.e., `c("--foo=bar", "--foo=baz")`)
#' * `choices`: character vector of allowed values for the argument (may be
#' empty and only applies for argument types `"STRING"` and `"STRING_LIST"`)
#' * `is_explicitly_set`: logical, `TRUE` if the argument value has been
#' explicitly set
#' * `has_default_value`: logical, `TRUE` if the argument has a declared
#' default value
#' * `is_hidden_for_cli`: logical, `TRUE` if the argument must not be
#' mentioned in CLI usage (e.g., "output-value" for "gdal raster info", which
#' is only meant when the algorithm is used from a non-CLI context such as
#' programmatically from R)
#' * `is_only_for_cli`: logical, `TRUE` if the argument is only for CLI usage
#' (e.g., "--help")
#' * `is_input`: logical, `TRUE` if the value of the argument is read-only
#' during the execution of the algorithm
#' * `is_output`: logical, `TRUE` if (at least part of) the value of the
#' argument is set during the execution of the algorithm
#' * `dataset_type_flags`: character vector containing strings `"RASTER"`,
#' `"VECTOR"`, `"MULTIDIM_RASTER"`, possibly with `"UPDATE"` (`NULL` if
#' the argument is not a dataset type)
#' * `dataset_input_flags`: character vector indicating if a dataset argument
#' supports specifying only the dataset name (`"NAME"`), only the dataset
#' object (`"OBJECT"`), or both (`"NAME", "OBJECT"`) when it is used as an
#' input (`NULL` if the argument is not a dataset type)
#' * `dataset_output_flags`: character vector indicating if a dataset argument
#' supports specifying only the dataset name (`"NAME"`), only the dataset
#' object (`"OBJECT"`), or both (`"NAME", "OBJECT"`) when it is used as an
#' output (`NULL` if the argument is not a dataset type)
#' * `mutual_exclusion_group`: character string, the name of the mutual
#' exclusion group to which this argument belongs
#'
#' \code{$usageAsJSON()}\cr
#' Returns the usage of the algorithm as a JSON-serialized string.
#'
#' \code{$parseCommandLineArgs()}\cr
#' Sets the value of arguments previously specified in the class constructor,
#' and instantiates the actual algorithm that will be run (but without running
#' it). Returns a logical value, `TRUE` indicating success or `FALSE` if an
#' error occurs.
#'
#' \code{$run()}\cr
#' Executes the algorithm, first parsing arguments if
#' \code{$parseCommandLineArgs()} has not already been called explicitly.
#' Returns a logical value, `TRUE` indicating success or `FALSE` if an error
#' occurs.
#'
#' \code{$output()}\cr
#' Returns the single output value of the algorithm, after it has been run.
#' If there are multiple output values, this method will raise an error, and
#' the \code{$outputs()} (plural) method should be called instead. The type of
#' the return value corresponds to the type of the single output argument value
#' (see method \code{$argInfo()} above).
#' If the output argument has type `"DATASET"`, an object of class `GDALRaster`
#' will be returned if the dataset is raster, or an object of class
#' `GDALVector` if the dataset is vector. In the latter case, by default the
#' `GDALVector` object will be opened on the first layer by index, but a
#' specific layer name may be specified by setting the value of the field
#' \code{$outputLayerNameForOpen} before calling the \code{$output()} method
#' (see above).
#' Note that currently, if the output dataset is multidimensional raster, only
#' the dataset name will be returned as a character string.
#'
#' \code{$outputs()}\cr
#' Returns the output value(s) of the algorithm as a named list, after it has
#' been run. Most algorithms only return a single output, in which case the
#' \code{$output()} method (singular) is preferable for easier use. The element
#' names in the returned list are the names of the arguments that have outputs
#' (with any dash characters replaced by underscore), and the values are the
#' argument values which may include `GDALRaster` or `GDALVector` objects.
#'
#' \code{$close()}\cr
#' Completes any pending actions, and returns the final status as a logical
#' value (`TRUE` if no errors occur during the underlying call to
#' `GDALAlgorithmFinalize()`). This is typically useful for algorithms that
#' generate an output dataset. It closes datasets and gets back potential error
#' status resulting from that, e.g., if an error occurs during flushing to disk
#' of the output dataset after successful \code{$run()} execution.
#'
#' \code{$release()}\cr
#' Release memory associated with the algorithm, potentially after attempting
#' to finalize. No return value, called for side-effects.
#'
#' @seealso
#'
#' @examples
#'

NULL

Rcpp::loadModule("mod_GDALAlg", TRUE)
