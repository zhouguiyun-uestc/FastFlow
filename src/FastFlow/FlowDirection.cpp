#include "stdafx.h"
#include "FlowDirection.h"
#include "utils.h"
bool FlowDirection::Allocate()
{
	delete[] pDir;
	pDir=new BYTE[width*height];
	if (pDir == NULL)
	{
		return false;
	}
	else
	{
		setNoData(pDir, width * height, NODATA);
		return true;
	}
} 
void FlowDirection::freeMem()
{
	delete[] pDir;
	pDir = NULL;
}
BYTE FlowDirection::asByte(int row, int col) const
{
	return pDir[row*width+col];
}
void FlowDirection::Set_Value(int row,int col, BYTE z)
{
	pDir[row*width+col]=z;
}

bool FlowDirection::is_NoData(int row, int col) const
{
	if (pDir[row*width+col] == NODATA) return true;
	return false;
}
void FlowDirection::Assign_NoData()
{
	for (int i = 0; i < width*height; i++)
		pDir[i] = NODATA;
}
int FlowDirection::Get_NY() const
{
	return height;
}
int FlowDirection::Get_NX() const
{
	return width;
}
BYTE* FlowDirection::getData() const
{
	return pDir;
}
void FlowDirection::SetHeight(int height) 
{
	this->height = height;
}
void FlowDirection::SetWidth(int width)
{
	this->width = width;
}
		
bool FlowDirection::is_InGrid(int row, int col) const
{
	if ((row >=0 && row < height) && (col >= 0 && col < width))
	{
		return true;
	}
	else
	{
		return false;
	}
}

float FlowDirection::getLength(unsigned int dir)
{
	if ((dir & 0x1) == 1)
	{
		return 1.41421f;
	}
	else return 1.0f;
}

bool FlowDirection::nextCell(unsigned int& row, unsigned int& col, BYTE dir) const
{
	static int indexDir[] = {1, 2, 4, 8, 16, 32, 64, 128};

	int iDir = -1;
	for (int i = 0; i < 8; i++)
	{
		if (indexDir[i] == dir)
		{
			iDir = i;
			break;
		}
	}
	if (iDir == -1) return false;
	int iRow = Get_rowTo(iDir, row);
	int iCol = Get_colTo(iDir, col);
	if (is_InGrid(iRow, iCol) && !is_NoData(iRow, iCol))
	{
		row = iRow;
		col = iCol;
		return true;
	}
	else
	{
		return false;
	}
}