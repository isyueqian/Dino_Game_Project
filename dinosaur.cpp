#include "dinosaur.h"
#include <QPainter>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <cmath>
#include <QDebug>

dinosaur::dinosaur(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Dinosaur Game (Qt Widget)");
    setFixedSize(640, 240);

    // Load sprites from resources
    dinoStandSprite.load(":/images/images/Right_Run.png");
    dinoCrouchSprite.load(":/images/images/Right_Duck.png");
    
    // Scale sprites to match game dimensions
    dinoStandSprite = dinoStandSprite.scaled(36, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    dinoCrouchSprite = dinoCrouchSprite.scaled(72, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Dinosaur animation
    auto loadFrames = [](QVector<QPixmap> &vec,
                         const QString &baseName,
                         int count,
                         const QSize &targetSize) {
        for (int i = 0; i < count; ++i) {
            QString path = QString(":/images/images/%1_%2.png").arg(baseName).arg(i+1);
            QPixmap pm(path);
            if (!pm.isNull()) {
                vec.push_back(pm.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
    };

    loadFrames(runFrames,  "Right_Run",  2, QSize(36, 40));
    loadFrames(duckFrames, "Right_Duck", 2, QSize(72, 25));

    qDebug() << "Size of runFrames: " << runFrames.size();
    qDebug() << "Size of duckFrames: " << duckFrames.size();

    if (runFrames.isEmpty())  runFrames.push_back(dinoStandSprite);
    if (duckFrames.isEmpty()) duckFrames.push_back(dinoCrouchSprite);

    // Sound effect

    reset();

    frame.setTimerType(Qt::PreciseTimer);
    connect(&frame, &QTimer::timeout, this, &dinosaur::tick);
    frame.start(16); // ~60 FPS
    clock.start();
}

void dinosaur::reset() {
    dino = QRect(40, groundY - 40, 36, 40); // x,y,w,h
    vy = 0;
    onGround = true;
    isCrouching = false;
    currentState = RUN;
    cactus.clear();
    birds.clear();
    speed = baseSpeed;
    score = 0;
    spawnTimer = 0.f;
    dayNightTimer = 0.f;
    isNight = false;
    groundOffset = 0.f;
    gameOver = false;
    started = false;
    animTimer = 0.f;
    currentRunFrame = currentDuckFrame = 0;
    clock.restart();
}

void dinosaur::spawnCactus() {
    // random cactus
    int w = 10 + (int)QRandomGenerator::global()->bounded(0, 12);
    int h = 20 + (int)QRandomGenerator::global()->bounded(0, 20);
    int x = width() + QRandomGenerator::global()->bounded(0, 40);
    int y = groundY - h;
    cactus.push_back(QRect(x, y, w, h));
}

void dinosaur::spawnBird() {
    int w = 28;
    int h = 18;
    int x = width() + QRandomGenerator::global()->bounded(0, 60);

    int yLevel = QRandomGenerator::global()->bounded(0, 3);
    int y;
    if (yLevel == 0)
        y = groundY - 60;
    else if (yLevel == 1)
        y = groundY - 90;
    else
        y = groundY - 120;
    birds.push_back(QRect(x, y, w, h));
}

void dinosaur::updateDinoState() {
    if (onGround && isCrouching) {
        currentState = DUCK;
    } else {
        currentState = RUN;
    }
}

void dinosaur::updateAnimation(float dt) {
    animTimer += dt;
    if (animTimer < animFrameDuration) return;
    animTimer -= animFrameDuration;

    if (currentState == RUN) {
        currentRunFrame = (currentRunFrame + 1) % runFrames.size();
    } else {
        currentDuckFrame = (currentDuckFrame + 1) % duckFrames.size();
    }
}

void dinosaur::updatePhysics(float dt) {
    if (!started) {
        dayNightTimer += dt;
        if (dayNightTimer >= dayNightPeriod) {
            dayNightTimer = 0.f;
            isNight = !isNight;
        }
        return;
    }

    // background moves toward left
    int dx = (int)std::round(-speed * dt);
    for (auto &r : cactus) r.translate(dx, 0);
    for (auto &b : birds)     b.translate(dx, 0);

    // ground pattern moves
    groundOffset += speed * dt * 0.3f;
    if (groundOffset > groundPatternSpacing)
        groundOffset = std::fmod(groundOffset, groundPatternSpacing);

    // remove the background and keep score
    for (int i = cactus.size() - 1; i >= 0; --i) {
        if (cactus[i].right() < 0) {
            cactus.remove(i);
            score += 1;
            // increase speed
            speed = std::min(maxSpeed, speed + 4.f);
        }
    }
    for (int i = birds.size() - 1; i >= 0; --i) {
        if (birds[i].right() < 0) {
            birds.remove(i);
            score += 2; // higher score
            // increase speed
            speed = std::min(maxSpeed, speed + 6.f);
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

    // dinosaur crouches
    if (onGround && isCrouching) {
        // Apply crouching state
        int oldBottom = dino.bottom();
        dino.setHeight(20);  
        dino.moveBottom(oldBottom); 
    }

    updateDinoState();
    updateAnimation(dt);

    // shift day & nights
    dayNightTimer += dt;
    if (dayNightTimer >= dayNightPeriod) {
        dayNightTimer = 0.f;
        isNight = !isNight;
    }

    // create cactus
    spawnTimer -= dt;
    if (spawnTimer <= 0.f) {

        float obstacleType = QRandomGenerator::global()->bounded(1000) / 1000.f;

        if (obstacleType < 0.7f) {
            spawnCactus();
        } else {
            spawnBird();
        }

        float r = QRandomGenerator::global()->bounded(1000) / 1000.f;
        float gap = spawnMin + r * (spawnMax - spawnMin);
        // slightly reduce the gap
        spawnTimer = std::max(0.7f, gap - (speed - 180.f) / 600.f);
    }
}

bool dinosaur::checkCollision() const {
    for (const auto &c : cactus) {
        if (dino.intersects(c)) return true;
    }
    for (const auto &b : birds) {
        if (dino.intersects(b)) return true;
    }
    return false;
}

void dinosaur::tick() {
    float dt = clock.restart() / 1000.0f;
    if (!gameOver) {
        updatePhysics(dt);
        if (started && checkCollision()) {
            gameOver = true;
        }
    }
    update();
}

void dinosaur::paintEvent(QPaintEvent*) {
    QPainter p(this);
    QColor bg = isNight ? QColor(30, 30, 30) : Qt::white;
    QColor fg = isNight ? Qt::white : Qt::black;
    p.fillRect(rect(), bg);

    // ground
    p.setPen(QPen(fg, 2));
    p.drawLine(0, groundY + 1, width(), groundY + 1);
    p.setPen(QPen(fg, 1));
    for (float x = -groundPatternSpacing; x < width() + groundPatternSpacing; x += groundPatternSpacing) {
        float px = x - std::fmod(groundOffset, groundPatternSpacing);
        p.drawLine(QPointF(px, groundY + 3), QPointF(px + 5, groundY + 3));
    }

    // dinosaur
    const QPixmap *sprite = nullptr;
    if (currentState == DUCK && !duckFrames.isEmpty()) {
        sprite = &duckFrames[currentDuckFrame];
    } else if (!runFrames.isEmpty()) {
        sprite = &runFrames[currentRunFrame];
    } else {
        // fallback
        sprite = (currentState == DUCK) ? &dinoCrouchSprite : &dinoStandSprite;
    }
    if (sprite)
        p.drawPixmap(dino.topLeft(), *sprite);

    // cactus
    p.setBrush(fg);
    p.setPen(Qt::NoPen);
    for (const auto &r : std::as_const(cactus)) {
        p.drawRect(r);
        p.drawRect(r.x()-4, r.y()+r.height()/3, 4, r.height()/3);
        p.drawRect(r.right(), r.y()+r.height()/2-6, 4, r.height()/3);
    }

    // birds
    for (const auto &b : std::as_const(birds)) {
        QRect body = b;
        body.adjust(2, 4, -2, -2);
        p.drawEllipse(body);
        QPoint wingCenter(body.center().x() - 6, body.center().y() - 3);
        p.drawLine(wingCenter, wingCenter + QPoint(-6, -5));
        p.drawLine(wingCenter, wingCenter + QPoint(6, -5));
    }

    // score
    p.setPen(Qt::black);
    p.setBrush(Qt::NoBrush);
    p.drawText(10, 20, QStringLiteral("Score: %1  Speed:%2").arg(score).arg((int)speed));

    // UI
    if (!started && !gameOver) {
        p.drawText(width()/2 - 120, height()/2 - 10,
                   QStringLiteral("Press SPACE / UP / W to start"));
        p.drawText(width()/2 - 80, height()/2 + 15,
                   QStringLiteral("DOWN / S to duck"));
    } else if (gameOver) {
        p.drawText(width()/2 - 100, height()/2,
                   QStringLiteral("GAME OVER - Press R to restart"));
    }
}

void dinosaur::keyPressEvent(QKeyEvent *e) {
    if (e->isAutoRepeat()) {
        QWidget::keyPressEvent(e);
        return;
    }

    if (e->key() == Qt::Key_Space || e->key() == Qt::Key_Up || e->key() == Qt::Key_W) {
        // if (onGround && !gameOver) {
        //     onGround = false;
        //     vy = jumpV;
        // }
        if (!gameOver) {
            if (!started) {
                started = true;
            }
            if (onGround) {
                onGround = false;
                vy = jumpV;

                int oldBottom = dino.bottom();
                dino.setHeight(40);
                dino.moveBottom(oldBottom);
            }
        }
    } else if (e->key() == Qt::Key_Down || e->key() == Qt::Key_S) {
        if (!gameOver) {
            if (!isCrouching) {
                isCrouching = true;
            }
            // accelerate falling to ground
            if (!onGround) vy += 300.f;
        }
    } else if (e->key() == Qt::Key_R) {
        reset();
    } else if (e->key() == Qt::Key_Escape) {
        close();
    }
    QWidget::keyPressEvent(e);
}

void dinosaur::keyReleaseEvent(QKeyEvent *e) {
    if (e->isAutoRepeat()) {
        QWidget::keyReleaseEvent(e);
        return;
    }

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
