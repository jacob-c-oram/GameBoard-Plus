#include <SFML/Graphics.hpp>
#include <cctype>
#include <fstream>
#include "Tile.h"
#include <random>
#include <vector>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <utility>
#include <iomanip>
using namespace std;

void placeMines(vector<vector<Tile>>& tiles, int rowCount, int colCount, int mineCount, sf::Texture& mineTexture) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> disRow(0, rowCount - 1);
    uniform_int_distribution<> disCol(0, colCount - 1);

    int minesPlaced = 0;
    while (minesPlaced < mineCount) {
        int row = disRow(gen);
        int col = disCol(gen);

        if (!tiles[row][col].hasMine) {
            tiles[row][col].hasMine = true;
            minesPlaced++;
            for (int i = max(0, row - 1); i <= min(row + 1, rowCount - 1); i++) {
                for (int j = max(0, col - 1); j <= min(col + 1, colCount - 1); j++) {
                    if (!(i == row && j == col)) {
                        tiles[i][j].adjacentMines++;
                    }
                }
            }
        }
    }
}

void updateLeaderboard(const string& playerTime, const string& playerName, vector<pair<string, string>>& entries, bool isWin);


void resetGame(vector<vector<Tile>>& tiles, int rowCount, int colCount, sf::Texture& hiddenTexture, sf::Texture& mineTexture, int mineCount, vector<pair<string, string>>& leaderboardEntries) {
    for (auto& row : tiles) {
        for (auto& tile : row) {
            tile.isRevealed = false;
            tile.isFlagged = false;
            tile.hasMine = false;
            tile.adjacentMines = 0;
            tile.baseSprite.setTexture(hiddenTexture);
            tile.overlaySprite.setColor(sf::Color(255, 255, 255, 0));
        }
    }
    placeMines(tiles, rowCount, colCount, mineCount, mineTexture);
    updateLeaderboard("", "", leaderboardEntries, false);
}

void revealTile(vector<vector<Tile>>& tiles, int row, int col, int rowCount, int colCount, const sf::Texture& revealedTexture, const vector<sf::Texture>& numberTextures, const sf::Texture& mineTexture) {
    if (row < 0 || row >= rowCount || col < 0 || col >= colCount || tiles[row][col].isRevealed || tiles[row][col].isFlagged) {
        return;
    }

    tiles[row][col].isRevealed = true;
    tiles[row][col].baseSprite.setTexture(revealedTexture);

    if (tiles[row][col].hasMine) {
        tiles[row][col].overlaySprite.setTexture(mineTexture);
        tiles[row][col].overlaySprite.setColor(sf::Color(255, 255, 255, 255));
    } else {
        if (tiles[row][col].adjacentMines > 0) {
            tiles[row][col].overlaySprite.setTexture(numberTextures[tiles[row][col].adjacentMines - 1]);
            tiles[row][col].overlaySprite.setColor(sf::Color(255, 255, 255, 255));
        } else {
            tiles[row][col].overlaySprite.setColor(sf::Color(255, 255, 255, 0));
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx != 0 || dy != 0) {
                        revealTile(tiles, row + dy, col + dx, rowCount, colCount, revealedTexture, numberTextures, mineTexture);
                    }
                }
            }
        }
    }
}

void drawCounter(sf::RenderWindow& window, int flagCounter, const sf::Texture& digitsTexture, int rowCount, int colWidth) {
    sf::Sprite digitSprite;
    digitSprite.setTexture(digitsTexture);

    string counterStr = to_string(abs(flagCounter));
    if (counterStr.length() < 3) {
        counterStr = string(3 - counterStr.length(), '0') + counterStr;
    }

    int baseXOffset = 33;
    int yOffset = static_cast<int>(32 * (rowCount + 0.5) + 16);

    int xOffset = (flagCounter < 0) ? 12 : baseXOffset;

    if (flagCounter < 0) {
        digitSprite.setTextureRect(sf::IntRect(10 * 21, 0, 21, 32));
        digitSprite.setPosition(xOffset, yOffset);
        window.draw(digitSprite);
        xOffset += 21;
    }

    for (char ch : counterStr) {
        if (isdigit(ch)) {
            int index = ch - '0';
            digitSprite.setTextureRect(sf::IntRect(index * 21, 0, 21, 32));
            digitSprite.setPosition(xOffset, yOffset);
            window.draw(digitSprite);
            xOffset += 21;
        }
    }
}

