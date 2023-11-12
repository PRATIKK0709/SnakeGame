#include <SFML/Graphics.hpp>
#include <iostream>
#include <deque>

const int gridSize = 20;
const int cellSize = 20;

enum class Direction { Up, Down, Left, Right };
bool inGame = false;
bool inGameOverScreen = false;
float gameOverScreenAlpha = 0.0f;
const float fadeSpeed = 150.0f; // Adjust the fade-in speed

class Snake {
public:
    Snake(int initialLength);
    void move();
    void grow();
    bool checkCollision();
    void setDirection(Direction dir);
    void draw(sf::RenderWindow& window);
    sf::Vector2i getHeadPosition();

private:
    std::deque<sf::Vector2i> body;
    Direction direction;
    bool hasEaten;

    void addBodySegment(sf::Vector2i position);
};

Snake::Snake(int initialLength) : hasEaten(false) {
    for (int i = 0; i < initialLength; ++i) {
        body.push_front({i, 0});
    }
    direction = Direction::Right;
}

void Snake::move() {
    sf::Vector2i head = body.front();
    switch (direction) {
        case Direction::Up:
            body.push_front({head.x, head.y - 1});
            break;
        case Direction::Down:
            body.push_front({head.x, head.y + 1});
            break;
        case Direction::Left:
            body.push_front({head.x - 1, head.y});
            break;
        case Direction::Right:
            body.push_front({head.x + 1, head.y});
            break;
    }

    if (hasEaten) {
        hasEaten = false;
    } else {
        body.pop_back();
    }
}

void Snake::grow() {
    hasEaten = true;
}

void Snake::addBodySegment(sf::Vector2i position) {
    body.push_front(position);
}

bool Snake::checkCollision() {
    sf::Vector2i head = body.front();

    if (head.x < 0 || head.x >= gridSize || head.y < 0 || head.y >= gridSize) {
        return true;
    }

    for (auto it = body.begin() + 1; it != body.end(); ++it) {
        if (*it == head) {
            return true;
        }
    }

    return false;
}

void Snake::setDirection(Direction dir) {
    if ((dir == Direction::Up && direction != Direction::Down) ||
        (dir == Direction::Down && direction != Direction::Up) ||
        (dir == Direction::Left && direction != Direction::Right) ||
        (dir == Direction::Right && direction != Direction::Left)) {
        direction = dir;
    }
}

void Snake::draw(sf::RenderWindow& window) {
    for (const sf::Vector2i& segment : body) {
        sf::RectangleShape bodySegment(sf::Vector2f(cellSize, cellSize));
        bodySegment.setPosition(segment.x * cellSize, segment.y * cellSize);
        bodySegment.setFillColor(sf::Color::Green);
        window.draw(bodySegment);
    }
}

sf::Vector2i Snake::getHeadPosition() {
    return body.front();
}

class Food {
public:
    Food();
    void respawn();
    void draw(sf::RenderWindow& window);
    sf::Vector2i getPosition();

private:
    sf::Vector2i position;
};

Food::Food() {
    respawn();
}

void Food::respawn() {
    position = {rand() % gridSize, rand() % gridSize};
}

void Food::draw(sf::RenderWindow& window) {
    sf::RectangleShape food(sf::Vector2f(cellSize, cellSize));
    food.setPosition(position.x * cellSize, position.y * cellSize);
    food.setFillColor(sf::Color::Red);
    window.draw(food);
}

sf::Vector2i Food::getPosition() {
    return position;
}

class Menu {
public:
    Menu(sf::RenderWindow& window);
    void draw();
    void update(sf::Event& event);
    int getSelectedItemIndex();
    sf::Font& getFont(); // Added function to get the font

private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text title;
    sf::Text startText;
    sf::Text exitText;
    int selectedItemIndex;

    void updateTextAppearance();
};

Menu::Menu(sf::RenderWindow& window) : window(window), selectedItemIndex(0) {
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error: Unable to load font 'arial.ttf'" << std::endl;
    }

    title.setFont(font);
    title.setString("SFML Snake Game");
    title.setCharacterSize(35);  // Set a smaller font size
    title.setFillColor(sf::Color::White);

    startText.setFont(font);
    startText.setString("Start Game");
    startText.setCharacterSize(25);  // Set a smaller font size
    startText.setFillColor(sf::Color::White);

    exitText.setFont(font);
    exitText.setString("Exit");
    exitText.setCharacterSize(25);  // Set a smaller font size
    exitText.setFillColor(sf::Color::White);

    updateTextAppearance();
}

void Menu::draw() {
    sf::RectangleShape menuBackground(sf::Vector2f(window.getSize().x / 2, window.getSize().y));
    menuBackground.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(menuBackground);

    window.draw(title);
    window.draw(startText);
    window.draw(exitText);
}

