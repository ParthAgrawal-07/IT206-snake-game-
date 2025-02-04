#include <iostream>
#include <unistd.h>  // For usleep function
#include <termios.h> // For terminal input settings
#include <fcntl.h>   // For file control options
#include <cstdlib>   // For rand and srand
#include <ctime>     // For time
#include <vector>    // For vector

using namespace std;

const int width = 20;
const int height = 20;
int highScore;

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };

class Snake {
public:
    int x, y;
    vector<int> tailX, tailY;
    int nTail;
    eDirection dir;

    Snake() {
        x = width / 2;
        y = height / 2;
        nTail = 0;
        dir = STOP;
    }

    void move() {
        int prevX = tailX.empty() ? x : tailX[0];
        int prevY = tailY.empty() ? y : tailY[0];
        int prev2X, prev2Y;
        if (!tailX.empty()) {
            tailX[0] = x;
            tailY[0] = y;
        }
        for (int i = 1; i < nTail; i++) {
            prev2X = tailX[i];
            prev2Y = tailY[i];
            tailX[i] = prevX;
            tailY[i] = prevY;
            prevX = prev2X;
            prevY = prev2Y;
        }
        switch (dir) {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        default:
            break;
        }
    }

    bool checkCollision() const {
        if (x >= width || x < 0 || y >= height || y < 0)
            return true;
        for (int i = 0; i < nTail; i++) {
            if (tailX[i] == x && tailY[i] == y)
                return true;
        }
        return false;
    }

    void grow() {
        tailX.push_back(x);
        tailY.push_back(y);
        nTail++;
    }
};

class Fruit {
public:
    int x, y;

    Fruit() {
        x = rand() % width;
        y = rand() % height;
    }

    void reset() {
        x = rand() % width;
        y = rand() % height;
    }
};

class GameState {
public:
    bool gameOver;
    int score;

    GameState() {
        gameOver = false;
        score = 0;
    }
};

class Game {
private:
    Snake snake;
    Fruit fruit;
    GameState state;

public:
    Game() {
        srand(time(0));
    }

    void draw() const {
        system("clear"); // Use "cls" on Windows and "clear" on Unix
        for (int i = 0; i < width + 2; i++)
            cout << "#";
        cout << endl;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (j == 0)
                    cout << "#";
                if (i == snake.y && j == snake.x)
                    cout << "O";
                else if (i == fruit.y && j == fruit.x)
                    cout << "F";
                else {
                    bool print = false;
                    for (int k = 0; k < snake.nTail; k++) {
                        if (snake.tailX[k] == j && snake.tailY[k] == i) {
                            cout << "o";
                            print = true;
                        }
                    }
                    if (!print)
                        cout << " ";
                }

                if (j == width - 1)
                    cout << "#";
            }
            cout << endl;
        }

        for (int i = 0; i < width + 2; i++)
            cout << "#";
        cout << endl;

        cout << "Score: " << state.score << " | High Score: " << highScore << endl;
    }

    void input() {
        struct termios oldt, newt;
        int oldf;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        char ch = getchar();
        switch (ch) {
        case 'a':
            snake.dir = LEFT;
            break;
        case 'd':
            snake.dir = RIGHT;
            break;
        case 'w':
            snake.dir = UP;
            break;
        case 's':
            snake.dir = DOWN;
            break;
        case 'x':
            state.gameOver = true;
            break;
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
    }

    void logic() {
        snake.move();

        if (snake.checkCollision()) {
            state.gameOver = true;
        }

        if (snake.x == fruit.x && snake.y == fruit.y) {
            state.score += 10;
            if (state.score > highScore) highScore = state.score;
            fruit.reset();
            snake.grow();
        }
    }

    void playAgain() {
        char choice;
        cout << "Do you want to play again? (y/n): ";
        cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            Game newGame;
            newGame.start();
        } else {
            exit(0);
        }
    }

    void start() {
        while (true) {
            draw();
            input();
            logic();
            usleep(100000); // Sleep for 0.1 seconds

            if (state.gameOver) {
                cout << "Game Over! Final Score: " << state.score << endl;
                if (state.score > highScore) {
                    highScore = state.score;
                    cout << "New High Score!" << endl;
                }
                playAgain();
            }
        }
    }
};

int main() {
    Game game;
    game.start();
    return 0;
}
