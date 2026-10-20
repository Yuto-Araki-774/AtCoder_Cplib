// =====================================================================
// graph/bfs.cpp  幅優先探索（重みなし最短路）と 01-BFS（重みが 0 か 1 の最短路）
//   本体A: 普通のグラフの BFS（多始点・経路復元）
//   本体B: グリッドの BFS
//   本体C: 01-BFS（辺の重みが 0 か 1 だけのとき。ダイクストラより速い O(N + M)）
//   A・B・C は互いに独立
// 依存: なし
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

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: グラフの BFS ----------
// g      : 隣接リスト（無向なら両方向に入れる）
// starts : 始点の集合（複数なら「最も近い始点までの距離」）
// 返り値 : dist[v]（到達不能は -1）、path(t) で経路復元
// ◆ struct BFSResult : bfs の結果
//   dist[v]   : 始点から v への最短距離（辺の本数）。行けなければ −1
//   prev[v]   : 最短路で v の 1 つ前の頂点（始点・行けない頂点は −1）
//   path(t)   : 始点から t への最短路の頂点の並び（行けなければ空）
struct BFSResult {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp
    vector<int> dist, prev;
    // --- 削除可: 経路復元 ---
    // path(t) : t までの最短路（始点 → … → t）。t : 終点の頂点番号
    vector<long long> path(int t) const {
        if (dist[t] == -1) return {};
        vector<long long> res;
        for (int v = t; v != -1; v = prev[v]) res.push_back(v);  // ★ prev を逆にたどる
        reverse(res.begin(), res.end());
        return res;
    }
    // --- 削除可ここまで ---
};
// ◆ bfs(g, starts) / bfs(g, s) : 重みなしグラフの最短距離。O(N + M)
//   g      : 隣接リスト（無向なら両方向に入れておく）
//   starts : 始点の一覧（多始点。各頂点の距離は「一番近い始点から」の距離）。重複してもよい
//   s      : 始点が 1 つのとき
BFSResult bfs(const vector<vector<int>>& g, const vector<int>& starts) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp
    int n = g.size();
    BFSResult r{vector<int>(n, -1), vector<int>(n, -1)};
    queue<int> q;
    for (int s : starts)
        if (r.dist[s] == -1) r.dist[s] = 0, q.push(s);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (int to : g[v]) {
            if (r.dist[to] != -1) continue;  // ★ 一度決まった距離は最短なので二度と更新しない
            r.dist[to] = r.dist[v] + 1;      // ★ 1 歩先
            r.prev[to] = v;
            q.push(to);                      // ★ キュー（先入れ先出し）なので近い順に処理される
        }
    }
    return r;
}
// bfs(g, s) : 始点が 1 つのとき（s : 始点）
BFSResult bfs(const vector<vector<int>>& g, int s) {
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
// 頂点を自分で定義する版（グリッドや「(マス, 向き)」などの状態に使う）:
//   bfs01_func(状態数, 始点の集合, adj)
//   【自分で定義するもの】adj(v, relax) : v から行ける先ごとに relax(行き先, コスト 0 か 1) を呼ぶ（ラムダ）
// 隣接リスト版:
//   g[a] に {b, c}（c は 0 か 1）を入れて bfs01(g, s)
// ◆ bfs01_func(n, starts, adj) : 辺の重みが 0 か 1 だけのグラフの最短距離（01-BFS）。O(N + M)
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
                if (c == 0) dq.push_front({dist[to], to});  // ★ コスト 0 は先頭に（今と同じ距離なのですぐ処理）
                else dq.push_back({dist[to], to});          // ★ コスト 1 は末尾に（普通の BFS と同じ）
            }
        });
    }
    return dist;
}
// ◆ bfs01(g, s) : 隣接リスト版の 01-BFS
//   g : g[a] = {{行き先, コスト（0 か 1）}, ...}、s : 始点
vector<long long> bfs01(const vector<vector<pair<int, int>>>& g, int s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp
    return bfs01_func(g.size(), {s}, [&](int v, auto&& relax) {
        for (auto [to, c] : g[v]) relax(to, c);
    });
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    vector<vector<int>> g = {{1}, {0, 2}, {1}, {}};
    auto r = bfs(g, 0);
    assert(r.dist[2] == 2 && r.dist[3] == -1 && (r.path(2) == vector<long long>{0, 1, 2}));

    vector<string> S = {"..#", ".#.", "..."};
    auto d = grid_bfs(S, {{0, 0}});
    assert(d[1][2] == 5 && d[0][2] == -1);
    auto d8 = grid_bfs(S, {{0, 0}}, '#', true);
    assert(d8[1][2] == 2);

    // 01-BFS（隣接リスト版）
    vector<vector<pair<int, int>>> g01(4);
    g01[0] = {{1, 1}, {2, 0}}, g01[2] = {{1, 0}}, g01[1] = {{3, 1}};
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
        for (int e = rng() % 15; e > 0; e--) g2[rng() % n].push_back({(int)(rng() % n), (int)(rng() % 2)});
        auto got = bfs01(g2, 0);
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
