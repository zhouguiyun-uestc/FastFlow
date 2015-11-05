#include "DEM.h"
#include "FlowDirection.h"
#include "FlowAccu.h"
#include "utils.h"
#include <time.h>
int computeAccumulation(FlowDirection& dirGrid, FlowAccu& accuGrid, unsigned int curRow, unsigned int curCol);
void accuMethodByRecursive(FlowDirection& dirGrid, FlowAccu& accuGrid, double* pTimeConsume)
{
	unsigned int width = dirGrid.getWidth();
	unsigned int height = dirGrid.getHeight();

	time_t startTime, endTime;

	fprintf(stdout, "\nUsing Recursive algorithm to compute the flow accumulation matrix ... \n");
	startTime = time(NULL);

	for(unsigned int row = 0; row < height; ++row)
	{
		for(unsigned int col = 0; col < width; ++col)
		{
			if(dirGrid.is_NoData(row, col)) continue;
			accuGrid.Set_Value(row, col, 0);
			computeAccumulation(dirGrid, accuGrid, row, col);
		}
	}
	endTime = time(NULL);
	double timeConsume = difftime(endTime, startTime);
	if(pTimeConsume) *pTimeConsume = timeConsume;
	fprintf(stdout, "Time used:%d seconds\n", (int)timeConsume);
}
int computeAccumulation(FlowDirection& dirGrid, FlowAccu& accuGrid, unsigned int curRow, unsigned int curCol)
{
	static int indexAndDir[] = {16, 32, 64, 128, 1, 2, 4, 8};
	
	if(accuGrid.asInt(curRow, curCol) < 1)
	{
		accuGrid.Set_Value(curRow, curCol, 1);
		for(int i = 0; i < 8; ++i)
		{
			unsigned int iRow = Get_rowTo(i, curRow);
			unsigned int iCol = Get_colTo(i, curCol);

			if(dirGrid.is_InGrid(iRow, iCol) 
				&&  !dirGrid.is_NoData(iRow, iCol)
				&& (dirGrid.asByte(iRow, iCol) == indexAndDir[i]))
			{
				int neighborValue = computeAccumulation(dirGrid, accuGrid, iRow, iCol);
				accuGrid.Set_Value(curRow, curCol, 
					accuGrid.asInt(curRow, curCol) + neighborValue);
			}
		}
	}
	return accuGrid.asInt(curRow, curCol);
}
