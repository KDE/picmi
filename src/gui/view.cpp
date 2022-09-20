/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "view.h"

View::View(QWidget *parent) :
    QGraphicsView(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    setAlignment( Qt::AlignLeft | Qt::AlignTop );
}

void View::resizeEvent(QResizeEvent *event) {
    if (event) {
        QGraphicsView::resizeEvent(event);
    }
    m_scene->resize(event->size());
}

void View::keyPressEvent(QKeyEvent *event) {
    /* make sure all key presses go to the currently selected cell item.
       without this, clicking on other areas of the scene causes cell items
       to lose focus.
       this seems like a hackish solution, please replace if there is
       a better way. */
    m_scene->forwardKeyPressEvent(event);
}

QSharedPointer<Scene> View::createScene(QSharedPointer<Picmi> game) {
    m_scene = QSharedPointer<Scene>(new Scene(game));
    setScene(m_scene.data());
    m_scene->resize(size());
    return m_scene;
}

void View::setPaused(bool paused) {
    setEnabled(!paused);
    setFocus();
    m_scene->setPaused(paused);
}
