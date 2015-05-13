#ifndef DEM_HEADER_H
#define DEM_HEADER_H

#include "stdafx.h"
#include <string>
#include <algorithm>
#include <fstream>
#include <queue>
#include <functional>
#include <Windows.h>

#define NO_DATA_VALUE -9999.0f

/*
*	reverse flow direction
*	2	4	8
*	1	0	16
*	128	64	32
*/
static BYTE inverse[8] = {16, 32, 64, 128, 1, 2, 4, 8};
/*
*	flow direction		
*	32	64	128		
*	16	0	1		
*	8	4	2		
*/
static BYTE	dir[8] = {1, 2, 4, 8, 16, 32, 64, 128};
class DEM
{
protected:
	float* pDem;
	int width, height;
public:
	DEM()
	{
		pDem=NULL;
	}
	~DEM()
	{
		delete[] pDem;
	}
	bool Allocate();

	void freeMem();

	void initialElementsNodata();
	float asFloat(int row,int col) const;
	void Set_Value(int row,int col, float z);
	bool is_NoData(int row, int col) const;
	void Assign_NoData();
	int Get_NY() const;
	int Get_NX() const;
	float* getDEMdata() const;

	void SetHeight(int height);
	void SetWidth(int width);
	void readDEM(const std::string& filePath);
	bool is_InGrid(int row, int col) const;
	float getLength(unsigned int dir);
	BYTE computeDirection(int row, int col, float spill);
};
#endif
