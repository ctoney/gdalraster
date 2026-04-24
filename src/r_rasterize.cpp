//  Rasterize one polygon in R

/*
  `rasterize_polygon()` calls back to a user-defined raster I/O function for
  each contiguous segment of pixels in each row of a raster that intersects the
  polygon. A pixel is considered inside the polygon if its center falls inside
  (subject to numerical precision inaccuracies where polygon edges cross
  exactly the center of a pixel).

  The callback data give the raster row offset and start/end column offsets of
  a specific row segment of pixels within the polygon. The callback also passes
  through:
  * a numeric `burn_value`
  * an optional string `attr_value` that could be, e.g., the name of a vector
    attribute from which to obtain a burn value.

  Polygon vertices x/y must be given in raster pixel/line coordinate space.
  Polygons may be multi-part and/or have one or more interior rings, with
  `part_sizes` giving the number of vertices in each part/ring (i.e, sum of part
  sizes must equal the length of the polygon X/Y vectors).

  Note that the raster I/O callback function might only read pixel data, for
  applications such as zonal statistics, rather than actually writing an
  output raster. In that case, `burn_value` would be the zone identifier.

  Based on:

  "Efficient Polygon Fill Algorithm With C Code Sample"
  Copyright (c) 2007 Darel Rex Finley
  https://alienryderflex.com/polygon_fill/

  dllImageFilledPolygon() in alg/llrasterize.cpp
  Copyright (c) 2000, Frank Warmerdam <warmerdam@pobox.com>
  Copyright (c) 2011, Even Rouault <even dot rouault at spatialys.com>
  SPDX-License-Identifier: MIT
*/

#include <Rcpp.h>

#include <algorithm>
#include <vector>

//' Rasterize one polygon
//'
//' @noRd
// [[Rcpp::export(name = ".rasterize_polygon")]]
int rasterize_polygon(int rasterXsize, int rasterYsize,
                      const Rcpp::IntegerVector &part_sizes,
                      const Rcpp::NumericVector &polygonX,
                      const Rcpp::NumericVector &polygonY,
                      Rcpp::Function fnRasterIO,
                      double burn_value,
                      Rcpp::String attr_value = NA_STRING) {

    if ((polygonX.size() != polygonY.size()) ||
        Rcpp::sum(part_sizes) != polygonX.size()) {

        return 1;
    }

    int nCoords = polygonX.size();
    int nParts = part_sizes.size();

    int minY = static_cast<int>(Rcpp::min(polygonY));
    int maxY = static_cast<int>(Rcpp::max(polygonY));

    if (minY < 0)
        minY = 0;
    if (maxY >= rasterYsize)
        maxY = rasterYsize - 1;

    const int minX = 0;
    const int maxX = rasterXsize - 1;

    std::vector<int> nodeX(nCoords);

    for (int y = minY; y <= maxY; y++) {
        const double scanY = y + 0.5;
        std::fill(nodeX.begin(), nodeX.end(), -1);
        int nNodes = 0;
        int part_offset = 0;
        for (int part = 0; part < nParts; part++) {
            int j = part_offset + part_sizes[part] - 1;
            for (int i = part_offset; i < part_offset + part_sizes[part]; i++) {
                if ((polygonY[i] < scanY && polygonY[j] >= scanY) ||
                    (polygonY[j] < scanY && polygonY[i] >= scanY)) {

                    const double intersectX =
                        polygonX[i] + (scanY - polygonY[i]) / (polygonY[j] -
                        polygonY[i]) * (polygonX[j] - polygonX[i]);

                    nodeX[nNodes++] =
                        static_cast<int>(std::floor(intersectX + 0.5));
                }
                j = i;
            }
            part_offset += part_sizes[part];
        }

        std::sort(nodeX.begin(), nodeX.begin() + nNodes);

        for (int i = 0; i + 1 < nNodes; i += 2) {
            if (nodeX[i] > maxX)
                break;
            if (nodeX[i + 1] > minX) {
                if (nodeX[i] < minX)
                    nodeX[i] = minX;
                if (nodeX[i + 1] > maxX)
                    nodeX[i + 1] = maxX;
            }
            else {
                continue;
            }
            if (nodeX[i + 1] > nodeX[i]) {
                fnRasterIO(
                    y, nodeX[i], nodeX[i + 1] - 1, burn_value, attr_value);
            }
        }
    }

    return 0;
}
