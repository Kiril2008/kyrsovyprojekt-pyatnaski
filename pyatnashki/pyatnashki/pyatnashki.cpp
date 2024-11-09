#include <iostream>    
#include <vector>      
#include <ctime>       
#include <cstdlib>     
#include <chrono>      // для виміру тривалості гри 
#include <fstream>     
#include <string>      
#include <algorithm>   
#include <thread>      // Бібліотека для роботи з потоками, наприклад, для паузи між ходами комп'ютера (sleep_for()).

using namespace std;
using namespace chrono; // компоненти для вимірювання часу гри 

#ifdef _WIN32
#define CLEAR_SCREEN() system("cls") // Очищення екрану для Windows
#else
#define CLEAR_SCREEN() system("clear") // Очищення екрану для Linux MacOS
#endif

// клас Game реалізує логіку гри 
class Game {
private:
    vector<vector<int>> board;  // ігрова дошка з плитками
    int size;  // розмір дошки (3x3 або 4x4)
    int empty_x, empty_y;  // координати порожньої клітинки
    int moves_count;  // підрахунок (лічильник) ходів
    time_point<steady_clock> start_time;  // час початку гри
    bool is_user_playing;  // прапорець, чи грає користувач (або комп'ютер)

    // функція перемішує плитки на дошці
    void shuffleBoard() {
        vector<int> tiles(size * size);  // одновимірний масив плиток
        for (int i = 0; i < size * size - 1; i++) {
            tiles[i] = i + 1;  // заповнюємо плитками від 1 до (size*size - 1)
        }
        tiles[size * size - 1] = 0;  // остання плитка порожня (0)

        // перемішуємо до отримання розв'язання
        do {
            random_shuffle(tiles.begin(), tiles.end());
        } while (!isSolvable(tiles));

        // заповнення дошки перемішаними плитками
        int k = 0;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                board[i][j] = tiles[k++];
                if (board[i][j] == 0) {  // встановлення координат порожньої плитки
                    empty_x = i;
                    empty_y = j;
                }
            }
        }
    }

    // перевіряє, чи є конфігурація плиток розв'язною
    bool isSolvable(vector<int>& tiles) {
        int inversions = 0;  // кількість інверсій в масиві плиток
        for (int i = 0; i < tiles.size(); i++) {
            for (int j = i + 1; j < tiles.size(); j++) {
                // підрахунок випадків, коли більший номер передує меншому
                if (tiles[i] && tiles[j] && tiles[i] > tiles[j]) inversions++;
            }
        }
        // перевірка розв'язності залежно від розміру дошки
        if (size % 2 == 1) {
            return inversions % 2 == 0;
        }
        else {
            int empty_row = size - empty_x;  // позиція порожньої клітинки знизу
            return (inversions + empty_row) % 2 == 1;
        }
    }

    // функція для відображення дошки на екран
    void displayBoard() {
        cout << "\n";
        for (int i = 0; i < size; i++) {
            cout << "+";
            for (int j = 0; j < size; j++) {
                cout << "---+";
            }
            cout << "\n|";
            for (int j = 0; j < size; j++) {
                if (board[i][j] == 0)
                    cout << "   |";  // вивід порожньої клітинки
                else
                    cout << (board[i][j] < 10 ? " " : "") << board[i][j] << " |";  // вивід чисел 
            }
            cout << "\n";
        }
        cout << "+";
        for (int j = 0; j < size; j++) {
            cout << "---+";
        }
        cout << "\n";
    }
    // перевірка чи досягнуто переможний стан (плитки в порядку)
    bool isSolved() {
        int k = 1;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (board[i][j] != k % (size * size))  // порівняння кожної плитки з потрібним значенням
                    return false;
                k++;
            }
        }
        return true;
    }

    // переміщення плитки на основі зміщення (dx, dy)
    void moveTile(int dx, int dy) {
        int new_x = empty_x + dx;
        int new_y = empty_y + dy;

        // перевіряємо, що нова координата в межах дошки
        if (new_x >= 0 && new_x < size && new_y >= 0 && new_y < size) {
            swap(board[empty_x][empty_y], board[new_x][new_y]);  // міняємо місцями плитку і пусте місце
            empty_x = new_x;  // оновлюємо координати пустої клітинки
            empty_y = new_y;
            moves_count++;  // збільшуємо лічильник ходів
        }
    }

    // алгоритм для вирішення головоломки комп'ютером
    void computerSolve() {
        cout << "Комп'ютер вирішує головоломку...\n";
        int step_count = 0;  // обмеження кількості випадкових ходів для уникнення довгого рішення
        while (!isSolved() && step_count < 1000) {
            int dx = 0, dy = 0;
            switch (rand() % 4) {
            case 0: dx = -1; break; // вгору
            case 1: dx = 1; break;  // вниз
            case 2: dy = -1; break; // вліво
            case 3: dy = 1; break;  // вправо
            }
            moveTile(dx, dy);  // робиться хід
            displayBoard();
            this_thread::sleep_for(chrono::milliseconds(50));  // затримка для візуального ефекту
            CLEAR_SCREEN();
            step_count++;
        }
        cout << "Комп'ютер завершив спробу вирішення!\n";
    }
