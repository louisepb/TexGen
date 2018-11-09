sudo apt-get install cmake

sudo apt-get install libvtk6-dev libvtk6.3 libvtk6-qt-dev libvtk6.3-qt
sudo apt-get install libwxgtk3.0-dev

git clone https://github.com/louisepb/TexGen.git
cd TexGen
mkdir bin
cd bin
ccmake ../

Press 'c' to configure, and change python version, and verify all is as follows (press 't' for this full list):

 BUILD_CASCADE_EXPORT            *OFF                                                                                                                                        
 BUILD_DOCUMENTATION             *OFF                                                                                                                                        
 BUILD_EXAMPLES                  *OFF                                                                                                                                        
 BUILD_GUI                       *ON                                                                                                                                         
 BUILD_PROFILE                   *OFF                                                                                                                                        
 BUILD_PYTHON_INTERFACE          *ON                                                                                                                                         
 BUILD_RENDERER                  *ON                                                                                                                                         
 BUILD_SHARED                    *ON                                                                                                                                         
 BUILD_UNIT_TESTS                *OFF                                                                                                                                        
 CMAKE_AR                        */usr/bin/ar                                                                                                                                
 CMAKE_BACKWARDS_COMPATIBILITY   *2.4                                                                                                                                        
 CMAKE_BUILD_TYPE                *                                                                                                                                           
 CMAKE_COLOR_MAKEFILE            *ON                                                                                                                                         
 CMAKE_CXX_COMPILER              */usr/bin/c++                                                                                                                               
 CMAKE_CXX_COMPILER_AR           */usr/bin/gcc-ar-7                                                                                                                          
 CMAKE_CXX_COMPILER_RANLIB       */usr/bin/gcc-ranlib-7                                                                                                                      
 CMAKE_CXX_FLAGS                 *                                                                                                                                           
 CMAKE_CXX_FLAGS_DEBUG           *-g                                                                                                                                         
 CMAKE_CXX_FLAGS_MINSIZEREL      *-Os -DNDEBUG                                                                                                                               
 CMAKE_CXX_FLAGS_RELEASE         *-O3 -DNDEBUG                                                                                                                               
 CMAKE_CXX_FLAGS_RELWITHDEBINFO  *-O2 -g -DNDEBUG                                                                                                                            
 CMAKE_C_COMPILER                */usr/bin/cc                                                                                                                                
 CMAKE_C_COMPILER_AR             */usr/bin/gcc-ar-7                                                                                                                          
 CMAKE_C_COMPILER_RANLIB         */usr/bin/gcc-ranlib-7                                                                                                                      
 CMAKE_C_FLAGS                   *                                                                                                                                           
 CMAKE_C_FLAGS_DEBUG             *-g                                                                                                                                         
 CMAKE_C_FLAGS_MINSIZEREL        *-Os -DNDEBUG                                                                                                                               
 CMAKE_C_FLAGS_RELEASE           *-O3 -DNDEBUG                                                                                                                               
 CMAKE_C_FLAGS_RELWITHDEBINFO    *-O2 -g -DNDEBUG                                                                                                                            
 CMAKE_EXE_LINKER_FLAGS          *                                                                                                                                           
 CMAKE_EXE_LINKER_FLAGS_DEBUG    *                                                                                                                                           
 CMAKE_EXE_LINKER_FLAGS_MINSIZE  *                                                                                                                                           
 CMAKE_EXE_LINKER_FLAGS_RELEASE  *                                                                                                                                           
 CMAKE_EXE_LINKER_FLAGS_RELWITH  *                                                                                                                                           
 CMAKE_EXPORT_COMPILE_COMMANDS   *OFF                                                                                                                                        
 CMAKE_INSTALL_PREFIX            *~/.                                                                                                                                        
 CMAKE_LINKER                    */usr/bin/ld                                                                                                                                
 CMAKE_MAKE_PROGRAM              */usr/bin/make                                                                                                                              
 CMAKE_MODULE_LINKER_FLAGS       *                                                                                                                                           
 CMAKE_MODULE_LINKER_FLAGS_DEBU  *                                                                                                                                           
 CMAKE_MODULE_LINKER_FLAGS_MINS  *                                                                                                                                           
 CMAKE_MODULE_LINKER_FLAGS_RELE  *                                                                                                                                           
 CMAKE_MODULE_LINKER_FLAGS_RELW  *                                                                                                                                           
 CMAKE_NM                        */usr/bin/nm   
 CMAKE_OBJCOPY                   */usr/bin/objcopy                                                                                                                           
 CMAKE_OBJDUMP                   */usr/bin/objdump                                                                                                                           
 CMAKE_RANLIB                    */usr/bin/ranlib                                                                                                                            
 CMAKE_SHARED_LINKER_FLAGS       *                                                                                                                                           
 CMAKE_SHARED_LINKER_FLAGS_DEBU  *                                                                                                                                           
 CMAKE_SHARED_LINKER_FLAGS_MINS  *                                                                                                                                           
 CMAKE_SHARED_LINKER_FLAGS_RELE  *                                                                                                                                           
 CMAKE_SHARED_LINKER_FLAGS_RELW  *                                                                                                                                           
 CMAKE_SKIP_INSTALL_RPATH        *OFF                                                                                                                                        
 CMAKE_SKIP_RPATH                *OFF                                                                                                                                        
 CMAKE_STATIC_LINKER_FLAGS       *                                                                                                                                           
 CMAKE_STATIC_LINKER_FLAGS_DEBU  *                                                                                                                                           
 CMAKE_STATIC_LINKER_FLAGS_MINS  *                                                                                                                                           
 CMAKE_STATIC_LINKER_FLAGS_RELE  *                                                                                                                                           
 CMAKE_STATIC_LINKER_FLAGS_RELW  *                                                                                                                                           
 CMAKE_STRIP                     */usr/bin/strip                                                                                                                             
 CMAKE_VERBOSE_MAKEFILE          *OFF                                                                                                                                        
 PYTHON_INCLUDE_DIR              */usr/include/python2.7                                                                                                                     
 PYTHON_LIBRARY                  */usr/lib/x86_64-linux-gnu/libpython2.7.so                                                                                                  
 PYTHON_LIBRARY_DEBUG            *PYTHON_LIBRARY_DEBUG-NOTFOUND                                                                                                              
 PYTHON_SITEPACKAGES_DIR         *~/lib/python2.7/dist-packages                                                                                                              
 Qt5Core_DIR                     */usr/lib/x86_64-linux-gnu/cmake/Qt5Core                                                                                                    
 Qt5Gui_DIR                      */usr/lib/x86_64-linux-gnu/cmake/Qt5Gui                                                                                                     
 Qt5Network_DIR                  */usr/lib/x86_64-linux-gnu/cmake/Qt5Network                                                                                                 
 Qt5WebKit_DIR                   */usr/lib/x86_64-linux-gnu/cmake/Qt5WebKit                                                                                                  
 SWIG_DIR                        */usr/share/swig3.0                                                                                                                         
 SWIG_EXECUTABLE                 */usr/bin/swig3.0                                                                                                                           
 SWIG_VERSION                    *3.0.12                                                                                                                                     
 VTK_DIR                         */usr/lib/cmake/vtk-6.3                                                                                                                     
 wxWidgets_CONFIG_EXECUTABLE     */usr/bin/wx-config    

Press 'c'
Press 'g' to generate the makefiles and exit

make 
make install
