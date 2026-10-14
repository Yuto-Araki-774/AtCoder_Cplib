// =====================================================================
// basic/brute_force.cpp  全探索（bit 全探索・順列全探索・N 進数全探索・組合せ全探索・部分集合の列挙）
//   「全部試す」ループの書き方をまとめたもの。どれも互いに独立
//   目安: 2^N は N <= 20、N! は N <= 10、K^N は全体が 1e7 程度まで
// 依存: なし
// ABC / ARC での使用例:
//   ABC128 C - Switches  https://atcoder.jp/contests/abc128/tasks/abc128_c
//     → スイッチの on/off を bit 全探索（bit_search）
//   ABC145 C - Average Length  https://atcoder.jp/contests/abc145/tasks/abc145_c
//     → 町を回る順番を順列全探索（perm_search）
//   ABC119 C - Synthetic Kadomatsu  https://atcoder.jp/contests/abc119/tasks/abc119_c
//     → 各竹を A / B / C / 使わない の 4 通りで N 進数全探索（product_search）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: bit 全探索（N 個それぞれ「選ぶ / 選ばない」の 2^N 通り） ----------
// f(mask) が 2^n 回呼ばれる。i 番目を選んでいるかは (mask >> i & 1)
// 例: bit_search(n, [&](int mask) { long long s = 0; for (int i = 0; i < n; i++) if (mask >> i & 1) s += a[i]; ... });
template <class F>
void bit_search(int n, F f) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/brute_force.cpp
    for (int mask = 0; mask < (1 << n); mask++) f(mask);  // ★ 0 〜 2^n - 1 の 2 進数 = 選び方
}

// ---------- 本体B: 順列全探索（0, 1, ..., n-1 の並べ方 n! 通り） ----------
// f(p) が n! 回呼ばれる。p は {0, ..., n-1} の並べ替え（辞書順）
// 例: 巡回セールスマン（小さい N）、並べ方を全部試す問題
template <class F>
void perm_search(int n, F f) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/brute_force.cpp
    vector<int> p(n);
    iota(p.begin(), p.end(), 0);  // ★ ソート済みから始めないと全部列挙されない
    do f(p);
    while (next_permutation(p.begin(), p.end()));
}

// ---------- 本体C: N 進数全探索（各位置 i に 0 〜 base[i]-1 のどれかを入れる、Π base[i] 通り） ----------
// f(d) が呼ばれる。d[i] は 0 〜 base[i]-1
// 例: N 個をそれぞれ 3 グループのどれかに入れる → product_search(vector<int>(n, 3), f)
//     重複順列（1〜K の数字を N 個並べる）→ product_search(vector<int>(n, K), f)
template <class F>
void product_search(const vector<int>& base, F f) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/brute_force.cpp
    int n = base.size();
    vector<int> d(n, 0);
    while (true) {
        f(d);
        int i = n - 1;
        while (i >= 0 && ++d[i] == base[i]) d[i--] = 0;  // ★ 末尾から 1 足して繰り上がり
        if (i < 0) break;                                 // ★ 一番上まで繰り上がったら終わり
    }
}

// ---------- 本体D: 組合せ全探索（0 〜 n-1 から k 個選ぶ nCk 通り） ----------
// f(c) が呼ばれる。c は選んだ番号の昇順の列（辞書順）
template <class F>
void comb_search(int n, int k, F f) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/brute_force.cpp
    if (k < 0 || k > n) return;
    vector<int> c(k);
    iota(c.begin(), c.end(), 0);
    while (true) {
        f(c);
        int i = k - 1;
        while (i >= 0 && c[i] == n - k + i) i--;  // ★ もう増やせない位置を飛ばす
        if (i < 0) break;
        c[i]++;
        for (int j = i + 1; j < k; j++) c[j] = c[j - 1] + 1;  // ★ 後ろは詰めて並べ直す
    }
}