void Menu::update(sf::Event& event) {
    switch (event.key.code) {
        case sf::Keyboard::Up:
            if (selectedItemIndex > 0) {
                selectedItemIndex--;
                updateTextAppearance();
            }
            break;

        case sf::Keyboard::Down:
            if (selectedItemIndex < 1) {
                selectedItemIndex++;
                updateTextAppearance();
            }
            break;

        case sf::Keyboard::Enter:
            if (selectedItemIndex == 0) {
                inGame = true;
                inGameOverScreen = false;
            } else if (selectedItemIndex == 1) {
                window.close();
            }
            break;

        default:
            break;
    }
}

int Menu::getSelectedItemIndex() {
    return selectedItemIndex;
}

sf::Font& Menu::getFont() {
    return font;
}

void Menu::updateTextAppearance() {
    switch (selectedItemIndex) {
        case 0: 
            startText.setFillColor(sf::Color::Yellow);
            exitText.setFillColor(sf::Color::White);
            break;

        case 1:
            startText.setFillColor(sf::Color::White);
            exitText.setFillColor(sf::Color::Yellow);
            break;

        default:
            break;
    }

    title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, window.getSize().y / 6);
    startText.setPosition(window.getSize().x / 2 - startText.getGlobalBounds().width / 2, window.getSize().y / 2);
    exitText.setPosition(window.getSize().x / 2 - exitText.getGlobalBounds().width / 2, window.getSize().y / 1.5);
}

void fadeInGameOverScreen(sf::RenderWindow& window, sf::Font& font) {
    if (gameOverScreenAlpha < 255.0f) {
        gameOverScreenAlpha += fadeSpeed * sf::Time(sf::seconds(1)).asSeconds(); // Fix for frame time
        if (gameOverScreenAlpha > 255.0f) {
            gameOverScreenAlpha = 255.0f;
        }
    }

    sf::RectangleShape fadeRect(sf::Vector2f(window.getSize().x, window.getSize().y));
    fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(gameOverScreenAlpha)));
    window.draw(fadeRect);

    sf::Text gameOverText;
    gameOverText.setFont(font); // Use the provided font
    gameOverText.setString("Game Over! Press Enter to restart.");
    gameOverText.setCharacterSize(20);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(window.getSize().x / 2 - gameOverText.getGlobalBounds().width / 2, window.getSize().y / 2 - 20);
    window.draw(gameOverText);
}

// ... (Previous code remains unchanged)

int main() {
    sf::RenderWindow window(sf::VideoMode(gridSize * cellSize, gridSize * cellSize), "SFML Snake Game");

    Snake snake(3);
    Food food;
    snake.setDirection(Direction::Right);

    sf::Clock clock;
    sf::Clock gameOverClock; // Clock for fade-in effect

    Menu menu(window);

    bool inMenu = true;
    bool inGameOverScreen = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (inGame) {
                    switch (event.key.code) {
                        case sf::Keyboard::Up:
                            snake.setDirection(Direction::Up);
                            break;
                        case sf::Keyboard::Down:
                            snake.setDirection(Direction::Down);
                            break;
                        case sf::Keyboard::Left:
                            snake.setDirection(Direction::Left);
                            break;
                        case sf::Keyboard::Right:
                            snake.setDirection(Direction::Right);
                            break;
                    }
                } else if (inGameOverScreen && event.key.code == sf::Keyboard::Enter) {
                    inGameOverScreen = false;
                    inMenu = true;
                    gameOverClock.restart();
                } else {
                    menu.update(event);
                }
            }
        }

        if (inMenu) {
            window.clear();
            menu.draw();

            if (menu.getSelectedItemIndex() == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                inGame = true;
                inMenu = false;
                snake = Snake(3); // Reset the snake
                food.respawn();   // Respawn the food
            } else if (menu.getSelectedItemIndex() == 1 && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                window.close();
            }

            window.display();
            continue;
        }

        if (!inGameOverScreen) {
            if (clock.getElapsedTime().asSeconds() >= 0.1) {
                snake.move();

                if (snake.checkCollision()) {
                    inGameOverScreen = true;
                    gameOverClock.restart();
                }

                if (snake.getHeadPosition() == food.getPosition()) {
                    snake.grow();
                    food.respawn();
                }

                clock.restart();
            }
        }

        window.clear();

        if (inGameOverScreen) {
            fadeInGameOverScreen(window, menu.getFont());

            // Add a delay after game over for the fade-in effect
            if (gameOverClock.getElapsedTime().asSeconds() >= 1.0) {
                inGameOverScreen = false;
                inMenu = true;
            }
        } else {
            food.draw(window);
            snake.draw(window);
        }

        window.display();
    }

    return 0;
}
