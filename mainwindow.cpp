#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , game_(new Game(this))
{
    ui->setupUi(this);
    game_->move(50,50);
    game_->setFixedSize(MAP_PIXEL, MAP_PIXEL);
    this->setFixedSize(MAP_PIXEL + 300, MAP_PIXEL+150);

    QFont font1 = QFont{"Consolas", 18};
    //QFont font2 = QFont{"Comic Sans MS", 26, QFont::Bold};

    QPushButton *start_btn = new QPushButton(this);
    QPushButton *end_btn = new QPushButton(this);
    QPushButton *pause_btn = new QPushButton(this);
    QPushButton *acc_btn = new QPushButton(this);
    QPushButton *slow_btn = new QPushButton(this);

    start_btn->move(MAP_PIXEL + 150, 50);
    start_btn->setText("开始");
    start_btn->adjustSize();
    connect(start_btn, &QPushButton::clicked, game_, &Game::start);
    connect(start_btn, &QPushButton::clicked, [=](){
        start_btn->setEnabled(false);
        end_btn->setEnabled(true);
        pause_btn->setEnabled(true);
        acc_btn->setEnabled(true);
        slow_btn->setEnabled(true);
    });

    end_btn->move(MAP_PIXEL + 150, 150);
    end_btn->setText("结束");
    end_btn->adjustSize();
    end_btn->setEnabled(false);
    connect(end_btn, &QPushButton::clicked, game_, &Game::end);
    connect(end_btn, &QPushButton::clicked, [=](){
        start_btn->setEnabled(true);
        end_btn->setEnabled(false);
        pause_btn->setEnabled(false);
        acc_btn->setEnabled(false);
        slow_btn->setEnabled(false);
    });
    connect(game_, &Game::endSignal, end_btn, &QPushButton::click);

    pause_btn->move(MAP_PIXEL + 150, 250);
    pause_btn->setText("暂停");
    pause_btn->adjustSize();
    pause_btn->setEnabled(false);
    connect(pause_btn, &QPushButton::clicked, game_, &Game::pausePress);
    connect(pause_btn, &QPushButton::clicked, [=](){
        if(pause_btn->text() == "暂停"){
            pause_btn->setText("继续");
            acc_btn->setEnabled(false);
            slow_btn->setEnabled(false);
        }
        else if(pause_btn->text() == "继续"){
            pause_btn->setText("暂停");
            acc_btn->setEnabled(true);
            slow_btn->setEnabled(true);
        }

    });

    acc_btn->move(MAP_PIXEL + 150, 350);
    acc_btn->setText("加速");
    acc_btn->adjustSize();
    acc_btn->setEnabled(false);
    connect(acc_btn, &QPushButton::clicked, game_, &Game::accelerate);

    slow_btn->move(MAP_PIXEL + 150, 450);
    slow_btn->setText("减速");
    slow_btn->adjustSize();
    slow_btn->setEnabled(false);
    connect(slow_btn, &QPushButton::clicked, game_, &Game::slowDown);

    start_btn->setFont(font1);
    end_btn->setFont(font1);
    pause_btn->setFont(font1);
    acc_btn->setFont(font1);
    slow_btn->setFont(font1);

    start_btn->setFixedSize(100, 40);
    end_btn->setFixedSize(100, 40);
    pause_btn->setFixedSize(100, 40);
    acc_btn->setFixedSize(100, 40);
    slow_btn->setFixedSize(100, 40);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    switch(ev->key()){
    case Qt::Key_A:
        game_->setDirection(Game::Direction::WEST);
        break;
    case Qt::Key_W:
        game_->setDirection(Game::Direction::NORTH);
        break;
    case Qt::Key_S:
        game_->setDirection(Game::Direction::SOUTH);
        break;
    case Qt::Key_D:
        game_->setDirection(Game::Direction::EAST);
        break;
    case Qt::Key_Space:
        game_->pausePress();
        break;
    }
}
