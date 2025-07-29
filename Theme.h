#ifndef THEME_H
#define THEME_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <QFont>
#include <QColor>

struct Theme {
    QColor nodeSelectedColor;
    bool gridDisplayOn;

    static Theme loadFromJson(const QString &filePath) {
        Theme theme;
    
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Failed to open theme file:" << filePath;
            return theme;
        }
    
        QByteArray data = file.readAll();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if (doc.isNull()) {
            qWarning() << "Failed to parse JSON:" << parseError.errorString();
            return theme;
        }
    
        QJsonObject obj = doc.object();
    
        theme.nodeSelectedColor = QColor(obj.value("nodeSelectedColor").toString());
        theme.gridDisplayOn = obj.value("gridDisplayOn").toBool();
        return theme;
    }
};


inline static Theme themeInstance = Theme::loadFromJson("css/main.json");
#endif