// Game.cpp
#include "Game.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <stack>

Game::Game()
    : window(sf::VideoMode(COLS * TILE_SIZE, ROWS * TILE_SIZE + 50), "GEMS"),
    firstSelected(false),
    selectedRow(-1),
    selectedCol(-1),
    rng(std::random_device{}()),
    colors({
        sf::Color::Red,
        sf::Color::Green,
        sf::Color::Blue,
        sf::Color::Yellow,
        sf::Color::Magenta,
        sf::Color(255, 165, 0) // ��������� ��� ��������������� �����
        }),
    score(0)
{
    // �������� �������� ����� ResourceManager
    ResourceManager& rm = ResourceManager::getInstance();

    // �������� �������� ��� Recolor ������ (���� ������������)
    if (rm.loadTexture("recolor_icon", "recolor_icon.png")) {
     // �������� ������� ���������
     }

    // �������� ������
    if (rm.loadSoundBuffer("bomb_sound", "bomb_activation.wav")) {
        bombSound.setBuffer(rm.getSoundBuffer("bomb_sound"));
    }
    if (rm.loadSoundBuffer("recolor_sound", "recolor_activation.wav")) {
        recolorSound.setBuffer(rm.getSoundBuffer("recolor_sound"));
    }

    // �������� ������
    if (!rm.loadFont("default_font", "arial.ttf")) {
        std::cerr << "�� ������� ��������� ����� arial.ttf\n";
    }

    // ��������� ������ �����
    scoreText.setFont(rm.getFont("default_font"));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10.0f, ROWS * TILE_SIZE + 10.0f);
    scoreText.setString("Score: 0");

    initializeGrid();
}

void Game::initializeGrid() {
    grid.resize(ROWS, std::vector<Tile>(COLS));

    // ���������� ���� ���������� �������
    std::uniform_int_distribution<int> dist(0, NUM_COLORS - 1);
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            grid[row][col].color = dist(rng);
            grid[row][col].shape.setFillColor(colors[grid[row][col].color]);
            grid[row][col].shape.setPosition(static_cast<float>(col * TILE_SIZE + 1), static_cast<float>(row * TILE_SIZE + 1));
            grid[row][col].bonus = BonusType::None; // ��������, ��� ������ �� �����������
            grid[row][col].isSelected = false;
            grid[row][col].resetFuse();

            // ����� ������ ����������
            grid[row][col].recolorIcon.setPosition(-100, -100);

            // ����� highlight
            grid[row][col].highlight.setOutlineColor(sf::Color::Transparent);

            // ��������� ��������
            grid[row][col].animationOverlay.setFillColor(sf::Color::Transparent);
        }
    }

    // ������� ��������� ���������� ��� ��������� �������
    std::vector<std::pair<int, int>> initialMatches;
    int matchChecks = 0;
    while (checkMatches(initialMatches) && matchChecks < MAX_MATCH_CHECKS) {
        removeMatches(initialMatches, false); // ������� false, ����� ������ �� ��������������
        applyGravity();
        initialMatches.clear(); // ������� ������ ����� ����� ���������
        matchChecks++;
    }
}

