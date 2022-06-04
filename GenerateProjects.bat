@REM Uncomment below to generate 2015.
@REM We should separate solutions based on VS versions instead of this.
@REM call vendor\bin\premake\premake5.exe vs2015

@ECHO "Generating 2019 solutions. For other versions, switch target version in this .bat"
call vendor\bin\premake\premake5.exe vs2019

pause