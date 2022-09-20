/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

#include "src/logic/settings.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow() override;

private Q_SLOTS:
    void accept() override;
    void bgToolButtonClicked();
    void selectSolvedColor();
    void selectUnsolvedColor();

private:
    void restoreSettings();
    void saveSettings();
    QString m_font_color_solved, m_font_color_unsolved;

    Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
