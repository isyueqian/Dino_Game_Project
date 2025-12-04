#include "mainWindow.h"
#include "dinosaur.h"
#include <QEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

  setFixedSize(480, 272); // Fixed size for LCD

  stack = new QStackedWidget(this);
  scoreManager = new ScoreManager();

  // Menu Page
  menuPage = new QWidget;
  QVBoxLayout *mlay = new QVBoxLayout(menuPage);
  mlay->setSpacing(15);
  mlay->setContentsMargins(40, 20, 40, 25);

  // Title
  QLabel *title = new QLabel(
      "<h1 style='font-family:Courier New;font-size:36px;'>Dino Game</h1>");
  title->setAlignment(Qt::AlignCenter);
  mlay->addWidget(title);

  mlay->addSpacing(-15);

  // Start button
  QPushButton *btnStart = new QPushButton("▶ Play");
  btnStart->setFixedHeight(50);
  btnStart->setMaximumWidth(250);
  btnStart->setStyleSheet(R"(
        QPushButton {
            background-color: #f5f5f5;
            border: 3px solid #ccc;
            border-radius: 10px;
            font-family: 'Courier New';
            font-size: 16px;
            font-weight: bold;
            padding: 10px 20px;
            padding-left: 15px;
            color: #000000;
            text-align: left;
        }
        QPushButton:hover {
            background-color: #d0e8ff;
            border-color: #4a90e2;
        }
        QPushButton:pressed {
            background-color: #b8d7f5;
        }
    )");

  // Character Select button
  QPushButton *btnChar = new QPushButton(" Character Select");
  btnChar->setFixedHeight(50);
  btnChar->setMaximumWidth(250);

  QPixmap dinoIcon(":/images/images/Dino_Start.png");
  dinoIcon =
      dinoIcon.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  btnChar->setIcon(QIcon(dinoIcon));
  btnChar->setIconSize(QSize(30, 30));

  btnChar->setStyleSheet(btnStart->styleSheet());

  // Leaderboard button
  QPushButton *btnLeader = new QPushButton(" Leaderboard");
  btnLeader->setFixedHeight(50);
  btnLeader->setMaximumWidth(250);

  QPixmap trophyIcon(":/images/images/Trophy.png");
  trophyIcon =
      trophyIcon.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  btnLeader->setIcon(QIcon(trophyIcon));
  btnLeader->setIconSize(QSize(30, 30));

  btnLeader->setStyleSheet(btnStart->styleSheet());

  // Center the buttons
  QWidget *buttonContainer = new QWidget;
  QVBoxLayout *buttonLayout = new QVBoxLayout(buttonContainer);
  buttonLayout->setSpacing(14);
  buttonLayout->addWidget(btnStart, 0, Qt::AlignCenter);
  buttonLayout->addWidget(btnChar, 0, Qt::AlignCenter);
  buttonLayout->addWidget(btnLeader, 0, Qt::AlignCenter);

  mlay->addWidget(buttonContainer);

  connect(btnStart, &QPushButton::clicked, this, &MainWindow::startGame);
  connect(btnChar, &QPushButton::clicked, this,
          &MainWindow::openCharacterSelect);
  connect(btnLeader, &QPushButton::clicked, this, &MainWindow::openLeaderboard);

  // Game Page
  gamePage = new dinosaur;
  connect(gamePage, &dinosaur::exitToMenu,
          [this]() { stack->setCurrentIndex(0); });
  connect(gamePage, &dinosaur::gameOverSignal, this,
          &MainWindow::handleGameOver);

  // Character Select Page
  charPage = new QWidget;
  QVBoxLayout *clay = new QVBoxLayout(charPage);
  clay->setSpacing(10);
  clay->setContentsMargins(10, 10, 10, 10);

  QLabel *charTitle = new QLabel("<h2>Select Your Character</h2>");
  charTitle->setAlignment(Qt::AlignCenter);
  clay->addWidget(charTitle);

  // Character cards layout
  QHBoxLayout *charLayout = new QHBoxLayout;
  charLayout->setSpacing(10);

  // Create preview cards
  for (int i = 0; i < 5; ++i) {
    QWidget *charCard = new QWidget;
    charCard->setObjectName(QString("charCard_%1").arg(i));
    charCard->setCursor(Qt::PointingHandCursor);
    charCard->setFixedSize(80, 100);

    QVBoxLayout *cardLayout = new QVBoxLayout(charCard);
    cardLayout->setSpacing(5);
    cardLayout->setContentsMargins(10, 10, 10, 10);

    // Preview image
    QLabel *previewLabel = new QLabel;
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setFixedSize(60, 60);

    QPixmap preview;
    QString charName;

    if (i == 0) {
      preview.load(":/images/images/Dino_Start.png");
      charName = "Normal";
    } else if (i == 1) {
      preview.load(":/images/images/Hat_Start.png");
      charName = "Yellow Hat";
    } else if (i == 2) {
      preview.load(":/images/images/Santa_Start.png");
      charName = "Santa";
    } else if (i == 3) {
      preview.load(":/images/images/Cowboy_Start.png");
      charName = "Cowboy";
    } else {
      preview.load(":/images/images/Pirate_Start.png");
      charName = "Pirate";
    }

    preview =
        preview.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    previewLabel->setPixmap(preview);

    // Name label
    QLabel *nameLabel = new QLabel(charName);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet("font-weight: bold; font-size: 8px;");

    cardLayout->addWidget(previewLabel);
    cardLayout->addWidget(nameLabel);

    // Store widget for styling
    if (i == 0)
      charCard0 = charCard;
    else if (i == 1)
      charCard1 = charCard;
    else if (i == 2)
      charCard2 = charCard;
    else if (i == 3)
      charCard3 = charCard;
    else
      charCard4 = charCard;

    // Enable click events
    charCard->installEventFilter(this);

    charLayout->addWidget(charCard);
  }

  clay->addLayout(charLayout);

  // Back button
  QPushButton *backBtn = new QPushButton("← Back");
  backBtn->setFixedHeight(35);
  backBtn->setMaximumWidth(180);
  backBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #f5f5f5;
            border: 3px solid #ccc;
            border-radius: 10px;
            font-family: 'Courier New';
            font-size: 14px;
            font-weight: bold;
            padding: 8px 16px;
            color: #000000;
        }
        QPushButton:hover {
            background-color: #ffffff;
            border-color: #999;
            color: #000000;
        }
        QPushButton:pressed {
            background-color: #e8e8e8;
            color: #000000;
        }
    )");
  connect(backBtn, &QPushButton::clicked,
          [this]() { stack->setCurrentIndex(0); });

  clay->addWidget(backBtn, 0, Qt::AlignCenter);

  // Leaderboard Page
  leaderboardPage = new QWidget;

  setStyleSheet(R"(
        QWidget {
            background-color: white;
        }
        QPushButton {
            font-size: 18px;
            padding: 8px 16px;
            background-color: #e0e0e0;
            border: 2px solid #333;
            border-radius: 6px;
        }
        QPushButton:hover {
            background-color: #f5f5f5;
        }
        QLabel {
            font-family: 'Courier New';
            color: #333;
        }
        QWidget[objectName^="charCard"] {
            background-color: #f5f5f5;
            border: 3px solid #ccc;
            border-radius: 10px;
        }
        QWidget[objectName^="charCard"]:hover {
            background-color: #ffffff;
            border-color: #999;
        }
    )");

  // Stack pages
  stack->addWidget(menuPage);        // 0
  stack->addWidget(charPage);        // 1
  stack->addWidget(leaderboardPage); // 2
  stack->addWidget(gamePage);        // 3

  setCentralWidget(stack);
  stack->setCurrentIndex(0);

  updateCharacterSelection();
}

