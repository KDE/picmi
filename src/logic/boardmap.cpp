/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "boardmap.h"

#include <qglobal.h>
#include <QList>
#include <QRandomGenerator>

static int box_count(const QList<Board::State> &data) {
    int count = 0;
    for (int i = 0; i < data.size(); i++) {
        if (data[i] != Board::Nothing) {
            count++;
        }
    }
    return count;
}

BoardMap::BoardMap(int width, int height, double box_ratio) :
    Board(width, height), m_box_count(width * height * box_ratio)
{
    genRandom();
}

BoardMap::BoardMap(int width, int height, const QList<Board::State> &map) :
    Board(width, height), m_box_count(box_count(map))
{
    for (int i = 0; i < map.size(); i++) {
        m_state[i] = map[i];
    }
}

void BoardMap::genRandom() {
    /* To maintain a uniformly random selection of k elements:
     * element i enters the selection with probability k/i. */

    QList<int> indices(m_box_count);
    auto *generator = QRandomGenerator::global();
    for (int i = 0; i < m_size; i++) {
        if (i < m_box_count) {
            indices[i] = i;
            continue;
        }

        if (generator->bounded(i) <= m_box_count) {
            indices[generator->bounded(indices.size())] = i;
        }
    }

    for (int i = 0; i < indices.size(); i++) {
        m_state[indices[i]] = Box;
    }
}
