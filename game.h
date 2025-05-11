#ifndef GAME_H
#define GAME_H

#include "Snake.h"
#include "Food.h"
#include <chrono>
#include <string>
#include <vector>
#include <list>

class Game {
public:
    enum class Difficulty {
        EASY,
        NORMAL,
        HARD
    };

    static const int AUTO_PATH_DURATION = 60;  // 自动寻路持续时间（秒）

    Game();
    ~Game();

    void update();
    void togglePause();
    bool isPaused() const { return paused; }
    int getScore() const { return score; }
    int getHighScore() const { return highScore; }
    const Snake& getSnake() const { return snake; }
    Snake& getSnake() { return snake; }
    const Food& getFood() const { return food; }
    Food& getFood() { return food; }
    const std::vector<std::pair<int, int>>& getObstacles() const { return obstacles; }
    void setDifficulty(Difficulty d) { 
        difficulty = d; 
        generateObstacles();
    }
    Difficulty getDifficulty() const { return difficulty; }
    bool saveGame(const std::string& filename) const;
    bool loadGame(const std::string& filename);
    const std::chrono::steady_clock::time_point& getAutoPathStartTime() const { return autoPathStartTime; }
    bool isAutoPathActive() const;

private:
    static const int WIDTH = 20;
    static const int HEIGHT = 20;
    static const int MAX_SAVES = 5;

    Snake snake;
    Food food;
    int score;
    int highScore;
    bool paused;
    Difficulty difficulty;
    std::vector<std::pair<int, int>> obstacles;
    bool autoPathEnabled;
    std::chrono::steady_clock::time_point autoPathStartTime;
    std::vector<Direction> currentPath;
    bool isFollowingPath;

    void generateObstacles();
    bool isObstacle(int x, int y) const;
    void saveHighScore() const;
    void loadHighScore();
    void enableAutoPath();
    void disableAutoPath();
    bool isValidPosition(int x, int y, const std::list<std::pair<int, int>>& currentBody) const;
    Direction findPathToFood();
    Direction findFallbackDirection();
};

#endif // GAME_H 