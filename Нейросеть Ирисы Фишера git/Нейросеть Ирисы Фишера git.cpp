#include <iostream>
#include <math.h>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class Neuron_Web {
public:
    const int size_X0 = 5, size_X1 = 5, size_X2 = 4, size_X3 = 3;
    //float W01[size_X0][size_X1 - 1], W12[size_X1][size_X2 - 1], W23[size_X2][size_X3], X0[size_X0][2], X1[size_X1][2], X2[size_X2][2], X3[size_X3][2];
    float W01[5][4], W12[5][3], W23[4][3], X0[5][2], X1[5][2], X2[4][2], X3[3][2]; // Создаём матрицы слоёв и весов. Я переделаю так, чтобы это было не хардкодом, а создавалось динамически
    
    Neuron_Web() { // Инициализируем веса рандомными значениями, также инициализируем нейроны смещения на всех слоях, кроме выходного
        random(*W01, size_X0, size_X1 - 1);
        random(*W12, size_X1, size_X2 - 1);
        random(*W23, size_X2, size_X3);
        X0[size_X0 - 1][0] = 1;
        X1[size_X1 - 1][0] = 1;
        X2[size_X2 - 1][0] = 1;
    }

    void go(float* a, float* b) { // Выполняем проход по нейронной сети с входными параметрами (a) и со значениями, который должны быть на выходе (b)
        for (int i = 0; i < size_X0 - 1; i++) {
            X0[i][0] = a[i]; // Инициализируем нейроны первого слоя входными параметрами
        }

        probeg(*X0, *X1, *W01, size_X0, size_X1 - 1); // Пробегаем по всем слоям
        probeg(*X1, *X2, *W12, size_X1, size_X2 - 1);
        probeg(*X2, *X3, *W23, size_X2, size_X3);

        for (int i = 0; i < size_X3; i++) {
            X3[i][1] = (b[i] - X3[i][0]); // Получаем значения ошибок на выходном слое
        }

        error(*X2, *X3, *W23, size_X2, size_X3); // Получаем значения ошибок на всех остальных слоях
        error(*X1, *X2, *W12, size_X1, size_X2 - 1);
        error(*X0, *X1, *W01, size_X0, size_X1 - 1);

        //error(b);
        fix(*X0, *X1, *W01, size_X0, size_X1 - 1); // Корректируем веса в зависимости от ошибок
        fix(*X1, *X2, *W12, size_X1, size_X2 - 1);
        fix(*X2, *X3, *W23, size_X2, size_X3);
    }

    float go_check_error(float* a, float* b) { // То же самое, что и метод void go(float* a, float* b), только этот метод возвращает сумму квадратов ошибок на выходе
        for (int i = 0; i < size_X0 - 1; i++) {
            X0[i][0] = a[i];
        }

        probeg(*X0, *X1, *W01, size_X0, size_X1 - 1);
        probeg(*X1, *X2, *W12, size_X1, size_X2 - 1);
        probeg(*X2, *X3, *W23, size_X2, size_X3);

        for (int i = 0; i < size_X3; i++) {
            X3[i][1] = (b[i] - X3[i][0]);
        }

        error(*X2, *X3, *W23, size_X2, size_X3);
        error(*X1, *X2, *W12, size_X1, size_X2 - 1);
        error(*X0, *X1, *W01, size_X0, size_X1 - 1);

        //error(b);
        fix(*X0, *X1, *W01, size_X0, size_X1 - 1);
        fix(*X1, *X2, *W12, size_X1, size_X2 - 1);
        fix(*X2, *X3, *W23, size_X2, size_X3);

        return X3[0][1] * X3[0][1] + X3[1][1] * X3[1][1] + X3[2][1] * X3[2][1];
    }

    void go(float* a) { // Выполняем проход по нейросети без исправления весов
        for (int i = 0; i < size_X0 - 1; i++) {
            X0[i][0] = a[i];
        }

        probeg(*X0, *X1, *W01, size_X0, size_X1 - 1);
        probeg(*X1, *X2, *W12, size_X1, size_X2 - 1);
        probeg(*X2, *X3, *W23, size_X2, size_X3);
    }

    void error(float* x1, float* x2, float* w, int size1, int size2) { // Вычисление ошибки для матрицы весов
        for (int i = 0; i < size1; i++) {
            x1[i * 2 + 1] = 0;
            for (int k = 0; k < size2; k++) {
                x1[i * 2 + 1] += x2[k * 2 + 1] * w[k + i * size2];
            }
        }
    }

    void probeg(float* x1, float* x2, float* w, int size1, int size2) { // Вычисление значений для слоя
        for (int i = 0; i < size2; i++) {
            x2[i * 2] = 0;
            for (int k = 0; k < size1; k++) {
                x2[i * 2] += x1[k * 2] * w[k * size2 + i];
            }
            x2[i * 2] = 1 / (1 + exp(-1 * x2[i * 2])); // Использую логистическую функцию активации
        }
    }

    void fix(float* x1, float* x2, float* w, int size1, int size2) { // Корректировка весов в зависимости от ошибки
        for (int i = 0; i < size1; i++) {
            for (int k = 0; k < size2; k++) {
                w[k + i * size2] += 0.1 * x2[k * 2 + 1] * x1[i * 2] * x2[k * 2] * (1 - x2[k * 2]); // Корректировка весов для логистической функции активации
            }
        }
    }


    void print() { // Вывод матриц всех весов и слоёв
        print(*W01, size_X0, size_X1-1);
        cout << endl;
        print(*W12, size_X1, size_X2-1);
        cout << endl;
        print(*W23, size_X2, size_X3);
        cout << endl;
        print(*X0, size_X0, 2);
        cout << endl;
        print(*X1, size_X1, 2);
        cout << endl;
        print(*X2, size_X2, 2);
        cout << endl;
        print(*X3, size_X3, 2);
        cout << endl;
    }

    void random(float* a, int size1, int size2) { // Инициализация матрицы весов
        for (int i = 0; i < size1; i++) {
            for (int k = 0; k < size2; k++) {
                a[i * size2 + k] = float(rand() % 10) / 10;
            }
        }
    }

    void print(float* a, int size1, int size2) { // Вывод матрицы
        for (int i = 0; i < size1; i++) {
            for (int k = 0; k < size2; k++) {
                cout << a[i * size2 + k] << "\t";
            }
            cout << endl;
        }
    }
};

