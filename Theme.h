#ifndef THEME_H
#define THEME_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QColor>

class Theme {
public:
    QColor nodeSelectedColor;
    bool gridDisplayOn;
    QColor socketBackgroundColor;
    QColor socketOutlineColor;

    // Load a theme from file and override the singleton instance
     void loadFromJson(const QString &filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Failed to open theme file:" << filePath;
            return;
        }

        QByteArray data = file.readAll();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if (doc.isNull()) {
            qWarning() << "Failed to parse JSON:" << parseError.errorString();
            return;
        }

        QJsonObject obj = doc.object();

        instance().nodeSelectedColor = QColor(obj.value("nodeSelectedColor").toString());
        instance().socketBackgroundColor = QColor(obj.value("socketBackgroundColor").toString());
        instance().socketOutlineColor = QColor(obj.value("socketOutlineColor").toString());
        instance().gridDisplayOn = obj.value("gridDisplayOn").toBool();

        qDebug() << "Theme loaded from:" << filePath;
    }

    // Accessor for the singleton instance
    static Theme& instance() {
        static Theme _instance;
        return _instance;
    }

private:
    Theme() {
        initFromJson("C:\\Users\\sujan\\Documents\\GitHub\\QNodeEditor\\styles\\main.json");
    }

    void initFromJson(const QString &filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Failed to open theme file:" << filePath;
            return;
        }

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
        if (doc.isNull()) {
            qWarning() << "Failed to parse JSON:" << parseError.errorString();
            return;
        }

        QJsonObject obj = doc.object();

        nodeSelectedColor = QColor(obj.value("nodeSelectedColor").toString());
        socketBackgroundColor = QColor(obj.value("socketBackgroundColor").toString());
        socketOutlineColor = QColor(obj.value("socketOutlineColor").toString());
        gridDisplayOn = obj.value("gridDisplayOn").toBool();

        qDebug() << "Theme loaded from:" << filePath;
    }
};

#endif // THEME_H
