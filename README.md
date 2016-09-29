# Fuck "Garena+ Plugin Host Service" Project

This project help you to auto kill and remove "Garena+ Plugin Host Service" in task schedule.

## Install

Run test.exe without any parameters.

## Uninstall

1. Open Command Prompt with Administrator privilege
2. Type `sc stop MonitorProc` then press ENTER
3. Type `sc delete MonitorProc` then press ENTER
4. Delete `%systemroot%\testmonitor.exe`

## License

GENERAL PUBLIC LICENSE version 3
