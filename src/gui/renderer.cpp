/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <config.h>
#include "renderer.h"

#include <QDir>
#include <QFile>
#include <QPainter>
#include <QPixmapCache>
#include <QStandardPaths>
#include <assert.h>
#include <iostream>

#include "src/constants.h"
#include "src/outofboundsexception.h"
#include "src/logic/settings.h"
#include "src/systemexception.h"

#define MIN_STREAK_COUNT (4)

Renderer::Renderer() : m_tilesize(47), m_overview_tilesize(12),
    m_streak_grid_count(6)
{
    loadResources();

    m_names << QStringLiteral("transparent") << QStringLiteral("background") << QStringLiteral("cellframe")
            << QStringLiteral("box") << QStringLiteral("cross") << QStringLiteral("highlight") << QStringLiteral("streak1")
            << QStringLiteral("streak2") << QStringLiteral("divider") << QStringLiteral("overview_box")
            << QStringLiteral("overview_cross");
}

void Renderer::loadResources() {

    /* Fonts. */

    for (int i = 0; i < FontSizeLength; i++) {
        m_fonts[i].setPointSize(24);
    }

    /* Tiles. */

    const QString prefix = QStringLiteral("themes/");
    QList<QString> paths;
    paths << QString(prefix)
          << QStringLiteral(FILEPATH) + QStringLiteral("/") + prefix
          << QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                    prefix,
                                    QStandardPaths::LocateOption::LocateDirectory);

    /* try loading first from working directory, then the system directories */
    for (int i = 0; i < paths.size(); i++) {
        const QString filenameSvg = QDir::toNativeSeparators(paths[i] + QStringLiteral("picmi.svg"));

        if (!QFile::exists(filenameSvg)) {
            continue;
        }
        m_renderer = QSharedPointer<QSvgRenderer>(new QSvgRenderer(filenameSvg));
        return;
    }

    throw SystemException(QStringLiteral("Resources not found"));
}

int Renderer::gridSize(const QSize &size, int board_width, int board_height) const {
    int grid = size.width() / (board_width + m_streak_grid_count);

    if ((board_height + m_streak_grid_count) * grid > size.height()) {
        grid = size.height() / (board_height + m_streak_grid_count);
    }

    return grid;
}

bool Renderer::streaksFit(const QStringList &streaks) const {
    QFontMetrics fm(m_fonts[Regular]);

    /* Subtract a little from real size to account for padding. */
    const int len = m_streak_grid_count * m_tilesize - m_tilesize;
    const int limit = 8 * m_tilesize;
    const QRect limrect(0, 0, limit, limit);

    for (const QString &str : streaks) {
        /* QFontMetrics.boundingRect defaults to Qt::SingleLine, which handles \n
           as a normal character instead of a line break. Manually specify flags. */
        QRect rect = fm.boundingRect(limrect, Qt::AlignLeft | Qt::AlignTop, str);
        if (qMax(rect.width(), rect.height()) > len) {
            return false;
        }
    }

    return true;
}

void Renderer::setSize(const QSize &size, int board_width, int board_height,
                       const QStringList &streaks) {
    if (board_width < 0 || board_height < 0) {
        throw OutOfBoundsException();
    }

    /* Calculate the tile size, given the window size, the board dimensions,
       and the list of streak strings. The tile size must be the largest value
       such that all streaks will still fit into
       m_streak_grid_count * m_tilesize.

       Start with the default minimum grid size, and keep
       recalculating the tile size until all streaks fit. */

    m_streak_grid_count = MIN_STREAK_COUNT - 1;
    do {
        m_streak_grid_count++;
        m_tilesize = gridSize(size, board_width, board_height);
        setFontSize();
    } while (!streaksFit(streaks));

    /* the overview is a square area at the top left of the field with dimensions
       getXOffset() x getYOffset(). using the same logic as for calculating the
       main tilesize, get the overview tilesize such that the entire board fits */

    const int buffer = 15;
    QSize overview_size(getXOffset() - buffer, getYOffset() - buffer);
    m_overview_tilesize = gridSize(overview_size, board_width, board_height);
}

int Renderer::getOverviewTilesize() const {
    return m_overview_tilesize;
}

const QFont &Renderer::getFont(enum FontSize size) const {
    return m_fonts[size];
}

#define MIN_FONT_SIZE (5)

void Renderer::setFontSize() {
    int size;

    size = (m_tilesize - 10) * 0.5 + 5;
    m_fonts[Regular].setPointSize(qMax(MIN_FONT_SIZE, size));

    size = (m_tilesize - 10) * 0.75 + 7;
    m_fonts[Large].setPointSize(qMax(MIN_FONT_SIZE, size));
}

Renderer *Renderer::instance() {
    static Renderer instance;
    return &instance;
}

QPixmap Renderer::getPixmap(Renderer::Resource resource) const {
    switch (resource) {
    case Background: return getCachedPixmap(resource, 1200, 1920);
    case Streak1:
    case Streak2: return getCachedPixmap(resource, m_tilesize, m_tilesize * m_streak_grid_count);
    case OverviewBox:
    case OverviewCross: return getCachedPixmap(resource, m_overview_tilesize, m_overview_tilesize);
    default: return getCachedPixmap(resource, m_tilesize, m_tilesize);
    }
}

QPixmap Renderer::getCachedPixmap(Renderer::Resource resource, int h, int w) const
{
    /* Special case for custom background. */
    if (resource == Background) {
        if (Settings::instance()->customBgEnabled()) {
            return QPixmap(Settings::instance()->customBgPath());
        }
    }

    QString key = QStringLiteral("%1:%2x%3").arg(m_names[resource]).arg(w).arg(h);

    QPixmap pixmap;
    if (!QPixmapCache::find(key, &pixmap)) {
        pixmap = QPixmap(w, h);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        m_renderer->render(&painter, m_names[resource], QRectF(0, 0, w, h));
        painter.end();
        QPixmapCache::insert(key, pixmap);
    }

    return pixmap;
}

int Renderer::getTilesize() const {
    return m_tilesize;
}

int Renderer::getYOffset() const {
    return m_streak_grid_count * m_tilesize;
}

int Renderer::getXOffset() const {
    return m_streak_grid_count * m_tilesize;
}

int Renderer::getStreakGridCount() const {
    return m_streak_grid_count;
}
