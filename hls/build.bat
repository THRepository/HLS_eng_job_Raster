@echo off
set "SOURCE_FILES=raster_functions.c VGA_raster.c"
set "FPGA_OUTPUT_NAME=ver_2"
set "FPGA_FLAGS=--quartus-compile"
set "CLOCK_FLAG=--clock 50MHz"
set "SIMULATION_FLAG=--simulator none"

if not "%1"=="" (
  set "TARGET=%1"
) else (
  set "TARGET=emulate"
)

if "%TARGET%" == "emulate" (
  set "CMD=i++ -march=x86-64 -o %TARGET%.exe" 
) else if "%TARGET%" == "fpga" (
  set "CMD=i++ -march=MAX10 %FPGA_FLAGS% %CLOCK_FLAG% %SIMULATION_FLAG% -o %FPGA_OUTPUT_NAME%.exe"
)
set "CMD=%CMD:""="%"

echo %CMD% %SOURCE_FILES%
echo:
%CMD% %SOURCE_FILES%