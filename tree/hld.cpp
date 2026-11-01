// =====================================================================
// tree/hld.cpp  HLD（重軽分解 / Heavy-Light Decomposition）
//   木の頂点に番号 pos[v] を振り直して、
//     ・u-v パス = O(log N) 個の番号の区間
//     ・v の部分木 = 1 個の区間 [pos[v], pos[v] + sz[v])
//   にする。区間は ACL の segtree / lazy_segtree / fenwick_tree で処理する
//   → パス上の和・最大値・一括加算（値の更新あり）が O(log^2 N)
//   前計算 O(N)。再帰なし。tree.cpp / euler_tour.cpp とは独立
// 依存: tree/tree.cpp の Tree（先に貼る）。区間の処理に ACL のデータ構造を使う
// ABC / ARC での使用例:
//   ABC294 G - Distance Queries on a Tree  https://atcoder.jp/contests/abc294/tasks/abc294_g
//     → 辺の重みを子の位置に置き、path_ranges(u, v, true) の区間の和を BIT で
//   ABC014 D - 閉路  https://atcoder.jp/contests/abc014/tasks/abc014_4
//     → 答え = dist(a, b) + 1（HLD の lca でも求まる）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/fenwicktree>
#include <atcoder/segtree>
using namespace std;
// テスト用: tree.cpp の Tree を読み込む（提出するときは、tree.cpp のコピー範囲をこのファイルのコピー範囲より先に貼る）
#define TREE_NO_MAIN
#include "tree.cpp"

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体（tree.cpp の Tree が必要） ----------
// HLD<> hld(n);  hld.add_edge(a, b);  hld.build(root);    Tree と同じ使い方（Tree の関数 lca・dist・path なども使える）
//   hld.pos[v]                    頂点 v の番号（segtree の添字に使う）
//   hld.path_ranges(u, v, edge)   u-v パスを表す区間 [l, r) のリスト
//                                 edge = true なら「辺」のパス（LCA の頂点を含めない）
//                                 → 辺 (p, c) の値は子 c の位置 pos[c] に置く
//   hld.subtree(v)                v の部分木の区間 [l, r)
//   hld.par[v], hld.dep[v], hld.sz[v]（Tree のもの）
// 隣接リスト g がもうあるなら HLD<> hld(g, root); でもよい（add_edge と build をまとめてやる）
// 例: パスの和（segtree<long long, op, e> seg(n) に seg.set(pos[v], val[v]) しておく）
//   long long s = 0; for (auto [l, r] : hld.path_ranges(u, v)) s += seg.prod(l, r);
// 注意: 区間の向きは区別しない。演算の順番に意味がある（非可換な）とき は別の工夫が要る
// ◆ struct HLD<T> : HL 分解（パスを O(log N) 個の連続した区間に分ける）。Tree<T> を継承
//   HLD(n)                     : 頂点 0..n−1（辺はまだない）
//   HLD(g, root)               : 隣接リスト g（両方向）から作って、すぐ build(root) する
//   add_edge(a, b, w)          : Tree と同じ
//   build(root)                : Tree の前計算（O(N log N)）＋ HL 分解（O(N)）
//   pos[v]                     : 頂点 v の位置（segtree の添字）
//   path_ranges(u, v, edge)    : u-v パスを表す区間 [l, r) の一覧（O(log N) 個）
//   subtree(v)                 : v の部分木の区間 [l, r)
//   ※ Tree の path(u, v)（頂点の並び）はそのまま残っている。区間がほしいときは path_ranges
//   T : 辺の重みの型（既定 long long）
template <class T = long long>
struct HLD : Tree<T> {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/hld.cpp
    using B = Tree<T>;
    vector<int> heavy, head, pos;
    // n_ : 頂点数
    HLD(int n_) : B(n_) {}
    // g : 木の隣接リスト（両方向）、root : 根（省略で 0）
    HLD(const vector<vector<int>>& g, int root = 0) : B(g) { build(root); }
    // build(root) : root : 根（省略で 0）。辺を全部足してから呼ぶ
    void build(int root = 0) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/hld.cpp
        B::build(root);
        int n = this->n;
        const auto &ch = this->children, &sz = this->sz;
        heavy.assign(n, -1), head.assign(n, 0), pos.assign(n, 0);
        for (int v = 0; v < n; v++)
            for (int to : ch[v])
                if (heavy[v] == -1 || sz[to] > sz[heavy[v]]) heavy[v] = to;  // ★ 一番大きい子が heavy
        int t = 0;
        vector<int> st = {root};
        head[root] = root;
        while (!st.empty()) {
            int v = st.back();
            st.pop_back();
            pos[v] = t++;
            for (int to : ch[v])
                if (to != heavy[v]) head[to] = to, st.push_back(to);  // ★ 軽い子は新しい鎖の先頭
            if (heavy[v] != -1) head[heavy[v]] = head[v], st.push_back(heavy[v]);    // ★ 重い子を最後に積む → 次に取り出され、番号が連続する
        }
    }
    // path_ranges(u, v, edge) : u, v : パスの両端
    //   edge : true なら辺のパス（LCA の頂点を含めない。辺の値は子の側の pos に置いておく）。既定 false（頂点のパス）
    //   注意 : 区間の向き（u 側から v 側か）は区別しない。順番に意味がある演算では工夫が要る
    vector<pair<long long, long long>> path_ranges(int u, int v, bool edge = false) const {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/hld.cpp
        const auto &dep = this->dep, &par = this->par;
        vector<pair<long long, long long>> res;
        while (head[u] != head[v]) {
            if (dep[head[u]] < dep[head[v]]) swap(u, v);
            res.push_back({pos[head[u]], pos[u] + 1});  // ★ u から鎖の先頭までは番号が連続
            u = par[head[u]];                           // ★ 鎖の外へ出る（軽い辺は高々 log N 本）
        }
        if (pos[u] > pos[v]) swap(u, v);
        if (pos[u] + edge < pos[v] + 1) res.push_back({pos[u] + edge, pos[v] + 1});  // u が LCA。edge なら LCA を除く
        return res;
    }
    // subtree(v) : v の部分木の区間 [pos[v], pos[v] + sz[v])
    pair<long long, long long> subtree(int v) const { return {pos[v], pos[v] + this->sz[v]}; }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
