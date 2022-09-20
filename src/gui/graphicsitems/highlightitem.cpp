/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "highlightitem.h"

#include "src/constants.h"

HighlightItem::HighlightItem(Renderer::Resource resource, int x, int y, QGraphicsItem *parent) :
    PixmapItem(resource, x, y, parent)
{
    setEnabled(false);
    setZValue(ZVALUE_HIGHLIGHT);
}

void HighlightItem::highlight(const QPoint &p) {
    bool highlighted = (p.x() == m_x || p.y() == m_y);
    setVisible(highlighted);
}
