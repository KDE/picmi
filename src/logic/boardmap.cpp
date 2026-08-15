/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "boardmap.h"

#include <algorithm>
#include <numeric>

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
    Board(width, height),
    m_box_count(qBound(0, static_cast<int>(width * height * box_ratio), m_size))
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
    /* Pick m_box_count cells uniformly at random from m_size. The
     * previous implementation attempted reservoir sampling but the
     * probabilities were off. Shuffle-and-take is correct and easy
     * to read. */

    QList<int> indices(m_size);
    std::iota(indices.begin(), indices.end(), 0);

    auto *generator = QRandomGenerator::global();
    for (int i = m_size - 1; i > 0; i--) {
        const int j = generator->bounded(i + 1);
        std::swap(indices[i], indices[j]);
    }

    for (int i = 0; i < m_box_count; i++) {
        m_state[indices[i]] = Box;
    }
}
