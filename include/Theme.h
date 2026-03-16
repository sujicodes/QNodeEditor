#ifndef THEME_H
#define THEME_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QColor>
#include <QCoreApplication>

/**
 * @brief Singleton that holds the active visual theme for the node editor.
 *
 * Theme loads colour and display settings from a JSON file and exposes them
 * as public members for use throughout the editor (nodes, sockets, grid, etc.).
 *
 * On first access, the singleton automatically loads the default theme bundled
 * as a Qt resource at `://styles/main.json`. The active theme can be replaced
 * at runtime by calling loadFromJson() with a different file path, allowing
 * users or applications to supply custom themes without recompilation.
 *
 * @par JSON schema (all fields optional, unrecognised fields are ignored):
 * @code
 * {
 *   "nodeSelectedColor":          "#RRGGBB",
 *   "socketBackgroundColor":      "#RRGGBB",
 *   "socketOutlineColor":         "#RRGGBB",
 *   "nodeContentBackgroundColor": "#RRGGBB",
 *   "nodeTitleBackgroundColor":   "#RRGGBB",
 *   "gridDisplayOn":              true
 * }
 * @endcode
 *
 * @par Example usage:
 * @code
 * // Read a theme value:
 * QColor selected = Theme::instance().nodeSelectedColor;
 *
 * // Override with a custom theme at runtime:
 * Theme::instance().loadFromJson("/path/to/custom-theme.json");
 * @endcode
 */
class Theme
{
public:

    /// @brief Colour used to highlight a node's outline when it is selected.
    QColor nodeSelectedColor;

    /// @brief Whether the background grid is rendered in the scene.
    bool gridDisplayOn;

    /// @brief Fill colour for socket circle graphics.
    QColor socketBackgroundColor;

    /// @brief Outline colour for socket circle graphics.
    QColor socketOutlineColor;

    /// @brief Background colour for the content (body) area of a node.
    QColor nodeContentBackgroundColor;

    /// @brief Background colour for the title bar area of a node.
    QColor nodeTitleBackgroundColor;

    /**
     * @brief Loads a theme from a JSON file and applies it to the singleton instance.
     *
     * Parses the given file and overwrites all theme fields on the singleton.
     * If the file cannot be opened or the JSON is malformed, a warning is
     * logged and the existing theme values are preserved unchanged.
     *
     * @param filePath The path to a JSON theme file. Can be a filesystem path
     *                 or a Qt resource path (e.g. "://styles/custom.json").
     */
    void loadFromJson(const QString& filePath)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qWarning() << "Failed to open theme file:" << filePath;
            return;
        }

        QByteArray data = file.readAll();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if (doc.isNull())
        {
            qWarning() << "Failed to parse JSON:" << parseError.errorString();
            return;
        }

        QJsonObject obj = doc.object();
        instance().nodeSelectedColor          = QColor(obj.value("nodeSelectedColor").toString());
        instance().socketBackgroundColor      = QColor(obj.value("socketBackgroundColor").toString());
        instance().socketOutlineColor         = QColor(obj.value("socketOutlineColor").toString());
        instance().gridDisplayOn              = obj.value("gridDisplayOn").toBool();
        instance().nodeContentBackgroundColor = QColor(obj.value("nodeContentBackgroundColor").toString());
        instance().nodeTitleBackgroundColor   = QColor(obj.value("nodeTitleBackgroundColor").toString());

        qDebug() << "Theme loaded from:" << filePath;
    }

    /**
     * @brief Returns the global Theme singleton instance.
     *
     * On first call, constructs the instance and loads the default theme
     * from the bundled Qt resource `://styles/main.json`.
     *
     * @return Reference to the singleton Theme instance.
     */
    static Theme& instance()
    {
        static Theme instance;
        return instance;
    }

private:

    /**
     * @brief Private constructor. Loads the default bundled theme on construction.
     *
     * Called once by instance() on first access. Invokes initFromJson() with
     * the default resource path `://styles/main.json`.
     */
    Theme()
    {
        initFromJson("://styles/main.json");
    }

    /**
     * @brief Loads and applies theme values from the given JSON file path.
     *
     * Used internally by the constructor to initialise the singleton from
     * the default bundled resource. Logs a warning if the file cannot be
     * opened or the JSON is invalid, leaving members at their default-constructed
     * values in that case.
     *
     * @param filePath The Qt resource or filesystem path to the theme JSON file.
     */
    void initFromJson(const QString& filePath)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qWarning() << "Failed to open theme file :" << filePath;
            return;
        }

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
        if (doc.isNull())
        {
            qWarning() << "Failed to parse JSON :" << parseError.errorString();
            return;
        }

        QJsonObject obj = doc.object();
        nodeSelectedColor          = QColor(obj.value("nodeSelectedColor").toString());
        socketBackgroundColor      = QColor(obj.value("socketBackgroundColor").toString());
        socketOutlineColor         = QColor(obj.value("socketOutlineColor").toString());
        gridDisplayOn              = obj.value("gridDisplayOn").toBool();
        nodeContentBackgroundColor = QColor(obj.value("nodeContentBackgroundColor").toString());
        nodeTitleBackgroundColor   = QColor(obj.value("nodeTitleBackgroundColor").toString());

        qDebug() << "Theme loaded from :" << filePath;
    }
};

#endif // THEME_H