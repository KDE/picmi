/*
    Copyright 2015 Jakob Gruber <jakob.gruber@gmail.com>
    Copyright 2011 Julian Helfferich <julian.helfferich@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <QImage>
#include <QList>
#include <QPixmap>
#include <QString>
#include <QSharedPointer>

#include "src/logic/board.h"

class QDomDocument;
class QDomElement;
class LevelLoader;

class Level
{
    friend class LevelLoader;
public:
    Level();

    QString name() const;
    QString author() const;
    QString levelset() const { return m_levelset; }
    int difficulty() const { return m_difficulty; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    QList<Board::State> map() const { return m_map; }
    QPixmap preview() const { return m_preview; }

    QString visibleName() const;
    bool solved() const { return m_solved; }
    int solvedTime() const { return m_solved_time; }
    void setSolved(int seconds);

    bool operator==(const Level &that) const;

private:
    void finalize(); /* needs to be called by loader when done constructing */
    void constructPreview();
    void readSettings();
    void writeSettings(int seconds);
    QString key() const;

    QString m_name, m_author, m_levelset;
    int m_difficulty;
    int m_width, m_height;
    QList<Board::State> m_map;
    bool m_solved;
    int m_solved_time;
    QPixmap m_preview;
};

class LevelLoader
{
public:
    explicit LevelLoader(const QString &filename);

    QList<QSharedPointer<Level> > loadLevels();
    static QList<QSharedPointer<Level> > load();

private:
    void setLevelset(const QString& levelname);
    QSharedPointer<Level> loadLevel(const QDomElement &node) const;
    QList<Board::State> loadRow(const QDomElement &node) const;
    QImage openXPM(const QDomElement &node) const;
    QList<Board::State> loadXPM(const QImage &xpm) const;

    QSharedPointer<QDomDocument> m_levelset;
    QString m_levelsetname;

    const QString m_filename;

    bool m_valid;
};

#endif // LEVELLOADER_H
