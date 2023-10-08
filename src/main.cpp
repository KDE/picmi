/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <config.h>
#include <picmi_version.h>

#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>
#include <QApplication>
#include <QCommandLineParser>

#include "gui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("picmi");

    KAboutData about(QStringLiteral("picmi"),
                      i18n("Picmi"),
                      QStringLiteral(PICMI_VERSION_STRING),
                      i18n("Picmi - a nonogram puzzle game"),
                      KAboutLicense::GPL_V2,
                      i18n("(c) 2012 - 2015 The Picmi Authors"),
                      QString(),
                      QStringLiteral("https://apps.kde.org/picmi"));
    about.addAuthor(i18n("Jakob Gruber"), i18n("Picmi Author"), QStringLiteral("jakob.gruber@gmail.com"));

    KAboutData::setApplicationData(about);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("picmi")));

    KCrash::initialize();

    QCommandLineParser parser;
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    KDBusService service;

    MainWindow *w = new MainWindow;
    w->show();

    return app.exec();
}
