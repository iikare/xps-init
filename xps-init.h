#pragma once
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
#include <thread>

using namespace std;

#define MAX_SHUTDOWN_ATTEMPT 3
#define REFRESH_INTERVAL 3000
#define PATH_01 ".\\target\\TTB.lnk"

struct QueryMonitor;
struct monTimer;
void ReSyncTime();
void DeInit();
void StartTB();
void MessageHandler();
void LogicHandler();
static long WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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