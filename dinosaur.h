#ifndef DINOSAUR_H
#define DINOSAUR_H

#include <QElapsedTimer>
#include <QPixmap>
#include <QPushButton>
#include <QRect>
#include <QTimer>
#include <QVector>
#include <QWidget>

// #define SOUND

#ifdef SOUND
#include <QSoundEffect>
#endif

class dinosaur : public QWidget {
    Q_OBJECT
   public:
    explicit dinosaur(QWidget* parent = nullptr);
    void reset();
    void setSkin(int skin);

   protected:
    void paintEvent(QPaintEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void keyReleaseEvent(QKeyEvent* e) override;

   private slots:
    void tick();

   signals:
    void exitToMenu();

   private:
    enum DinoState { RUN, DUCK, START, JUMP, DEAD };

    void spawnCactus();
    void spawnBird();
    void spawnCloud();
    void updateDinoState();
    void updatePhysics(float dt);
    bool checkCollision() const;
    void updateAnimation(float dt);
    void resizeEvent(QResizeEvent* event) override;

    // control buttons
    QPushButton* btnReturn;
    QPushButton* btnRestart;

    // dinosaur
    QRect dino;
    float vy = 0.f;
    bool onGround = true;
    bool isCrouching = false;
    DinoState currentState = RUN;
    bool gameOver = false;
    bool started = false;

    // sprites
    QPixmap gameOverImage;
    QPixmap dinoStartSprite;
    QPixmap dinoJumpSprite;
    QPixmap dinoDeadSprite;
    QVector<QPixmap> runFrames;
    QVector<QPixmap> duckFrames;
    int currentRunFrame = 0;
    int currentDuckFrame = 0;
    float animTimer = 0.f;
    float animFrameDuration = 0.08f;

    // bird sprites
    QPixmap birdSprite1;
    QPixmap birdSprite2;
    int currentBirdFrame = 0;

    // cactus sprites
    QVector<QPixmap> largeCactusSprites;
    QVector<QPixmap> smallCactusSprites;
    QVector<int> cactusTypes;  // Track which sprite to use for each cactus

    // clouds
    QVector<QRect> clouds;
    QPixmap cloudSprite;

    // ground tile sprite
    QPixmap groundSprite;
    float groundX = 0.f;

    // obstacles
    QVector<QRect> cactus;
    QVector<QRect> birds;

    // timers
    QTimer frame;
    QElapsedTimer clock;

    // game parameters
    float speed = 200.f;
    const float baseSpeed = 200.f;
    const float maxSpeed = 420.f;
    int score = 0;
    int highScore = 0;
    float distanceTraveled = 0.f;
    float spawnTimer = 0.f;
    float spawnMin = 1.0f;
    float spawnMax = 1.8f;

    // physics constants
    const int groundY = 200;
    const float gravity = 2400.f;
    const float jumpV = -700.f;

    // day / night cycle
    bool isNight = false;
    int lastColorSwitch = 0;  // last score for color switch

    // ground pattern
    float groundOffset = 0.f;
    float groundPatternSpacing = 20.f;

#ifdef SOUND
    QSoundEffect sJump;
    QSoundEffect sHit;
    QSoundEffect sPoint;
#endif
};

#endif  // DINOSAUR_H
