/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include <QObject>
#include <QPoint>
#include <QStack>
#include <QSharedPointer>

#include "board.h"

class BoardState : public QObject, public Board
{
    Q_OBJECT
public:
    /* initializes an empty field of given width and height,
      and creates initial (empty) streaks.
      width, height > 0 */
    BoardState(int width, int height);

    /* 0 <= x < m_width; 0 <= y < m_height
      sets point (x, y) to state */
    void set(int x, int y, enum State state);

    /* undo the last stored action and return the changed coordinate.
       if none are left, nothing occurs and QPoint() is returned*/
    QPoint undo();

    /* save the current state and push it onto the state stack. */
    void saveState();

    /* pop and load a state if state stack is non-empty.
       otherwise, do nothing */
    void loadState();

    /* returns the age (in steps) of the current game state
       compared to the latest saved state. for example, if the
       current undo_queue.size() == 15, and the top of saved_states
       is 10, currentStateAge returns 5. */
    int currentStateAge() const;

    /* returns the count of player-set boxes */
    int boxCount() const { return m_box_count; }

    /* replaces all occurrences of prev with next.
       bookkeeping information (undo history, box count) is _not_ updated */
    void replace(enum State prev, enum State next);

    /* Copies the board to this state. Like replace(), bookkeeping
       information is not updated since this is the final action in a game. */
    void solve(const Board *board);

Q_SIGNALS:
    void undoStackSizeChanged(int size);
    void saveStackSizeChanged(int size);

private:

    /* updates the box count according to old state prev and incoming state next. */
    void updateBoxCount(Board::State prev, Board::State next);

private:

    struct UndoAction {
        int x, y;
        enum State state;
    };

    QStack<UndoAction> m_undo_queue;
    QStack<int> m_saved_states;     /* size of m_undo_queue when state was saved */

    int m_box_count;
};

#endif // BOARDSTATE_H
