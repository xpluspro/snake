#ifndef FOOD_H
#define FOOD_H

#include <utility>
#include <list>
#include <random>
#include <vector>

class Food {
public:
    enum class Type {
        NORMAL,     // 普通食物
        SPECIAL     // 特殊食物（触发自动寻路）
    };

    Food();                        // 构造函数
    void generateNew(int width, int height, const std::list<std::pair<int, int>>& snakeBody,
                    const std::vector<std::pair<int, int>>& obstacles);  // 生成新的食物
    std::pair<int, int> getPosition() const;  // 获取食物位置
    Type getType() const;
    bool isSpecial() const;

private:
    std::pair<int, int> position;
    Type type;
    static std::random_device rd;  // 静态随机设备
    static std::mt19937 gen;       // 静态随机数生成器
};

#endif // FOOD_H 