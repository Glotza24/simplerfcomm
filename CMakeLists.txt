cmake_minimum_required(VERSION 3.16)

project(btapp)

find_package(Qt6 REQUIRED COMPONENTS DBus)

qt_add_dbus_adaptor(BLUEZAGENT_ADAPTOR_SRC res/org.bluez.Agent1.xml bluezagent.h BluezAgent bluezagentadaptor BluezAgentAdaptor)
qt_add_dbus_interface(BLUEZMANAGER_INTERFACE_SRC res/org.bluez.AgentManager1.xml bluezagentmanager)
qt_add_dbus_interface(BLUEZADAPTER_INTERFACE_SRC res/org.bluez.Adapter1.xml bluezadapter)


set(SOURCES
    src/bt.cpp
    src/main.cpp
    ${BLUEZMANAGER_INTERFACE_SRC}
    ${BLUEZADAPTER_INTERFACE_SRC}
    ${BLUEZAGENT_ADAPTOR_SRC}
)

add_executable(simplerfcomm ${SOURCES})

target_link_libraries(simplerfcomm PUBLIC Qt6::DBus)

target_include_directories(simplerfcomm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)
