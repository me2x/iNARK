cmake_minimum_required(VERSION 3.0.2)
project(iNARK)
find_package(Qt4 4.4.3 REQUIRED)


SET(GRAPHICAL_SRCS
maininterface.cpp
mainwindow.cpp
)

add_library (graphical ${GRAPHICAL_SRCS})



target_link_libraries(graphical ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})