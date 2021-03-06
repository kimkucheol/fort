#include "appgroup.h"

AppGroup::AppGroup(QObject *parent) :
    QObject(parent),
    m_enabled(true),
    m_limitInEnabled(false),
    m_limitOutEnabled(false),
    m_speedLimitIn(0),
    m_speedLimitOut(0)
{
}

void AppGroup::setEnabled(bool enabled)
{
    if (bool(m_enabled) != enabled) {
        m_enabled = enabled;
        emit enabledChanged();
    }
}

void AppGroup::setLimitInEnabled(bool enabled)
{
    if (bool(m_limitInEnabled) != enabled) {
        m_limitInEnabled = enabled;
        emit limitInEnabledChanged();
    }
}

void AppGroup::setLimitOutEnabled(bool enabled)
{
    if (bool(m_limitOutEnabled) != enabled) {
        m_limitOutEnabled = enabled;
        emit limitOutEnabledChanged();
    }
}

void AppGroup::setSpeedLimitIn(quint32 limit)
{
    if (m_speedLimitIn != limit) {
        m_speedLimitIn = limit;
        emit speedLimitInChanged();
    }
}

void AppGroup::setSpeedLimitOut(quint32 limit)
{
    if (m_speedLimitOut != limit) {
        m_speedLimitOut = limit;
        emit speedLimitOutChanged();
    }
}

void AppGroup::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

void AppGroup::setBlockText(const QString &blockText)
{
    if (m_blockText != blockText) {
        m_blockText = blockText;
        emit blockTextChanged();
    }
}

void AppGroup::setAllowText(const QString &allowText)
{
    if (m_allowText != allowText) {
        m_allowText = allowText;
        emit allowTextChanged();
    }
}

QVariant AppGroup::toVariant() const
{
    QVariantMap map;

    map["limitInEnabled"] = limitInEnabled();
    map["limitOutEnabled"] = limitOutEnabled();
    map["speedLimitIn"] = speedLimitIn();
    map["speedLimitOut"] = speedLimitOut();

    map["name"] = name();
    map["blockText"] = blockText();
    map["allowText"] = allowText();

    return map;
}

void AppGroup::fromVariant(const QVariant &v)
{
    const QVariantMap map = v.toMap();

    m_limitInEnabled = map["limitInEnabled"].toBool();
    m_limitOutEnabled = map["limitOutEnabled"].toBool();
    m_speedLimitIn = map["speedLimitIn"].toUInt();
    m_speedLimitOut = map["speedLimitOut"].toUInt();

    m_name = map["name"].toString();
    m_blockText = map["blockText"].toString();
    m_allowText = map["allowText"].toString();
}
