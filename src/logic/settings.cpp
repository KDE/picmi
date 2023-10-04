/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "settings.h"

Settings::Settings() {

    m_keys << QStringLiteral("game/width")
           << QStringLiteral("game/height")
           << QStringLiteral("game/box_ratio")
           << QStringLiteral("game/prevent_mistakes")
           << QStringLiteral("game/level")
           << QStringLiteral("game/custom_bg_enabled")
           << QStringLiteral("game/custom_bg_path")
           << QStringLiteral("game/font_color_solved")
           << QStringLiteral("game/font_color_unsolved");

    /* We explicitly pass "picmi" as the organization name in order to
     * access the correct settings.
     * This has historical reason in that we used to set the organization
     * name globally, which however caused problems in
     * QStandardPaths::locate().
     */
    m_qsettings = QSharedPointer<QSettings>(new QSettings(QStringLiteral("picmi"), QStringLiteral("picmi")));
    restore();
}

int Settings::height() const {
    return m_height;
}

int Settings::width() const {
    return m_width;
}

double Settings::boxDensity() const {
    return m_box_density;
}

bool Settings::preventMistakes() const {
    return m_prevent_mistakes;
}

KGameDifficultyLevel::StandardLevel Settings::level() const {
    return m_level;
}

bool Settings::customBgEnabled() const
{
    return m_custom_bg_enabled;
}

QString Settings::customBgPath() const
{
    return m_custom_bg_path;
}

QString Settings::fontColorSolved() const
{
    return m_font_color_solved;
}

QString Settings::fontColorUnsolved() const
{
    return m_font_color_unsolved;
}

void Settings::setWidth(int width) {
    m_width = width;
    setValue(Width, width);
}

void Settings::setHeight(int height) {
    m_height = height;
    setValue(Height, height);
}

void Settings::setBoxDensity(double box_density) {
    m_box_density = box_density;
    setValue(BoxDensity, box_density);
}

void Settings::setPreventMistakes(bool prevent_mistakes) {
    m_prevent_mistakes = prevent_mistakes;
    setValue(PreventMistakes, prevent_mistakes);
}

void Settings::setLevel(KGameDifficultyLevel::StandardLevel level) {
    m_level = level;
    setValue(Level, level);
}

void Settings::setCustomBgEnabled(bool enabled)
{
    m_custom_bg_enabled = enabled;
    setValue(CustomBgEnabled, enabled);
}

void Settings::setCustomBgPath(const QString &path)
{
    m_custom_bg_path = path;
    setValue(CustomBgPath, path);
}

void Settings::setFontColorSolved(const QString &color)
{
    m_font_color_solved = color;
    setValue(FontColorSolved, color);
}

void Settings::setFontColorUnsolved(const QString &color)
{
    m_font_color_unsolved = color;
    setValue(FontColorUnsolved, color);
}

void Settings::restore() {
    m_width = m_qsettings->value(m_keys[Width], 15).toInt();
    m_height = m_qsettings->value(m_keys[Height], 10).toInt();
    m_box_density = m_qsettings->value(m_keys[BoxDensity], 0.55).toDouble();
    m_prevent_mistakes = m_qsettings->value(m_keys[PreventMistakes], false).toBool();
    m_level = (KGameDifficultyLevel::StandardLevel)m_qsettings->value(m_keys[Level],
        KGameDifficultyLevel::Medium).toInt();
    m_custom_bg_enabled = m_qsettings->value(m_keys[CustomBgEnabled], false).toBool();
    m_custom_bg_path = m_qsettings->value(m_keys[CustomBgPath], QString()).toString();
    m_font_color_solved = m_qsettings->value(m_keys[FontColorSolved], QStringLiteral("#5500ff")).toString();
    m_font_color_unsolved = m_qsettings->value(m_keys[FontColorUnsolved], QStringLiteral("#000000")).toString();
}

void Settings::setValue(SettingsType type, const QVariant &value)
{
    m_qsettings->setValue(m_keys[type], value);
    Q_EMIT settingChanged(type);
}

void Settings::sync()
{
    m_qsettings->sync();
}

Settings *Settings::instance()
{
    static Settings settings;
    return &settings;
}

#include "moc_settings.cpp"
