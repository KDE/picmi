/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SELECTBOARD_H
#define SELECTBOARD_H

#include <QDialog>
#include <QSharedPointer>

#include "ui_selectboardwindow.h"

class Level;
class LevelTableModel;

class SelectBoardWindow : public QDialog
{
    Q_OBJECT
public:
    explicit SelectBoardWindow(QWidget *parent);

    ~SelectBoardWindow() override;

    QSharedPointer<Level> selectedBoard() const;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private Q_SLOTS:
    void selectedLevelChanged(const QModelIndex &current, const QModelIndex &previous);
    void levelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private:
    void updateDetails(QSharedPointer<Level> level);
    void resetSelection();

    Ui::LevelSelectUi *ui;

    QList<QSharedPointer<Level> > m_levels;
    QSharedPointer<LevelTableModel> m_model;
};

#endif
