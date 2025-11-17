#include "dinosaur.h"
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QRandomGenerator>
#include <cmath>

dinosaur::dinosaur(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Dinosaur Game (Qt Widget)");
    setFixedSize(640, 240);

    // Load sprites from resources
    dinoStandSprite.load(":/images/images/Right_Run.png");
    dinoCrouchSprite.load(":/images/images/Right_Duck.png");

    dinoStartSprite.load(":/images/images/Dino_Start.png");
    dinoJumpSprite.load(":/images/images/Dino_Jump.png");
    dinoDeadSprite.load(":/images/images/Dino_Dead.png");

    cloudSprite.load(":/images/images/Cloud.png");
    groundSprite.load(":/images/images/Ground.png");

    // Scale sprites to match game dimensions
    dinoStandSprite = dinoStandSprite.scaled(36, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    dinoCrouchSprite = dinoCrouchSprite.scaled(72, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    dinoStartSprite = dinoStartSprite.scaled(36, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    dinoJumpSprite = dinoJumpSprite.scaled(36, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    dinoDeadSprite = dinoDeadSprite.scaled(36, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    cloudSprite = cloudSprite.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    groundSprite = groundSprite.scaledToHeight(20, Qt::SmoothTransformation);

    // Dinosaur animation
    auto loadFrames = [](QVector<QPixmap>& vec, const QString& baseName, int count, const QSize& targetSize) {
        for (int i = 0; i < count; ++i) {
            QString path = QString(":/images/images/%1_%2.png").arg(baseName).arg(i + 1);
            QPixmap pm(path);
            if (!pm.isNull()) {
                vec.push_back(pm.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
    };

    loadFrames(runFrames, "Right_Run", 2, QSize(36, 40));
    loadFrames(duckFrames, "Right_Duck", 2, QSize(72, 25));

    if (runFrames.isEmpty()) runFrames.push_back(dinoStandSprite);
    if (duckFrames.isEmpty()) duckFrames.push_back(dinoCrouchSprite);

    // Sound effect

    reset();

    frame.setTimerType(Qt::PreciseTimer);
    connect(&frame, &QTimer::timeout, this, &dinosaur::tick);
    frame.start(16);  // ~60 FPS
    clock.start();
}

void dinosaur::reset() {
    dino = QRect(40, groundY - 40, 36, 40);  // x,y,w,h
    vy = 0;
    onGround = true;
    isCrouching = false;
    currentState = START;
    cactus.clear();
    birds.clear();
    clouds.clear();
    groundX = 0.f;
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

void dinosaur::spawnCloud() {
    int w = cloudSprite.width();
    int h = cloudSprite.height();
    int x = width() + QRandomGenerator::global()->bounded(0, 50);

    // clouds appear at random heights above the ground
    int y = QRandomGenerator::global()->bounded(20, 120);
    clouds.push_back(QRect(x, y, w, h));
}

void dinosaur::updateDinoState() {
    if (currentState == JUMP) return;

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
    for (auto& r : cactus) r.translate(dx, 0);
    for (auto& b : birds) b.translate(dx, 0);
    // move clouds (slower than cactus)
    for (auto& c : clouds) c.translate(dx, 0);

    // ground pattern moves
    groundOffset += speed * dt * 0.3f;
    if (groundOffset > groundPatternSpacing) groundOffset = std::fmod(groundOffset, groundPatternSpacing);

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
            score += 2;  // higher score
            // increase speed
            speed = std::min(maxSpeed, speed + 6.f);
        }
    }
    for (int i = clouds.size() - 1; i >= 0; --i) {
        if (clouds[i].right() < 0) {
            clouds.remove(i);
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

            if (isCrouching) {
                currentState = DUCK;
            } else {
                currentState = RUN;
            }
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

    // create obstacles
    spawnTimer -= dt;
    if (spawnTimer <= 0.f) {
        float obstacleType = QRandomGenerator::global()->bounded(1000) / 1000.f;

        if (obstacleType < 0.7f) {
            spawnCactus();
        } else {
            spawnBird();
        }

        // maybe spawn a cloud (about 20% chance)
        float cloudChance = QRandomGenerator::global()->bounded(1000) / 1000.f;
        if (cloudChance < 0.8f) {
            spawnCloud();
        }

        float r = QRandomGenerator::global()->bounded(1000) / 1000.f;
        float gap = spawnMin + r * (spawnMax - spawnMin);
        // slightly reduce the gap
        spawnTimer = std::max(0.7f, gap - (speed - 180.f) / 600.f);
    }

    groundX -= speed * dt;
    if (groundX <= -groundSprite.width()) groundX += groundSprite.width();
}

bool dinosaur::checkCollision() const {
    for (const auto& c : cactus) {
        if (dino.intersects(c)) return true;
    }
    for (const auto& b : birds) {
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
            currentState = DEAD;
        }
    }
    update();
}

void dinosaur::paintEvent(QPaintEvent*) {
    QPainter p(this);
    QColor bg = isNight ? QColor(30, 30, 30) : Qt::white;
    QColor fg = isNight ? Qt::white : Qt::black;
    p.fillRect(rect(), bg);

    // draw ground sprite repeating
    float gx = groundX;
    while (gx < width()) {
        p.drawPixmap((int)gx, groundY - groundSprite.height() + 2, groundSprite);
        gx += groundSprite.width();
    }

    // dinosaur
    const QPixmap* sprite = nullptr;

    switch (currentState) {
        case START:
            sprite = &dinoStartSprite;
            break;

        case JUMP:
            sprite = &dinoJumpSprite;
            break;

        case DEAD:
            sprite = &dinoDeadSprite;
            break;

        case DUCK:
            if (!duckFrames.isEmpty())
                sprite = &duckFrames[currentDuckFrame];
            else
                sprite = &dinoCrouchSprite;
            break;

        case RUN:
        default:
            if (!runFrames.isEmpty())
                sprite = &runFrames[currentRunFrame];
            else
                sprite = &dinoStandSprite;
            break;
    }

    if (sprite) p.drawPixmap(dino.topLeft(), *sprite);

    // cactus
    p.setBrush(fg);
    p.setPen(Qt::NoPen);
    for (const auto& r : std::as_const(cactus)) {
        p.drawRect(r);
        p.drawRect(r.x() - 4, r.y() + r.height() / 3, 4, r.height() / 3);
        p.drawRect(r.right(), r.y() + r.height() / 2 - 6, 4, r.height() / 3);
    }

    // birds
    for (const auto& b : std::as_const(birds)) {
        QRect body = b;
        body.adjust(2, 4, -2, -2);
        p.drawEllipse(body);
        QPoint wingCenter(body.center().x() - 6, body.center().y() - 3);
        p.drawLine(wingCenter, wingCenter + QPoint(-6, -5));
        p.drawLine(wingCenter, wingCenter + QPoint(6, -5));
    }

    // draw clouds (behind dinosaur)
    for (const auto& c : std::as_const(clouds)) {
        p.drawPixmap(c.topLeft(), cloudSprite);
    }

    // score
    p.setPen(Qt::black);
    p.setBrush(Qt::NoBrush);
    p.drawText(10, 20, QStringLiteral("Score: %1  Speed:%2").arg(score).arg((int)speed));

    // UI
    if (!started && !gameOver) {
        p.drawText(width() / 2 - 120, height() / 2 - 10, QStringLiteral("Press SPACE / UP / W to start"));
        p.drawText(width() / 2 - 80, height() / 2 + 15, QStringLiteral("DOWN / S to duck"));
    } else if (gameOver) {
        p.drawText(width() / 2 - 100, height() / 2, QStringLiteral("GAME OVER - Press R to restart"));
    }
}

void dinosaur::keyPressEvent(QKeyEvent* e) {
    if (e->isAutoRepeat()) {
        QWidget::keyPressEvent(e);
        return;
    }

    if (e->key() == Qt::Key_Space || e->key() == Qt::Key_Up || e->key() == Qt::Key_W) {
        if (!gameOver) {
            if (!started) {
                started = true;
            }
            if (onGround) {
                onGround = false;
                vy = jumpV;
                currentState = JUMP;

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

void dinosaur::keyReleaseEvent(QKeyEvent* e) {
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