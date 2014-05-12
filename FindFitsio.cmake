include(FindPackageHandleStandardArgs)

if(Fitsio_INCLUDE_DIRS AND Fitsio_LIBRARIES)
	set(Fitsio_FIND_QUIETLY TRUE)
endif(Fitsio_INCLUDE_DIRS AND Fitsio_LIBRARIES)

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
	set(Fitsio_INCLUDE "$ENV{Fitsio_ROOT}")
	if(${MSVC})
		set(Fitsio_LIB_DIRS "$ENV{Fitsio_ROOT}")
	elseif(${BORLAND)
		set(Fitsio_LIB_DIRS "$ENV{Fitsio_ROOT}cbuilder")
	endif(${MSVC})
elseif(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
	set(Fitsio_INCLUDE /usr/include/cfitsio /usr/local/include/cfitsio)
	if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64)
		set(Fitsio_LIB_DIRS /usr/lib64 /usr/local/lib64)
	else(${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64)
		set(Fitsio_LIB_DIRS /usr/lib /usr/local/lib)
	endif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL x86_64)
endif(${CMAKE_SYSTEM_NAME} STREQUAL Windows)

find_path(Fitsio_INCLUDE_DIR 
		NAMES fitsio.h 
		HINTS ${Fitsio_INCLUDE}
		DOC "The directory where headers files resides")

find_library(Fitsio_LIBRARIES
	   	NAMES cfitsio 
		HINTS ${Fitsio_LIB_DIRS}
		DOC "The Fitsio library")

find_package_handle_standard_args(Fitsio 
		DEFAULT_MSG 
		Fitsio_LIBRARIES 
		Fitsio_INCLUDE_DIR)

MARK_AS_ADVANCED( Fitsio_FOUND )
