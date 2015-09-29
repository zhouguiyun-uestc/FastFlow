#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <stack>
#include <algorithm>
#include "FlowAccu.h"
#include "DEM.h"
#include "Node.h"
#include "FlowDirection.h"
#include "utils.h"
#include <time.h>
#include <list>
#include <unordered_map>

using namespace std;
using std::cout;
using std::endl;
using std::string;
using std::getline;
using std::fstream;
using std::ifstream;
using std::priority_queue;
using std::binary_function;


typedef std::vector<Node> NodeVector;
typedef std::priority_queue<Node, NodeVector, Node::Greater> PriorityQueue;


void accuMethodByBTI(FlowDirection& dirGrid, FlowAccu& accuGrid, double* pTimeConsume);
void accuMethodByJiang(FlowDirection& dirGrid, FlowAccu& accuGrid, double* consumeTime);
void accuMethodByRecursive(FlowDirection& dirGrid, FlowAccu& accuGrid, double* pTimeConsume);
void accuMethodByWang(FlowDirection& dirGrid, FlowAccu& accuDem, double* pTimeConsume);
void accuMethodByZhou(FlowDirection& dirGrid, FlowAccu& accuGrid, double* consumeTime);

//Compute statistics for a DEM matrix
void calculateStatistics(const DEM& dem, double* min, double* max, double* mean, double* stdDev)
{
	int width = dem.Get_NX();
	int height = dem.Get_NY();

	int validElements = 0;
	double minValue, maxValue;
	double sum = 0.0;
	double sumSqurVal = 0.0;
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			if (!dem.is_NoData(row, col))
			{
				double value = dem.asFloat(row, col);
				//Initialize minValue and maxValue using the first valid value
				if (validElements == 0)
				{
					minValue = maxValue = value;
				}
				validElements++;
				if (minValue > value)
				{
					minValue = value;
				}
				if (maxValue < value)
				{
					maxValue = value;
				}

				sum += value;
				sumSqurVal += (value * value);
			}
		}
	}
	
	double meanValue = sum / validElements;
	double stdDevValue = sqrt((sumSqurVal / validElements) - (meanValue * meanValue));
	*min = minValue;
	*max = maxValue;
	*mean = meanValue;
	*stdDev = stdDevValue;
}

//Compute statistics for a FlowAccu matrix
void calculateStatistics(const FlowAccu& dem, double* min, double* max, double* mean, double* stdDev)
{
	int width = dem.Get_NX();
	int height = dem.Get_NY();

	int validElements = 0;
	double minValue, maxValue;
	double sum = 0.0;
	double sumSqurVal = 0.0;
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			if (!dem.is_NoData(row, col))
			{
				double value = dem.asInt(row, col);
				//Initialize minValue and maxValue using the first valid value
				if (validElements == 0)
				{
					minValue = maxValue = value;
				}
				validElements++;
				if (minValue > value)
				{
					minValue = value;
				}
				if (maxValue < value)
				{
					maxValue = value;
				}

				sum += value;
				sumSqurVal += (value * value);
			}
		}
	}

	double meanValue = sum / validElements;
	double stdDevValue = sqrt((sumSqurVal / validElements) - (meanValue * meanValue));
	*min = minValue;
	*max = maxValue;
	*mean = meanValue;
	*stdDev = stdDevValue;
}
//Compute statistics for a FlowDirection matrix
void calculateStatistics(const FlowDirection& dem, double* min, double* max, double* mean, double* stdDev)
{
	int width = dem.Get_NX();
	int height = dem.Get_NY();

	int validElements = 0;
	double minValue, maxValue;
	double sum = 0.0;
	double sumSqurVal = 0.0;
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			if (!dem.is_NoData(row, col))
			{
				double value = dem.asByte(row, col);
				//Initialize minValue and maxValue using the first valid value
				if (validElements == 0)
				{
					minValue = maxValue = value;
				}
				validElements++;
				if (minValue > value)
				{
					minValue = value;
				}
				if (maxValue < value)
				{
					maxValue = value;
				}

				sum += value;
				sumSqurVal += (value * value);
			}
		}
	}

	double meanValue = sum / validElements;
	double stdDevValue = sqrt((sumSqurVal / validElements) - (meanValue * meanValue));
	*min = minValue;
	*max = maxValue;
	*mean = meanValue;
	*stdDev = stdDevValue;
}

