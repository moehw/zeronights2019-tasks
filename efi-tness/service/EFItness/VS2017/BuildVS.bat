set WORKSPACE=%~dp0..\..
set PACKAGES_PATH=%~dp0..\..\edk2;%~dp0..\..
cd ..\..\edk2
@call edksetup.bat
build -p ..\EFItness\EFItness.dsc -t VS2017 %*
REM build -p OvmfPkg\OvmfPkgX64.dsc -t VS2017 %*
REM build -p ShellPkg\ShellPkg.dsc -t VS2017 %*