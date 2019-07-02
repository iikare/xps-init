#include "xps-init.h"

QueryMonitor monIndex;
monTimer timer = {0, 0, REFRESH_INTERVAL};

BOOL setFlag = FALSE;
BOOL wndMsg = TRUE;
BOOL msgStatus;
long timeInt = 0;

HKEY AccessKeyTime;
HKEY AccessKeyStart;
HWND shadow;
MSG msg;

int main(int argc, char const *argv[]){		//TODO: add .ini file
	WNDCLASS wc = {0};

	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.lpszClassName = TEXT("xps-init");
	RegisterClass(&wc);
	HWND shadow = CreateWindow(TEXT("xps-init"), TEXT(""), 0, 0, 0, 0, 0, NULL, NULL, NULL, 0);
	BOOL msgStatus = GetMessage(&msg, shadow, 0, 0);

	ConfigureStartup();

	thread THREAD_MessageHandler(MessageHandler);
	thread THREAD_LogicHandler(LogicHandler);

	THREAD_LogicHandler.join();
	THREAD_MessageHandler.join();

	return 0;	
}

void ReSyncTime(){

	if(RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\W32Time\\Parameters"), &AccessKeyTime) != ERROR_SUCCESS){
		cout << "Failed to open registry key.\n";
	}

	//NTP = on, NoSync = off
	if (RegSetValueEx(AccessKeyTime, TEXT("Type"), 0, REG_SZ, (LPBYTE)"NoSync", strlen("NoSync")*sizeof(char)) != ERROR_SUCCESS){
		cout << "Failed to clear registry value.\n";
	}

	if (RegSetValueEx(AccessKeyTime, TEXT("Type"), 0, REG_SZ, (LPBYTE)"NTP", strlen("NTP")*sizeof(char)) != ERROR_SUCCESS){
		cout << "Failed to set registry value.\n";
	}

	system("w32tm /resync /nowait");

	RegCloseKey(AccessKeyTime);
}

void DeInit(){
	for(int tryCount = 0; tryCount < MAX_SHUTDOWN_ATTEMPT; tryCount++){
		if(RegCloseKey(AccessKeyTime) == ERROR_SUCCESS){
			break;
		}
		if(tryCount == 2){
			break;
			cout << "Failed to deinitialize registry key.\n";
		}
	}
}

void ConfigureStartup(){	//TODO: Add .ini based options

	RegCreateKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &AccessKeyStart);
	
	if (RegSetValueEx(AccessKeyStart, TEXT("xps-init"), 0, REG_SZ, (LPBYTE)"C:\\files\\projects\\xps-init\\xpsinit.exe", strlen("C:\\files\\projects\\xps-init\\xpsinit.exe")*sizeof(char)) != ERROR_SUCCESS){
		cout << "Failed to set program to run at startup.\n";
	}

	RegCloseKey(AccessKeyStart);
}

void StartTB(){
	if(!ShellExecuteA(NULL, "open", PATH_01, NULL, NULL, SW_HIDE)){
		cout << "Error: Could not open TTB.\n";
	}
}

static long WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch(message){
    	case WM_POWERBROADCAST:
    		switch(wParam){
    			case PBT_APMRESUMEAUTOMATIC:
    				StartTB();
    				break;
    			case PBT_APMRESUMESUSPEND:
    				StartTB();
    				break;
    		}
			break;
		case WM_DESTROY:
			wndMsg = FALSE;
			DeInit();

			if (!FreeConsole()) {
				cout << "Error: Could not free console.\n";
			}

			DestroyWindow(shadow);

			PostQuitMessage(0);
			break;
		default:
        	return DefWindowProc(hWnd, message, wParam, lParam);
        	break;
    }
}

void MessageHandler(){
	while(msgStatus != 0){
		if(msgStatus == -1){
			wndMsg = FALSE;
			cout << "Failed to initialize application.\n";
		}
		else{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		GetMessage(&msg, shadow, 0, 0);
	}
}

void LogicHandler(){
	while(wndMsg){
		cout<<"test\n";

		QueryMonitor();
		timer.initCount = monIndex.monNum.size();

		Sleep(timer.monInterval);

		QueryMonitor();
		timer.finCount = monIndex.monNum.size();

		cout << monIndex.monNum.size() << "\n";

		if(timer.initCount < timer.finCount && monIndex.monNum.size() >= 2){
			if(setFlag == FALSE){
				system("displayswitch.exe /extend");
				cout << "Display Extended.\n";
				setFlag = TRUE;
			}
		}

		if(timer.initCount > timer.finCount){
			if(monIndex.monNum.size() == 1){
				setFlag = FALSE;
			}
			else{
				system("displayswitch.exe /extend");
			}
		}

		if(timeInt % 30 == 0){	//90sec refresh interval
			ReSyncTime();
		}

		if(timeInt % 15 == 0){
			StartTB();
		}

		timeInt++;
	}

}