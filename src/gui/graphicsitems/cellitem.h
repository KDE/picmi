/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
    CellItem(int x, int y, QSharedPointer<Picmi> game, QGraphicsItem *parent = nullptr);

    /* updates displayed pixmap according to current cell state */
    virtual void refresh();

    void reload(const QSize &size) override;

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
    OverviewCellItem(int x, int y, QSharedPointer<Picmi> game, QGraphicsItem *parent = nullptr);

protected:
    int getTilesize() const override;
    QPixmap getPixmap() const override;
};

class GameCellItem : public QObject, public CellItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
public:
    /* creates the item with field coordinates (x,y) and the specified
      game and scene */
    GameCellItem(int x, int y, QSharedPointer<Picmi> game, Scene *scene, QGraphicsItem *parent = nullptr);

    void keyPressEvent(QKeyEvent *event) override;

    void refresh() override;
    void reload(const QSize &size) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    int getTilesize() const override;
    QPixmap getPixmap() const override;

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
