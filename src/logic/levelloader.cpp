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

#include <config.h>
#include "levelloader.h"

#include <KLocalizedString>
#include "picmi_debug.h"
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QStandardPaths>

#include "src/settings.h"
#include "src/systemexception.h"

class LevelList : public QList<QSharedPointer<Level> >
{
public:
    LevelList() : QList<QSharedPointer<Level> >() { }
    void append(const QList<QSharedPointer<Level> > &t);
private:
    bool containsLevel(QSharedPointer<Level> level) const;
};

void LevelList::append(const QList<QSharedPointer<Level> > &t) {
    for (int i = 0; i < t.size(); i++) {
        QSharedPointer<Level> level = t[i];
        if (!containsLevel(level)) {
            QList<QSharedPointer<Level> >::append(level);
        }
    }
}

bool LevelList::containsLevel(QSharedPointer<Level> level) const {
    for (int i = 0; i < size(); i++) {
        if (*at(i) == *level) {
            return true;
        }
    }
    return false;
}

Level::Level() : m_solved(false), m_solved_time(0) { }

QString Level::visibleName() const
{
    if (solved()) {
        return name();
    }
    /* Mask the real name if unsolved. Unicode 0x26AB is the bullet point ('⚫') */
    return QString(name().length(), QChar(0x26AB));
}

QString Level::name() const {
    QByteArray bytes = m_name.toUtf8();
    return i18n(bytes.constData());
}

QString Level::author() const {
    QByteArray bytes = m_author.toUtf8();
    return i18n(bytes.constData());
}

QString Level::key() const {
    return QStringLiteral("preset_scores/%1_%2").arg(m_levelset, m_name);
}

void Level::writeSettings(int seconds) {
    QSharedPointer<QSettings> settings = Settings::instance()->qSettings();
    QString k = key();

    settings->setValue(k, seconds);
    settings->sync();
}

void Level::finalize() {
    constructPreview();
    readSettings();
}

void Level::readSettings() {
    QSharedPointer<QSettings> settings = Settings::instance()->qSettings();
    QString k = key();

    if (settings->contains(k)) {
        m_solved = true;
        m_solved_time = settings->value(k).toInt();
    }
}

void Level::constructPreview() {
    QImage preview(width(), height(), QImage::Format_Mono);
    preview.fill(Qt::color1);

    for (int y = 0; y < height(); y++) {
        for (int x = 0; x < width(); x++) {
            if (m_map[y * width() + x] == Board::Box) {
                preview.setPixel(x, y, 0);
            }
        }
    }

    m_preview = QPixmap::fromImage(preview);
}

void Level::setSolved(int seconds) {
    if (m_solved_time > 0 && seconds >= m_solved_time) {
        return;
    }
    m_solved = true;
    m_solved_time = seconds;
    writeSettings(seconds);
}

bool Level::operator==(const Level &that) const {
    return (that.m_name == m_name && that.m_author == m_author);
}

QList<QSharedPointer<Level> > LevelLoader::load() {
    const QString prefix = QStringLiteral("levels/");
    QList<QString> paths;
    paths << QString(prefix)
          << QString(FILEPATH "/" + prefix)
          << QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                    prefix,
                                    QStandardPaths::LocateOption::LocateDirectory);

    LevelList list;

    for (int i = 0; i < paths.size(); i++) {
        QDir dir(paths[i]);
        if (!dir.exists()) {
            continue;
        }

        QStringList files = dir.entryList(QStringList(QStringLiteral("*.xml")));

        for (int j = 0; j < files.size(); j++) {
            LevelLoader loader(dir.absoluteFilePath(files[j]));
            list.append(loader.loadLevels());
        }
    }

    return list;
}

LevelLoader::LevelLoader(const QString &filename) :
    m_filename(filename), m_valid(true)
{
    setLevelset(filename);
}

void LevelLoader::setLevelset(const QString& filename)
{
    m_levelset = QSharedPointer<QDomDocument>(new QDomDocument());

    QFile file(filename);
    if (!file.open( QIODevice::ReadOnly)) {
        throw SystemException(QStringLiteral("Can't open file %1").arg(filename));
    }

    QString errorString;
    int errorLine;
    int errorColumn;
    bool success = m_levelset->setContent( &file, false, &errorString, &errorLine, &errorColumn);

    file.close();
    if (!success) {
        qCDebug(PICMIC_LOG) << QStringLiteral("Can't read levelset from %1 \nError: %2 in Line %3, Column %4")
                              .arg(filename, errorString).arg(errorLine).arg(errorColumn);
        m_valid = false;
    }
}

