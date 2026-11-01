// =====================================================================
// tree/auxiliary_tree.cpp  Auxiliary Tree（虚樹）
//   木の頂点を K 個指定したとき、「その K 個と、それらの LCA」だけを残して、親子関係を保った小さい木を作る
//   残る頂点は 2K − 1 個以下。前計算 O(N log N)、1 回の構築 O(K log N)。tree.cpp の Tree を継承していて、使い方は Tree と同じ
//   ★ 使いどころ: 「色ごとに」「クエリごとに」指定された頂点の集合で木 DP をしたい。全部で ΣK が N 程度なら、
//      毎回 N 頂点の木 DP をする代わりに、K に比例する大きさの木で DP できる
// 依存: tree/tree.cpp の Tree（先に貼る）
// ABC / ARC での使用例:
//   ABC340 G - Leaf Color  https://atcoder.jp/contests/abc340/tasks/abc340_g
//     → 色ごとにその色の頂点で compress し、虚樹の上で木 DP（Σ 頂点数 = O(N)）
//   ABC359 G - Sum of Tree Distance  https://atcoder.jp/contests/abc359/tasks/abc359_g
//     → ラベルごとに compress し、虚樹の各辺について「辺の長さ × 片側の個数 × 反対側の個数」を足す
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;
// テスト用: tree.cpp の Tree を読み込む（提出するときは、tree.cpp のコピー範囲をこのファイルのコピー範囲より先に貼る）
#define TREE_NO_MAIN
#include "tree.cpp"

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体（tree.cpp の Tree が必要） ----------
// AuxiliaryTree<> at(n);  at.add_edge(a, b);  at.build(root);    Tree と同じ使い方（Tree の関数 lca・dist なども使える）
//   at.lca(u, v), at.dist(u, v), at.dep[v]（根からの辺の本数）、at.wdist(u, v)（重み付きの距離）
//   auto [vs, par] = at.compress(S);  S = 指定する頂点の集合（重複 OK）
//     vs[i]  : 残った頂点（元の番号）。行きがけ順に並んでいて、vs[0] が根
//     par[i] : vs[i] の親の、vs の中での位置（vs[0] は -1）。元の木での辺の本数は dep[vs[i]] − dep[vs[par[i]]]
//   DP は i = vs.size() − 1 から 0 へ回せば「子が先」になる（行きがけ順の逆）
// 隣接リスト g がもうあるなら AuxiliaryTree<> at(g, root); でもよい（add_edge と build をまとめてやる）
// ◆ struct AuxiliaryTree<T> : Auxiliary Tree（虚樹。指定した頂点とその LCA だけを残した木）。Tree<T> を継承
//   AuxiliaryTree(n)       : 頂点 0..n−1（辺はまだない）
//   AuxiliaryTree(g, root) : 隣接リスト g（両方向）から作って、すぐ build(root) する
//   add_edge(a, b, w)      : Tree と同じ
//   build(root)            : Tree の前計算＋行きがけ順の番号 tin。O(N log N)
//   compress(S)            : {vs, par}。vs = 残った頂点（行きがけ順、vs[0] が根）、par[i] = vs[i] の親の vs での位置（根は −1）
//   T : 辺の重みの型（既定 long long）
template <class T = long long>
struct AuxiliaryTree : Tree<T> {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/auxiliary_tree.cpp
    using B = Tree<T>;
    vector<int> tin, pos;
    // n_ : 頂点数
    AuxiliaryTree(int n_) : B(n_) {}
    // g : 木の隣接リスト（両方向）、root : 根（省略で 0）
    AuxiliaryTree(const vector<vector<int>>& g, int root = 0) : B(g) { build(root); }
    // build(root) : root : 根（省略で 0）。辺を全部足してから呼ぶ
    void build(int root = 0) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/auxiliary_tree.cpp
        B::build(root);
        int n = this->n;
        tin.assign(n, 0), pos.assign(n, -1);
        vector<int> st = {root};
        int t = 0;
        while (!st.empty()) {  // 行きがけ順の番号 tin をつける
            int v = st.back();
            st.pop_back();
            tin[v] = t++;
            for (int to : this->children[v]) st.push_back(to);
        }
    }
    // compress(S) : S : 指定する頂点の一覧（重複・順不同 OK）。O(|S| log N)。build の後に呼ぶ
    //   返り値の頂点数は 2|S| − 1 以下。元の木での辺の長さは dep[vs[i]] − dep[vs[par[i]]]
    pair<vector<long long>, vector<long long>> compress(vector<int> vs) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/auxiliary_tree.cpp
        auto by_tin = [&](int a, int b) { return tin[a] < tin[b]; };
        sort(vs.begin(), vs.end(), by_tin);
        vs.erase(unique(vs.begin(), vs.end()), vs.end());
        int k = vs.size();
        for (int i = 0; i + 1 < k; i++) vs.push_back(this->lca(vs[i], vs[i + 1]));  // ★ 行きがけ順で隣どうしの LCA だけ足せば、すべての組の LCA が入る
        sort(vs.begin(), vs.end(), by_tin);
        vs.erase(unique(vs.begin(), vs.end()), vs.end());
        for (int i = 0; i < (int)vs.size(); i++) pos[vs[i]] = i;
        vector<long long> par(vs.size(), -1);
        for (int i = 1; i < (int)vs.size(); i++) par[i] = pos[this->lca(vs[i - 1], vs[i])];  // ★ 1 つ前の頂点との LCA が親になる
        for (int v : vs) pos[v] = -1;
        return {vector<long long>(vs.begin(), vs.end()), par};
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 木: 0-1, 0-2, 1-3, 1-4, 2-5, 4-6（根 0。0 の子は 1, 2、1 の子は 3, 4、2 の子は 5、4 の子は 6）
    // S = {3, 6, 5} → 残るのは 3, 6, 5 と LCA の 1, 0 → 0 の子は 1 と 5、1 の子は 3 と 6
    AuxiliaryTree<> at(7);  // Tree と同じく add_edge → build
    for (auto [a, b] : vector<pair<int, int>>{{0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}, {4, 6}}) at.add_edge(a, b);
    at.build(0);
    {
        auto [vs, par] = at.compress({3, 6, 5});
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
            AuxiliaryTree<> a(h, root);
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
                auto [vs, par] = a.compress(S);
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
