cmake_minimum_required (VERSION 3.1)

project (CSVFilter)
set (CSVFILTER_VERSION_MAJOR 0)
set (CSVFILTER_VERSION_MINOR 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
enable_testing()

FIND_PROGRAM(DATE_EXECUTABLE date)
FIND_PROGRAM(TCLSH_EXECUTABLE tclsh)
FIND_PROGRAM(AWK_EXECUTABLE awk)
MESSAGE("DATE_EXECUTABLE is " ${DATE_EXECUTABLE})
MESSAGE("TCLSH_EXECUTABLE is " ${TCLSH_EXECUTABLE})
MESSAGE("AWK_EXECUTABLE is " ${AWK_EXECUTABLE})

string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
if ( DATE_EXECUTABLE )
    # CMake's facilities to format the current date don't support the format
    # expected for man pages. Where possible we'll use the date command

    execute_process(COMMAND date "+%d %b %Y" OUTPUT_VARIABLE BUILD_DATE)
    string(REPLACE "\n" "" BUILD_DATE "${BUILD_DATE}")
else()
    MESSAGE("Cannot find the date executable - the date in the man page will  not be formatted correctly")
endif ()

CONFIGURE_FILE(src/app/configure.h.in ${PROJECT_BINARY_DIR}/include/configure.h)
CONFIGURE_FILE(man/man1/csvfilter.1.in 
               ${PROJECT_BINARY_DIR}/man/man1/csvfilter.1)
install(FILES ${PROJECT_BINARY_DIR}/man/man1/csvfilter.1
        DESTINATION share/man/man1) 

# detect popt
find_package(Popt REQUIRED)
include_directories(SYSTEM ${POPT_INCLUDES})

# Turn on c++11 features. Note that this doesn't work for Apple's clang compiler
# at the moment (cmake 3.1.0), so we do it manually.
if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(CMAKE_CXX_FLAGS 
        "-std=c++11 ${CMAKE_CXX_FLAGS}")
else()
    set (CMAKE_CXX_STANDARD "11")
    set(CMAKE_CXX_STANDARD_REQUIRED on)
endif()

# Enable compiler warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS 
        "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif()

include_directories(${PROJECT_BINARY_DIR}/include ${PROJECT_SOURCE_DIR}/src)

add_library(applib 
            src/app/application.cc
            src/app/cmdOptions.cc
            src/app/fileReader.cc
            src/app/lineParser.cc
            src/app/field.cc
            src/app/headers.cc
            src/app/filterExpression/lexToken.cc
            src/app/filterExpression/lexer.cc
            src/app/filterExpression/parseTree.cc
            src/app/filterExpression/operand.cc
            src/app/filterExpression/binaryOperator.cc
            src/app/filterExpression/unaryOperator.cc
            src/app/filterExpression/expression.cc
            src/app/filterExpression/variant.cc 
            src/app/filterExpression/parseError.cc)

add_executable(csvfilter src/app/main.cc)
target_link_libraries(csvfilter applib ${POPT_LIBRARIES})
install(TARGETS csvfilter DESTINATION bin)

#Doxygen API docs
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${PROJECT_SOURCE_DIR}/doxygen/Doxyfile.in
                   ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
                      ${DOXYGEN_EXECUTABLE}
                      ${PROJECT_BINARY_DIR}/Doxyfile
                      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)


# Automated tests
add_executable(unitTest src/test/main.cc 
                        src/test/test.cc
                        src/test/cmdOptions.cc
                        src/test/field.cc
                        src/test/lineParser.cc
                        src/test/headers.cc
                        src/test/filterExpression/lexer.cc
                        src/test/filterExpression/expression.cc)
target_link_libraries(unitTest applib ${POPT_LIBRARIES})

add_test(unitTest bin/unitTest)

if ( TCLSH_EXECUTABLE )
    CONFIGURE_FILE(test/appTest.in ${PROJECT_BINARY_DIR}/test/appTest)
    add_test(appTest ${PROJECT_BINARY_DIR}/test/appTest)
else()
    MESSAGE("Application tests are disabled - they require tclsh")
endif()

if ( TCLSH_EXECUTABLE AND AWK_EXECUTABLE )
    CONFIGURE_FILE(perfTest/perfTest.in ${PROJECT_BINARY_DIR}/perfTest/perfTest)
    add_custom_target(perfTest
                 ${PROJECT_BINARY_DIR}/perfTest/perfTest
                 DEPENDS ${PROJECT_BINARY_DIR}/bin/csvfilter
                 COMMENT "Run basic performance tests - this may take some time"
                 VERBATIM)
else()
    MESSAGE("Performance tests are disabled - they require tclsh and awk")
endif()
