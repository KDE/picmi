/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QList>
#include <QSettings>
#include <QSharedPointer>
#include <KGameDifficulty>

class Settings : public QObject
{
    Q_OBJECT

public:
    enum SettingsType {
        Width = 0,
        Height,
        BoxDensity,
        PreventMistakes,
        Level,
        CustomBgEnabled,
        CustomBgPath,
        FontColorSolved,
        FontColorUnsolved
    };

    int width() const;
    int height() const;
    double boxDensity() const;
    bool preventMistakes() const;
    KGameDifficultyLevel::StandardLevel level() const;
    bool customBgEnabled() const;
    QString customBgPath() const;
    QString fontColorSolved() const;
    QString fontColorUnsolved() const;

    void setWidth(int width);
    void setHeight(int height);
    void setBoxDensity(double box_density);
    void setPreventMistakes(bool prevent_mistakes);
    void setLevel(KGameDifficultyLevel::StandardLevel level);
    void setCustomBgEnabled(bool enabled);
    void setCustomBgPath(const QString &path);
    void setFontColorSolved(const QString &color);
    void setFontColorUnsolved(const QString & color);

    void sync();

    /** Raw access to the underlying QSettings object.
     *  TODO: Wrap all accesses in custom functions and remove this. */
    QSharedPointer<QSettings> qSettings() const { return m_qsettings; }

    static Settings *instance();

Q_SIGNALS:
    void settingChanged(Settings::SettingsType type);

private:
    Settings();
    Q_DISABLE_COPY(Settings)

    void restore();
    void setValue(SettingsType type, const QVariant &value);

    QList<QString> m_keys;

    int m_width, m_height;
    double m_box_density;
    bool m_prevent_mistakes;
    KGameDifficultyLevel::StandardLevel m_level;
    bool m_custom_bg_enabled;
    QString m_custom_bg_path;
    QString m_font_color_unsolved, m_font_color_solved;

    QSharedPointer<QSettings> m_qsettings;
};

#endif // SETTINGS_H
