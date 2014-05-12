include(FindPackageHandleStandardArgs)

set (LIBNAME
	ccl)

if(CCL_INCLUDE_DIRS AND CCL_LIBRARIES)
	set(CCL_FIND_QUIETLY TRUE)
endif(CCL_INCLUDE_DIRS AND CCL_LIBRARIES)

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	set(CCL_INCLUDE "$ENV{CCL_ROOT}include")
	if(${MSVC})
		set(CCL_LIB_DIRS "$ENV{CCL_ROOT}lib/msvc"
                                 "$ENV{PROGRAMFILES}/${LIBNAME}/lib/msvc")
                set(CCL_DEBUG_LIB_DIRS
                                 "$ENV{CCL_ROOT}lib/msvc/Debug"
                                 "$ENV{PROGRAMFILES}/${LIBNAME}/lib/msvc/Debug")
	elseif(${BORLAND)
		set(CCL_LIB_DIRS "$ENV{CCL_ROOT}lib/cbuilder"
                                 "$ENV{PROGRAMFILES}/${LIBNAME}/lib/cbuilder")
                set(CCL_DEBUG_LIB_DIRS
                                 "$ENV{CCL_ROOT}lib/cbuilder/Debug"
                                 "$ENV{PROGRAMFILES}/${LIBNAME}/lib/cbuilder/Debug")
	endif(${MSVC})
elseif(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
	set(CCL_INCLUDE /usr/include/${LIBNAME}
					/usr/local/include/${LIBNAME})
	if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64)
		set(CCL_LIB_DIRS /usr/lib64 /usr/local/lib64)
	else(${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64)
		set(CCL_LIB_DIRS /usr/lib /usr/local/lib)
	endif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64)
endif(${CMAKE_SYSTEM_NAME} STREQUAL Windows)

find_path(CCL_INCLUDE_DIR 
		NAMES ${LIBNAME}.hpp
		HINTS ${CCL_INCLUDE}
		DOC "The directory where headers files resides")

find_library(CCL_LIBRARIES
	   	NAMES ${LIBNAME}
		HINTS ${CCL_LIB_DIRS}
		DOC "The CCL library")

find_library(CCL_LIBRARY_DEBUG
                NAMES ${LIBNAME}
                HINTS ${CCL_DEBUG_LIB_DIRS}
		DOC "The CCL library (Debug)")

find_package_handle_standard_args(CCL 
		DEFAULT_MSG 
		CCL_LIBRARIES 
		CCL_INCLUDE_DIR)

message("${CCL_LIBRARY_DEBUG} ${CCL_LIBRARIES}")

if(CCL_LIBRARY_DEBUG AND CCL_LIBRARIES)
    set(CCL_LIBRARIES debug ${CCL_LIBRARY_DEBUG} 
                      optimized ${CCL_LIBRARIES})
endif(CCL_LIBRARY_DEBUG AND CCL_LIBRARIES)

MARK_AS_ADVANCED( CCL_FOUND )
