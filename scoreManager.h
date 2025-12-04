#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <QMap>
#include <QString>

class ScoreManager {
public:
  ScoreManager();

  // Loads scores from the persistence file
  void loadScores();

  // Saves a score if it's a high score for the given skin
  // Returns true if it was a new high score
  bool saveScore(int skinIdx, int score);

  // Returns the map of skin index to high score
  QMap<int, int> getTopScores() const;

  // Get high score for a specific skin
  int getHighScore(int skinIdx) const;

private:
  QMap<int, int> highScores;
  const QString filename = "scores.dat";

  void writeToFile();
};

#endif // SCOREMANAGER_H