void Game::run() {
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i clickPos(event.mouseButton.x, event.mouseButton.y);
            float elapsed = clickClock.getElapsedTime().asSeconds();
            bool isDoubleClick = false;

            // ���������, �������� �� ������� ���� �������
            if (elapsed < DOUBLE_CLICK_TIME) {
                float distance = std::sqrt(std::pow(static_cast<float>(clickPos.x - lastClickPos.x), 2) + std::pow(static_cast<float>(clickPos.y - lastClickPos.y), 2));
                if (distance < DOUBLE_CLICK_DISTANCE) {
                    isDoubleClick = true;
                }
            }

            // ��������� ����� � ������� ���������� �����
            lastClickTime = clickClock.restart();
            lastClickPos = clickPos;

            int col = clickPos.x / TILE_SIZE;
            int row = clickPos.y / TILE_SIZE;
            if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
                if (isDoubleClick && grid[row][col].bonus != BonusType::None) {
                    // ������� ���� �� ������ - ���������� �����
                    applyBonus(grid[row][col].bonus, row, col);
                    std::cout << "Bonus activated at (" << row << ", " << col << ")\n";

                    // ����� ������ ����� ��������� ������
                    if (firstSelected) {
                        grid[selectedRow][selectedCol].isSelected = false;
                        firstSelected = false;
                    }

                    // ��������� ���������� ����� ��������� ������
                    applyGravity();
                    // ��������� �� ����� ���������� ����� ���������� ������
                    std::vector<std::pair<int, int>> newMatches;
                    int matchChecks = 0;
                    while (checkMatches(newMatches) && matchChecks < MAX_MATCH_CHECKS) {
                        removeMatches(newMatches);
                        applyGravity();
                        newMatches.clear();
                        matchChecks++;
                    }
                }
                else {
                    if (!firstSelected) {
                        firstSelected = true;
                        selectedRow = row;
                        selectedCol = col;
                        grid[row][col].isSelected = true; // ���������
                    }
                    else {
                        // �������� �� ���������
                        if (isAdjacent(selectedRow, selectedCol, row, col)) {
                            swapTiles(selectedRow, selectedCol, row, col);
                            // �������� �� ����������
                            std::vector<std::pair<int, int>> matches;
                            if (!checkMatches(matches)) {
                                // ���� ��� ����������, ������� �������
                                swapTiles(selectedRow, selectedCol, row, col);
                            }
                            else {
                                // ������� ���������� � ��������� ����������
                                removeMatches(matches);
                                applyGravity();
                                // ��������� �������� �� ����� ����������
                                int matchChecks = 0;
                                while (checkMatches(matches) && matchChecks < MAX_MATCH_CHECKS) {
                                    removeMatches(matches);
                                    applyGravity();
                                    matches.clear();
                                    matchChecks++;
                                }
                            }
                        }
                        // ����� ������
                        grid[selectedRow][selectedCol].isSelected = false;
                        firstSelected = false;
                    }
                }
            }
        }
    }
}

void Game::update() {
    // ���������� ������� �����
    float deltaTime = clock.restart().asSeconds();
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            grid[row][col].updateFuse(deltaTime);
        }
    }

    // ���������� ��������
    for (auto it = activeAnimations.begin(); it != activeAnimations.end();) {
        if (!it->active) {
            it = activeAnimations.erase(it);
            continue;
        }

        float elapsed = it->clock.getElapsedTime().asSeconds();
        if (elapsed > it->duration) {
            it = activeAnimations.erase(it);
            continue;
        }

        // ������: ��������� ������������ overlay
        float alpha = 255 * (1.0f - (elapsed / it->duration));
        sf::Color overlayColor = it->shape.getFillColor();
        overlayColor.a = static_cast<sf::Uint8>(alpha);
        it->shape.setFillColor(overlayColor);

        ++it;
    }
}

void Game::render() {
    window.clear(sf::Color::Black);
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            window.draw(grid[row][col].shape);

            // ��������� ������ ����������, ���� ����
            if (grid[row][col].bonus == BonusType::Recolor) {
                window.draw(grid[row][col].recolorIcon);
                // ��������, ��� ������� highlight ��������� � �������� ��������
                grid[row][col].highlight.setPosition(grid[row][col].shape.getPosition());
                window.draw(grid[row][col].highlight);
            }

            // ��������� ������ �����
            if (grid[row][col].bonus == BonusType::Bomb) {
                window.draw(grid[row][col].fuse);
                // ��������, ��� ������� highlight ��������� � �������� ��������
                grid[row][col].highlight.setPosition(grid[row][col].shape.getPosition());
                window.draw(grid[row][col].highlight);
            }

            // ��������� ���������� ����������
            if (grid[row][col].isSelected) {
                sf::RectangleShape highlightShape;
                highlightShape.setSize(sf::Vector2f(static_cast<float>(TILE_SIZE - 2), static_cast<float>(TILE_SIZE - 2)));
                highlightShape.setFillColor(sf::Color::Transparent);
                highlightShape.setOutlineThickness(3.0f);
                highlightShape.setOutlineColor(sf::Color::White);
                highlightShape.setPosition(grid[row][col].shape.getPosition());
                window.draw(highlightShape);
            }

            // ��������� �������� overlay, ���� ����
            if (grid[row][col].isAnimating) {
                window.draw(grid[row][col].animationOverlay);
            }
        }
    }

    // ��������� �������� ��������
    for (const auto& anim : activeAnimations) {
        window.draw(anim.shape);
    }

    // ����������� �����
    window.draw(scoreText);

    window.display();
}

