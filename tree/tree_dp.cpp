// =====================================================================
// tree/tree_dp.cpp  木 DP のテンプレート
//   根を 1 つ決めて、「子の値を集めて自分の値を作る」DP を葉から根へ計算する。O(N)。再帰なし
//   自分で決める 4 つ（e, merge, put_edge, put_vertex）は rerooting.cpp（全方位木 DP）と同じ
//   → 根を 1 つに決める問題はこれ、すべての頂点を根にした答えが欲しいなら rerooting.cpp
// 依存: なし
// ABC / ARC での使用例:
//   ABC036 D - 塗り絵  https://atcoder.jp/contests/abc036/tasks/abc036_d
//     → 隣り合う 2 つを両方黒にしない塗り方の数（使用例の「例3」を mint で）
//   EDPC P - Independent Set  https://atcoder.jp/contests/dp/tasks/dp_p
//     → ABC036 D と同じ問題（ABC ではないが典型）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// S : DP の値の型（作るときに決まる。値の型が違う DP には別の TreeDP を作る）
// TreeDP<S> td(n);  td.add_edge(a, b, w);  （重みなしなら w 省略 → 0）
// auto dp = td.solve(root, e, merge, put_edge, put_vertex);   dp[v] = v を根とする部分木の DP 値
//
// 【自分で定義するもの】（main の中にラムダで書いてよい）
//   dp[v] = put_vertex( merge_{子 c}( put_edge(dp[c], c, v, w) ), v ) の形に分解する
//   e                                     : merge の単位元（子がいないときの「集めた結果」）
//   merge(S a, S b) -> S                  : 子たちからの値を集める（+, max, 積 など。順番によらない演算）
//   put_edge(S x, int c, int p, W w) -> S : 子 c の値 x を、辺 c-p（重み w）を通して親 p に渡すときの変換
//   put_vertex(S x, int v) -> S           : 集めた値 x から頂点 v の値を作る
//
// 例1: 部分木のサイズ      e = 0, merge = +, put_edge = x, put_vertex = x + 1
// 例2: 部分木の高さ        e = 0, merge = max, put_edge = x + 1, put_vertex = x
// 例3: 独立集合の数え上げ（隣り合う 2 頂点を両方黒にしない塗り方）  使用例を参照
// 処理順（td.order）と親（td.par）も solve の後に使える
// ◆ struct TreeDP : 木 DP のテンプレート（再帰なし）
//   TreeDP(n)                                       : 頂点 0..n−1
//   add_edge(a, b, w)                               : 無向辺を足す（w 省略で 0）
//   solve(root, e, merge, put_edge, put_vertex)     : dp[v] = v を根とする部分木の値 を全頂点について返す。O(N)
//   order, par                                      : solve の後に使える（BFS 順、親）
//   S : DP の値の型、W : 辺の重みの型（既定 long long）
template <class S, class W = long long>
struct TreeDP {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree_dp.cpp
    int n;
    vector<vector<pair<int, W>>> g;
    vector<int> order, par;  // order: 根からの BFS 順、par: 親（根は -1）
    // n_ : 頂点数
    TreeDP(int n_) : n(n_), g(n_) {}
    // add_edge(a, b, w) : a, b : 端点、w : 重み（put_edge に渡される）
    void add_edge(int a, int b, W w = W(0)) { g[a].push_back({b, w}), g[b].push_back({a, w}); }

