#include "logger.h"

#include <QDateTime>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../../common/version.h"

#define LOGGER_FILE_PREFIX      "log_fort_"
#define LOGGER_FILE_SUFFIX      ".txt"
#define LOGGER_FILE_MAX_SIZE    (1024 * 1024)
#define LOGGER_KEEP_FILES       9

static QtMessageHandler g_oldMessageHandler = nullptr;

Logger::Logger(QObject *parent) :
    QObject(parent),
    m_active(false),
    m_debug(false),
    m_console(false),
    m_writing(false)
{
    g_oldMessageHandler = qInstallMessageHandler(messageHandler);
}

Logger *Logger::instance()
{
    static Logger *g_instanceLogger = 0;

    if (!g_instanceLogger) {
        g_instanceLogger = new Logger();
    }
    return g_instanceLogger;
}

void Logger::setActive(bool active)
{
    if (m_active != active) {
        m_active = active;
    }
}

void Logger::setDebug(bool debug)
{
    if (m_debug != debug) {
        m_debug = debug;

        QLoggingCategory::setFilterRules(debug ? QString() : "*.debug=false");
    }
}

void Logger::setConsole(bool console)
{
    if (m_console != console) {
        m_console = console;

        if (console) {
            AllocConsole();
        } else {
            FreeConsole();
        }
    }
}

void Logger::setPath(const QString &path)
{
    m_dir.setPath(path);
    m_dir.mkpath(".");
}

bool Logger::openLogFile()
{
    const QString filename = QString("%1%2%3").arg(
                QLatin1String(LOGGER_FILE_PREFIX),
                QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss_zzz"),
                QLatin1String(LOGGER_FILE_SUFFIX));

    m_file.setFileName(m_dir.filePath(filename));

    if (!m_file.open(QIODevice::WriteOnly | QIODevice::Text
                     | QIODevice::Truncate)) {
        qWarning() << "Cannot open log file: " << m_file.errorString();
        return false;
    }

    return true;
}

void Logger::closeLogFile()
{
    m_file.close();
}

void Logger::writeLogLine(Logger::LogLevel level, const QString &dateString,
                          const QString &message)
{
    static const char * const g_levelTexts[] = {
        "[Info ]", "[Warn ]", "[Error]"};

    m_file.write(QString("%1%2 %3\n").arg(g_levelTexts[int(level)],
                 dateString, message).toLatin1());
    m_file.flush();
}

void Logger::writeLog(const QString &message, LogLevel level)
{
    if (m_writing)
        return;  // avoid recursive calls

    m_writing = true;

    const QString dateString = QDateTime::currentDateTime()
            .toString("'['yyyy-MM-dd HH:mm:ss']'");

    // Create file when required to avoid empty files
    if (!m_file.isOpen()) {
        if (!openLogFile()) {
            m_writing = false;
            return;
        }

        // Write file header
        writeLogLine(Info, dateString, APP_NAME " version: " APP_VERSION_STR
                                       " build date: " __DATE__ " " __TIME__);
    }

    writeLogLine(level, dateString, message);

    if (m_file.size() > LOGGER_FILE_MAX_SIZE) {
        closeLogFile();  // Too big file
        checkLogFiles();
    }

    m_writing = false;
}

void Logger::writeLogList(const QString &message, const QStringList &list,
                          LogLevel level)
{
    writeLog(message + '{' + list.join(',') + '}', level);
}

void Logger::checkLogFiles()
{
    int count = LOGGER_KEEP_FILES;

    // Remove old files
    foreach (const QString &fileName, m_dir.entryList(
                 QStringList() << (QLatin1String(LOGGER_FILE_PREFIX) + '*'
                                   + QLatin1String(LOGGER_FILE_SUFFIX)),
                 QDir::Files, QDir::Time)) {
        if (--count < 0) {
            QFile::remove(m_dir.filePath(fileName));
        }
    }
}

void Logger::messageHandler(QtMsgType type,
                            const QMessageLogContext &context,
                            const QString &message)
{
    if (instance()->active()) {
        LogLevel level = Info;

        switch (type) {
        case QtWarningMsg:
            level = Warning;
            break;
        case QtCriticalMsg:
        case QtFatalMsg:
            level = Error;
            break;
        default:
            break;
        }

        // Write only errors to log file
        if (level != Info) {
            const QString text = context.category
                    ? (QLatin1String(context.category) + ": " + message)
                    : message;

            instance()->writeLog(text, level);
        }
    }

    if (instance()->console()) {
        const HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD nw;

        QByteArray data;
        if (context.category) {
            data.append(context.category);
            data.append(": ");
        }
        data.append(message.toLatin1());
        data.append('\n');

        WriteFile(stdoutHandle, data.constData(), data.size(), &nw, NULL);
    }

    if (g_oldMessageHandler) {
        g_oldMessageHandler(type, context, message);
    }
}
