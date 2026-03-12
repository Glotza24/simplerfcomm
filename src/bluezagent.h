#ifndef BLUEZAGENT_H_
#define BLUEZAGENT_H_

#include <QObject>

class BluezAgent : public QObject
{
    Q_OBJECT

public:
    BluezAgent(QObject *parent = nullptr) : QObject(parent) {}

public Q_SLOTS:
    void AuthorizeService(const QDBusObjectPath &in0, const QString &in1)
    {
    }
    void Cancel()
    {
    }
    void DisplayPasskey(const QDBusObjectPath &in0, uint in1, ushort in2)
    {
    }
    void DisplayPinCode(const QDBusObjectPath &in0, const QString &in1)
    {
    }
    void Release()
    {
    }
    void RequestAuthorization(const QDBusObjectPath &in0)
    {
    }
    void RequestConfirmation(const QDBusObjectPath &in0, uint in1) {

    }
    uint RequestPasskey(const QDBusObjectPath &in0) {
        return 0xaffe;
    }
    QString RequestPinCode(const QDBusObjectPath &in0) {
        return "1234";
    }
};

#endif /* BLUEZAGENT_H_ */
