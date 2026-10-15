// =====================================================================
// tree/auxiliary_tree.cpp  Auxiliary Tree（虚樹）
//   木の頂点を K 個指定したとき、「その K 個と、それらの LCA」だけを残して、親子関係を保った小さい木を作る
//   残る頂点は 2K − 1 個以下。前計算 O(N log N)、1 回の構築 O(K log N)
//   ★ 使いどころ: 「色ごとに」「クエリごとに」指定された頂点の集合で木 DP をしたい。全部で ΣK が N 程度なら、
//      毎回 N 頂点の木 DP をする代わりに、K に比例する大きさの木で DP できる
// 依存: なし
// ABC / ARC での使用例:
//   ABC340 G - Leaf Color  https://atcoder.jp/contests/abc340/tasks/abc340_g
//     → 色ごとにその色の頂点で build し、虚樹の上で木 DP（Σ 頂点数 = O(N)）
//   ABC359 G - Sum of Tree Distance  https://atcoder.jp/contests/abc359/tasks/abc359_g
//     → ラベルごとに build し、虚樹の各辺について「辺の長さ × 片側の個数 × 反対側の個数」を足す
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// AuxiliaryTree at(g, root);          g = 木の隣接リスト（頂点 0..n-1）
//   at.lca(u, v), at.dist(u, v), at.dep[v]（根からの辺の本数）
//   auto [vs, par] = at.build(S);     S = 指定する頂点の集合（重複 OK）
//     vs[i]  : 残った頂点（元の番号）。行きがけ順に並んでいて、vs[0] が根
//     par[i] : vs[i] の親の、vs の中での位置（vs[0] は -1）。元の木での辺の本数は dep[vs[i]] − dep[vs[par[i]]]
//   DP は i = vs.size() − 1 から 0 へ回せば「子が先」になる（行きがけ順の逆）
// ◆ struct AuxiliaryTree : Auxiliary Tree（虚樹。指定した頂点とその LCA だけを残した木）
//   AuxiliaryTree(g, root) : 前計算 O(N log N)
//   lca(u, v), dist(u, v)  : O(log N)
//   dep[v]                 : 根からの辺の本数
//   build(S)               : {vs, par}。vs = 残った頂点（行きがけ順、vs[0] が根）、par[i] = vs[i] の親の vs での位置（根は −1）
struct AuxiliaryTree {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/auxiliary_tree.cpp
    int n, LOG;
    vector<int> tin, dep, pos;
    vector<vector<int>> up;
    // g : 木の隣接リスト（両方向）、root : 根（省略で 0）
    AuxiliaryTree(const vector<vector<int>>& g, int root = 0) : n(g.size()), tin(n), dep(n, 0), pos(n, -1) {
        LOG = 1;
        while ((1 << LOG) < n) LOG++;
        up.assign(LOG, vector<int>(n, root));
        vector<int> st = {root};
        vector<bool> seen(n, false);
        seen[root] = true;
        int t = 0;
        while (!st.empty()) {  // 行きがけ順の番号 tin をつける
            int v = st.back();
            st.pop_back();
            tin[v] = t++;
            for (int to : g[v]) {
                if (seen[to]) continue;
                seen[to] = true, up[0][to] = v, dep[to] = dep[v] + 1;
                st.push_back(to);
            }
        }
        for (int k = 1; k < LOG; k++)
            for (int v = 0; v < n; v++) up[k][v] = up[k - 1][up[k - 1][v]];
    }
    // lca(u, v) : 最小共通祖先
    int lca(int u, int v) const {
        if (dep[u] < dep[v]) swap(u, v);
        for (int k = LOG - 1; k >= 0; k--) if (dep[u] - (1 << k) >= dep[v]) u = up[k][u];
        if (u == v) return u;
        for (int k = LOG - 1; k >= 0; k--) if (up[k][u] != up[k][v]) u = up[k][u], v = up[k][v];
        return up[0][u];
    }
    // dist(u, v) : 辺の本数
    int dist(int u, int v) const { return dep[u] + dep[v] - 2 * dep[lca(u, v)]; }
    // build(S) : S : 指定する頂点の一覧（重複・順不同 OK）。O(|S| log N)
    //   返り値の頂点数は 2|S| − 1 以下。元の木での辺の長さは dep[vs[i]] − dep[vs[par[i]]]
    pair<vector<int>, vector<int>> build(vector<int> vs) {
        auto by_tin = [&](int a, int b) { return tin[a] < tin[b]; };
        sort(vs.begin(), vs.end(), by_tin);
        vs.erase(unique(vs.begin(), vs.end()), vs.end());
        int k = vs.size();
        for (int i = 0; i + 1 < k; i++) vs.push_back(lca(vs[i], vs[i + 1]));  // ★ 行きがけ順で隣どうしの LCA だけ足せば、すべての組の LCA が入る
        sort(vs.begin(), vs.end(), by_tin);
        vs.erase(unique(vs.begin(), vs.end()), vs.end());
        for (int i = 0; i < (int)vs.size(); i++) pos[vs[i]] = i;
        vector<int> par(vs.size(), -1);
        for (int i = 1; i < (int)vs.size(); i++) par[i] = pos[lca(vs[i - 1], vs[i])];  // ★ 1 つ前の頂点との LCA が親になる
        for (int v : vs) pos[v] = -1;
        return {vs, par};
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 木: 0-1, 0-2, 1-3, 1-4, 2-5, 4-6（根 0。0 の子は 1, 2、1 の子は 3, 4、2 の子は 5、4 の子は 6）
    // S = {3, 6, 5} → 残るのは 3, 6, 5 と LCA の 1, 0 → 0 の子は 1 と 5、1 の子は 3 と 6
    vector<vector<int>> g(7);
    for (auto [a, b] : vector<pair<int, int>>{{0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}, {4, 6}}) g[a].push_back(b), g[b].push_back(a);
    AuxiliaryTree at(g, 0);
    {
        auto [vs, par] = at.build({3, 6, 5});
        assert(vs.size() == 5 && vs[0] == 0);
        map<int, int> p;  // 元の番号での親
        for (int i = 1; i < (int)vs.size(); i++) p[vs[i]] = vs[par[i]];
        assert(p[1] == 0 && p[5] == 0 && p[3] == 1 && p[6] == 1);
        assert(at.dep[6] - at.dep[1] == 2 && at.dist(3, 5) == 4);
        // 使用例: S の頂点どうしをつなぐのに必要な辺の本数（S を含む最小の部分木の辺の数）
        //   = 虚樹の辺の長さの合計 = (0-1) 1 + (0-5) 2 + (1-3) 1 + (1-6) 2 = 6
        int sum = 0;
        for (int i = 1; i < (int)vs.size(); i++) sum += at.dep[vs[i]] - at.dep[vs[par[i]]];
        assert(sum == 6);
    }
    // ランダムテスト: 定義どおり（S と、S のすべての組の LCA を集め、各頂点の親 = 集合の中で一番近い祖先）と比較
    {
        mt19937 rr(164);
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 12 + 1;
            vector<vector<int>> h(n);
            vector<int> lab(n);
            iota(lab.begin(), lab.end(), 0);
            shuffle(lab.begin(), lab.end(), rr);
            for (int v = 1; v < n; v++) {
                int p = rr() % v;
                h[lab[p]].push_back(lab[v]), h[lab[v]].push_back(lab[p]);
            }
            int root = rr() % n;
            AuxiliaryTree a(h, root);
            // 素朴な親・深さ・LCA
            vector<int> pr(n, -1), dp(n, 0), bfs = {root};
            pr[root] = root;
            for (int i = 0; i < (int)bfs.size(); i++)
                for (int to : h[bfs[i]]) if (pr[to] == -1) pr[to] = bfs[i], dp[to] = dp[bfs[i]] + 1, bfs.push_back(to);
            auto slow_lca = [&](int u, int v) {
                while (dp[u] > dp[v]) u = pr[u];
                while (dp[v] > dp[u]) v = pr[v];
                while (u != v) u = pr[u], v = pr[v];
                return u;
            };
            for (int q = 0; q < 5; q++) {
                int k = rr() % n + 1;
                vector<int> S;
                for (int i = 0; i < k; i++) S.push_back(rr() % n);
                set<int> want(S.begin(), S.end());
                for (int u : S) for (int v : S) want.insert(slow_lca(u, v));
                auto [vs, par] = a.build(S);
                assert(set<int>(vs.begin(), vs.end()) == want && vs.size() == want.size());
                assert((int)vs.size() <= 2 * (int)set<int>(S.begin(), S.end()).size() - 1);
                for (int i = 0; i < (int)vs.size(); i++) {
                    int v = vs[i], up = -1;
                    for (int x = v; x != root;) { x = pr[x]; if (want.count(x)) { up = x; break; } }
                    assert(up == (par[i] == -1 ? -1 : vs[par[i]]));
                    if (i > 0) assert(a.tin[vs[i - 1]] < a.tin[vs[i]]);
                    assert(a.dep[v] == dp[v]);
                }
                int u = rr() % n, v = rr() % n;
                assert(a.lca(u, v) == slow_lca(u, v));
            }
        }
    }
    puts("OK");
}
