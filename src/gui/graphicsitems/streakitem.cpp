/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "streakitem.h"

#include "src/constants.h"
#include "src/logic/settings.h"

StreakItem::StreakItem(int x, int y, QSharedPointer<Picmi> game, QGraphicsItem *parent) :
    QGraphicsTextItem(parent), ReloadableItem(x, y), m_game(game)
{
    setEnabled(false);
    setZValue(ZVALUE_STREAKTEXT);
    setFont(Renderer::instance()->getFont(Renderer::Regular));

    connect(Settings::instance(), &Settings::settingChanged,
            this, &StreakItem::settingChanged);
}

void StreakItem::settingChanged(Settings::SettingsType type)
{
    switch (type) {
    case Settings::FontColorSolved:
    case Settings::FontColorUnsolved: setFontColors(); break;
    default: break;
    }
}

void StreakItem::setFontColors()
{
    m_color_solved = Settings::instance()->fontColorSolved();
    m_color_unsolved = Settings::instance()->fontColorUnsolved();
    refresh();
}

int StreakItem::padding(int tilesize) const {
    return tilesize / 5;
}

RowStreakItem::RowStreakItem(QSharedPointer<Picmi> game, int y, QGraphicsItem *parent) :
    StreakItem(0, y, game, parent)
{
    setFontColors();
}

void RowStreakItem::refresh() {
    QVector<Streaks::Streak> streak = m_game->getRowStreak(m_y);
    QString text;

    for (int i = 0; i < (int)streak.size(); i++) {
        QString color = (streak[i].solved ? m_color_solved : m_color_unsolved);
        text.append(QStringLiteral("<font color=\"%1\">%2</font>").arg(color).arg(streak[i].value));
        if (i != (int)streak.size() - 1) {
            text.append(QStringLiteral(" "));
        }
    }

    setHtml(text);
    reload(QSize());
}

void RowStreakItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int tilesize = Renderer::instance()->getTilesize();

    setFont(Renderer::instance()->getFont(Renderer::Regular));

    QRectF rect = boundingRect();

    /* right align + a little padding */
    const int x = 0 - rect.width() - padding(tilesize);

    /* one per row, center text */
    const int y = tilesize * m_y + (tilesize - rect.height()) / 2;

    setPos(x, y);
}

ColStreakItem::ColStreakItem(QSharedPointer<Picmi> game, int x, QGraphicsItem *parent) :
    StreakItem(x, 0, game, parent)
{
    setFontColors();
}

void ColStreakItem::refresh() {
    QVector<Streaks::Streak> streak = m_game->getColStreak(m_x);
    QString text;

    for (int i = 0; i < (int)streak.size(); i++) {
        QString color = (streak[i].solved ? m_color_solved : m_color_unsolved);
        text.append(QStringLiteral("<font color=\"%1\">%2</font>").arg(color).arg(streak[i].value));
        if (i != (int)streak.size() - 1) {
            text.append(QStringLiteral("<br/>"));
        }
    }

    setHtml(text);
    reload(QSize());
}

void ColStreakItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int tilesize = Renderer::instance()->getTilesize();

    setFont(Renderer::instance()->getFont(Renderer::Regular));

    QRectF rect = boundingRect();

    const int x = tilesize * m_x + (tilesize - rect.width()) / 2;
    const int y = 0 - rect.height() - padding(tilesize);

    setPos(x, y);
}
