/* *************************************************************************
 *  Copyright 2015 Jakob Gruber <jakob.gruber@gmail.com>                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */

#include <QRandomGenerator>
#include <config.h>
#include "picmi.h"

#include <assert.h>

void IOHandler::set(int x, int y, Board::State state) {
    switch (state) {
    case Board::Cross: setCross(x, y); break;
    case Board::Box: setBox(x, y); break;
    default: assert(0);
    }
}

void IOHandler::setCross(int x, int y) {
    switch (m_state->get(x, y)) {
    case Board::Cross: m_state->set(x, y, Board::Nothing); break;
    case Board::Box: break;
    case Board::Nothing: m_state->set(x, y, Board::Cross); break;
    default: assert(0);
    }
}

class IOHandlerNoHints : public IOHandler
{
public:
    IOHandlerNoHints(BoardMap *map, BoardState *state, ElapsedTime *timer) : IOHandler(map, state, timer) { }

protected:
    void setBox(int x, int y) Q_DECL_OVERRIDE;
};


void IOHandlerNoHints::setBox(int x, int y) {
    switch (m_state->get(x, y)) {
    case Board::Cross: break;
    case Board::Box: m_state->set(x, y, Board::Nothing); break;
    case Board::Nothing: m_state->set(x, y, Board::Box); break;
    default: assert(0);
    }
}

class IOHandlerHints : public IOHandler
{
public:
    IOHandlerHints(BoardMap *map, BoardState *state, ElapsedTime *timer) : IOHandler(map, state, timer) { }

protected:
    void setBox(int x, int y) Q_DECL_OVERRIDE;
};


void IOHandlerHints::setBox(int x, int y) {
    switch (m_state->get(x, y)) {
    case Board::Cross: break;
    case Board::Box: m_state->set(x, y, Board::Nothing); break;
    case Board::Nothing:
        if (m_map->get(x, y) == Board::Box) {
            m_state->set(x, y, Board::Box);
        } else {
            m_state->set(x, y, Board::Cross);
            m_timer->addPenaltyTime();
        }
        break;
    default: assert(0);
    }
}

Picmi::Picmi()
{
    int width, height;
    double density;
    bool prevent_mistakes;

    switch (Settings::instance()->level()) {
    case KgDifficultyLevel::Easy: width = height = 10; density = 0.55; prevent_mistakes = false; break;
    case KgDifficultyLevel::Medium: width = 15; height = 10; density = 0.55; prevent_mistakes = false; break;
    case KgDifficultyLevel::Hard: width = height = 15; density = 0.55; prevent_mistakes = false; break;
    case KgDifficultyLevel::Custom:
    default:
        width = Settings::instance()->width();
        height = Settings::instance()->height();
        density = Settings::instance()->boxDensity();
        prevent_mistakes = Settings::instance()->preventMistakes();
        break;
    }

    m_map = QSharedPointer<BoardMap>(new BoardMap(width, height, density));
    m_state = QSharedPointer<BoardState>(new BoardState(width, height));
    m_streaks = QSharedPointer<Streaks>(new Streaks(m_map, m_state));

    if (prevent_mistakes) {
        m_io_handler = QSharedPointer<IOHandler>(new IOHandlerHints(m_map.data(), m_state.data(), &m_timer));
    } else {
        m_io_handler = QSharedPointer<IOHandler>(new IOHandlerNoHints(m_map.data(), m_state.data(), &m_timer));
    }

    m_timer.start();

    setupSlots();
}

Picmi::Picmi(QSharedPointer<BoardMap> board) {
    m_map = board;
    m_state = QSharedPointer<BoardState>(new BoardState(board->width(), board->height()));
    m_streaks = QSharedPointer<Streaks>(new Streaks(m_map, m_state));
    m_io_handler = QSharedPointer<IOHandler>(new IOHandlerNoHints(m_map.data(), m_state.data(), &m_timer));
    m_timer.start();

    setupSlots();
}