// ---------- 本体E: 部分集合の列挙（mask の部分集合をすべて。bit DP で使う） ----------
// f(sub) が mask の部分集合（mask 自身と 0 を含む）ごとに大きい順に呼ばれる
// すべての mask について回すと全体で 3^n 回
template <class F>
void subset_search(int mask, F f) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/brute_force.cpp
    for (int sub = mask;; sub = (sub - 1) & mask) {  // ★ 1 引いて mask と AND = 次に小さい部分集合
        f(sub);
        if (sub == 0) break;
    }
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // bit 全探索: 部分和が 10 になる選び方の数
    vector<int> a = {3, 5, 2, 7};
    int n = a.size(), ways = 0;
    bit_search(n, [&](int mask) {
        int s = 0;
        for (int i = 0; i < n; i++)
            if (mask >> i & 1) s += a[i];
        if (s == 10) ways++;
    });
    assert(ways == 2);  // {3, 5, 2}, {3, 7}

    // 順列全探索
    int cnt = 0;
    vector<int> last;
    perm_search(4, [&](const vector<int>& p) { cnt++, last = p; });
    assert(cnt == 24 && (last == vector<int>{3, 2, 1, 0}));

    // N 進数全探索: 3 つを 0/1/2 のグループに分ける 27 通り
    cnt = 0;
    set<vector<int>> st;
    product_search({3, 3, 3}, [&](const vector<int>& d) { cnt++, st.insert(d); });
    assert(cnt == 27 && st.size() == 27);
    cnt = 0;
    product_search({2, 1, 3}, [&](const vector<int>&) { cnt++; });
    assert(cnt == 6);

    // 組合せ全探索
    vector<vector<int>> cs;
    comb_search(4, 2, [&](const vector<int>& c) { cs.push_back(c); });
    assert(cs.size() == 6 && (cs.front() == vector<int>{0, 1}) && (cs.back() == vector<int>{2, 3}));
    cnt = 0;
    comb_search(5, 0, [&](const vector<int>& c) { cnt++, assert(c.empty()); });
    assert(cnt == 1);

    // 部分集合の列挙
    vector<int> subs;
    subset_search(0b1010, [&](int s) { subs.push_back(s); });
    assert((subs == vector<int>{0b1010, 0b1000, 0b0010, 0}));
    // ランダムテスト: 列挙の個数・重複なし・範囲を確認
    {
        mt19937 rr(102);
        for (int n = 0; n <= 10; n++) {
            set<int> s;
            int c = 0;
            bit_search(n, [&](int m) { c++, s.insert(m); });
            assert(c == (1 << n) && (int)s.size() == c);
        }
        for (int n = 1; n <= 6; n++) {
            set<vector<int>> s;
            vector<int> prev;
            int c = 0;
            perm_search(n, [&](const vector<int>& p) {
                c++;
                if (!prev.empty()) assert(prev < p);  // 辞書順
                prev = p, s.insert(p);
                vector<int> q = p;
                sort(q.begin(), q.end());
                for (int i = 0; i < n; i++) assert(q[i] == i);
            });
            int f = 1;
            for (int i = 2; i <= n; i++) f *= i;
            assert(c == f && (int)s.size() == f);
        }
        for (int it = 0; it < 300; it++) {
            int n = rr() % 5;
            vector<int> base(n);
            long long tot = 1;
            for (auto& b : base) b = rr() % 4 + 1, tot *= b;
            set<vector<int>> s;
            product_search(base, [&](const vector<int>& d) {
                for (int i = 0; i < n; i++) assert(0 <= d[i] && d[i] < base[i]);
                s.insert(d);
            });
            assert((long long)s.size() == tot);
        }
        for (int n = 0; n <= 8; n++)
            for (int k = 0; k <= n; k++) {
                set<vector<int>> s;
                comb_search(n, k, [&](const vector<int>& c) {
                    assert((int)c.size() == k);
                    for (int i = 0; i < k; i++) assert(0 <= c[i] && c[i] < n && (i == 0 || c[i - 1] < c[i]));
                    s.insert(c);
                });
                long long C = 1;
                for (int i = 0; i < k; i++) C = C * (n - i) / (i + 1);
                assert((long long)s.size() == C);
            }
        for (int it = 0; it < 300; it++) {
            int mask = rr() % (1 << 10), prev = INT_MAX;
            set<int> s;
            subset_search(mask, [&](int sub) { assert((sub & mask) == sub && sub < prev); prev = sub, s.insert(sub); });
            assert((int)s.size() == (1 << __builtin_popcount(mask)));
        }
    }
    puts("OK");
}