int learning(Neuron_Web& web) { // Обучение нейросети
    ifstream file;
    file.open("file.txt"); // Подключаем датасет

    if (!file) {
        cout << "Ошибка открытия файла\n";
        return 0;
    }

    vector <vector <float>> data;
    string name;
    float value;

    while (!file.eof()) { // Сохраняем в матрицу data данные для обучения
        vector <float> values;
        value = 0;
        name = "";
        for (int i = 0; i < 4; i++) { // Считываем входные параметры
            file >> value;
            values.push_back(value);
        }
        file >> name;
        if (name == "setosa") { // Считываем вид Ириса для считанных входных параметров
            values.push_back(0.8);
            values.push_back(0.2);
            values.push_back(0.2);
        }
        else if (name == "versicolor") {
            values.push_back(0.2);
            values.push_back(0.8);
            values.push_back(0.2);
        }
        else if (name == "virginica") {
            values.push_back(0.2);
            values.push_back(0.2);
            values.push_back(0.8);
        }
        else if (name != "") {
            cout << "Ошибка в названии растения\n";
            return 0;
        }
        data.push_back(values);
    }
    file.close();
    for (int i = 0; i < data.size(); i++) { // Выводим на экран матрицу данных для обучения
        for (int k = 0; k < data[i].size(); k++) {
            cout << data[i][k] << "\t";
        }
        cout << "\n";
    }

    float error = 1;
    int j = 0;

    float a[4] = { 0, 0, 0, 0 };
    float b[3] = { 0.2, 0.2, 0.2 };

    while (error >= 0.00001) { // Обучаем нейросеть, пока сумма квадратов ошибок на выходных нейронах по всем примерам не меньше, чем заданное значение ошибки
        j++;
        error = 0;
        for (int i = 0; i < data.size(); i++) {
            for (int k = 0; k < 4; k++)
                a[k] = data[i][k];
            for (int k = 0; k < 3; k++)
                b[k] = data[i][k + 4];
            error += web.go_check_error(a, b);
        }

        if (j % 100000 == 0) cout << error << endl; // Выводим в консоль сумму квадратов ошибок каждой 100.000-ой эпохи
    }

    return 1;
}

int main()
{
    setlocale(LC_ALL, "RU");

    Neuron_Web web; // Создаём нейросеть

    web.print();
    float a[4] = { 0, 0, 0, 0 };
    float b[3] = { 0.2, 0.2, 0.2 };
    web.go(a, b); // Тестовый проход по нейросети
    web.print();

    if (!learning(web)) { // Обучаем нейросеть
        cout << "Обучение не было завершено\n";
        return 0;
    }

    web.print(); // Выводим значения весов и слоёв

    cout << "\nОбучение завершено\n\n\n\n\n\n\n";

    while (true) { // Пользуемся обученной нейросетью
        cout << "Введите данные: "; // В коносль вводим входные данные через пробел либо через tab, например: 4.8 3.4 1.6 0.2
        for (int i = 0; i < 4; i++) {
            cin >> a[i];
        }

        web.go(a); // Пробегаем по нейросети с введёнными входными данными
        web.print(); // Выводим значения весов и слоёв

        if (web.X3[0][0] > 0.7) cout << "Setosa\n\n"; // Проверяем, под какой вид попадает предположение нейросети (я обучал её так, чтобы на выходном нейроне неправильного вида было значение 0.2, а правильного - 0.8)
        else if (web.X3[1][0] > 0.7) cout << "Versicolor\n\n";
        else if (web.X3[2][0] > 0.7) cout << "Virginica\n\n";
        else cout << "Не удалось однозначно определить вид Ириса\n\n";
    }
}