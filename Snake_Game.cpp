#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

#define HEIGHT 20
#define WIDTH 40

class SnakeGame {
private:
    int snakeTailX[100], snakeTailY[100];
    int snakeTailLen;
    int gameover, key, score;
    int x, y, fruitx, fruity;
    int levelSpeed;

public:
    SnakeGame() {
        snakeTailLen = 0;
        gameover = 0;
        score = 0;
        levelSpeed = 150; // Default Easy mode speed
    }

    int kbhit() {
        struct termios oldt, newt;
        int ch;
        int oldf;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        if (ch != EOF) {
            ungetc(ch, stdin);
            return 1;
        }
        return 0;
    }

    void input() {
        if (kbhit()) {
            char temp = getchar();
            switch (tolower(temp)) {
                case 'a': if (key != 2) key = 1; break;
                case 'd': if (key != 1) key = 2; break;
                case 'w': if (key != 4) key = 3; break;
                case 's': if (key != 3) key = 4; break;
                case 'x': gameover = 1; break;
            }
        }
    }

    void setup() {
        x = WIDTH / 2;
        y = HEIGHT / 2;
        spawnFruit();
        key = 0;
    }

    void spawnFruit() {
        bool fruitPlaced = false;
        int attempts = 0;
        while (!fruitPlaced && attempts < 100) { // Limit attempts to 100
            fruitx = rand() % WIDTH;
            fruity = rand() % HEIGHT;
            fruitPlaced = true;

            // Ensure fruit does not spawn on the snake's head or body
            if (fruitx == x && fruity == y) {
                fruitPlaced = false;
                attempts++;
                continue;
            }

            for (int i = 0; i < snakeTailLen; i++) {
                if (snakeTailX[i] == fruitx && snakeTailY[i] == fruity) {
                    fruitPlaced = false;
                    attempts++;
                    break;
                }
            }
        }

        // If fruit couldn't be placed after 100 attempts
        if (attempts >= 100) {
            cout << "Failed to place fruit after 100 attempts, try again!" << endl;
        }
    }

    void draw() {
        system("clear");
        for (int i = 0; i < WIDTH + 2; i++) cout << "-";
        cout << endl;
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j <= WIDTH; j++) {
                if (j == 0 || j == WIDTH) cout << "#";
                else if (i == y && j == x) cout << "O";
                else if (i == fruity && j == fruitx) cout << "@";
                else {
                    bool printTail = false;
                    for (int k = 0; k < snakeTailLen; k++) {
                        if (snakeTailX[k] == j && snakeTailY[k] == i) {
                            cout << "o";
                            printTail = true;
                        }
                    }
                    if (!printTail) cout << " ";
                }
            }
            cout << endl;
        }
        for (int i = 0; i < WIDTH + 2; i++) cout << "-";
        cout << endl;
        cout << "Score: " << score << "\nPress X to exit" << endl;
    }

    void logic() {
        int prevX = snakeTailX[0], prevY = snakeTailY[0];
        int prev2X, prev2Y;
        snakeTailX[0] = x;
        snakeTailY[0] = y;
        for (int i = 1; i < snakeTailLen; i++) {
            prev2X = snakeTailX[i];
            prev2Y = snakeTailY[i];
            snakeTailX[i] = prevX;
            snakeTailY[i] = prevY;
            prevX = prev2X;
            prevY = prev2Y;
        }
        switch (key) {
            case 1: x--; break;
            case 2: x++; break;
            case 3: y--; break;
            case 4: y++; break;
        }

        // Check for collisions with walls
        if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) gameover = 1;

        // Check for collisions with the snake body
        for (int i = 0; i < snakeTailLen; i++) {
            if (snakeTailX[i] == x && snakeTailY[i] == y) gameover = 1;
        }

        // Check for fruit collision and spawn new fruit
        if (x == fruitx && y == fruity) {
            spawnFruit(); // Re-spawn fruit
            score += 10;
            snakeTailLen++;
        }
    }

    void selectDifficulty() {
        int choice;
        cout << "Select Difficulty Level:\n1. Easy\n2. Medium\n3. Hard" << endl;
        cin >> choice;
        if (choice == 1) levelSpeed = 150;
        else if (choice == 2) levelSpeed = 100;
        else levelSpeed = 50; 
    }

    void showRules() {
        cout << "Welcome to Snake Game!" << endl;
        cout << "Rules: " << endl;
        cout << "1. Control the snake using W (Up), S (Down), A (Left), D (Right)." << endl;
        cout << "2. Eat the fruit (@) to grow and gain points." << endl;
        cout << "3. Avoid hitting the walls or yourself." << endl;
        cout << "4. Press X to exit the game anytime." << endl;
        cout << "Press Enter to Start...";
        cin.ignore();
        cin.get();
    }

    void startGame() {
        srand(time(0));
        showRules();
        selectDifficulty();
        setup();
        while (!gameover) {
            draw();
            input();
            logic();
            this_thread::sleep_for(chrono::milliseconds(levelSpeed));
        }
        system("clear");
        cout << "Game Over!" << endl;
        cout << "Your final score is: " << score << endl;
    }
};

int main() {
    SnakeGame game;
    game.startGame();
    return 0;
}
