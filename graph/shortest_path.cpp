// =====================================================================
// graph/shortest_path.cpp  ベルマンフォード法・ワーシャルフロイド法・Johnson 法
//   本体A: ベルマンフォード（負の辺がある単一始点最短路、負閉路の検出）  O(NM)
//   本体B: ワーシャルフロイド（全点対の最短路。負の辺も可）              O(N^3)、N <= 500 程度
//   本体C: Johnson 法（負の辺があるグラフの全点対。ポテンシャルを付けてダイクストラ N 回） O(NM log N)、辺が少ないとき B より速い
//   A・B・C は互いに独立。負の辺がないなら dijkstra.cpp の方が速い
// 依存: graph/graph.cpp の Graph（先に貼る）
// ABC / ARC での使用例:
//   ABC061 D - Score Attack  https://atcoder.jp/contests/abc061/tasks/abc061_d
//     → スコアを −1 倍してベルマンフォード。dist[N] == NEG_INF なら inf
//   ABC012 D - バスと避けられない運命  https://atcoder.jp/contests/abc012/tasks/abc012_4
//     → ワーシャルフロイドで全点対
//   ABC375 F - Road Blocked  https://atcoder.jp/contests/abc375/tasks/abc375_f
//     → クエリを逆順にして「道路の追加」にし、wf_add_edge で O(N^2) 更新
//   ABC237 E - Skiing  https://atcoder.jp/contests/abc237/tasks/abc237_e
//     → 幸福度を −1 倍すると負の辺がある最短路。ポテンシャルを高さ H にすると重みが 0 以上になるので、johnson の中身と同じくダイクストラ 1 回（全点対の johnson は N が大きいので使わない）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;
// テスト用: graph.cpp の Graph を読み込む（提出するときは、graph.cpp のコピー範囲をこのファイルのコピー範囲より先に貼る）
#define GRAPH_NO_MAIN
#include "graph.cpp"

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: ベルマンフォード法（graph.cpp の Graph が必要） ----------
// Graph<> g(n); g.add_arc(a, b, c); ...（無向辺 add_edge も使えるが、負の無向辺はそれだけで負閉路 a → b → a になる）
// auto r = bellman_ford(g, s);
//   r.dist[v]      s から v への最短距離
//                   到達不能なら BFResult<T>::INF
//                   負閉路を通っていくらでも小さくできるなら BFResult<T>::NEG_INF
//   r.neg_cycle     s から行ける負閉路があるか
// 典型: 「t までのスコアをいくらでも大きくできるなら inf と出力」→ スコアを -1 倍して r.dist[t] == NEG_INF か判定
// ◆ BFResult<T> : bellman_ford の結果
//   dist[v]   : 最短距離。行けなければ INF、負閉路でいくらでも小さくできるなら NEG_INF
//   neg_cycle : 始点から行ける負閉路があるか
template <class T> struct BFResult {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/shortest_path.cpp
    static constexpr T INF = numeric_limits<T>::max() / 4;
    static constexpr T NEG_INF = -INF;
    vector<T> dist;
    bool neg_cycle;
};
// ◆ bellman_ford(g, s) : 負の辺があってもよい単一始点最短路。O(NM)
//   g : Graph<T>
//   s : 始点
//   注意 : 重みの絶対値 × 辺の本数 が T の最大値の 1/4 を超えないこと
template <class T>
BFResult<T> bellman_ford(const Graph<T>& g, int s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/shortest_path.cpp
    int n = g.n;
    const T INF = BFResult<T>::INF, NEG_INF = BFResult<T>::NEG_INF;
    BFResult<T> r{vector<T>(n, INF), false};
    r.dist[s] = 0;
    for (int it = 0; it < n - 1; it++) {  // ★ 最短路は高々 n-1 本の辺なので n-1 回すべての辺で更新すれば確定
        bool updated = false;
        for (int v = 0; v < n; v++)
            for (auto& e : g.adj[v])
                if (r.dist[v] != INF && r.dist[v] + e.cost < r.dist[e.to]) r.dist[e.to] = r.dist[v] + e.cost, updated = true;
        if (!updated) break;  // 更新がなくなったら打ち切り
    }
    for (int it = 0; it < n; it++)  // ★ さらに更新できる頂点は負閉路の影響を受けている。それを n 回広める
        for (int v = 0; v < n; v++)
            for (auto& e : g.adj[v]) {
                if (r.dist[v] == INF) continue;
                if (r.dist[v] == NEG_INF || r.dist[v] + e.cost < r.dist[e.to]) {
                    if (r.dist[e.to] != NEG_INF) r.dist[e.to] = NEG_INF, r.neg_cycle = true;
                }
            }
    return r;
}

