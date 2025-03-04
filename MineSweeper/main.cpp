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
#include "GameWindowFunction.h"
using namespace std;

int main() {
    ifstream configFile("../files/config.cfg");
    int colCount, rowCount, mineCount;
    if (!configFile.is_open()) {
        cout << "files error" << endl;
        return -1;
    }
    configFile >> colCount >> rowCount >> mineCount;
    configFile.close();

    sf::RenderWindow window(sf::VideoMode((colCount * 32), (rowCount * 32) + 100), "Minesweeper Welcome", sf::Style::Close);
    window.setFramerateLimit(30);

    sf::Color backgroundColor = sf::Color::Blue;

    sf::Font font;
    if (!font.loadFromFile("../files/font.ttf")) {
        cout << "font error" << endl;
        return -1;
    }

    sf::Text welcomeText("WELCOME TO MINESWEEPER!", font, 24);
    welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    welcomeText.setFillColor(sf::Color::White);

    sf::Text promptText("Enter your name:", font, 20);
    promptText.setStyle(sf::Text::Bold);
    promptText.setFillColor(sf::Color::White);

    sf::Text nameText("|", font, 18);
    nameText.setStyle(sf::Text::Bold);
    nameText.setFillColor(sf::Color::Yellow);

    auto centerText = [&](sf::Text& text, float y) {
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(window.getSize().x / 2.0f, y));
    };

    centerText(welcomeText, window.getSize().y / 2 - 150);
    centerText(promptText, window.getSize().y / 2 - 75);
    centerText(nameText, window.getSize().y / 2 - 45);

    string playerName;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !playerName.empty()) {
                    playerName.pop_back();
                } else if (event.text.unicode < 128 && isalpha(static_cast<char>(event.text.unicode)) && playerName.length() < 10) {
                    char typedChar = static_cast<char>(event.text.unicode);
                    if (playerName.empty()) {
                        playerName += toupper(typedChar);
                    } else {
                        playerName += tolower(typedChar);
                    }
                }
                if (playerName.empty()) {
                    nameText.setString("|");
                } else {
                    nameText.setString(playerName + '|');
                }
                centerText(nameText, window.getSize().y / 2 - 45);
            } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && !playerName.empty()) {
                window.close();
                openGameWindow(rowCount, colCount, mineCount, playerName);
            }
        }

        window.clear(backgroundColor);
        window.draw(welcomeText);
        window.draw(promptText);
        window.draw(nameText);
        window.display();
    }

    return 0;
}





