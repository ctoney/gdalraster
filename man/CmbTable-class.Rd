% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/cmb_table.R
\name{CmbTable-class}
\alias{CmbTable-class}
\alias{Rcpp_CmbTable}
\alias{Rcpp_CmbTable-class}
\alias{CmbTable}
\title{Class for counting unique combinations of integers}
\arguments{
\item{keyLen}{The number of integer values comprising each combination.}

\item{varNames}{Optional character vector of names for the variables in the
combination.}
}
\value{
An object of class \code{CmbTable}. Contains a hash table having a
vector of \code{keyLen} integers as the key, and the count of occurrences of each
unique integer combination as the value. Class methods that operate on the
hash table are described in Details.
}
\description{
\code{CmbTable} implements a hash table having a vector of integers as the key,
and the count of occurrences of each unique integer combination as the
value. A unique ID is assigned to each unique combination of input values.

\code{CmbTable} is a C++ class exposed directly to R (via \code{RCPP_EXPOSED_CLASS}).
Methods of the class are accessed using the \code{$} operator. \strong{Note that all
arguments to class methods are required and must be given in the
order documented.} Naming the arguments is optional but may be preferred
for readability.
}
\section{Usage (see Details)}{

\preformatted{
## Constructors
cmb <- new(CmbTable, keyLen)
# or, giving the variable names:
cmb <- new(CmbTable, keyLen, varNames)

## Methods
cmb$update(int_cmb, incr)
cmb$updateFromMatrix(int_cmbs, incr)
cmb$updateFromMatrixByRow(int_cmbs, incr)
cmb$asDataFrame()
cmb$asMatrix()
}
}

\section{Details}{

\subsection{Constructors}{

\code{new(CmbTable, keyLen)}\cr
Default variable names will be assigned as \code{V1}, \code{V2}, ....
Returns an object of class \code{CmbTable}.

\code{new(CmbTable, keyLen, varNames)}\cr
Alternate constructor to specify variable names.
Returns an object of class \code{CmbTable}.
}

\subsection{Methods}{

\code{$update(int_cmb, incr)}\cr
Updates the hash table for the integer combination in the numeric vector
\code{int_cmb} (coerced to integer by truncation).
If this combination exists in the table, its count will be
incremented by \code{incr}. If the combination is not found in the table,
it will be inserted with count set to \code{incr}.
Returns the unique ID assigned to this combination.
Combination IDs are sequential integers starting at \code{1}.

\code{$updateFromMatrix(int_cmbs, incr)}\cr
This method is the same as \verb{$update()} but for a numeric matrix of
integer combinations \code{int_cmbs} (coerced to integer by truncation).
The matrix is arranged with each column vector forming an integer
combination. For example, the rows of the matrix could be
one row each from a set of \code{keyLen} rasters all read at the
same extent and pixel resolution (i.e., row-by-row raster overlay).
The method calls \verb{$update()} on each combination (each column of
\code{int_cmbs}), incrementing count by \code{incr} for existing
combinations, or inserting new combinations with count set to \code{incr}.
Returns a numeric vector of length \code{ncol(int_cmbs)} containing the
IDs assigned to the combinations.

\code{$updateFromMatrixByRow(int_cmbs, incr)}\cr
This method is the same as \verb{$updateFromMatrix()} above except the
integer combinations are in rows of the matrix \code{int_cmbs} (columns
are the variables).
The method calls \verb{$update()} on each combination (each row of
\code{int_cmbs}), incrementing count by \code{incr} for existing
combinations, or inserting new combinations with count set to \code{incr}.
Returns a numeric vector of length \code{nrow(int_cmbs)} containing the
IDs assigned to the combinations.

\code{$asDataFrame()}\cr
Returns the \code{CmbTable} as a data frame with column \code{cmbid} containing
the unique combination IDs, column \code{count} containing the counts of
occurrences, and \code{keyLen} columns (with names from \code{varNames}) containing
the integer values comprising each unique combination.

\code{$asMatrix()}\cr
Returns the \code{CmbTable} as a matrix with column \code{1} (\code{cmbid})
containing the unique combination IDs, column \code{2} (\code{count})
containing the counts of occurrences, and columns \code{3:keyLen+2}
(with names from \code{varNames}) containing the integer values comprising each
unique combination.
}
}

\examples{
m <- matrix(c(1,2,3,1,2,3,4,5,6,1,3,2,4,5,6,1,1,1), 3, 6, byrow = FALSE)
rownames(m) <- c("layer1", "layer2", "layer3")
print(m)

cmb <- new(CmbTable, 3, rownames(m))
cmb

cmb$updateFromMatrix(m, 1)
cmb$asDataFrame()

cmb$update(c(4,5,6), 1)
cmb$update(c(1,3,5), 1)
cmb$asDataFrame()

# same as above but matrix arranged with integer combinations in the rows
m <- matrix(c(1,2,3,1,2,3,4,5,6,1,3,2,4,5,6,1,1,1), 6, 3, byrow = TRUE)
colnames(m) <- c("V1", "V2", "V3")
print(m)

cmb <- new(CmbTable, 3)
cmb$updateFromMatrixByRow(m, 1)
cmb$asDataFrame()

cmb$update(c(4,5,6), 1)
cmb$update(c(1,3,5), 1)
cmb$asDataFrame()
}
