#include "DEM.h"
#include "FlowDirection.h"
#include "FlowAccu.h"
#include <vector>
#include <time.h>
#include <iostream>
#include <queue>
#include "utils.h"

void accuMethodByWang(FlowDirection& dirGrid, FlowAccu& accuDem, double* pTimeConsume)
{
	time_t startTime, endTime;
	unsigned int width = dirGrid.Get_NX();
	unsigned int height = dirGrid.Get_NY();
	fprintf(stdout, "\nUsing Wang's algorithm to compute the flow accumulation matrix ... \n");
	startTime = time(NULL);

	height = dirGrid.getHeight();
	width = dirGrid.getWidth();
	//FlowDirection can also be used as the NIDP matrix
	FlowDirection* nipsGrid = new FlowDirection();
	nipsGrid->SetHeight(height);
	nipsGrid->SetWidth(width);
	if (!nipsGrid->Allocate())
	{
		std::cout<<"Failed to allocate memory for the  NIDP matrix!"<<std::endl;
		delete nipsGrid;
		return;
	}
	calNips(dirGrid, *nipsGrid);

	unsigned int nipsValueEqules0Nums = 0;
	unsigned int validNums = 0;
	std::queue<unsigned int> que;
	
	for(unsigned int row = 0; row < height; ++row)
	{
		for(unsigned int col = 0; col < width; ++col)
		{
			if(!dirGrid.is_NoData(row, col))
			{
				accuDem.Set_Value(row, col, 1);
				++validNums;
				if(nipsGrid->asByte(row, col) == 0)
				{
					++nipsValueEqules0Nums;
					que.push(width * row + col);
				}
			}
		}
	}	

	unsigned int curCellIndex = 0;
	while(que.size() != 0)
	{
		curCellIndex = que.front();
		que.pop();

		unsigned int curRow = curCellIndex / width;
		unsigned int curCol = curCellIndex % width;

		unsigned int nextRow = curRow;
		unsigned int nextCol = curCol;
		if(!dirGrid.nextCell(nextRow, nextCol, dirGrid.asByte(curRow, curCol))) continue;

		accuDem.Set_Value(nextRow, nextCol, accuDem.asInt(nextRow, nextCol) + accuDem.asInt(curRow, curCol));

		nipsGrid->Set_Value(nextRow, nextCol, nipsGrid->asByte(nextRow, nextCol) - 1);
		if(nipsGrid->asByte(nextRow, nextCol) == 0) que.push(width * nextRow + nextCol);
	}
	endTime = time(NULL);
	double timeConsume = difftime(endTime, startTime);
	std::cout<<"Time used:"<<(int)timeConsume<<" seconds"<<std::endl;
	if(pTimeConsume) *pTimeConsume = timeConsume;
	delete nipsGrid;
}


