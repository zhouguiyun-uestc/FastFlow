#include "FlowAccu.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <omp.h>
#include <vector>
#include <list>

void accuMethodByZhou(FlowDirection& dirGrid, FlowAccu& accuGrid, double* consumeTime)
{
	time_t startTime, endTime;
	unsigned int width = dirGrid.Get_NX();
	unsigned int height = dirGrid.Get_NY();
	fprintf(stdout, "\nUsing Zhou's algorithm to compute the flow accumulation matrix ... \n");
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

	//Initialize flow accumulation matrix with one
	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int col = 0; col < width; col++)
		{
			if (!dirGrid.is_NoData(row, col))
			{
				accuGrid.Set_Value(row, col, 1);
			}
		}
	} 
	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int col = 0; col < width; col++)
		{
			if (!dirGrid.is_NoData(row, col))
			{
				if (nipsGrid->asByte(row, col) != 0)
				{
					continue;
				}
				unsigned int iRow = row;
				unsigned int iCol = col;
				int nAccuCellNum = 0;
				do
				{
					int currentNodeAccu = accuGrid.asInt(iRow, iCol);
					currentNodeAccu += nAccuCellNum;
					accuGrid.Set_Value(iRow, iCol, currentNodeAccu);
					nAccuCellNum = currentNodeAccu;
					if (nipsGrid->asByte(iRow, iCol) >= 2)
					{
						nipsGrid->Set_Value(iRow, iCol, nipsGrid->asByte(iRow, iCol) - 1);
						break;
					}
				}
				while (dirGrid.nextCell(iRow, iCol, dirGrid.asByte(iRow, iCol)));
			}
		}
	}
	endTime = time(NULL);
	double timeConsume = difftime(endTime, startTime);
	std::cout<<"Time used"<<(int)timeConsume<<" seconds"<<std::endl;
	if(consumeTime) *consumeTime = timeConsume;
	delete nipsGrid;
}
