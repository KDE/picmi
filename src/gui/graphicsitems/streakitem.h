/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef STREAKITEM_H
#define STREAKITEM_H

#include <QGraphicsTextItem>

#include "reloadableitem.h"
#include "src/gui/renderer.h"
#include "src/logic/picmi.h"

class StreakItem : public QGraphicsTextItem, public ReloadableItem
{
    Q_OBJECT
public:
    StreakItem(int x, int y, QSharedPointer<Picmi> game, QGraphicsItem *parent = nullptr);

    /* update displayed streak */
    virtual void refresh() = 0;

    void reload(const QSize &size) override = 0;

private Q_SLOTS:
    void settingChanged(Settings::SettingsType type);

protected:
    int padding(int tilesize) const;
    void setFontColors();

    const QSharedPointer<Picmi> m_game;
    QString m_color_solved, m_color_unsolved;
};

class RowStreakItem : public StreakItem {
public:
    RowStreakItem(QSharedPointer<Picmi> game, int y, QGraphicsItem *parent = nullptr);

    void refresh() override;
    void reload(const QSize &size) override;
};

class ColStreakItem : public StreakItem {
public:
    ColStreakItem(QSharedPointer<Picmi> game, int x, QGraphicsItem *parent = nullptr);

    void refresh() override;
    void reload(const QSize &size) override;
};

#endif // STREAKITEM_H
