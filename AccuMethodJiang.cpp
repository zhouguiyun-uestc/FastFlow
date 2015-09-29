#include "DEM.h"
#include "FlowDirection.h"
#include "FlowAccu.h"
#include "utils.h"
#include <stack>
#include <time.h>

void accuMethodByJiang(FlowDirection& dirGrid, FlowAccu& accuGrid, double* consumeTime)
{
	unsigned int width = dirGrid.getWidth();
	unsigned int height = dirGrid.getHeight();
	static const unsigned char inverseDir[8] = {16, 32, 64, 128, 1, 2, 4, 8};
	std::stack<unsigned int> sourcestack;

	time_t startTime, endTime;
	fprintf(stdout, "\nUsing Jiang's algorithm to compute the flow accumulation matrix ... \n");
	startTime = time(NULL);
	int cellFlag = -1;
	for(unsigned int row = 0; row < height; ++row)
	{
		for(unsigned int col = 0; col < width; ++col)
		{
			if(dirGrid.is_NoData(row, col)) continue;

			cellFlag = -1;
			for(int i = 0; i < 8; ++i)
			{
				unsigned int iRow = Get_rowTo(i, row);
				unsigned int iCol = Get_colTo(i, col);

				if(dirGrid.is_InGrid(iRow, iCol)
					&& !dirGrid.is_NoData(iRow, iCol)
					&& (dirGrid.asByte(iRow, iCol) == inverseDir[i]))
				{
					--cellFlag;
				}
			}
			accuGrid.Set_Value(row, col, cellFlag);
			if(cellFlag == -1)
			{
				sourcestack.push(row * width + col);
			}
		}
	}

	while(!sourcestack.empty())
	{
		unsigned int curNodeIndex = sourcestack.top();
		sourcestack.pop();

		unsigned int curRow = curNodeIndex / width;
		unsigned int curCol = curNodeIndex % width;

		int accuValue = 1;

		if((accuGrid.asInt(curRow, curCol) + 1) == 0)
		{	
			accuValue = 1;
			for(int i = 0; i < 8; ++i)
			{
				unsigned int iRow = Get_rowTo(i, curRow);
				unsigned int iCol = Get_colTo(i, curCol);

				if(dirGrid.is_InGrid(iRow, iCol)
					&& !dirGrid.is_NoData(iRow, iCol)
					&& (dirGrid.asByte(iRow, iCol) == inverseDir[i]))
				{
					accuValue += accuGrid.asInt(iRow, iCol);
				}
			}
			accuGrid.Set_Value(curRow, curCol, accuValue);
		}

		unsigned int nextRow = curRow;
		unsigned int nextCol = curCol;
		if(!dirGrid.nextCell(nextRow, nextCol, dirGrid.asByte(curRow, curCol)))
			continue;

		accuGrid.Set_Value(nextRow, nextCol, accuGrid.asInt(nextRow, nextCol) + 1);
		if((accuGrid.asInt(nextRow, nextCol) + 1) == 0)
			sourcestack.push(nextRow * width + nextCol);
	}
	endTime = time(NULL);
	double timeUsed = difftime(endTime, startTime);
	if(consumeTime) *consumeTime = timeUsed;
	printf("Time used: %d seconds\n", (int)timeUsed);
}
