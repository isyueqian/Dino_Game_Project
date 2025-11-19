#include "mainWindow.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>
#include "dinosaur.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    stack = new QStackedWidget(this);

    // * 1. Menu Page
    menuPage = new QWidget;
    QVBoxLayout* mlay = new QVBoxLayout(menuPage);
    mlay->setSpacing(20);
    mlay->setContentsMargins(40, 20, 40, 40);

    mlay->addStretch(1);
    
    // Title
    QLabel* title = new QLabel("<h1 style='font-family:Courier New;font-size:36px;'>Dino Game</h1>");
    title->setAlignment(Qt::AlignCenter);
    mlay->addWidget(title);
    
    mlay->addSpacing(0);
    
    // Start button
    QPushButton* btnStart = new QPushButton("▶ Play");
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
            color: #000000;
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
    QPushButton* btnChar = new QPushButton(" Character Select");
    btnChar->setFixedHeight(50);
    btnChar->setMaximumWidth(250);
    
    // Add dinosaur icon to button
    QPixmap dinoIcon(":/images/images/Dino_Start.png");
    dinoIcon = dinoIcon.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    btnChar->setIcon(QIcon(dinoIcon));
    btnChar->setIconSize(QSize(30, 30));
    
    btnChar->setStyleSheet(R"(
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

    // Center the buttons
    QWidget* buttonContainer = new QWidget;
    QVBoxLayout* buttonLayout = new QVBoxLayout(buttonContainer);
    buttonLayout->setSpacing(15);
    buttonLayout->addWidget(btnStart, 0, Qt::AlignCenter);
    buttonLayout->addWidget(btnChar, 0, Qt::AlignCenter);
    
    mlay->addWidget(buttonContainer);
    mlay->addStretch(2);

    connect(btnStart, &QPushButton::clicked, this, &MainWindow::startGame);
    connect(btnChar, &QPushButton::clicked, this, &MainWindow::openCharacterSelect);

    // * 2. Character Select Page
    charPage = new QWidget;
    QVBoxLayout* clay = new QVBoxLayout(charPage);
    clay->setSpacing(15);
    clay->setContentsMargins(20, 20, 20, 20);
    
    QLabel* charTitle = new QLabel("<h2>Select Your Character</h2>");
    charTitle->setAlignment(Qt::AlignCenter);
    clay->addWidget(charTitle);

    // Horizontal layout for character cards
    QHBoxLayout* charLayout = new QHBoxLayout;
    charLayout->setSpacing(20);

    // Create character preview cards
    for (int i = 0; i < 3; ++i) {
        QWidget* charCard = new QWidget;
        charCard->setObjectName(QString("charCard_%1").arg(i));
        charCard->setCursor(Qt::PointingHandCursor);
        charCard->setFixedSize(100, 120);
        
        QVBoxLayout* cardLayout = new QVBoxLayout(charCard);
        cardLayout->setSpacing(5);
        cardLayout->setContentsMargins(10, 10, 10, 10);

        // Character preview image
        QLabel* previewLabel = new QLabel;
        previewLabel->setAlignment(Qt::AlignCenter);
        previewLabel->setFixedSize(80, 80);
        
        QPixmap preview;
        QString charName;
        
        if (i == 0) {
            preview.load(":/images/images/Dino_Start.png");
            charName = "Normal";
        } else if (i == 1) {
            preview.load(":/images/images/Hat_Start.png");
            charName = "Yellow Hat";
        } else {
            preview.load(":/images/images/Santa_Start.png");
            charName = "Santa";
        }
        
        preview = preview.scaled(70, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        previewLabel->setPixmap(preview);
        
        // Character name
        QLabel* nameLabel = new QLabel(charName);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
        
        cardLayout->addWidget(previewLabel);
        cardLayout->addWidget(nameLabel);
        
        // Store card widget for styling updates
        if (i == 0) charCard0 = charCard;
        else if (i == 1) charCard1 = charCard;
        else charCard2 = charCard;
        
        // Make cards clickable
        charCard->installEventFilter(this);
        
        charLayout->addWidget(charCard);
    }

    clay->addLayout(charLayout);
    
    // Back button 
    QPushButton* backBtn = new QPushButton("← Back");
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
    connect(backBtn, &QPushButton::clicked, [this]() {
        stack->setCurrentIndex(0);
    });
    
    clay->addWidget(backBtn, 0, Qt::AlignCenter);

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
    
    // Set fixed size to match game widget
    setFixedSize(640, 240);

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
    
    // Update initial selection styling
    updateCharacterSelection();
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

void MainWindow::updateCharacterSelection() {
    // Update all cards to show selection state
    if (charCard0) {
        charCard0->setStyleSheet(selectedSkin == 0 ? 
            "background-color: #d0e8ff; border: 4px solid #4a90e2; border-radius: 10px;" :
            "background-color: #f5f5f5; border: 3px solid #ccc; border-radius: 10px;");
    }
    if (charCard1) {
        charCard1->setStyleSheet(selectedSkin == 1 ? 
            "background-color: #d0e8ff; border: 4px solid #4a90e2; border-radius: 10px;" :
            "background-color: #f5f5f5; border: 3px solid #ccc; border-radius: 10px;");
    }
    if (charCard2) {
        charCard2->setStyleSheet(selectedSkin == 2 ? 
            "background-color: #d0e8ff; border: 4px solid #4a90e2; border-radius: 10px;" :
            "background-color: #f5f5f5; border: 3px solid #ccc; border-radius: 10px;");
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget && widget->objectName().startsWith("charCard_")) {
            // Extract character index from object name
            int charIndex = widget->objectName().back().digitValue();
            
            if (charIndex == 0) chooseNormalDino();
            else if (charIndex == 1) chooseHatDino();
            else if (charIndex == 2) chooseSantaDino();
            
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
