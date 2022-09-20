/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef STREAKS_H
#define STREAKS_H

#include <QVector>
#include <QSharedPointer>

#include "boardmap.h"
#include "boardstate.h"

class Streaks
{
public:
    struct Streak {
        Streak() : solved(false) { }
        int value;
        bool solved;
    };

    Streaks(QSharedPointer<BoardMap> map, QSharedPointer<BoardState> state);

    /* Updates streaks affected by changes to (x,y). */
    void update(int x, int y);

    /* Updates streaks, taking the entire board into account. */
    void update();

    /* Returns the request row/col streak. These contain the least information required by
       the frontend, which is (for each position within a streak): "which number is this",
       and "is this position solved" */
    QVector<Streaks::Streak> getRowStreak(int y) const;
    QVector<Streaks::Streak> getColStreak(int x) const;

private: /* Types. */
    struct StreakPrivate : public Streak {
        int begin;  /**< The first index of this streak. */
        int end;    /**< The first index after this streak. */
    };

private: /* Functions. */
    /* Takes a sequence of states and returns streaks. */
    static QVector<StreakPrivate> lineToStreaks(const QVector<Board::State> &line,
                                                Board::State filler);

    /** Given a map streak sequence as well as the current state of the associated line,
     *  processStreak() returns the state streaks. */
    static QVector<Streak> processStreak(const QVector<StreakPrivate> &map,
                                         const QVector<Board::State> &l);

private: /* Variables. */
    QSharedPointer<BoardMap> m_map;
    QSharedPointer<BoardState> m_state;

    /** Map streaks are calculated once upon construction and then stay immutable
     *  for the remaining lifetime of a Streaks object. They store information about
     *  the actual map streaks, including the streak beginning index, end index, and
     *  length. */
    QVector<QVector<StreakPrivate> > m_map_row_streaks;
    QVector<QVector<StreakPrivate> > m_map_col_streaks;

    /** State streaks are recomputed whenever the state of an associated cell changes.
     *  Unlike the map streaks, row streaks only store the publicly available information:
     *  the length of a streak, and whether it's solved or not. */
    QVector<QVector<Streak> > m_state_row_streaks;
    QVector<QVector<Streak> > m_state_col_streaks;
};

#endif // STREAKS_H
