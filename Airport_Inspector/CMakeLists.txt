cmake_minimum_required(VERSION 3.5)

project(Airport_Inspector VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Sql Concurrent Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Concurrent Charts)

set(PROJECT_SOURCES
        ${CMAKE_SOURCE_DIR}/source/main.cpp
        ${CMAKE_SOURCE_DIR}/source/mainwindow.cpp
        ${CMAKE_SOURCE_DIR}/headers/mainwindow.h
        ${CMAKE_SOURCE_DIR}/source/mainwindow.ui
        ${CMAKE_SOURCE_DIR}/source/about.cpp
        ${CMAKE_SOURCE_DIR}/headers/about.h
        ${CMAKE_SOURCE_DIR}/source/about.ui
        ${CMAKE_SOURCE_DIR}/resources/resources.qrc
        ${CMAKE_SOURCE_DIR}/headers/database.h
        ${CMAKE_SOURCE_DIR}/source/database.cpp
        ${CMAKE_SOURCE_DIR}/headers/enumerations.h
        ${CMAKE_SOURCE_DIR}/headers/statwindow.h
        ${CMAKE_SOURCE_DIR}/source/statwindow.cpp
        ${CMAKE_SOURCE_DIR}/source/statwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Airport_Inspector
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Airport_Inspector APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Airport_Inspector SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Airport_Inspector
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/PG_Libs)

include_directories("${PROJECT_SOURCE_DIR}/PG_Libs")

target_link_libraries(Airport_Inspector PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                                Qt${QT_VERSION_MAJOR}::Concurrent
                                                Qt${QT_VERSION_MAJOR}::Charts
                                                Qt${QT_VERSION_MAJOR}::Sql
                                                libcrypto-3-x64
                                                libiconv-2
                                                libintl-9
                                                libpq
                                                libssl-3-x64
                                            )

set_target_properties(Airport_Inspector PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

file(COPY PG_Libs DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

install(TARGETS Airport_Inspector
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Airport_Inspector)
endif()
