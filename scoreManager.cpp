#include "scoreManager.h"
#include <QDebug>
#include <QFile>
#include <QStringList>
#include <QTextStream>

ScoreManager::ScoreManager() { loadScores(); }

void ScoreManager::loadScores() {
  QFile file(filename);
  if (!file.exists()) {
    return; // File doesn't exist yet, skip loading
  }

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Could not open scores file for reading:" << filename;
    return;
  }

  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine();
    QStringList parts = line.split(":");
    if (parts.size() == 2) {
      bool okKey, okVal;
      int skin = parts[0].toInt(&okKey);
      int score = parts[1].toInt(&okVal);

      if (okKey && okVal) {
        highScores[skin] = score;
      }
    }
  }
  file.close();
}

bool ScoreManager::saveScore(int skinIdx, int score) {
  if (!highScores.contains(skinIdx) || score > highScores[skinIdx]) {
    highScores[skinIdx] = score;
    writeToFile();
    return true;
  }
  return false;
}

void ScoreManager::writeToFile() {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Could not open scores file for writing:" << filename;
    return;
  }

  QTextStream out(&file);
  // Iterate over the map and write each entry
  QMapIterator<int, int> i(highScores);
  while (i.hasNext()) {
    i.next();
    out << i.key() << ":" << i.value() << "\n";
  }
  file.close();
}

QMap<int, int> ScoreManager::getTopScores() const { return highScores; }

int ScoreManager::getHighScore(int skinIdx) const {
  return highScores.value(skinIdx, 0);
}
