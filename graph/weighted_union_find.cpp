// =====================================================================
// graph/weighted_union_find.cpp  重み付き UnionFind（ポテンシャル付き UnionFind）
//   「b は a より w 大きい」という情報を次々に追加し、矛盾チェックと 2 点の差を高速に求める
//   ほぼ O(1)（α(N)）。普通の連結判定だけなら ACL の dsu を使う
// 依存: なし
// ABC / ARC での使用例:
//   ABC087 D - People on a Line  https://atcoder.jp/contests/abc087/tasks/arc090_b
//     → x_R − x_L = D を merge し、矛盾したら No
//   ABC328 F - Good Set Query  https://atcoder.jp/contests/abc328/tasks/abc328_f
//     → merge が true を返したクエリの番号を出力
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// T : 差の型（long long など）
// WeightedUnionFind<long long> uf(n);
//   uf.merge(a, b, w)  「weight(b) - weight(a) = w」を追加。既存の情報と矛盾したら false（何もしない）
//   uf.same(a, b)      同じグループか
//   uf.diff(a, b)      weight(b) - weight(a)（same(a, b) のときだけ意味がある）
//   uf.size(a)         a のグループの大きさ
// 典型: 「x_b - x_a = w」という条件が M 個。全部同時に満たせるか / 2 点の差はいくつか
// ◆ struct WeightedUnionFind : 「b は a より w 大きい」という条件を足していき、矛盾と 2 点の差を調べる UnionFind
//   WeightedUnionFind(n) : 頂点 0..n−1、最初は全部ばらばら
//   merge(a, b, w)       : weight(b) − weight(a) = w を追加。今までと矛盾するなら false（何も変えない）
//   same(a, b)           : 同じグループか（差が決まっているか）
//   diff(a, b)           : weight(b) − weight(a)。same(a, b) のときだけ意味がある
//   size(a)              : a のグループの頂点数
//   leader(a), weight(a) : 根の番号、根との差（内部でも使う）
template <class T>
struct WeightedUnionFind {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/weighted_union_find.cpp
    vector<int> par, sz;
    vector<T> pot;  // pot[v] = weight(v) - weight(par[v])
    // n : 頂点数
    WeightedUnionFind(int n) : par(n), sz(n, 1), pot(n, T(0)) { iota(par.begin(), par.end(), 0); }

    // leader(v) : v のグループの根（経路圧縮あり。再帰の深さは log N 以下）
    int leader(int v) {
        if (par[v] == v) return v;
        int r = leader(par[v]);
        pot[v] += pot[par[v]];  // ★ 親までの差 + 親から根までの差 = 根までの差
        return par[v] = r;      // ★ 根に直接つなぎ替える（経路圧縮）
    }
    // weight(v) : weight(v) − weight(根)
    T weight(int v) { leader(v); return pot[v]; }  // weight(v) - weight(根)
    // same(a, b) : 同じグループか
    bool same(int a, int b) { return leader(a) == leader(b); }
    // diff(a, b) : weight(b) − weight(a)。違うグループなら意味のない値
    T diff(int a, int b) { return weight(b) - weight(a); }
    // size(v) : v のグループの頂点数
    int size(int v) { return sz[leader(v)]; }

    // merge(a, b, w) : a, b : 頂点、w : weight(b) − weight(a) の値（負でもよい）。矛盾しなければ true
    bool merge(int a, int b, T w) {
        if (same(a, b)) return diff(a, b) == w;  // ★ すでに差が決まっているなら矛盾チェックだけ
        w += weight(a) - weight(b);              // ★ 根どうしの差に変換: weight(rb) - weight(ra)
        a = leader(a), b = leader(b);
        if (sz[a] < sz[b]) swap(a, b), w = -w;   // 小さい方を大きい方の下につなぐ
        par[b] = a, pot[b] = w, sz[a] += sz[b];
        return true;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    WeightedUnionFind<long long> uf(5);
    assert(uf.merge(0, 1, 3));   // x1 - x0 = 3
    assert(uf.merge(1, 2, 4));   // x2 - x1 = 4
    assert(uf.merge(3, 4, -2));  // x4 - x3 = -2
    assert(uf.diff(0, 2) == 7 && uf.diff(2, 0) == -7);
    assert(!uf.same(0, 3));
    assert(uf.merge(2, 4, 1));   // x4 - x2 = 1 → 2 つのグループがつながる
    assert(uf.diff(0, 3) == 10 && uf.size(0) == 5);
    assert(!uf.merge(0, 4, 0));  // x4 - x0 = 8 のはずなので矛盾
    assert(uf.merge(0, 4, 8));   // 矛盾しない情報は true
    // ランダムテスト: 受け入れた条件だけで差を愚直に伝播させたものと比較
    {
        mt19937 rr(115);
        for (int it = 0; it < 2000; it++) {
            int n = rr() % 7 + 1;
            WeightedUnionFind<long long> w(n);
            vector<tuple<int, int, long long>> acc;
            auto pot = [&](int s) {  // s を 0 としたときの各頂点の値（つながっていなければ LLONG_MIN）
                vector<long long> d(n, LLONG_MIN);
                d[s] = 0;
                for (bool ch = true; ch;) {
                    ch = false;
                    for (auto [a, b, c] : acc) {
                        if (d[a] != LLONG_MIN && d[b] == LLONG_MIN) d[b] = d[a] + c, ch = true;
                        if (d[b] != LLONG_MIN && d[a] == LLONG_MIN) d[a] = d[b] - c, ch = true;
                    }
                }
                return d;
            };
            for (int q = 0; q < 15; q++) {
                int a = rr() % n, b = rr() % n;
                long long c = (long long)(rr() % 7) - 3;
                auto d = pot(a);
                bool expect = d[b] == LLONG_MIN ? true : d[b] == c;
                assert(w.merge(a, b, c) == expect);
                if (d[b] == LLONG_MIN) acc.push_back({a, b, c});
                int x = rr() % n, y = rr() % n;
                auto dx = pot(x);
                assert(w.same(x, y) == (dx[y] != LLONG_MIN));
                if (dx[y] != LLONG_MIN) assert(w.diff(x, y) == dx[y]);
                int cnt = 0;
                for (long long v : dx) cnt += v != LLONG_MIN;
                assert(w.size(x) == cnt);
            }
        }
    }
    puts("OK");
}
