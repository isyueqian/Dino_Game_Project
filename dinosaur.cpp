#include "dinosaur.h"
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QRandomGenerator>
#include <cmath>

void loadFrames(QVector<QPixmap>& vec, const QString& baseName, int count, const QSize& targetSize) {
    for (int i = 0; i < count; ++i) {
        QString path = QString(":/images/images/%1_%2.png").arg(baseName).arg(i + 1);
        QPixmap pm(path);
        if (!pm.isNull()) {
            vec.push_back(pm.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

dinosaur::dinosaur(QWidget* parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);

    setWindowTitle("Dinosaur Game (Qt Widget)");
    setFixedSize(480, 272);

    cloudSprite.load(":/images/images/Cloud.png");
    groundSprite.load(":/images/images/Ground.png");
    gameOverImage.load(":/images/images/Game_Over.png");
    cloudSprite = cloudSprite.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    groundSprite = groundSprite.scaledToHeight(20, Qt::SmoothTransformation);
    gameOverImage = gameOverImage.scaled(200, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Setup control buttons
    btnReturn = new QPushButton(this);
    btnRestart = new QPushButton(this);

    QPixmap returnImg(":/images/images/Back_Button.png");
    QPixmap scaledReturn = returnImg.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap restartImg(":/images/images/Restart.png");
    QPixmap scaledRestart = restartImg.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    btnReturn->setIcon(scaledReturn);
    btnReturn->setIconSize(QSize(48, 48));
    btnReturn->setFixedSize(48, 48);

    btnRestart->setIcon(scaledRestart);
    btnRestart->setIconSize(QSize(48, 48));
    btnRestart->setFixedSize(48, 48);

    btnReturn->move(10, 10);

    btnRestart->move(width() / 2, height() / 2);
    btnRestart->hide();

    btnReturn->setFocusPolicy(Qt::NoFocus);
    btnRestart->setFocusPolicy(Qt::NoFocus);

    connect(btnReturn, &QPushButton::clicked, [this]() { emit exitToMenu(); });

    connect(btnRestart, &QPushButton::clicked, [this]() {
        reset();
        btnRestart->hide();
        setFocus();
    });
    btnReturn->setStyleSheet("border: none; background: transparent;");
    btnRestart->setStyleSheet("border: none; background: transparent;");

    // Load bird sprites
    birdSprite1.load(":/images/images/Bird1.png");
    birdSprite2.load(":/images/images/Bird2.png");
    birdSprite1 = birdSprite1.scaled(42, 27, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    birdSprite2 = birdSprite2.scaled(42, 27, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Load cactus sprites
    for (int i = 1; i <= 3; ++i) {
        QPixmap largeCactus(QString(":/images/images/LargeCactus%1.png").arg(i));
        largeCactusSprites.push_back(largeCactus.scaled(60, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        QPixmap smallCactus(QString(":/images/images/SmallCactus%1.png").arg(i));
        smallCactusSprites.push_back(smallCactus.scaled(60, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    
#ifdef SOUND
    sJump.setSource(QUrl("qrc:/sounds/sounds/jump.wav"));
    sJump.setVolume(0.25f);
    sHit.setSource(QUrl("qrc:/sounds/sounds/hit.wav"));
    sHit.setVolume(0.35f);
    sPoint.setSource(QUrl("qrc:/sounds/sounds/point.wav"));
    sPoint.setVolume(0.20f);
#endif

    reset();

    frame.setTimerType(Qt::PreciseTimer);
    connect(&frame, &QTimer::timeout, this, &dinosaur::tick);
    frame.start(16);  // ~60 FPS
    clock.start();
}

void dinosaur::setSkin(int skin) {
    runFrames.clear();
    duckFrames.clear();
    if (skin == 0) {
        // normal dino sprite
        dinoStartSprite.load(":/images/images/Dino_Start.png");
        dinoJumpSprite.load(":/images/images/Dino_Jump.png");
        dinoDeadSprite.load(":/images/images/Dino_Dead.png");
        dinoStartSprite = dinoStartSprite.scaled(36, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        dinoJumpSprite = dinoJumpSprite.scaled(36, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        dinoDeadSprite = dinoDeadSprite.scaled(36, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Dinosaur animation
        loadFrames(runFrames, "Dino_Run", 2, QSize(36, 40));
        loadFrames(duckFrames, "Dino_Duck", 2, QSize(72, 25));

    } else if (skin == 1) {
        // hat dino sprite
        dinoStartSprite.load(":/images/images/Hat_Start.png");
        dinoJumpSprite.load(":/images/images/Hat_Jump.png");
        dinoDeadSprite.load(":/images/images/Hat_Dead.png");
        dinoStartSprite = dinoStartSprite.scaled(38, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        dinoJumpSprite = dinoJumpSprite.scaled(38, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        dinoDeadSprite = dinoDeadSprite.scaled(38, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Dinosaur animation
        loadFrames(runFrames, "Hat_Run", 2, QSize(38, 42));
        loadFrames(duckFrames, "Hat_Duck", 2, QSize(72, 28));

    } else if (skin == 2) {
        // santa dino sprite
        dinoStartSprite.load(":/images/images/Santa_Start.png");
        dinoJumpSprite.load(":/images/images/Santa_Jump.png");
        dinoDeadSprite.load(":/images/images/Santa_Dead.png");
        dinoStartSprite = dinoStartSprite.scaled(38, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        dinoJumpSprite = dinoJumpSprite.scaled(38, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        dinoDeadSprite = dinoDeadSprite.scaled(38, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Dinosaur animation
        loadFrames(runFrames, "Santa_Run", 2, QSize(38, 42));
        loadFrames(duckFrames, "Santa_Duck", 2, QSize(72, 28));

    } else if (skin == 3) {
        // cowboy dino sprite
        dinoStartSprite.load(":/images/images/Cowboy_Start.png");
        dinoJumpSprite.load(":/images/images/Cowboy_Jump.png");
        dinoDeadSprite.load(":/images/images/Cowboy_Dead.png");
        dinoStartSprite = dinoStartSprite.scaled(38, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        dinoJumpSprite = dinoJumpSprite.scaled(38, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        dinoDeadSprite = dinoDeadSprite.scaled(38, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Dinosaur animation
        loadFrames(runFrames, "Cowboy_Run", 2, QSize(38, 42));
        loadFrames(duckFrames, "Cowboy_Duck", 2, QSize(72, 28));

    } else if (skin == 4) {
        // pirate dino sprite
        dinoStartSprite.load(":/images/images/Pirate_Start.png");
        dinoJumpSprite.load(":/images/images/Pirate_Jump.png");
        dinoDeadSprite.load(":/images/images/Pirate_Dead.png");
        dinoStartSprite = dinoStartSprite.scaled(38, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        dinoJumpSprite = dinoJumpSprite.scaled(38, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        dinoDeadSprite = dinoDeadSprite.scaled(38, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Dinosaur animation
        loadFrames(runFrames, "Pirate_Run", 2, QSize(38, 42));
        loadFrames(duckFrames, "Pirate_Duck", 2, QSize(72, 28));
    }
}

void dinosaur::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    btnReturn->move(10, 10);

    int restartW = btnRestart->width();
    int restartH = btnRestart->height();
    btnRestart->move((width() - restartW) / 2, (height() - restartH) / 2 + 35);
}

void dinosaur::reset() {
    dino = QRect(40, groundY - 40, 36, 40);  // x,y,w,h
    vy = 0;
    onGround = true;
    isCrouching = false;
    currentState = START;
    cactus.clear();
    cactusTypes.clear();
    birds.clear();
    clouds.clear();
    groundX = 0.f;
    speed = baseSpeed;
    score = 0;
    distanceTraveled = 0.f;
    spawnTimer = 0.f;
    isNight = false;
    lastColorSwitch = 0;
    groundOffset = 0.f;
    gameOver = false;
    started = false;
    animTimer = 0.f;
    currentRunFrame = currentDuckFrame = 0;
    currentBirdFrame = 0;
    btnRestart->hide();
    clock.restart();
}

void dinosaur::spawnCactus() {
    // Randomly choose between large and small cactus
    bool isLarge = QRandomGenerator::global()->bounded(2) == 0;
    int spriteIndex = QRandomGenerator::global()->bounded(0, 3);

    QPixmap cactusSprite;
    if (isLarge) {
        cactusSprite = largeCactusSprites[spriteIndex];
        cactusTypes.push_back(spriteIndex);  // 0-2 for large
    } else {
        cactusSprite = smallCactusSprites[spriteIndex];
        cactusTypes.push_back(spriteIndex + 3);  // 3-5 for small
    }

    int w = cactusSprite.width();
    int h = cactusSprite.height();
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

    // Update bird animation
    currentBirdFrame = (currentBirdFrame + 1) % 2;
}

void dinosaur::updatePhysics(float dt) {
    if (!started) {
        return;
    }

    // background moves toward left
    int dx = (int)std::round(-speed * dt);
    for (auto& r : cactus) r.translate(dx, 0);
    // move birds (faster than cactus)
    int birdDx = (int)std::round(-speed * dt * 1.4f);
    for (auto& b : birds) b.translate(birdDx, 0);
    // move clouds (slower than cactus)
    for (auto& c : clouds) c.translate(dx * 1.5, 0);

    // ground pattern moves
    groundOffset += speed * dt * 0.3f;
    if (groundOffset > groundPatternSpacing) groundOffset = std::fmod(groundOffset, groundPatternSpacing);

    // update distance traveled and calculate score
    distanceTraveled += speed * dt;
    int newScore = (int)(distanceTraveled / 10.0f);

    // play sound and increase speed when score increases by 100
    if (newScore / 100 > score / 100) {
        speed = std::min(maxSpeed, speed + 10.f);
#ifdef SOUND
        if (sPoint.isLoaded()) {
            sPoint.play();
        }
#endif
    }
    score = newScore;

    // remove obstacles when they go off screen
    for (int i = cactus.size() - 1; i >= 0; --i) {
        if (cactus[i].right() < 0) {
            cactus.remove(i);
            cactusTypes.remove(i);
        }
    }
    for (int i = birds.size() - 1; i >= 0; --i) {
        if (birds[i].right() < 0) {
            birds.remove(i);
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

    // Day/night cycle based on score milestones
    int initial = 200;
    if (score >= initial) {
        int milestone = ((score - initial) / 200) + 1;
        if (milestone != lastColorSwitch) {
            lastColorSwitch = milestone;
            isNight = !isNight;
        }
    }

    // create obstacles
    spawnTimer -= dt;
    if (spawnTimer <= 0.f) {
        float obstacleType = QRandomGenerator::global()->bounded(1000) / 1000.f;

        bool isBird = (obstacleType >= 0.8f);

        if (isBird) {
            spawnBird();
        } else {
            spawnCactus();
        }

        // maybe spawn a cloud (about 20% chance)
        float cloudChance = QRandomGenerator::global()->bounded(1000) / 1000.f;
        if (cloudChance < 0.8f) {
            spawnCloud();
        }

        float r = QRandomGenerator::global()->bounded(1000) / 1000.f;
        float gap = spawnMin + r * (spawnMax - spawnMin);
        // Adjust gap based on speed, and increase gap for birds since they move faster
        float adjustedGap = gap - (speed - 180.f) / 600.f;
        if (isBird) {
            adjustedGap *= 1.3f;  // Increase gap for birds to compensate for their faster speed
        }
        spawnTimer = std::max(0.9f, adjustedGap);  // Minimum gap increased to 0.9s for safety
    }

    groundX -= speed * dt;
    if (groundX <= -groundSprite.width()) groundX += groundSprite.width();
}

bool dinosaur::checkCollision() const {
    const int MIN_OVERLAP = 30;

    for (const auto& c : cactus) {
        QRect inter = dino.intersected(c);
        if (inter.width() > 0 && inter.height() > 0) {
            if (inter.width() * inter.height() > MIN_OVERLAP) {
                return true;
            }
        }
    }

    for (const auto& b : birds) {
        QRect inter = dino.intersected(b);
        if (inter.width() > 0 && inter.height() > 0) {
            if (inter.width() * inter.height() > MIN_OVERLAP) {
                return true;
            }
        }
    }

    return false;
}

void dinosaur::tick() {
    float dt = clock.restart() / 1000.0f;
    if (!gameOver) {
        updatePhysics(dt);
        if (started && checkCollision()) {
            gameOver = true;
            btnRestart->show();
            currentState = DEAD;
#ifdef SOUND
            if (sHit.isLoaded()) {
                sHit.play();
            }
#endif
            // Update high score if current score is higher
            if (score > highScore) {
                highScore = score;
            }
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

    // draw clouds (behind dinosaur and birds)
    for (const auto& c : std::as_const(clouds)) {
        p.drawPixmap(c.topLeft(), cloudSprite);
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
            sprite = &duckFrames[currentDuckFrame];
            break;

        case RUN:
        default:
            sprite = &runFrames[currentRunFrame];
            break;
    }

    if (sprite) p.drawPixmap(dino.topLeft(), *sprite);

    // cactus
    for (int i = 0; i < cactus.size(); ++i) {
        const QRect& r = cactus[i];
        int type = cactusTypes[i];

        const QPixmap& cactusSprite = (type < 3) ? largeCactusSprites[type] : smallCactusSprites[type - 3];

        if (isNight) {
            // Convert to grayscale for night time
            QImage img = cactusSprite.toImage().convertToFormat(QImage::Format_ARGB32);
            for (int y = 0; y < img.height(); ++y) {
                for (int x = 0; x < img.width(); ++x) {
                    QColor col = img.pixelColor(x, y);
                    if (col.alpha() > 0) {  // Only modify non-transparent pixels
                        int gray = qGray(col.rgb());
                        // Lighten the gray value for better visibility
                        gray = qMin(255, gray + 100);
                        img.setPixelColor(x, y, QColor(gray, gray, gray, col.alpha()));
                    }
                }
            }
            p.drawPixmap(r.topLeft(), QPixmap::fromImage(img));
        } else {
            p.drawPixmap(r.topLeft(), cactusSprite);
        }
    }

    // birds
    for (const auto& b : std::as_const(birds)) {
        const QPixmap& birdSprite = (currentBirdFrame == 0) ? birdSprite1 : birdSprite2;
        // Bird 2 (wings up) needs to be slightly higher to align properly
        int yOffset = (currentBirdFrame == 0) ? 0 : -7;

        if (isNight) {
            // Convert to grayscale for night time
            QImage img = birdSprite.toImage().convertToFormat(QImage::Format_ARGB32);
            for (int y = 0; y < img.height(); ++y) {
                for (int x = 0; x < img.width(); ++x) {
                    QColor col = img.pixelColor(x, y);
                    if (col.alpha() > 0) {  // Only modify non-transparent pixels
                        int gray = qGray(col.rgb());
                        // Lighten the gray value for better visibility
                        gray = qMin(255, gray + 100);
                        img.setPixelColor(x, y, QColor(gray, gray, gray, col.alpha()));
                    }
                }
            }
            p.drawPixmap(b.x(), b.y() + yOffset, QPixmap::fromImage(img));
        } else {
            p.drawPixmap(b.x(), b.y() + yOffset, birdSprite);
        }
    }

    // scores
    QFont gameFont("Menlo", 15, QFont::Bold);
    p.setFont(gameFont);
    p.setPen(isNight ? Qt::white : QColor(83, 83, 83));  // Dark gray
    p.setBrush(Qt::NoBrush);

    QFontMetrics fm(gameFont);

    // Display high score if it exists (after first game)
    if (highScore > 0) {
        // Format: "HI 00123 00045"
        QString scoreDisplay = QString("HI %1 %2").arg(highScore, 5, 10, QChar('0')).arg(score, 5, 10, QChar('0'));
        int displayWidth = fm.horizontalAdvance(scoreDisplay);
        p.drawText(width() - displayWidth - 20, 30, scoreDisplay);
    } else {
        // current score
        QString scoreText = QString("%1").arg(score, 5, 10, QChar('0'));
        int scoreWidth = fm.horizontalAdvance(scoreText);
        p.drawText(width() - scoreWidth - 20, 30, scoreText);
    }

    // UI
    QFont uiFont("Menlo", 15, QFont::Normal);
    p.setFont(uiFont);
    if (!started && !gameOver) {
        p.drawText(width() / 2 - 150, height() / 2 - 12, QStringLiteral("Press SPACE/UP/W to start"));
        p.drawText(width() / 2 - 90, height() / 2 + 17, QStringLiteral("DOWN/S to duck"));
    } else if (gameOver) {
        QFont gameOverFont("Menlo", 15, QFont::Normal);
        p.setFont(gameOverFont);
        p.drawText(width() / 2 - 100, height() / 2, QStringLiteral("Press R to restart"));

        int imgW = gameOverImage.width();
        int imgH = gameOverImage.height();
        int x = (width() - imgW) / 2;
        int y = (height() - imgH) / 2 - 30;

        p.drawPixmap(x, y, gameOverImage);
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
#ifdef SOUND
                if (sJump.isLoaded()) {
                    sJump.play();
                }
#endif
                currentState = JUMP;

                int oldBottom = dino.bottom();
                dino.setHeight(40);
                dino.moveBottom(oldBottom);
            }
        } else {
            reset();
        }
    } else if (e->key() == Qt::Key_Down || e->key() == Qt::Key_S) {
        if (!gameOver) {
            if (!isCrouching) {
                isCrouching = true;
            }
            // accelerate falling to ground
            if (!onGround) vy += 300.f;
        } else {
            emit exitToMenu();
        }
    } else if (e->key() == Qt::Key_R) {
        reset();
    } else if (e->key() == Qt::Key_Escape) {
        emit exitToMenu();
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
