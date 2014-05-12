set(PROJECT flexo_viewer)

project(${PROJECT})

cmake_minimum_required(VERSION 2.6)

set(CMAKE_include_directories_BEFORE ON)

find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
	src/image_dialog.cpp
	src/table_model.cpp
	src/image_model.cpp
	src/gnuplot_console.cpp 
	src/data_seaker.cpp
	src/gnu_ploter.cpp
	src/ploter.cpp
	src/flexo_table_view.cpp
	src/search_widget.cpp
	src/tab_panel.cpp
	src/interval_combo_model.cpp
	src/new_curve_widget.cpp
	src/new_range_widget.cpp
	src/plot_dialog.cpp
	src/headers_model.cpp
	src/data_model.cpp
	src/flexo_viewer.cpp
	src/fits_model.cpp
	src/data_plot_model.cpp
	src/data_plot_widget.cpp
	src/data_delegate.cpp
	main.cpp)

set(HEADERS 
	src/image_dialog.h
	src/table_model.h
	src/image_model.h
	src/gnuplot_console.h
	src/data_seaker.h
	src/gnu_ploter.h
	src/ploter.h
	src/flexo_table_view.h
	src/search_widget.h
	src/tab_panel.h
	src/interval_combo_model.h
	src/new_curve_widget.h
	src/new_range_widget.h
	src/plot_dialog.h
	src/headers_model.h
	src/data_model.h
	src/flexo_viewer.h
	src/data_plot_model.h
	src/data_plot_widget.h
	src/data_delegate.h)
if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    set (BOOST_COMPONENTS
		filesystem
		system
		regex)
    set (Boost_USE_STATIC_LIBS ON)
    set (Boost_USE_MULTITHREADED ON) 
    find_package (Boost COMPONENTS ${BOOST_COMPONENTS})
    include_directories(${Boost_INCLUDE_DIRS})
    set(LIBRARIES ${LIBRARIES} ${Boost_LIBRARIES})	
    if(${MSVC})
		message(STATUS "Compiler: MSVC, version: " ${MSVC_VERSION})
		set(COMPILER_FLAGS "/EHa")
		set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MD /Zi  /Ob0 /Od /RTC1 ${COMPILER_FLAGS}")
		set(CMAKE_CXX_FLAGS_MINSIZEREL     "/MD  /O1 /Ob1 /D NDEBUG ${COMPILER_FLAGS}")
		set(CMAKE_CXX_FLAGS_RELEASE        "/MD /O2 /Ob2 /D NDEBUG ${COMPILER_FLAGS}")
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MD /Zi /O2 /Ob1 /D NDEBUG ${COMPILER_FLAGS}")
		set(CMAKE_MODULE_LINKER_FLAGS 
			${CMAKE_MODULE_LINKER_FLAGS_INIT} 
			$ENV{LDFLAGS} 
			/NODEFAULLIB:msvcrt)
	endif(${MSVC})
        set_source_files_properties(src/data_plot_model.h
				PROPERTIES COMPILE_DEFINITIONS _WIN32)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
	set(LIBRARIES ${LIBRARIES} 
	stdc++
        boost_filesystem
	boost_system
	boost_regex
        )
endif(${CMAKE_SYSTEM_NAME} STREQUAL Windows)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

find_package(CCL REQUIRED)
find_package(Fitsio REQUIRED)
set (LIBRARIES ${LIBRARIES}
    ${CCL_LIBRARIES}
    ${Fitsio_LIBRARIES})
include_directories(${CCL_INCLUDE_DIR}
                    ${Fitsio_INCLUDE_DIR})

SET(RCCS flexo_viewer.qrc)

QT4_ADD_RESOURCES(RCC_SOURCES ${RCCS})

qt4_automoc(${SOURCES})

add_executable(${PROJECT} WIN32 ${SOURCES} ${HEADERS} ${RCC_SOURCES})

target_link_libraries(${PROJECT} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTMAIN_LIBRARY} ${LIBRARIES})

add_subdirectory(src)

set(BIN_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/bin)
install(TARGETS ${PROJECT} DESTINATION ${BIN_INSTALL_PATH})

set (CMAKE_BUILD_TYPE Debug)
