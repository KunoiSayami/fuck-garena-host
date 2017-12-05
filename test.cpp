/*
 * This source code was published under GPL v3
 *
 * Copyright (C) 2017 Too-Naive
 *
 */

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <stdlib.h>
#include <tlhelp32.h>

#define syscallwithname(x) system(x"\"gxx speed launcher\"")
//define _pNULL_
#ifndef _pNULL_
	#if (defined(__GXX_EXPERIMENTAL_CXX0X__)||\
	    (defined(_MSC_VER)&&(_MSC_VER>=1800)))
		#define _pNULL_ nullptr
	#else
		#define _pNULL_ NULL
	#endif
#endif
//end

#define THROWERR(x) throw expection(x)

TCHAR const * _szTaskName=_T("gxx speed launcher");
TCHAR  Sname[]=_T("MonitorProc");
const TCHAR * szServiceShowName=_T("Monitor process");
const TCHAR * _szProcName=_T("gxxsvc.exe");
const TCHAR * _szTargetName=_T("GarenaPlatform");
TCHAR const *SzName[]={
	Sname
};

SERVICE_STATUS_HANDLE ssh;
SERVICE_STATUS ss;
HANDLE hdGThread=INVALID_HANDLE_VALUE;

void WINAPI Service_Main(DWORD, LPTSTR *);
void WINAPI Service_Control(DWORD);
DWORD WINAPI GuardThread(LPVOID);
DWORD WINAPI garenaHostsFucker(LPVOID);
bool Func_FindProcess(const TCHAR *,DWORD * =NULL,DWORD * =NULL);


PROCESSENTRY32 _pe32={sizeof(PROCESSENTRY32),0l,0l,0l,0l,0l,0l,0l,0l,{0}};
bool Func_FindProcess(const TCHAR * szProcName,DWORD * lpdwProcessID,DWORD * lpdwParentProcessID){
	HANDLE hdProcessList=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	for (bool _More=Process32First(hdProcessList,&_pe32);_More;_More=Process32Next(hdProcessList,&_pe32)){
		if (!_tcscmp(szProcName,_pe32.szExeFile)){
			CloseHandle(hdProcessList);
			if (lpdwProcessID && lpdwParentProcessID)
				return ((*lpdwProcessID=_pe32.th32ProcessID,*lpdwParentProcessID=_pe32.th32ParentProcessID),true);
			else if (lpdwProcessID)
					return ((*lpdwProcessID=_pe32.th32ProcessID),true);
				 else if (lpdwParentProcessID)
						 return ((*lpdwParentProcessID=_pe32.th32ParentProcessID),true);
					  else
						 return true; 
		}
	}
	CloseHandle(hdProcessList);
	return false;
}

const size_t localbufsize=1024;
TCHAR buf1[localbufsize],buf2[localbufsize],buf3[localbufsize],szline[localbufsize];
char iobuffer[localbufsize];
void Func_Service_Install(){
	struct expection{
		const TCHAR * Message;
		expection(const TCHAR *_){
			this->Message=_;
		}
	};
	SC_HANDLE shMang=_pNULL_,shSvc=_pNULL_;
	_tprintf(_T("    LICENSE:General Public License\n    \
Copyright (C) 2017 @Too-Naive\n\n"));
	try {
		if (!GetSystemDirectory(buf3,localbufsize))
			THROWERR(_T("GetSystemDirectory() Error in Install Service."));
		_stprintf(buf1,_T("%s\\..\\testmonitor.exe"),buf3);
		_stprintf(buf2,_T("\"%s\\..\\testmonitor.exe\" -svc"),buf3);
		if (!GetModuleFileName(_pNULL_,szline,sizeof(szline)/sizeof(TCHAR)))
			THROWERR(_T("GetModuleFileName() Error in Install Service."));
		_tprintf(_T("    Step1:Copy file.\n"));
		if (!CopyFile(szline,buf1,FALSE))
			THROWERR(_T("CopyFile() Error in Install Service.(Is service has been installed?)"));
		_tprintf(_T("    Step2:Connect to SCM.\n"));
		if (!(shMang=OpenSCManager(_pNULL_,_pNULL_,SC_MANAGER_ALL_ACCESS)))
			THROWERR(_T("OpenSCManager() failed."));
		_tprintf(_T("    Step3:Write service.\n"));
		if (!(shSvc=CreateService(shMang,
			Sname,
			szServiceShowName,
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			buf2,//Program located
			_pNULL_,
			_pNULL_,
			_pNULL_,
			_pNULL_,
			_pNULL_
			))){
			if (GetLastError()==ERROR_SERVICE_EXISTS){
				if (!(shSvc=OpenService(shMang,Sname,SERVICE_ALL_ACCESS)))
					THROWERR(_T("OpenService() Error in install service."));
				if (!DeleteService(shSvc))
					THROWERR(_T("DeleteService() Error in Install Service."));
				CloseServiceHandle(shSvc);
				if (!(shSvc=CreateService(shMang,
					Sname,
					szServiceShowName,
					SERVICE_ALL_ACCESS,
					SERVICE_WIN32_OWN_PROCESS,
					SERVICE_AUTO_START,
					SERVICE_ERROR_NORMAL,
					buf2,
					_pNULL_,
					_pNULL_,
					_pNULL_,
					_pNULL_,
					_pNULL_
					)))
					THROWERR(_T("CreateService() failed.(2)")),CloseServiceHandle(shMang);
			}
			else
				THROWERR(_T("CreateService() failed."));
		}
		else 
			_tprintf(_T("Install service successfully.\n"));
		if (!(shSvc=OpenService(shMang,Sname,SERVICE_START)))
			THROWERR(_T("OpenService() Failed"));
		else
			if (!StartService(shSvc,1,SzName))
				THROWERR(_T("StartService() Failed."));
				else MessageBox(_pNULL_,_T("Service started successfully"),
								_T("Congratulations!"),
								MB_SETFOREGROUND|MB_ICONINFORMATION);
	}
	catch (expection _r){
		_tprintf(_T("\nFatal Error:\n%s (GetLastError():%ld)\n\
Please contact the application's support team for more information.\n"),
		_r.Message,GetLastError());
		_tprintf(_T("\n[Debug Message]\n%s\n%s\n%s\n"),buf1,buf2,buf3);
		abort();
	}
	CloseServiceHandle(shMang);
	CloseServiceHandle(shSvc);
	system("pause");
	return ;
}

