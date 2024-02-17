@echo off
color 2
git add .
git commit . -m"%date% %time%"
set /p input=Go to Git[Y/N]： 
if "%input%"=="y" start https://github.com/wufake70/mySoftwareReverse
color 7