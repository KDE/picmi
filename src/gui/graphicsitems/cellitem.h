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


#ifndef CELLITEM_H
#define CELLITEM_H

#include <QGraphicsPixmapItem>

#include "reloadableitem.h"
#include "src/logic/picmi.h"

class Scene;
class DragManager;
class QAbstractAnimation;

class CellItem : public QGraphicsPixmapItem, public ReloadableItem
{
public:
    CellItem(int x, int y, QSharedPointer<Picmi> game, QGraphicsItem *parent = 0);

    /* updates displayed pixmap according to current cell state */
    virtual void refresh();

    void reload(const QSize &size) Q_DECL_OVERRIDE;

protected:
    virtual int getTilesize() const = 0;
    virtual QPixmap getPixmap() const = 0;

    const QSharedPointer<Picmi> m_game;
};

class OverviewCellItem : public CellItem
{
public:
    /* creates the item with field coordinates (x,y) and the specified
      game and scene */
    OverviewCellItem(int x, int y, QSharedPointer<Picmi> game, QGraphicsItem *parent = 0);

protected:
    int getTilesize() const Q_DECL_OVERRIDE;
    QPixmap getPixmap() const Q_DECL_OVERRIDE;
};

class GameCellItem : public QObject, public CellItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
public:
    /* creates the item with field coordinates (x,y) and the specified
      game and scene */
    GameCellItem(int x, int y, QSharedPointer<Picmi> game, Scene *scene, QGraphicsItem *parent = 0);

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    void refresh() Q_DECL_OVERRIDE;
    void reload(const QSize &size) Q_DECL_OVERRIDE;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

    int getTilesize() const Q_DECL_OVERRIDE;
    QPixmap getPixmap() const Q_DECL_OVERRIDE;

private:
    /** Converts scene- to game coordinates. */
    QPoint sceneToGame(const QPointF &p) const;
    QAbstractAnimation *createAnimation();

private:
    Scene *m_scene;
    QSharedPointer<DragManager> m_dragmanager;
    Qt::MouseButton m_dragbutton;
    QPointF m_sceneorigin;

    /* Animation members. */
    Board::State m_state;
    QAbstractAnimation *m_anim;
};

#endif // CELLITEM_H
