FILE(GLOB RENDERER_SOURCE_FILES *.cpp)
FILE(GLOB RENDERER_HEADER_FILES *.h)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

ADD_LIBRARY(TexGenRenderer STATIC ${RENDERER_SOURCE_FILES} ${RENDERER_HEADER_FILES})
# The header files are not strictly necessary but it adds them to the project IDE making development easier

# Make use of precompiled headers if running on windows (note this probably only applies to MSVC, so it might screw things up for non-MSVC windows builds)
# GCC supports precompiled headers on linux too, but not sure how to activate it (only really necessary for somebody who is actively developing under linux)
IF(WIN32)
	SET_SOURCE_FILES_PROPERTIES(${RENDERER_SOURCE_FILES} PROPERTIES COMPILE_FLAGS /Yu"PrecompiledHeaders.h")
	SET_SOURCE_FILES_PROPERTIES(PrecompiledHeaders.cpp PROPERTIES COMPILE_FLAGS /Yc"PrecompiledHeaders.h")
ENDIF(WIN32)

IF(UNIX)
	SET_TARGET_PROPERTIES(TexGenRenderer PROPERTIES COMPILE_FLAGS -Wno-deprecated)
	SET_TARGET_PROPERTIES(TexGenRenderer PROPERTIES LINK_FLAGS "-Wl,-rpath,${VTK_DIR}/bin -L${VTK_DIR}/bin")
	TARGET_LINK_LIBRARIES(TexGenRenderer
		TexGenCore
		${VTK_LIBRARIES}
		#vtkGraphics
		#vtkFiltering
		#vtkCommon
		#vtkRendering
		#vtkHybrid
		#vtkIO
		#vtkWidgets
		#vtkzlib
		#vtkfreetype
		#vtkftgl
		#vtkpng
		#vtkImaging
		#vtkjpeg
		#vtktiff
		#vtksys
		#vtkexpat
		#vtkNetCDF
		#vtkhdf5
		#vtkmetaio
	)
ENDIF(UNIX)

FIND_PACKAGE(VTK 6.3 REQUIRED NO_MODULE)

INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS})
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS ${VTK_DEFINITIONS})

# Note ${VTK_LIBRARY_DIRS} are necessary for MSVC++, libs arn't found otherwise
# Please do not remove unless the source of the problem is resolved

IF(WIN32)
	TARGET_LINK_LIBRARIES(TexGenRenderer
		TexGenCore opengl32 glu32
		${VTK_LIBRARIES}
		#${VTK_LIBRARY_DIRS}/vtkGraphics
		#${VTK_LIBRARY_DIRS}/vtkFiltering
		#${VTK_LIBRARY_DIRS}/vtkCommon
		#${VTK_LIBRARY_DIRS}/vtkRendering
		#${VTK_LIBRARY_DIRS}/vtkHybrid
		#${VTK_LIBRARY_DIRS}/vtkIO
		#${VTK_LIBRARY_DIRS}/vtkWidgets
		#${VTK_LIBRARY_DIRS}/vtkzlib
		#${VTK_LIBRARY_DIRS}/vtkfreetype
		#${VTK_LIBRARY_DIRS}/vtkftgl
		#${VTK_LIBRARY_DIRS}/vtkpng
		#${VTK_LIBRARY_DIRS}/vtkImaging
		#${VTK_LIBRARY_DIRS}/vtkjpeg
		#${VTK_LIBRARY_DIRS}/vtktiff
		#${VTK_LIBRARY_DIRS}/vtksys
		#${VTK_LIBRARY_DIRS}/vtkexpat
		#${VTK_LIBRARY_DIRS}/vtknetcdf
		#${VTK_LIBRARY_DIRS}/vtkhdf5
		#${VTK_LIBRARY_DIRS}/vtkmetaio
	)
ENDIF(WIN32)
