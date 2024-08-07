% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/gdalvector.R
\name{GDALVector-class}
\alias{GDALVector-class}
\alias{Rcpp_GDALVector}
\alias{Rcpp_GDALVector-class}
\alias{GDALVector}
\title{Class encapsulating a vector layer in a GDAL dataset}
\arguments{
\item{dsn}{Character string containing the data source name (DSN), usually a
filename or database connection string.}

\item{layer}{Character string containing the name of a layer within the
data source. May also be given as an SQL SELECT statement to be executed
against the data source, defining a layer as the result set.}

\item{read_only}{Logical scalar. \code{TRUE} to open the layer read-only (the
default), or \code{FALSE} to open with write access.}

\item{open_options}{Optional character vector of \code{NAME=VALUE} pairs
specifying dataset open options.}

\item{spatial_filter}{Optional character string containing a geometry in
Well Known Text (WKT) format which represents a spatial filter.}

\item{dialect}{Optional character string to control the statement dialect
when SQL is used to define the layer. By default, the OGR SQL engine will
be used, except for RDBMS drivers that will use their dedicated SQL engine,
unless \code{"OGRSQL"} is explicitly passed as the dialect. The \code{"SQLITE"}
dialect can also be used.}
}
\value{
An object of class \code{GDALVector} which contains pointers to the
opened layer and the dataset that owns it, and methods that operate on
the layer as described in Details. \code{GDALVector} is a C++ class exposed
directly to R (via \code{RCPP_EXPOSED_CLASS}). Fields and methods of the class
are accessed using the \code{$} operator. Note that all arguments to exposed
class methods are required (but do not have to be named). The read/write
fields are per-object settings which can be changed as needed during the
lifetime of the object.
}
\description{
\code{GDALVector} provides an interface for accessing a vector layer in a GDAL
dataset and calling methods on the underlying \code{OGRLayer} object.
An object of class \code{GDALVector} persists an open connection to the dataset,
and exposes methods for retrieving layer information, setting attribute and
spatial filters, and reading/writing feature data.
See \url{https://gdal.org/api/index.html} for details of the GDAL
Vector API.

\strong{Class \code{GDALVector} is currently under development}. An initial
implementation supporting read access was added in gdalraster 1.11.1.9100.
A working document with draft specifications is available at:\cr
\url{https://usdaforestservice.github.io/gdalraster/articles/gdalvector-draft.html}\cr
and discussion thread/status updates at:\cr
\url{https://github.com/USDAForestService/gdalraster/issues/241}.
}
\section{Usage (see Details)}{

\preformatted{
## Constructors
# for single-layer file formats such as shapefile
lyr <- new(GDALVector, dsn)
# specifying the layer name, or SQL statement defining the layer
lyr <- new(GDALVector, dsn, layer)
# for update access
lyr <- new(GDALVector, dsn, layer, read_only = FALSE)
# using dataset open options
lyr <- new(GDALVector, dsn, layer, read_only, open_options)
# setting a spatial filter and/or specifying the SQL dialect
lyr <- new(GDALVector, dsn, layer, read_only, open_options, spatial_filter, dialect)

## Read-only fields
lyr$featureTemplate

## Read/write fields
lyr$defaultGeomFldName
lyr$returnGeomAs
lyr$wkbByteOrder

## Methods
lyr$open(read_only)
lyr$isOpen()
lyr$getDsn()
lyr$getFileList()
lyr$getDriverShortName()
lyr$getDriverLongName()

lyr$getName()
lyr$testCapability()
lyr$getFIDColumn()
lyr$getGeomType()
lyr$getGeometryColumn()
lyr$getSpatialRef()
lyr$bbox()
lyr$getLayerDefn()

lyr$setAttributeFilter(query)
lyr$getAttributeFilter()
lyr$setIgnoredFields(fields)

lyr$setSpatialFilter(wkt)
lyr$setSpatialFilterRect(bbox)
lyr$getSpatialFilter()
lyr$clearSpatialFilter()

lyr$getFeatureCount()
lyr$getNextFeature()
lyr$setNextByIndex(i)
lyr$getFeature(fid)
lyr$resetReading()
lyr$fetch(n)

lyr$deleteFeature(fid)

lyr$startTransaction(force)
lyr$commitTransaction()
lyr$rollbackTransaction()

lyr$close()
}
}

\section{Details}{

\subsection{Constructors}{

\code{new(GDALVector, dsn)}\cr
The first layer by index is assumed if the \code{layer} argument is omitted, so
this form of the constructor might be used for single-layer formats like
shapefile.

\code{new(GDALVector, dsn, layer)}\cr
Constructor specifying the name of a layer to open. The \code{layer} argument
may also be given as an SQL SELECT statement to define a layer as the result
set.

\code{new(GDALVector, dsn, layer, read_only)}\cr
Constructor specifying read/write access (\verb{read_only = \{TRUE|FALSE\})}.
The \code{layer} argument is required in this form of the constructor, but may be
given as empty string (\code{""}), in which case the first layer by index will be
assumed.

\code{new(GDALVector, dsn, layer, read_only, open_options)}\cr
Constructor specifying dataset open options as a character vector of
\code{NAME=VALUE} pairs.

\code{new(GDALVector, dsn, layer, read_only, open_options, spatial_filter, dialect))}\cr
Constructor to specify a spatial filter and/or SQL dialect. All arguments
are required in this form of the constructor, but \code{open_options} may be
\code{NULL}, and \code{spatial_filter} or \code{dialect} may be an empty string (\code{""}).
}

\subsection{Read-only fields}{

\code{$featureTemplate}\cr
A list of the attribute and geometry field names, with \code{NA} values equivalent
to OGR NULL values. The list elements are fully typed with the corresponding
missing value types assigned (\code{NA_integer_}, \code{NA_real_}, \code{NA_character_},
etc.). The \code{featureTemplate} is useful to initialize a new empty feature,
to which field and geometry values can be assigned, for use with the
\verb{$createFeature()} method (create and write a new feature within the layer).
Note that geometry fields are initialized as \code{character} type in the
template, but may be set either to a \code{character} string specifying a
geometry in WKT format, or to a \code{raw} vector containing a geometry as WKB.
}

\subsection{Read/write fields}{

\code{$defaultGeomFldName}\cr
Character string specifying a name to use for returned columns when the
geometry column name in the source layer is empty, like with shapefiles etc.
Defaults to \code{"geometry"}.

\code{$returnGeomAs}\cr
Character string specifying the return format of feature geometries.
Must be one of \code{WKT}, \code{WKT_ISO}, \code{WKB}, \code{WKB_ISO}, \code{TYPE_NAME} or
\code{NONE} (the default). Using \code{WKB}/\code{WKT} exports as 99-402 extended
dimension (Z) types for Point, LineString, Polygon, MultiPoint,
MultiLineString, MultiPolygon and GeometryCollection. For other geometry
types, it is equivalent to using \code{WKB_ISO}/\code{WKT_ISO}
(see \url{https://libgeos.org/specifications/wkb/}).

\code{$wkbByteOrder}\cr
Character string specifying the byte order for WKB geometries.
Must be either \code{LSB} (Least Significant Byte first, the default) or
\code{MSB} (Most Significant Byte first).
}

\subsection{Methods}{

\code{$open(read_only)}\cr
(Re-)opens the vector layer on the existing DSN. Use this method to
open a layer that has been closed using \code{$close()}. May be used to
re-open a layer with a different read/write access (\code{read_only} set to
\code{TRUE} or \code{FALSE}). The method will first close an open dataset, so it is
not required to call \code{$close()} explicitly in this case.
No return value, called for side effects.

\code{$isOpen()}\cr
Returns a \code{logical} scalar indicating whether the vector dataset is open.

\code{$getDsn()}\cr
Returns a character string containing the \code{dsn} associated with this
\code{GDALVector} object (\code{dsn} originally used to open the layer).

\code{$getFileList()}\cr
Returns a character vector of files believed to be part of the data source.
If it returns an empty string (\code{""}) it means there is believed to be no
local file system files associated with the dataset (e.g., a virtual file
system). The returned filenames will normally be relative or absolute
paths depending on the path used to originally open the dataset.

\code{$getDriverShortName()}\cr
Returns the short name of the vector format driver.

\code{$getDriverLongName()}\cr
Returns the long name of the vector format driver.

\code{$getName()}\cr
Returns the layer name.

\code{$testCapability()}\cr
Tests whether the layer supports named capabilities based on the current
read/write access. Returns a list of capabilities with values \code{TRUE} or
\code{FALSE}. The returned list contains the following named elements:
\code{RandomRead}, \code{SequentialWrite}, \code{RandomWrite}, \code{UpsertFeature},
\code{FastSpatialFilter}, \code{FastFeatureCount}, \code{FastGetExtent},
\code{FastSetNextByIndex}, \code{CreateField}, \code{CreateGeomField}, \code{DeleteField},
\code{ReorderFields}, \code{AlterFieldDefn}, \code{AlterGeomFieldDefn}, \code{DeleteFeature},
\code{StringsAsUTF8}, \code{Transactions}, \code{CurveGeometries}.
(See the GDAL documentation for
\href{https://gdal.org/api/vector_c_api.html#_CPPv420OGR_L_TestCapability9OGRLayerHPKc}{\code{OGR_L_TestCapability()}}.)

\code{$getFIDColumn()}\cr
Returns the name of the underlying database column being used as the FID
column, or empty string (\code{""}) if not supported.

\code{$getGeomType()}\cr
Returns the well known name of the layer geometry type as character string.
For layers with multiple geometry fields, this method only returns the
geometry type of the first geometry column. For other columns, use
\verb{$getLayerDefn()}. For layers without any geometry field, this method
returns \code{"NONE"}.

\code{$getGeometryColumn()}\cr
Returns he name of the underlying database column being used as the geometry
column, or an empty string (\code{""}) if not supported.
For layers with multiple geometry fields, this method only returns the
name of the first geometry column. For other columns, use \verb{$getLayerDefn()}.

\code{$getSpatialRef()}\cr
Returns a WKT string containing the spatial reference system for this layer,
or empty string (\code{""}) if no spatial reference exists.

\code{$bbox()}\cr
Returns a numeric vector of length four containing the bounding box
for this layer (xmin, ymin, xmax, ymax). Note that \code{bForce = true} is set in
the underlying API call to \code{OGR_L_GetExtent()}, so the entire layer may be
scanned to compute a minimum bounding rectangle (see \code{FastGetExtent} in the
list returned by \verb{$testCapability()}). Depending on the format driver, a
spatial filter may or may not be taken into account, so it is safer to call
\verb{$bbox()} without setting a spatial filter.

\code{$getLayerDefn()}\cr
Returns a list containing the OGR feature class definition for this layer
(a.k.a. layer definition). The list contains zero or more attribute field
definitions, along with one or more geometry field definitions.
See \link{ogr_define} for details of the field and feature class definitions.

\code{$setAttributeFilter(query)}\cr
Sets an attribute query string to be used when fetching features via the
\verb{$getNextFeature()} or \verb{$fetch()} methods.
Only features for which \code{query} evaluates as true will be returned.
The query string should be in the format of an SQL WHERE clause, described
in the \href{https://gdal.org/user/ogr_sql_dialect.html#where}{"WHERE"}
section of the OGR SQL dialect documentation (e.g.,
\code{"population > 1000000 and population < 5000000"}, where \code{population} is an
attribute in the layer).
In some cases (RDBMS backed drivers, SQLite, GeoPackage) the native
capabilities of the database may be used to to interpret the WHERE clause,
in which case the capabilities will be broader than those of OGR SQL.
Note that installing a query string will generally result in resetting the
current reading position (as with \verb{$resetReading()} described below).
The \code{query} parameter may be set to empty string (\code{""}) to clear the current
attribute filter.

\code{$getAttributeFilter()}\cr
Returns the attribute query string currently in use, or empty string (\code{""})
if an attribute filter is not set.

\code{$setIgnoredFields(fields)}\cr
Set which fields can be omitted when retrieving features from the layer.
The \code{fields} argument is a character vector of field names.
If the format driver supports this functionality (testable using
\verb{$testCapability()$IgnoreFields}), it will not fetch the specified fields
in subsequent calls to \verb{$getFeature()} / \verb{$getNextFeature()} / \verb{$fetch()},
and thus save some processing time and/or bandwidth. Besides field names of
the layer, the following special fields can be passed: \code{"OGR_GEOMETRY"} to
ignore geometry and \code{"OGR_STYLE"} to ignore layer style. By default, no
fields are ignored. Note that fields that are used in an attribute filter
should generally not be set as ignored fields, as most drivers (such as
those relying on the OGR SQL engine) will be unable to correctly evaluate
the attribute filter. No return value, called for side effects.

\code{$setSpatialFilter(wkt)}\cr
Sets a new spatial filter from a geometry in WKT format. This method sets
the geometry to be used as a spatial filter when fetching features via the
\verb{$getNextFeature()} or \verb{$fetch()} methods. Only features that geometrically
intersect the filter geometry will be returned. Currently this test may be
inaccurately implemented (depending on the vector format driver), but it is
guaranteed that all features whose envelope overlaps the envelope of the
spatial filter will be returned. This can result in more shapes being
returned that should strictly be the case.
\code{wkt} is a character string containing a WKT geometry in the same coordinate
system as the layer. An empty string (\code{""}) may be passed indicating that
the current spatial filter should be cleared, but no new one instituted.

\code{$setSpatialFilterRect(bbox)}\cr
Sets a new rectangular spatial filter. This method sets a rectangle to be
used as a spatial filter when fetching features via the \verb{$getNextFeature()}
or \verb{$fetch()} methods. Only features that geometrically intersect the given
rectangle will be returned.
\code{bbox} is a numeric vector of length four containing xmin, ymin, xmax, ymax
in the same coordinate system as the layer as a whole (as returned by
\verb{$getSpatialRef()}).

\code{$getSpatialFilter()}\cr
Returns the current spatial filter geometry as a WKT string, or empty string
(\code{""}) if a spatial filter is not set.

\code{$clearSpatialFilter()}\cr
Clears a spatial filter that was set with \verb{$setSpatialFilterRect()}.
No return value, called for that side effect.

\code{$getFeatureCount()}\cr
Returns the number of features in the layer. For dynamic databases the count
may not be exact. This method forces a count in the underlying API call
(i.e., \code{bForce = TRUE} in the call to \code{OGR_L_GetFeatureCount()}). Note that
some vector drivers will actually scan the entire layer once to count
features. The \code{FastFeatureCount} element in the list returned by
the \verb{$testCapability()} method can be checked if this might be a concern.
The number of features returned takes into account the spatial and/or
attribute filters. Some driver implementations of this method may alter the
read cursor of the layer.

\code{$getNextFeature()}\cr
Fetch the next available feature from this layer. Only features matching the
current spatial and/or attribute filter (if defined) will be returned.
This method implements sequential access to the features of a layer.
The \verb{$resetReading()} method can be used to start at the beginning again.
Returns a list with the unique feature identifier (FID), the attribute and
geometry field names, and their values. \code{NULL} is returned if no more
features are available.

\code{$setNextByIndex(i)}\cr
Moves the read cursor to the \code{i}th feature in the current result set
(with 0-based indexing).
This method allows positioning of a layer such that a call to
\verb{$getNextFeature()} or \code{fetch()} will read the requested feature(s), where
\code{i} is an absolute index into the current result set. So, setting \code{i = 3}
would mean the next feature read with \verb{$getNextFeature()} would have been
the 4th feature read if sequential reading took place from the beginning of
the layer, including accounting for spatial and attribute filters.
This method is not implemented efficiently by all vector format drivers. The
default implementation simply resets reading to the beginning and then calls
\code{GetNextFeature()} \code{i} times.
To determine if fast seeking is available on the current layer, check
the \code{FastSetNextByIndex} element in the list returned by the
\verb{$testCapability()} method. No return value, called for side effect.

\code{$getFeature(fid)}\cr
Returns a feature by its identifier. The value of \code{fid} must be a numeric
scalar, optionally carrying the \code{bit64::integer64} class attribute.
Success or failure of this operation is unaffected by any spatial or
attribute filters that may be in effect.
The \code{RandomRead} element in the list returned by \verb{$testCapability()} can
be checked to establish if this layer supports efficient random access
reading; however, the call should always work if the feature exists since a
fallback implementation just scans all the features in the layer looking for
the desired feature. Returns a list with the unique feature identifier (FID),
the attribute and geometry field names, and their values, or \code{NULL} on
failure. Note that sequential reads (with \verb{$getNextFeature()}) are generally
considered interrupted by a call to \verb{$getFeature()}.

\code{$resetReading()}\cr
Reset feature reading to start on the first feature. No return value, called
for that side effect.

\code{$fetch(n)}\cr
Fetches the next \code{n} features from the layer and returns them as a data
frame. This allows retrieving the entire set of features, one page of
features at a time, or the remaining features (from the current cursor
position). Returns a data frame with as many rows as features were fetched,
and as many columns as attribute plus geometry fields in the result set,
even if the result is a single value or has one or zero rows.

This method is an analog of
\href{https://dbi.r-dbi.org/reference/dbFetch.html}{\code{DBI::dbFetch()}}.

The \code{n} argument is the maximum number of features to retrieve per fetch
given as \code{integer} or \code{numeric} but assumed to be a whole number (will
be truncated). Use \code{n = -1} or \code{n = Inf} to retrieve all pending features
(resets reading to the first feature).
Otherwise, \verb{$fetch()} can be called multiple times to perform forward paging
from the current cursor position. Passing \code{n = NA} is also supported and
returns the remaining features.
Fetching zero features is possible to retrieve the structure of the feature
set as a data frame (columns fully typed).

OGR field types are returned as the following R types (\code{NA} for OGR NULL
values):
\itemize{
\item \code{OFTInteger}: \code{integer}
\item \code{OFTInteger} subtype \code{OFSTBoolean}: \code{logical}
\item \code{OFTIntegerList}: vector of \code{integer} (list column)
\item \code{OFTInteger64}: \code{bit64::integer64}
\item \code{OFTInteger64} subtype \code{OFSTBoolean}: \code{logical}
\item \code{OFTInteger64List}: vector of \code{bit64::integer64} (list column)
\item \code{OFTReal}: \code{numeric}
\item \code{OFTRealList}: vector of \code{numeric} (list column)
\item \code{OFTString}: \code{character} string
\item \code{OFTStringList}: vector of \code{character} strings (list column)
\item \code{OFTDate}: \code{Date}
\item \code{OFTDateTime}: \code{POSIXct} (millisecond accuracy and adjustment for time zone
flag if present)
\item \code{OFTBinary}: \code{raw} vector (list column, \code{NULL} entries for OGR NULL values)
}

Geomtries are not returned if the field \code{returnGeomAs} is set to \code{NONE}
(currently the default). Omitting the geometries may be beneficial for
performance and memory usage when access only to feature attributes is
needed. Geometries are returned as \code{raw} vectors in a data frame list column
when \code{returnGeomAs} is set to \code{WKB} or \code{WKB_ISO}. Otherwise, geometries are
returned as \code{character} strings when \code{returnGeomAs} is set to one of \code{WKT},
\code{WKT_ISO} or \code{TYPE_NAME}.

Note that \verb{$getFeatureCount()} is called internally when fetching the full
feature set or all remaining features (but not for a page of features).

\code{$deleteFeature(fid)}\cr
Deletes a feature from the layer. The feature with the indicated feature ID
is deleted from the layer if supported by the format driver. The value of
\code{fid} must be a numeric scalar, optionally carrying the \code{bit64::integer64}
class attribute (should be a whole number, will be truncated).
The \code{DeleteFeature} element in the list returned by \verb{$testCapability()} can
be checked to establish if this layer has delete feature capability. Returns
logical \code{TRUE} if the operation succeeds, or \code{FALSE} on failure.

\code{$startTransaction(force)}\cr
Creates a transaction if supported by the vector data source. The \code{force}
argument is a logical value. If \code{force = FALSE}, only "efficient"
transactions will be attempted. Some drivers may offer an emulation of
transactions, but sometimes with significant overhead, in which case the
user must explicitly allow for such an emulation by setting \code{force =TRUE}.
The function \code{ogr_ds_test_cap()} can be used to determine whether a vector
data source supports efficient or emulated transactions.

All changes done after the start of the transaction are definitely applied
in the data source if \verb{$commitTransaction()} is called. They can be canceled
by calling \code{rollbackTransaction()} instead.
Nested transactions are not supported. Transactions are implemented at the
dataset level, so multiple \code{GDALVector} objects using the same data source
should not have transactions active at the same time.

In case \verb{$startTransaction()} fails, neither \verb{$commitTransaction()} nor
\verb{$rollbackTransaction()} should be called.
If an error occurs after a successful \verb{$startTransaction()}, the whole
transaction may or may not be implicitly canceled, depending on the format
driver (e.g., the PostGIS driver will cancel it, SQLite/GPKG will not). In
any case, in the event of an error, an explicit call to
\code{rollbackTransaction()} should be done to keep things balanced.

Returns logical \code{TRUE} if the transaction is created, or \code{FALSE} on failure.

\code{$commitTransaction()}\cr
Commits a transaction if supported by the vector data source.
Returns a logical value, \code{TRUE} if the transaction is successfully committed.
Returns \code{FALSE} if no transaction is active, or the rollback fails, or if the
data source does not support transactions.
Depending on the format driver, this may or may not abort layer sequential
reading that may be active.

\code{$rollbackTransaction()}\cr
Rolls back a data source to its state before the start of the current
transaction, if transactions are supported by the data source.
Returns a logical value, \code{TRUE} if the transaction is successfully rolled
back. Returns \code{FALSE} if no transaction is active, or the rollback fails,
or if the data source does not support transactions.

\code{$close()}\cr
Closes the vector dataset (no return value, called for side effects).
Calling \code{$close()} results in proper cleanup, and flushing of any
pending writes.
The \code{GDALVector} object is still available after calling \code{$close()}.
The layer can be re-opened on the existing \code{dsn} with
\code{$open(read_only = {TRUE|FALSE})}.
}
}

\examples{
# MTBS fire perimeters in Yellowstone National Park 1984-2022
f <- system.file("extdata/ynp_fires_1984_2022.gpkg", package = "gdalraster")

# copy to a temporary file that is writeable
dsn <- file.path(tempdir(), basename(f))
file.copy(f, dsn)

lyr <- new(GDALVector, dsn, "mtbs_perims")

# object of class GDALVector
lyr
str(lyr)

# dataset info
lyr$getDriverShortName()
lyr$getDriverLongName()
lyr$getFileList()

# layer info
lyr$getName()
lyr$getGeomType()
lyr$getGeometryColumn()
lyr$getFIDColumn()
lyr$getSpatialRef()
lyr$bbox()

# layer capabilities
lyr$testCapability()

# re-open with write access
lyr$open(read_only = FALSE)
lyr$testCapability()$SequentialWrite
lyr$testCapability()$RandomWrite

# feature class definition - a list of field names and their definitions
defn <- lyr$getLayerDefn()
names(defn)
str(defn)

# default value of the read/write field 'returnGeomAs'
print(lyr$returnGeomAs)

lyr$getFeatureCount()

# sequential read cursor
feat <- lyr$getNextFeature()
# a list of field names and their values
str(feat)

# set an attribute filter
lyr$setAttributeFilter("ig_year = 2020")
lyr$getFeatureCount()

feat <- lyr$getNextFeature()
str(feat)

# NULL when no more features are available
feat <- lyr$getNextFeature()
str(feat)

# reset reading to the start and return geometries as WKT
lyr$resetReading()
lyr$returnGeomAs <- "WKT"
feat <- lyr$getNextFeature()
str(feat)

# clear the attribute filter
lyr$setAttributeFilter("")
lyr$getFeatureCount()

# set a spatial filter
# get the bounding box of the largest 1988 fire and use as spatial filter
# first set a temporary attribute filter to do the lookup
lyr$setAttributeFilter("ig_year = 1988 ORDER BY burn_bnd_ac DESC")
feat <- lyr$getNextFeature()
str(feat)

bbox <- bbox_from_wkt(feat$geom)
print(bbox)

# set spatial filter on the full layer
lyr$setAttributeFilter("")
lyr$setSpatialFilterRect(bbox)
lyr$getFeatureCount()

# fetch in chunks and return as data frame
d <- lyr$fetch(20)
str(d)

# the next chunk
d <- lyr$fetch(20)
nrow(d)

# no features remaining
d <- lyr$fetch(20)
nrow(d)
str(d) # 0-row data frame with columns typed

# fetch all pending features with geometries as WKB
lyr$returnGeomAs <- "WKB"
d <- lyr$fetch(-1)  # resets reading to the first feature
str(d)

# parse WKB using package wk
wk_obj <- wk::wkb(d$geom, crs = lyr$getSpatialRef())
plot(wk_obj)

lyr$clearSpatialFilter()
lyr$getFeatureCount()

lyr$close()
unlink(dsn)
}
\seealso{
\link{ogr_define}, \link{ogr_manage}, \code{\link[=ogr2ogr]{ogr2ogr()}}, \code{\link[=ogrinfo]{ogrinfo()}}

GDAL vector format descriptions:\cr
\url{https://gdal.org/drivers/vector/index.html}

GDAL-supported SQL dialects:\cr
\url{https://gdal.org/user/ogr_sql_sqlite_dialect.html})
}
