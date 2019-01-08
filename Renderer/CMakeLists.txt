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
		vtkGraphics
		vtkFiltering
		vtkCommon
		vtkRendering
		vtkHybrid
		vtkIO
		vtkWidgets
		vtkzlib
		vtkfreetype
		vtkftgl
		vtkpng
		vtkImaging
		vtkjpeg
		vtktiff
		vtksys
		vtkexpat
		vtkNetCDF
		vtkhdf5
		vtkmetaio
	)
ENDIF(UNIX)

FIND_PACKAGE(VTK REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../OctreeRefinement/Include)
INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS})

# Note ${VTK_LIBRARY_DIRS} are necessary for MSVC++, libs arn't found otherwise
# Please do not remove unless the source of the problem is resolved

IF(WIN32)
	TARGET_LINK_LIBRARIES(TexGenRenderer
		TexGenCore opengl32 glu32
		${VTK_LIBRARY_DIRS}/vtkGraphics.lib
		${VTK_LIBRARY_DIRS}/vtkFiltering.lib
		${VTK_LIBRARY_DIRS}/vtkCommon.lib
		${VTK_LIBRARY_DIRS}/vtkRendering.lib
		${VTK_LIBRARY_DIRS}/vtkHybrid.lib
		${VTK_LIBRARY_DIRS}/vtkIO.lib
		${VTK_LIBRARY_DIRS}/vtkWidgets.lib
		${VTK_LIBRARY_DIRS}/vtkzlib.lib
		${VTK_LIBRARY_DIRS}/vtkfreetype.lib
		${VTK_LIBRARY_DIRS}/vtkftgl.lib
		${VTK_LIBRARY_DIRS}/vtkpng.lib
		${VTK_LIBRARY_DIRS}/vtkImaging.lib
		${VTK_LIBRARY_DIRS}/vtkjpeg.lib
		${VTK_LIBRARY_DIRS}/vtktiff.lib
		${VTK_LIBRARY_DIRS}/vtksys.lib
		${VTK_LIBRARY_DIRS}/vtkexpat.lib
		${VTK_LIBRARY_DIRS}/vtknetcdf.lib
		${VTK_LIBRARY_DIRS}/vtkhdf5.lib
		${VTK_LIBRARY_DIRS}/vtkmetaio.lib
	)
ENDIF(WIN32)