bool Game::isAdjacent(int row1, int col1, int row2, int col2) {
    return (abs(row1 - row2) + abs(col1 - col2)) == 1;
}

bool Game::swapTiles(int firstRow, int firstCol, int secondRow, int secondCol) {
    std::swap(grid[firstRow][firstCol].color, grid[secondRow][secondCol].color);
    std::swap(grid[firstRow][firstCol].bonus, grid[secondRow][secondCol].bonus);

    // ��������� �����
    grid[firstRow][firstCol].shape.setFillColor(colors[grid[firstRow][firstCol].color]);
    grid[secondRow][secondCol].shape.setFillColor(colors[grid[secondRow][secondCol].color]);

    // ����� ������� ��� ������
    grid[firstRow][firstCol].resetFuse();
    grid[secondRow][secondCol].resetFuse();

    // ����� ������ ����������
    grid[firstRow][firstCol].recolorIcon.setPosition(-100, -100);
    grid[secondRow][secondCol].recolorIcon.setPosition(-100, -100);

    // ����� highlight
    grid[firstRow][firstCol].highlight.setOutlineColor(sf::Color::Transparent);
    grid[secondRow][secondCol].highlight.setOutlineColor(sf::Color::Transparent);

    return true;
}

// ������� checkMatches ��� ���������
bool Game::checkMatches(std::vector<std::pair<int, int>>& matchesOut) {
    bool found = false;
    std::vector<std::vector<bool>> visited(ROWS, std::vector<bool>(COLS, false));

    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (visited[row][col] || grid[row][col].color == -1)
                continue;
            int currentColor = grid[row][col].color;
            std::vector<std::pair<int, int>> cluster;
            // �������� DFS � ���� ������
            std::stack<std::pair<int, int>> stack;
            stack.push({ row, col });
            visited[row][col] = true;
            cluster.push_back({ row, col });

            while (!stack.empty()) {
                auto [r, c] = stack.top();
                stack.pop();

                // ��������� ������� (�����, ����, �����, ������)
                std::vector<std::pair<int, int>> neighbors = {
                    {r - 1, c},
                    {r + 1, c},
                    {r, c - 1},
                    {r, c + 1}
                };
                for (auto [nr, nc] : neighbors) {
                    if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {
                        if (!visited[nr][nc] && grid[nr][nc].color == currentColor) {
                            visited[nr][nc] = true;
                            stack.push({ nr, nc });
                            cluster.push_back({ nr, nc });
                        }
                    }
                }
            }
            // ����� ����� ��������, ��������� ��� ������
            if (cluster.size() >= 3) {
                matchesOut.insert(matchesOut.end(), cluster.begin(), cluster.end());
                found = true;
            }
        }
    }

    // ������� ���������
    if (found) {
        std::sort(matchesOut.begin(), matchesOut.end());
        matchesOut.erase(std::unique(matchesOut.begin(), matchesOut.end()), matchesOut.end());
    }

    return found;
}

