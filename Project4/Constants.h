// Constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SFML/Graphics.hpp>

// ������� �������� ����
const int ROWS = 12;
const int COLS = 12;
const int TILE_SIZE = 64;

// ���������� ������
const int NUM_COLORS = 6;

// ����������� ��������� ������ (��������, 10%)
const float BONUS_CHANCE = 0.05f;

// ������������ ���������� ��������� �������� �� ����������, ����� �������� ����������� ������
const int MAX_MATCH_CHECKS = 10;

// ���� �������
enum class BonusType {
    None,
    Recolor,
    Bomb
};

#endif // CONSTANTS_H
