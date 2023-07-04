/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "pixmapitem.h"

#include "src/constants.h"

PixmapItem::PixmapItem(Renderer::Resource resource, int x, int y, QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent), ReloadableItem(x, y), m_resource(resource)
{
    setEnabled(false);
    reload(QSize());
}

void PixmapItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int tilesize = Renderer::instance()->getTilesize();
    setPixmap(Renderer::instance()->getPixmap(m_resource));
    setPos(m_x * tilesize, m_y * tilesize);
}

BackgroundItem::BackgroundItem(Renderer::Resource resource, int x, int y, QGraphicsItem *parent) :
    PixmapItem(resource, x, y, parent)
{
    connect(Settings::instance(), &Settings::settingChanged,
                     this, &BackgroundItem::settingChanged);
}

void BackgroundItem::settingChanged(Settings::SettingsType type) {
    switch (type) {
    case Settings::CustomBgEnabled:
    case Settings::CustomBgPath: reload(QSize()); break;
    default: break;
    }
}

StreakHBackgroundItem::StreakHBackgroundItem(Renderer::Resource resource, int y, QGraphicsItem *parent) :
    PixmapItem(resource, 0, y, parent)
{
    setZValue(ZVALUE_STREAKFRAME);
}

void StreakHBackgroundItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int tilesize = Renderer::instance()->getTilesize();
    const int offset = Renderer::instance()->getXOffset();

    setPixmap(Renderer::instance()->getPixmap(m_resource));
    setPos(0 - offset, m_y * tilesize);
}

StreakVBackgroundItem::StreakVBackgroundItem(Renderer::Resource resource, int x, QGraphicsItem *parent) :
    PixmapItem(resource, x, 0, parent)
{
    setZValue(ZVALUE_STREAKFRAME);
    setRotation(90);
}

void StreakVBackgroundItem::reload(const QSize &size) {
    Q_UNUSED(size);
    const int tilesize = Renderer::instance()->getTilesize();
    const int offset = Renderer::instance()->getYOffset();

    setPixmap(Renderer::instance()->getPixmap(m_resource));
    setPos((m_x + 1) * tilesize, 0 - offset);
}

#include "moc_pixmapitem.cpp"
