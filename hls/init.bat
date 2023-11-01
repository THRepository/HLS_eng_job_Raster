@set INTELFPGA_ROOT=C:\intelFPGA_lite\18.1
@set MODELSIM_TYPE=modelsim_ase

call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\amd64\vcvars64.bat"

@set HLS_ROOT=%INTELFPGA_ROOT%\hls
@set VSIM_BIN=%INTELFPGA_ROOT%\%VSIM_ROOT%\win32aloem

@set PATH=%VSIM_BIN%;%PATH%
@call %HLS_ROOT%\init_hls.bat