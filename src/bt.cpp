#include "bt.h"

BtDeviceDiscoveryService::BtDeviceDiscoveryService(OrgBluezAdapter1Interface &adapterInterface, QObject *parent) : QObject(parent), m_objectManagerInterface("org.bluez", "/", QDBusConnection::systemBus()), m_adapterInterface(adapterInterface)
{
    EnumerateAvailableDevices();

    connect(&m_objectManagerInterface, &OrgFreedesktopDBusObjectManagerInterface::InterfacesAdded, this, &BtDeviceDiscoveryService::OnDeviceAdded);
    connect(&m_objectManagerInterface, &OrgFreedesktopDBusObjectManagerInterface::InterfacesRemoved, this, &BtDeviceDiscoveryService::OnDeviceRemoved);
}

void BtDeviceDiscoveryService::OnDeviceAdded(const QDBusObjectPath &path, const QVariantMapMap &interfaces)
{
}

void BtDeviceDiscoveryService::OnDeviceRemoved(const QDBusObjectPath &path, const QStringList &interfaces)
{
}

void RegisterMetaTypes()
{
    qDBusRegisterMetaType<DBusManagerStruct>();
    qRegisterMetaType<QVariantMapMap>("QVariantMapMap");
    qDBusRegisterMetaType<QVariantMapMap>();
}
