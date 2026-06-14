cmake_minimum_required(VERSION 3.16)

project(ChemFluidAnalysisQt VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(ChemFluidAnalysisQt
    src/main.cpp
    src/mainwindow.cpp
    include/mainwindow.h
    include/DatabaseManager.h
    third_party/sqlite3/sqlite3.h
    third_party/sqlite3/sqlite3.c
)

target_include_directories(ChemFluidAnalysisQt PRIVATE
    include
    third_party/sqlite3
)

target_link_libraries(ChemFluidAnalysisQt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(ChemFluidAnalysisQt PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ChemFluidAnalysisQt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
