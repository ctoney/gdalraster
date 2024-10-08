% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/geom.R
\name{g_summary}
\alias{g_summary}
\title{Obtain text summaries of WKB/WKT geometries}
\usage{
g_summary(geom, quiet = FALSE)
}
\arguments{
\item{geom}{Either a raw vector of WKB or list of raw vectors, or a
character vector containing one or more WKT strings.}

\item{quiet}{Logical, \code{TRUE} to suppress warnings. Defaults to \code{FALSE}.}
}
\value{
character vector of the same length as the number of input
geometries in \code{geom}, containing summaries for the corresponding geometies.
}
\description{
\code{g_summary()} returns text summaries of WKB/WKT geometries.
Requires GDAL >= 3.7.
}
\examples{
# Requires GDAL >= 3.7
if (as.integer(gdal_version()[2]) >= 3070000) {
  f <- system.file("extdata/ynp_fires_1984_2022.gpkg", package = "gdalraster")
  lyr <- new(GDALVector, f, "mtbs_perims")

  feat <- lyr$getNextFeature()
  g_summary(feat$geom)

  feat_set <- lyr$fetch(5)
  g_summary(feat_set$geom)

  lyr$close()
}
}
