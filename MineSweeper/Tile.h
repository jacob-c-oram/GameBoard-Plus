#include <SFML/Graphics.hpp>
using namespace std;

class Tile {
public:
    sf::Sprite baseSprite;
    sf::Sprite overlaySprite;
    bool isRevealed;
    bool isFlagged;
    bool hasMine;
    int adjacentMines;

    Tile() : isRevealed(false), isFlagged(false), hasMine(false), adjacentMines(0) {}

    void setBaseTexture(sf::Texture& texture) {
        baseSprite.setTexture(texture);
    }

    void setOverlayTexture(sf::Texture& texture) {
        overlaySprite.setTexture(texture);
        overlaySprite.setColor(sf::Color(255, 255, 255, 0));
    }

    void setPosition(float x, float y) {
        baseSprite.setPosition(x, y);
        overlaySprite.setPosition(x, y);
    }
};
