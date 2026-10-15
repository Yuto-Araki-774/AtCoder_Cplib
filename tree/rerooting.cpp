// =====================================================================
// tree/rerooting.cpp  全方位木 DP（rerooting）
//   「すべての頂点 v について、v を根にしたときの木 DP の値」をまとめて O(N) で求める
//   例: 各頂点から一番遠い頂点までの距離、各頂点から全頂点への距離の和
//   普通にやると根ごとに O(N) で全体 O(N^2) かかるものを O(N) にする
// 依存: なし（tree/tree.cpp とは独立）
// ABC / ARC での使用例:
//   ABC220 F - Distance Sums 2  https://atcoder.jp/contests/abc220/tasks/abc220_f
//     → 各頂点からの距離の和（使用例の「例2」そのもの）
//   ABC348 E - Minimize Sum of Distances  https://atcoder.jp/contests/abc348/tasks/abc348_e
//     → 例2 の頂点数を C_i の和に置き換える
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// S : DP の値の型
// Rerooting<S> rr(n);  rr.add_edge(a, b, w);  （重みなしなら w 省略 → 0）
// auto ans = rr.solve(e, merge, put_edge, put_vertex);   ans[v] = v を根にしたときの DP 値
//
// 【自分で定義するもの】（どれも main の中にラムダで書いてよい）
//   木 DP を「子の値を集めて自分の値を作る」形で考え、次の 4 つに分ける:
//   e                                    : merge の単位元（子が 1 つもないときの「集めた結果」）
//   merge(S a, S b) -> S                 : 子たちからの値を集める（順番によらない演算: +, max など）
//   put_edge(S x, int c, int p, W w) -> S: 子 c の部分木の値 x を、辺 c-p（重み w）を通して親 p に渡すときの変換
//   put_vertex(S x, int v) -> S          : 集めた値 x から、頂点 v の部分木の値を作る
//
//   普通の木 DP で書くと: dp[v] = put_vertex( merge_{子 c}( put_edge(dp[c], c, v, w) ), v )
//
// 例1: 各頂点から一番遠い頂点までの距離
//   e = 0, merge = max, put_edge = x + w, put_vertex = x
// 例2: 各頂点から全頂点への距離の和   S = pair<long long, long long>（{距離の和, 頂点数}）
//   e = {0, 0}, merge = 成分ごとの和, put_edge = {x.first + x.second * w, x.second},
//   put_vertex = {x.first, x.second + 1}
// ◆ struct Rerooting : 全方位木 DP（すべての頂点 v について「v を根にしたときの木 DP の値」を O(N) で）
//   Rerooting(n)                                 : 頂点 0..n−1
//   add_edge(a, b, w)                            : 無向辺を足す（w 省略で 0）
//   solve(e, merge, put_edge, put_vertex)        : ans[v] を全頂点について返す
//   S : DP の値の型、W : 辺の重みの型（既定 long long）
template <class S, class W = long long>
struct Rerooting {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/rerooting.cpp
    int n;
    vector<vector<pair<int, W>>> g;
    // n_ : 頂点数（1 以上。木がつながっていること）
    Rerooting(int n_) : n(n_), g(n_) {}
    // add_edge(a, b, w) : a, b : 端点、w : 重み（put_edge に渡される）
    void add_edge(int a, int b, W w = W(0)) { g[a].push_back({b, w}), g[b].push_back({a, w}); }

