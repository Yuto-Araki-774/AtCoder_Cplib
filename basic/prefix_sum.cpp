// =====================================================================
// basic/prefix_sum.cpp  累積和・累積 min/max など
//   区間和を O(1)。「i 番目以外の min」なども O(1)
//   3 つのブロックは互いに独立。必要なものだけコピーしてよい
// 依存: なし
// ABC / ARC での使用例:
//   ABC122 C - GeT AC  https://atcoder.jp/contests/abc122/tasks/abc122_c
//     → "AC" の出現位置の累積和で区間クエリ（PrefixSum）
//   ABC125 C - GCD on Blackboard  https://atcoder.jp/contests/abc125/tasks/abc125_c
//     → 1 つを除いた gcd の最大（PrefixFold の except）
//   ABC106 D - AtCoder Express 2  https://atcoder.jp/contests/abc106/tasks/abc106_d
//     → (L, R) の 2 次元累積和（PrefixSum2D）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 1 次元累積和 ----------
// T : + と - ができる型（int, long long, modint）
// sum(l, r) : a[l] + ... + a[r-1]
// ◆ struct PrefixSum : 1 次元累積和
//   PrefixSum(a)    : 前計算 O(N)
//   sum(l, r)       : a[l] + … + a[r−1]。O(1)。l >= r なら 0
//   prefix(r)       : a[0] + … + a[r−1]
template <class T>
struct PrefixSum {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/prefix_sum.cpp
    vector<T> s;  // s[i] = a[0] + ... + a[i-1]
    // PrefixSum() : 空で作る（後で代入して使う用）
    PrefixSum() {}
    // a : 元の配列（T は + と − ができる型。long long や mint）
    PrefixSum(const vector<T>& a) : s(a.size() + 1, T(0)) {
        for (int i = 0; i < (int)a.size(); i++) s[i + 1] = s[i] + a[i];  // ★ 前から足していく
    }
    // l, r : 区間 [l, r)。0 <= l, r <= N であること（範囲外は未定義）
    T sum(int l, int r) const { return l >= r ? T(0) : s[r] - s[l]; }  // ★ 引き算で区間を取り出す
    // r : 0..N。a[0..r) の和
    T prefix(int r) const { return s[r]; }
};

// ---------- 本体B: 累積 fold（min / max / gcd / xor / 積 など、引き算できない演算） ----------
// 【自分で定義するもの】
//   S op(S a, S b) : 結合則 op(op(a,b),c) == op(a,op(b,c)) を満たす演算
//   S e()          : 単位元 op(e(), a) == op(a, e()) == a
//   例 min: int op(int a, int b){ return min(a, b); }  int e(){ return INT_MAX; }
//      max → e = INT_MIN、gcd → 0、xor → 0、積 → 1
//   ※ op と e は main の外（グローバル）に普通の関数として書く。ラムダや main 内の定義は不可
//     使うとき: PrefixFold<int, op, e> pf(a);   （S は要素の型を書く）
// 任意区間はできない（それは Sparse Table / ACL segtree）。取れるのは前・後ろ・「一部を除いた全体」
// ◆ struct PrefixFold : 引き算できない演算（min, max, gcd, xor, 積 など）の前からの累積・後ろからの累積
//   PrefixFold(a)        : 前計算 O(N)
//   prefix(r)            : op(a[0..r))
//   suffix(l)            : op(a[l..N))
//   all()                : 全体
//   except(i)            : a[i] だけを除いた全体
//   except_range(l, r)   : [l, r) を除いた全体
template <class S, S (*op)(S, S), S (*e)()>
struct PrefixFold {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/prefix_sum.cpp
    int n;
    vector<S> pre, suf;  // pre[i] = op(a[0..i)),  suf[i] = op(a[i..n))
    // PrefixFold() : 空で作る（後で代入して使う用）
    PrefixFold() : n(0) {}
    // a : 元の配列
    PrefixFold(const vector<S>& a) : n(a.size()), pre(n + 1, e()), suf(n + 1, e()) {
        for (int i = 0; i < n; i++) pre[i + 1] = op(pre[i], a[i]);       // ★ 前から
        for (int i = n - 1; i >= 0; i--) suf[i] = op(a[i], suf[i + 1]);  // ★ 後ろから
    }
    // r : 0..N
    S prefix(int r) const { return pre[r]; }  // a[0..r)
    // l : 0..N
    S suffix(int l) const { return suf[l]; }  // a[l..n)
    // all() : 全体 op(a[0..N))
    S all() const { return pre[n]; }
    // i : 除く位置 0..N−1
    S except(int i) const { return op(pre[i], suf[i + 1]); }            // ★ a[i] を除いた全体 = 左側 op 右側
    // l, r : 除く区間 [l, r)。0 <= l <= r <= N
    S except_range(int l, int r) const { return op(pre[l], suf[r]); }  // [l, r) を除いた全体
};

