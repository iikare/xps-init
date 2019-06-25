#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>

using namespace std;

struct QueryMonitor;
struct monTimer;
void ReSyncTime();

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

void ReSyncTime(){
	HKEY AccessKey;

	if(RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\W32Time\\Parameters"), &AccessKey) != ERROR_SUCCESS){
		cout << "Failed to open registry key.\n";
	}
	//NTP = on, NoSync = off
	if (RegSetValueEx(AccessKey, TEXT("Type"), 0, REG_SZ, (LPBYTE)"NoSync", strlen("NoSync")*sizeof(char)) != ERROR_SUCCESS){
		cout << "Failed to clear registry value.\n";
	}

	if (RegSetValueEx(AccessKey, TEXT("Type"), 0, REG_SZ, (LPBYTE)"NTP", strlen("NTP")*sizeof(char)) != ERROR_SUCCESS){
		cout << "Failed to set registry value.\n";
	}

	system("w32tm /resync");

	RegCloseKey(AccessKey);
}