    // solve(e, merge, put_edge, put_vertex) : 引数の意味は TreeDP::solve と同じ（根はいらない）
    //   注意 : merge は結合的で順番によらないこと（左右からの累積で「1 方向を除いた値」を作るため）
    template <class Merge, class PutEdge, class PutVertex>
    vector<S> solve(S e, Merge merge, PutEdge put_edge, PutVertex put_vertex) {
        vector<int> order = {0}, par(n, -1);
        vector<W> pw(n, W(0));  // pw[v] = v と親をつなぐ辺の重み
        for (int i = 0; i < (int)order.size(); i++) {  // 頂点 0 を根に BFS 順を作る
            int v = order[i];
            for (auto [to, w] : g[v])
                if (to != par[v]) par[to] = v, pw[to] = w, order.push_back(to);
        }
        // ★ 1 回目（葉 → 根）: down[v] = v を根とする部分木の DP 値（普通の木 DP）
        vector<S> down(n, e);
        for (int i = n - 1; i >= 0; i--) {
            int v = order[i];
            S acc = e;
            for (auto [to, w] : g[v])
                if (to != par[v]) acc = merge(acc, put_edge(down[to], to, v, w));
            down[v] = put_vertex(acc, v);
        }
        // ★ 2 回目（根 → 葉）: up[v] = 「親の側」の部分木から v に渡ってくる値
        vector<S> up(n, e), ans(n);
        for (int v : order) {
            int k = g[v].size();
            vector<S> val(k);
            for (int i = 0; i < k; i++) {
                auto [to, w] = g[v][i];
                val[i] = (to == par[v]) ? up[v] : put_edge(down[to], to, v, w);  // ★ 隣接する全方向からの値
            }
            // ★ 「i 番目の方向以外」を O(1) で取るために左右から累積
            vector<S> pre(k + 1, e), suf(k + 1, e);
            for (int i = 0; i < k; i++) pre[i + 1] = merge(pre[i], val[i]);
            for (int i = k - 1; i >= 0; i--) suf[i] = merge(val[i], suf[i + 1]);
            ans[v] = put_vertex(pre[k], v);  // ★ 全方向を集めた値 = v を根にしたときの答え
            for (int i = 0; i < k; i++) {
                auto [to, w] = g[v][i];
                if (to == par[v]) continue;
                // ★ 子 to から見た「v 側」の部分木 = v から to の方向を除いたもの
                up[to] = put_edge(put_vertex(merge(pre[i], suf[i + 1]), v), v, to, w);
            }
        }
        return ans;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    //  0 --1-- 1 --2-- 2
    //          |
    //          5
    //          |
    //          3
    Rerooting<long long> rr(4);
    rr.add_edge(0, 1, 1), rr.add_edge(1, 2, 2), rr.add_edge(1, 3, 5);

    // 例1: 各頂点から一番遠い頂点までの距離
    auto far = rr.solve(
        0LL,
        [](long long a, long long b) { return max(a, b); },
        [](long long x, int c, int p, long long w) { return x + w; },
        [](long long x, int v) { return x; });
    assert((far == vector<long long>{6, 5, 7, 7}));

    // 例2: 各頂点から全頂点への距離の和
    using P = pair<long long, long long>;  // {距離の和, 頂点数}
    Rerooting<P> r2(4);
    r2.add_edge(0, 1, 1), r2.add_edge(1, 2, 2), r2.add_edge(1, 3, 5);
    auto sum = r2.solve(
        P{0, 0},
        [](P a, P b) { return P{a.first + b.first, a.second + b.second}; },
        [](P x, int c, int p, long long w) { return P{x.first + x.second * w, x.second}; },
        [](P x, int v) { return P{x.first, x.second + 1}; });
    assert(sum[0].first == 1 + 3 + 6 && sum[1].first == 1 + 2 + 5 && sum[3].first == 6 + 5 + 7);
    // ランダムテスト: ワーシャルフロイドで全点対の距離を求めて、最遠距離・距離の和と比較
    {
        mt19937 rr(3);
        using P2 = pair<long long, long long>;
        for (int it = 0; it < 2000; it++) {
            int n = rr() % 9 + 1;
            Rerooting<long long> q1(n);
            Rerooting<P2> q2(n);
            vector<vector<long long>> d(n, vector<long long>(n, (long long)1e18));
            for (int v = 0; v < n; v++) d[v][v] = 0;
            for (int v = 1; v < n; v++) {
                int p = rr() % v;
                long long w = rr() % 10;
                q1.add_edge(p, v, w), q2.add_edge(p, v, w), d[p][v] = d[v][p] = w;
            }
            for (int k = 0; k < n; k++) for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
            auto fr = q1.solve(0LL, [](long long a, long long b) { return max(a, b); },
                               [](long long x, int, int, long long w) { return x + w; }, [](long long x, int) { return x; });
            auto sm = q2.solve(P2{0, 0}, [](P2 a, P2 b) { return P2{a.first + b.first, a.second + b.second}; },
                               [](P2 x, int, int, long long w) { return P2{x.first + x.second * w, x.second}; },
                               [](P2 x, int) { return P2{x.first, x.second + 1}; });
            for (int v = 0; v < n; v++) {
                long long mx = 0, s = 0;
                for (int u = 0; u < n; u++) mx = max(mx, d[v][u]), s += d[v][u];
                assert(fr[v] == mx && sm[v].first == s && sm[v].second == n);
            }
        }
    }
    puts("OK");
}
