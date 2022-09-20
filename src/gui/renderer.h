/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef RENDERER_H
#define RENDERER_H

#include <QFont>
#include <QPixmap>
#include <QString>
#include <QVector>
#include <QSvgRenderer>
#include <QSharedPointer>

class Renderer
{
public:
    enum Resource {
        Transparent = 0,
        Background,
        CellFrame,
        Box,
        Cross,
        Highlight,
        Streak1,
        Streak2,
        Divider,
        OverviewBox,
        OverviewCross
    };

    enum FontSize {
        Tiny,
        Small,
        Regular,
        Large,
        Huge,
        FontSizeLength
    };

    static Renderer *instance();

    /* returns the pixmap representing the given resource at the
      current tilesize */
    QPixmap getPixmap(Renderer::Resource res) const;

    int getTilesize() const;
    int getOverviewTilesize() const;

    const QFont &getFont(enum FontSize size) const;

    /* returns the width of streak areas in pixels */
    int getYOffset() const;
    int getXOffset() const;

    /* returns the width of streak areas in tiles */
    int getStreakGridCount() const;

    /* 0 < board_width, board_height */
    void setSize(const QSize &size, int board_width, int board_height,
                 const QStringList &streaks);

private:
    Renderer();

    QPixmap getCachedPixmap(Renderer::Resource resource, int h, int w) const;
    void loadResources();

    /* calculates the largest tile size such that the entire game area fits
      into the provided window size */
    int gridSize(const QSize &size, int board_width, int board_height) const;

    /* Returns true if the given streaks fit into the current area specified
       by m_streak_count and m_tilesize. */
    bool streaksFit(const QStringList &streaks) const;

    void setFontSize();

private:

    int m_tilesize;
    int m_overview_tilesize;
    int m_streak_grid_count;

    QFont m_fonts[FontSizeLength];

    QSharedPointer<QSvgRenderer> m_renderer;

    QVector<QString> m_names;
};

#endif // RENDERER_H
