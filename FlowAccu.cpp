#include "FlowAccu.h"
#include "utils.h"
bool FlowAccu::Allocate()
{
	delete[] pData;
	try
	{
		pData=new int[width*height];
		Assign_NoData();
	}
	catch(const std::bad_alloc& e)
	{
		fprintf(stderr, "Failed to create memory for flow accumulation matrices!\n");
		return false;
	}
	return true;
}
void FlowAccu::freeMem()
{
	delete[] pData;
	pData = NULL;
}
void FlowAccu::initialElementsNodata()
{
	Assign_NoData();
}
int FlowAccu::asInt(unsigned int row, unsigned int col) const
{
	return pData[row*width+col];
}
void FlowAccu::Set_Value(unsigned int row, unsigned int col, int z)
{
	pData[row*width+col]=z;
}

bool FlowAccu::is_NoData(unsigned int row, unsigned int col) const
{
	if (pData[row*width+col] == -9999) return true;
	return false;
}
void FlowAccu::Assign_NoData()
{
	for (int i=0; i<width*height; i++)
		pData[i]=-9999;
}
unsigned int FlowAccu::Get_NY() const
{
	return height;
}
unsigned int FlowAccu::Get_NX() const
{
	return width;
}
int* FlowAccu::getData() const
{
	return pData;
}
void FlowAccu::SetHeight(unsigned int height) 
{
	this->height = height;
}
void FlowAccu::SetWidth(unsigned int width)
{
	this->width = width;
}


bool FlowAccu::is_InGrid(unsigned int row, unsigned int col) const
{
	if ((row >=0 && row < height) && (col >= 0 && col < width))
		return true;
	return false;
}