void Game::removeMatches(const std::vector<std::pair<int, int>>& matches, bool generateBonuses) {
    if (matches.empty()) return;

    std::uniform_real_distribution<float> realDist(0.0f, 1.0f);
    std::uniform_int_distribution<int> bonusDist(1, 2);
    std::uniform_int_distribution<int> offsetDist(-3, 3);

    ResourceManager& rm = ResourceManager::getInstance();

    for (auto& [row, col] : matches) {
        // �������� ��������� ��� ��������
        grid[row][col].color = -1;
        grid[row][col].shape.setFillColor(sf::Color::Black);
        grid[row][col].resetFuse();

        // ���������� �����
        score += 10;

        // ������, ����� �� �������� �����
        if (generateBonuses && realDist(rng) < BONUS_CHANCE) {
            // ������� ��������� ����� ��������� ����������
            bool bonusAssigned = false;
            for (int attempts = 0; attempts < 20 && !bonusAssigned; ++attempts) {
                int rowOffset = offsetDist(rng);
                int colOffset = offsetDist(rng);
                int bonusRow = row + rowOffset;
                int bonusCol = col + colOffset;
                if (bonusRow >= 0 && bonusRow < ROWS && bonusCol >= 0 && bonusCol < COLS) {
                    // ����������, ��� ������ �� ������������ � �� �������� �����
                    bool isBeingDestroyed = std::find(matches.begin(), matches.end(), std::make_pair(bonusRow, bonusCol)) != matches.end();
                    if (!isBeingDestroyed && grid[bonusRow][bonusCol].bonus == BonusType::None && grid[bonusRow][bonusCol].color != -1) {
                        // ��������� �����
                        grid[bonusRow][bonusCol].bonus = static_cast<BonusType>(bonusDist(rng));
                        std::cout << "Bonus spawned at (" << bonusRow << ", " << bonusCol << ") Type: "
                            << (grid[bonusRow][bonusCol].bonus == BonusType::Recolor ? "Recolor" : "Bomb") << "\n";
                        bonusAssigned = true;

                        // ��������� highlight ��� ������
                        if (grid[bonusRow][bonusCol].bonus == BonusType::Recolor) {
                            grid[bonusRow][bonusCol].highlight.setOutlineColor(sf::Color::Cyan);

                            // ��������� ������ ����������, ���� �������� ���������
                            if (rm.getTexture("recolor_icon").getSize().x > 0) {
                                grid[bonusRow][bonusCol].recolorIcon.setTexture(rm.getTexture("recolor_icon"));
                                grid[bonusRow][bonusCol].recolorIcon.setPosition(
                                    grid[bonusRow][bonusCol].shape.getPosition().x + grid[bonusRow][bonusCol].shape.getSize().x / 2 - grid[bonusRow][bonusCol].recolorIcon.getGlobalBounds().width / 2,
                                    grid[bonusRow][bonusCol].shape.getPosition().y + grid[bonusRow][bonusCol].shape.getSize().y / 2 - grid[bonusRow][bonusCol].recolorIcon.getGlobalBounds().height / 2
                                );
                            }
                        }
                        else if (grid[bonusRow][bonusCol].bonus == BonusType::Bomb) {
                            grid[bonusRow][bonusCol].highlight.setOutlineColor(sf::Color::Red);
                        }
                    }
                }
            }
        }
    }

    // ���������� ������ �����
    scoreText.setString("Score: " + std::to_string(score));
}

