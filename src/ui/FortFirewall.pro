QT += core gui qml widgets

CONFIG += c++11

TEMPLATE = app
TARGET = FortFirewall
DESTDIR = ./
MOC_DIR = .moc
OBJECTS_DIR = .obj
RCC_DIR = .rcc

SOURCES += \
    main.cpp \
    conf/addressgroup.cpp \
    conf/appgroup.cpp \
    conf/firewallconf.cpp \
    db/databasemanager.cpp \
    db/databasesql.cpp \
    db/quotamanager.cpp \
    driver/drivermanager.cpp \
    driver/driverworker.cpp \
    fortcommon.cpp \
    fortmanager.cpp \
    fortsettings.cpp \
    log/logbuffer.cpp \
    log/logentry.cpp \
    log/logentryblocked.cpp \
    log/logentryprocnew.cpp \
    log/logentrystattraf.cpp \
    log/logmanager.cpp \
    log/model/appblockedmodel.cpp \
    log/model/appstatmodel.cpp \
    log/model/iplistmodel.cpp \
    log/model/stringlistmodel.cpp \
    log/model/traflistmodel.cpp \
    mainwindow.cpp \
    task/taskdownloader.cpp \
    task/taskinfo.cpp \
    task/taskmanager.cpp \
    task/tasktasix.cpp \
    task/taskupdatechecker.cpp \
    task/taskuzonline.cpp \
    task/taskworker.cpp \
    translationmanager.cpp \
    util/conf/addressrange.cpp \
    util/conf/confutil.cpp \
    util/dateutil.cpp \
    util/device.cpp \
    util/fileutil.cpp \
    util/guiutil.cpp \
    util/logger.cpp \
    util/net/hostinfo.cpp \
    util/net/hostinfocache.cpp \
    util/net/hostinfoworker.cpp \
    util/net/ip4range.cpp \
    util/net/netdownloader.cpp \
    util/net/netutil.cpp \
    util/osutil.cpp \
    util/processinfo.cpp \
    util/stringutil.cpp \
    util/windowstatewatcher.cpp

HEADERS += \
    conf/addressgroup.h \
    conf/appgroup.h \
    conf/firewallconf.h \
    db/databasemanager.h \
    db/databasesql.h \
    db/quotamanager.h \
    driver/drivermanager.h \
    driver/driverworker.h \
    fortcommon.h \
    fortmanager.h \
    fortsettings.h \
    log/logbuffer.h \
    log/logentry.h \
    log/logentryblocked.h \
    log/logentryprocnew.h \
    log/logentrystattraf.h \
    log/logmanager.h \
    log/model/appblockedmodel.h \
    log/model/appstatmodel.h \
    log/model/iplistmodel.h \
    log/model/stringlistmodel.h \
    log/model/traflistmodel.h \
    mainwindow.h \
    task/taskdownloader.h \
    task/taskinfo.h \
    task/taskmanager.h \
    task/tasktasix.h \
    task/taskupdatechecker.h \
    task/taskuzonline.h \
    task/taskworker.h \
    translationmanager.h \
    util/conf/addressrange.h \
    util/conf/confutil.h \
    util/dateutil.h \
    util/device.h \
    util/fileutil.h \
    util/guiutil.h \
    util/logger.h \
    util/net/hostinfo.h \
    util/net/hostinfocache.h \
    util/net/hostinfoworker.h \
    util/net/ip4range.h \
    util/net/netdownloader.h \
    util/net/netutil.h \
    util/osutil.h \
    util/processinfo.h \
    util/stringutil.h \
    util/windowstatewatcher.h

QML_FILES += \
    qml/*.qml \
    qml/controls/*.qml \
    qml/pages/*.qml \
    qml/pages/addresses/*.qml \
    qml/pages/apps/*.qml \
    qml/pages/log/*.qml \
    qml/pages/schedule/*.qml

OTHER_FILES += \
    $${QML_FILES}

# QML files
RESOURCES += fort_qml.qrc

# Images
RESOURCES += fort_images.qrc

# Shadow Build: Copy i18n/ to build path
!equals(PWD, $${OUT_PWD}) {
    i18n.files = i18n/*.qm
    i18n.path = $${OUT_PWD}/i18n
    COPIES += i18n
}

# Windows
LIBS += -lfwpuclnt -lkernel32 -luser32 -luuid -lws2_32
RC_FILE = FortFirewall.rc

# Kernel Driver
installer_build {
    BUILDCMD = MSBuild $$PWD/../driver/fortdrv.vcxproj /p:OutDir=./;IntDir=$$OUT_PWD/driver/

    fortdrv32.target = $$PWD/../driver/fortfw32.sys
    fortdrv32.commands = $$BUILDCMD /p:Platform=Win32

    fortdrv64.target = $$PWD/../driver/fortfw64.sys
    fortdrv64.commands = $$BUILDCMD /p:Platform=x64

    QMAKE_EXTRA_TARGETS += fortdrv32 fortdrv64
    PRE_TARGETDEPS += $$fortdrv32.target $$fortdrv64.target
}

include(db/sqlite/sqlite.pri)