//Fill depresspions and calculate flow directions from input DEM using the algorithm in Wang and Liu (2006)
void FillDEMAndComputeFlowDirection(char* inputFile, char* outputDirPath)
{
	DEM dem;
	double geoTransformArgs[6];
	if (!readTIFF(inputFile, dem, geoTransformArgs))
	{
		cout<<"Error occured when reading DEM!"<<endl;
		return;
	}
	
	int width = dem.Get_NX();
	int height = dem.Get_NY();
	cout<<"DEM width:"<<width<<"\theight£º"<<height<<endl;
	
	//Prepare a flag array of Boolean values
	int nums = (width * height + 7) / 8;
	unsigned char* flagArray = new unsigned char[nums]();

	//Flow direction matrix
	FlowDirection dirDEM;
	dirDEM.SetHeight(height);
	dirDEM.SetWidth(width);
	if(dirDEM.Allocate() == false)
	{
		cout<<"Failed to allocate memory for the output FlowDirection matrix"<<endl;
		return;
	}
	
	PriorityQueue queue; 
	int validElementsCount = 0;
	cout<<"Using the method in Wang&Liu(2006) to fill depressions...\n";
	//Push all border cells into the queue
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			Node tmpNode;
			if (!dem.is_NoData(row, col))
			{
				validElementsCount++;
				for (int i = 0; i < 8; i++)
				{
					int iRow, iCol;
					iRow = Get_rowTo(i, row);
					iCol = Get_colTo(i, col);

					if (!dem.is_InGrid(iRow, iCol) || dem.is_NoData(iRow, iCol))
					{
						tmpNode.col = col;
						tmpNode.row = row;
						tmpNode.spill = dem.asFloat(row, col);
						queue.push(tmpNode);
						setFlag(row * width + col, flagArray);
						break;
					}
				}
			}
		}
	}

	int percentFive = validElementsCount / 20;

	cout<<"Start filling depressions and calculationg flow direciton matrix ..."<<endl;
	time_t timeStart, timeEnd;
	timeStart = time(NULL);

	int count = 0;
	while (!queue.empty())
	{
		count++;
		if (count % percentFive == 0)
		{
			int percentNum = count / percentFive;
			cout<<"Progress£º"<<percentNum * 5 <<"%\r";
		}
		Node tmpNode = queue.top();
		queue.pop();

		int row = tmpNode.row;
		int col = tmpNode.col;
		float spill = tmpNode.spill;

		for (int i = 0; i < 8; i++)
		{
			int iRow, iCol;
			iRow = Get_rowTo(i, row);
			iCol = Get_colTo(i, col);
			float iSpill;

			if (dem.is_InGrid(iRow, iCol) && !dem.is_NoData(iRow, iCol))
			{
				if (!isProcessed(iRow * width + iCol, flagArray))
				{
					iSpill = dem.asFloat(iRow, iCol);
					// a depression or a flat region is encountered
					if (iSpill <= spill)
					{
						dem.Set_Value(iRow, iCol, spill);
						setFlag(iRow * width + iCol, flagArray);
						tmpNode.row = iRow;
						tmpNode.col = iCol;
						tmpNode.spill = spill;

						//set the flow direction as the reverse of the search direction
						dirDEM.Set_Value(iRow, iCol, inverse[i]);
					}
					else
					{
						dem.Set_Value(iRow, iCol, iSpill);
						setFlag(iRow * width + iCol, flagArray);
						tmpNode.row = iRow;
						tmpNode.col = iCol;
						tmpNode.spill = iSpill;
					}
					queue.push(tmpNode);
				}
			}
		}
		//if the flow direction of the cell is unspecified, use D8 rule to assign the direction
		if (dirDEM.is_NoData(row, col))
		{
			unsigned char dir = dem.computeDirection(row, col, spill);
			dirDEM.Set_Value(row, col, dir);
		}

	}
	cout<<"Finish filling depressions and calculating flow direciton matrix."<<endl;


	timeEnd = time(NULL);
	double consumeTime = difftime(timeEnd, timeStart);
	cout<<"Times used: "<<consumeTime<<" seconds"<<endl;

	cout<<"Writing flow direction matrix ..."<<endl;
	double min, max, mean, stdDev;
	delete[] flagArray;
	calculateStatistics(dirDEM, &min, &max, &mean, &stdDev);
	CreateGeoTIFF(outputDirPath, dirDEM.Get_NY(), dirDEM.Get_NX(), 
		(void *)dirDEM.getData(),GDALDataType::GDT_Byte, geoTransformArgs,
		&min, &max, &mean, &stdDev, 0);
	cout<<"Flow direction grid created!"<<endl;
}