// ---------- 本体B: ワーシャルフロイド法（wf_matrix(g) は graph.cpp の Graph が必要） ----------
// 使い方:
//   auto d = wf_matrix(g);                     // d[a][b] = 辺 a → b の重みの最小（なければ WF_INF）、d[i][i] = 0
//   bool neg = warshall_floyd(d);              // d[i][j] が i から j への最短距離になる（到達不能は WF_INF のまま）
//   neg が true なら負閉路がある（そのときの d は意味を持たない）
template <class T> constexpr T WF_INF = numeric_limits<T>::max() / 4;
// ◆ wf_matrix(g) : Graph からワーシャルフロイド用の n × n の行列（対角 0、辺は重みの最小、他は WF_INF）を作る
//   g : Graph<T>（無向辺は両向き、多重辺は一番軽いもの）
template <class T>
vector<vector<T>> wf_matrix(const Graph<T>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/shortest_path.cpp
    vector<vector<T>> d(g.n, vector<T>(g.n, WF_INF<T>));
    for (int i = 0; i < g.n; i++) d[i][i] = 0;
    for (int v = 0; v < g.n; v++)
        for (auto& e : g.adj[v]) d[v][e.to] = min(d[v][e.to], e.cost);  // ★ 多重辺に備えて min
    return d;
}
// ◆ warshall_floyd(d) : 全点対の最短距離。O(N^3)（N <= 500 くらい）
//   d : wf_matrix で作った行列。この関数が d を最短距離に書き換える（行けない組は WF_INF のまま）
//   返り値 : 負閉路があれば true（そのときの d は使えない）
template <class T>
bool warshall_floyd(vector<vector<T>>& d) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/shortest_path.cpp
    int n = d.size();
    for (int k = 0; k < n; k++)      // ★ k は一番外側（「頂点 0..k だけを経由してよい」を k を増やしながら広げる）
        for (int i = 0; i < n; i++) {
            if (d[i][k] == WF_INF<T>) continue;
            for (int j = 0; j < n; j++)
                if (d[k][j] != WF_INF<T>) d[i][j] = min(d[i][j], d[i][k] + d[k][j]);  // ★ k を経由した方が短いか
        }
    for (int i = 0; i < n; i++)
        if (d[i][i] < 0) return true;  // ★ 自分に戻って負 = 負閉路
    return false;
}
// --- 削除可: 辺を 1 本追加したときの更新 O(N^2)（クエリごとに辺が増える問題） ---
// warshall_floyd 済みの d に、辺 a -> b コスト c（無向なら undirected = true）を追加する
// ◆ wf_add_edge(d, a, b, c, undirected) : 最短距離にした d に辺を 1 本足して O(N^2) で更新する
//   d          : warshall_floyd 済みの行列（書き換わる）
//   a, b, c    : 足す辺 a → b、重み c
//   undirected : true（既定）なら b → a も足す。有向なら false
template <class T>
void wf_add_edge(vector<vector<T>>& d, int a, int b, T c, bool undirected = true) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/shortest_path.cpp
    int n = d.size();
    for (int i = 0; i < n; i++) {
        if (d[i][a] == WF_INF<T>) continue;
        for (int j = 0; j < n; j++)
            if (d[b][j] != WF_INF<T>) d[i][j] = min(d[i][j], d[i][a] + c + d[b][j]);  // ★ 新しい辺を通る道だけ試す
    }
    if (undirected) {
        for (int i = 0; i < n; i++) {
            if (d[i][b] == WF_INF<T>) continue;
            for (int j = 0; j < n; j++)
                if (d[a][j] != WF_INF<T>) d[i][j] = min(d[i][j], d[i][b] + c + d[a][j]);
        }
    }
}
// --- 削除可ここまで ---

