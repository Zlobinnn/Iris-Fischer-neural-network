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
    float W01[5][4], W12[5][3], W23[4][3], X0[5][2], X1[5][2], X2[4][2], X3[3][2];
    Neuron_Web() {
        random(*W01, size_X0, size_X1 - 1);
        random(*W12, size_X1, size_X2 - 1);
        random(*W23, size_X2, size_X3);
        X0[size_X0 - 1][0] = 1;
        X1[size_X1 - 1][0] = 1;
        X2[size_X2 - 1][0] = 1;
    }

    void go(float* a, float* b) {
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
    }

    float go_check_error(float* a, float* b) {
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

    void go(float* a) {
        for (int i = 0; i < size_X0 - 1; i++) {
            X0[i][0] = a[i];
        }

        probeg(*X0, *X1, *W01, size_X0, size_X1 - 1);
        probeg(*X1, *X2, *W12, size_X1, size_X2 - 1);
        probeg(*X2, *X3, *W23, size_X2, size_X3);
    }

    void error(float* x1, float* x2, float* w, int size1, int size2) {
        for (int i = 0; i < size1; i++) {
            x1[i * 2 + 1] = 0;
            for (int k = 0; k < size2; k++) {
                x1[i * 2 + 1] += x2[k * 2 + 1] * w[k + i * size2];
            }
        }
    }

    void probeg(float* x1, float* x2, float* w, int size1, int size2) {
        for (int i = 0; i < size2; i++) {
            x2[i * 2] = 0;
            for (int k = 0; k < size1; k++) {
                x2[i * 2] += x1[k * 2] * w[k * size2 + i];
            }
            x2[i * 2] = 1 / (1 + exp(-1 * x2[i * 2]));
        }
    }

    void fix(float* x1, float* x2, float* w, int size1, int size2) {
        for (int i = 0; i < size1; i++) {
            for (int k = 0; k < size2; k++) {
                w[k + i * size2] += 0.1 * x2[k * 2 + 1] * x1[i * 2] * x2[k * 2] * (1 - x2[k * 2]);
            }
        }
    }


    void print() {
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

    void random(float* a, int size1, int size2) {
        for (int i = 0; i < size1; i++) {
            for (int k = 0; k < size2; k++) {
                a[i * size2 + k] = float(rand() % 10) / 10;
                //a[i * size2 + k] = i * size2 + k;
            }
        }
    }

    void print(float* a, int size1, int size2) {
        for (int i = 0; i < size1; i++) {
            for (int k = 0; k < size2; k++) {
                cout << a[i * size2 + k] << "\t";
            }
            cout << endl;
        }
    }
};

int learning(Neuron_Web& web) {
    ifstream file;
    file.open("file.txt");

    if (!file) {
        cout << "Ошибка открытия файла\n";
        return 0;
    }

    vector <vector <float>> data;
    string name;
    float value;

    while (!file.eof()) {
        vector <float> values;
        value = 0;
        name = "";
        for (int i = 0; i < 4; i++) {
            file >> value;
            values.push_back(value);
        }
        file >> name;
        if (name == "setosa") {
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
    for (int i = 0; i < data.size(); i++) {
        for (int k = 0; k < data[i].size(); k++) {
            cout << data[i][k] << "\t";
        }
        cout << "\n";
    }

    float error = 1;
    int j = 0;

    float a[4] = { 0, 0, 0, 0 };
    float b[3] = { 0.2, 0.2, 0.2 };

    while (error >= 0.00001) {
        j++;
        error = 0;
        for (int i = 0; i < data.size(); i++) {
            for (int k = 0; k < 4; k++)
                a[k] = data[i][k];
            for (int k = 0; k < 3; k++)
                b[k] = data[i][k + 4];
            error += web.go_check_error(a, b);
        }

        if (j % 100000 == 0) cout << error << endl;
    }

    return 1;
}

int main()
{
    setlocale(LC_ALL, "RU");

    Neuron_Web web;

    web.print();
    float a[4] = { 0, 0, 0, 0 };
    float b[3] = { 0.2, 0.2, 0.2 };
    web.go(a, b);
    web.print();

    if (!learning(web)) {
        cout << "Обучение не было завершено\n";
        return 0;
    }

    web.print();

    cout << "\nОбучение завершено\n\n\n\n\n\n\n";

    while (true) {
        cout << "Введите данные: ";
        for (int i = 0; i < 4; i++) {
            cin >> a[i];
        }

        web.go(a);
        web.print();

        if (web.X3[0][0] > 0.7) cout << "Setosa\n\n";
        else if (web.X3[1][0] > 0.7) cout << "Versicolor\n\n";
        else if (web.X3[2][0] > 0.7) cout << "Virginica\n\n";
        else cout << "Не удалось однозначно определить вид ириса\n\n";
    }
}