int main(int argc, char* argv[])
{
	FlowDirection dirGrid;
	FlowAccu accuGrid;

    if (argc < 4)
	{
		cout<<"FastFlow is a softwared tool that implements different algorithms for calculating flow accumulation matrices from flow direction matrices."<<endl;
		cout<<"FastFlow also calculates flow direction matrices from unfilled raw DEMs."<<endl;
		cout<<"FastFlow supports GeoTIFF file format through thd GDAL library."<<endl;
		cout<<"\n Usage: FastFlow [method] inputFile outputFile."<<endl;
		cout<<"\n [method] can be 'flowdirection', 'Wang','Jiang','BTI','Recursive','Zhou'"<<endl;

		cout<<"\nExample Usage1: FastFlow flowdirection dem.tif flowdir.tif. \n\tCreate the flow direction GeoTIFF file from raw dem.tif file. The flow direction is coded in the same way as ArcGIS. \n\tPlease see http://help.arcgis.com/en/arcgisdesktop/10.0/help/index.html#//009z00000063000000.htm"<<endl;
		cout<<"\nExample Usage2: FastFlow Zhou flowdir.tif flowaccu.tif. \n\tCreate the flow accumulation GeoTIFF file from flow direction file flowdir.tif using Zhou's method"<<endl;
		return 1;
	}

	char* algName = argv[1];
	char* inputPath = argv[2];
	char* outputPath = argv[3];
	double  consumeTime;
	double geoTransformArgs[6];
	double min, max, mean, stdDev;
	double noDataValue = -9999.0;

	if(strcmp(algName, "flowdirection") == 0)
	{
		FillDEMAndComputeFlowDirection(inputPath, outputPath);
	}
	else if(strcmp(algName, "Wang") == 0)
	{
		if(!readTIFF(inputPath, GDALDataType::GDT_Byte, dirGrid, geoTransformArgs))
		{
			printf("\nFailed to read flow direction GeoTIFF file! Make sure the input file is a flow direction file!\n");
			return -1;
		}
		accuGrid.SetWidth(dirGrid.Get_NX());
		accuGrid.SetHeight(dirGrid.Get_NY());
		if(!accuGrid.Allocate())
		{
			fprintf(stderr, "Failed to allocate memory for accumulation matrix£¡\n");
			return 0;
		}
		accuMethodByWang(dirGrid, accuGrid, &consumeTime);
		calculateStatistics(accuGrid, &min, &max, &mean, &stdDev);
		CreateGeoTIFF(outputPath, accuGrid.Get_NY(), accuGrid.Get_NX(), (void*)accuGrid.getData(), GDALDataType::GDT_Int32,
			geoTransformArgs, &min, &max, &mean, &stdDev, noDataValue);
	}
	else if(strcmp(algName, "Jiang") == 0)
	{
		if(!readTIFF(inputPath, GDALDataType::GDT_Byte, dirGrid, geoTransformArgs))
		{
			fprintf(stderr, "read direction tif failed!\n");
			return -1;
		}
		accuGrid.SetWidth(dirGrid.Get_NX());
		accuGrid.SetHeight(dirGrid.Get_NY());
		if(!accuGrid.Allocate())
		{
			fprintf(stderr, "Failed to allocate memory for accumulation matrix£¡\n");
			return 0;
		}
		accuMethodByJiang(dirGrid, accuGrid, &consumeTime);
		calculateStatistics(accuGrid, &min, &max, &mean, &stdDev);
		CreateGeoTIFF(outputPath, accuGrid.Get_NY(), accuGrid.Get_NX(), (void*)accuGrid.getData(), GDALDataType::GDT_Int32,
			geoTransformArgs, &min, &max, &mean, &stdDev, noDataValue);
	}
	else if(strcmp(algName, "BTI") == 0)
	{
		if(!readTIFF(inputPath, GDALDataType::GDT_Byte, dirGrid, geoTransformArgs))
		{
			fprintf(stderr, "read direction tif failed!\n");
			return -1;
		}
		accuGrid.SetWidth(dirGrid.Get_NX());
		accuGrid.SetHeight(dirGrid.Get_NY());
		if(!accuGrid.Allocate())
		{
			fprintf(stderr, "Failed to allocate memory for accumulation matrix£¡\n");
			return 0;
		}
		accuMethodByBTI(dirGrid, accuGrid, &consumeTime);
		calculateStatistics(accuGrid, &min, &max, &mean, &stdDev);
		CreateGeoTIFF(outputPath, accuGrid.Get_NY(), accuGrid.Get_NX(), (void*)accuGrid.getData(), GDALDataType::GDT_Int32,
			geoTransformArgs, &min, &max, &mean, &stdDev, noDataValue);
	}
	else if(strcmp(algName, "Recursive") == 0)
	{
		if(!readTIFF(inputPath, GDALDataType::GDT_Byte, dirGrid, geoTransformArgs))
		{
			fprintf(stderr, "read direction tif failed!\n");
			return -1;
		}
		accuGrid.SetWidth(dirGrid.Get_NX());
		accuGrid.SetHeight(dirGrid.Get_NY());
		if(!accuGrid.Allocate())
		{
			fprintf(stderr, "Failed to allocate memory for accumulation matrix£¡\n");
			return 0;
		}
		accuMethodByRecursive(dirGrid, accuGrid, &consumeTime);
		calculateStatistics(accuGrid, &min, &max, &mean, &stdDev);
		CreateGeoTIFF(outputPath, accuGrid.Get_NY(), accuGrid.Get_NX(), (void*)accuGrid.getData(), GDALDataType::GDT_Int32,
			geoTransformArgs, &min, &max, &mean, &stdDev, noDataValue);
	}
	else if(strcmp(algName, "Zhou") == 0)
	{
		if(!readTIFF(inputPath, GDALDataType::GDT_Byte, dirGrid, geoTransformArgs))
		{
			fprintf(stderr, "read direction tif failed!\n");
			return -1;
		}
		accuGrid.SetWidth(dirGrid.Get_NX());
		accuGrid.SetHeight(dirGrid.Get_NY());
		if(!accuGrid.Allocate())
		{
			fprintf(stderr, "Failed to allocate memory for accumulation matrix£¡\n");
			return 0;
		}
		accuMethodByZhou(dirGrid, accuGrid, &consumeTime);
		calculateStatistics(accuGrid, &min, &max, &mean, &stdDev);
		CreateGeoTIFF(outputPath, accuGrid.Get_NY(), accuGrid.Get_NX(), (void*)accuGrid.getData(), GDALDataType::GDT_Int32,
			geoTransformArgs, &min, &max, &mean, &stdDev, noDataValue);
	}
	else
	{
		cout<<"Unknown command type."<<endl;
		return -1;
	}
	return 0;
}

