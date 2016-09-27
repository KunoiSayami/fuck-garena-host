@set EXENAME=test
windres %EXENAME%.rc -o tmp.o
g++ -o %EXENAME%.exe %EXENAME%.cpp tmp.o -s -O2
del /q tmp.o
@sc create MonitorProc binpath="%~dp0%test.exe -svc" Displayname="Monitor process"
pause