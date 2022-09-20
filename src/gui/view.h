/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QResizeEvent>

#include "scene.h"
#include "src/logic/picmi.h"

class View : public QGraphicsView
{
public:
    explicit View(QWidget *parent = nullptr);

    /* sets up the scene and returns a pointer to it */
    QSharedPointer<Scene> createScene(QSharedPointer<Picmi> game);
    void setPaused(bool paused);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QSharedPointer<Scene> m_scene;
};

#endif // VIEW_H
