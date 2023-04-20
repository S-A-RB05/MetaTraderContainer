@echo off
setlocal

@call "%~3%\vcvars64.bat"
@set "INCLUDE=%INCLUDE%;include"

cl /Fo"Cache/" /D"int=__declspec(dllexport) int" "%~1" "Source\VC++\ZorroRules.c" /link /DLL /NOLOGO /MACHINE:X64 /IMPLIB:"Cache\Imp.lib" /OUT:"%~2" > Log\rules.log

endlocal