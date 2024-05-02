/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"

#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>
#include <KGameStandardAction>
#include <KToggleFullScreenAction>
#include <QCoreApplication>
#include <QGraphicsSimpleTextItem>
#include <QPointer>
#include <QPushButton>
#include <QStatusBar>

#include "src/constants.h"
#include "src/logic/levelloader.h"
#include "src/logic/picmi.h"
#include "src/logic/settings.h"
#include "selectboardwindow.h"
#include "settingswindow.h"
#include "scene.h"

MainWindow::MainWindow(QWidget *parent) :
    KXmlGuiWindow(parent),
    m_key_pos(QStringLiteral("window/position")), m_in_progress(false), m_mode(Random)
{
    QCoreApplication::setApplicationName(QStringLiteral("picmi"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));

    m_timer.setInterval(500);

    setCentralWidget(&m_view);

    setupActions();
    restoreWindowState();

    startRandomGame();
}

void MainWindow::setupActions() {
    KGameStandardAction::gameNew(this, &MainWindow::startRandomGame, actionCollection());
    KGameStandardAction::load(this, &MainWindow::loadBoard, actionCollection());
    KGameStandardAction::restart(this, &MainWindow::restartGame, actionCollection());
    KGameStandardAction::highscores(this, &MainWindow::highscores, actionCollection());
    KGameStandardAction::quit(this, &MainWindow::close, actionCollection());
    KStandardAction::preferences(this, &MainWindow::settings, actionCollection());
    KStandardAction::fullScreen(this, &MainWindow::toggleFullscreen, this, actionCollection());
    m_action_pause = KGameStandardAction::pause(this, &MainWindow::togglePaused, actionCollection());
    m_action_undo = KGameStandardAction::undo(this, &MainWindow::undo, actionCollection());
    m_action_hint = KGameStandardAction::hint(this, &MainWindow::hint, actionCollection());
    m_action_solve = KGameStandardAction::solve(this, &MainWindow::solve, actionCollection());

    /* Prevent the default hint shortcut from overwriting our HJKL vim-like control mapping. */
    KActionCollection::setDefaultShortcut(m_action_hint, QKeySequence(Qt::CTRL | Qt::Key_I));

    m_action_save_state = actionCollection()->addAction(QStringLiteral("save-position"));
    m_action_save_state->setText(i18nc("@action", "Save Position"));
    m_action_save_state->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));
    KActionCollection::setDefaultShortcut(m_action_save_state, QKeySequence(Qt::CTRL | Qt::Key_S));
    connect(m_action_save_state, &QAction::triggered, this, &MainWindow::saveState);

    m_action_load_state = actionCollection()->addAction(QStringLiteral("load-position"));
    m_action_load_state->setText(i18nc("@action", "Load Position"));
    m_action_load_state->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh")));
    KActionCollection::setDefaultShortcut(m_action_load_state, QKeySequence(Qt::CTRL | Qt::Key_L));
    connect(m_action_load_state, &QAction::triggered, this, &MainWindow::loadState);

    m_status_time = new QLabel;
    m_status_time->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_status_position = new QLabel;
    m_status_position->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_new_game = new QPushButton;
    m_new_game->setText(i18nc("@action:button", "New Game"));
    m_new_game->setVisible(false);
    connect(m_new_game, &QAbstractButton::clicked, this, &MainWindow::startRandomGame);

    m_load_game = new QPushButton;
    m_load_game->setText(i18nc("@action:button", "Load New Game"));
    m_load_game->setVisible(false);
    connect(m_load_game, &QAbstractButton::clicked, this, &MainWindow::loadBoard);

    this->statusBar()->addWidget(m_new_game, 0);
    this->statusBar()->addWidget(m_load_game, 0);
    this->statusBar()->addWidget(m_status_position, 1);
    this->statusBar()->addWidget(m_status_time, 1);

    KGameDifficulty::global()->addStandardLevel(KGameDifficultyLevel::Easy);
    KGameDifficulty::global()->addStandardLevel(KGameDifficultyLevel::Medium, true);
    KGameDifficulty::global()->addStandardLevel(KGameDifficultyLevel::Hard);

    KGameDifficultyLevel *configurable = new KGameDifficultyLevel(90, QByteArray("Custom"), i18nc("@item difficulty", "Custom"));
    KGameDifficulty::global()->addLevel(configurable);

    KGameDifficultyGUI::init(this);
    connect(KGameDifficulty::global(), &KGameDifficulty::currentLevelChanged, this,
            &MainWindow::levelChanged);

    /* Disable the toolbar configuration menu entry.
     * The default size is used at first start up. */
    setupGUI(QSize(700, 560), Keys | StatusBar | Save | Create);
}

