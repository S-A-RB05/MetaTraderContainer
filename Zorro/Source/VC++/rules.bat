@echo off
setlocal

@call "%~3%\vcvars32.bat"
@set "INCLUDE=%INCLUDE%;include"

cl /Fo"Cache/" /D"int=__declspec(dllexport) int" "%~1" "Source\VC++\ZorroRules.c" /link /DLL /NOLOGO /IMPLIB:"Cache\Imp.lib" /OUT:"%~2" > Log\rules.log

endlocal