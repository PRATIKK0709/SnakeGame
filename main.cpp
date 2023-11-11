#include <SFML/Graphics.hpp>
#include <iostream>
#include <deque>
#include <cstdlib> // for rand function

// Define the size of the grid and the size of each cell
const int gridSize = 20;
const int cellSize = 20;

// Define the direction enum
enum class Direction { Up, Down, Left, Right };

// Define the Snake class
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

    // Helper method to add a new body segment without removing the tail
    void addBodySegment(sf::Vector2i position);
};

Snake::Snake(int initialLength) : hasEaten(false) {
    // Initialize the snake with a starting length and direction
    for (int i = 0; i < initialLength; ++i) {
        body.push_front({i, 0});
    }
    direction = Direction::Right;
}

void Snake::move() {
    // Move the snake in the current direction
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

    // Check if the snake has eaten
    if (hasEaten) {
        hasEaten = false;
    } else {
        // If not, remove the tail
        body.pop_back();
    }
}

void Snake::grow() {
    // Set a flag to indicate that the snake has eaten
    hasEaten = true;
}

void Snake::addBodySegment(sf::Vector2i position) {
    // Add a new body segment to the snake without removing the tail
    body.push_front(position);
}

bool Snake::checkCollision() {
    // Check if the snake has collided with the window boundaries or itself
    sf::Vector2i head = body.front();

    // Check if the head is outside the window boundaries
    if (head.x < 0 || head.x >= gridSize || head.y < 0 || head.y >= gridSize) {
        return true;
    }

    // Check if the snake has collided with itself
    for (auto it = body.begin() + 1; it != body.end(); ++it) {
        if (*it == head) {
            return true;
        }
    }

    return false;
}

void Snake::setDirection(Direction dir) {
    // Set the direction, ensuring that the snake cannot reverse direction instantly
    if ((dir == Direction::Up && direction != Direction::Down) ||
        (dir == Direction::Down && direction != Direction::Up) ||
        (dir == Direction::Left && direction != Direction::Right) ||
        (dir == Direction::Right && direction != Direction::Left)) {
        direction = dir;
    }
}

void Snake::draw(sf::RenderWindow& window) {
    // Draw each segment of the snake
    for (const sf::Vector2i& segment : body) {
        sf::RectangleShape bodySegment(sf::Vector2f(cellSize, cellSize));
        bodySegment.setPosition(segment.x * cellSize, segment.y * cellSize);
        bodySegment.setFillColor(sf::Color::Green);
        window.draw(bodySegment);
    }
}

sf::Vector2i Snake::getHeadPosition() {
    // Get the position of the snake's head
    return body.front();
}

// Define the Food class
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
    // Set the initial position of the food
    respawn();
}

void Food::respawn() {
    // Respawn the food in a random position within the grid
    position = {rand() % gridSize, rand() % gridSize};
}

void Food::draw(sf::RenderWindow& window) {
    // Draw the food at its position
    sf::RectangleShape food(sf::Vector2f(cellSize, cellSize));
    food.setPosition(position.x * cellSize, position.y * cellSize);
    food.setFillColor(sf::Color::Red);
    window.draw(food);
}

sf::Vector2i Food::getPosition() {
    // Get the position of the food
    return position;
}

// Define the main function
int main() {
    // Create a window
    sf::RenderWindow window(sf::VideoMode(gridSize * cellSize, gridSize * cellSize), "SFML Snake Game");

    // Create the snake and food with an initial length of 3
    Snake snake(3);
    Food food;

    // Set the initial direction to the right
    snake.setDirection(Direction::Right);

    // Create a clock for controlling the frame rate
    sf::Clock clock;

    // Main game loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle keyboard input to change the snake's direction
            if (event.type == sf::Event::KeyPressed) {
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
            }
        }

        // Move the snake every 0.1 seconds
        if (clock.getElapsedTime().asSeconds() >= 0.1) {
            // Move the snake
            snake.move();

            // Check for collisions with itself or the window boundaries
            if (snake.checkCollision()) {
                std::cout << "Game Over! Collision with the window boundary or self.\n";
                window.close();
            }

            // Check for collisions with food
            if (snake.getHeadPosition() == food.getPosition()) {
                // If the snake eats the food, grow and respawn the food
                snake.grow();
                food.respawn();
            }

            clock.restart();
        }

        // Clear the window
        window.clear();

        // Draw the food and snake
        food.draw(window);
        snake.draw(window);

        // Display the contents of the window
        window.display();
    }

    return 0;
}