void Game::applyGravity() {
    for (int col = 0; col < COLS; ++col) {
        int empty = ROWS - 1;
        for (int row = ROWS - 1; row >= 0; --row) {
            if (grid[row][col].color != -1) {
                if (row != empty) {
                    // ����������� ���������� ����
                    grid[empty][col].color = grid[row][col].color;
                    grid[empty][col].bonus = grid[row][col].bonus;
                    grid[empty][col].shape.setFillColor(colors[grid[empty][col].color]);

                    // ����������� ������, ���� ����
                    grid[empty][col].fuse.setPosition(grid[row][col].fuse.getPosition());

                    // ����������� ������ ����������
                    grid[empty][col].recolorIcon.setPosition(grid[row][col].recolorIcon.getPosition());

                    // ����������� highlight
                    grid[empty][col].highlight.setOutlineColor(grid[row][col].highlight.getOutlineColor());

                    // ����������� �������� overlay
                    grid[empty][col].animationOverlay.setFillColor(grid[row][col].animationOverlay.getFillColor());
                    grid[empty][col].animationOverlay.setPosition(grid[row][col].animationOverlay.getPosition());

                    // ����� ��������� ����������
                    grid[row][col].color = -1;
                    grid[row][col].bonus = BonusType::None;
                    grid[row][col].shape.setFillColor(sf::Color::Black);
                    grid[row][col].fuse.setPosition(-10, -10);
                    grid[row][col].isSelected = false;

                    // ����� ������ ����������
                    grid[row][col].recolorIcon.setPosition(-100, -100);

                    // ����� highlight
                    grid[row][col].highlight.setOutlineColor(sf::Color::Transparent);

                    // ����� �������� overlay
                    grid[row][col].animationOverlay.setFillColor(sf::Color::Transparent);
                }
                empty--;
            }
        }
        // ���������� ������� ������
        std::uniform_int_distribution<int> dist(0, NUM_COLORS - 1);

        for (int row = empty; row >= 0; --row) {
            grid[row][col].color = dist(rng);
            grid[row][col].bonus = BonusType::None; // ������ �� ������������ �����

            grid[row][col].shape.setFillColor(colors[grid[row][col].color]);
            grid[row][col].fuse.setPosition(-10, -10);
            grid[row][col].isSelected = false;

            // ����� ������ ����������
            grid[row][col].recolorIcon.setPosition(-100, -100);

            // ����� highlight (����� ��� �������)
            grid[row][col].highlight.setOutlineColor(sf::Color::Transparent);

            // ����� ��������
            grid[row][col].animationOverlay.setFillColor(sf::Color::Transparent);
        }
    }
}

