#include "hostinfoworker.h"

#include "netutil.h"

HostInfoWorker::HostInfoWorker(QObject *parent) :
    QObject(parent),
    m_aborted(false)
{
}

void HostInfoWorker::run()
{
    QSysInfo::machineHostName();  // Initialize ws2_32.dll

    do {
        const QString address = dequeueAddress();

        if (!address.isEmpty()) {
            const QString hostName = NetUtil::getHostName(address);

            if (m_aborted) break;

            emit lookupFinished(address, hostName);
        }
    } while (!m_aborted);
}

QString HostInfoWorker::dequeueAddress()
{
    QMutexLocker locker(&m_mutex);

    while (m_queue.isEmpty()) {
        if (m_aborted)
            return QString();

        m_waitCondition.wait(&m_mutex);
    }

    return m_queue.dequeue();
}

void HostInfoWorker::lookupHost(const QString &address)
{
    QMutexLocker locker(&m_mutex);

    m_queue.enqueue(address);

    m_waitCondition.wakeOne();
}

void HostInfoWorker::clear()
{
    QMutexLocker locker(&m_mutex);

    m_queue.clear();
}

void HostInfoWorker::abort()
{
    QMutexLocker locker(&m_mutex);

    m_aborted = true;

    m_waitCondition.wakeOne();
}