void drawTimer(sf::RenderWindow& window, const chrono::steady_clock::time_point& startTime, const chrono::steady_clock::time_point& currentTime, const sf::Texture& digitsTexture, int rowCount, int colCount) {
    using namespace chrono;
    auto elapsed = duration_cast<seconds>(currentTime - startTime);
    int minutes = (elapsed.count() / 60) % 60;
    int seconds = elapsed.count() % 60;

    sf::Sprite digitSprite;
    digitSprite.setTexture(digitsTexture);

    string minutesStr = (minutes < 10 ? "0" : "") + to_string(minutes);
    int xOffset = (colCount * 32) - 97;
    int yOffset = static_cast<int>(32 * (rowCount + 0.5) + 16);
    for (char ch : minutesStr) {
        int index = ch - '0';
        digitSprite.setTextureRect(sf::IntRect(index * 21, 0, 21, 32));
        digitSprite.setPosition(xOffset, yOffset);
        window.draw(digitSprite);
        xOffset += 21;
    }

    string secondsStr = (seconds < 10 ? "0" : "") + to_string(seconds);
    xOffset = (colCount * 32) - 54;
    for (char ch : secondsStr) {
        int index = ch - '0';
        digitSprite.setTextureRect(sf::IntRect(index * 21, 0, 21, 32));
        digitSprite.setPosition(xOffset, yOffset);
        window.draw(digitSprite);
        xOffset += 21;
    }
}

bool checkWin(const vector<vector<Tile>>& tiles) {
    for (const auto& row : tiles) {
        for (const auto& tile : row) {
            if (!tile.isRevealed && !tile.hasMine) {
                return false;
            }
        }
    }
    return true;
}

void flagAllMines(vector<vector<Tile>>& tiles, const sf::Texture& flagTexture) {
    for (auto& row : tiles) {
        for (auto& tile : row) {
            if (tile.hasMine && !tile.isFlagged) {
                tile.isFlagged = true;
                tile.overlaySprite.setTexture(flagTexture);
                tile.overlaySprite.setColor(sf::Color(255, 255, 255, 255));
            }
        }
    }
}

bool timeCompare(const pair<string, string>& a, const pair<string, string>& b) {
    int hourA, minuteA, hourB, minuteB;
    char colon;

    stringstream ssA(a.first);
    ssA >> hourA >> colon >> minuteA;

    stringstream ssB(b.first);
    ssB >> hourB >> colon >> minuteB;

    return hourA == hourB ? minuteA < minuteB : hourA < hourB;
}

bool isBetterTime(const string& newTime, const string& existingTime) {
    int newMin = stoi(newTime.substr(0, 2));
    int newSec = stoi(newTime.substr(3, 2));
    int existMin = stoi(existingTime.substr(0, 2));
    int existSec = stoi(existingTime.substr(3, 2));

    if (newMin < existMin) return true;
    if (newMin == existMin && newSec < existSec) return true;
    return false;
}

vector<pair<string, string>> readLeaderboard() {
    ifstream file("../files/leaderboard.txt");
    vector<pair<string, string>> entries;
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string time, name;
        if (getline(iss, time, ',') && getline(iss, name)) {
            entries.push_back({time, name});
        }
    }
    return entries;
}

void writeLeaderboard(const vector<pair<string, string>>& entries) {
    ofstream file("../files/leaderboard.txt", ios::trunc);
    for (const auto& entry : entries) {
        string name = entry.second;
        if (!name.empty() && name.back() == '*') {
            name.pop_back();
        }
        file << entry.first << ',' << name << '\n';
    }
}


void updateLeaderboard(const string& playerTime, const string& playerName, vector<pair<string, string>>& entries, bool isWin) {
    if (!isWin) {
        for (auto &entry : entries) {
            if (entry.second.back() == '*') {
                entry.second.pop_back();
            }
        }
    } else {
        for (auto &entry : entries) {
            if (!entry.second.empty() && entry.second.back() == '*') {
                entry.second.pop_back();
            }
        }

        bool updated = false;
        for (auto it = entries.begin(); it != entries.end(); ++it) {
            if (isBetterTime(playerTime, it->first)) {
                entries.insert(it, {playerTime, playerName + "*"});
                updated = true;
                break;
            }
        }
        if (!updated && entries.size() < 5) {
            entries.emplace_back(playerTime, playerName + "*");
        }

        if (entries.size() > 5) {
            entries.resize(5);
        }
    }

    sort(entries.begin(), entries.end(), [](const pair<string, string>& a, const pair<string, string>& b) {
        return isBetterTime(a.first, b.first);
    });
    writeLeaderboard(entries);
}


