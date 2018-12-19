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

### VTK
From the [VTK](https://www.vtk.org/download/) website download __VTK-6.3.0.zip__, and extract this in your Download directory in folder `VTK-6.3.0`. We now need to build this with VS2017 using first cmake to set our compile options.

From your start menu, select cmake, and set as source code directory

    YOUR_LOCATION/Downloads/VTK-6.3.0/VTK-6.3.0
    
Creqate a directory bin there and set that as build directory:

    YOUR_LOCATION/Downloads/VTK-6.3.0/VTK-6.3.0/bin

You can click the `Advanced` checkbox to indicate components to build. For TexGen you need:

    BUILD_EXAMPLES           OFF
    BUILD_SHARED_LIBS        OFF or ON
    VTK_GROUP_STANDALONE     ON
    VTK_GROUP_TK             ON
    VTK_PYTHON_VERSION       2
    VTK_RENDERING_BACKEND    OpenGL

Press `Configure`. No errors should appear (in red). If so, fix the errors, and press `Configure` again. When no errors, press `Generate`, which will create a solution for VS2017.

Now that you have created a VS2017 project, start VS2017, and open __YOUR_LOCATION\Downloads\VTK-6.3.0\VTK-6.3.0\bin\VTK.sln__
Select in Solution Explorer target ALL_BUILD, set in the toolbar as configuration __Release__, and build VTK (F7). 

When finished, start VS2017 as admin (right-click in start menu`-->more-->run as admin`), select VTK.sln, select target `INSTALL`and build this (F7) to install VTK to __C:\Program Files (x86)\VTK__.

Note that this builds VTK for platform WIN32.

## Build TexGen

You downloaded this TexGen repo when installing Github Desktop, see above. Now we use cmake to set build options, and use VS2017 to build TexGen. 
From your start menu, select cmake, and set as source code directory

    YOUR_LOCATION/Documents/GitHub/TexGen
    
Creqate a directory bin there and set that as build directory:

    YOUR_LOCATION/Documents/GitHub/TexGen/bin

You can click the `Advanced` checkbox to indicate components to build. It is recommended to begin with a minimal build, this is achieved by setting the cache values as follows:

    BUILD_CASCADE_EXPORT           OFF
    BUILD_DOCUMENTATION            OFF
    BUILD_EXAMPLES                 OFF
    BUILD_GUI                      OFF
    BUILD_PYTHON_INTERFACE         OFF
    BUILD_RENDERER                 OFF
    BUILD_SHARED                   ON
    BUILD_UNIT_TESTS               OFF
    
Press `Configure`. No errors should appear (in red). If so, fix the errors, and press `Configure` again. When no errors, press `Generate`, which will create a solution for VS2017.

Now that you have created a VS2017 project, start VS2017, and open __YOUR_LOCATION/Documents/GitHub/TexGen/bin/TexGen.sln__
Select in Solution Explorer target `ALL_BUILD`, set in the toolbar as configuration __Release__, and build TexGen. 

In order to build the GUI, you need to set the options in cmake instead as: 

    BUILD_CASCADE_EXPORT           ON
    BUILD_DOCUMENTATION            OFF
    BUILD_EXAMPLES                 OFF
    BUILD_GUI                      ON
    BUILD_PYTHON_INTERFACE         ON
    BUILD_RENDERER                 ON
    BUILD_SHARED                   ON
    BUILD_UNIT_TESTS               OFF

Configure and when no errors, press Generate to update the solution.

When finished, start VS2017 as admin (right-click in start menu`-->more-->run as admin`), select TexGen.sln, select target `INSTALL`and build this (F7) to install TexGen to __C:\Program Files (x86)\TexGen__.

Note that this builds TexGen for platform WIN32. Only in this final directory can you run the `TexGenGUI.exe` application. If `dll`-files are missing, copy them into this directory as the most easy fix.


### cmake variables
In case of problems, make sure following variables in cmake are set correct:

__PYTHON_SITEPACKAGES_DIR__ - This should be the folder where the TexGen python modules will be placed. With above install this should be `C:/Python27/Lib/site-packages`. This will depend on the version of Python installed and where you decided to install it.

__SWIG_DIR__ - This is the location where you extracted the SWIG executable. e.g. ´C:/Program Files/swigwin-2.0.10´.

__SWIG_EXECUTABLE__ - Same as root directory followed by swig.exe e.g.
´C:/Program Files/swigwin-2.0.10/swig.exe´.

__wxWidgets_ROOT_DIR__ - Location where you install wxWidgets. e.g. `C:/wxWidgets-2.8.12`.

__wxWidgets_LIB_DIR__ - Location where the wxWidgets libraries were compiled to, should be the same as the root directory followed by lib/vc_lib: `C:/wxWidgets-2.8.12/lib/vc_lib`.

__wxWidgets_INCLUDE_DIR__ - Location where you install header files of wxWidgets. e.g. `C:/wxWidgets-2.8.12/include`.

__wxWidgets_CONFIGURATION__ - `msw` 

__OPENCASCADE_INCLUDE_DIR__ - Location where the OpenCascade header files are located. e.g. `C:/OpenCASCADE6.5.2/ros/inc`.

__OPENCASCADE_LIBRARY_DIR__ - Location where the OpenCascade libraries are located. e.g. `C:/OpenCASCADE6.5.2/ros/win32/vc8/lib`.

__OPENCASCADE_DEBUG_LIBRARY_DIR__ - Location where the OpenCascade debug libraries are located. e.g. 'C:/OpenCASCADE6.5.2/ros/win32/vc8/libd'. If no debug made, use OPENCASCADE_LIBRARY_DIR value.

__VTK_DIR__ - Location of the VTK binary directory (note: not the source directory): `C:/Program Files (x86)/VTK/lib/cmake/vtk-6.3`

