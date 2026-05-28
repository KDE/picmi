/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QRandomGenerator>
#include <config.h>
#include "picmi.h"

Picmi::Picmi()
    : m_prevent_mistakes(false)
{
    int width, height;
    double density;

    switch (Settings::instance()->level()) {
    case KGameDifficultyLevel::Easy: width = height = 10; density = 0.55; break;
    case KGameDifficultyLevel::Medium: width = 15; height = 10; density = 0.55; break;
    case KGameDifficultyLevel::Hard: width = height = 15; density = 0.55; break;
    case KGameDifficultyLevel::Custom:
    default:
        width = Settings::instance()->width();
        height = Settings::instance()->height();
        density = Settings::instance()->boxDensity();
        m_prevent_mistakes = Settings::instance()->preventMistakes();
        break;
    }

    m_map = QSharedPointer<BoardMap>(new BoardMap(width, height, density));
    m_state = QSharedPointer<BoardState>(new BoardState(width, height));
    m_streaks = QSharedPointer<Streaks>(new Streaks(m_map, m_state));

    m_timer.start();

    setupSlots();
}

Picmi::Picmi(QSharedPointer<BoardMap> board)
    : m_prevent_mistakes(false)
{
    m_map = board;
    m_state = QSharedPointer<BoardState>(new BoardState(board->width(), board->height()));
    m_streaks = QSharedPointer<Streaks>(new Streaks(m_map, m_state));
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
        QList<Streaks::Streak> streak = getColStreak(x);
        for (int i = 0; i < (int)streak.size(); i++) {
            if (!streak[i].solved) {
                return false;
            }
        }
    }

    for (int y = 0; y < height(); y++) {
        QList<Streaks::Streak> streak = getRowStreak(y);
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
    Q_EMIT stateChanged();
    return coord;
}

QPoint Picmi::hint()
{
    QList<QPoint> incorrect_cells;
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
    Q_EMIT gameCompleted();
}

KGameHighScoreDialog::FieldInfo Picmi::endGame() {
    m_timer.stop();

    KGameHighScoreDialog::FieldInfo score;
    score[KGameHighScoreDialog::Score].setNum(m_timer.elapsedSecs());
    score[KGameHighScoreDialog::Time] = Time(m_timer.elapsedSecs()).toString();
    score[KGameHighScoreDialog::Date] = m_timer.startDate().toString(QStringLiteral("dd MMM yyyy hh:mm"));

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

void Picmi::applyRequest(int x, int y, Board::State request) {
    const Board::State current = m_state->get(x, y);

    if (request == Board::Cross) {
        if (current == Board::Cross) {
            m_state->set(x, y, Board::Nothing);
        } else if (current == Board::Nothing) {
            m_state->set(x, y, Board::Cross);
        }
        return;
    }

    Q_ASSERT(request == Board::Box);

    if (current == Board::Box) {
        m_state->set(x, y, Board::Nothing);
        return;
    }
    if (current == Board::Cross) {
        return;
    }

    Q_ASSERT(current == Board::Nothing);
    if (!m_prevent_mistakes) {
        m_state->set(x, y, Board::Box);
        return;
    }

    /* Prevent-mistakes mode: only allow Box where the map has one;
     * otherwise the cell becomes a cross and the player pays a penalty. */
    if (m_map->get(x, y) == Board::Box) {
        m_state->set(x, y, Board::Box);
    } else {
        m_state->set(x, y, Board::Cross);
        m_timer.addPenaltyTime();
    }
}

void Picmi::setState(int x, int y, Board::State state) {
    applyRequest(x, y, state);
    m_streaks->update(x, y);
    Q_EMIT stateChanged();
    if (m_state->boxCount() == m_map->boxCount() && won()) {
        m_state->replace(Board::Nothing, Board::Cross);
        Q_EMIT gameCompleted();
        Q_EMIT gameWon();
    }
}

QList<Streaks::Streak> Picmi::getRowStreak(int y) const {
    return m_streaks->getRowStreak(y);
}

QList<Streaks::Streak> Picmi::getColStreak(int x) const {
    return m_streaks->getColStreak(x);
}

#include "moc_picmi.cpp"
