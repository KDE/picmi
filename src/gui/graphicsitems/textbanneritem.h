/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef TEXTBANNERITEM_H
#define TEXTBANNERITEM_H

#include <QGraphicsSimpleTextItem>

#include "reloadableitem.h"

class TextBannerItem : public QGraphicsTextItem, public ReloadableItem
{
public:
    explicit TextBannerItem(QGraphicsItem *parent = nullptr);

    void reload(const QSize &size) override;
};

class PauseBannerItem : public TextBannerItem
{
public:
    explicit PauseBannerItem(QGraphicsItem *parent = nullptr);

    void reload(const QSize &size) override;
};

#endif // TEXTBANNERITEM_H
