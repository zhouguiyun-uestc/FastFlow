#include "DEM.h"
#include "FlowDirection.h"
#include "utils.h"
#include "FlowAccu.h"
#include <vector>
#include <time.h>
#include <iostream>
void accuMethodByBTI(FlowDirection& dirGrid, FlowAccu& accuGrid, double* pTimeConsume)
{
	fprintf(stdout, "\nUsing BTI-based algorithm to compute the flow accumulation matrix ... \n");

	time_t startTime, endTime;
	startTime = time(NULL);
	unsigned int width = dirGrid.getWidth();
	unsigned int height = dirGrid.getHeight();

	int indexAndDir[] = {16, 32, 64, 128, 1, 2, 4, 8};
	
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

	std::vector<std::pair<unsigned int, unsigned int>> accuOutCells;

	static const unsigned char inverseDir[8] = {16, 32, 64, 128, 1, 2, 4, 8};

	unsigned int iRow, iCol;
	bool isBoundaryCell;
	unsigned int tmpRow, tmpCol;
	unsigned int validNums = 0;

	for(unsigned int row = 0; row < height; ++row)
	{
		for(unsigned int col = 0; col < width; ++col)
		{
			if(dirGrid.is_NoData(row, col)) continue;

			++validNums;
			//Initialze the accumulation value with 0ne
			accuGrid.Set_Value(row, col, 1);
			
			//find border cells
			isBoundaryCell = false;
			for(int i = 0; i < 8; ++i)
			{
				iRow = Get_rowTo(i, row);
				iCol = Get_colTo(i, col);

				if(!dirGrid.is_InGrid(iRow, iCol) || dirGrid.is_NoData(iRow, iCol))
				{
					isBoundaryCell = true;
					break;
				}
			}
			//if the border cell flows to the outside of the DEM
			if(isBoundaryCell) 
			{
				tmpRow = row;
				tmpCol = col;
				if(!dirGrid.nextCell(tmpRow, tmpCol, dirGrid.asByte(row, col)))
					accuOutCells.push_back(std::pair<unsigned int, unsigned int>(row, col));
			}
		}
	}

	//Build BTI tree
	//create an array for to hold the indices of all valid cells
	unsigned int* cellsDataBlockedByWatershed = new unsigned int[validNums + 1];
	std::vector<std::pair<unsigned int, unsigned int>> watershedStartEndIndex;

	unsigned int start = 0;
	unsigned int end = 0;
	std::pair<unsigned int, unsigned int> tmpPair;
	unsigned int lastSize = 0;
	for(std::vector<std::pair<unsigned int, unsigned int>>::iterator iter = accuOutCells.begin();
		iter != accuOutCells.end(); ++iter)
	{
		unsigned int cur = 0;
		unsigned int last = 1; 
		unsigned int curSize = 1;
		
		start += lastSize;
		tmpPair = *iter;
		cellsDataBlockedByWatershed[start] = tmpPair.first * width + tmpPair.second;

		while(cur < curSize)
		{
			while(cur < last)
			{
				for(int i = 0; i < 8; ++i)
				{
					iRow = Get_rowTo(i, cellsDataBlockedByWatershed[cur + end] / width);
					iCol = Get_colTo(i, cellsDataBlockedByWatershed[cur + end] % width);

					if(dirGrid.is_InGrid(iRow, iCol) && !dirGrid.is_NoData(iRow, iCol)
						&& dirGrid.asByte(iRow, iCol) == inverseDir[i])
					{
						cellsDataBlockedByWatershed[curSize + end] = iRow * width + iCol;
						++curSize;
					}
				}
				++cur;
			}
			last = curSize;
		}
		end	+= curSize;
		lastSize = curSize;
		
		watershedStartEndIndex.push_back(std::pair<unsigned int, unsigned int>(start, end - 1));
	}

	//calucalte flow accumulation
	for(std::vector<std::pair<unsigned int, unsigned int>>::iterator iter = watershedStartEndIndex.begin();
		iter != watershedStartEndIndex.end(); ++iter)
	{
		int begin = iter->first;
		int end = iter->second;
		if(begin == end) continue;

		while(begin <= end)
		{
			unsigned int curIndex = cellsDataBlockedByWatershed[end];

			unsigned int curRow = curIndex / width;
			unsigned int curCol = curIndex % width;

			unsigned int nextRow = curRow;
			unsigned int nextCol = curCol;
		
			if(dirGrid.nextCell(nextRow, nextCol, dirGrid.asByte(curRow, curCol)))
			{
				accuGrid.Set_Value(nextRow, nextCol, accuGrid.asInt(curRow, curCol) + accuGrid.asInt(nextRow, nextCol));
			}
			--end;
		}
	}
	endTime = time(NULL);

	double timeConsume = difftime(endTime, startTime);
	if(pTimeConsume) *pTimeConsume = timeConsume;
	printf("Time used: %d seconds\n", (int)timeConsume);
	delete nipsGrid;
	delete[] cellsDataBlockedByWatershed;
}

