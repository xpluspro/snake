#include "game.h"
#include <fstream>
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>

Game::Game() : snake(WIDTH / 2, HEIGHT / 2), score(0), highScore(0), paused(false),
    difficulty(Difficulty::NORMAL), autoPathEnabled(false), isFollowingPath(false) {
    loadHighScore();
    food = Food();  // 创建新的食物对象
    food.generateNew(WIDTH, HEIGHT, snake.getBody(), obstacles);
    generateObstacles();
}

Game::~Game() {
    saveHighScore();
}

void Game::update() {
    if (paused) return;

    // 检查自动寻路状态
    if (isAutoPathActive()) {
        // 只有在没有路径或路径已用完时才重新寻找路径
        if (!isFollowingPath || currentPath.empty()) {
            Direction newDir = findPathToFood();
            if (newDir != snake.getDirection()) {
                snake.changeDirection(newDir);
            }
        } else {
            // 使用路径中的下一个方向
            Direction nextDir = currentPath.front();
            currentPath.erase(currentPath.begin());
            
            // 验证方向是否安全
            auto head = snake.getBody().front();
            auto currentBody = snake.getBody();
            std::pair<int, int> nextPos = head;
            switch (nextDir) {
                case Direction::UP: nextPos.second--; break;
                case Direction::DOWN: nextPos.second++; break;
                case Direction::LEFT: nextPos.first--; break;
                case Direction::RIGHT: nextPos.first++; break;
                default: break;
            }
            
            currentBody.pop_back();
            currentBody.push_front(nextPos);
            
            if (isValidPosition(nextPos.first, nextPos.second, currentBody)) {
                snake.changeDirection(nextDir);
            } else {
                // 如果方向不安全，重新寻找路径
                isFollowingPath = false;
                currentPath.clear();
                Direction newDir = findPathToFood();
                if (newDir != snake.getDirection()) {
                    snake.changeDirection(newDir);
                }
            }
        }
    }

    snake.move();

    // 检查是否吃到食物
    if (snake.getBody().front() == food.getPosition()) {
        snake.grow();
        score += 10;
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
        
        // 如果吃到特殊食物，启用或重置自动寻路
        if (food.isSpecial()) {
            enableAutoPath();
        }
        
        // 重新生成食物
        food = Food();  // 创建新的食物对象
        food.generateNew(WIDTH, HEIGHT, snake.getBody(), obstacles);
        // 吃到食物后重新寻找路径
        isFollowingPath = false;
        currentPath.clear();
    }

    // 检查碰撞
    auto head = snake.getBody().front();
    if (snake.checkCollision(WIDTH, HEIGHT) || 
        snake.isCollidingWithSelf() || 
        isObstacle(head.first, head.second)) {
        snake.setAlive(false);
    }
}

void Game::togglePause() {
    paused = !paused;
}