    // solve(root, e, merge, put_edge, put_vertex) :
    //   root       : 根
    //   e          : merge の単位元（子がいないときの「集めた結果」）
    //   merge      : S merge(S a, S b)。子からの値を集める（順番によらない演算）
    //   put_edge   : S put_edge(S x, int c, int p, W w)。子 c の値 x を、辺 c-p（重み w）を通して親 p へ渡すときの変換
    //   put_vertex : S put_vertex(S x, int v)。集めた値 x から頂点 v の値を作る
    template <class Merge, class PutEdge, class PutVertex>
    vector<S> solve(int root, S e, Merge merge, PutEdge put_edge, PutVertex put_vertex) {
        order = {root}, par.assign(n, -1);
        for (int i = 0; i < (int)order.size(); i++) {  // ★ 根から BFS して親子関係を決める
            int v = order[i];
            for (auto [to, w] : g[v])
                if (to != par[v]) par[to] = v, order.push_back(to);
        }
        vector<S> dp(n, e);
        for (int i = n - 1; i >= 0; i--) {  // ★ BFS 順の逆 = 子が必ず先に計算済み
            int v = order[i];
            S acc = e;
            for (auto [to, w] : g[v])
                if (to != par[v]) acc = merge(acc, put_edge(dp[to], to, v, w));  // ★ 子の値を集める
            dp[v] = put_vertex(acc, v);                                           // ★ 自分の値にする
        }
        return dp;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    //     0
    //    /|\ .
    //   1 2 3
    //   |
    //   4
    TreeDP<long long> td(5);
    td.add_edge(0, 1, 5), td.add_edge(0, 2, 1), td.add_edge(0, 3, 2), td.add_edge(1, 4, 7);

    // 例1: 部分木のサイズ
    auto sz = td.solve(0, 0LL, [](long long a, long long b) { return a + b; },
                       [](long long x, int, int, long long) { return x; }, [](long long x, int) { return x + 1; });
    assert((sz == vector<long long>{5, 2, 1, 1, 1}));

    // 例2: 部分木の高さ（重み付き = 一番遠い子孫までの距離）
    auto h = td.solve(0, 0LL, [](long long a, long long b) { return max(a, b); },
                      [](long long x, int, int, long long w) { return x + w; }, [](long long x, int) { return x; });
    assert(h[0] == 12 && h[1] == 7 && h[2] == 0);

    // 例3: 独立集合の数え上げ  S = {v を白にしたときの数, v を黒にしたときの数}
    //   親が白なら子は白でも黒でもよい、親が黒なら子は白だけ
    //   put_edge: 子 → 親へ {親が白のときの数, 親が黒のときの数} = {白 + 黒, 白}
    //   merge   : 子ごとに独立なので成分ごとの積、e = {1, 1}
    using P = pair<long long, long long>;
    TreeDP<P> tp(5);  // S が違うので別に作る
    tp.add_edge(0, 1), tp.add_edge(0, 2), tp.add_edge(0, 3), tp.add_edge(1, 4);
    auto is = tp.solve(0, P{1, 1}, [](P a, P b) { return P{a.first * b.first, a.second * b.second}; },
                       [](P x, int, int, long long) { return P{x.first + x.second, x.first}; },
                       [](P x, int) { return x; });
    assert(is[0].first + is[0].second == 14);  // 0 が白: 3 × 2 × 2 = 12、0 が黒: 2 × 1 × 1 = 2

    // ランダムテスト: 独立集合の数を全探索と比較
    mt19937 rng(12);
    for (int it = 0; it < 500; it++) {
        int n = rng() % 12 + 1;
        TreeDP<P> t(n);
        vector<pair<int, int>> es;
        for (int v = 1; v < n; v++) { int p = rng() % v; t.add_edge(p, v), es.push_back({p, v}); }
        auto r = t.solve(rng() % n, P{1, 1}, [](P a, P b) { return P{a.first * b.first, a.second * b.second}; },
                         [](P x, int, int, long long) { return P{x.first + x.second, x.first}; }, [](P x, int) { return x; });
        int root = t.order[0];
        long long cnt = 0;
        for (int m = 0; m < (1 << n); m++) {
            bool ok = true;
            for (auto [a, b] : es) if ((m >> a & 1) && (m >> b & 1)) ok = false;
            cnt += ok;
        }
        assert(r[root].first + r[root].second == cnt);
    }
    puts("OK");
}
