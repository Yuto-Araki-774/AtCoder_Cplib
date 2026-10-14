// =====================================================================
// math/matrix.cpp  行列と行列累乗
//   N×N 行列の K 乗を O(N^3 log K) で求める。K が 1e18 でもよい
//   典型: 漸化式（フィボナッチなど）の第 K 項、「K 歩で i から j に行く方法の数」
// 依存: なし（mod を取るなら ACL の modint を T に使う）
// ABC / ARC での使用例:
//   ABC293 E - Geometric Progression  https://atcoder.jp/contests/abc293/tasks/abc293_e
//     → (和, A^i) を 2×2 行列で X 乗。M が素数とは限らないので T に atcoder::modint（set_mod）を使う
//   EDPC R - Walk  https://atcoder.jp/contests/dp/tasks/dp_r
//     → 隣接行列の K 乗（ABC ではないが典型）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/modint>
using namespace std;
using mint = atcoder::modint998244353;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// T : 要素の型（mint 推奨。long long だと掛け算ですぐあふれる）
// Matrix<mint> A(n, m);  A[i][j] = ...;      // n 行 m 列、0 で初期化
// Matrix<mint>::identity(n)                   // 単位行列
// A * B, A.pow(k)                             // 積、累乗（A は正方行列）
// A.apply(v)                                  // 行列 × 列ベクトル
//
// 漸化式の使い方（例: a[i+2] = a[i+1] + a[i]）:
//   ( a[i+2] )   ( 1 1 ) ( a[i+1] )
//   ( a[i+1] ) = ( 1 0 ) ( a[i]   )   → (a[k+1], a[k]) = A^k (a[1], a[0])
template <class T>
struct Matrix {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/matrix.cpp
    int n, m;
    vector<vector<T>> a;
    Matrix(int n_, int m_) : n(n_), m(m_), a(n_, vector<T>(m_, T(0))) {}
    static Matrix identity(int n) {
        Matrix I(n, n);
        for (int i = 0; i < n; i++) I.a[i][i] = T(1);
        return I;
    }
    vector<T>& operator[](int i) { return a[i]; }
    const vector<T>& operator[](int i) const { return a[i]; }

    Matrix operator*(const Matrix& b) const {
        Matrix c(n, b.m);
        for (int i = 0; i < n; i++)
            for (int k = 0; k < m; k++)  // ★ i-k-j の順にするとメモリを順に読むので速い
                for (int j = 0; j < b.m; j++) c.a[i][j] += a[i][k] * b.a[k][j];
        return c;
    }
    Matrix pow(long long k) const {
        Matrix r = identity(n), x = *this;
        while (k > 0) {
            if (k & 1) r = r * x;  // ★ 数の繰り返し二乗と同じ形
            x = x * x;
            k >>= 1;
        }
        return r;
    }
    vector<T> apply(const vector<T>& v) const {
        vector<T> r(n, T(0));
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) r[i] += a[i][j] * v[j];
        return r;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // フィボナッチ F(0)=0, F(1)=1
    Matrix<mint> A(2, 2);
    A[0][0] = 1, A[0][1] = 1, A[1][0] = 1;
    auto fib = [&](long long k) { return A.pow(k).apply({1, 0})[1]; };  // (F(k+1), F(k))
    assert(fib(0).val() == 0 && fib(1).val() == 1 && fib(10).val() == 55 && fib(90).val() == 2880067194370816120ULL % 998244353);

    // K 歩で 0 から 2 に行く方法の数（有向グラフ 0→1, 1→2, 0→2, 2→0）
    Matrix<long long> G(3, 3);
    G[0][1] = G[1][2] = G[0][2] = G[2][0] = 1;
    auto G3 = G.pow(3);
    // 0→1→2→0, 0→2→0→1, 0→2→0→2 ... 手で数えた値と愚直な掛け算の比較
    Matrix<long long> P = Matrix<long long>::identity(3);
    for (int i = 0; i < 3; i++) P = P * G;
    assert(G3.a == P.a && G3[0][2] == 1 && G3[0][0] == 1);

    // 3 項間漸化式 a[i+3] = a[i+2] + 2 a[i+1] + 3 a[i]、a0=1, a1=1, a2=1
    Matrix<mint> T3(3, 3);
    T3[0][0] = 1, T3[0][1] = 2, T3[0][2] = 3, T3[1][0] = 1, T3[2][1] = 1;
    vector<mint> seq = {1, 1, 1};
    for (int i = 3; i < 50; i++) seq.push_back(seq[i - 1] + 2 * seq[i - 2] + 3 * seq[i - 3]);
    for (int k = 0; k + 2 < 50; k++) assert(T3.pow(k).apply({1, 1, 1})[2] == seq[k]);
    // ランダムテスト: 積を定義どおりに計算したもの、繰り返し掛けたものと比較
    {
        mt19937 rr(117);
        for (int it = 0; it < 500; it++) {
            int n = rr() % 4 + 1, m = rr() % 4 + 1, l = rr() % 4 + 1;
            Matrix<mint> X(n, m), Y(m, l);
            for (auto& row : X.a) for (auto& x : row) x = rr() % 10;
            for (auto& row : Y.a) for (auto& x : row) x = rr() % 10;
            auto Z = X * Y;
            for (int i = 0; i < n; i++)
                for (int j = 0; j < l; j++) {
                    mint s = 0;
                    for (int k = 0; k < m; k++) s += X[i][k] * Y[k][j];
                    assert(Z[i][j] == s);
                }
            Matrix<mint> S(n, n), P = Matrix<mint>::identity(n);
            for (auto& row : S.a) for (auto& x : row) x = rr() % 10;
            int K = rr() % 20;
            for (int k = 0; k < K; k++) P = P * S;
            assert(S.pow(K).a == P.a);
            vector<mint> v(n);
            for (auto& x : v) x = rr() % 10;
            auto av = S.apply(v);
            for (int i = 0; i < n; i++) {
                mint s = 0;
                for (int k = 0; k < n; k++) s += S[i][k] * v[k];
                assert(av[i] == s);
            }
        }
    }
    puts("OK");
}
