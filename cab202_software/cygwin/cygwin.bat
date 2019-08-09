@echo off

start "" %~dp0%\bin\mintty.exe -i /Cygwin.ico bin\bash.exe --login -i
exit
