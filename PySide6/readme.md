## info
学习pyside6和qml的项目

学习网站：
[b站  QML6(Qt Quick)开发教程](https://www.bilibili.com/video/BV1j14y1e7Hf?p=27&spm_id_from=pageDriver&vd_source=1525beaa0252a57633d96942eedf27d4)
[Qt官方教程 PySide6](https://doc.qt.io/qtforpython-6/examples/example_quickcontrols_filesystemexplorer.html#filesystemexplorer-example)
https://www.pythonguis.com/tutorials/pyside6-creating-multiple-windows/
PySide ThreadPool: https://www.pythonguis.com/tutorials/multithreading-pyside6-applications-qthreadpool/
QAbstractListModel in QML: https://doc.qt.io/qtforpython-6/examples/example_qml_editingmodel.html
Qmlonline editor: [qmlonline (kde.org)](https://qmlonline.kde.org/)
QmlTypes: [ComboBox QML Type | Qt Quick Controls 5.15.15](https://doc.qt.io/qt-5/qml-qtquick-controls2-combobox.html)
https://doc.qt.io/qt-5/qtquick-modelviewsdata-modelview.html#qml-data-models
[QML-Coding-Guide/README.md at master · Furkanzmc/QML-Coding-Guide (github.com)](https://github.com/Furkanzmc/QML-Coding-Guide/blob/master/README.md)
[PyQt/PySide6快速入门 - 3 QML简介与Qt Creator开发环境 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/521532869)
[PyQt/PySide6快速入门 - 5 QML天气小程序（b） - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/525630361)

## install and run
clone之后，进入项目根部目录，建立虚拟环境
```bash
python3 -m venv venv
```
激活虚拟环境
```bash
# windows
.\venv\Scripts\activate.bat

# linux
source venv/bin/activate
```
安装依赖
```bash
pip install -r requirements.txt
```

运行
```bash
python3 main.py
```