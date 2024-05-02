/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "textbanneritem.h"

#include <KLocalizedString>

#include "src/constants.h"

TextBannerItem::TextBannerItem(QGraphicsItem *parent) :
    QGraphicsTextItem(parent), ReloadableItem(0, 0)
{
    setEnabled(false);
    setFont(Renderer::instance()->getFont(Renderer::Large));
    setZValue(ZVALUE_BANNER);
}

void TextBannerItem::reload(const QSize &size) {
    Q_UNUSED(size);
    setFont(Renderer::instance()->getFont(Renderer::Large));
}

PauseBannerItem::PauseBannerItem(QGraphicsItem *parent) : TextBannerItem(parent)
{
    setPlainText(i18n("PAUSED"));
    setVisible(false);
}

void PauseBannerItem::reload(const QSize &size) {
    TextBannerItem::reload(size);
    QRectF rect = boundingRect();
    setPos((size.width() - rect.width()) / 2,
           (size.height() - rect.height()) / 2);
}
