@echo off
chcp 65001 >nul
color 2
git add .
git commit . -m"%date% %time%"
git push origin master
set /p input=Go to Git[Y/N]:
if "%input%"=="y" goto:GoToGit
if "%input%"=="Y" goto:GoToGit





:GoToGit
start https://github.com/wufake70/mySoftwareReverse





color 7
pause > nul