void MainWindow::startGame() {
  gamePage->setSkin(selectedSkin);
  gamePage->reset();
  stack->setCurrentIndex(3); // Changed index to 3 for gamePage

  gamePage->setFocus();
}

void MainWindow::openCharacterSelect() { stack->setCurrentIndex(1); }

void MainWindow::openLeaderboard() {
  // Clear existing layout
  if (leaderboardPage->layout()) {
    QLayout *oldLayout = leaderboardPage->layout();
    QLayoutItem *item;
    while ((item = oldLayout->takeAt(0)) != nullptr) {
      delete item->widget();
      delete item;
    }
    delete oldLayout;
  }

  QVBoxLayout *llay = new QVBoxLayout(leaderboardPage);
  llay->setSpacing(10);
  llay->setContentsMargins(20, 20, 20, 20);

  QLabel *title = new QLabel("<h2>Leaderboard</h2>");
  title->setAlignment(Qt::AlignCenter);
  llay->addWidget(title);

  // Scores grid
  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(10);

  QStringList names = {"Normal", "Yellow Hat", "Santa", "Cowboy", "Pirate"};
  QStringList iconPaths = {
      ":/images/images/Dino_Start.png", ":/images/images/Hat_Start.png",
      ":/images/images/Santa_Start.png", ":/images/images/Cowboy_Start.png",
      ":/images/images/Pirate_Start.png"};

  QMap<int, int> scores = scoreManager->getTopScores();

  // Sort scores descending
  QList<QPair<int, int>> sortedScores;
  for (int i = 0; i < 5; ++i) {
    sortedScores.append(qMakePair(scores.value(i, 0), i));
  }

  std::sort(sortedScores.begin(), sortedScores.end(),
            [](const QPair<int, int> &a, const QPair<int, int> &b) {
              return a.first > b.first;
            });

  for (int i = 0; i < sortedScores.size(); ++i) {
    int s = sortedScores[i].first;
    int skinIdx = sortedScores[i].second;

    // Icon
    QLabel *icon = new QLabel;
    QPixmap pm(iconPaths[skinIdx]);
    pm = pm.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    icon->setPixmap(pm);
    icon->setFixedSize(30, 30);

    // Name
    QLabel *name = new QLabel(names[skinIdx]);
    name->setStyleSheet("font-weight: bold; font-size: 14px;");

    // Score
    QLabel *scoreLbl = new QLabel(QString::number(s));
    scoreLbl->setStyleSheet("font-family: 'Courier New'; font-size: 14px;");
    scoreLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    grid->addWidget(icon, i, 0);
    grid->addWidget(name, i, 1);
    grid->addWidget(scoreLbl, i, 2);
  }

  llay->addLayout(grid);
  llay->addStretch(1);

  // Back button
  QPushButton *backBtn = new QPushButton("← Back");
  backBtn->setFixedHeight(35);
  backBtn->setMaximumWidth(180);
  backBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #f5f5f5;
            border: 3px solid #ccc;
            border-radius: 10px;
            font-family: 'Courier New';
            font-size: 14px;
            font-weight: bold;
            padding: 8px 16px;
            color: #000000;
        }
        QPushButton:hover {
            background-color: #ffffff;
            border-color: #999;
            color: #000000;
        }
        QPushButton:pressed {
            background-color: #e8e8e8;
            color: #000000;
        }
    )");
  connect(backBtn, &QPushButton::clicked,
          [this]() { stack->setCurrentIndex(0); });

  llay->addWidget(backBtn, 0, Qt::AlignCenter);

  stack->setCurrentIndex(2);
}

