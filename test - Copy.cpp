#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <stdlib.h>
#include <tlhelp32.h>
#include "ptrerr.hpp"

#define syscallwithname(x) system(x"\"Garena+ Plugin Host Service\"")

TCHAR const * _szTaskName=_T("Garena+ Plugin Host Service");
TCHAR  Sname[]=_T("MonitorProc");
//const TCHAR * szServiceShowName=_T("Monitor process");
const TCHAR * _szProcName=_T("ggdllhost.exe");

SERVICE_STATUS_HANDLE ssh;
SERVICE_STATUS ss;
TOKEN_PRIVILEGES TP;
HANDLE hdGThread=INVALID_HANDLE_VALUE,hdToken;


void WINAPI Service_Main(DWORD, LPTSTR *);
void WINAPI Service_Control(DWORD);
DWORD WINAPI GuardThread(LPVOID);
void * Func_FindProcess(const TCHAR *,DWORD *,DWORD *,bool =false);



PROCESSENTRY32 _pe32={sizeof(PROCESSENTRY32),0l,0l,0l,0l,0l,0l,0l,0l,{0}};
void * Func_FindProcess(const TCHAR * szProcName,DWORD * lpdwProcessID,DWORD * lpdwParentProcessID,bool checknext){
	HANDLE hdProcessList=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (szProcName){
		for (bool _More=Process32First(hdProcessList,&_pe32);_More;_More=Process32Next(hdProcessList,&_pe32)){
			if (!_tcscmp(szProcName,_pe32.szExeFile)){
				CloseHandle(hdProcessList);
				if (!lpdwProcessID && !lpdwParentProcessID)
					return ((*lpdwProcessID=_pe32.th32ProcessID,*lpdwParentProcessID=_pe32.th32ParentProcessID),(void*)1);
				else if (lpdwProcessID)
						return ((*lpdwProcessID=_pe32.th32ProcessID),(void*)1);
					 else if (lpdwParentProcessID)
							 return ((*lpdwParentProcessID=_pe32.th32ParentProcessID),(void*)1);
						  else
							 return (void*)1; 
			}
		}
		return NULL;
	}
	else{
		if (lpdwProcessID){
			for (bool _More=Process32First(hdProcessList,&_pe32);_More;_More=Process32Next(hdProcessList,&_pe32)){
				if (checknext) {
					checknext=false;
					continue;
				}
				if (*lpdwProcessID==_pe32.th32ProcessID){
					CloseHandle(hdProcessList);
					if (lpdwParentProcessID)
						return ((*lpdwParentProcessID=_pe32.th32ParentProcessID),_pe32.szExeFile);
					else
						return _pe32.szExeFile;
				}
			}
			return NULL;
		}
		if (lpdwParentProcessID){
			for (bool _More=Process32First(hdProcessList,&_pe32);_More;_More=Process32Next(hdProcessList,&_pe32)){
				if (*lpdwParentProcessID==_pe32.th32ParentProcessID){
					CloseHandle(hdProcessList);
					return _pe32.szExeFile;
				}
			}
			return NULL;
		}
	}
	return NULL;
}

int _tmain(int argc,TCHAR ** argv){
	if (!(argc>1&&!_tcscmp(argv[1],_T("-svc")))) return 0;
	SERVICE_TABLE_ENTRY STE[]={{Sname,(LPSERVICE_MAIN_FUNCTION)Service_Main},{NULL,NULL}};
	StartServiceCtrlDispatcher(STE);
	return 0;
}


void WINAPI Service_Main(DWORD, LPTSTR *){
	ss.dwServiceType=SERVICE_WIN32_OWN_PROCESS;
	ss.dwCurrentState=SERVICE_START_PENDING;
	ss.dwControlsAccepted=SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN;
	ss.dwServiceSpecificExitCode=0;
	ss.dwWin32ExitCode=0;
	ss.dwCheckPoint=0;
	ss.dwWaitHint=2;
	if (!(ssh=RegisterServiceCtrlHandler(Sname,Service_Control))) return ;//_assert("NULL","NULL",0);
	SetServiceStatus(ssh,&ss);
	hdGThread=CreateThread(NULL,0,GuardThread,NULL,0,NULL);
	ss.dwCurrentState=SERVICE_RUNNING;
	ss.dwCheckPoint=0;
	ss.dwWaitHint=0;
	SetServiceStatus(ssh,&ss);
	WaitForSingleObject(hdGThread,INFINITE);
	return ;
}



void WINAPI Service_Control(DWORD dwControl){
	switch (dwControl)
	{
		case SERVICE_CONTROL_SHUTDOWN:
		case SERVICE_CONTROL_STOP:
		TerminateThread(hdGThread,0);
		ss.dwCurrentState=SERVICE_STOPPED;
		ss.dwCheckPoint=0;
		ss.dwWaitHint=0;
		SetServiceStatus(ssh,&ss);
		default:break;
	}
	return ;
}

DWORD Func_GetToken(){
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hdToken);
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &TP.Privileges[0].Luid);
	TP.PrivilegeCount = 1;
	TP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hdToken, FALSE, &TP, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	return GetLastError();
}

DWORD Func_KillProcess(TCHAR const * _szProcName_){
	HANDLE hdProc=INVALID_HANDLE_VALUE;
	DWORD dwProcId=0l;
	while (Func_FindProcess(_szProcName_,&dwProcId,NULL)){
		if ((hdProc=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcId))==INVALID_HANDLE_VALUE)
			return ERROR_ACCESS_DENIED;
		TerminateProcess(hdProc,0);
		CloseHandle(hdProc);
	}
	return GetLastError();
}

DWORD WINAPI GuardThread(LPVOID){
	Func_SetErrorFile(_T("C:\\log.log"),_T("w"));
	Func_GetToken();
	DWORD dwProcessId=0l,dwId2=0l;
	while (1){
		if (Func_FindProcess(_szProcName,&dwProcessId,&dwId2)){
//			Sleep(5*600000);
			Func_FastPMNTS("fuck");
			if (Func_FindProcess(_T("lol.exe"),NULL,NULL) ||
				Func_FindProcess(_T("League of Legends.exe"),NULL,NULL) ||
				Func_FindProcess(_T("LolClient.exe"),NULL,NULL))
				continue;
			else{
//				if (!_tcscmp((TCHAR*)Func_FindProcess(NULL,NULL,&dwId2),_szProcName))
//					dwId2=dwProcessId,Func_FindProcess(_szProcName,&dwProcessId,NULL,true);
			Func_FastPMNTS("fuck1");
				if (Func_KillProcess(_szProcName)==ERROR_ACCESS_DENIED)
				Func_FastPMNTS("fuck3");
				syscallwithname("schtasks /end /TN");
				syscallwithname("schtasks /delete /TN");
			}
		}
		Sleep(5*600000);
	}
	return 0;
}