void Picmi::setupSlots()
{
    connect(m_state.data(), &BoardState::undoStackSizeChanged, this, &Picmi::undoStackSizeChanged);
    connect(m_state.data(), &BoardState::saveStackSizeChanged, this, &Picmi::saveStackSizeChanged);
}

bool Picmi::won() const {
    /* detect a win by comparing streaks.
       the reason we don't use the raw map and state is because we can't guarantee
       that our generated puzzles have exactly one solution, but we can work around
       this by ending the game once all streaks are marked solved. */

    for (int x = 0; x < width(); x++) {
        QVector<Streaks::Streak> streak = getColStreak(x);
        for (int i = 0; i < (int)streak.size(); i++) {
            if (!streak[i].solved) {
                return false;
            }
        }
    }

    for (int y = 0; y < height(); y++) {
        QVector<Streaks::Streak> streak = getRowStreak(y);
        for (int i = 0; i < (int)streak.size(); i++) {
            if (!streak[i].solved) {
                return false;
            }
        }
    }

    return true;
}

QPoint Picmi::undo() {
    QPoint coord = m_state->undo();
    m_streaks->update();
    emit stateChanged();
    return coord;
}

QPoint Picmi::hint()
{
    QVector<QPoint> incorrect_cells;
    for (int x = 0; x != width(); ++x) {
        for (int y = 0; y != height(); ++y) {
            const Board::State m = m_map->get(x, y);
            const Board::State s = m_state->get(x, y);

            if ((m == Board::Box && s != Board::Box) ||
                (m == Board::Nothing && s != Board::Cross)) {
                incorrect_cells.push_back(QPoint(x, y));
            }
        }
    }

    if (incorrect_cells.size() == 0) {
        return QPoint(0, 0);
    }

    const int idx = QRandomGenerator::global()->bounded(incorrect_cells.size());
    const QPoint cell(incorrect_cells.at(idx));
    Board::State state = m_map->get(cell.x(), cell.y());
    if (state == Board::Nothing) {
        state = Board::Cross;
    }

    /* Clear the state in order to ensure the subsequent setState succeeds. */
    m_state->set(cell.x(), cell.y(), Board::Nothing);
    setState(cell.x(), cell.y(), state);
    m_timer.addPenaltyTime();
    return cell;
}

void Picmi::solve() {
    m_state->solve(m_map.data());
    m_streaks->update();
    endGame();
    emit gameCompleted();
}

KScoreDialog::FieldInfo Picmi::endGame() {
    m_timer.stop();

    KScoreDialog::FieldInfo score;
    score[KScoreDialog::Score].setNum(m_timer.elapsedSecs());
    score[KScoreDialog::Time] = Time(m_timer.elapsedSecs()).toString();
    score[KScoreDialog::Date] = m_timer.startDate().toString("dd MMM yyyy hh:mm");

    return score;
}

int Picmi::height() const {
    return m_map->height();
}

int Picmi::width() const {
    return m_map->width();
}

bool Picmi::outOfBounds(int x, int y) const {
    return m_map->outOfBounds(x, y);
}

void Picmi::setPaused(bool paused) {
    m_timer.pause(paused);
}

int Picmi::elapsedSecs() const {
    return m_timer.elapsedSecs();
}

void Picmi::setState(int x, int y, Board::State state) {
    m_io_handler->set(x, y, state);
    m_streaks->update(x, y);
    emit stateChanged();
    if (m_state->boxCount() == m_map->boxCount() && won()) {
        m_state->replace(Board::Nothing, Board::Cross);
        emit gameCompleted();
        emit gameWon();
    }
}

QVector<Streaks::Streak> Picmi::getRowStreak(int y) const {
    return m_streaks->getRowStreak(y);
}

QVector<Streaks::Streak> Picmi::getColStreak(int x) const {
    return m_streaks->getColStreak(x);
}
