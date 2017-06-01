/* *************************************************************************
 *  Copyright 2015 Jakob Gruber <jakob.gruber@gmail.com>                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */


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
    StreakItem(int x, int y, QSharedPointer<Picmi> game, QGraphicsItem *parent = 0);

    /* update displayed streak */
    virtual void refresh() = 0;

    void reload(const QSize &size) Q_DECL_OVERRIDE = 0;

private slots:
    void settingChanged(Settings::SettingsType type);

protected:
    int padding(int tilesize) const;
    void setFontColors();

    const QSharedPointer<Picmi> m_game;
    QString m_color_solved, m_color_unsolved;
};

class RowStreakItem : public StreakItem {
public:
    RowStreakItem(QSharedPointer<Picmi> game, int y, QGraphicsItem *parent = 0);

    void refresh() Q_DECL_OVERRIDE;
    void reload(const QSize &size) Q_DECL_OVERRIDE;
};

class ColStreakItem : public StreakItem {
public:
    ColStreakItem(QSharedPointer<Picmi> game, int x, QGraphicsItem *parent = 0);

    void refresh() Q_DECL_OVERRIDE;
    void reload(const QSize &size) Q_DECL_OVERRIDE;
};

#endif // STREAKITEM_H