void MainWindow::chooseNormalDino() {
  selectedSkin = 0;
  updateCharacterSelection();
}

void MainWindow::chooseHatDino() {
  selectedSkin = 1;
  updateCharacterSelection();
}

void MainWindow::chooseSantaDino() {
  selectedSkin = 2;
  updateCharacterSelection();
}

void MainWindow::chooseCowboyDino() {
  selectedSkin = 3;
  updateCharacterSelection();
}

void MainWindow::choosePirateDino() {
  selectedSkin = 4;
  updateCharacterSelection();
}

void MainWindow::updateCharacterSelection() {
  // Update all cards to show selection state
  if (charCard0) {
    charCard0->setStyleSheet(selectedSkin == 0
                                 ? "background-color: #d0e8ff; border: 4px "
                                   "solid #4a90e2; border-radius: 10px;"
                                 : "background-color: #f5f5f5; border: 3px "
                                   "solid #ccc; border-radius: 10px;");
  }
  if (charCard1) {
    charCard1->setStyleSheet(selectedSkin == 1
                                 ? "background-color: #d0e8ff; border: 4px "
                                   "solid #4a90e2; border-radius: 10px;"
                                 : "background-color: #f5f5f5; border: 3px "
                                   "solid #ccc; border-radius: 10px;");
  }
  if (charCard2) {
    charCard2->setStyleSheet(selectedSkin == 2
                                 ? "background-color: #d0e8ff; border: 4px "
                                   "solid #4a90e2; border-radius: 10px;"
                                 : "background-color: #f5f5f5; border: 3px "
                                   "solid #ccc; border-radius: 10px;");
  }
  if (charCard3) {
    charCard3->setStyleSheet(selectedSkin == 3
                                 ? "background-color: #d0e8ff; border: 4px "
                                   "solid #4a90e2; border-radius: 10px;"
                                 : "background-color: #f5f5f5; border: 3px "
                                   "solid #ccc; border-radius: 10px;");
  }
  if (charCard4) {
    charCard4->setStyleSheet(selectedSkin == 4
                                 ? "background-color: #d0e8ff; border: 4px "
                                   "solid #4a90e2; border-radius: 10px;"
                                 : "background-color: #f5f5f5; border: 3px "
                                   "solid #ccc; border-radius: 10px;");
  }
}

void MainWindow::handleGameOver(int skin, int score) {
  scoreManager->saveScore(skin, score);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::MouseButtonPress) {
    QWidget *widget = qobject_cast<QWidget *>(obj);
    if (widget && widget->objectName().startsWith("charCard_")) {
      // Extract character index from object name
      int charIndex = widget->objectName().back().digitValue();

      if (charIndex == 0)
        chooseNormalDino();
      else if (charIndex == 1)
        chooseHatDino();
      else if (charIndex == 2)
        chooseSantaDino();
      else if (charIndex == 3)
        chooseCowboyDino();
      else if (charIndex == 4)
        choosePirateDino();

      return true;
    }
  }
  return QMainWindow::eventFilter(obj, event);
}
