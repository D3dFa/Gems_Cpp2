// Constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SFML/Graphics.hpp>

// Размеры игрового поля
const int ROWS = 12;
const int COLS = 12;
const int TILE_SIZE = 64;

// Количество цветов
const int NUM_COLORS = 6;

// Вероятность выпадения бонуса (например, 10%)
const float BONUS_CHANCE = 0.05f;

// Максимальное количество повторных проверок на совпадения, чтобы избежать бесконечных циклов
const int MAX_MATCH_CHECKS = 10;

// Типы бонусов
enum class BonusType {
    None,
    Recolor,
    Bomb
};

#endif // CONSTANTS_H
