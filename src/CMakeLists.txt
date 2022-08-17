cmake_minimum_required(VERSION 3.1.3)

option(OSX_PACKAGE "Create OSX package" OFF)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

if(APPLE)
	# build universal binaries by default
	set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif()

if(MSVC)
	if (NOT ENABLE_EXCEPTIONS)
		# support for C/C++ conformant preprocessor in MSVC was added starting with
		# version Visual Studio 2019 version 16.6 (equivalent to 1926)
		# https://devblogs.microsoft.com/cppblog/announcing-full-support-for-a-c-c-conformant-preprocessor-in-msvc/
		if(MSVC_VERSION GREATER_EQUAL 1926)
			remove_definitions("/EHsc /Zc:preprocessor")
		else()
			remove_definitions("/EHsc /experimental:preprocessor")
		endif()
	else()
		if(MSVC_VERSION GREATER_EQUAL 1926)
			add_definitions("/permissive /EHsc /Zc:preprocessor")
		else()
			add_definitions("/permissive /EHsc /experimental:preprocessor")
		endif()
	endif()
endif()

SET(CMAKE_EXE_LINKER_FLAGS  "/FORCE")

FILE(GLOB_RECURSE SRC_LIST  *.cpp *_impl.hpp */*_impl.hpp)

FILE(GLOB_RECURSE HEADERS_LIST ${CMAKE_SOURCE_DIR}/include/libm2k/*.hpp)
FILE(GLOB_RECURSE HEADERS_LIST_LV ${CMAKE_SOURCE_DIR}/include/libm2k/*.h)

if (ENABLE_TOOLS)
	FILE(GLOB_RECURSE COMMUNICATION_SRC_LIST ../tools/communication/src/*.cpp)
	list(APPEND SRC_LIST ${COMMUNICATION_SRC_LIST})
else()
	foreach(filepath ${HEADERS_LIST})
		if(${filepath} MATCHES "${CMAKE_SOURCE_DIR}/include/libm2k/tools/*")
			list(REMOVE_ITEM HEADERS_LIST ${filepath})
		endif()
	endforeach()
endif()

#Create the libm2k library
add_library(${PROJECT_NAME} SHARED
			${SRC_LIST}
			${HEADERS_LIST}
			${DEVICES})

# build with LIBM2K_EXPORTS defined in order to export everything that is marked with LIBM2K_API.
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBM2K_EXPORTS)
if (ENABLE_LOG)
	target_compile_definitions(${PROJECT_NAME} PUBLIC LIBM2K_ENABLE_LOG)
endif()
if (MSVC)
	add_definitions("/MD /wd4250 /wd4251 /wd4275 /wd5105")
endif()

if(NOT MSVC)
	target_compile_options(${PROJECT_NAME}
			PRIVATE
			-Wall
			-Wextra)
	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		target_compile_options(${PROJECT_NAME}
				PRIVATE
				-Wreturn-stack-address)
	elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
		target_compile_options(${PROJECT_NAME}
				PRIVATE
				-Wreturn-local-addr)
	endif()
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        ${IIO_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if (NOT WIN32)
	find_library(PTHREAD_LIBRARIES pthread)
	if (PTHREAD_LIBRARIES)
                target_link_libraries(libm2k PRIVATE ${PTHREAD_LIBRARIES})
	endif()
endif()


#Link the required dependencies
target_link_libraries(${PROJECT_NAME}
		PUBLIC
		${IIO_LIBRARIES}
		)
if (ENABLE_LOG)
	target_link_libraries(${PROJECT_NAME}
			PUBLIC
			glog::glog
			)
endif()


#Take care of the library properties
set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION}
)

if (MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME libm2k)
endif()


if (APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES FRAMEWORK FALSE)
	set(CMAKE_MACOSX_RPATH ON)
	if (OSX_PACKAGE)
		set_target_properties(${PROJECT_NAME} PROPERTIES FRAMEWORK TRUE)
		get_filename_component(OSX_INSTALL_FRAMEWORKSDIR "${OSX_INSTALL_FRAMEWORKSDIR}" REALPATH BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
		set(SKIP_INSTALL_ALL ${OSX_PACKAGE})
	endif()
endif()

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	FRAMEWORK DESTINATION "${OSX_INSTALL_FRAMEWORKSDIR}"
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/libm2k/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
	FILES_MATCHING PATTERN "*.hpp")
install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/version.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# Add alias to libm2k as libm2k::libm2k
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake ${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
