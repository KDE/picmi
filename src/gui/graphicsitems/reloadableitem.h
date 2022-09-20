/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef RELOADABLEITEM_H
#define RELOADABLEITEM_H

#include <QSize>

#include "src/gui/renderer.h"

class ReloadableItem
{
public:
    /* constructs item with field coordinates (x,y) */
    ReloadableItem(int x, int y);

    virtual ~ReloadableItem() { }

    /* Called with view dimensions when window has been resized and
       item needs to reload and reposition itself correctly. */
    virtual void reload(const QSize &size) = 0;
protected:
    const int m_x, m_y;
};

#endif // RELOADABLEITEM_H