void Game::generateObstacles() {
    obstacles.clear();
    if (difficulty == Difficulty::EASY) {
        return;  // 简单难度没有障碍物
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(2, WIDTH - 3);
    std::uniform_int_distribution<> disY(2, HEIGHT - 3);

    int numObstacles = (difficulty == Difficulty::NORMAL) ? 5 : 10;
    
    for (int i = 0; i < numObstacles; ++i) {
        int x, y;
        bool valid;
        do {
            valid = true;
            x = disX(gen);
            y = disY(gen);
            
            // 检查是否与蛇身重叠
            for (const auto& segment : snake.getBody()) {
                if (segment.first == x && segment.second == y) {
                    valid = false;
                    break;
                }
            }
            
            // 检查是否与食物重叠
            if (food.getPosition().first == x && food.getPosition().second == y) {
                valid = false;
            }
            
            // 检查是否与其他障碍物重叠
            for (const auto& obstacle : obstacles) {
                if (obstacle.first == x && obstacle.second == y) {
                    valid = false;
                    break;
                }
            }
        } while (!valid);
        
        obstacles.push_back({x, y});
    }
}

bool Game::isObstacle(int x, int y) const {
    for (const auto& obstacle : obstacles) {
        if (obstacle.first == x && obstacle.second == y) {
            return true;
        }
    }
    return false;
}

void Game::saveHighScore() const {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

void Game::loadHighScore() {
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
}

bool Game::saveGame(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    // 保存游戏状态
    file.write(reinterpret_cast<const char*>(&score), sizeof(score));
    file.write(reinterpret_cast<const char*>(&highScore), sizeof(highScore));
    file.write(reinterpret_cast<const char*>(&difficulty), sizeof(difficulty));

    // 保存蛇的状态
    const auto& body = snake.getBody();
    size_t bodySize = body.size();
    file.write(reinterpret_cast<const char*>(&bodySize), sizeof(bodySize));
    for (const auto& segment : body) {
        file.write(reinterpret_cast<const char*>(&segment.first), sizeof(segment.first));
        file.write(reinterpret_cast<const char*>(&segment.second), sizeof(segment.second));
    }

    // 保存食物位置
    auto foodPos = food.getPosition();
    file.write(reinterpret_cast<const char*>(&foodPos.first), sizeof(foodPos.first));
    file.write(reinterpret_cast<const char*>(&foodPos.second), sizeof(foodPos.second));

    // 保存障碍物
    size_t obstacleSize = obstacles.size();
    file.write(reinterpret_cast<const char*>(&obstacleSize), sizeof(obstacleSize));
    for (const auto& obstacle : obstacles) {
        file.write(reinterpret_cast<const char*>(&obstacle.first), sizeof(obstacle.first));
        file.write(reinterpret_cast<const char*>(&obstacle.second), sizeof(obstacle.second));
    }

    return true;
}

bool Game::loadGame(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    // 读取游戏状态
    file.read(reinterpret_cast<char*>(&score), sizeof(score));
    file.read(reinterpret_cast<char*>(&highScore), sizeof(highScore));
    file.read(reinterpret_cast<char*>(&difficulty), sizeof(difficulty));

    // 读取蛇的状态
    size_t bodySize;
    file.read(reinterpret_cast<char*>(&bodySize), sizeof(bodySize));
    std::list<std::pair<int, int>> newBody;
    for (size_t i = 0; i < bodySize; ++i) {
        int x, y;
        file.read(reinterpret_cast<char*>(&x), sizeof(x));
        file.read(reinterpret_cast<char*>(&y), sizeof(y));
        newBody.push_back({x, y});
    }

    // 读取食物位置
    int foodX, foodY;
    file.read(reinterpret_cast<char*>(&foodX), sizeof(foodX));
    file.read(reinterpret_cast<char*>(&foodY), sizeof(foodY));

    // 读取障碍物
    size_t obstacleSize;
    file.read(reinterpret_cast<char*>(&obstacleSize), sizeof(obstacleSize));
    obstacles.clear();
    for (size_t i = 0; i < obstacleSize; ++i) {
        int x, y;
        file.read(reinterpret_cast<char*>(&x), sizeof(x));
        file.read(reinterpret_cast<char*>(&y), sizeof(y));
        obstacles.push_back({x, y});
    }

    // 更新游戏状态
    snake = Snake(newBody.front().first, newBody.front().second);
    for (auto it = ++newBody.begin(); it != newBody.end(); ++it) {
        snake.grow();
    }
    food = Food();
    food.generateNew(WIDTH, HEIGHT, snake.getBody(), obstacles);

    return true;
}

void Game::enableAutoPath() {
    autoPathEnabled = true;
    autoPathStartTime = std::chrono::steady_clock::now();
    findPathToFood();
}

void Game::disableAutoPath() {
    autoPathEnabled = false;
}

bool Game::isAutoPathActive() const {
    if (!autoPathEnabled) return false;
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - autoPathStartTime);
    return duration.count() < AUTO_PATH_DURATION;
}

bool Game::isValidPosition(int x, int y, const std::list<std::pair<int, int>>& currentBody) const {
    // 检查是否在边界内
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return false;
    }
    
    // 检查是否是障碍物
    if (isObstacle(x, y)) {
        return false;
    }
    
    // 检查是否与蛇身重叠（除了尾部，因为蛇移动时尾部会离开）
    auto tail = currentBody.back();
    for (auto it = currentBody.begin(); it != --currentBody.end(); ++it) {
        if (it->first == x && it->second == y) {
            return false;
        }
    }
    
    return true;
}

