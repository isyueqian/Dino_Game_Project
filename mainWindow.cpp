#include "mainWindow.h"
#include <QLabel>
#include <QVBoxLayout>
#include "dinosaur.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    stack = new QStackedWidget(this);

    // * 1. Menu Page
    menuPage = new QWidget;
    QVBoxLayout* mlay = new QVBoxLayout(menuPage);

    QPushButton* btnStart = new QPushButton("▶ Start Game");
    QPushButton* btnChar = new QPushButton("🦖 Character Select");

    btnStart->setFixedHeight(40);
    btnChar->setFixedHeight(40);

    mlay->addStretch();
    mlay->addWidget(new QLabel("<h1 style='font-family:Courier New;font-size:32px;'>Dino Game</h1>"));
    mlay->addWidget(btnStart);
    mlay->addWidget(btnChar);
    mlay->addStretch();

    connect(btnStart, &QPushButton::clicked, this, &MainWindow::startGame);
    connect(btnChar, &QPushButton::clicked, this, &MainWindow::openCharacterSelect);

    // * 2. Character Select Page
    charPage = new QWidget;
    QVBoxLayout* clay = new QVBoxLayout(charPage);

    QPushButton* normalBtn = new QPushButton("Normal Dinosaur");
    QPushButton* hatBtn = new QPushButton("Dinosaur with Yellow Hat");
    QPushButton* santaBtn = new QPushButton("Merry Christmas!");

    clay->addStretch();
    clay->addWidget(new QLabel("<h2>Select Your Character</h2>"));
    clay->addWidget(normalBtn);
    clay->addWidget(hatBtn);
    clay->addWidget(santaBtn);
    clay->addStretch();

    connect(normalBtn, &QPushButton::clicked, this, &MainWindow::chooseNormalDino);
    connect(hatBtn, &QPushButton::clicked, this, &MainWindow::chooseHatDino);
    connect(santaBtn, &QPushButton::clicked, this, &MainWindow::chooseSantaDino);

    // * 3. Game Page
    gamePage = new dinosaur;
    connect(gamePage, &dinosaur::exitToMenu, [this]() {
        stack->setCurrentIndex(0);  // back to menu
    });

    // * Stack pages
    stack->addWidget(menuPage);  // index 0
    stack->addWidget(charPage);  // index 1
    stack->addWidget(gamePage);  // index 2

    setCentralWidget(stack);
    stack->setCurrentIndex(0);  // show menu

    setStyleSheet(R"(
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
    )");
}

void MainWindow::startGame() {
    gamePage->setSkin(selectedSkin);
    gamePage->reset();
    stack->setCurrentIndex(2);

    gamePage->setFocus();
}

void MainWindow::openCharacterSelect() { stack->setCurrentIndex(1); }

void MainWindow::chooseNormalDino() {
    selectedSkin = 0;
    stack->setCurrentIndex(0);
}

void MainWindow::chooseHatDino() {
    selectedSkin = 1;
    stack->setCurrentIndex(0);
}

void MainWindow::chooseSantaDino() {
    selectedSkin = 2;
    stack->setCurrentIndex(0);
}
