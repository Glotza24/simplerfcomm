#ifndef BTAGENT_H_
#define BTAGENT_H_

#include <string>
#include <memory>
#include <map>

#include <QDBusObjectPath>
#include <QObject>
#include <QIODevice>
#include <QFuture>

#include "bluezadapter.h"
#include "bluezagentmanager.h"
#include "bluezagent.h"
#include "bluezobjectmanager.h"
#include "dbusproperties.h"

struct BtDevice
{
    std::string name;
    std::string address;

    bool paired;
    bool connected;

    qint32 rssi;
};

void RegisterMetaTypes();


class RfCommObserver
{
public:
    bool IsConnected() const
    {
        return m_connected;
    }

    void Disconnected()
    {
        m_connected = false;
    }

    void Connected(std::shared_ptr<QIODevice> device)
    {
        m_connected = true;

        OnConnected(device);
    }

    virtual void OnConnected(std::shared_ptr<QIODevice> device)
    {
    }

private:
    bool m_connected = false;
};

class BluetoothInterface
{
public:
    /**
     * @brief Virtual destructor for the Bluetooth adapter.
     */
    virtual ~BluetoothInterface() = default;

    /**
     * @brief Enables the bluetooth adapter
     */
    virtual void Enable() = 0;

    /**
     * @brief Aka air plane mode.
     */
    virtual void Disable() = 0;

    /**
     * @brief Opens an RFCOMM connection to the given device.
     * @param address The address of the device to connect to.
     * @return A shared pointer to the opened RFCOMM connection.
     */
    virtual std::shared_ptr<RfCommObserver> GetRfComm(const QString &address) = 0;
};

class BtDeviceDiscoveryService : public QObject
{
    Q_OBJECT
public:

    BtDeviceDiscoveryService(OrgBluezAdapter1Interface &adapterInterface, QObject *parent = nullptr);

    /**
     * @brief Gets the currently available bluetooth devices.
     * @return A map of device paths to device information.
     */

    /**
     * @brief Starts the discovery of bluetooth devices.
     */
    void StartDiscovery()
    {
        QDBusPendingReply<> reply = m_adapterInterface.StartDiscovery();

        // todo: handle reply
    }

    /**
     * @brief Stops the discovery of bluetooth devices.
     */
    void StopDiscovery()
    {
        QDBusPendingReply<> reply = m_adapterInterface.StopDiscovery();

        // todo: handle reply
    }

private Q_SLOTS:
    void OnDeviceAdded(const QDBusObjectPath &path, const QVariantMapMap &interfaces);

    void OnDeviceRemoved(const QDBusObjectPath &path, const QStringList &interfaces);

private:
    void EnumerateAvailableDevices()
    {
        // Listen for device added/removed signals and update the devices map accordingly
        QDBusPendingReply<DBusManagerStruct> reply = m_objectManagerInterface.GetManagedObjects();

        auto future = QtFuture::connect(new QDBusPendingCallWatcher(reply), &QDBusPendingCallWatcher::finished).then([this](QDBusPendingCallWatcher *watcher)                                                                                                   {
            const QDBusPendingReply<DBusManagerStruct> &reply = *watcher;
            watcher->deleteLater();
            if (watcher->isError()) {
                qDebug() << "Failed to get managed objects:" << watcher->error().message();
            }
            else {
                qDebug() << "Successfully got managed objects";
                const DBusManagerStruct &objects = reply.value();

                for (const auto &[path, interfaces] : objects.toStdMap()) {
                    if (interfaces.contains("org.bluez.Device1")) {
                        QVariantMap deviceProperties = interfaces["org.bluez.Device1"];
                        BtDevice device;
                        UpdateDeviceFromVariantMap(device, deviceProperties);

                        AddDevice(path, device);
                    }
                }
            } });
    }

    void UpdateDeviceFromVariantMap(BtDevice& device, const QVariantMap &properties)
    {
        if (properties.contains("Name")) {
            device.name = properties["Name"].toString().toStdString();
        }
        if (properties.contains("Address")) {
            device.address = properties["Address"].toString().toStdString();
        }
        if (properties.contains("Paired")) {
            device.paired = properties["Paired"].toBool();
        }
        if (properties.contains("Connected")) {
            device.connected = properties["Connected"].toBool();
        }
        if (properties.contains("RSSI")) {
            device.rssi = properties["RSSI"].toInt();
        }
    }

    void AddDevice(const QDBusObjectPath &path, const BtDevice &device)
    {
        m_devices[path] = device;

        // Listen to property changes of this device and update the device info in the devices map accordingly
        OrgFreedesktopDBusPropertiesInterface propertiesInterface("org.bluez", path.path(), QDBusConnection::systemBus());
    }

    void RemoveDevice(const QDBusObjectPath &path, const BtDevice &device)
    {
        m_devices.erase(path);
    }

    OrgFreedesktopDBusObjectManagerInterface m_objectManagerInterface;
    OrgBluezAdapter1Interface &m_adapterInterface;
    std::map<QDBusObjectPath, BtDevice> m_devices;
};

class Bluetooth : public BluetoothInterface
{
public:
    Bluetooth() : m_adapterInterface("org.bluez", "/org/bluez/hci0", QDBusConnection::systemBus()),
                  m_agentManagerInterface("org.bluez", "/org/bluez", QDBusConnection::systemBus()),
                  m_deviceDiscoveryService(m_adapterInterface)
    {
        RegisterAsDefaultAgent();
    }

    BtDeviceDiscoveryService &GetDeviceDiscoveryService()
    {
        return m_deviceDiscoveryService;
    }

    void EnableScan()
    {
        m_deviceDiscoveryService.StartDiscovery();
    }

    void DisableScan()
    {
        m_deviceDiscoveryService.StopDiscovery();
    }

    void Enable() override
    {
        m_adapterInterface.setPowered(true);
    }

    void Disable() override
    {
        m_adapterInterface.setPowered(false);
    }

    std::shared_ptr<RfCommObserver> GetRfComm(const QString &address) override
    {
        return nullptr;
    }

private:
    void SetupAgent() {
        
    }

    void RegisterAsDefaultAgent()
    {
        SetupAgent();

        QDBusPendingReply<> reply = m_agentManagerInterface.RegisterAgent(QDBusObjectPath("/org/rfcomm/agent"), "DisplayYesNo");

        auto future = QtFuture::connect(new QDBusPendingCallWatcher(reply), &QDBusPendingCallWatcher::finished).then([](QDBusPendingCallWatcher *watcher)
                                                                                                                     {
            watcher->deleteLater();
            if (watcher->isError()) {
                qDebug() << "Failed to register agent:" << watcher->error().message();
            }
            else {
                qDebug() << "Agent registered successfully";
            } });
    }

    OrgBluezAgentManager1Interface m_agentManagerInterface;
    OrgBluezAdapter1Interface m_adapterInterface;
    BtDeviceDiscoveryService m_deviceDiscoveryService;
};

#endif /* BTAGENT_H_ */
