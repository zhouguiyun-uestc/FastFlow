#ifndef UTILS_HEAD_H
#define UTILS_HEAD_H

#include "gdal_priv.h"
#include <queue>
#include <algorithm>
#include "DEM.h"
#include "FlowDirection.h"

void calculateStatistics(const DEM& dem, double* min, double* max, double* mean, double* stdDev);
void filleDEMNew(char* inputFile, char* outputFilledPath, char* outputDirPath, char* outputAccuPath);
FlowDirection* calculataNIPS(const FlowDirection& dirDEM);
int Get_rowTo(int dir, int row);
int Get_colTo(int dir, int col);
void setNoData(unsigned char* data, int length, unsigned char noDataValue);
void setNoData(float* data, int length, float noDataValue);
void setFlag(int index, unsigned char* flagArray);
bool isProcessed(int index, const unsigned char* flagArray);
bool  CreateGeoTIFF(char* path,int height, int width,void* pData, GDALDataType type, double* geoTransformArray6Eles,
					double* min, double* max, double* mean, double* stdDev, double nodatavalue);
bool readTIFF(const char* path, DEM& dem, double* geoTransformArray6Eles);
bool readTIFF(const char* path, GDALDataType type, FlowDirection& dem, double* geoTransformArray6Eles);
DEM* diff(DEM& demA, DEM& demB);
void CreateDiffImage(const char *demA, const char *demB, char *resultPath, GDALDataType type, double nodatavalue);
void calNips(FlowDirection& dirGrid, FlowDirection& nipsGrid);
bool int16ToFloat32(const short* fromGrid, DEM& toDem, unsigned int width, unsigned int height);
bool int32ToFloat32(const int* fromGrid, DEM& toDem, unsigned int width, unsigned int height);
void changeNoDataValue(DEM& toDem, unsigned int width, unsigned int height, float originNoDataValue, float toNoDataValue);
extern const unsigned char value[8];
class Flag
{
public:
	int width, height;
	unsigned char* flagArray;
public:
	~Flag()
	{
		Free();
	}
	bool Init(int width,int height)
	{
		flagArray=NULL;
		this->width=width;
		this->height=height;
		flagArray = new unsigned char[(width * height + 7) / 8]();
		return flagArray!=NULL;
	}
	void Free()
	{
		delete[] flagArray;
		flagArray=NULL;
	}
	void SetFlag(int row, int col)
	{
		int index=row*width+col;
		flagArray[index / 8] |= value[index % 8];
	}
	bool IsProcessed(int row, int col)
	{
		if (row<0 ||row>=height || col<0|| col>=width) return true;
		int index=row*width+col;
		return flagArray[index / 8]&value[index % 8];
	}
	bool IsProcessedDirect(int row, int col)
	{
		int index=row*width+col;
		return flagArray[index / 8]&value[index % 8];
	}
};

#endif
