#ifndef SNAKE_H
#define SNAKE_H

#include <list>
#include <utility>

// 方向枚举
enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Snake {
public:
    Snake(int startX, int startY);        // 构造函数
    void move();                          // 移动蛇
    void grow();                          // 增长蛇身
    bool checkCollision(int width, int height) const;  // 检查碰撞
    bool isCollidingWithSelf() const;           // 检查是否撞到自己
    void changeDirection(Direction newDirection);      // 改变方向
    const std::list<std::pair<int, int>>& getBody() const;  // 获取蛇身
    bool getIsAlive() const;                    // 获取存活状态
    void setAlive(bool alive);                  // 设置存活状态
    Direction getDirection() const;               // 获取当前方向
    void setBody(const std::list<std::pair<int, int>>& newBody);  // 设置蛇身

private:
    std::list<std::pair<int, int>> body;  // 蛇身体，使用链表存储坐标
    Direction direction;                   // 当前移动方向
    bool isAlive;                         // 蛇是否存活
};

#endif // SNAKE_H 