// =====================================================================
// basic/imos.cpp  いもす法
//   区間（矩形）にまとめて値を足し、最後に全体を一括で得る。add O(1)、build O(n)
//   2 つのブロックは互いに独立
// 依存: なし（座標が大きいときは 04_compress と併用）
// ABC / ARC での使用例:
//   ABC035 C - オセロ  https://atcoder.jp/contests/abc035/tasks/abc035_c
//     → 区間に +1 して最後に偶奇を見る
//   ABC014 C - AtColor  https://atcoder.jp/contests/abc014/tasks/abc014_3
//     → 区間に +1 して最大値を取る
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 1 次元いもす ----------
// add(l, r, x) : [l, r) に x を足す（範囲外は切り捨て）
// build()      : 最終的な配列
template <class T>
struct Imos {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/imos.cpp
    int n;
    vector<T> d;
    Imos(int n_) : n(n_), d(n_ + 1, T(0)) {}
    void add(int l, int r, T x) {
        l = max(l, 0), r = min(r, n);
        if (l >= r) return;
        d[l] += x;  // ★ 始まりに +x
        d[r] -= x;  // ★ 終わりの次に -x
    }
    vector<T> build() const {
        vector<T> res(n);
        T cur = T(0);
        for (int i = 0; i < n; i++) res[i] = (cur += d[i]);  // ★ 最後に累積和
        return res;
    }
};

// ---------- 本体B: 2 次元いもす ----------
// add(r1, c1, r2, c2, x) : 行 [r1, r2) × 列 [c1, c2) に x を足す
template <class T>
struct Imos2D {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/imos.cpp
    int h, w;
    vector<vector<T>> d;
    Imos2D(int h_, int w_) : h(h_), w(w_), d(h_ + 1, vector<T>(w_ + 1, T(0))) {}
    void add(int r1, int c1, int r2, int c2, T x) {
        r1 = max(r1, 0), c1 = max(c1, 0), r2 = min(r2, h), c2 = min(c2, w);
        if (r1 >= r2 || c1 >= c2) return;
        d[r1][c1] += x, d[r1][c2] -= x, d[r2][c1] -= x, d[r2][c2] += x;  // ★ 4 隅に ±x
    }
    vector<vector<T>> build() const {
        auto t = d;
        for (int i = 0; i <= h; i++)
            for (int j = 0; j < w; j++) t[i][j + 1] += t[i][j];  // ★ 横に累積和
        for (int i = 0; i < h; i++)
            for (int j = 0; j <= w; j++) t[i + 1][j] += t[i][j];  // ★ 縦に累積和
        vector<vector<T>> res(h, vector<T>(w));
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = t[i][j];
        return res;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    Imos<int> im(5);
    im.add(1, 3, 2);
    im.add(-5, 2, 1);  // 範囲外は切り捨て
    assert((im.build() == vector<int>{1, 3, 2, 0, 0}));

    Imos2D<int> i2(3, 3);
    i2.add(0, 0, 2, 2, 1);
    i2.add(1, 1, 5, 5, 1);
    auto g = i2.build();
    assert(g[0][0] == 1 && g[1][1] == 2 && g[2][2] == 1 && g[0][2] == 0);
    // ランダムテスト: 愚直に配列へ足したものと比較（範囲外の指定も含む）
    {
        mt19937 rr(104);
        for (int it = 0; it < 2000; it++) {
            int n = rr() % 10 + 1;
            Imos<long long> im(n);
            vector<long long> ref(n, 0);
            for (int q = rr() % 10; q > 0; q--) {
                int l = (int)(rr() % (n + 4)) - 2, r = (int)(rr() % (n + 4)) - 2;
                long long x = (long long)(rr() % 11) - 5;
                im.add(l, r, x);
                for (int i = max(l, 0); i < min(r, n); i++) ref[i] += x;
            }
            assert(im.build() == ref);
            int h = rr() % 5 + 1, w = rr() % 5 + 1;
            Imos2D<long long> i2(h, w);
            vector<vector<long long>> R2(h, vector<long long>(w, 0));
            for (int q = rr() % 8; q > 0; q--) {
                int r1 = (int)(rr() % (h + 3)) - 1, c1 = (int)(rr() % (w + 3)) - 1, r2 = (int)(rr() % (h + 3)) - 1, c2 = (int)(rr() % (w + 3)) - 1;
                long long x = (long long)(rr() % 11) - 5;
                i2.add(r1, c1, r2, c2, x);
                for (int i = max(r1, 0); i < min(r2, h); i++)
                    for (int j = max(c1, 0); j < min(c2, w); j++) R2[i][j] += x;
            }
            assert(i2.build() == R2);
        }
    }
    puts("OK");
}
