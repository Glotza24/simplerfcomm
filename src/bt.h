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

struct BtDevice
{
    std::string name;
    std::string address;

    bool paired;
    bool connected;

    qint32 rssi;
};

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

class BtDeviceDiscoveryService
{
public:
    BtDeviceDiscoveryService(OrgBluezAdapter1Interface &adapterInterface) : m_adapterInterface(adapterInterface)
    {
        EnumerateAvailableDevices();
    }

    /**
     * @brief Gets the currently available bluetooth devices.
     * @return A map of device paths to device information.
     */

    /**
     * @brief Starts the discovery of bluetooth devices.
     */
    void StartDiscovery() {}

    /**
     * @brief Stops the discovery of bluetooth devices.
     */
    void StopDiscovery() {}

private:
    void EnumerateAvailableDevices()
    {
        // Listen for device added/removed signals and update the devices map accordingly
    }

    void AddDevice(const QDBusObjectPath &path, const BtDevice &device)
    {
        m_devices[path] = device;

        // Listen to property changes of this device and update the device info in the devices map accordingly
    }

    void RemoveDevice(const QDBusObjectPath &path, const BtDevice &device)
    {
        m_devices.erase(path);
    }

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
        m_adapterInterface.StartDiscovery();
    }

    void DisableScan()
    {
        m_adapterInterface.StopDiscovery();
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
    void RegisterAsDefaultAgent()
    {
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
