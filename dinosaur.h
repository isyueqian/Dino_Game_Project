#pragma once
#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QRect>
#include <QElapsedTimer>

class dinosaur : public QWidget {
    Q_OBJECT
public:
    explicit dinosaur(QWidget *parent=nullptr);

protected:
    void paintEvent(QPaintEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void keyReleaseEvent(QKeyEvent *e) override;

private slots:
    void tick();

private:

    enum DinoState {
        RUN,
        DUCK
    };

    void reset();
    void spawnObstacle();
    void updateDinoState();
    void updatePhysics(float dt);
    bool checkCollision() const;
    

    QRect dino;
    float vy = 0.f;
    bool onGround = true;
    bool isCrouching = false;
    DinoState currentState = RUN;
    int groundY = 180;
    QVector<QRect> obstacles;
    float speed = 180.f;
    float gravity = 900.f;
    float jumpV = -380.f;
    float spawnMin = 1.2f, spawnMax = 2.2f;
    float spawnTimer = 0.f;

    bool gameOver = false;
    int score = 0;
    QTimer frame;
    QElapsedTimer clock;

    QPixmap dinoStandSprite;
    QPixmap dinoCrouchSprite;
};
