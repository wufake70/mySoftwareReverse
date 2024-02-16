@echo off
color 2
git add .
git commit . -m"%date% %time%"
git push origin master
color 7