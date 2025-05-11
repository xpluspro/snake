#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "game.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateGame();
    void on_actionNew_Game_triggered();
    void on_actionPause_triggered();
    void on_actionSave_triggered();
    void on_actionLoad_triggered();
    void on_actionExit_triggered();
    void on_actionEasy_triggered();
    void on_actionNormal_triggered();
    void on_actionHard_triggered();

private:
    Ui::MainWindow *ui;
    Game *game;
    QTimer *gameTimer;
    static const int CELL_SIZE = 20;  // 每个格子的像素大小
    static const int GRID_WIDTH = 20;  // 游戏区域宽度（格子数）
    static const int GRID_HEIGHT = 20; // 游戏区域高度（格子数）

    void drawGame(QPainter &painter);
    void drawSnake(QPainter &painter);
    void drawFood(QPainter &painter);
    void drawObstacles(QPainter &painter);
    void drawScore(QPainter &painter);
    void drawBorder(QPainter &painter);
};
#endif // MAINWINDOW_H 