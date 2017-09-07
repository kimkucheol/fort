import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import "apps"
import com.fortfirewall 1.0

BasePage {

    readonly property var appGroups: firewallConf.appGroups
    readonly property int appGroupsCount: appGroups.length

    function resetGroupName() {
        editGroupName.text = "";
        editGroupName.forceActiveFocus();
    }

    function addAppGroup() {
        const lastIndex = appGroups.length;
        firewallConf.addAppGroupByName(editGroupName.text);
        barGroups.currentIndex = lastIndex;
        resetGroupName();

        setConfEdited();
    }

    function removeAppGroup(index) {
        firewallConf.removeAppGroup(index, index);
        var lastIndex = appGroupsCount - 1;
        barGroups.currentIndex = (index < appGroupsCount)
                ? index : appGroupsCount - 1;

        setConfEdited();
    }

    function renameAppGroup() {
        const appGroup = appsColumn.appGroup;
        appGroup.name = editGroupName.text;
        resetGroupName();

        setConfEdited();
    }

    function moveAppGroup(index, step) {
        var toIndex = index + step;
        if (toIndex < 0)
            toIndex = appGroupsCount - 1;
        else if (toIndex >= appGroupsCount)
            toIndex = 0;

        firewallConf.moveAppGroup(index, toIndex);
        barGroups.currentIndex = toIndex;

        setConfEdited();
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            TextField {
                enabled: appGroupsCount < 16
                id: editGroupName
                placeholderText: translationManager.dummyBool
                                 && qsTranslate("qml", "Group Name")
            }
            Button {
                enabled: editGroupName.text
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Add Group")
                onClicked: addAppGroup()
            }
            Button {
                enabled: editGroupName.text && appsColumn.enabled
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Rename Group")
                onClicked: renameAppGroup()
            }

            Item {
                Layout.fillWidth: true
            }

            CheckBox {
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Block All")
                checked: firewallConf.appBlockAll
                onToggled: {
                    firewallConf.appBlockAll = checked;

                    setConfFlagsEdited();
                }
            }
            CheckBox {
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Allow All")
                checked: firewallConf.appAllowAll
                onToggled: {
                    firewallConf.appAllowAll = checked;

                    setConfFlagsEdited();
                }
            }
        }

        TabBar {
            id: barGroups
            Layout.fillWidth: true
            clip: true

            Repeater {
                id: rptAppGroups
                model: appGroups

                TabButton {
                    width: Math.max(100, implicitWidth)
                    font.bold: checked
                    icon.source: "qrc:/images/application_double.png"
                    text: appGroup.name

                    readonly property AppGroup appGroup: modelData
                }
            }
        }

        Frame {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
        }

        AppsColumn {
            id: appsColumn
            Layout.fillWidth: true
            Layout.fillHeight: true

            enabled: index >= 0
            opacity: enabled ? 1.0 : 0

            Behavior on opacity { NumberAnimation { duration: 150 } }

            index: barGroups.currentIndex
            appGroup: enabled ? barGroups.currentItem.appGroup
                              : nullAppGroup

            readonly property AppGroup nullAppGroup: AppGroup {}
        }
    }
}
