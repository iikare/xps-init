#include "xps-init.h"

QueryMonitor monIndex;
monTimer timer = {0, 0, 3000};
BOOL setFlag = FALSE;
long timeInt = 0;

int main(int argc, char const *argv[]){
	while(true /*add shutdown flag*/){
		QueryMonitor();
		timer.initCount = monIndex.monNum.size();
		Sleep(timer.monInterval);
		
		QueryMonitor();
		timer.finCount = monIndex.monNum.size();

		cout << monIndex.monNum.size() << "\n";

		if(timer.initCount < timer.finCount && monIndex.monNum.size() >= 2){
			if(setFlag == FALSE){
				system("displayswitch.exe /extend");
				cout << "Display Extended\n";
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
		system("w32tm /resync /nowait");
		timeInt++;

		if(timeInt >= 30){	//90sec refresh interval
			system("w32tm /resync /nowait");
		}
	}
	return 0;	
}
