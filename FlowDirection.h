#ifndef FLOWDIRCTION_HEAD_H
#define FLOWDIRCTION_HEAD_H

#include <stdio.h>

class FlowDirection
{
protected:
	int width;
	int height;
	unsigned char* pDir;
	unsigned char noData = 0;
public:
	FlowDirection()
	{
		pDir = NULL;
	}
	~FlowDirection()
	{
		delete[] pDir;
	}
	int getWidth() const
	{
		return width;
	}
	int getHeight() const
	{
		return height;
	}

	bool Allocate();

	void freeMem();

	unsigned char asByte(int row, int col) const;

	void Set_Value(int row,int col, unsigned char z);
	void Set_NoData(double nodata);
	bool is_NoData(int row, int col) const;
	void Assign_NoData();
	int Get_NY() const;
	int Get_NX() const;
	unsigned char* getData() const;
	void SetHeight(int height) ;
	void SetWidth(int width);
	bool is_InGrid(int row, int col) const;
	float getLength(unsigned int dir);
	bool nextCell(unsigned int& row, unsigned int& col, unsigned char dir) const;
};
#endif