// ---------- 本体C: 2 次元累積和 ----------
// sum(r1, c1, r2, c2) : 行 [r1, r2) × 列 [c1, c2) の和
// ◆ struct PrefixSum2D : 2 次元累積和
//   PrefixSum2D(a)          : 前計算 O(HW)
//   sum(r1, c1, r2, c2)     : 行 [r1, r2) × 列 [c1, c2) の和。O(1)
template <class T>
struct PrefixSum2D {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/prefix_sum.cpp
    vector<vector<T>> s;  // s[i][j] = 行 [0, i) × 列 [0, j) の和
    // PrefixSum2D() : 空で作る（後で代入して使う用）
    PrefixSum2D() {}
    // a : H 行 W 列の配列（各行の長さはそろっていること）
    PrefixSum2D(const vector<vector<T>>& a) {
        int h = a.size(), w = a.empty() ? 0 : a[0].size();
        s.assign(h + 1, vector<T>(w + 1, T(0)));
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                s[i + 1][j + 1] = s[i][j + 1] + s[i + 1][j] - s[i][j] + a[i][j];  // ★ 上 + 左 - 左上 + 自分
    }
    // r1, r2 : 行の範囲 [r1, r2)、c1, c2 : 列の範囲 [c1, c2)。空なら 0。範囲外は未定義
    T sum(int r1, int c1, int r2, int c2) const {
        if (r1 >= r2 || c1 >= c2) return T(0);
        return s[r2][c2] - s[r1][c2] - s[r2][c1] + s[r1][c1];  // ★ 包除原理
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int op_min(int a, int b) { return min(a, b); }
int e_min() { return INT_MAX; }
int main() {
    PrefixSum<long long> ps(vector<long long>{1, 2, 3, 4});
    assert(ps.sum(1, 3) == 5 && ps.sum(2, 2) == 0);

    PrefixFold<int, op_min, e_min> pf({5, 1, 7, 3});
    assert(pf.except(1) == 3 && pf.all() == 1 && pf.except_range(1, 4) == 5);

    PrefixSum2D<int> p2({{1, 2}, {3, 4}});
    assert(p2.sum(0, 0, 2, 2) == 10 && p2.sum(1, 0, 2, 2) == 7);
    // ランダムテスト: すべての区間を愚直に計算して比較
    {
        mt19937 rr(105);
        for (int it = 0; it < 1000; it++) {
            int n = rr() % 10 + 1;
            vector<long long> a(n);
            vector<int> b(n);
            for (int i = 0; i < n; i++) a[i] = (long long)(rr() % 21) - 10, b[i] = rr() % 50;
            PrefixSum<long long> ps(a);
            PrefixFold<int, op_min, e_min> pf(b);
            for (int l = 0; l <= n; l++)
                for (int r = l; r <= n; r++) {
                    long long s = 0;
                    for (int i = l; i < r; i++) s += a[i];
                    assert(ps.sum(l, r) == s);
                    int ex = INT_MAX;
                    for (int i = 0; i < n; i++) if (i < l || i >= r) ex = min(ex, b[i]);
                    assert(pf.except_range(l, r) == ex);
                }
            for (int i = 0; i < n; i++) {
                int ex = INT_MAX, pre = INT_MAX, suf = INT_MAX;
                for (int j = 0; j < n; j++) if (j != i) ex = min(ex, b[j]);
                for (int j = 0; j < i; j++) pre = min(pre, b[j]);
                for (int j = i; j < n; j++) suf = min(suf, b[j]);
                assert(pf.except(i) == ex && pf.prefix(i) == pre && pf.suffix(i) == suf);
            }
            int h = rr() % 4 + 1, w = rr() % 4 + 1;
            vector<vector<long long>> g(h, vector<long long>(w));
            for (auto& row : g) for (auto& x : row) x = (long long)(rr() % 21) - 10;
            PrefixSum2D<long long> p2(g);
            for (int r1 = 0; r1 <= h; r1++)
                for (int r2 = 0; r2 <= h; r2++)
                    for (int c1 = 0; c1 <= w; c1++)
                        for (int c2 = 0; c2 <= w; c2++) {
                            long long s = 0;
                            for (int i = r1; i < r2; i++) for (int j = c1; j < c2; j++) s += g[i][j];
                            assert(p2.sum(r1, c1, r2, c2) == s);
                        }
        }
    }
    puts("OK");
}
