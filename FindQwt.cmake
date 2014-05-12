include(FindPackageHandleStandardArgs)

if(Qwt_INCLUDE_DIRS AND Qwt_LIBRARIES)
	set(Qwt_FIND_QUIETLY TRUE)
endif(Qwt_INCLUDE_DIRS AND Qwt_LIBRARIES)

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	set(Qwt_INCLUDE "$ENV{Qwt_ROOT}/include")
	set(Qwt_LIB_DIRS "$ENV{Qwt_ROOT}/lib"
                     "$ENV{PATH}/lib")
    set(Qwt_DEBUG_LIB_DIRS
                     "$ENV{Qwt_ROOT}/lib"
                     "$ENV{PATH}/lib")

	find_library(Qwt_LIBRARIES
	   	NAMES qwt
		HINTS ${Qwt_LIB_DIRS}
		DOC "The Qwt library")

	find_library(Qwt_DEBUG_LIBRARIES
	   	NAMES qwtd
		HINTS ${Qwt_DEBUG_LIB_DIRS}
		DOC "The Qwt library(Debug)")

	if(Qwt_DEBUG_LIBRARIES AND Qwt_LIBRARIES)
    	set(Qwt_LIBRARIES debug ${Qwt_DEBUG_LIBRARIES} 
                      optimized ${Qwt_LIBRARIES})
	endif(Qwt_DEBUG_LIBRARIES AND Qwt_LIBRARIES)

elseif(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
	set(Qwt_INCLUDE /usr/include/qwt
				/usr/local/include/qwt)
	if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64)
		set(Qwt_LIB_DIRS /usr/lib64 /usr/local/lib64)
	else(${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64)
		set(Qwt_LIB_DIRS /usr/lib /usr/local/lib)
	endif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64)

	find_library(Qwt_LIBRARIES
	   	NAMES qwt
		HINTS ${Qwt_LIB_DIRS}
		DOC "The Qwt library")

endif(${CMAKE_SYSTEM_NAME} STREQUAL Windows)

find_path(Qwt_INCLUDE_DIR 
		NAMES qwt.h
		HINTS ${Qwt_INCLUDE}
		DOC "The directory where headers files resides")

find_package_handle_standard_args(Qwt 
		DEFAULT_MSG 
		Qwt_LIBRARIES 
		Qwt_INCLUDE_DIR)

MARK_AS_ADVANCED( Qwt_FOUND )
