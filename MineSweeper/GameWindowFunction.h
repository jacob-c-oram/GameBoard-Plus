#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cctype>
#include <fstream>
#include <random>
#include <vector>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <utility>
#include <iomanip>
#include "Functions.h"
using namespace std;

void openGameWindow(int rowCount, int colCount, int mineCount, const string& playerName) {
    sf::RenderWindow gameWindow(sf::VideoMode(colCount * 32, rowCount * 32 + 100), "Minesweeper Game", sf::Style::Close);
    gameWindow.setFramerateLimit(30);

    sf::Texture hiddenTexture, flagTexture, revealedTexture, mineTexture, happyFaceTexture, loseFaceTexture, winFaceTexture, debugTexture, digitsTexture, pauseTexture, playTexture, leaderboardTexture;
    vector<sf::Texture> numberTextures(8);

    if (!hiddenTexture.loadFromFile("../files/images/tile_hidden.png") ||
        !flagTexture.loadFromFile("../files/images/flag.png") ||
        !revealedTexture.loadFromFile("../files/images/tile_revealed.png") ||
        !mineTexture.loadFromFile("../files/images/mine.png") ||
        !happyFaceTexture.loadFromFile("../files/images/face_happy.png") ||
        !loseFaceTexture.loadFromFile("../files/images/face_lose.png") ||
        !winFaceTexture.loadFromFile("../files/images/face_win.png") ||
        !debugTexture.loadFromFile("../files/images/debug.png") ||
        !digitsTexture.loadFromFile("../files/images/digits.png") ||
        !leaderboardTexture.loadFromFile("../files/images/leaderboard.png") ||
        !pauseTexture.loadFromFile("../files/images/pause.png") ||
        !playTexture.loadFromFile("../files/images/play.png")) {
        cout << "texture error" << endl;
        return;
    }

    for (int i = 0; i < 8; ++i) {
        if (!numberTextures[i].loadFromFile("../files/images/number_" + to_string(i+1) + ".png")) {
            cout << "numbers image error " << i+1 << endl;
            return;
        }
    }

    vector<vector<Tile>> tiles(rowCount, vector<Tile>(colCount));
    for (int i = 0; i < rowCount; i++) {
        for (int j = 0; j < colCount; j++) {
            tiles[i][j].setBaseTexture(hiddenTexture);
            tiles[i][j].setOverlayTexture(flagTexture);
            tiles[i][j].setPosition(j * 32, i * 32);
        }
    }

    chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point endTime;
    bool timerStopped = false;

    int flagCounter = mineCount;
    placeMines(tiles, rowCount, colCount, mineCount, mineTexture);

    sf::Sprite faceButton, debugButton, pausePlayButton, leaderboardButton;
    faceButton.setTexture(happyFaceTexture);
    faceButton.setPosition((colCount * 16) - 16, rowCount * 32 + 16);
    debugButton.setTexture(debugTexture);
    debugButton.setPosition((colCount * 32) - 304, rowCount * 32 + 16);
    pausePlayButton.setTexture(pauseTexture);
    pausePlayButton.setPosition((colCount * 32) - 240, rowCount * 32 + 16);
    leaderboardButton.setTexture(leaderboardTexture);
    leaderboardButton.setPosition((colCount * 32) - 176, 32 * (rowCount + 0.5));
    bool debugMode = false;
    bool gameWon = false;
    bool gameOver = false;
    bool isPaused = false;


    vector<pair<string, string>> leaderboardEntries;
    ifstream leaderboardFile("../files/leaderboard.txt");
    string line;

    while (getline(leaderboardFile, line)) {
        istringstream stream(line);
        string time, name;
        getline(stream, time, ',');
        getline(stream, name);
        leaderboardEntries.emplace_back(time, name);
    }
    leaderboardFile.close();

    sort(leaderboardEntries.begin(), leaderboardEntries.end(), timeCompare);

    bool leaderboardWindowOpen = false;

    while (gameWindow.isOpen()) {
        leaderboardWindowOpen = false;
        sf::Event event;
        while (gameWindow.pollEvent(event)) {
            if (!leaderboardWindowOpen){
                if (event.type == sf::Event::Closed) {
                    gameWindow.close();
                } else if (event.type == sf::Event::MouseButtonPressed) {

                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    if (faceButton.getGlobalBounds().contains(mouseX, mouseY)) {
                        resetGame(tiles, rowCount, colCount, hiddenTexture, mineTexture, mineCount, leaderboardEntries);
                        faceButton.setTexture(happyFaceTexture);
                        debugMode = false;
                        gameWon = false;
                        gameOver = false;
                        timerStopped = false;
                        flagCounter = mineCount;
                        isPaused = false;
                        pausePlayButton.setTexture(pauseTexture);
                        startTime = chrono::steady_clock::now();
                        endTime = startTime;
                    } else if (pausePlayButton.getGlobalBounds().contains(mouseX, mouseY) && !gameOver) {
                        isPaused = !isPaused;
                        pausePlayButton.setTexture(isPaused ? playTexture : pauseTexture);
                        if (isPaused) {
                            timerStopped = true;
                            endTime = chrono::steady_clock::now();
                        } else {
                            timerStopped = false;
                            startTime = chrono::steady_clock::now() - (endTime - startTime);
                        }
                    } else if (leaderboardButton.getGlobalBounds().contains(mouseX, mouseY)) {
                        bool wasPausedBeforeLeaderboard = isPaused;
                        chrono::steady_clock::time_point pauseStartTime = chrono::steady_clock::now();
                        isPaused = true;

                        if (!gameWon && !gameOver) {
                            revealAllTilesForLeaderboard(tiles, revealedTexture, gameWindow, faceButton, debugButton,
                                                         pausePlayButton,
                                                         leaderboardButton, flagCounter, digitsTexture, rowCount, colCount,
                                                         startTime, gameOver, gameWon);
                        }
                        sf::RenderWindow leaderboardWindow(sf::VideoMode(colCount * 16, (rowCount * 16) + 50),
                                                           "Leaderboard", sf::Style::Titlebar | sf::Style::Close);
                        leaderboardWindow.clear(sf::Color::Blue);

                        sf::Font font;
                        if (!font.loadFromFile("../files/font.ttf")) {
                            cout << "font error" << endl;
                            return;
                        }
                        sf::Text leaderboardText("LEADERBOARD", font, 20);
                        leaderboardText.setStyle(sf::Text::Bold | sf::Text::Underlined);
                        leaderboardText.setFillColor(sf::Color::White);
                        sf::FloatRect textRect = leaderboardText.getLocalBounds();
                        leaderboardText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                        leaderboardText.setPosition(leaderboardWindow.getSize().x / 2.0f, (leaderboardWindow.getSize().y / 2.0f) - 120);

                        string displayText;
                        for (size_t i = 0; i < leaderboardEntries.size() && i < 5; ++i) {
                            displayText += to_string(i + 1) + ".\t" + leaderboardEntries[i].first + "\t" +
                                           leaderboardEntries[i].second + "\n\n";
                        }

                        while (leaderboardWindow.isOpen()) {
                            sf::Event event;
                            while (leaderboardWindow.pollEvent(event)) {
                                if (event.type == sf::Event::Closed) {
                                    leaderboardWindow.close()
                                            ;
                                }
                            }
                            leaderboardWindow.clear(sf::Color::Blue);

                            sf::Text entriesText(displayText, font, 18);
                            entriesText.setStyle(sf::Text::Bold);
                            entriesText.setFillColor(sf::Color::White);
                            sf::FloatRect textRect = entriesText.getLocalBounds();
                            entriesText.setOrigin(textRect.width / 2.0f, textRect.height / 2.0f);
                            entriesText.setPosition(leaderboardWindow.getSize().x / 2.0f, (leaderboardWindow.getSize().y / 2.0f) + 20);

                            leaderboardWindow.draw(entriesText);
                            leaderboardWindow.draw(leaderboardText);
                            leaderboardWindow.display();
                        }

                        if(!leaderboardWindow.isOpen()){

                            leaderboardWindowOpen = true;
                        }

                        if (!gameWon) {
                            restoreTilesAfterLeaderboard(tiles, hiddenTexture, revealedTexture, gameWindow, faceButton,
                                                         debugButton,
                                                         pausePlayButton, leaderboardButton, flagCounter, digitsTexture,
                                                         rowCount, colCount, startTime, gameOver, gameWon);
                        }

                        if (!wasPausedBeforeLeaderboard) {
                            isPaused = false;


                        }
                        chrono::steady_clock::time_point pauseEndTime = chrono::steady_clock::now();
                        if (!gameOver && !gameWon && !wasPausedBeforeLeaderboard) {
                            chrono::duration<double> pausedDuration = pauseEndTime - pauseStartTime;
                            startTime += chrono::duration_cast<chrono::milliseconds>(
                                    pausedDuration);
                        }

                    } else if (!gameWon && !gameOver && !isPaused) {
                        if (debugButton.getGlobalBounds().contains(mouseX, mouseY)) {
                            debugMode = !debugMode;
                            for (auto &row: tiles) {
                                for (auto &tile: row) {
                                    if (tile.hasMine) {
                                        if (debugMode) {
                                            tile.overlaySprite.setTexture(mineTexture);
                                            tile.overlaySprite.setColor(sf::Color(255, 255, 255, 255));
                                        } else {
                                            if (tile.isFlagged) {
                                                tile.overlaySprite.setTexture(flagTexture);
                                                tile.overlaySprite.setColor(sf::Color(255, 255, 255, 255));
                                            } else {
                                                tile.overlaySprite.setColor(sf::Color(255, 255, 255, 0));
                                            }
                                        }
                                    }
                                }
                            }
                        } else {
                            int col = mouseX / 32;
                            int row = mouseY / 32;
                            if (row < rowCount && col < colCount) {
                                if (event.mouseButton.button == sf::Mouse::Left && !tiles[row][col].isFlagged) {
                                    revealTile(tiles, row, col, rowCount, colCount, revealedTexture, numberTextures,
                                               mineTexture);
                                    if (tiles[row][col].hasMine) {
                                        gameOver = true;
                                        faceButton.setTexture(loseFaceTexture);
                                        debugMode = false;
                                        timerStopped = true;
                                        endTime = chrono::steady_clock::now();
                                        for (auto &r: tiles) {
                                            for (auto &t: r) {
                                                if (t.hasMine) {
                                                    t.overlaySprite.setTexture(mineTexture);
                                                    t.overlaySprite.setColor(sf::Color(255, 255, 255, 255));
                                                    t.isRevealed = true;
                                                }
                                            }
                                        }
                                    }
                                } else if (event.mouseButton.button == sf::Mouse::Right && !tiles[row][col].isRevealed) {
                                    tiles[row][col].isFlagged = !tiles[row][col].isFlagged;
                                    flagCounter += tiles[row][col].isFlagged ? -1 : 1;
                                    if (tiles[row][col].isFlagged) {
                                        tiles[row][col].overlaySprite.setTexture(flagTexture);
                                        tiles[row][col].overlaySprite.setColor(sf::Color(255, 255, 255, 255));
                                    } else {
                                        if (debugMode && tiles[row][col].hasMine) {
                                            tiles[row][col].overlaySprite.setTexture(mineTexture);
                                            tiles[row][col].overlaySprite.setColor(sf::Color(255, 255, 255, 255));
                                        } else {
                                            tiles[row][col].overlaySprite.setColor(sf::Color(255, 255, 255, 0));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!gameWon && checkWin(tiles)) {
            gameWon = true;
            gameOver = true;
            timerStopped = true;
            endTime = chrono::steady_clock::now();
            faceButton.setTexture(winFaceTexture);
            flagAllMines(tiles, flagTexture);
            flagCounter = 0;

            auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();
            string formattedTime = formatTime(duration);
            updateLeaderboard(formattedTime, playerName, leaderboardEntries, true);


            gameWindow.clear(sf::Color::White);
            for (int i = 0; i < rowCount; i++) {
                for (int j = 0; j < colCount; j++) {
                    tiles[i][j].baseSprite.setTexture(tiles[i][j].isRevealed ? revealedTexture : hiddenTexture);
                    gameWindow.draw(tiles[i][j].baseSprite);
                    if (tiles[i][j].isFlagged || (tiles[i][j].hasMine && gameWon)) {
                        tiles[i][j].overlaySprite.setTexture(flagTexture);
                        tiles[i][j].overlaySprite.setColor(sf::Color(255, 255, 255, 255));
                        gameWindow.draw(tiles[i][j].overlaySprite);
                    } else if (tiles[i][j].isRevealed && tiles[i][j].adjacentMines > 0) {
                        gameWindow.draw(tiles[i][j].overlaySprite);
                    }
                }
            }
            drawCounter(gameWindow, flagCounter, digitsTexture, rowCount, colCount);
            chrono::steady_clock::time_point displayTime = timerStopped ? endTime : chrono::steady_clock::now();
            drawTimer(gameWindow, startTime, chrono::steady_clock::now(), digitsTexture, rowCount, colCount);
            gameWindow.draw(faceButton);
            gameWindow.draw(debugButton);
            gameWindow.draw(pausePlayButton);
            gameWindow.draw(leaderboardButton);
            gameWindow.display();

            sf::RenderWindow leaderboardWindow(sf::VideoMode(colCount * 16, (rowCount * 16) + 50), "Leaderboard", sf::Style::Titlebar | sf::Style::Close);
            leaderboardWindow.clear(sf::Color::Blue);

            sf::Font font;
            if (!font.loadFromFile("../files/font.ttf")) {
                cout << "font error" << endl;
                return;
            }
            sf::Text leaderboardText("LEADERBOARD", font, 20);
            leaderboardText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            leaderboardText.setFillColor(sf::Color::White);
            sf::FloatRect textRect = leaderboardText.getLocalBounds();
            leaderboardText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            leaderboardText.setPosition(leaderboardWindow.getSize().x / 2.0f,
                                        (leaderboardWindow.getSize().y / 2.0f) - 120);


            string displayText;
            for (size_t i = 0; i < leaderboardEntries.size() && i < 5; ++i) {
                displayText += to_string(i + 1) + ".\t" + leaderboardEntries[i].first + "\t" + leaderboardEntries[i].second + "\n\n";
            }

            while (leaderboardWindow.isOpen()) {
                sf::Event event;
                while (leaderboardWindow.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        leaderboardWindow.close()
                                ;
                    }
                }
                leaderboardWindow.clear(sf::Color::Blue);

                sf::Text entriesText(displayText, font, 18);
                entriesText.setStyle(sf::Text::Bold);
                entriesText.setFillColor(sf::Color::White);
                sf::FloatRect textRect = entriesText.getLocalBounds();
                entriesText.setOrigin(textRect.width / 2.0f, textRect.height / 2.0f);
                entriesText.setPosition(leaderboardWindow.getSize().x / 2.0f, (leaderboardWindow.getSize().y / 2.0f) + 20);

                leaderboardWindow.draw(entriesText);
                leaderboardWindow.draw(leaderboardText);
                leaderboardWindow.display();
            }

            sf::Text entriesText(displayText, font, 18);
            entriesText.setFillColor(sf::Color::White);
            entriesText.setPosition(leaderboardWindow.getSize().x / 2.0f, (leaderboardWindow.getSize().y / 2.0f) + 20);

            leaderboardWindow.draw(leaderboardText);
            leaderboardWindow.draw(entriesText);
            leaderboardWindow.display();


        }

        gameWindow.clear(sf::Color::White);
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                tiles[i][j].baseSprite.setTexture(
                        isPaused ? revealedTexture : (tiles[i][j].isRevealed ? revealedTexture : hiddenTexture));
                gameWindow.draw(tiles[i][j].baseSprite);

                if (!isPaused) {
                    if (gameOver && tiles[i][j].hasMine && !gameWon) {
                        tiles[i][j].overlaySprite.setTexture(mineTexture);
                        tiles[i][j].overlaySprite.setColor(sf::Color(255, 255, 255, 255));
                        gameWindow.draw(tiles[i][j].overlaySprite);
                    } else if (gameWon && tiles[i][j].hasMine) {
                        tiles[i][j].overlaySprite.setTexture(flagTexture);
                        tiles[i][j].overlaySprite.setColor(sf::Color(255, 255, 255, 255));
                        gameWindow.draw(tiles[i][j].overlaySprite);
                    } else if ((tiles[i][j].isFlagged || (debugMode && tiles[i][j].hasMine && !gameWon))) {
                        if (debugMode && tiles[i][j].hasMine && !gameWon) {
                            tiles[i][j].overlaySprite.setTexture(mineTexture);
                            tiles[i][j].overlaySprite.setColor(sf::Color(255, 255, 255, 255));
                            gameWindow.draw(tiles[i][j].overlaySprite);
                        }
                        if (tiles[i][j].isFlagged && !(debugMode && tiles[i][j].hasMine && !gameWon)) {
                            tiles[i][j].overlaySprite.setTexture(flagTexture);
                            tiles[i][j].overlaySprite.setColor(sf::Color(255, 255, 255, 255));
                            gameWindow.draw(tiles[i][j].overlaySprite);
                        }
                    } else if (tiles[i][j].adjacentMines > 0 && tiles[i][j].isRevealed) {
                        tiles[i][j].overlaySprite.setTexture(numberTextures[tiles[i][j].adjacentMines - 1]);
                        tiles[i][j].overlaySprite.setColor(sf::Color(255, 255, 255, 255));
                        gameWindow.draw(tiles[i][j].overlaySprite);
                    }
                }
            }
        }
        drawCounter(gameWindow, flagCounter, digitsTexture, rowCount, colCount);
        chrono::steady_clock::time_point displayTime = timerStopped ? endTime : chrono::steady_clock::now();
        drawTimer(gameWindow, startTime, displayTime, digitsTexture, rowCount, colCount);
        gameWindow.draw(faceButton);
        gameWindow.draw(debugButton);
        gameWindow.draw(pausePlayButton);
        gameWindow.draw(leaderboardButton);
        gameWindow.display();
    }
}