// ---------- 本体C: Johnson 法（graph.cpp の Graph が必要） ----------
// auto d = johnson(g);    d[s][t] = s から t への最短距離（到達不能は numeric_limits<T>::max() / 4 = WF_INF と同じ値）
//                          負閉路がどこかにあれば空の vector を返す
// ★ ポテンシャル h（どの頂点にもコスト 0 で行ける仮の始点からの最短距離）を求めると、
//    w'(u→v) = w + h[u] − h[v] >= 0 になる（最短距離の性質 h[v] <= h[u] + w）→ 負の辺が消えるのでダイクストラが使える
//    本当の距離 = （w' での距離）− h[s] + h[t]
// 1 始点だけでよいなら、h を求めた後のダイクストラ 1 回で済む（「負の辺があるが、ポテンシャルが最初からわかる」問題で使う形）
// ◆ johnson(g) : 負の辺があるグラフの全点対の最短距離。O(NM log N)（辺が少ないならワーシャルフロイドより速い）
//   g : Graph<T>
//   返り値 : d[s][t]（行けなければ numeric_limits<T>::max() / 4）。負閉路がどこかにあれば空の vector
template <class T>
vector<vector<T>> johnson(const Graph<T>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/shortest_path.cpp
    int n = g.n;
    const T INF = numeric_limits<T>::max() / 4;
    vector<T> h(n, 0);  // 仮の始点から全頂点へコスト 0 の辺があるとみなすので、最初は全部 0
    for (int it = 0; it <= n; it++) {  // ★ ベルマンフォード。n + 1 回目にも更新があれば負閉路
        bool upd = false;
        for (int v = 0; v < n; v++)
            for (auto& e : g.adj[v])
                if (h[v] + e.cost < h[e.to]) h[e.to] = h[v] + e.cost, upd = true;
        if (!upd) break;
        if (it == n) return {};
    }
    vector<vector<T>> d(n, vector<T>(n, INF));
    using P = pair<T, int>;
    for (int s = 0; s < n; s++) {
        vector<T> dist(n, INF);
        priority_queue<P, vector<P>, greater<P>> pq;
        dist[s] = 0, pq.push({0, s});
        while (!pq.empty()) {
            auto [dv, v] = pq.top();
            pq.pop();
            if (dv != dist[v]) continue;
            for (auto& e : g.adj[v]) {
                T w = e.cost + h[v] - h[e.to];  // ★ 0 以上になった重み
                if (dv + w < dist[e.to]) dist[e.to] = dv + w, pq.push({dist[e.to], e.to});
            }
        }
        for (int t = 0; t < n; t++)
            if (dist[t] != INF) d[s][t] = dist[t] - h[s] + h[t];  // ★ ポテンシャルを戻す
    }
    return d;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    using R = BFResult<long long>;
    // 0 -> 1 (4), 0 -> 2 (1), 2 -> 1 (-2), 1 -> 3 (1)
    Graph<long long> g(5);
    g.add_arc(0, 1, 4), g.add_arc(0, 2, 1), g.add_arc(2, 1, -2), g.add_arc(1, 3, 1);
    auto r = bellman_ford(g, 0);
    assert(r.dist[1] == -1 && r.dist[3] == 0 && r.dist[4] == R::INF && !r.neg_cycle);
    // 負閉路 1 -> 3 -> 1 を追加。4 は負閉路の外から到達不能
    Graph<long long> g2 = g;
    g2.add_arc(3, 1, -5);
    auto r2 = bellman_ford(g2, 0);
    assert(r2.neg_cycle && r2.dist[1] == R::NEG_INF && r2.dist[3] == R::NEG_INF && r2.dist[2] == 1 && r2.dist[4] == R::INF);
    // 負閉路が s から行けない場所にあるだけなら影響しない
    {
        Graph<long long> g3(5);
        g3.add_arc(0, 1, 3), g3.add_arc(2, 3, -1), g3.add_arc(3, 2, -1);
        auto r3 = bellman_ford(g3, 0);
        assert(!r3.neg_cycle && r3.dist[1] == 3);
    }
    {
        Graph<long long> u(4);  // 無向辺
        u.add_edge(0, 1, 5), u.add_edge(1, 2, 3), u.add_edge(0, 2, 10);
        auto d = wf_matrix(u);
        assert(!warshall_floyd(d));
        assert(d[0][2] == 8 && d[2][0] == 8 && d[3][0] == WF_INF<long long>);
        wf_add_edge(d, 2, 3, 1LL);
        assert(d[0][3] == 9 && d[3][1] == 4);
        assert(bellman_ford(u, 0).dist[2] == 8);
        Graph<long long> ng(2);
        ng.add_arc(0, 1, 1), ng.add_arc(1, 0, -3);
        auto dn = wf_matrix(ng);
        assert(warshall_floyd(dn));  // 負閉路
        Graph<long long> nu(2);
        nu.add_edge(0, 1, -1);  // 負の無向辺は a → b → a で負閉路
        assert(bellman_ford(nu, 0).neg_cycle);
    }

    // ランダムテスト: ベルマンフォード（負閉路なし）とワーシャルフロイドを比較
    mt19937 rng(4);
    for (int it = 0; it < 2000; it++) {
        int n = rng() % 6 + 1, m = rng() % 12;
        Graph<long long> e(n);
        vector<vector<long long>> dd(n, vector<long long>(n, WF_INF<long long>));
        for (int i = 0; i < n; i++) dd[i][i] = 0;
        for (int i = 0; i < m; i++) {
            int a = rng() % n, b = rng() % n;
            long long c = (long long)(rng() % 10) - 2;
            e.add_arc(a, b, c);
            dd[a][b] = min(dd[a][b], c);
        }
        assert(wf_matrix(e) == dd);
        bool neg = warshall_floyd(dd);
        for (int s = 0; s < n; s++) {
            auto br = bellman_ford(e, s);
            if (neg) continue;
            assert(!br.neg_cycle);
            for (int v = 0; v < n; v++)
                assert((br.dist[v] == R::INF) == (dd[s][v] == WF_INF<long long>) && (br.dist[v] == R::INF || br.dist[v] == dd[s][v]));
        }
    }
    // Johnson: 上の負閉路なしのグラフ（0 -> 1 (4), 0 -> 2 (1), 2 -> 1 (-2), 1 -> 3 (1)）で 0 -> 3 は 1 − 2 + 1 = 0、2 -> 3 は −1
    {
        auto d = johnson(g);
        const long long INF = numeric_limits<long long>::max() / 4;
        assert(d[0][3] == 0 && d[2][3] == -1 && d[0][1] == -1 && d[3][0] == INF && d[4][4] == 0);
        Graph<long long> e2 = g;
        e2.add_arc(3, 2, 0);  // 2 -> 1 -> 3 -> 2 = −2 + 1 + 0 = −1 の負閉路
        assert(johnson(e2).empty());
    }
    // ランダムテスト: ワーシャルフロイドと比較（負閉路なしのもの・ありのもの両方）
    {
        mt19937 r8(190);
        for (int it = 0; it < 3000; it++) {
            int n = r8() % 7 + 1, m = r8() % 15;
            vector<long long> pot(n);
            for (auto& p : pot) p = r8() % 20;
            bool any = r8() % 3 == 0;  // 3 回に 1 回は負閉路ができてもよい重みにする
            Graph<long long> e2(n);
            for (int i = 0; i < m; i++) {
                int a = r8() % n, b = r8() % n;
                long long c = any ? (long long)(r8() % 21) - 10 : (long long)(r8() % 10) + pot[a] - pot[b];  // ポテンシャルから作ると負閉路はできない
                e2.add_arc(a, b, c);
            }
            auto d = wf_matrix(e2);
            bool neg = warshall_floyd(d);
            auto j = johnson(e2);
            assert(j.empty() == neg);
            if (!neg) assert(j == d);
        }
    }
    puts("OK");
}
