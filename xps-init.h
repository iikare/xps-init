#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>

using namespace std;

struct QueryMonitor;
struct monTimer;

struct QueryMonitor{
    vector<RECT> monNum;

    static BOOL CALLBACK MonitorEnum(HMONITOR hMon,HDC hdc,LPRECT lprcMonitor,LPARAM dwData){
        QueryMonitor* data = reinterpret_cast<QueryMonitor*>(dwData);
        data->monNum.push_back(*lprcMonitor);
        return TRUE;
    }

    QueryMonitor(){
        EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
    }
};

struct monTimer{
	int initCount;
	int finCount;
	long monInterval;
};
