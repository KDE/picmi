/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "streaks.h"

#include <algorithm>
#include <optional>

/** 0 <= x < m_width; 0 <= y < m_height; returns a row as a sequence of states */
static QList<Board::State> colToLine(const QSharedPointer<Board> &board,
                                       int x)
{
    QList<Board::State> line;
    for (int y = 0; y < board->height(); y++) {
        line.push_back(board->get(x, y));
    }
    return line;
}

/** 0 <= x < m_width; 0 <= y < m_height; returns a row as a sequence of states */
static QList<Board::State> rowToLine(const QSharedPointer<Board> &board,
                                       int y)
{
    QList<Board::State> line;
    for (int x = 0; x < board->width(); x++) {
        line.push_back(board->get(x, y));
    }
    return line;
}

/* Streaks are generated using a small state machine: we are either
 * traversing filler (Nothing for the map line, Cross for the state line)
 * or inside a Box streak. Hitting a non-filler non-Box character ends
 * the line. */
enum class ScanState {
    Filler,
    Streak,
    End,
};

QList<Streaks::StreakPrivate>
Streaks::lineToStreaks(const QList<Board::State> &line,
                       Board::State filler)
{
    StreakPrivate s;
    QList<StreakPrivate> streaks;
    ScanState state = ScanState::Filler;

    for (int i = 0; i < line.size(); i++) {
        const Board::State t = line[i];

        switch (state) {
        case ScanState::Filler:
            if (t == Board::Box) {
                s.begin = i;
                s.value = 0;
                state = ScanState::Streak;
            } else if (t != filler) {
                state = ScanState::End;
            }
            break;
        case ScanState::Streak:
            if (t != Board::Box) {
                s.end = i;
                s.value = s.end - s.begin;
                streaks.append(s);
                state = (t == filler) ? ScanState::Filler : ScanState::End;
            }
            break;
        case ScanState::End:
            return streaks;
        }
    }

    if (state == ScanState::Streak) {
        s.end = line.size();
        s.value = s.end - s.begin;
        streaks.append(s);
    }

    return streaks;
}

QList<Streaks::Streak>
Streaks::processStreak(const QList<StreakPrivate> &map,
                       const QList<Board::State> &l)
{
    QList<Streaks::Streak> streak;
    QList<std::optional<StreakPrivate> > assocs(map.size());

    /* Initial values for returned streaks. */
    for (int i = 0; i < map.size(); i++) {
        streak.push_back(map[i]);
    }

    QList<StreakPrivate> streaks_regular = lineToStreaks(l, Board::Cross);

    QList<Board::State> line_reversed(l);
    std::reverse(line_reversed.begin(), line_reversed.end());
    QList<StreakPrivate> streaks_reversed = lineToStreaks(line_reversed, Board::Cross);

    /* Convert begin/end of reversed streaks back into forward coordinates. */
    for (int i = 0; i < streaks_reversed.size(); i++) {
        streaks_reversed[i].begin = l.size() - streaks_reversed[i].begin;
        streaks_reversed[i].end   = l.size() - streaks_reversed[i].end;
        std::swap(streaks_reversed[i].begin, streaks_reversed[i].end);
    }

    /* Bail out if the count of detected streaks is impossible for the
     * solution:
     *   1. More streaks than the map specifies in either direction.
     *   2. Fully-determined line (no Nothing) whose streak count
     *      differs from the map. Fixes https://bugs.kde.org/435211. */
    if (streaks_regular.size() > map.size() || streaks_reversed.size() > map.size()
            || (!l.contains(Board::Nothing) && (streaks_regular.size() != map.size()))) {
        return streak;
    }

    /* A streak is solved iff it is matched by exactly one of the two
     * directional scans, or matched by both with identical ranges. */
    for (int i = 0; i < streaks_regular.size(); i++) {
        streak[i].solved = (streak[i].value == streaks_regular[i].value);
        assocs[i] = streaks_regular[i];
    }

    for (int i = 0; i < streaks_reversed.size(); i++) {
        const int ix = map.size() - i - 1;
        streak[ix].solved = (streak[ix].value == streaks_reversed[i].value);

        if (assocs[ix].has_value()) {
            const bool range_matches =
                (assocs[ix]->begin == streaks_reversed[i].begin &&
                 assocs[ix]->end   == streaks_reversed[i].end);
            streak[ix].solved &= range_matches;
        }
    }

    return streak;
}

Streaks::Streaks(QSharedPointer<BoardMap> map, QSharedPointer<BoardState> state)
    : m_map(map), m_state(state)
{
    for (int x = 0; x < m_map->width(); x++) {
        QList<Board::State> line = colToLine(m_map, x);
        m_map_col_streaks.push_back(lineToStreaks(line, Board::Nothing));
    }

    for (int y = 0; y < m_map->height(); y++) {
        QList<Board::State> line = rowToLine(m_map, y);
        m_map_row_streaks.push_back(lineToStreaks(line, Board::Nothing));
    }

    update();
}

void Streaks::update(int x, int y) {
    m_state_col_streaks[x] = processStreak(m_map_col_streaks[x], colToLine(m_state, x));
    m_state_row_streaks[y] = processStreak(m_map_row_streaks[y], rowToLine(m_state, y));
}

void Streaks::update() {
    m_state_col_streaks.clear();
    for (int x = 0; x < m_state->width(); x++) {
        m_state_col_streaks.push_back(processStreak(m_map_col_streaks[x], colToLine(m_state, x)));
    }

    m_state_row_streaks.clear();
    for (int y = 0; y < m_state->height(); y++) {
        m_state_row_streaks.push_back(processStreak(m_map_row_streaks[y], rowToLine(m_state, y)));
    }
}

QList<Streaks::Streak> Streaks::getRowStreak(int y) const {
    return m_state_row_streaks[y];
}

QList<Streaks::Streak> Streaks::getColStreak(int x) const {
    return m_state_col_streaks[x];
}
