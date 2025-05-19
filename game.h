#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QVector>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QLabel>
#include <QPixmap>
#include <random>

constexpr int MAP_PIXEL = 750;
constexpr int GRID_PIXEL = 25;
constexpr int MAP_SIZE = MAP_PIXEL / GRID_PIXEL;

namespace Ui {
class Game;
}

class Game : public QWidget
{
    Q_OBJECT

public:
    explicit Game(QWidget *parent = nullptr);
    ~Game() override;

    enum class Direction{
        NORTH, SOUTH, WEST, EAST
    };
    enum {EMPTY, WALL, FOOD, BODY, UP, LEFT, DOWN, RIGHT, HEAD};
public slots:
    void snakeMove();
    void addFood();
    void start();
    void end();

    void accelerate();
    void slowDown();

public:
    void pausePress();
    void pause();
    void resume(); //暂停后的继续

    void setDirection(Direction dir);
    void moveLabel(QLabel *label,const QPoint &dst, int type);
    void moveLabelNext(QLabel *label, int type);

    void paintEvent(QPaintEvent *ev) override;
    QLabel* draw(int type, int x, int y);
    bool isNextFood();
    bool isNextWall();
    bool isNextBody();

    void resetTimer();
    void resetSta();

    static QPoint pixelToIndex(const QLabel*);

signals:
    void endSignal();

private:
    Ui::Game *ui;
    QVector<QVector<int>> map_;

    bool is_end_ = true;
    bool eaten = false;
    bool is_paused_ = false;
    Direction dir_ = Direction::NORTH;

    QLabel *head_ = nullptr;
    QList<QLabel*> bodies_;
    QList<QLabel*> foods_;
    QTimer *timer_ = nullptr;
    QTimer *food_timer_ = nullptr;
    const int min_interval_ = 100;
    int interval_ = 300;
    const int max_interval_ = 900;
    int food_interval_ = 3000;
    int speed_ = 1;

    static QMap<Direction, QPointF> dir_and_point_;
    QMap<int, QPixmap> pixmaps_;
    std::default_random_engine e_;
    std::uniform_int_distribution<int> d_;

};

#endif // GAME_H
