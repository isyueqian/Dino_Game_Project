#include "dinosaur.h"
#include <QPainter>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <cmath>

dinosaur::dinosaur(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Dinosaur Game (Qt Widget)");
    setFixedSize(640, 240);

    // Load sprites from resources
    dinoStandSprite.load(":/images/images/Right_Run.png");
    dinoCrouchSprite.load(":/images/images/Right_Duck.png");
    
    // Scale sprites to match game dimensions
    dinoStandSprite = dinoStandSprite.scaled(36, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    dinoCrouchSprite = dinoCrouchSprite.scaled(36, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    reset();

    frame.setTimerType(Qt::PreciseTimer);
    connect(&frame, &QTimer::timeout, this, &dinosaur::tick);
    frame.start(16); // ~60 FPS
    clock.start();
}

void dinosaur::reset() {
    dino = QRect(40, groundY - 40, 36, 40); // x,y,w,h
    vy = 0; onGround = true;
    isCrouching = false;
    currentState = RUN;
    obstacles.clear();
    speed = 180.f;
    score = 0;
    spawnTimer = 0.f;
    gameOver = false;
    clock.restart();
}

void dinosaur::spawnObstacle() {
    // random cactus
    int w = 18 + (int)QRandomGenerator::global()->bounded(0, 12);
    int h = 30 + (int)QRandomGenerator::global()->bounded(0, 20);
    int x = width() + QRandomGenerator::global()->bounded(0, 40);
    int y = groundY - h;
    obstacles.push_back(QRect(x, y, w, h));
}

void dinosaur::updateDinoState() {
    if (onGround && isCrouching) {
        currentState = DUCK;
    } else {
        currentState = RUN;
    }
}

void dinosaur::updatePhysics(float dt) {
    // background moves toward left
    for (auto &r : obstacles) r.translate((int)std::round(-speed * dt), 0);

    // remove the background and keep score
    for (int i = obstacles.size()-1; i >= 0; --i) {
        if (obstacles[i].right() < 0) {
            obstacles.remove(i);
            score += 1;
            // increase speed
            speed = std::min(420.f, speed + 4.f);
        }
    }

    // dinosaur move vertically
    if (!onGround) {
        vy += gravity * dt;
        dino.translate(0, (int)std::round(vy * dt));
        if (dino.bottom() >= groundY) {
            dino.moveBottom(groundY);
            vy = 0.f;
            onGround = true;
        }
    }

    if (onGround && isCrouching) {
        // Apply crouching state
        int oldBottom = dino.bottom();
        dino.setHeight(20);  
        dino.moveBottom(oldBottom); 
    }
    updateDinoState();

    // create obstacles
    spawnTimer -= dt;
    if (spawnTimer <= 0.f) {
        spawnObstacle();
        // float gap = spawnMin + QRandomGenerator::global()->bounded(0.0, (double)(spawnMax - spawnMin));
        float r = QRandomGenerator::global()->bounded(1000) / 1000.f;
        float gap = spawnMin + r * (spawnMax - spawnMin);
        // slightly reduce the gap
        spawnTimer = std::max(0.7f, gap - (speed - 180.f) / 600.f);
    }
}

bool dinosaur::checkCollision() const {
    for (const auto &r : obstacles) {
        if (dino.intersects(r)) return true;
    }
    return false;
}

void dinosaur::tick() {
    float dt = clock.restart() / 1000.0f;
    if (!gameOver) {
        updatePhysics(dt);
        if (checkCollision()) gameOver = true;
    }
    update();
}

void dinosaur::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), Qt::white);

    // ground
    p.setPen(QPen(Qt::black, 2));
    p.drawLine(0, groundY + 1, width(), groundY + 1);

    // dinosaur
    switch (currentState) {
        case DUCK:
            p.drawPixmap(dino.topLeft(), dinoCrouchSprite);
            break;
        case RUN:
        default:
            p.drawPixmap(dino.topLeft(), dinoStandSprite);
            break;
    }

    // cactus
    p.setBrush(Qt::black);
    for (const auto &r : std::as_const(obstacles)) {
        p.drawRect(r);
        p.drawRect(r.x()-4, r.y()+r.height()/3, 4, r.height()/3);
        p.drawRect(r.right(), r.y()+r.height()/2-6, 4, r.height()/3);
    }

    // score
    p.setPen(Qt::black);
    p.setBrush(Qt::NoBrush);
    p.drawText(10, 20, QStringLiteral("Score: %1  Speed:%2").arg(score).arg((int)speed));

    if (gameOver) {
        p.drawText(width()/2 - 60, height()/2,
                   QStringLiteral("GAME OVER - Press R to restart"));
    }
}

void dinosaur::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Space || e->key() == Qt::Key_Up || e->key() == Qt::Key_W) {
        if (onGround && !gameOver) {
            onGround = false;
            vy = jumpV;
        }
    } else if (e->key() == Qt::Key_Down || e->key() == Qt::Key_S) {
        if (!gameOver && !isCrouching) {
            isCrouching = true;
        }
        // accelerate falling to ground
        if (!onGround) vy += 300.f;
    } else if (e->key() == Qt::Key_R) {
        reset();
    } else if (e->key() == Qt::Key_Escape) {
        close();
    }
    QWidget::keyPressEvent(e);
}

void dinosaur::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Down || e->key() == Qt::Key_S) {
        if (isCrouching) {
            isCrouching = false;
            // Return to normal height
            int oldBottom = dino.bottom();
            dino.setHeight(40);  
            dino.moveBottom(oldBottom); 
        }
    }
    QWidget::keyReleaseEvent(e);
}
