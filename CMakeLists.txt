cmake_minimum_required(VERSION 3.16)

project(RealEstateCRM-Qt VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

  qt_add_executable(RealEstateCRM-Qt
          MANUAL_FINALIZATION
          ${PROJECT_SOURCES}
          AdminAccount.h
          Agent.h
          Client.h
          Contract.h
          CRMSystem.h
          DatabaseManager.h
          Agent.cpp
          Date.h
          Exceptions.h
          EmployeeAccount.h
          Inspection.h
          UserAccount.h
          Property.h
          Client.cpp
          Contract.cpp
          CRMSystem.cpp
          DatabaseManager.cpp
          Date.cpp
          Inspection.cpp
          Property.cpp
          sqlite3.h
          sqlite3.c
          agentdialog.h
          agentdialog.cpp
          agentdialog.ui
          clientdialog.h clientdialog.cpp clientdialog.ui
          propertydialog.h propertydialog.cpp propertydialog.ui
          contractdialog.h contractdialog.cpp contractdialog.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RealEstateCRM-Qt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(RealEstateCRM-Qt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RealEstateCRM-Qt
            $
        )
    endif()
endif()

target_link_libraries(RealEstateCRM-Qt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RealEstateCRM-Qt)
endif()
set_target_properties(RealEstateCRM-Qt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS RealEstateCRM-Qt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RealEstateCRM-Qt)
endif()
