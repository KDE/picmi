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


#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <assert.h>
#include <KLocalizedString>
#include <QColorDialog>
#include <QFileDialog>

static inline QString toStylesheet(const QString &color)
{
    return QStringLiteral("QLabel { color : %1 }").arg(color);
}

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    restoreSettings();

    connect(ui->bgToolButton, &QToolButton::clicked, this, &SettingsWindow::bgToolButtonClicked);
    connect(ui->fontColorSolvedPushButton, &QPushButton::clicked, this, &SettingsWindow::selectSolvedColor);
    connect(ui->fontColorUnsolvedPushButton, &QPushButton::clicked, this, &SettingsWindow::selectUnsolvedColor);
    connect(ui->bgCustomRadioButton, &QRadioButton::toggled, ui->bgToolButton, &QToolButton::setEnabled);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::selectSolvedColor()
{
    QColor color(m_font_color_solved);
    color = QColorDialog::getColor(color, this);
    if (!color.isValid()) {
        return;
    }
    m_font_color_solved = color.name();
    ui->fontColorSolvedLabel->setStyleSheet(toStylesheet(m_font_color_solved));
}

void SettingsWindow::selectUnsolvedColor()
{
    QColor color(m_font_color_unsolved);
    color = QColorDialog::getColor(color, this);
    if (!color.isValid()) {
        return;
    }
    m_font_color_unsolved = color.name();
    ui->fontColorUnsolvedLabel->setStyleSheet(toStylesheet(m_font_color_unsolved));
}

void SettingsWindow::bgToolButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, i18n("Select Background"), QString(),
                                                    i18n("Image Files (*.png *.jpg)"));

    if (fileName.isNull()) {
        return;
    }

    ui->bgLineEdit->setText(fileName);
}

void SettingsWindow::restoreSettings() {
    ui->heightSpinBox->setValue(Settings::instance()->height());
    ui->widthSpinBox->setValue(Settings::instance()->width());
    ui->densitySlider->setValue(Settings::instance()->boxDensity() * 100.0);
    ui->preventMistakesCheckBox->setChecked(Settings::instance()->preventMistakes());
    ui->bgCustomRadioButton->setChecked(Settings::instance()->customBgEnabled());
    ui->bgToolButton->setEnabled(Settings::instance()->customBgEnabled());
    ui->bgLineEdit->setText(Settings::instance()->customBgPath());
    m_font_color_solved = Settings::instance()->fontColorSolved();
    m_font_color_unsolved = Settings::instance()->fontColorUnsolved();
    ui->fontColorUnsolvedLabel->setStyleSheet(toStylesheet(m_font_color_unsolved));
    ui->fontColorSolvedLabel->setStyleSheet(toStylesheet(m_font_color_solved));
}

void SettingsWindow::saveSettings() {
    Settings::instance()->setHeight(ui->heightSpinBox->value());
    Settings::instance()->setWidth(ui->widthSpinBox->value());
    Settings::instance()->setBoxDensity(ui->densitySlider->value() / 100.0);
    Settings::instance()->setPreventMistakes(ui->preventMistakesCheckBox->isChecked());
    Settings::instance()->setCustomBgEnabled(ui->bgCustomRadioButton->isChecked());
    Settings::instance()->setCustomBgPath(ui->bgLineEdit->text());
    Settings::instance()->setFontColorSolved(m_font_color_solved);
    Settings::instance()->setFontColorUnsolved(m_font_color_unsolved);

    Settings::instance()->sync();
}

void SettingsWindow::accept() {
    saveSettings();
    QDialog::accept();
}