long long op_max(long long a, long long b) { return max(a, b); }
long long e_max() { return LLONG_MIN; }
int main() {
    // パスの最大値（頂点の値の更新あり）を segtree で
    HLD<> hld(6);  // Tree と同じく add_edge → build
    hld.add_edge(0, 1), hld.add_edge(1, 2), hld.add_edge(1, 3), hld.add_edge(0, 4), hld.add_edge(4, 5);
    hld.build(0);
    vector<long long> val = {5, 1, 9, 2, 7, 3};
    atcoder::segtree<long long, op_max, e_max> seg(6);
    for (int v = 0; v < 6; v++) seg.set(hld.pos[v], val[v]);
    auto path_max = [&](int u, int v) {
        long long r = LLONG_MIN;
        for (auto [l, rr] : hld.path_ranges(u, v)) r = max(r, seg.prod(l, rr));
        return r;
    };
    assert(path_max(3, 5) == 7 && path_max(2, 3) == 9 && hld.lca(2, 5) == 0 && hld.dist(2, 5) == 4);
    seg.set(hld.pos[4], 0);
    assert(path_max(3, 5) == 5);
    assert((hld.path(3, 5) == vector<long long>{3, 1, 0, 4, 5}));  // Tree の path（頂点の並び）も使える
    auto [l, r] = hld.subtree(1);
    assert(r - l == 3);

    // ランダムテスト: パス上の頂点集合・辺集合・部分木・LCA を愚直と比較
    mt19937 rng(13);
    for (int it = 0; it < 300; it++) {
        int n = rng() % 25 + 1;
        vector<vector<int>> h(n);
        vector<int> p(n, -1);
        for (int v = 1; v < n; v++) p[v] = rng() % v, h[p[v]].push_back(v), h[v].push_back(p[v]);
        int root = rng() % n;
        HLD<> d(h, root);
        // 根を root にしたときの親を愚直に求める
        vector<int> P(n, -1), D(n, 0), q = {root};
        vector<bool> seen(n, false);
        seen[root] = true;
        for (int i = 0; i < (int)q.size(); i++)
            for (int to : h[q[i]]) if (!seen[to]) seen[to] = true, P[to] = q[i], D[to] = D[q[i]] + 1, q.push_back(to);
        vector<int> inv(n);
        for (int v = 0; v < n; v++) inv[d.pos[v]] = v;
        for (int a = 0; a < n; a++)
            for (int b = 0; b < n; b++) {
                set<int> ref;
                int x = a, y = b;
                while (x != y) { if (D[x] < D[y]) swap(x, y); ref.insert(x), x = P[x]; }
                int L = x;
                for (int edge = 0; edge < 2; edge++) {
                    set<int> got, want = ref;
                    if (!edge) want.insert(L);
                    for (auto [lo, hi] : d.path_ranges(a, b, edge))
                        for (int k = lo; k < hi; k++) assert(got.insert(inv[k]).second);  // 重複なし
                    assert(got == want);
                }
                assert(d.lca(a, b) == L);
            }
        for (int v = 0; v < n; v++) {
            auto [lo, hi] = d.subtree(v);
            for (int k = lo; k < hi; k++) {  // 区間の頂点はすべて v の子孫
                int x = inv[k];
                while (x != -1 && x != v) x = P[x];
                assert(x == v);
            }
        }
    }
    // 区間の個数は O(log N): 子を大きさの順にしない木（後から付く子ほど大きい）でも、どのパスも 2 log2 N + 2 個以下
    {
        mt19937 r2(31);
        int n = 4000;
        HLD<> big(n);
        for (int v = 1; v < n; v++) big.add_edge(r2() % 2 ? v - 1 : (int)(r2() % v), v);
        big.build(0);
        int lim = 2 * __lg(n) + 2;
        for (int q = 0; q < 2000; q++) assert((int)big.path_ranges(r2() % n, r2() % n).size() <= lim);
        HLD<> line(n);  // 一直線なら区間は 1 個
        for (int v = 1; v < n; v++) line.add_edge(v - 1, v);
        line.build(0);
        assert(line.path_ranges(0, n - 1).size() == 1);
        // 背骨 0-2-4-...（偶数）の各頂点に、先に葉（奇数）を付けてから次の背骨を付ける → 最初の子は葉
        int m = n - 1;  // 頂点 0..m−1（m は奇数なので、最後の背骨 m−1 まで全部つながる）
        HLD<> broom(m);
        for (int v = 0; v + 2 < m; v += 2) broom.add_edge(v, v + 1), broom.add_edge(v, v + 2);
        broom.build(0);
        assert(broom.path_ranges(0, m - 1).size() <= 2);  // 背骨はほぼ 1 本の重い鎖になる（最後だけ葉と大きさが同じ）
    }
    puts("OK");
}
