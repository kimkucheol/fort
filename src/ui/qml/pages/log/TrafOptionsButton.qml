import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import "../../controls"
import com.fortfirewall 1.0

ButtonPopup {

    icon.source: "qrc:/images/database_save.png"
    text: translationManager.dummyBool
          && qsTranslate("qml", "Options…")

    readonly property var trafKeepDayValues: [
        90, -1, 90, 180, 365, 365 * 3
    ]

    readonly property var trafKeepDayNames:
        translationManager.dummyBool
        && [
            qsTranslate("qml", "Custom"),
            qsTranslate("qml", "Forever"),
            qsTranslate("qml", "3 months"),
            qsTranslate("qml", "6 months"),
            qsTranslate("qml", "1 year"),
            qsTranslate("qml", "3 years")
        ]

    readonly property var trafKeepMonthValues: [
        360, -1, 3, 6, 12, 360
    ]

    readonly property var trafKeepMonthNames:
        translationManager.dummyBool
        && [
            qsTranslate("qml", "Custom"),
            qsTranslate("qml", "Forever"),
            qsTranslate("qml", "3 months"),
            qsTranslate("qml", "6 months"),
            qsTranslate("qml", "1 year"),
            qsTranslate("qml", "3 years")
        ]

    readonly property var quotaValues: [
        10, 0, 100, 500, 1024, 8 * 1024, 10 * 1024, 30 * 1024,
        50 * 1024, 100 * 1024
    ]

    readonly property var quotaNames: {
        var list = translationManager.dummyBool
                && [qsTranslate("qml", "Custom"),
                    qsTranslate("qml", "Disabled")];

        const n = quotaValues.length;
        for (var i = list.length; i < n; ++i) {
            list.push(formatQuota(quotaValues[i]));
        }
        return list;
    }

    function formatQuota(mbytes) {
        return netUtil.formatDataSize(mbytes * 1024 * 1024, 1);
    }

    ColumnLayout {
        SpinComboRow {
            values: {
                var arr = [];
                for (var i = 1; i <= 31; ++i) {
                    arr.push(i);
                }
                return arr;
            }
            checkBox {
                indicator: null
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Month starts on:")
            }
            field {
                from: 1
                to: 31
                value: firewallConf.monthStart
                onValueChanged: {
                    const value = field.value;
                    if (firewallConf.monthStart == value)
                        return;

                    firewallConf.monthStart = value;

                    setConfFlagsEdited();
                }
            }
        }

        HSeparator {}

        SpinComboRow {
            names: trafKeepDayNames
            values: trafKeepDayValues
            checkBox {
                indicator: null
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Keep days for 'Hourly':")
            }
            field {
                from: -1
                value: firewallConf.trafHourKeepDays
                onValueChanged: {
                    const value = field.value;
                    if (firewallConf.trafHourKeepDays == value)
                        return;

                    firewallConf.trafHourKeepDays = value;

                    setConfFlagsEdited();
                }
            }
        }

        SpinComboRow {
            names: trafKeepDayNames
            values: trafKeepDayValues
            checkBox {
                indicator: null
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Keep days for 'Daily':")
            }
            field {
                from: -1
                value: firewallConf.trafDayKeepDays
                onValueChanged: {
                    const value = field.value;
                    if (firewallConf.trafDayKeepDays == value)
                        return;

                    firewallConf.trafDayKeepDays = value;

                    setConfFlagsEdited();
                }
            }
        }

        SpinComboRow {
            names: trafKeepMonthNames
            values: trafKeepMonthValues
            checkBox {
                indicator: null
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Keep months for 'Monthly':")
            }
            field {
                from: -1
                value: firewallConf.trafMonthKeepMonths
                onValueChanged: {
                    const value = field.value;
                    if (firewallConf.trafMonthKeepMonths == value)
                        return;

                    firewallConf.trafMonthKeepMonths = value;

                    setConfFlagsEdited();
                }
            }
        }

        HSeparator {}

        SpinComboRow {
            names: quotaNames
            values: quotaValues
            checkBox {
                indicator: null
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Day's Quota:")
            }
            field {
                from: 0
                to: 999 * 1024
                value: firewallConf.quotaDayMb
                onValueChanged: {
                    const value = field.value;
                    if (firewallConf.quotaDayMb == value)
                        return;

                    firewallConf.quotaDayMb = value;

                    setConfFlagsEdited();
                }
            }
        }

        SpinComboRow {
            names: quotaNames
            values: quotaValues
            checkBox {
                indicator: null
                text: translationManager.dummyBool
                      && qsTranslate("qml", "Month's Quota:")
            }
            field {
                from: 0
                to: 999 * 1024
                value: firewallConf.quotaMonthMb
                onValueChanged: {
                    const value = field.value;
                    if (firewallConf.quotaMonthMb == value)
                        return;

                    firewallConf.quotaMonthMb = value;

                    setConfFlagsEdited();
                }
            }
        }
    }
}