public:
    // конструктор гри ініціалізує основні змінні
    Game(int n, bool is_user) : size(n), moves_count(0), is_user_playing(is_user) {
        board.resize(size, vector<int>(size));  // Створюємо порожню дошку заданого розміру.
        start_time = steady_clock::now();  // Фіксуємо час початку.
        shuffleBoard();  // перемішування чисел на дошці
    }

    // Виведення правил гри
    void displayRules() {
        cout << "+-------------------------+\n";
        cout << "|    Гра 'П'ятнашки'      |\n";
        cout << "| Мета гри: зібрати всі   |\n";
        cout << "| плитки в порядку від 1  |\n";
        cout << "| до " << size * size - 1 << ", де 0 -  це порожня |\n";
        cout << "| клітинка. Використовуйте|\n";
        cout << "| W, A, S, D для руху.    |\n";
        cout << "+-------------------------+\n\n";
    }

    // Основний цикл гри для користувача або комп'ютера
    void play() {
        displayRules();  // Виводимо правила перед початком гри

        if (is_user_playing) {  // якщо грає користувач
            char input;
            cout << "Використовуйте W, A, S, D для переміщення плиток.\n";
            displayBoard();  // відображення дошки перед першим ходом

            // Основний ігровий цикл для користувача
            while (!isSolved()) {
                cout << "Ваш хід: ";
                cin >> input;
                CLEAR_SCREEN();  // очищення екрану для зручності

                // Виконуємо дію в залежності від введеної команди
                if (input == 'W' || input == 'w') moveTile(-1, 0);  // вгору
                else if (input == 'S' || input == 's') moveTile(1, 0);  // вниз
                else if (input == 'A' || input == 'a') moveTile(0, -1);  // вліво
                else if (input == 'D' || input == 'd') moveTile(0, 1);   // вправо

                displayBoard();  // оновлюємо відображення дошки після ходу
            }
            cout << "Вітаємо, ви вирішили головоломку!\n";
        }
        else {  // якщо грає комп'ютер
            computerSolve();  // запуск функції автоматичного розв'язання головоломки
        }

        saveStatistics();  // збереження статистики гри
        showStatistics();  // виведення історії попередніх ігор
    }

    // Функція зберігання статистики у файл
    void saveStatistics() {
        ofstream file("statistics.txt", ios::app);
        if (file.is_open()) {
            auto end_time = steady_clock::now();
            auto time_spent = duration_cast<seconds>(end_time - start_time).count();  // розрахунок часу гри
            file << (is_user_playing ? "Гравець" : "Комп'ютер") << " вирішив головоломку "
                << size * size - 1 << "-п'ятнашок за " << time_spent << " секунд і "
                << moves_count << " ходів.\n";
            file.close();
        }
    }

    // Виведення історії попередніх ігор з файлу
    void showStatistics() {
        ifstream file("statistics.txt");
        string line;
        if (file.is_open()) {
            cout << "\n--- Історія ігор ---\n";
            while (getline(file, line)) {
                cout << line << endl;
            }
            file.close();
        }
        else {
            cout << "Немає збереженої статистики.\n";
        }
    }
};
// головна функція
int main() {
    setlocale(LC_ALL, "Ukrainian");  // Встановлення локалізації для української мови (може не працювати на всіх платформах)
    srand(time(0));  // Ініціалізація генератора випадкових чисел для перемішування плиток

    int mode, size;
    bool is_user;

    // Пропонуємо користувачу вибрати розмір гри (3x3 або 4x4)
    cout << "Оберіть режим гри:\n1. 8-п'ятнашки (3x3)\n2. 15-п'ятнашки (4x4)\nВаш вибір: ";
    cin >> mode;
    size = (mode == 1) ? 3 : 4;  // Встановлюємо розмір залежно від вибору

    // Пропонуємо користувачу вибрати, хто буде грати - гравець чи комп'ютер
    cout << "Оберіть, хто буде збирати п'ятнашки:\n1. Гравець\n2. Комп'ютер\nВаш вибір: ";
    cin >> mode;
    is_user = (mode == 1);  // встановлюємо прапорець чи грає користувач

    Game game(size, is_user);  // створюємо об'єкт гри з відповідним розміром і типом гравця
    game.play();  // запускаємо гру

    return 0;
}
