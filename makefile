fastflow:AccuMethodBTI.o AccuMethodJiang.o AccuMethodRecursive.o AccuMethodWang.o AccuMethodZhou.o dem.o FlowAccu.o FlowDirection.o utils.o FastFlow.o
	g++ AccuMethodBTI.o AccuMethodJiang.o AccuMethodRecursive.o AccuMethodWang.o AccuMethodZhou.o dem.o FlowAccu.o FlowDirection.o utils.o FastFlow.o -lgdal --std=c++11 -fpermissive -o fastflow
AccuMethodBTI.o:AccuMethodBTI.cpp DEM.h dem.cpp FlowDirection.h FlowDirection.cpp FlowAccu.h FlowAccu.cpp utils.h utils.cpp
	g++ -c AccuMethodBTI.cpp --std=c++11 -fpermissive
AccuMethodJiang.o:AccuMethodJiang.cpp DEM.h dem.cpp FlowDirection.h FlowDirection.cpp FlowAccu.h FlowAccu.cpp utils.h utils.cpp
	g++ -c AccuMethodJiang.cpp --std=c++11 -fpermissive
AccuMethodRecursive.o:AccuMethodRecursive.cpp DEM.h dem.cpp FlowDirection.h FlowDirection.cpp FlowAccu.h FlowAccu.cpp utils.h utils.cpp
	g++ -c AccuMethodRecursive.cpp --std=c++11 -fpermissive
AccuMethodWang.o:AccuMethodWang.cpp DEM.h dem.cpp FlowDirection.h FlowDirection.cpp FlowAccu.h FlowAccu.cpp utils.h utils.cpp
	g++ -c AccuMethodWang.cpp --std=c++11 -fpermissive
AccuMethodZhou.o:AccuMethodZhou.cpp FlowAccu.h FlowAccu.cpp utils.h utils.cpp
	g++ -c AccuMethodZhou.cpp --std=c++11 -fpermissive
dem.o:DEM.h dem.cpp utils.h utils.cpp
	g++ -c dem.cpp --std=c++11 -fpermissive
FlowAccu.o:FlowAccu.cpp FlowAccu.h utils.h utils.cpp
	g++ -c FlowAccu.cpp --std=c++11 -fpermissive
FlowDirection.o:FlowDirection.cpp FlowDirection.h utils.h utils.cpp
	g++ -c FlowDirection.cpp --std=c++11 -fpermissive
utils.o:DEM.h dem.cpp FlowDirection.h FlowDirection.cpp utils.h utils.cpp
	g++ -c utils.cpp --std=c++11 -fpermissive
FastFlow.o:FastFlow.cpp FlowAccu.h FlowAccu.cpp DEM.h dem.cpp FlowDirection.h FlowDirection.cpp utils.h utils.cpp
	g++ -c FastFlow.cpp --std=c++11 -fpermissive
clean:
	@echo "cleanning project"
	-rm fastflow *.o
	@echo "clean completed"
.PHONY: clean

