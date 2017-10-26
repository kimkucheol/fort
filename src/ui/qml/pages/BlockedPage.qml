import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import com.fortfirewall 1.0

BasePage {

    readonly property DriverManager driverManager: fortManager.driverManager

    property bool enableLogReading: false

    property var appPaths: []
    property var appPathIpMap: ({})
    property var appPathIpArray: ({})

    property var hostNames: ({})

    function readLogAsync() {
        driverManager.readLogAsync(logBuffer);
    }

    function cancelDeviceIo() {
        driverManager.cancelDeviceIo();
    }

    function switchLogReading(enable) {
        if (enableLogReading === enable)
            return;

        enableLogReading = enable;

        fortManager.setAppLogBlocked(enable);

        if (enable) {
            readLogAsync();
        } else {
            cancelDeviceIo();
        }
    }

    function clearAppPaths() {
        appPaths = [];
        appPathIpMap = ({});
        appPathIpArray = ({});

        hostNames = ({});
        hostInfo.abortHostLookups();

        refreshListViews();
    }

    function processLogBuffer() {
        var isNewEntry = false;

        while (logBuffer.read(logEntry)) {
            var path = getEntryPath(logEntry);
            var ipText = netUtil.ip4ToText(logEntry.ip);

            var ipTextsMap = appPathIpMap[path];
            if (!ipTextsMap) {
                ipTextsMap = ({});
                appPathIpMap[path] = ipTextsMap;
                appPathIpArray[path] = [];
                appPaths.push(path);

                isNewEntry = true;
            }

            var ipCount = ipTextsMap[ipText];
            ipTextsMap[ipText] = (ipCount || 0) + 1;

            var ipTextsArray = appPathIpArray[path];
            if (!ipCount) {
                if (ipTextsArray.length > 64) {
                    var oldIp = ipTextsArray.pop();
                    delete ipTextsMap[oldIp];
                }
                ipTextsArray.unshift(ipText);

                isNewEntry = true;
            }

            // Host name
            if (hostNames[ipText] === undefined) {
                hostNames[ipText] = false;
                //hostInfo.lookupHost(ipText);  // Crashes on exit with live lookup requests
            }
        }

        if (isNewEntry) {
            refreshListViews();
        }
    }

    function getEntryPath(logEntry) {
        var kernelPath = logEntry.kernelPath;
        if (!kernelPath) {
            kernelPath = osUtil.pidToKernelPath(logEntry.pid);
        }

        return fileUtil.kernelPathToPath(kernelPath);
    }

    function refreshListViews() {
        const curIndex = appListView.currentIndex;
        appListView.model = undefined;

        appListView.model = appPaths;
        appListView.currentIndex = curIndex;
    }

    Connections {
        target: mainPage
        onClosed: switchLogReading(false)
    }

    Connections {
        target: driverManager
        onReadLogResult: {
            if (success) {
                processLogBuffer();
            }

            if (enableLogReading) {
                readLogAsync();
            }
        }
    }

    HostInfo {
        id: hostInfo
        onHostLookedup: {
            if (success) {
                hostNames[name] = hostName;
                refreshListViews();
            }
        }
    }

    LogBuffer {
        id: logBuffer
    }
    LogEntry {
        id: logEntry
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        RowLayout {
            Button {
                enabled: btCopy.enabled
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Clear")
                onClicked: clearAppPaths()
            }
            Button {
                id: btCopy
                enabled: currentItem
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Copy Path")
                readonly property Item currentItem: appListView.currentItem
                onClicked: {
                    osUtil.setClipboardData(currentItem.text);
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Switch {
                id: cbShowBlockedApps
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Log Blocked Applications")
                onToggled: switchLogReading(checked)
            }
        }

        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            RowLayout {
                anchors.fill: parent
                spacing: 20

                ListView {
                    id: appListView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 10

                    highlightRangeMode: ListView.ApplyRange
                    highlightResizeDuration: 0
                    highlightMoveDuration: 200

                    highlight: Item {
                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: -7
                            radius: 2
                            border.width: 3
                            border.color: "black"
                            color: "transparent"
                        }
                    }

                    delegate: Label {
                        width: appListView.width
                        font.pixelSize: 20
                        elide: Text.ElideRight
                        text: modelData
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            const index = appListView.indexAt(mouse.x, mouse.y);
                            if (index >= 0) {
                                appListView.currentIndex = index;
                            }
                        }
                    }
                }

                ListView {
                    id: ipListView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 4

                    model: {
                        const curIndex = appListView.currentIndex;
                        if (curIndex < 0)
                            return undefined;

                        const path = appPaths[curIndex];
                        return appPathIpArray[path];
                    }

                    delegate: Label {
                        width: ipListView.width
                        elide: Text.ElideRight
                        text: hostNames[ipText] || ipText
                        readonly property string ipText: modelData
                    }
                }
            }
        }
    }
}