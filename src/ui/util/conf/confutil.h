#ifndef CONFUTIL_H
#define CONFUTIL_H

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QObject>
#include <QVector>

#include "addressrange.h"

QT_FORWARD_DECLARE_CLASS(AddressGroup)
QT_FORWARD_DECLARE_CLASS(AppGroup)
QT_FORWARD_DECLARE_CLASS(FirewallConf)

QT_FORWARD_DECLARE_STRUCT(fort_conf_limit)

typedef QVector<quint32> numbers_arr_t;

typedef QVarLengthArray<AddressRange, 2> addrranges_arr_t;

typedef QMap<QString, quint32> appperms_map_t;
typedef QMap<QString, qint8> appgroups_map_t;

class ConfUtil : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit ConfUtil(QObject *parent = nullptr);

    QString errorMessage() const { return m_errorMessage; }

signals:
    void errorMessageChanged();

public slots:
    int write(const FirewallConf &conf, QByteArray &buf);
    int writeFlags(const FirewallConf &conf, QByteArray &buf);

private:
    void setErrorMessage(const QString &errorMessage);

    bool parseAddressGroups(const QList<AddressGroup *> &addressGroups,
                            addrranges_arr_t &addressRanges,
                            numbers_arr_t &addressGroupOffsets,
                            quint32 &addressGroupsSize);

    // Convert app. groups to plain lists
    bool parseAppGroups(const QList<AppGroup *> &appGroups,
                        QStringList &appPaths,
                        quint32 &appPathsLen,
                        numbers_arr_t &appPerms,
                        appgroups_map_t &appGroupIndexes);

    bool parseApps(const QString &text, bool blocked,
                   appperms_map_t &appPermsMap,
                   appgroups_map_t &appGroupIndexes,
                   int groupOffset);

    static QString parseAppPath(const QStringRef &line);

    static void writeData(char *output, const FirewallConf &conf,
                          const addrranges_arr_t &addressRanges,
                          const numbers_arr_t &addressGroupOffsets,
                          const QStringList &appPaths,
                          const numbers_arr_t &appPerms,
                          const appgroups_map_t &appGroupIndexes);

    static quint16 writeLimits(struct fort_conf_limit *limits,
                               const QList<AppGroup *> &appGroups);

    static void writeAddressRanges(char **data,
                                   const addrranges_arr_t &addressRanges);
    static void writeAddressRange(char **data,
                                  const AddressRange &addressRange);

    static void writeNumbers(char **data, const QVector<quint32> &array);
    static void writeChars(char **data, const QVector<qint8> &array);
    static void writeStrings(char **data, const QStringList &list);

private:
    QString m_errorMessage;
};

#endif // CONFUTIL_H
