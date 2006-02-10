@echo off
rem MSVC71OPTS.BAT
rem
rem    Compile and link options used for building MEX-files
rem    using the Microsoft Visual C++ compiler version 7.1 
rem
rem    $Revision: 1.1 $  $Date: 2006-02-10 21:02:23 $
rem
rem ********************************************************************
rem General parameters
rem ********************************************************************

set MATLAB=%MATLAB%
set MSVCDir=C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7
set DevEnvDir=%MSVCDir%\..\Common7\Tools
set PATH=%MSVCDir%\BIN;%DevEnvDir%;%DevEnvDir%\bin;%MSVCDir%\..\Common7\IDE;%MATLAB_BIN%;%PATH%;
set INCLUDE=%MSVCDir%\ATLMFC\INCLUDE;%MSVCDir%\INCLUDE;%MSVCDir%\PlatformSDK\include;%INCLUDE%;%ITKBIN%\Utilities\vxl\core;%ITKBIN%\Utilities\vxl\vcl;%ITKSRC%\Utilities\vxl\core;%ITKSRC%\Utilities\vxl\vcl;%ITKSRC%\Utilities;%ITKBIN%\Utilities;%ITKBIN%\Utilities\gdcm;%ITKSRC%\Utilities\nifti\znzlib;%ITKSRC%\Utilities\nifti\niftilib;%ITKSRC%\Utilities\expat;%ITKBIN%\Utilities\expat;%ITKBIN%\Utilities\DICOMParser;%ITKSRC%\Utilities\DICOMParser;%ITKSRC%\Utilities\NrrdIO;%ITKSRC%\Utilities\MetaIO;%ITKSRC%\Code\SpatialObject;%ITKSRC%\Code\Numerics\NeuralNetworks;%ITKSRC%\Code\Numerics\Statistics;%ITKSRC%\Code\Numerics\FEM;%ITKSRC%\Code\IO;%ITKSRC%\Code\Numerics;%ITKSRC%\Code\Common;%ITKSRC%\Code\BasicFilters;%ITKSRC%\Code\Algorithms;%ITKBIN%;
set LIB=%MSVCDir%\ATLMFC\LIB;%MSVCDir%\LIB;%MSVCDir%\PlatformSDK\lib;%MATLAB%\extern\lib\win32;%LIB%;%ITKBIN%\bin\\

rem ********************************************************************
rem Compiler parameters
rem ********************************************************************
set COMPILER=cl
set COMPFLAGS=-c -Zp8 -G5 -GR -W3 -DMATLAB_MEX_FILE -nologo
set OPTIMFLAGS=/MD -O2 -Oy- -DNDEBUG
set DEBUGFLAGS=/MDd -Zi -Fd"%OUTDIR%%MEX_NAME%.pdb"
set NAME_OBJECT=/Fo

rem ********************************************************************
rem Linker parameters
rem ********************************************************************
set LIBLOC=%MATLAB%\extern\lib\win32\microsoft\msvc71
set LINKER=link
set LINKFLAGS=/dll /export:%ENTRYPOINT% /MAP /LIBPATH:"%LIBLOC%" libmx.lib libmex.lib libmat.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  /LIBPATH:"%ITKBIN%\bin\release" ITKAlgorithms.lib ITKBasicFilters.lib ITKCommon.lib ITKDICOMParser.lib ITKEXPAT.lib itkgdcm.lib ITKIO.lib itkjpeg12.lib itkjpeg16.lib itkjpeg8.lib ITKMetaIO.lib itknetlib.lib ITKniftiio.lib ITKNrrdIO.lib ITKNumerics.lib itkpng.lib ITKSpatialObject.lib ITKStatistics.lib itksys.lib itktiff.lib itkvcl.lib itkvnl.lib itkvnl_algo.lib itkvnl_inst.lib itkzlib.lib ITKznz.lib snmpapi.lib wsock32.lib /implib:%LIB_NAME%.x
set LINKOPTIMFLAGS=
set LINKDEBUGFLAGS=/debug
set LINK_FILE=
set LINK_LIB=
set NAME_OUTPUT=/out:"%OUTDIR%%MEX_NAME%.dll"
set RSP_FILE_INDICATOR=@

rem ********************************************************************
rem Resource compiler parameters
rem ********************************************************************
set RC_COMPILER=rc /fo "%OUTDIR%mexversion.res"
set RC_LINKER=

set POSTLINK_CMDS=del "%OUTDIR%%MEX_NAME%.map"
set POSTLINK_CMDS1=del %LIB_NAME%.x
