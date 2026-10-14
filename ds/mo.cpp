// =====================================================================
// ds/mo.cpp  Mo's algorithm（区間クエリをまとめて処理）
//   「区間 [l, r) の答え」を聞くクエリが Q 個あり、値の更新がないとき、
//   区間の端を 1 つ伸ばす・縮めるだけで答えを更新できれば、全体 O((N + Q) √N) で全部に答えられる
//   クエリを先に全部読んで並べ替えるので、オフライン（答える順番を自由にしてよい）のとき専用
//   典型: 区間の種類数、区間で同じ値の組の数、区間で出現回数が条件を満たす値の個数
// 依存: なし
// ABC / ARC での使用例:
//   ABC242 G - Range Pairing Query  https://atcoder.jp/contests/abc242/tasks/abc242_g
//     → 色ごとの個数を持ち、ペアの数 Σ⌊個数/2⌋ を add / erase で更新
//   ABC293 G - Triple Index  https://atcoder.jp/contests/abc293/tasks/abc293_g
//     → 値ごとの個数 c を持ち、C(c, 3) の和を add / erase で更新
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// Mo mo(n);
//   mo.add_query(l, r)        区間 [l, r) のクエリを追加（追加した順に 0, 1, 2, ... の番号）
//   mo.run(add, erase, out)   すべてのクエリを処理
// 【自分で定義するもの】（main の中にラムダで書く。区間の状態はラムダの外の変数に持つ）
//   add(i)   : 位置 i が区間に入った（状態を更新）
//   erase(i) : 位置 i が区間から出た（状態を更新）
//   out(q)   : 今の区間がクエリ q の区間になっている。答えを ans[q] に記録する
// 左から入る・右から入るで処理を変える必要があるときは run4 を使う
struct Mo {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/mo.cpp
    int n;
    vector<pair<int, int>> qs;
    Mo(int n_) : n(n_) {}
    void add_query(int l, int r) { qs.push_back({l, r}); }

    template <class AL, class AR, class EL, class ER, class Out>
    void run4(AL add_left, AR add_right, EL erase_left, ER erase_right, Out out) {
        int q = qs.size();
        int B = max(1, (int)(n / max(1.0, sqrt(q))));  // ★ ブロック幅 ≈ N / √Q
        vector<int> ord(q);
        iota(ord.begin(), ord.end(), 0);
        sort(ord.begin(), ord.end(), [&](int a, int b) {
            int ba = qs[a].first / B, bb = qs[b].first / B;
            if (ba != bb) return ba < bb;                                          // ★ 左端のブロック順
            return (ba & 1) ? qs[a].second > qs[b].second : qs[a].second < qs[b].second;  // ★ ブロック内は右端順（交互に向きを変えると速い）
        });
        int l = 0, r = 0;  // 今の区間 [l, r)
        for (int i : ord) {
            auto [ql, qr] = qs[i];
            while (l > ql) add_left(--l);     // ★ 先に広げてから
            while (r < qr) add_right(r++);
            while (l < ql) erase_left(l++);   // ★ 後で縮める（区間が「負の長さ」にならない）
            while (r > qr) erase_right(--r);
            out(i);
        }
    }
    template <class Add, class Erase, class Out>
    void run(Add add, Erase erase, Out out) { run4(add, add, erase, erase, out); }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 区間の種類数
    vector<int> a = {1, 2, 1, 3, 2, 2};
    Mo mo(a.size());
    vector<pair<int, int>> q = {{0, 3}, {1, 6}, {2, 2}, {0, 6}};
    for (auto [l, r] : q) mo.add_query(l, r);
    vector<int> cnt(4, 0), ans(q.size());
    int kinds = 0;
    mo.run([&](int i) { if (cnt[a[i]]++ == 0) kinds++; },
           [&](int i) { if (--cnt[a[i]] == 0) kinds--; },
           [&](int qi) { ans[qi] = kinds; });
    assert((ans == vector<int>{2, 3, 0, 3}));

    // ランダムテスト: 種類数と「同じ値の組の数」を愚直と比較
    mt19937 rr(124);
    for (int it = 0; it < 300; it++) {
        int n = rr() % 30 + 1, Q = rr() % 40 + 1;
        vector<int> b(n);
        for (auto& x : b) x = rr() % 5;
        Mo m2(n);
        vector<pair<int, int>> qq;
        for (int i = 0; i < Q; i++) {
            int l = rr() % (n + 1), r = rr() % (n + 1);
            if (l > r) swap(l, r);
            qq.push_back({l, r}), m2.add_query(l, r);
        }
        vector<long long> c(5, 0), res1(Q), res2(Q);
        long long kd = 0, pairs = 0;
        m2.run([&](int i) { pairs += c[b[i]]; if (c[b[i]]++ == 0) kd++; },
               [&](int i) { if (--c[b[i]] == 0) kd--; pairs -= c[b[i]]; },
               [&](int qi) { res1[qi] = kd, res2[qi] = pairs; });
        for (int i = 0; i < Q; i++) {
            auto [l, r] = qq[i];
            set<int> s;
            long long p = 0;
            for (int x = l; x < r; x++) { s.insert(b[x]); for (int y = x + 1; y < r; y++) p += b[x] == b[y]; }
            assert(res1[i] == (long long)s.size() && res2[i] == p);
        }
    }
    puts("OK");
}
