#include "mainwindow.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>

namespace {
void writeLog(const QString& message)
{
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (tempDir.isEmpty()) {
        tempDir = QDir::tempPath();
    }

    QFile file(tempDir + "/SmartSpellChecker.log");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
               << " | " << message << "\n";
    }
}
}

int main(int argc, char *argv[])
{
    writeLog("Application starting.");
    QApplication app(argc, argv);
    writeLog("QApplication created.");
    QApplication::setFont(QFont("Segoe UI", 10));

    MainWindow window;
    writeLog("MainWindow constructed.");
    window.show();
    writeLog("window.show() called.");

    QTimer::singleShot(0, [&window]() {
        window.showNormal();
        window.raise();
        window.activateWindow();
    });
    writeLog("Visibility timer scheduled.");

    const int result = app.exec();
    writeLog(QString("Application exiting with code %1.").arg(result));
    return result;
}