string formatTime(int totalSeconds) {
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    ostringstream ss;
    ss << setw(2) << setfill('0') << minutes
       << ":" << setw(2) << setfill('0') << seconds;
    return ss.str();
}

void redrawGameWindow(const vector<vector<Tile>>& tiles, sf::RenderWindow& gameWindow,
                      sf::Sprite& faceButton, sf::Sprite& debugButton, sf::Sprite& pausePlayButton, sf::Sprite& leaderboardButton,
                      int flagCounter, const sf::Texture& digitsTexture, int rowCount, int colCount, const chrono::steady_clock::time_point& startTime) {
    gameWindow.clear(sf::Color::White);

    const sf::Texture* revealedTexturePtr = tiles[0][0].baseSprite.getTexture();

    for (const auto& row : tiles) {
        for (const auto& tile : row) {
            gameWindow.draw(tile.baseSprite);
            if (tile.baseSprite.getTexture() != revealedTexturePtr && tile.isRevealed && tile.adjacentMines > 0) {
                gameWindow.draw(tile.overlaySprite);
            }
        }
    }

    gameWindow.draw(faceButton);
    gameWindow.draw(debugButton);
    gameWindow.draw(pausePlayButton);
    gameWindow.draw(leaderboardButton);
    drawCounter(gameWindow, flagCounter, digitsTexture, rowCount, colCount);
    drawTimer(gameWindow, startTime, chrono::steady_clock::now(), digitsTexture, rowCount, colCount);

    gameWindow.display();
}



void revealAllTilesForLeaderboard(vector<vector<Tile>>& tiles, const sf::Texture& revealedTexture, sf::RenderWindow& gameWindow,
                                  sf::Sprite& faceButton, sf::Sprite& debugButton, sf::Sprite& pausePlayButton,
                                  sf::Sprite& leaderboardButton, int flagCounter, const sf::Texture& digitsTexture,
                                  int rowCount, int colCount, const chrono::steady_clock::time_point& startTime, bool gameOver, bool gameWon) {
    if (!gameOver || gameWon) {
        for (auto& row : tiles) {
            for (auto& tile : row) {
                if (!tile.isRevealed) {
                    tile.baseSprite.setTexture(revealedTexture);
                }
            }
        }
    } else {
        for (auto& row : tiles) {
            for (auto& tile : row) {
                if (tile.isFlagged || tile.hasMine) {
                    tile.baseSprite.setTexture(*tile.baseSprite.getTexture());
                    tile.overlaySprite.setColor(sf::Color(255, 255, 255, 255));
                }
            }
        }
    }

    redrawGameWindow(tiles, gameWindow, faceButton, debugButton, pausePlayButton, leaderboardButton,
                     flagCounter, digitsTexture, rowCount, colCount, startTime);
}
void restoreTilesAfterLeaderboard(vector<vector<Tile>>& tiles, const sf::Texture& hiddenTexture, const sf::Texture& revealedTexture,
                                  sf::RenderWindow& gameWindow, sf::Sprite& faceButton, sf::Sprite& debugButton,
                                  sf::Sprite& pausePlayButton, sf::Sprite& leaderboardButton, int flagCounter,
                                  const sf::Texture& digitsTexture, int rowCount, int colCount,
                                  const chrono::steady_clock::time_point& startTime, bool gameOver, bool gameWon) {
    for (auto &row: tiles) {
        for (auto &tile: row) {
            if (tile.isRevealed) {
                tile.baseSprite.setTexture(revealedTexture);
            } else {
                tile.baseSprite.setTexture(hiddenTexture);
            }
        }
    }
    redrawGameWindow(tiles, gameWindow, faceButton, debugButton, pausePlayButton, leaderboardButton,
                     flagCounter, digitsTexture, rowCount, colCount, startTime);
}

