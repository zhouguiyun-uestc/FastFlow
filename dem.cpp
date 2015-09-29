#include "DEM.h"
#include "utils.h"
bool DEM::Allocate()
{
	delete[] pDem;
	try
	{
		pDem=new float[width*height];
		setNoData(pDem, width * height, NO_DATA_VALUE);
	}
	catch(const std::bad_alloc& e)
	{
		fprintf(stderr, "Failed to create memory for the DEM!\n");
		return false;
	}
	return true;
}
void DEM::freeMem()
{
	delete[] pDem;
	pDem = NULL;
}
void DEM::initialElementsNodata()
{
	setNoData(pDem, width * height, NO_DATA_VALUE);
}
float DEM::asFloat(int row,int col) const
{
	return pDem[row*width+col];
}
void DEM::Set_Value(int row,int col, float z)
{
	pDem[row*width+col]=z;
}

bool DEM::is_NoData(int row, int col) const
{
	if (fabs(pDem[row*width+col]-NO_DATA_VALUE)<0.00001) return true;
	return false;
}
void DEM::Assign_NoData()
{
	for (int i=0; i<width*height; i++)
		pDem[i]=NO_DATA_VALUE;
}
int DEM::Get_NY() const
{
	return height;
}
int DEM::Get_NX() const
{
	return width;
}
float* DEM::getDEMdata() const
{
	return pDem;
}
void DEM::SetHeight(int height) 
{
	this->height = height;
}
void DEM::SetWidth(int width)
{
	this->width = width;
}

void DEM::readDEM(const std::string& filePath)
{
	std::ifstream is;
	is.open (filePath, std::ios::binary);
	is.read((char*)pDem,sizeof(float)*width*height);
	is.close();
}
bool DEM::is_InGrid(int row, int col) const
{
	if ((row >=0 && row < height) && (col >= 0 && col < width))
		return true;

	return false;
}

float DEM::getLength(unsigned int dir)
{
	if ((dir & 0x1) == 1)
	{
		return 1.41421f;
	}
	else return 1.0f;
}
/*
*	flow direction		
*	32	64	128		
*	16	0	1		
*	8	4	2		
*/
unsigned char DEM::computeDirection(int row, int col, float spill)
{

	int iRow, iCol;
	float iSpill, max, gradient;
	unsigned char steepestSpill;
	max = 0.0f;
	steepestSpill = 255;
	unsigned char lastIndexINGridNoData = 0;
	//using D8 method to calculate the flow direction
	for (int i = 0; i < 8; i++)
	{
		iRow = Get_rowTo(i, row);
		iCol = Get_colTo(i, col);

		if (is_InGrid(iRow, iCol) && !is_NoData(iRow, iCol) && (iSpill = asFloat(iRow, iCol)) < spill)
		{
			gradient = (spill - iSpill) / getLength(i);
			if (max < gradient)
			{
				max = gradient;
				steepestSpill = i;
			}
		}
		if (!is_InGrid(iRow, iCol) || is_NoData(iRow, iCol))
		{
			lastIndexINGridNoData = i;
		}

	}
	return steepestSpill != 255 ? dir[steepestSpill] : dir[lastIndexINGridNoData];
}
