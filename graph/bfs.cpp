// =====================================================================
// graph/bfs.cpp  幅優先探索（重みなし最短路）と 01-BFS（重みが 0 か 1 の最短路）
//   本体A: グラフの BFS（多始点・経路復元）
//   本体B: グリッドの BFS（Graph は要らない）
//   本体C: 01-BFS（辺の重みが 0 か 1 だけのとき。ダイクストラより速い O(N + M)）
//   A・B・C は互いに独立
// 依存: graph/graph.cpp の Graph（本体A と bfs01 で使う。先に貼る）
// ABC / ARC での使用例:
//   ABC007 C - 幅優先探索  https://atcoder.jp/contests/abc007/tasks/abc007_3
//     → グリッド BFS（grid_bfs）
//   ABC168 D - .. (Double Dots)  https://atcoder.jp/contests/abc168/tasks/abc168_d
//     → BFS の prev がそのまま答え
//   ABC213 E - Stronger Takahashi  https://atcoder.jp/contests/abc213/tasks/abc213_e
//     → 歩く = 0、パンチ = 1 の 01-BFS（bfs01_func）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;
// テスト用: graph.cpp の Graph を読み込む（提出するときは、graph.cpp のコピー範囲をこのファイルのコピー範囲より先に貼る）
#define GRAPH_NO_MAIN
#include "graph.cpp"

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: グラフの BFS（graph.cpp の Graph が必要） ----------
// Graph<> g(n); g.add_edge(a, b); ...（重みは見ない）
// auto r = bfs(g, s);         r.dist[v]（到達不能は -1）、r.path(t) で経路復元。始点が複数なら bfs(g, {s1, s2, ...})
// ◆ struct BFSResult : bfs の結果
//   dist[v]   : 始点から v への最短距離（辺の本数）。行けなければ −1
//   prev[v]   : 最短路で v の 1 つ前の頂点（始点・行けない頂点は −1）
//   path(t)   : 始点から t への最短路の頂点の並び（行けなければ空）
struct BFSResult {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp
    vector<long long> dist, prev;
    // --- 削除可: 経路復元 ---
    // path(t) : t までの最短路（始点 → … → t）。t : 終点の頂点番号
    vector<long long> path(int t) const {
        if (dist[t] == -1) return {};
        vector<long long> res;
        for (long long v = t; v != -1; v = prev[v]) res.push_back(v);  // ★ prev を逆にたどる
        reverse(res.begin(), res.end());
        return res;
    }
    // --- 削除可ここまで ---
};
// ◆ bfs(g, starts) / bfs(g, s) : 重みなしグラフの最短距離。O(N + M)
//   g      : Graph（辺の重みは見ない。有向辺 add_arc は向きどおりにだけ進む）
//   starts : 始点の一覧（多始点。各頂点の距離は「一番近い始点から」の距離）。重複してもよい
//   s      : 始点が 1 つのとき
template <class T>
BFSResult bfs(const Graph<T>& g, const vector<int>& starts) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp
    int n = g.n;
    BFSResult r{vector<long long>(n, -1), vector<long long>(n, -1)};
    queue<int> q;
    for (int s : starts)
        if (r.dist[s] == -1) r.dist[s] = 0, q.push(s);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (auto& e : g.adj[v]) {
            if (r.dist[e.to] != -1) continue;  // ★ 一度決まった距離は最短なので二度と更新しない
            r.dist[e.to] = r.dist[v] + 1;      // ★ 1 歩先
            r.prev[e.to] = v;
            q.push(e.to);                      // ★ キュー（先入れ先出し）なので近い順に処理される
        }
    }
    return r;
}
// bfs(g, s) : 始点が 1 つのとき（s : 始点）
template <class T>
BFSResult bfs(const Graph<T>& g, int s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp
    return bfs(g, vector<int>{s});
}

