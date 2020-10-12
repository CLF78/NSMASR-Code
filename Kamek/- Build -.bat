@echo OFF
python tools/mapfile_tool.py
if %errorlevel%==0 goto kamek
pause
exit

:kamek
python tools/kamek.py NewerProjectKP.yaml --no-rels --gcc-path=tools\devkitPPC\bin --gcc-append-exe --use-mw --mw-path="C:\Program Files (x86)\Freescale\CW for MPC55xx and MPC56xx 2.10\PowerPC_EABI_Tools\Command_Line_Tools" --fast-hack
if %errorlevel%==0 goto buildkor
pause
exit


:buildkor
python tools/kamek.py NewerProjectKP.yaml --no-rels --gcc-path=tools\devkitPPC\bin --gcc-append-exe --use-mw --mw-path="C:\Program Files (x86)\Freescale\CW for MPC55xx and MPC56xx 2.10\PowerPC_EABI_Tools\Command_Line_Tools" --fast-hack --configs=tools/kamek_configs_kor.yaml
if %errorlevel%==0 goto move
pause
exit

:move
move "%~dp0\NewerASM\*.bin" C:\Dolphin\NSMASR\NewerRes >nul
if %errorlevel%==0 goto end
pause
exit

:end
echo Built all!
pause
