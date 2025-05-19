#include "game.h"
#include "ui_game.h"

#include <QPainter>

QMap<Game::Direction, QPointF> Game::dir_and_point_ =
    {
        {Direction::NORTH, QPointF{0, -1}},
        {Direction::SOUTH, QPointF(0, 1)},
        {Direction::EAST, QPointF(1, 0)},
        {Direction::WEST, QPointF(-1, 0)}
    };

Game::Game(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Game)
    , map_(MAP_SIZE, QVector<int>(MAP_SIZE, EMPTY))
    , e_(time(nullptr))
    , d_(5, MAP_SIZE - 5)
{
    ui->setupUi(this);

    this->setFixedSize(MAP_PIXEL, MAP_PIXEL);
    pixmaps_[WALL].load(":/gamepics/wall.jpg");
    pixmaps_[FOOD].load(":/gamepics/food.png");
    pixmaps_[BODY].load(":/gamepics/body.png");
    pixmaps_[UP].load(":/gamepics/up.png");
    pixmaps_[DOWN].load(":/gamepics/down.png");
    pixmaps_[LEFT].load(":/gamepics/left.png");
    pixmaps_[RIGHT].load(":/gamepics/right.png");
}

Game::~Game()
{
    if(!this->is_end_){
        this->end();
    }
    delete ui;
}

void Game::snakeMove()
{
    bool is_to_die = isNextBody() || isNextWall();
    bool eaten = isNextFood();
    QPoint h = pixelToIndex(head_);
    moveLabelNext(head_, HEAD);
    if(!bodies_.empty()){
        if(!eaten){
            auto back = bodies_.back();
            bodies_.pop_back();
            bodies_.emplaceFront(back);
            moveLabel(back, h, BODY);
        }
        else{
            bodies_.emplaceFront(draw(BODY, h.y(), h.x()));
            h = pixelToIndex(head_);
            eaten = false;
            for(auto p : foods_){
                if(pixelToIndex(p) == h){
                    //qDebug() << "removeFood";
                    foods_.removeAll(p);
                    delete p;
                }
            }
        }
    }
    else{
        if(eaten){
            bodies_.emplaceFront(draw(BODY, h.y(), h.x()));
            h = pixelToIndex(head_);
            eaten = false;
            for(auto p : foods_){
                if(pixelToIndex(p) == h){
                    //qDebug() << "removeFood";
                    foods_.removeOne(p);
                    delete p;
                    break;
                }
            }
        }
    }

    switch(dir_){
    case Direction::NORTH:
        head_->setPixmap(pixmaps_[UP]);
        break;
    case Direction::SOUTH:
        head_->setPixmap(pixmaps_[DOWN]);
        break;
    case Direction::WEST:
        head_->setPixmap(pixmaps_[LEFT]);
        break;
    case Direction::EAST:
        head_->setPixmap(pixmaps_[RIGHT]);
        break;
    }

    if(is_to_die){
        emit endSignal();
        //this->end();
        return;
    }
}

void Game::start()
{
    this->resetSta();
    //qDebug() << "start!";
    is_end_ = false;
    for(int i = 0; i < MAP_SIZE; ++i){
        for(int j = 0; j < MAP_SIZE; ++j){
            if(i == 0 || i == MAP_SIZE - 1
                || j == 0 || j == MAP_SIZE - 1){
                map_[i][j] = WALL;
                draw(WALL, j, i);
            }
        }
    }

    head_ = draw(UP, d_(e_), d_(e_));

    timer_ = new QTimer(this);
    timer_->start(interval_);
    connect(timer_, &QTimer::timeout, this, &Game::snakeMove);

    food_timer_ = new QTimer(this);
    food_timer_->start(food_interval_);
    connect(food_timer_, &QTimer::timeout, this, &Game::addFood);
}

void Game::end()
{
    //qDebug() << "end!";
    is_end_ = true;

    this->resetTimer();
    delete timer_;
    timer_ = nullptr;
    delete food_timer_;
    food_timer_ = nullptr;
}

void Game::accelerate()
{
    interval_ = std::max(interval_ / 3, min_interval_);
    timer_->setInterval(interval_);
    food_interval_ = interval_ * 10;
    qDebug() << "food_interval_ "<< food_interval_;
    food_timer_->setInterval(food_interval_);
}

void Game::slowDown()
{
    interval_ = std::min(interval_ * 3, max_interval_);
    timer_->setInterval(interval_);
    food_interval_ = interval_ * 10;
    qDebug() << "food_interval_ "<< food_interval_;
    food_timer_->setInterval(food_interval_);
}

void Game::addFood()
{
    //qDebug() << "AddFood!";
    int x = 0;
    int y = 0;
    do{
        x = d_(e_);
        y = d_(e_);
    }while(map_[x][y] != EMPTY);
    foods_.emplaceBack(draw(FOOD, x, y));
}

