/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef HIGHLIGHTITEM_H
#define HIGHLIGHTITEM_H

#include "pixmapitem.h"

class HighlightItem : public PixmapItem
{
public:
    /* creates a highlight at field coordinates (x,y) */
    HighlightItem(Renderer::Resource resource, int x, int y, QGraphicsItem *parent = nullptr);

    /* switches current highlight on/off according to the currently
      focused point p */
    void highlight(const QPoint &p);
};

#endif // HIGHLIGHTITEM_H
