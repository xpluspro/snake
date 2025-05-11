#include "Snake.h"
#include <algorithm>

Snake::Snake(int startX, int startY) : direction(Direction::RIGHT), isAlive(true) {
    // 初始化蛇身，长度为3
    body.push_back({startX, startY});
    body.push_back({startX - 1, startY});
    body.push_back({startX - 2, startY});
}

void Snake::move() {
    if (!isAlive) return;

    // 获取头部位置
    auto head = body.front();
    
    // 根据方向移动头部
    switch (direction) {
        case Direction::UP:
            head.second--;
            break;
        case Direction::DOWN:
            head.second++;
            break;
        case Direction::LEFT:
            head.first--;
            break;
        case Direction::RIGHT:
            head.first++;
            break;
    }
    
    // 在头部添加新的位置
    body.push_front(head);
    // 移除尾部
    body.pop_back();
}

void Snake::grow() {
    // 复制尾部位置
    auto tail = body.back();
    // 在尾部添加新的位置
    body.push_back(tail);
}

bool Snake::checkCollision(int width, int height) const {
    auto head = body.front();
    return head.first < 0 || head.first >= width || 
           head.second < 0 || head.second >= height;
}

bool Snake::isCollidingWithSelf() const {
    auto head = body.front();
    auto it = body.begin();
    ++it;  // 跳过头部
    for (; it != body.end(); ++it) {
        if (it->first == head.first && it->second == head.second) {
            return true;
        }
    }
    return false;
}

void Snake::changeDirection(Direction newDirection) {
    // 防止180度转向
    if ((direction == Direction::UP && newDirection == Direction::DOWN) ||
        (direction == Direction::DOWN && newDirection == Direction::UP) ||
        (direction == Direction::LEFT && newDirection == Direction::RIGHT) ||
        (direction == Direction::RIGHT && newDirection == Direction::LEFT)) {
        return;
    }
    direction = newDirection;
}

const std::list<std::pair<int, int>>& Snake::getBody() const {
    return body;
}

bool Snake::getIsAlive() const {
    return isAlive;
}

void Snake::setAlive(bool alive) {
    isAlive = alive;
}

Direction Snake::getDirection() const {
    return direction;
}

void Snake::setBody(const std::list<std::pair<int, int>>& newBody) {
    body = newBody;
} 