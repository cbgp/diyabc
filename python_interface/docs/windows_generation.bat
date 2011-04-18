@echo off
set pyinstaller=%1%
set prog=%3%
set icon=%2%
set output=%4%
python %pyinstaller% %prog% --onefile --icon=%icon% -o %output%
