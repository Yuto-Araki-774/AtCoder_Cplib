// =====================================================================
// graph/dominator_tree.cpp  支配木（Lengauer-Tarjan 法）
//   有向グラフで、根 root から v への道が必ず通る頂点を「v の支配点」という。
//   v の支配点のうち v に一番近いもの（直接支配点 idom[v]）を親にすると木になる（支配木）。O(M log N)
//   → u が v の支配点 ⇔ 支配木で u が v の祖先。「頂点 u を壊すと root から v に行けなくなるか」がわかる
//   ★ DFS 木の上で「半支配点」（DFS 番号が大きい頂点だけを通って v に来られる、一番番号の小さい頂点）を
//      DFS の逆順に求め、それを使って直接支配点を決める
// 依存: なし
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// g : 有向グラフの隣接リスト（g[v] = v から出る辺の行き先）
// auto idom = dominator_tree(g, root);
//   idom[v] = v の直接支配点（root は root 自身、root から行けない頂点は -1）
//   支配木で u が v の祖先 ⇔ root から v へのどの道も u を通る
vector<int> dominator_tree(const vector<vector<int>>& g, int root) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dominator_tree.cpp
    int n = g.size();
    vector<vector<int>> rg(n), bucket(n);
    for (int v = 0; v < n; v++) for (int to : g[v]) rg[to].push_back(v);
    vector<int> ord(n, -1), par(n, -1), vs;  // ord[v] = DFS で訪れた順番、vs[i] = i 番目に訪れた頂点
    {
        vector<pair<int, int>> st = {{root, -1}};  // 再帰なしの DFS（行きがけ順）
        while (!st.empty()) {
            auto [v, p] = st.back();
            st.pop_back();
            if (ord[v] != -1) continue;
            ord[v] = vs.size(), par[v] = p, vs.push_back(v);
            for (int i = (int)g[v].size() - 1; i >= 0; i--)
                if (ord[g[v][i]] == -1) st.push_back({g[v][i], v});
        }
    }
    int k = vs.size();
    vector<int> semi(n), idom(n, -1), anc(n, -1), label(n);
    for (int v = 0; v < n; v++) semi[v] = ord[v], label[v] = v;
    auto eval = [&](int v) {  // v から（森の）根までで、semi が一番小さい頂点。経路圧縮つき（再帰なし）
        if (anc[v] == -1) return v;
        vector<int> path;
        for (int x = v; anc[anc[x]] != -1; x = anc[x]) path.push_back(x);
        for (int i = (int)path.size() - 1; i >= 0; i--) {  // 根に近い方から
            int x = path[i];
            if (semi[label[anc[x]]] < semi[label[x]]) label[x] = label[anc[x]];
            anc[x] = anc[anc[x]];
        }
        return label[v];
    };
    for (int i = k - 1; i >= 1; i--) {  // ★ DFS の逆順に
        int w = vs[i];
        for (int v : rg[w]) {
            if (ord[v] == -1) continue;  // root から行けない頂点からの辺は無視
            int u = eval(v);
            semi[w] = min(semi[w], semi[u]);  // ★ 半支配点
        }
        bucket[vs[semi[w]]].push_back(w);
        int p = par[w];
        anc[w] = p;  // w を森につなぐ
        for (int v : bucket[p]) {
            int u = eval(v);
            idom[v] = semi[u] < semi[v] ? u : p;  // ★ いったん仮の値（あとで直す）
        }
        bucket[p].clear();
    }
    for (int i = 1; i < k; i++) {  // ★ DFS の順に、仮の値を本当の直接支配点に直す
        int w = vs[i];
        if (idom[w] != vs[semi[w]]) idom[w] = idom[idom[w]];
    }
    idom[root] = root;
    return idom;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 0 → 1, 0 → 2, 1 → 3, 2 → 3, 3 → 4, 4 → 1, 5 → 4
    //   3 へは 1 経由と 2 経由があるので idom[3] = 0。4 は必ず 3 を通るので idom[4] = 3。5 は 0 から行けない
    {
        vector<vector<int>> g = {{1, 2}, {3}, {3}, {4}, {1}, {4}};
        auto idom = dominator_tree(g, 0);
        assert((idom == vector<int>{0, 0, 0, 0, 3, -1}));
    }
    // ランダムテスト: 「頂点 d を取り除くと root から v に行けなくなる ⇔ d は v の支配点」で求めた答えと比較
    {
        mt19937 rr(199);
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 9 + 1, m = rr() % 20, root = rr() % n;
            vector<vector<int>> g(n);
            for (int i = 0; i < m; i++) g[rr() % n].push_back(rr() % n);
            auto reach = [&](int ban) {  // ban を通らずに root から行ける頂点
                vector<bool> seen(n, false);
                if (ban == root) return seen;
                vector<int> st = {root};
                seen[root] = true;
                while (!st.empty()) { int v = st.back(); st.pop_back(); for (int to : g[v]) if (to != ban && !seen[to]) seen[to] = true, st.push_back(to); }
                return seen;
            };
            auto base = reach(-1);
            vector<vector<int>> doms(n);  // doms[v] = v の支配点（v 自身は除く）
            for (int d = 0; d < n; d++) {
                auto r = reach(d);
                for (int v = 0; v < n; v++) if (v != d && base[v] && !r[v]) doms[v].push_back(d);
            }
            auto idom = dominator_tree(g, root);
            for (int v = 0; v < n; v++) {
                if (!base[v]) { assert(idom[v] == -1); continue; }
                if (v == root) { assert(idom[v] == root); continue; }
                // 直接支配点 = 支配点のうち、他のすべての支配点に支配されているもの（= 支配点の数が一番多いもの）
                int want = -1;
                for (int d : doms[v]) if (want == -1 || doms[d].size() > doms[want].size()) want = d;
                assert(idom[v] == want);
            }
        }
    }
    puts("OK");
}
