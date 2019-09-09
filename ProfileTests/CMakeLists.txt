FILE(GLOB PROFILE_SOURCE_FILES *.cpp)
FILE(GLOB PROFILE_HEADER_FILES *.h)

IF(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

ADD_EXECUTABLE(TexGenProfile ${PROFILE_SOURCE_FILES} ${PROFILE_HEADER_FILES})

#TARGET_LINK_LIBRARIES(TexGenProfile TexGenCore TexGenRenderer)


#IF(WIN32)
#	ADD_CUSTOM_COMMAND(TARGET TexGenProfile POST_BUILD COMMAND $(TargetPath) WORKING_DIRECTORY $(TargetDir))
#ENDIF(WIN32)

FIND_PACKAGE(VTK 6.3 REQUIRED NO_MODULE)
LINK_DIRECTORIES(${VTK_LIBRARY_DIRS} )
INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS} )

IF(WIN32)
	# Note ${VTK_LIBRARY_DIRS} are necessary for MSVC++, libs arn't found otherwise
	# Please do not remove unless the source of the problem is resolved
	TARGET_LINK_LIBRARIES( TexGenProfile
	TexGenCore
	TexGenRenderer
	opengl32 
	glu32
	${VTK_LIBRARIES}
	${wxWidgets_LIBRARIES}
	)
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../OctreeRefinement/Include)
ENDIF(WIN32)