// ---------- 本体B: グリッド BFS ----------
// grid : H 行の文字列、starts : 始点 {行, 列} の集合、wall : 壁の文字、diag : 8 方向なら true
// 返り値 : dist[i][j]（到達不能・壁は -1）
// ◆ grid_bfs(grid, starts, wall, diag) : グリッドの最短距離。O(HW)
//   grid   : H 行の文字列（各行の長さは同じ W であること）
//   starts : 始点 {行, 列} の一覧（0 始まり）。壁の上にある始点は無視する
//   wall   : 通れないマスの文字（既定 '#'）。それ以外の文字のマスは全部通れる
//   diag   : true なら斜めを含む 8 方向に動ける（既定 false = 上下左右の 4 方向）
//   返り値 : dist[i][j]（行けない・壁は −1）
vector<vector<long long>> grid_bfs(const vector<string>& grid, const vector<pair<int, int>>& starts,
                             char wall = '#', bool diag = false) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp
    static const int DR[8] = {1, 0, -1, 0, 1, 1, -1, -1}, DC[8] = {0, 1, 0, -1, 1, -1, 1, -1};
    int h = grid.size(), w = grid[0].size(), k = diag ? 8 : 4;
    vector<vector<long long>> dist(h, vector<long long>(w, -1));
    queue<pair<int, int>> q;
    for (auto [r, c] : starts)
        if (grid[r][c] != wall && dist[r][c] == -1) dist[r][c] = 0, q.push({r, c});
    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();
        for (int d = 0; d < k; d++) {
            int nr = r + DR[d], nc = c + DC[d];                         // ★ 隣のマス
            if (nr < 0 || nr >= h || nc < 0 || nc >= w) continue;       // ★ 盤面の外
            if (grid[nr][nc] == wall || dist[nr][nc] != -1) continue;   // ★ 壁・訪問済み
            dist[nr][nc] = dist[r][c] + 1;
            q.push({nr, nc});
        }
    }
    return dist;
}

