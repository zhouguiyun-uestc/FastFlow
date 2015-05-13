#pragma once
#include "stdafx.h"
#include <string>
#include <algorithm>
#include <fstream>
#include <queue>
#include <functional>
#include <Windows.h>
class FlowAccu
{
private:
	int* pData;
	unsigned int width, height;

public:
	FlowAccu()
	{
		pData = NULL;
	}
	~FlowAccu()
	{
		freeMem();
	}

	bool Allocate();

	void freeMem();

	void initialElementsNodata();
	int asInt(unsigned int row, unsigned int col) const;
	void Set_Value(unsigned int row, unsigned int col, int z);
	bool is_NoData(unsigned int row, unsigned int col) const;
	void Assign_NoData();
	unsigned int Get_NY() const;
	unsigned int Get_NX() const;
	int* getData() const;
	void SetHeight(unsigned int height);
	void SetWidth(unsigned int width);
	bool is_InGrid(unsigned int row, unsigned int col) const;
};