int _tmain(int argc,TCHAR ** argv){
	if (!(argc>1&&!_tcscmp(argv[1],_T("-svc")))) Func_Service_Install(),exit(EXIT_SUCCESS);
	SERVICE_TABLE_ENTRY STE[]={{Sname,Service_Main},{NULL,NULL}};
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
	if (!(ssh=RegisterServiceCtrlHandler(Sname,Service_Control))) return ;
	SetServiceStatus(ssh,&ss);
	hdGThread=CreateThread(NULL,0,garenaHostsFucker,NULL,0,NULL);
	ss.dwCurrentState=SERVICE_RUNNING;
	ss.dwCheckPoint=0;
	ss.dwWaitHint=0;
	SetServiceStatus(ssh,&ss);
	WaitForSingleObject(hdGThread,INFINITE);
	return ;
}

void WINAPI Service_Control(DWORD dwControl){
	switch (dwControl){
		case SERVICE_CONTROL_SHUTDOWN:
		case SERVICE_CONTROL_STOP:
		TerminateThread(hdGThread,0);
		CloseHandle(hdGThread);
		ss.dwCurrentState=SERVICE_STOPPED;
		ss.dwCheckPoint=0;
		ss.dwWaitHint=0;
		SetServiceStatus(ssh,&ss);
//		CloseServiceHandle()
		default:break;
	}
	return ;
}

DWORD Func_KillProcess(TCHAR const * _szProcName_){
	HANDLE hdProc=INVALID_HANDLE_VALUE;
	DWORD dwProcId=0l;
	while (Func_FindProcess(_szProcName_,&dwProcId)){
		if ((hdProc=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcId))==INVALID_HANDLE_VALUE)
			return ERROR_ACCESS_DENIED;
		TerminateProcess(hdProc,0);
		CloseHandle(hdProc);
	}
	return GetLastError();
}

bool killSwitch(){
	return true;
}


void kill_gxx_svc(){
    SERVICE_STATUS_PROCESS ssp;
    DWORD dwBytesNeeded;
	SC_HANDLE shMang=_pNULL_,shSvc=_pNULL_;
	shMang=OpenSCManager(_pNULL_,_pNULL_,SC_MANAGER_ALL_ACCESS);
	if (!(shSvc=OpenService(shMang,_szTargetName,SERVICE_ALL_ACCESS)))
		if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
			return;
    if (!QueryServiceStatusEx(shSvc,SC_STATUS_PROCESS_INFO,(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded) ||
		ssp.dwCurrentState == SERVICE_STOPPED || 
		ssp.dwCurrentState == SERVICE_STOP_PENDING)
			CloseServiceHandle(shSvc),CloseServiceHandle(shMang);
	if (!ControlService(shSvc,SERVICE_CONTROL_STOP,(LPSERVICE_STATUS)&ssp));
	CloseServiceHandle(shSvc);
	CloseServiceHandle(shMang);	
}

void disable_gxx_svc(){
	SC_HANDLE shMang=_pNULL_,shSvc=_pNULL_;
	shMang=OpenSCManager(_pNULL_,_pNULL_,SC_MANAGER_ALL_ACCESS);
	if (!(shSvc=OpenService(shMang,_szTargetName,SERVICE_ALL_ACCESS)))
		if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
			return;
	ChangeServiceConfig(shSvc,SERVICE_NO_CHANGE,SERVICE_DEMAND_START,SERVICE_NO_CHANGE,
		_pNULL_,_pNULL_,_pNULL_,_pNULL_,_pNULL_,_pNULL_,_pNULL_);
	CloseServiceHandle(shSvc);
	CloseServiceHandle(shMang);
}

DWORD WINAPI garenaHostsFucker(LPVOID) {
	int operation_code = -1;
	while (1) {
		if (killSwitch()){
			operation_code = syscallwithname("schtasks /query /tn ");
			if (!operation_code){
				syscallwithname("schtasks /end /f /tn ");
				syscallwithname("schtasks /delete /f /tn ");
			}
			disable_gxx_svc();
		}
		if (!Func_FindProcess(_T("Garena.exe"))){
			if (Func_FindProcess(_T("gxxsvc.exe")))
				kill_gxx_svc();
		}
		Sleep(5 * 60000);
	}
	return 0;
}

DWORD WINAPI GuardThread(LPVOID){
	while (1){
		if (Func_FindProcess(_szProcName)){
			Sleep(5*60000);
			if (Func_FindProcess(_T("lol.exe")) ||
				Func_FindProcess(_T("League of Legends.exe")) ||
				Func_FindProcess(_T("LolClient.exe")) || 
				Func_FindProcess(_T("LeagueClient.exe")))
				continue;
			else{
				Func_KillProcess(_szProcName);
				syscallwithname("schtasks /end /TN ");
				syscallwithname("schtasks /delete /f /TN ");
			}
		}
		Sleep(5*60000);
	}
	return 0;
}
