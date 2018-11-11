Requirements on Ubuntu 18.04 LTS:

    sudo apt-get install cmake cmake-curses-gui
    sudo apt-get install swig libvtk6-dev libvtk6.3 libvtk6-qt-dev libvtk6.3-qt
    sudo apt-get install libwxgtk3.0-dev libgtk2.0-dev

Next, clone the repo

    git clone https://github.com/louisepb/TexGen.git
    cd TexGen
    mkdir bin
    cd bin
    ccmake ../

Press 'c' to configure, and set first to only core compile by setting to following settings (press 't' for this full list):

    BUILD_CASCADE_EXPORT             OFF                                                                                                                          
    BUILD_DOCUMENTATION              OFF                                                                                                                          
    BUILD_EXAMPLES                   OFF                                                                                                                          
    BUILD_GUI                        OFF                                                                                                                          
    BUILD_PROFILE                    OFF                                                                                                                          
    BUILD_PYTHON_INTERFACE           OFF                                                                                                                          
    BUILD_RENDERER                   OFF                                                                                                                          
    BUILD_SHARED                     ON                                                                                                                           
    BUILD_UNIT_TESTS                 OFF                                                                                                                          
    CMAKE_BACKWARDS_COMPATIBILITY    2.4                                                                                                                          
    CMAKE_BUILD_TYPE                                                                                                                                              
    CMAKE_INSTALL_PREFIX             /usr/local                                                                                                                   
    PYTHON_SITEPACKAGES_DIR          PYTHON_SITEPACKAGES_DIR-NOTFOUND                                                                                             
    Qt5Core_DIR                      /usr/lib/x86_64-linux-gnu/cmake/Qt5Core                                                                                      
    Qt5Gui_DIR                       /usr/lib/x86_64-linux-gnu/cmake/Qt5Gui                                                                                       
    Qt5Network_DIR                   /usr/lib/x86_64-linux-gnu/cmake/Qt5Network                                                                                   
    Qt5WebKit_DIR                    /usr/lib/x86_64-linux-gnu/cmake/Qt5WebKit                                                                                    
    SWIG_EXECUTABLE                  /usr/bin/swig3.0                                                                                                             
    VTK_DIR                          /usr/lib/cmake/vtk-6.3                                                                                                       
    wxWidgets_CONFIG_EXECUTABLE      /usr/bin/wx-config

Press 'c' again to see there are no errors (in case of errors, change settings to fix these).

Press 'g' to generate the makefiles and exit

Now compile with:

    make 

which will build targets `TexGenCore` and `LatinHypercube`.

Install with:

    make install