void Game::pausePress()
{
    if(is_paused_){
        resume();
    }
    else{
        pause();
    }
    is_paused_ = !is_paused_;
}

void Game::pause()
{
    this->resetTimer();
}

void Game::resume()
{
    timer_->start();
    food_timer_->start();
}


void Game::setDirection(Direction dir)
{
    switch (dir) {
    case Direction::NORTH:
        if(dir_ != Direction::SOUTH){
            dir_ = dir;
        }
        break;
    case Direction::SOUTH:
        if(dir_ != Direction::NORTH){
            dir_ = dir;
        }
        break;
    case Direction::WEST:
        if(dir_ != Direction::EAST){
            dir_ = dir;
        }
        break;
    case Direction::EAST:
        if(dir_ != Direction::WEST){
            dir_ = dir;
        }
        break;
    default:
        break;
    }
}

void Game::moveLabel(QLabel *label,const QPoint &dst, int type)
{
    QPoint p = pixelToIndex(label);
    map_[p.x()][p.y()] = EMPTY;
    label->move(dst.y() * GRID_PIXEL + 2,
                dst.x() * GRID_PIXEL + 2);
    map_[dst.x()][dst.y()] = type;
}

void Game::moveLabelNext(QLabel *label, int type)
{
    QPoint p = pixelToIndex(label);
    map_[p.x()][p.y()] = EMPTY;
    label->move(label->x() + speed_ * dir_and_point_[dir_].x() * GRID_PIXEL,
                label->y() + speed_ * dir_and_point_[dir_].y() * GRID_PIXEL);
    p = pixelToIndex(label);
    map_[p.x()][p.y()] = type;
}

void Game::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    painter.setPen(QPen{Qt::black});
    for(int i = 0; i <= MAP_SIZE; ++i){
        painter.drawLine(0, i * GRID_PIXEL, MAP_PIXEL, i * GRID_PIXEL);
        painter.drawLine(i * GRID_PIXEL, MAP_PIXEL, i * GRID_PIXEL, 0);
    }

}

QLabel* Game::draw(int type, int x, int y)
{
    //qDebug() << "draw " << type;
    QLabel *label = new QLabel(this);
    label->setPixmap(pixmaps_[type]);
    label->setFixedSize(GRID_PIXEL - 4, GRID_PIXEL - 4);
    label->move(x * GRID_PIXEL + 2, y * GRID_PIXEL + 2);
    map_[y][x] = type;
    //qDebug() << "map_[y][x]: " << map_[y][x];
    label->setVisible(true);
    return label;
}

bool Game::isNextFood()
{
    QPoint head_pos_ = pixelToIndex(head_);
    switch (dir_) {
    case Direction::NORTH:
        return map_[head_pos_.x() - 1][head_pos_.y()] == FOOD;
        break;
    case Direction::SOUTH:
        return map_[head_pos_.x() + 1][head_pos_.y()] == FOOD;
        break;
    case Direction::WEST:
        return map_[head_pos_.x()][head_pos_.y() - 1] == FOOD;
        break;
    case Direction::EAST:
        return map_[head_pos_.x()][head_pos_.y() + 1] == FOOD;
        break;
    default:
        return false;
        break;
    }
}

bool Game::isNextWall()
{
    QPoint head_pos_ = pixelToIndex(head_);
    switch (dir_) {
    case Direction::NORTH:
        return head_pos_.x() == 1;
        break;
    case Direction::SOUTH:
        return head_pos_.x() == MAP_SIZE - 2;
        break;
    case Direction::WEST:
        return head_pos_.y() == 1;
        break;
    case Direction::EAST:
        return head_pos_.y() == MAP_SIZE - 2;
        break;
    default:
        return false;
        break;
    }

}

bool Game::isNextBody()
{
    QPoint head_pos_ = pixelToIndex(head_);
    switch (dir_) {
    case Direction::NORTH:
        return map_[head_pos_.x() - 1][head_pos_.y()] == BODY;
        break;
    case Direction::SOUTH:
        return map_[head_pos_.x() + 1][head_pos_.y()] == BODY;
        break;
    case Direction::WEST:
        return map_[head_pos_.x()][head_pos_.y() - 1] == BODY;
        break;
    case Direction::EAST:
        return map_[head_pos_.x()][head_pos_.y() + 1] == BODY;
        break;
    default:
        return false;
        break;
    }
}

void Game::resetTimer()
{
    timer_->stop();
    food_timer_->stop();
}

void Game::resetSta()
{
    for(auto p : bodies_){
        delete p;
    }
    bodies_.clear();

    if(head_ != nullptr){
        delete head_;
    }
    head_ = nullptr;

    for(auto p : foods_){
        delete p;
    }
    foods_.clear();
}

QPoint Game::pixelToIndex(const QLabel *label)
{
    int x = (label->x() - 2) / GRID_PIXEL;
    int y = (label->y() - 2) / GRID_PIXEL;
    return QPoint{y, x};
}

