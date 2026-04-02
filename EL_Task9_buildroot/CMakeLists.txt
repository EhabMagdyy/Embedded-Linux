cmake_minimum_required(VERSION 3.16)

project(Task02_Calculator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appTask02_Calculator
    main.cpp
)

qt_add_qml_module(appTask02_Calculator
    URI Task02_Calculator
    QML_FILES
        Main.qml
        QML_FILES MButton.qml
)

set_target_properties(appTask02_Calculator PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appTask02_Calculator
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appTask02_Calculator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