void MainWindow::loadBoard() {
    QPointer<SelectBoardWindow> w(new SelectBoardWindow(this));
    if (w->exec() == QDialog::Accepted) {
        startPresetGame(w->selectedBoard());
    }
    delete w;
}

void MainWindow::levelChanged(const KGameDifficultyLevel* level) {
    Settings::instance()->setLevel(level->standardLevel());
    Settings::instance()->sync();
    startRandomGame();
}

void MainWindow::toggleFullscreen(bool full_screen) {
    KToggleFullScreenAction::setFullScreen(this, full_screen);
}

void MainWindow::undoStackSizeChanged(int size)
{
    m_action_undo->setEnabled(size != 0);
}

void MainWindow::saveStackSizeChanged(int size)
{
    m_action_load_state->setEnabled(size != 0);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveWindowState();
    KXmlGuiWindow::closeEvent(event);
}

void MainWindow::saveWindowState() {
    QSharedPointer<QSettings> settings = Settings::instance()->qSettings();
    settings->setValue(m_key_pos, pos());
    settings->sync();
}

void MainWindow::restoreWindowState() {
    QSharedPointer<QSettings> settings = Settings::instance()->qSettings();
    QPoint p = settings->value(m_key_pos, pos()).toPoint();

    move(p);
}

void MainWindow::undo() {
    QPoint p = m_game->undo();
    m_scene->refresh(p);
}

void MainWindow::hint()
{
    QPoint p = m_game->hint();

    if (m_in_progress) {
        m_scene->refresh(p);
        m_scene->hover(p.x(), p.y());
    }
}

void MainWindow::solve()
{
    m_game->solve();
}

void MainWindow::saveState() {
    m_game->saveState();
    updatePositions();
}

void MainWindow::loadState() {
    m_game->loadState();
    m_scene->refresh();
}

void MainWindow::startRandomGame() {
    m_game = QSharedPointer<Picmi>(new Picmi());
    m_mode = Random;

    startGame();
}

void MainWindow::restartGame()
{
    m_game = QSharedPointer<Picmi>(new Picmi(m_game->getBoardMap()));

    startGame();
}

void MainWindow::startPresetGame(QSharedPointer<Level> board) {
    QSharedPointer<BoardMap> p(new BoardMap(board->width(), board->height(), board->map()));
    m_game = QSharedPointer<Picmi>(new Picmi(p));
    m_mode = Preset;
    m_current_level = board;

    startGame();
}

void MainWindow::startGame() {

    if (m_scene) {
        disconnect(&m_timer, &QTimer::timeout, this, &MainWindow::updatePlayedTime);
    }

    m_action_undo->setEnabled(false);
    m_action_hint->setEnabled(true);
    m_action_solve->setEnabled(true);
    m_action_save_state->setEnabled(true);
    m_action_load_state->setEnabled(false);
    m_action_pause->setEnabled(true);
    m_action_pause->setChecked(false);
    m_status_position->setVisible(true);
    m_load_game->setVisible(false);
    m_new_game->setVisible(false);
    KGameDifficulty::global()->setGameRunning(true);

    m_timer.start();
    m_scene = m_view.createScene(m_game);
    updatePlayedTime();
    updatePositions();

    m_view.setEnabled(true);
    m_view.setFocus();
    m_view.setPaused(false);

    connect(&m_timer, &QTimer::timeout, this, &MainWindow::updatePlayedTime);
    connect(m_game.data(), &Picmi::stateChanged, this, &MainWindow::updatePositions);
    connect(m_game.data(), &Picmi::gameCompleted, this, &MainWindow::gameCompleted);
    connect(m_game.data(), &Picmi::gameWon, this, &MainWindow::gameWon);
    connect(m_game.data(), &Picmi::undoStackSizeChanged, this, &MainWindow::undoStackSizeChanged);
    connect(m_game.data(), &Picmi::saveStackSizeChanged, this, &MainWindow::saveStackSizeChanged);

    m_in_progress = true;
}

