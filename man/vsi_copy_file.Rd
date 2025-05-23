% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{vsi_copy_file}
\alias{vsi_copy_file}
\title{Copy a source file to a target filename}
\usage{
vsi_copy_file(src_file, target_file, show_progress = FALSE)
}
\arguments{
\item{src_file}{Character string. Filename of the source file.}

\item{target_file}{Character string. Filename of the target file.}

\item{show_progress}{Logical scalar. If \code{TRUE}, a progress bar will be
displayed (the size of \code{src_file} will be retrieved in GDAL with
\code{VSIStatL()}). Default is \code{FALSE}.}
}
\value{
\code{0} on success or \code{-1} on an error.
}
\description{
\code{vsi_copy_file()} is a wrapper for \code{VSICopyFile()} in the GDAL Common
Portability Library. The GDAL VSI functions allow virtualization of disk
I/O so that non file data sources can be made to appear as files.
See \url{https://gdal.org/en/stable/user/virtual_file_systems.html}.
Requires GDAL >= 3.7.
}
\details{
The following copies are made fully on the target server, without local
download from source and upload to target:
\itemize{
\item /vsis3/ -> /vsis3/
\item /vsigs/ -> /vsigs/
\item /vsiaz/ -> /vsiaz/
\item /vsiadls/ -> /vsiadls/
\item any of the above or /vsicurl/ -> /vsiaz/ (starting with GDAL 3.8)
}
}
\note{
If \code{target_file} has the form /vsizip/foo.zip/bar, the default options
described for the function \code{addFilesInZip()} will be in effect.
}
\examples{
elev_file <- system.file("extdata/storml_elev.tif", package="gdalraster")
tmp_file <- "/vsimem/elev_temp.tif"

# Requires GDAL >= 3.7
if (gdal_version_num() >= gdal_compute_version(3, 7, 0)) {
  result <- vsi_copy_file(elev_file, tmp_file)
  (result == 0)
  print(vsi_stat(tmp_file, "size"))

  vsi_unlink(tmp_file)
}
}
\seealso{
\code{\link[=copyDatasetFiles]{copyDatasetFiles()}}, \code{\link[=vsi_stat]{vsi_stat()}}, \code{\link[=vsi_sync]{vsi_sync()}}
}