Direction Game::findPathToFood() {
    auto head = snake.getBody().front();
    auto foodPos = food.getPosition();
    auto currentBody = snake.getBody();
    
    // 使用BFS寻找到食物的最短路径
    struct State {
        std::pair<int, int> pos;
        std::list<std::pair<int, int>> body;
        std::vector<Direction> path;  // 存储到达该状态的路径
        
        size_t hash() const {
            // 只使用头部位置和身体的前几个部分来计算哈希值
            size_t h = pos.first * 1000 + pos.second;
            int count = 0;
            for (const auto& segment : body) {
                if (count >= 3) break;  // 只使用前3个身体部分
                h = h * 31 + segment.first * 1000 + segment.second;
                count++;
            }
            return h;
        }
    };
    
    std::queue<State> q;
    std::unordered_set<size_t> visited;
    
    // 初始状态
    State initialState = {head, currentBody, {}};
    q.push(initialState);
    visited.insert(initialState.hash());
    
    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        
        if (current.pos == foodPos) {
            // 验证整个路径是否有效
            bool pathValid = true;
            std::list<std::pair<int, int>> simulatedBody = currentBody;
            
            for (const auto& dir : current.path) {
                auto nextPos = simulatedBody.front();
                switch (dir) {
                    case Direction::UP: nextPos.second--; break;
                    case Direction::DOWN: nextPos.second++; break;
                    case Direction::LEFT: nextPos.first--; break;
                    case Direction::RIGHT: nextPos.first++; break;
                    default: break;
                }
                
                if (!isValidPosition(nextPos.first, nextPos.second, simulatedBody)) {
                    pathValid = false;
                    break;
                }
                
                simulatedBody.pop_back();
                simulatedBody.push_front(nextPos);
            }
            
            if (pathValid) {
                // 只有在整个路径都有效时才保存
                currentPath = current.path;
                isFollowingPath = true;
                return current.path.empty() ? snake.getDirection() : current.path[0];
            }
            // 如果路径无效，继续搜索
            continue;
        }
        
        // 尝试所有可能的移动方向
        std::vector<std::pair<Direction, std::pair<int, int>>> moves = {
            {Direction::UP, {current.pos.first, current.pos.second - 1}},
            {Direction::DOWN, {current.pos.first, current.pos.second + 1}},
            {Direction::LEFT, {current.pos.first - 1, current.pos.second}},
            {Direction::RIGHT, {current.pos.first + 1, current.pos.second}}
        };
        
        // 根据食物位置调整移动优先级
        int dx = foodPos.first - current.pos.first;
        int dy = foodPos.second - current.pos.second;
        
        // 根据距离食物的方向调整优先级
        std::sort(moves.begin(), moves.end(), 
            [dx, dy](const std::pair<Direction, std::pair<int, int>>& a, 
                    const std::pair<Direction, std::pair<int, int>>& b) {
                int distA = std::abs(a.second.first - dx) + std::abs(a.second.second - dy);
                int distB = std::abs(b.second.first - dx) + std::abs(b.second.second - dy);
                return distA < distB;
            });
        
        for (const auto& move : moves) {
            const Direction& dir = move.first;
            const std::pair<int, int>& next = move.second;
            
            if (isValidPosition(next.first, next.second, current.body)) {
                std::list<std::pair<int, int>> newBody = current.body;
                newBody.pop_back();
                newBody.push_front(next);
                
                State newState = {next, newBody, current.path};
                newState.path.push_back(dir);
                
                size_t stateHash = newState.hash();
                if (visited.find(stateHash) == visited.end()) {
                    visited.insert(stateHash);
                    q.push(newState);
                }
            }
        }
    }
    
    // 如果找不到路径，使用备选策略
    isFollowingPath = false;
    currentPath.clear();
    return findFallbackDirection();
}

Direction Game::findFallbackDirection() {
    auto head = snake.getBody().front();
    auto foodPos = food.getPosition();
    auto currentBody = snake.getBody();
    
    int dx = foodPos.first - head.first;
    int dy = foodPos.second - head.second;
    
    std::vector<std::pair<Direction, std::pair<int, int>>> fallbackMoves;
    
    if (std::abs(dx) > std::abs(dy)) {
        if (dx > 0) {
            fallbackMoves.push_back({Direction::RIGHT, {head.first + 1, head.second}});
            if (dy > 0) {
                fallbackMoves.push_back({Direction::DOWN, {head.first, head.second + 1}});
                fallbackMoves.push_back({Direction::UP, {head.first, head.second - 1}});
            } else {
                fallbackMoves.push_back({Direction::UP, {head.first, head.second - 1}});
                fallbackMoves.push_back({Direction::DOWN, {head.first, head.second + 1}});
            }
            fallbackMoves.push_back({Direction::LEFT, {head.first - 1, head.second}});
        } else {
            fallbackMoves.push_back({Direction::LEFT, {head.first - 1, head.second}});
            if (dy > 0) {
                fallbackMoves.push_back({Direction::DOWN, {head.first, head.second + 1}});
                fallbackMoves.push_back({Direction::UP, {head.first, head.second - 1}});
            } else {
                fallbackMoves.push_back({Direction::UP, {head.first, head.second - 1}});
                fallbackMoves.push_back({Direction::DOWN, {head.first, head.second + 1}});
            }
            fallbackMoves.push_back({Direction::RIGHT, {head.first + 1, head.second}});
        }
    } else {
        if (dy > 0) {
            fallbackMoves.push_back({Direction::DOWN, {head.first, head.second + 1}});
            if (dx > 0) {
                fallbackMoves.push_back({Direction::RIGHT, {head.first + 1, head.second}});
                fallbackMoves.push_back({Direction::LEFT, {head.first - 1, head.second}});
            } else {
                fallbackMoves.push_back({Direction::LEFT, {head.first - 1, head.second}});
                fallbackMoves.push_back({Direction::RIGHT, {head.first + 1, head.second}});
            }
            fallbackMoves.push_back({Direction::UP, {head.first, head.second - 1}});
        } else {
            fallbackMoves.push_back({Direction::UP, {head.first, head.second - 1}});
            if (dx > 0) {
                fallbackMoves.push_back({Direction::RIGHT, {head.first + 1, head.second}});
                fallbackMoves.push_back({Direction::LEFT, {head.first - 1, head.second}});
            } else {
                fallbackMoves.push_back({Direction::LEFT, {head.first - 1, head.second}});
                fallbackMoves.push_back({Direction::RIGHT, {head.first + 1, head.second}});
            }
            fallbackMoves.push_back({Direction::DOWN, {head.first, head.second + 1}});
        }
    }
    
    for (const auto& move : fallbackMoves) {
        const Direction& dir = move.first;
        const std::pair<int, int>& pos = move.second;
        if (isValidPosition(pos.first, pos.second, currentBody)) {
            return dir;
        }
    }
    
    return snake.getDirection();
} 