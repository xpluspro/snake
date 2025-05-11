#include "Food.h"
#include <algorithm>

// 初始化静态成员
std::random_device Food::rd;
std::mt19937 Food::gen(Food::rd());

Food::Food() : type(Type::NORMAL) {
}

void Food::generateNew(int width, int height, const std::list<std::pair<int, int>>& snakeBody, 
                      const std::vector<std::pair<int, int>>& obstacles) {
    std::uniform_int_distribution<> disX(1, width - 2);
    std::uniform_int_distribution<> disY(1, height - 2);
    std::uniform_real_distribution<> disType(0.0, 1.0);

    bool valid = false;
    while (!valid) {
        valid = true;
        position.first = disX(gen);
        position.second = disY(gen);

        // 检查是否与蛇身重叠
        for (const auto& segment : snakeBody) {
            if (segment.first == position.first && segment.second == position.second) {
                valid = false;
                break;
            }
        }

        // 检查是否与障碍物重叠
        if (valid) {
            for (const auto& obstacle : obstacles) {
                if (obstacle.first == position.first && obstacle.second == position.second) {
                    valid = false;
                    break;
                }
            }
        }
    }

    // 20%的概率生成特殊食物
    type = (disType(gen) < 0.2) ? Type::SPECIAL : Type::NORMAL;
}

std::pair<int, int> Food::getPosition() const {
    return position;
}

Food::Type Food::getType() const {
    return type;
}

bool Food::isSpecial() const {
    return type == Type::SPECIAL;
} 