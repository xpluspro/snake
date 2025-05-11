#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <chrono>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , game(new Game())
    , gameTimer(new QTimer(this))
{
    ui->setupUi(this);
    setFixedSize(GRID_WIDTH * CELL_SIZE + 200, GRID_HEIGHT * CELL_SIZE + 80);  // 进一步增加顶部空间

    // 连接定时器信号
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateGame);
    gameTimer->start(200);  // 200ms 更新一次
}

MainWindow::~MainWindow()
{
    delete ui;
    delete game;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!game->isPaused()) {
        switch (event->key()) {
            case Qt::Key_Up:
                game->getSnake().changeDirection(Direction::UP);
                break;
            case Qt::Key_Down:
                game->getSnake().changeDirection(Direction::DOWN);
                break;
            case Qt::Key_Left:
                game->getSnake().changeDirection(Direction::LEFT);
                break;
            case Qt::Key_Right:
                game->getSnake().changeDirection(Direction::RIGHT);
                break;
            case Qt::Key_P:
                game->togglePause();
                break;
        }
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    drawGame(painter);
}

void MainWindow::drawGame(QPainter &painter)
{
    // 设置抗锯齿
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制背景
    painter.fillRect(rect(), Qt::black);
    
    // 将游戏区域向下移动
    painter.translate(0, 40);  // 向下移动40像素，确保在菜单栏下方
    
    // 绘制游戏元素
    drawBorder(painter);
    drawSnake(painter);
    drawFood(painter);
    drawObstacles(painter);
    drawScore(painter);
}

void MainWindow::drawSnake(QPainter &painter)
{
    painter.setBrush(Qt::green);
    painter.setPen(Qt::NoPen);
    
    for (const auto& segment : game->getSnake().getBody()) {
        QRectF rect(segment.first * CELL_SIZE + 1, 
                   segment.second * CELL_SIZE + 1,
                   CELL_SIZE - 2, 
                   CELL_SIZE - 2);
        painter.drawRoundedRect(rect, 5, 5);
    }
}

void MainWindow::drawFood(QPainter &painter)
{
    auto foodPos = game->getFood().getPosition();
    if (game->getFood().isSpecial()) {
        painter.setBrush(Qt::yellow);
    } else {
        painter.setBrush(Qt::red);
    }
    painter.setPen(Qt::NoPen);
    
    QRectF rect(foodPos.first * CELL_SIZE + 1,
                foodPos.second * CELL_SIZE + 1,
                CELL_SIZE - 2,
                CELL_SIZE - 2);
    painter.drawEllipse(rect);
}

void MainWindow::drawObstacles(QPainter &painter)
{
    painter.setBrush(Qt::gray);
    painter.setPen(Qt::NoPen);
    
    for (const auto& obstacle : game->getObstacles()) {
        QRectF rect(obstacle.first * CELL_SIZE + 1,
                   obstacle.second * CELL_SIZE + 1,
                   CELL_SIZE - 2,
                   CELL_SIZE - 2);
        painter.drawRect(rect);
    }
}

void MainWindow::drawScore(QPainter &painter)
{
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    
    // 将分数显示移到更下方，避免被菜单栏遮挡
    QString scoreText = QString("Score: %1").arg(game->getScore());
    QString highScoreText = QString("High Score: %1").arg(game->getHighScore());
    QString difficultyText = "Difficulty: ";
    
    // 添加当前难度显示
    switch (game->getDifficulty()) {
        case Game::Difficulty::EASY:
            difficultyText += "Easy";
            break;
        case Game::Difficulty::NORMAL:
            difficultyText += "Normal";
            break;
        case Game::Difficulty::HARD:
            difficultyText += "Hard";
            break;
    }
    
    // 调整显示位置，从菜单栏下方开始
    int startY = 50;  // 调整起始位置
    painter.drawText(GRID_WIDTH * CELL_SIZE + 10, startY, scoreText);
    painter.drawText(GRID_WIDTH * CELL_SIZE + 10, startY + 30, highScoreText);
    painter.drawText(GRID_WIDTH * CELL_SIZE + 10, startY + 60, difficultyText);
    
    // 添加自动控制剩余时间显示
    if (game->isAutoPathActive()) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            now - game->getAutoPathStartTime());
        int remainingTime = Game::AUTO_PATH_DURATION - duration.count();
        QString autoText = QString("Auto Control: %1s").arg(remainingTime);
        painter.drawText(GRID_WIDTH * CELL_SIZE + 10, startY + 90, autoText);
    }
    
    if (game->isPaused()) {
        painter.drawText(GRID_WIDTH * CELL_SIZE + 10, startY + 120, "PAUSED");
    }
}

void MainWindow::drawBorder(QPainter &painter)
{
    painter.setPen(QPen(Qt::white, 2));
    painter.drawRect(0, 0, GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE);
}

void MainWindow::updateGame()
{
    if (!game->isPaused()) {
        game->update();
        if (!game->getSnake().getIsAlive()) {
            gameTimer->stop();
            QMessageBox::information(this, "Game Over", 
                QString("Game Over!\nScore: %1").arg(game->getScore()));
        }
        update();  // 触发重绘
    }
}

void MainWindow::on_actionNew_Game_triggered()
{
    Game::Difficulty currentDifficulty = game->getDifficulty();
    delete game;
    game = new Game();
    game->setDifficulty(currentDifficulty);  // 保持当前难度
    gameTimer->start(200);
}

void MainWindow::on_actionPause_triggered()
{
    game->togglePause();
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Game", "", "Snake Game Files (*.snake)");
    if (!fileName.isEmpty()) {
        if (game->saveGame(fileName.toStdString())) {
            QMessageBox::information(this, "Success", "Game saved successfully!");
        } else {
            QMessageBox::warning(this, "Error", "Failed to save game!");
        }
    }
}

void MainWindow::on_actionLoad_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Load Game", "", "Snake Game Files (*.snake)");
    if (!fileName.isEmpty()) {
        if (game->loadGame(fileName.toStdString())) {
            QMessageBox::information(this, "Success", "Game loaded successfully!");
        } else {
            QMessageBox::warning(this, "Error", "Failed to load game!");
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionEasy_triggered()
{
    game->setDifficulty(Game::Difficulty::EASY);
    if (!game->getSnake().getIsAlive()) {
        on_actionNew_Game_triggered();
    }
}

void MainWindow::on_actionNormal_triggered()
{
    game->setDifficulty(Game::Difficulty::NORMAL);
    if (!game->getSnake().getIsAlive()) {
        on_actionNew_Game_triggered();
    }
}

void MainWindow::on_actionHard_triggered()
{
    game->setDifficulty(Game::Difficulty::HARD);
    if (!game->getSnake().getIsAlive()) {
        on_actionNew_Game_triggered();
    }
} 