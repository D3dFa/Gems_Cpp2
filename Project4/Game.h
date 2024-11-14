// Game.h
#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <random>
#include "Tile.h"
#include "Constants.h"
#include "ResourceManager.h"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();

    void initializeGrid();
    bool swapTiles(int firstRow, int firstCol, int secondRow, int secondCol);
    bool checkMatches(std::vector<std::pair<int, int>>& matchesOut);
    void removeMatches(const std::vector<std::pair<int, int>>& matches, bool generateBonuses = true);
    void applyGravity();
    void applyBonus(BonusType bonus, int row, int col);
    bool isAdjacent(int row1, int col1, int row2, int col2);
    void activateBomb(int row, int col);

    sf::RenderWindow window;
    std::vector<std::vector<Tile>> grid;

    // Для обработки выбора двух квадратов
    bool firstSelected;
    int selectedRow, selectedCol;

    // Переменные для отслеживания двойных кликов
    sf::Clock clickClock;
    sf::Time lastClickTime;
    sf::Vector2i lastClickPos;
    const float DOUBLE_CLICK_TIME = 0.3f; // Максимальное время между кликами (в секундах)
    const int DOUBLE_CLICK_DISTANCE = 5;   // Максимальное расстояние между кликами (в пикселях)

    // Генератор случайных чисел
    std::mt19937 rng;

    // Вектор цветов
    std::vector<sf::Color> colors;

    // Время между кадрами для анимации
    sf::Clock clock;

    // Счёт
    int score;
    sf::Text scoreText;

    // Звуковые эффекты
    sf::Sound bombSound;
    sf::Sound recolorSound;

    // Анимации
    struct Animation {
        sf::RectangleShape shape;
        sf::Clock clock;
        float duration;
        bool active;

        Animation() : duration(0.5f), active(false) {}
    };

    std::vector<Animation> activeAnimations;
};

#endif // GAME_H
