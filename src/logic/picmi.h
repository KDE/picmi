/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PICMI_H
#define PICMI_H

#include <KGameHighScoreDialog>

#include "boardmap.h"
#include "boardstate.h"
#include "elapsedtime.h"
#include "settings.h"
#include "streaks.h"

class Picmi : public QObject
{
    Q_OBJECT
public:

    Picmi();
    explicit Picmi(QSharedPointer<BoardMap> board);

    int width() const;
    int height() const;
    int remainingBoxCount() const { return m_map->boxCount() - m_state->boxCount(); }
    QSharedPointer<BoardMap> getBoardMap() const { return m_map; }
    bool outOfBounds(int x, int y) const;

    /* 0 <= x < width(); 0 <= y < height() */
    Board::State stateAt(int x, int y) const { return m_state->get(x, y); }
    void setState(int x, int y, Board::State state);

    void setPaused(bool paused);
    int elapsedSecs() const;

    /* ends the current game and returns the current high score object */
    KGameHighScoreDialog::FieldInfo endGame();

    /* undo last action (if it exists) and return the changed coordinate. */
    QPoint undo();

    /* Uncovers a single, random, still uncovered cell. */
    QPoint hint();

    /* Solves the entire board, but does not emit the gameWon signal. */
    void solve();

    /* if a saved state exists, load it. otherwise, do nothing */
    void loadState() { m_state->loadState(); m_streaks->update(); Q_EMIT stateChanged(); }
    void saveState() { m_state->saveState(); }
    int currentStateAge() const { return m_state->currentStateAge(); }

    /* returns the request row/col streak. these contain the least information required by
      the frontend, which is (for each position within a streak): "which number is this",
      and "is this position solved" */
    QList<Streaks::Streak> getRowStreak(int y) const;
    QList<Streaks::Streak> getColStreak(int x) const;

Q_SIGNALS:
    /** Emitted when the game has been completed in any way. Also triggered if "Solve" was used. */
    void gameCompleted();

    /** Emitted when the game has actually been won (i.e. "Solve" wasn't used). */
    void gameWon();

    /**
     * Emitted whenever the board state has changed (i.e. by undoing an action,
     * setting a state explicitly, or loading a saved state).
     */
    void stateChanged();

    void undoStackSizeChanged(int size);
    void saveStackSizeChanged(int size);

private:

    /* returns true if the game has been won */
    bool won() const;

    /* Applies a player Box/Cross request, honoring prevent-mistakes mode. */
    void applyRequest(int x, int y, Board::State request);

    void setupSlots();

private:
    QSharedPointer<BoardMap> m_map;
    QSharedPointer<BoardState> m_state;
    QSharedPointer<Streaks> m_streaks;
    bool m_prevent_mistakes;

    ElapsedTime m_timer;
};

#endif // PICMI_H
