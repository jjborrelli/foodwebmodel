@echo off
set MIN_X=1
set MIN_Y=1
set MAX_X=500
set MAX_Y=500
set /a MED_Y=(%MAX_Y%-%MIN_Y%)/2
set SECTION_LENGTH=40
set /a INITIAL_Y=%MED_Y%-%SECTION_LENGTH%/2
set /a FINAL_Y=%MED_Y%+%SECTION_LENGTH%/2
for %%f in ("DCMesh_Z*") do (
	gdalwarp -te %MIN_X% %INITIAL_Y% %MAX_X% %FINAL_Y% %%f %%~nf_clipped.nc
)