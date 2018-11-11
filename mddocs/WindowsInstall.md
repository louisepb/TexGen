In order to compile [TexGen](../README.md) and all of its modules, the following software is required:

The following programs are needed to build TexGen:

1. CMake
2. SWIG 

The following libraries are needed to link to TexGen:

1. VTK
2. wxWidgets
3. OpenCascade
4. Python 

# Build using Visual Studio 2017 Community on Windows 10

We present here how to build TexGen on __Windows 10__, 64 bit, using VS2017. 
## Install tools
1. Install [cmake](https://cmake.org/download/). At the time of writing this is 
version 3.12.4: `cmake-3.12.4-win64-x64.msi`. This will install the graphical 
application __CMake__ (cmake-gui).
2. Install [Github Desktop](https://desktop.github.com/). Once installed, start it, 
and select __Clone Repository__ `CTRL+SHIFT+O`, and clone this repository. 
3. Install [Visual Studio Community 2017](https://visualstudio.microsoft.com/downloads/),
 which is the free version of the Microsoft Compiler

## Install and Build Dependencies
### SWIG
TexGen requires SWIG. We install an older version, namely `2.0.10`. On the [download page](http://www.swig.org/download.html), go to the older releases and download package `swigwin-2.0.10.zip`. Extract this zip in __C:\Program Files\swigwin-2.0.10__

### OpenCascade
Go to the download area of [OpenCascade](https://www.opencascade.com/content/download-center), select `Previous Releases`, and 
download version 6.5.2: __OpenCASCADE652.exe__.

Run this exe file to install.

### Python 2.7
Next download the WIN32 (x86) version of [Python 2.7](https://www.python.org/downloads/windows/). At the time of writing this is 
[python-2.7.15.msi](https://www.python.org/ftp/python/2.7.15/python-2.7.15.msi).

Run the installer, which should install in location __C:\Python27__.

### wxWidgets
[Download](http://www.wxwidgets.org/downloads/) from the [sourceforge archive](https://sourceforge.net/projects/wxwindows/files/2.8.12/)
the installer `wxMSW-2.8.12-Setup.exe`. Run it, which will install in __C:\wxWidgets-2.8.12__

This code still needs to be build. Start Visual Studio 2017, and open project __C:\wxWidgets-2.8.12\build\msw\wx.dsw__. VS2017 will require you to upgrade this project, and will save it as __C:\wxWidgets-2.8.12\build\msw\wx.sln__. Change in __ALL__ projects the target platform to _Windows 10_(right click project, select `Properties-->General`, and select correct platform). Build the solution (F7).

You can download a zip of the already converted project from [ugent.be](http://users.ugent.be/~bmalengi/TexGen/), which for use you need to extract in the location __C:\wxWidgets-2.8.12__.