QList<QSharedPointer<Level> > LevelLoader::loadLevels() {
    QList<QSharedPointer<Level> > l;

    if (!m_valid) {
        return l;
    }

    QDomElement levels = m_levelset->documentElement();
    if (!levels.hasAttribute(QStringLiteral("name"))) {
        qCDebug(PICMIC_LOG) << "Loading level failed: no levelset name specified";
        return l;
    }
    m_levelsetname = levels.attribute(QStringLiteral("name"));

    QDomNodeList childNodes = levels.childNodes();
    for (int i = 0; i < childNodes.size(); i++) {
        try {
            l.append(loadLevel(childNodes.at(i).toElement()));
        } catch (const SystemException &e) {
            qCDebug(PICMIC_LOG) << "Loading level failed: " << e.what();
        }
    }
    return l;
}

QSharedPointer<Level> LevelLoader::loadLevel(const QDomElement &node) const {
    if (node.isNull() || node.tagName() != QLatin1String("board")) {
        throw SystemException(QStringLiteral("Unexpected level node"));
    }

    if (!node.hasAttribute(QStringLiteral("name")) || !node.hasAttribute(QStringLiteral("author"))
            || !node.hasAttribute(QStringLiteral("difficulty"))) {
        throw SystemException(QStringLiteral("Level node missing attribute."));
    }

    QSharedPointer<Level> p(new Level);
    p->m_name = node.attribute(QStringLiteral("name"));
    p->m_author = node.attribute(QStringLiteral("author"));
    p->m_levelset = m_levelsetname;
    p->m_difficulty = node.attribute(QStringLiteral("difficulty")).toInt();

    QDomNodeList childNodes = node.childNodes();

    if (childNodes.isEmpty()) {
        throw SystemException(QStringLiteral("Empty level definition."));
    }

    const QString tag_name = childNodes.at(0).toElement().tagName();
    if (tag_name == QLatin1String("row")) {
        int i;
        QList<Board::State> l;
        for (i = 0; i < childNodes.size(); i++) {
            l = loadRow(childNodes.at(i).toElement());
            p->m_map.append(l);
        }
        p->m_width = l.size();
        p->m_height = i;
    } else if (tag_name == QLatin1String("xpm")) {
        QImage xpm = openXPM(childNodes.at(0).toElement());
        p->m_map = loadXPM(xpm);
        p->m_width = xpm.width();
        p->m_height = xpm.height();
    }

    if (p->m_map.size() != p->height() * p->width()) {
        throw SystemException(QStringLiteral("Invalid board size"));
    }

    p->finalize();

    return p;
}

static Board::State charToState(const QChar &c) {
    switch (c.toLatin1()) {
    case '-': return Board::Nothing;
    case '1': return Board::Box;
    default: throw SystemException(QStringLiteral("Invalid char in level definition"));
    }
}

QImage LevelLoader::openXPM(const QDomElement &node) const {
    if (node.isNull() || node.tagName() != QLatin1String("xpm")) {
        throw SystemException(QStringLiteral("Unexpected row node"));
    }

    QFileInfo file(m_filename);
    QString filepath = file.absolutePath() + '/' + node.text();

    QImage xpm(filepath);

    if (xpm.isNull()) {
        throw SystemException(QStringLiteral("Could not load %1").arg(filepath));
    }

    return xpm;
}

QList<Board::State> LevelLoader::loadXPM(const QImage &xpm) const {
    QList<Board::State> list;
    for (int y = 0; y < xpm.height(); y++) {
        for (int x = 0; x < xpm.width(); x++) {
            QRgb pix = xpm.pixel(x, y);
            list.append((pix == 0) ? Board::Nothing : Board::Box);
        }
    }

    return list;
}

QList<Board::State> LevelLoader::loadRow(const QDomElement &node) const {
    if (node.isNull() || node.tagName() != QLatin1String("row")) {
        throw SystemException(QStringLiteral("Unexpected row node"));
    }

    QString text = node.text();
    QList<Board::State> list;
    foreach (const QChar &c, text) {
        Board::State s = charToState(c);
        list.append(s);
    }

    return list;
}
