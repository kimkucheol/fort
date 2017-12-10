#include "traflistmodel.h"

#include "../../db/databasemanager.h"
#include "../../db/databasesql.h"
#include "../../util/dateutil.h"

TrafListModel::TrafListModel(DatabaseManager *databaseManager,
                             QObject *parent) :
    QAbstractListModel(parent),
    m_type(TrafHourly),
    m_appId(0),
    m_minTrafTime(0),
    m_maxTrafTime(0),
    m_databaseManager(databaseManager)
{
}

void TrafListModel::setType(TrafListModel::TrafType type)
{
    m_type = type;
}

void TrafListModel::setAppId(qint64 appId)
{
    m_appId = appId;
}

void TrafListModel::reset()
{
    const char *sqlMinTrafTime = getSqlMinTrafTime(m_type, m_appId);

    beginResetModel();

    m_minTrafTime = m_databaseManager->getTrafficTime(
                sqlMinTrafTime, m_appId);

    m_maxTrafTime = getMaxTrafTime(m_type);

    if (m_minTrafTime == 0) {
        m_minTrafTime = m_maxTrafTime;
    }

    m_trafCount = getTrafCount(m_type, m_minTrafTime, m_maxTrafTime);

    m_rowCache.invalidate();

    endResetModel();
}

int TrafListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_trafCount;
}

QVariant TrafListModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role >= DateTimeRole && role <= SumRole) {
        const int row = index.row();

        if (!m_rowCache.isValid(row)) {
            updateRowCache(row);
        }

        switch (role) {
        case DateTimeRole: return formatTrafTime(m_rowCache.trafTime);
        case DownloadRole: return m_rowCache.inBytes;
        case UploadRole: return m_rowCache.outBytes;
        case SumRole: return m_rowCache.inBytes + m_rowCache.outBytes;
        }
    }
    return QVariant();
}

QHash<int, QByteArray> TrafListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        {DateTimeRole, "dateTime"},
        {DownloadRole, "download"},
        {UploadRole, "upload"},
        {SumRole, "sum"}
    };

    return roles;
}

void TrafListModel::clear()
{
}

void TrafListModel::updateRowCache(int row) const
{
    m_rowCache.row = row;
    m_rowCache.trafTime = getTrafTime(row);

    const char *sqlSelectTraffic = getSqlSelectTraffic(m_type, m_appId);

    m_databaseManager->getTraffic(
                sqlSelectTraffic, m_rowCache.trafTime,
                m_rowCache.inBytes, m_rowCache.outBytes,
                m_appId);
}

QString TrafListModel::formatTrafTime(qint32 trafTime) const
{
    const qint64 unixTime = DateUtil::toUnixTime(trafTime);

    switch (m_type) {
    case TrafTotal: Q_FALLTHROUGH();
    case TrafHourly: return DateUtil::formatHour(unixTime);
    case TrafDaily: return DateUtil::formatDay(unixTime);
    case TrafMonthly: return DateUtil::formatMonth(unixTime);
    }
    return QString();
}

qint32 TrafListModel::getTrafTime(int row) const
{
    switch (m_type) {
    case TrafHourly: return m_maxTrafTime - row;
    case TrafDaily: return m_maxTrafTime - row * 24;
    case TrafMonthly: return DateUtil::addUnixMonths(m_maxTrafTime, -row);
    case TrafTotal: return m_minTrafTime;
    }
    return 0;
}

qint32 TrafListModel::getTrafCount(TrafType type, qint32 minTrafTime,
                                   qint32 maxTrafTime)
{
    if (type == TrafTotal)
        return 1;

    const qint32 hours = maxTrafTime - minTrafTime + 1;
    if (type == TrafHourly)
        return hours;

    const qint32 days = hours / 24 + 1;
    if (type == TrafDaily)
        return days;

    const qint32 months = days / 30 + 1;
    if (type == TrafMonthly)
        return months;

    Q_UNREACHABLE();
    return 0;
}

qint32 TrafListModel::getMaxTrafTime(TrafType type)
{
    const qint64 unixTime = DateUtil::getUnixTime();

    switch (type) {
    case TrafTotal: Q_FALLTHROUGH();
    case TrafHourly: return DateUtil::getUnixHour(unixTime);
    case TrafDaily: return DateUtil::getUnixDay(unixTime);
    case TrafMonthly: return DateUtil::getUnixMonth(unixTime);
    }

    Q_UNREACHABLE();
    return 0;
}

const char *TrafListModel::getSqlMinTrafTime(TrafType type, qint64 appId)
{
    switch (type) {
    case TrafHourly: return appId ? DatabaseSql::sqlSelectMinTrafAppHour
                                  : DatabaseSql::sqlSelectMinTrafHour;
    case TrafDaily: return appId ? DatabaseSql::sqlSelectMinTrafAppDay
                                 : DatabaseSql::sqlSelectMinTrafDay;
    case TrafMonthly: return appId ? DatabaseSql::sqlSelectMinTrafAppMonth
                                   : DatabaseSql::sqlSelectMinTrafMonth;
    case TrafTotal: return appId ? DatabaseSql::sqlSelectMinTrafAppTotal
                                 : DatabaseSql::sqlSelectMinTrafTotal;
    }

    Q_UNREACHABLE();
    return nullptr;
}

const char *TrafListModel::getSqlSelectTraffic(TrafType type, qint64 appId)
{
    switch (type) {
    case TrafHourly: return appId ? DatabaseSql::sqlSelectTrafAppHour
                                  : DatabaseSql::sqlSelectTrafHour;
    case TrafDaily: return appId ? DatabaseSql::sqlSelectTrafAppDay
                                 : DatabaseSql::sqlSelectTrafDay;
    case TrafMonthly: return appId ? DatabaseSql::sqlSelectTrafAppMonth
                                   : DatabaseSql::sqlSelectTrafMonth;
    case TrafTotal: return appId ? DatabaseSql::sqlSelectTrafAppTotal
                                 : DatabaseSql::sqlSelectTrafTotal;
    }

    Q_UNREACHABLE();
    return nullptr;
}