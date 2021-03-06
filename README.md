# FastFlow
**Manuscript Title**: A fast and simple algorithm for calculating flow accumulation matrices from raster digital elevation models

This repository contains the implementation of the algorithms presented in the manuscript above. These implementations were used in performing the tests described in the manuscript.

There is source code for every pseudocode algorithm with O(N) time complexity presented in the manuscript. The codes can be compiled on both Windows and Linux platforms. 

FastFlow supports GeoTIFF file format through the GDAL library. GDAL 1.91 was used in our experiments.

To use this program, first create a flow direction GeoTIFF file from raw unfilled DEM GeoTIFF file.

**Example**: FastFlow flowdirection dem.tif flowdir.tif. 

"dem.tif" is the input DEM file and "flowdir.tif" is the output flow direction file. The flow direction is coded in the same way as descrbed at http://help.arcgis.com/en/arcgisdesktop/10.0/help/index.html#//009z00000063000000.htm

The algorithms described in the manuscript can be run using the following command line:

FastFlow AlgorithmName Input_Flow_Direction Output_Flow_Accumulation

**Example**: FastFlow Zhou flowdir.tif flowaccu.tif. Use the method proposed in the manuscript to calculate flow accumulation matrix.

The algorithms available are described briefly below and in greater detail in the manuscript.

**Wang's Algorithm (Wang)**: An NIDP-based algorithm using a plain queue to hold the cells without input cells.

**Jiang's Algorithm (Jiang)**: An NIDP-based algorithm without creating the NIDP matrix by reusing the output flow accumuation matrix to store the NIDP information.

**BTI-based Algorithm (BTI)**: An BTI-based algorithm to create the flow accumulation matrix.

**Recursive Algorithm (Recursive)**: Use a recursive procedure to calculate the flow accumulation matrix.

**Zhou's Algorithm (Zhou)**: Use the algorithm proposed in the manuscript to calculate the flow accumulation matrix.

<p> The test data used in the manuscript can be downloaded at http://www.mngeo.state.mn.us/. You need ArcGIS to convert the DEM into GeoTIFF format.