void Game::applyBonus(BonusType bonus, int row, int col) {
    ResourceManager& rm = ResourceManager::getInstance();

    if (bonus == BonusType::Recolor) {
        // ��������������� ����� Recolor
        recolorSound.play();

        std::cout << "Applying Recolor bonus at (" << row << ", " << col << ")\n";

        // ������ �������� ����������
        Animation anim;
        anim.shape.setSize(sf::Vector2f(static_cast<float>(TILE_SIZE - 2), static_cast<float>(TILE_SIZE - 2)));
        anim.shape.setFillColor(sf::Color::White);
        anim.shape.setPosition(grid[row][col].shape.getPosition());
        anim.duration = 0.5f;
        anim.active = true;
        anim.clock.restart();
        activeAnimations.push_back(anim);

        // ������������� ������� � 2 ��������� � ������� 3
        std::vector<std::pair<int, int>> vicinity;
        for (int r = std::max(0, row - 3); r <= std::min(ROWS - 1, row + 3); ++r) {
            for (int c = std::max(0, col - 3); c <= std::min(COLS - 1, col + 3); ++c) {
                if (r == row && c == col) continue;
                vicinity.emplace_back(r, c);
            }
        }
        // �������� 2 ���������
        std::shuffle(vicinity.begin(), vicinity.end(), rng);
        int recolorColor = grid[row][col].color;
        grid[row][col].shape.setFillColor(colors[recolorColor]); // ���� ��� ����������
        if (vicinity.size() >= 2) {
            grid[vicinity[0].first][vicinity[0].second].color = recolorColor;
            grid[vicinity[0].first][vicinity[0].second].shape.setFillColor(colors[recolorColor]);
            grid[vicinity[1].first][vicinity[1].second].color = recolorColor;
            grid[vicinity[1].first][vicinity[1].second].shape.setFillColor(colors[recolorColor]);
            // ���������� ����� �� ��������������
            score += 10;
            scoreText.setString("Score: " + std::to_string(score));

            // ����� ������� �� ������������� ���������
            grid[vicinity[0].first][vicinity[0].second].bonus = BonusType::None;
            grid[vicinity[1].first][vicinity[1].second].bonus = BonusType::None;

            // ����� ������ ���������� � highlight
            grid[vicinity[0].first][vicinity[0].second].recolorIcon.setPosition(-100, -100);
            grid[vicinity[0].first][vicinity[0].second].highlight.setOutlineColor(sf::Color::Transparent);

            grid[vicinity[1].first][vicinity[1].second].recolorIcon.setPosition(-100, -100);
            grid[vicinity[1].first][vicinity[1].second].highlight.setOutlineColor(sf::Color::Transparent);
        }

        // ����� ������ Recolor ����� ���������
        grid[row][col].bonus = BonusType::None;
        grid[row][col].recolorIcon.setPosition(-100, -100); // ���������� ������ �� ������� ������
        grid[row][col].highlight.setOutlineColor(sf::Color::Transparent); // ���������� ���������
        std::cout << "Recolor bonus at (" << row << ", " << col << ") has been reset.\n";
    }
    else if (bonus == BonusType::Bomb) {
        // ��������������� ����� Bomb
        bombSound.play();

        std::cout << "Applying Bomb bonus at (" << row << ", " << col << ")\n";

        // ������ �������� ������
        Animation anim;
        anim.shape.setSize(sf::Vector2f(static_cast<float>(TILE_SIZE - 2), static_cast<float>(TILE_SIZE - 2)));
        anim.shape.setFillColor(sf::Color::Red);
        anim.shape.setPosition(grid[row][col].shape.getPosition());
        anim.duration = 0.5f;
        anim.active = true;
        anim.clock.restart();
        activeAnimations.push_back(anim);

        // ���������� 5 ��������� ���������, ������� �������
        std::vector<std::pair<int, int>> tilesToDestroy = { {row, col} };
        std::vector<std::pair<int, int>> candidates;
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                if (r != row || c != col) {
                    candidates.emplace_back(r, c);
                }
            }
        }
        std::shuffle(candidates.begin(), candidates.end(), rng);
        for (int i = 0; i < 4 && i < static_cast<int>(candidates.size()); ++i) {
            tilesToDestroy.emplace_back(candidates[i]);
        }
        for (auto& [r, c] : tilesToDestroy) {
            grid[r][c].color = -1;
            grid[r][c].bonus = BonusType::None;

            // �������� ������ �����, ���� �� ���
            grid[r][c].fuse.setPosition(-10, -10);

            // ���������� ����� �� ������������ ����������
            score += 20;
            scoreText.setString("Score: " + std::to_string(score));

            // ����� ������ ���������� � highlight
            grid[r][c].recolorIcon.setPosition(-100, -100);
            grid[r][c].highlight.setOutlineColor(sf::Color::Transparent);
        }

        // ����� ������ Bomb ����� ���������
        grid[row][col].bonus = BonusType::None;
        grid[row][col].fuse.setPosition(-10, -10); // �������� ������
        grid[row][col].highlight.setOutlineColor(sf::Color::Transparent); // ���������� ���������
        std::cout << "Bomb bonus at (" << row << ", " << col << ") has been reset.\n";

        // ��������� ���������� ����� ��������� �����
        applyGravity();

        // ��������� �� ����� ���������� ����� ���������� �����
        std::vector<std::pair<int, int>> newMatches;
        int matchChecks = 0;
        while (checkMatches(newMatches) && matchChecks < MAX_MATCH_CHECKS) {
            removeMatches(newMatches);
            applyGravity();
            newMatches.clear();
            matchChecks++;
        }
    }
}

void Game::activateBomb(int row, int col) {
    if (grid[row][col].bonus != BonusType::Bomb) {
        return;
    }
    applyBonus(BonusType::Bomb, row, col);
}