// ---------- 本体C: 01-BFS ----------
// 辺の重みが 0 か 1 だけのグラフの最短路。返り値 dist[v]（到達不能は -1）
// 典型: 「壁を壊すと 1 回、普通に進むと 0 回」「向きを変えるとコスト 1」など
//
// Graph 版（graph.cpp の Graph が必要）: g.add_edge(a, b, c)（c は 0 か 1）で作って bfs01(g, s)
// 頂点を自分で定義する版（グリッドや「(マス, 向き)」などの状態に使う。Graph は要らない）:
//   bfs01_func(状態数, 始点の集合, adj)
//   【自分で定義するもの】adj(v, relax) : v から行ける先ごとに relax(行き先, コスト 0 か 1) を呼ぶ（ラムダ）
// ◆ bfs01(g, starts) / bfs01(g, s) : Graph の 01-BFS。O(N + M)
//   g : Graph（辺の重みは 0 か 1 だけ）、starts / s : 始点
//   返り値 : dist[v]（行けなければ −1）
template <class T>
vector<long long> bfs01(const Graph<T>& g, const vector<int>& starts) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp
    vector<long long> dist(g.n, -1);
    deque<pair<long long, int>> dq;  // {距離, 頂点}
    for (int s : starts)
        if (dist[s] != 0) dist[s] = 0, dq.push_back({0, s});
    while (!dq.empty()) {
        auto [d, v] = dq.front();
        dq.pop_front();
        if (d != dist[v]) continue;  // 古い情報は捨てる
        for (auto& e : g.adj[v]) {
            long long nd = d + e.cost;
            if (dist[e.to] == -1 || nd < dist[e.to]) {
                dist[e.to] = nd;
                if (e.cost == 0) dq.push_front({nd, e.to});  // ★ コスト 0 は先頭に（今と同じ距離なのですぐ処理）
                else dq.push_back({nd, e.to});              // ★ コスト 1 は末尾に（普通の BFS と同じ）
            }
        }
    }
    return dist;
}
// bfs01(g, s) : 始点が 1 つのとき
template <class T>
vector<long long> bfs01(const Graph<T>& g, int s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp
    return bfs01(g, vector<int>{s});
}
// ◆ bfs01_func(n, starts, adj) : 頂点を自分で定義する 01-BFS。O(N + M)。Graph は要らない
//   n      : 頂点（状態）の数。状態は 0..n−1 の番号にしておく（例: (マス, 向き) → (i * W + j) * 4 + dir）
//   starts : 始点の一覧
//   adj    : void adj(int v, auto relax)。v から行ける先ごとに relax(行き先, コスト) を呼ぶ。コストは 0 か 1 だけ
//   返り値 : dist[v]（行けなければ −1）
template <class F>
vector<long long> bfs01_func(int n, const vector<int>& starts, F adj) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp
    vector<long long> dist(n, -1);
    deque<pair<long long, int>> dq;  // {距離, 頂点}
    for (int s : starts)
        if (dist[s] != 0) dist[s] = 0, dq.push_back({0, s});
    while (!dq.empty()) {
        auto [d, v] = dq.front();
        dq.pop_front();
        if (d != dist[v]) continue;  // 古い情報は捨てる
        adj(v, [&](int to, int c) {
            if (dist[to] == -1 || d + c < dist[to]) {
                dist[to] = d + c;
                if (c == 0) dq.push_front({dist[to], to});  // ★ コスト 0 は先頭に
                else dq.push_back({dist[to], to});          // ★ コスト 1 は末尾に
            }
        });
    }
    return dist;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    Graph<> g(4);
    g.add_edge(0, 1), g.add_edge(1, 2);
    auto r = bfs(g, 0);
    assert(r.dist[2] == 2 && r.dist[3] == -1 && (r.path(2) == vector<long long>{0, 1, 2}));

    vector<string> S = {"..#", ".#.", "..."};
    auto d = grid_bfs(S, {{0, 0}});
    assert(d[1][2] == 5 && d[0][2] == -1);
    auto d8 = grid_bfs(S, {{0, 0}}, '#', true);
    assert(d8[1][2] == 2);

    {   // 有向辺は向きどおりにだけ進む
        Graph<> h(3);
        h.add_arc(0, 1), h.add_arc(2, 1);
        auto rh = bfs(h, 1);
        assert(rh.dist[0] == -1 && rh.dist[2] == -1 && bfs(h, 2).dist[1] == 1 && bfs(h, {0, 2}).dist[1] == 1);
    }
    // 01-BFS（Graph 版）
    Graph<int> g01(4);
    g01.add_arc(0, 1, 1), g01.add_arc(0, 2, 0), g01.add_arc(2, 1, 0), g01.add_arc(1, 3, 1);
    auto d01 = bfs01(g01, 0);
    assert((d01 == vector<long long>{0, 0, 0, 1}));

    // 01-BFS（グリッド）: 普通に進むとコスト 0、壁 '#' を壊して進むとコスト 1
    vector<string> G = {"..#..", "####.", "....."};
    int H = G.size(), W = G[0].size();
    auto dg = bfs01_func(H * W, {0}, [&](int id, auto&& relax) {
        int i = id / W, j = id % W;
        const int di[4] = {1, 0, -1, 0}, dj[4] = {0, 1, 0, -1};
        for (int k = 0; k < 4; k++) {
            int ni = i + di[k], nj = j + dj[k];
            if (ni < 0 || ni >= H || nj < 0 || nj >= W) continue;
            relax(ni * W + nj, G[ni][nj] == '#' ? 1 : 0);
        }
    });
    assert(dg[2 * W + 0] == 1 && dg[0 * W + 4] == 1 && dg[0 * W + 1] == 0);

    // ランダムテスト: 01-BFS とダイクストラ（愚直）を比較
    mt19937 rng(3);
    for (int it = 0; it < 2000; it++) {
        int n = rng() % 8 + 1;
        vector<vector<pair<int, int>>> g2(n);
        Graph<int> gg(n);
        for (int e = rng() % 15; e > 0; e--) {
            int a = rng() % n, b = rng() % n, c = rng() % 2;
            if (rng() % 2) g2[a].push_back({b, c}), gg.add_arc(a, b, c);
            else g2[a].push_back({b, c}), g2[b].push_back({a, c}), gg.add_edge(a, b, c);
        }
        auto got = bfs01(gg, 0);
        auto got2 = bfs01_func(n, {0}, [&](int v, auto&& relax) { for (auto [to, c] : g2[v]) relax(to, c); });
        assert(got == got2);
        auto br = bfs(gg, 0);  // 普通の BFS: 重みを全部 1 とみなした距離
        vector<int> rb(n, INT_MAX);
        rb[0] = 0;
        for (int rep = 0; rep < n; rep++)
            for (int v = 0; v < n; v++)
                if (rb[v] != INT_MAX) for (auto [to, c] : g2[v]) rb[to] = min(rb[to], rb[v] + 1);
        for (int v = 0; v < n; v++) {
            assert(br.dist[v] == (rb[v] == INT_MAX ? -1 : rb[v]));
            if (br.dist[v] > 0) assert((int)br.path(v).size() == br.dist[v] + 1 && br.path(v)[0] == 0);
        }
        vector<int> ref(n, INT_MAX);
        ref[0] = 0;
        for (int rep = 0; rep < n; rep++)
            for (int v = 0; v < n; v++)
                if (ref[v] != INT_MAX)
                    for (auto [to, c] : g2[v]) ref[to] = min(ref[to], ref[v] + c);
        for (int v = 0; v < n; v++) assert(got[v] == (ref[v] == INT_MAX ? -1 : ref[v]));
    }
    puts("OK");
}
