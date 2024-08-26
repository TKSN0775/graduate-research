#include <iostream>
#include <string>
#include <vector>
#include <complex>
#include <math.h>

using namespace std;

// ゲートを定義
vector<vector<complex<double>>> H = { // Hゲート
    {1 / sqrt(2), 1 / sqrt(2)},
    {1 / sqrt(2), -1 / sqrt(2)}};
vector<vector<complex<double>>> X = { // Xゲート
    {0, 1},
    {1, 0}};
vector<vector<complex<double>>> I = { // Iゲート
    {1, 0},
    {0, 1}};
vector<vector<complex<double>>> E = { // |0><0|
    {1, 0},
    {0, 0}};
vector<vector<complex<double>>> F = { // |1><1|
    {0, 0},
    {0, 1}};

// 内積 n次元のベクトルv1,v2の内積を返す
complex<double> inner_product(vector<complex<double>> v1, vector<complex<double>> v2, int N)
{
    complex<double> sum = 0;
    for (int i = 0; i < N; i++)
        sum += v1[i] * v2[i];
    return sum;
}

// ベクトルと行列の積 NxN行列とN次元ベクトルvの積をとり、vに格納する
void product(vector<complex<double>> &v, vector<vector<complex<double>>> U, int N)
{
    vector<complex<double>> v_new(N, 0); // N次元ベクトルを生成
    for (int i = 0; i < N; i++)
        v_new[i] = inner_product(v, U[i], N);
    v = v_new;
}

// 2つの行列のテンソル積を計算(Aに格納)
vector<vector<complex<double>>> tensor(vector<vector<complex<double>>> A, vector<vector<complex<double>>> B)
{
    int n = A[0].size();
    vector<vector<complex<double>>> C(2 * n, vector<complex<double>>(2 * n));
    for (int i = 0; i < 2 * n; i++)
        for (int j = 0; j < 2 * n; j++)
            C[i][j] = A[(i - (i % 2)) >> 1][(j - (j % 2)) >> 1] * B[i % 2][j % 2];
    return C;
}

// 文字に対応する行列を返す
vector<vector<complex<double>>> setMatrix(char a)
{
    if (a == 'H')
        return H;
    if (a == 'X')
        return X;
    if (a == 'I')
        return I;
    if (a == 'E')
        return E;
    if (a == 'F')
        return F;
    return I;
}

// ゲート文字列から対応する行列を返す
vector<vector<complex<double>>> getMatrix(string gate)
{
    // 制御NOTの場所を格納する
    int x = -1;
    int dot;
    // 制御NOTゲートを判断
    for (int i = 0; i < gate.length(); i++)
    {
        if (isdigit(gate[i])) // i文字目が数字のとき->制御NOT
        {
            x = i;               // 標的ビットを格納
            dot = gate[i] - '0'; // 制御ビットを格納
        }
    }
    if (x == -1) // 制御NOTがない->普通にテンソル積
    {
        vector<vector<complex<double>>> A = setMatrix(gate[0]);
        for (int i = 1; i < gate.length(); i++)
        {
            vector<vector<complex<double>>> B = setMatrix(gate[i]);
            A = tensor(A, B);
        }
        return A;
    }
    else // 制御NOTがある
    {
        // 制御NOTの式に基づいて、行列を生成
        vector<vector<complex<double>>> A;
        vector<vector<complex<double>>> B;
        string gate_A(gate.length(), 'I');
        string gate_B(gate.length(), 'I');
        gate_A[dot] = 'E';
        gate_B[dot] = 'F';
        gate_B[x] = 'X';
        A = getMatrix(gate_A);
        B = getMatrix(gate_B);

        for (int i = 0; i < A[0].size(); i++)
        {
            for (int j = 0; j < A[0].size(); j++)
            {
                A[i][j] += B[i][j];
            }
        }
        return A;
    }
}

// 初期値と回路から量子計算を行う
void qsim(vector<complex<double>> &q, vector<string> L, int n)
{
    for (int i = 0; i < L[0].length(); i++)
    {
        // ゲートのi列でゲート文字列gateを生成
        string gate = "";
        for (int j = 0; j < n; j++)
            gate += L[j][i];
        // ゲート文字列から行列を生成
        vector<vector<complex<double>>> matrix = getMatrix(gate);
        // 量子ビットのベクトルにゲートをかける
        product(q, matrix, pow(2, n));
    }
}

int main()
{
    int i, j, n;
    // ビット数nを入力
    cout << "How many bits? n = ";
    cin >> n;

    // 量子ビットの初期値を入力 ex) 4bit: 0001
    string q;
    cout << "Enter the value of qbit. = ";
    cin >> q;

    // 回路を表す文字列を入力(行ごとに) ex) HXHI0
    vector<string> L(n);
    cout << "Enter the circuit." << endl;
    for (i = 0; i < n; i++)
    {
        cout << "What is the circuit in the row " << i << "? = ";
        cin >> L[i];
    }

    // 初期値から、量子ビットのベクトルを生成(2^n次元)
    // 初期値は0,1のみのため、1つの要素のみ1になる
    vector<complex<double>> v(pow(2, n), 0);
    int num = stoi(q, nullptr, 2);
    v[num] = 1;

    // qsimに渡して実行
    qsim(v, L, n);

    // 出力
    cout << "result:" << endl;
    for (auto &num : v)
        cout << num << endl;

    // 確率を出力
    cout << "probability:" << endl;
    for (auto &num : v)
        cout << pow(abs(num), 2) << ", ";
}