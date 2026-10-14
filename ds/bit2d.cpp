// =====================================================================
// ds/bit2d.cpp  2 次元 BIT（Fenwick Tree）
//   H×W のグリッドで「1 点に足す」と「長方形の和」を、値の更新と質問が混ざっていても O(log H log W)
//   ※ 更新がなく最初に値が全部わかっているなら basic/prefix_sum.cpp の PrefixSum2D（O(1)）で十分
//   ※ 1 次元なら ACL の fenwick_tree
// 依存: なし
// ABC / ARC での使用例:
//   ABC106 D - AtCoder Express 2  https://atcoder.jp/contests/abc106/tasks/abc106_d
//     → (L, R) に点を足して長方形の和（更新がないので PrefixSum2D で十分だが、BIT2D でも解ける）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// T : 値の型（long long など）
// BIT2D<long long> bit(H, W);
//   bit.add(i, j, x)               マス (i, j) に x を足す（0-indexed）
//   bit.sum(r1, c1, r2, c2)        行 [r1, r2) × 列 [c1, c2) の和
//   bit.prefix(r, c)               行 [0, r) × 列 [0, c) の和
// 典型: 「点の追加」と「長方形の中の点の個数」が交互に来る。座標が大きいなら座標圧縮してから
template <class T>
struct BIT2D {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/bit2d.cpp
    int h, w;
    vector<vector<T>> d;  // 1-indexed で持つ
    BIT2D(int h_, int w_) : h(h_), w(w_), d(h_ + 1, vector<T>(w_ + 1, T(0))) {}
    void add(int i, int j, T x) {
        for (int a = i + 1; a <= h; a += a & -a)      // ★ 行方向に BIT
            for (int b = j + 1; b <= w; b += b & -b)  // ★ その中で列方向に BIT
                d[a][b] += x;
    }
    T prefix(int r, int c) const {
        T s = T(0);
        for (int a = r; a > 0; a -= a & -a)
            for (int b = c; b > 0; b -= b & -b) s += d[a][b];
        return s;
    }
    T sum(int r1, int c1, int r2, int c2) const {
        if (r1 >= r2 || c1 >= c2) return T(0);
        return prefix(r2, c2) - prefix(r1, c2) - prefix(r2, c1) + prefix(r1, c1);  // ★ 2 次元累積和と同じ包除原理
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    BIT2D<long long> bit(3, 4);
    bit.add(0, 0, 5), bit.add(1, 2, 3), bit.add(2, 3, -1);
    assert(bit.sum(0, 0, 3, 4) == 7 && bit.sum(1, 1, 3, 3) == 3 && bit.prefix(1, 1) == 5 && bit.sum(2, 2, 1, 1) == 0);

    // ランダムテスト: 更新と長方形の和を愚直な 2 次元配列と比較
    mt19937 rr(122);
    for (int it = 0; it < 500; it++) {
        int h = rr() % 6 + 1, w = rr() % 6 + 1;
        BIT2D<long long> b(h, w);
        vector<vector<long long>> g(h, vector<long long>(w, 0));
        for (int q = 0; q < 50; q++) {
            if (rr() % 2) {
                int i = rr() % h, j = rr() % w;
                long long x = (long long)(rr() % 21) - 10;
                b.add(i, j, x), g[i][j] += x;
            } else {
                int r1 = rr() % (h + 1), r2 = rr() % (h + 1), c1 = rr() % (w + 1), c2 = rr() % (w + 1);
                long long s = 0;
                for (int i = r1; i < r2; i++) for (int j = c1; j < c2; j++) s += g[i][j];
                assert(b.sum(r1, c1, r2, c2) == s);
            }
        }
    }
    puts("OK");
}
