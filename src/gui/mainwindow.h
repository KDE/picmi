/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <config.h>

#include <QTimer>
#include <highscore/kscoredialog.h>
#include <QAction>
#include <KToggleAction>
#include <KXmlGuiWindow>
#include <KGameDifficulty>

#include "view.h"

class Level;
class Picmi;
class Scene;
class QLabel;

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private Q_SLOTS:
    void startRandomGame();
    void restartGame();
    void togglePaused(bool paused);
    void settings();
    void gameCompleted();
    void gameWon();
    void undo();
    void hint();
    void solve();
    void saveState();
    void loadState();
    void highscores();
    void levelChanged(const KGameDifficultyLevel* level);
    void updatePlayedTime();
    void updatePositions();
    void loadBoard();
    void toggleFullscreen(bool full_screen);

    /* Enable or disable undo/save state related actions. */
    void undoStackSizeChanged(int size);
    void saveStackSizeChanged(int size);

private:
    enum Mode {
        Random, /* board is randomly generated, highscores enabled */
        Preset  /* board is fixed, highscores disabled */
    };

    void startGame();
    void startPresetGame(QSharedPointer<Level> board);

    void restoreWindowState();
    void saveWindowState();
    void pauseGame();
    QSharedPointer<KScoreDialog> createScoreDialog();
    void setupActions();

    QAction *m_action_undo,
            *m_action_save_state,
            *m_action_load_state,
            *m_action_hint,
            *m_action_solve;
    KToggleAction *m_action_pause;
    QLabel *m_status_time, *m_status_position;
    QPushButton *m_new_game, *m_load_game;
    View m_view;
    QSharedPointer<Picmi> m_game;
    QSharedPointer<Scene> m_scene;
    QTimer m_timer;

    const QString m_key_pos;

    bool m_in_progress;
    enum Mode m_mode;

    QSharedPointer<Level> m_current_level;
};

#endif // MAINWINDOW_H
