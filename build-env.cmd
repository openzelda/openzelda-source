@ECHO off
cls
@rem Make sure these paths are correct.
set MAKE=mingw32-make.exe
set MINGWPATH=C:\dev\TDM-GCC-64
set MSYSPATH=C:\dev\mingw\msys\1.0
set SUPPORTPATH=C:\dev\supportlibs
set PATH=;%MINGWPATH%\bin\;%MSYSPATH%\bin;%SUPPORTPATH%\bin;%PATH%

set LDFLAGS=-L"%SUPPORTPATH%/lib"
set CPPFLAGS=-I"%SUPPORTPATH%/include" -I"%SUPPORTPATH%/include/sdl2" -I"%MINGWPATH%/include" -I"%MINGWPATH%/include/sdl2"
set CFLAGS=-I"%SUPPORTPATH%/include" -I"%SUPPORTPATH%/include/sdl2" -I"%MINGWPATH%/include" -I"%MINGWPATH%/include/sdl2"

set BUILDDEBUG=FALSE

echo Open Zelda Build Environment - Run %MAKE%
echo ----------------------------------------

IF EXIST %MSYSPATH%\bin\mkdir.exe ( IF EXIST %MSYSPATH%\bin\uname.exe ( @echo - MSYS path: %MSYSPATH% ) ) ELSE ( @echo - MSYS not Installed at %MSYSPATH% )
IF EXIST %MINGWPATH%\bin\gcc.exe ( @echo - Compiler path: %MINGWPATH% ) ELSE ( @echo - Compiler not Installed at %MINGWPATH% )
IF EXIST %SUPPORTPATH%\lib\libSDL2.a ( @echo - SDL2 path: %SUPPORTPATH% ) ELSE ( @echo - SDL2 not Installed at %SUPPORTPATH% )
IF EXIST %SUPPORTPATH%\lib\gobject-2.0.lib ( @echo - GTK path: %SUPPORTPATH% ) ELSE ( @echo - GTK not Installed at %SUPPORTPATH% )

cmd /k echo ----------------------------------------
