#ifndef TRAFLISTMODEL_H
#define TRAFLISTMODEL_H

#include <QAbstractListModel>

QT_FORWARD_DECLARE_CLASS(DatabaseManager)

struct TrafficRow {
    bool isValid(int row) const { return row == this->row; }
    void invalidate() { row = -1; }

    int row;
    qint32 trafTime;
    qint64 inBytes;
    qint64 outBytes;
};

class TrafListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum TrafRole {
        DateTimeRole = Qt::DisplayRole,
        DownloadRole = Qt::UserRole,
        UploadRole,
        SumRole
    };
    Q_ENUM(TrafRole)

    enum TrafType {
        TrafHourly = 0,
        TrafDaily,
        TrafMonthly,
        TrafTotal
    };
    Q_ENUM(TrafType)

    explicit TrafListModel(DatabaseManager *databaseManager,
                           QObject *parent = nullptr);

    TrafListModel::TrafType type() const { return m_type; }
    void setType(TrafListModel::TrafType type);

    qint64 appId() const { return m_appId; }
    void setAppId(qint64 appId);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:

public slots:
    void clear();

    void resetAppTotals();

    void reset();
    void refresh();

private:
    void invalidateRowCache();
    void updateRowCache(int row) const;

    QString formatTrafUnit(qint64 bytes) const;
    QString formatTrafTime(qint32 trafTime) const;

    qint32 getTrafTime(int row) const;

    static qint32 getTrafCount(TrafType type, qint32 minTrafTime,
                               qint32 maxTrafTime);
    static qint32 getMaxTrafTime(TrafType type);

    static const char *getSqlMinTrafTime(TrafType type, qint64 appId);
    static const char *getSqlSelectTraffic(TrafType type, qint64 appId);

private:
    bool m_isEmpty;

    TrafType m_type;

    qint64 m_appId;

    qint32 m_minTrafTime;
    qint32 m_maxTrafTime;
    qint32 m_trafCount;

    mutable TrafficRow m_rowCache;

    DatabaseManager *m_databaseManager;
};

#endif // TRAFLISTMODEL_H
