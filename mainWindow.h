#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>

class dinosaur;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(QWidget *parent = nullptr);

private slots:
  void startGame();
  void openCharacterSelect();
  void chooseNormalDino();
  void chooseHatDino();
  void chooseSantaDino();
  void chooseCowboyDino();
  void choosePirateDino();
  void updateCharacterSelection();

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

private:
  QStackedWidget *stack;
  QWidget *menuPage;
  QWidget *charPage;
  dinosaur *gamePage;

  // Character selection cards
  QWidget *charCard0 = nullptr;
  QWidget *charCard1 = nullptr;
  QWidget *charCard2 = nullptr;
  QWidget *charCard3 = nullptr;
  QWidget *charCard4 = nullptr;

  int selectedSkin =
      0; // 0 = normal, 1 = yellow hat, 2 = santa, 3 = cowboy, 4 = pirate
};

#endif // MAINWINDOW_H
