// =====================================================================
// graph/general_matching.cpp  一般グラフの最大マッチング（Edmonds の花アルゴリズム）
//   二部グラフでなくても、端点を共有しない辺をできるだけ多く選ぶ。O(N^3)（N <= 500 程度）
//   二部グラフなら bipartite_matching.cpp（Hopcroft-Karp）の方が速い
//   ★ 増加路（マッチしていない頂点から、使っていない辺・使っている辺を交互にたどってマッチしていない頂点に着く道）を
//      BFS で探して反転する、を繰り返す。奇数長の閉路（花）に出会ったら、それを 1 頂点に縮めて探し続ける
// 依存: なし
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// GeneralMatching gm(n);  gm.add_edge(a, b);  ll k = gm.solve();
//   k = マッチングの大きさ（辺の本数）、gm.mate[v] = v の相手（いなければ -1）
// ◆ struct GeneralMatching : 一般グラフの最大マッチング（Edmonds の花アルゴリズム）。O(N^3)
//   GeneralMatching(n) : 頂点 0..n−1
//   add_edge(a, b)     : 無向辺を足す（自己ループは無視）
//   solve()            : 最大マッチングの辺の本数。mate[v] に相手（いなければ −1）が入る
struct GeneralMatching {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/general_matching.cpp
    int n;
    vector<vector<int>> g;
    vector<int> mate, par, base;
    vector<bool> used, blossom;
    // n_ : 頂点数（N <= 500 くらいまで）
    GeneralMatching(int n_) : n(n_), g(n_), mate(n_, -1) {}
    // add_edge(a, b) : a, b : 端点（多重辺は 1 本と同じ）
    void add_edge(int a, int b) {
        if (a == b) return;
        g[a].push_back(b), g[b].push_back(a);
    }
    // （内部用）交互木での 2 頂点の共通の祖先
    int lca(int a, int b) {  // 交互木の上で、2 頂点（の花）の共通の祖先
        vector<bool> seen(n, false);
        while (true) {
            a = base[a];
            seen[a] = true;
            if (mate[a] == -1) break;
            a = par[mate[a]];
        }
        while (true) {
            b = base[b];
            if (seen[b]) return b;
            b = par[mate[b]];
        }
    }
    // （内部用）花を縮めるときの印付け
    void mark_path(int v, int b, int child) {  // v から花の根 b までの頂点に「花の中」の印を付ける
        while (base[v] != b) {
            blossom[base[v]] = blossom[base[mate[v]]] = true;
            par[v] = child;
            child = mate[v];
            v = par[mate[v]];
        }
    }
    // （内部用）root から増加路を探す
    int find_path(int root) {  // root から増加路を BFS で探し、終点を返す（なければ -1）
        used.assign(n, false), par.assign(n, -1), base.resize(n);
        iota(base.begin(), base.end(), 0);
        used[root] = true;
        queue<int> q;
        q.push(root);
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int to : g[v]) {
                if (base[v] == base[to] || mate[v] == to) continue;
                if (to == root || (mate[to] != -1 && par[mate[to]] != -1)) {  // ★ 奇数長の閉路（花）を見つけた → 縮める
                    int cb = lca(v, to);
                    blossom.assign(n, false);
                    mark_path(v, cb, to), mark_path(to, cb, v);
                    for (int i = 0; i < n; i++)
                        if (blossom[base[i]]) {
                            base[i] = cb;
                            if (!used[i]) used[i] = true, q.push(i);
                        }
                } else if (par[to] == -1) {
                    par[to] = v;
                    if (mate[to] == -1) return to;  // ★ マッチしていない頂点に着いた = 増加路
                    used[mate[to]] = true, q.push(mate[to]);
                }
            }
        }
        return -1;
    }
    // solve() : 辺を全部足してから呼ぶ。返り値 = マッチングの本数
    long long solve() {
        for (int v = 0; v < n; v++)  // 最初に貪欲にマッチさせておく（速くなる）
            if (mate[v] == -1)
                for (int to : g[v])
                    if (mate[to] == -1) { mate[v] = to, mate[to] = v; break; }
        for (int v = 0; v < n; v++) {
            if (mate[v] != -1) continue;
            int u = find_path(v);
            while (u != -1) {  // ★ 増加路を反転（使っている辺と使っていない辺を入れ替える）
                int pv = par[u], ppv = mate[pv];
                mate[u] = pv, mate[pv] = u;
                u = ppv;
            }
        }
        int cnt = 0;
        for (int v = 0; v < n; v++) cnt += mate[v] != -1;
        return cnt / 2;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 5 角形（奇数長の閉路）は 2 本、ピーターセングラフ（10 頂点）は 5 本（完全マッチング）
    {
        GeneralMatching gm(5);
        for (int i = 0; i < 5; i++) gm.add_edge(i, (i + 1) % 5);
        assert(gm.solve() == 2);
        GeneralMatching pt(10);
        for (int i = 0; i < 5; i++) pt.add_edge(i, (i + 1) % 5), pt.add_edge(i, i + 5), pt.add_edge(i + 5, (i + 2) % 5 + 5);
        assert(pt.solve() == 5);
    }
    // 三角形 0-1-2 に 3-0 と 2-4 をつなぐ → 最大は 2 本（5 頂点なので 2 本が上限。例: 3-0, 2-4）
    {
        GeneralMatching gm(5);
        gm.add_edge(0, 1), gm.add_edge(1, 2), gm.add_edge(2, 0), gm.add_edge(3, 0), gm.add_edge(2, 4);
        assert(gm.solve() == 2);
    }
    // ランダムテスト: bit DP で求めた最大マッチングと比較し、mate が本当に辺のマッチングになっているか確認
    {
        mt19937 rr(197);
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 12 + 1;
            vector<vector<bool>> adj(n, vector<bool>(n, false));
            GeneralMatching gm(n);
            int p = rr() % 100;
            vector<pair<int, int>> es;
            for (int a = 0; a < n; a++) for (int b = a + 1; b < n; b++) if ((int)(rr() % 100) < p) es.push_back({a, b});
            shuffle(es.begin(), es.end(), rr);
            for (auto [a, b] : es) {
                if (rr() % 2) swap(a, b);
                gm.add_edge(a, b), adj[a][b] = adj[b][a] = true;
            }
            vector<int> dp(1 << n, 0);  // dp[S] = S の頂点だけでの最大マッチング（一番下の頂点を使わないか、誰かと組む）
            for (int S = 1; S < (1 << n); S++) {
                int v = __builtin_ctz(S), R = S & ~(1 << v);
                dp[S] = dp[R];
                for (int u = 0; u < n; u++) if ((R >> u & 1) && adj[v][u]) dp[S] = max(dp[S], dp[R & ~(1 << u)] + 1);
            }
            int k = gm.solve();
            assert(k == dp[(1 << n) - 1]);
            int c = 0;
            for (int v = 0; v < n; v++)
                if (gm.mate[v] != -1) { assert(gm.mate[gm.mate[v]] == v && adj[v][gm.mate[v]]); c++; }
            assert(c == 2 * k);
        }
    }
    puts("OK");
}