void MainWindow::updatePlayedTime() {
    m_status_time->setText(i18n("Elapsed time: %1",
                                Time(m_game->elapsedSecs()).toString()));
}

void MainWindow::updatePositions() {
    m_status_position->setText(i18n("Actions since last saved position: %1",
                                    m_game->currentStateAge()));
}

QSharedPointer<KGameHighScoreDialog> MainWindow::createScoreDialog() {
    QSharedPointer<KGameHighScoreDialog> p(new KGameHighScoreDialog(KGameHighScoreDialog::Name | KGameHighScoreDialog::Date | KGameHighScoreDialog::Time));

    p->initFromDifficulty(KGameDifficulty::global());
    p->hideField(KGameHighScoreDialog::Score);

    return p;
}

void MainWindow::gameWon() {
    KGameHighScoreDialog::FieldInfo score = m_game->endGame();
    m_status_position->setVisible(false);
    if (m_mode == Random) {
        bool notified = false;
        m_new_game->setVisible(true);
        if (KGameDifficulty::globalLevel() != KGameDifficultyLevel::Custom) {
            QSharedPointer<KGameHighScoreDialog> scoreDialog = createScoreDialog();
            if (scoreDialog->addScore(score, KGameHighScoreDialog::LessIsMore | KGameHighScoreDialog::AskName) != 0) {
                scoreDialog->exec();
                notified = true;
            }
        }

        /* Ensure that the user gets some kind of feedback about solving the board. */
        if (!notified) {
            KMessageBox::information(this, i18n("Congratulations, you have solved this board!"),
                                     i18nc("@title:window", "Board Solved!"));
        }
    } else if (m_mode == Preset) {
        m_load_game->setVisible(true);
        m_current_level->setSolved(m_game->elapsedSecs());
        KMessageBox::information(this, i18n("Congratulations, you have solved board '%1'!",
                                            m_current_level->name()),
                                 i18nc("@title:window", "Board Solved!"));
    }

    m_view.setFocus();
}

void MainWindow::gameCompleted() {
    m_view.setEnabled(false);
    m_action_pause->setEnabled(false);
    m_action_solve->setEnabled(false);
    m_action_hint->setEnabled(false);
    m_action_undo->setEnabled(false);
    m_action_save_state->setEnabled(false);
    m_action_load_state->setEnabled(false);
    KGameDifficulty::global()->setGameRunning(false);
    m_timer.stop();
    updatePlayedTime();
    m_in_progress = false;
}

void MainWindow::highscores() {
    pauseGame();

    QSharedPointer<KGameHighScoreDialog> scoreDialog = createScoreDialog();
    scoreDialog->exec();

    m_view.setFocus();
}

void MainWindow::togglePaused(bool paused) {
    m_view.setPaused(paused);
    m_action_undo->setEnabled(!paused);
    m_action_hint->setEnabled(!paused);
    m_action_solve->setEnabled(!paused);
    m_action_save_state->setEnabled(!paused);
    m_action_load_state->setEnabled(!paused);

    if (paused) {
        m_timer.stop();
    } else {
        m_timer.start();
    }
}

void MainWindow::pauseGame() {
    if (m_action_pause->isChecked() || !m_in_progress) {
        return;
    }

    m_action_pause->setChecked(true);
    togglePaused(true);
}

void MainWindow::settings() {
    pauseGame();

    QPointer<SettingsWindow> w(new SettingsWindow(this));
    w->exec();
    delete w;
}

#include "moc_mainwindow.cpp"
