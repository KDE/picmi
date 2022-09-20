/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include <QGraphicsPixmapItem>

#include "reloadableitem.h"
#include "src/logic/settings.h"

class PixmapItem : public QGraphicsPixmapItem, public ReloadableItem
{
public:
    PixmapItem(Renderer::Resource resource, int x, int y, QGraphicsItem *parent = nullptr);
    void reload(const QSize &size) override;

protected:
    const Renderer::Resource m_resource;
};

class BackgroundItem : public QObject, public PixmapItem
{
    Q_OBJECT
public:
    BackgroundItem(Renderer::Resource resource, int x, int y, QGraphicsItem *parent = nullptr);

private Q_SLOTS:
    void settingChanged(Settings::SettingsType type);
};

class StreakHBackgroundItem : public PixmapItem
{
public:
    StreakHBackgroundItem(Renderer::Resource resource,int y, QGraphicsItem *parent = nullptr);
    void reload(const QSize &size) override;
};

class StreakVBackgroundItem : public PixmapItem
{
public:
    StreakVBackgroundItem(Renderer::Resource resource,int x, QGraphicsItem *parent = nullptr);
    void reload(const QSize &size) override;
};

#endif // PIXMAPITEM_H
