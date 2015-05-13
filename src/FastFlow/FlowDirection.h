#ifndef FLOWDIRCTION_HEAD_H
#define FLOWDIRCTION_HEAD_H
#include "stdafx.h"
#include <Windows.h>
static const BYTE NODATA = 0;

class FlowDirection
{
protected:
	int width;
	int height;
	BYTE* pDir;
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

	BYTE asByte(int row, int col) const;

	void Set_Value(int row,int col, BYTE z);
	bool is_NoData(int row, int col) const;
	void Assign_NoData();
	int Get_NY() const;
	int Get_NX() const;
	BYTE* getData() const;
	void SetHeight(int height) ;
	void SetWidth(int width);
	bool is_InGrid(int row, int col) const;
	float getLength(unsigned int dir);
	bool nextCell(unsigned int& row, unsigned int& col, BYTE dir) const;
};
#endif