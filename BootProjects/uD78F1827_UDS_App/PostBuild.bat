@echo off
setlocal enabledelayedexpansion

:: ==========================================
:: CONFIGURATION SECTION
:: ==========================================
set "HEXVIEW_PATH=C:\Users\nandu\Desktop\HexView\hexview.exe"

set "INPUT_HEX=C:\Nandu\Per\Projects\Boot_Project\UDS_App\DefaultBuild\UDS_App.hex"
set "OUTPUT_HEX=C:\Nandu\Per\Projects\Boot_Project\UDS_App\DefaultBuild\UDS_App_PostBuild.hex"

:: Passing BOTH windows to be deleted (separated by a colon)
set "CUT_RANGE_1=0x00000000-0x00003FFF"
set "CUT_RANGE_2=0x00010000-0x00017FFF"

:: ==========================================
:: EXECUTION LABELS
:: ==========================================

:VALIDATE
if not exist "%HEXVIEW_PATH%" (
    echo [ERROR] HexView executable not found at: %HEXVIEW_PATH%
    goto :END
)
if not exist "%INPUT_HEX%" (
    echo [ERROR] Input HEX file not found at: %INPUT_HEX%
    goto :END
)

:PROCESS
echo Removing ranges: %CUT_RANGE_1% and %CUT_RANGE_2%...
echo Keeping all other code blocks...

:: Executing HexView to cut both unwanted blocks simultaneously
"%HEXVIEW_PATH%" "%INPUT_HEX%" /CR:%CUT_RANGE_1%:%CUT_RANGE_2% /XI:32 /s -o "%OUTPUT_HEX%"

if %ERRORLEVEL% equ 0 (
    echo [SUCCESS] Remapped hex successfully created at: %OUTPUT_HEX%
) else (
    echo [ERROR] HexView execution failed with error code %ERRORLEVEL%.
)

:END
echo.
pause
