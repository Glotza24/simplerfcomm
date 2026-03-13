cmake_minimum_required(VERSION 3.16)

project(simplerfcomm)

find_package(Qt6 REQUIRED COMPONENTS DBus)

qt_standard_project_setup()

qt_add_dbus_adaptor(DBUS_SRCS res/org.bluez.Agent1.xml bluezagent.h BluezAgent bluezagentadaptor BluezAgentAdaptor)
qt_add_dbus_interface(DBUS_SRCS res/org.bluez.AgentManager1.xml bluezagentmanager)
qt_add_dbus_interface(DBUS_SRCS res/org.bluez.Adapter1.xml bluezadapter)
qt_add_dbus_interface(DBUS_SRCS res/org.freedesktop.DBus.Properties.xml dbusproperties)

set_source_files_properties(res/org.freedesktop.DBus.ObjectManager.xml PROPERTIES INCLUDE "bluezqt_dbustypes.h")
qt_add_dbus_interface(DBUS_SRCS res/org.freedesktop.DBus.ObjectManager.xml bluezobjectmanager)

set(SOURCES
    src/bt.cpp
    src/main.cpp
    ${DBUS_SRCS}
)

add_executable(simplerfcomm ${SOURCES})

target_link_libraries(simplerfcomm PUBLIC Qt6::DBus)

target_include_directories(simplerfcomm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)
