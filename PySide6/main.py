import sys
from time import strftime, localtime
import os
from pathlib import Path
from typing import Optional
from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine, QmlElement
from PySide6.QtQuickControls2 import QQuickStyle
from PySide6.QtCore import QTimer, QObject, Slot, QUrl, Signal, QCoreApplication, QAbstractListModel, QModelIndex, Qt


QML_IMPORT_NAME = "MainModule"
QML_IMPORT_MAJOR_VERSION = 1

class DeviceModel(QAbstractListModel):
    def __init__(self, parent=None):
        super().__init__(parent)
        self._items = []

    def rowCount(self, parent=QModelIndex()):
        return len(self._items)

    def data(self, index, role=Qt.DisplayRole):
        if not index.isValid() or not (0 <= index.row() < len(self._items)):
            return None

        item = self._items[index.row()]

        if role == Qt.DisplayRole:
            return item.name
        elif role == Qt.UserRole:
            return item.value

        return None

    # def append(self, item):
    #     self.beginInsertRows(QModelIndex(), len(self._items), len(self._items))
    #     self._items.append(ListItem(item["name"], item["value"]))
    #     self.endInsertRows()

    def clear(self):
        self.beginRemoveRows(QModelIndex(), 0, len(self._items) - 1)
        self._items = []
        self.endRemoveRows()


@QmlElement
class Bridge(QObject):

    timer_signal = Signal(str, arguments=['time'])

    def __init__(self):
        super().__init__()
        # Define timer.
        self.timer = QTimer()
        self.timer.setInterval(100)  # msecs 100 = 1/10th sec
        self.timer.timeout.connect(self.update_time)
        self.timer.start()

    @Slot(str)
    def print_log(self, str):
        print('received from qml:' + str)

    @Slot()
    def update_time(self):
        # Pass the current time to QML.
        curr_time = strftime("%H:%M:%S", localtime())
        self.timer_signal.emit(curr_time)
    
    @Slot(str, result=str)
    def getColor(self, s):
        return 'blue'

if __name__ == "__main__":
    QCoreApplication.setApplicationName('LearnPySide6')
    QCoreApplication.setOrganizationName('tainzhi')
    app = QGuiApplication(sys.argv)
    QQuickStyle.setStyle("Material")
    engine = QQmlApplicationEngine()
    # engine.loadData(QML.encode('utf-8'))
    # engine.load(os.fspath(Path(__file__).resolve().parent / "main.qml"))
    engine.addImportPath(sys.path[0])
    engine.loadFromModule('MainModule', 'Main')
    if not engine.rootObjects():
        sys.exit(-1)
    exit_code = app.exec()
    del engine
    sys.exit(exit_code)