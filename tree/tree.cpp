// =====================================================================
// tree/tree.cpp  木の基本情報 + LCA（ダブリング）+ 木の直径・重心
//   親・深さ・部分木サイズ・高さ・BFS 順を前計算（O(N)）。LCA・距離・k 個上の祖先を O(log N)、直径、重心
//   LCA の表（ダブリング）は、lca などを初めて呼んだときに作る（使わなければ作らない）
//   前提: 頂点 0..n-1 の連結な木
// 依存: なし
// ABC / ARC での使用例:
//   ABC014 D - 閉路  https://atcoder.jp/contests/abc014/tasks/abc014_4
//     → 答え = dist(a, b) + 1（LCA）
//   ABC361 E - Tree and Hamilton Path 2  https://atcoder.jp/contests/abc361/tasks/abc361_e
//     → 答え = 2 × (辺の重みの和) − 直径（diameter）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// Tree<long long> t(n);  t.add_edge(a, b) / t.add_edge(a, b, w);  t.build(root);
// build 後: par[v]（根は -1）, dep[v], wdep[v]（重み付き深さ）, sz[v], children[v], order（BFS 順）
//           lca(a, b), dist(a, b), wdist(a, b), kth_ancestor(v, k)
// 木 DP : order を逆順に回すと「子 → 親」の順になる
// ◆ struct Tree : 木の基本情報（親・深さ・部分木サイズ・BFS 順）と LCA（ダブリング）
//   Tree(n)              : 頂点 0..n−1 の木を作る準備（辺はまだない）
//   Tree(g)              : 隣接リスト g（両方向）から辺をまとめて入れる（build は自分で呼ぶ）
//   add_edge(a, b, w)    : 無向辺を足す（w 省略で 1）
//   read(weighted, one)  : 「a b」（weighted なら「a b c」）が n − 1 行の入力を読む（one = true（既定）なら 1 始まり）
//   read_parents(one)    : 「p_2 … p_n」（頂点 i の親が p_i）の入力を読む
//   build(root)          : 根を決めて前計算する。O(N)。辺を全部足してから呼ぶ
//   kth_ancestor(v, k)   : v の k 個上の祖先（なければ −1）。O(log N)
//     ※ kth_ancestor・lca と、それを使う dist・wdist・jump・path・is_on_path は、初めて呼んだときに
//        LCA の表を作る（O(N log N)、1 回だけ）。使わなければ作らないので、木 DP などでは余計な時間がかからない
//   lca(a, b)            : 最小共通祖先。O(log N)
//   dist(a, b)           : 辺の本数での距離、wdist(a, b) : 重みでの距離
//   jump(a, b, k)        : a から b へ k 歩進んだ頂点（はみ出したら −1）
//   path(a, b)           : a から b への頂点の並び。O(パスの長さ)
//   is_on_path(a, b, c)  : c が a-b パス上にあるか
//   diameter()           : 直径 {長さ, 端から端への頂点の並び}（build 不要）
//   centroids()          : 木の重心（取り除くと残りがどれも N/2 以下になる頂点）。1 個か 2 個、小さい順（build 不要）
//   ※ 他の木のライブラリ（tree_dp, rerooting, euler_tour, hld, centroid, tree_hash, auxiliary_tree）は
//      この Tree を継承している。使い方は全部同じ: X x(n); x.add_edge(a, b); x.build(root);
//      lca・dist・path なども全部そのまま使える。使うときはこの Tree も一緒に貼る（Tree を先に）
//   T : 辺の重みの型（既定 long long）
//   【変数】（build の後に使える。adj は add_edge の後から）。例は根 0、辺 0-1, 0-2, 1-3 の木
//   n              : 頂点数
//   adj[v]         : v の隣の {頂点, 辺の重み} の一覧（両方向）                    例: adj[1] = {{0, 1}, {3, 1}}
//   par[v]         : 親（根は −1）                                                  例: par[3] = 1, par[0] = −1
//   dep[v]         : 根からの辺の本数                                               例: dep[3] = 2
//   wdep[v]        : 根からの重みの和                                               （重みがすべて 1 なら dep と同じ）
//   sz[v]          : v の部分木の頂点数（v 自身を含む）                             例: sz[1] = 2, sz[0] = 4
//   height[v]      : v の部分木の高さ（v から一番深い子孫までの辺の本数。葉は 0）     例: height[0] = 2, height[1] = 1
//   children[v]    : 子の一覧                                                       例: children[0] = {1, 2}
//   order          : 根から近い順（BFS 順）の頂点の並び。order[0] が根               例: {0, 1, 2, 3}
//                    前から回すと「親が先」、後ろから回すと「子が先」（木 DP はこれ）
//   up[k][v], LOG  : （内部用）v の 2^k 個上の祖先（根より上は根）。lca などを初めて呼んだときに作る。LOG は k の個数
template <class T = long long>
struct Tree {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree.cpp
    int n;                              // 頂点数
    mutable int LOG = 0;                // up の段数（内部用。表を作るまでは 0）
    vector<vector<pair<int, T>>> adj;   // adj[v] = v の隣の {頂点, 重み}
    vector<int> par, dep, sz, order;    // 親（根は −1）、根からの辺の本数、部分木の頂点数、根から近い順の並び
    vector<int> height;                 // 部分木の高さ（v から一番深い子孫までの辺の本数）
    vector<T> wdep;                     // 根からの重みの和
    vector<vector<int>> children;       // 子の一覧
    mutable vector<vector<int>> up;     // up[k][v] = v の 2^k 個上（内部用。lca などを初めて呼んだときに作る）

    // n_ : 頂点数
    Tree(int n_) : n(n_), adj(n_) {}
    // g : 隣接リスト（両方向。重みはすべて 1）。add_edge をまとめてやる（build はしない）
    Tree(const vector<vector<int>>& g) : n(g.size()), adj(g.size()) {
        for (int v = 0; v < n; v++) for (int to : g[v]) if (v < to) add_edge(v, to);
    }
    // add_edge(a, b, w) : a, b : 端点（0 始まり）、w : 重み（省略で 1。wdep / wdist / diameter で使う）
    void add_edge(int a, int b, T w = T(1)) { adj[a].push_back({b, w}), adj[b].push_back({a, w}); }
    // --- 削除可: 入力 ---
    // read(weighted, one_indexed) : 「a b」（weighted なら「a b c」）が n − 1 行の入力を読んで辺を足す。one_indexed : 1 始まりか（既定 true）
    void read(bool weighted = false, bool one_indexed = true) {
        for (int i = 0; i < n - 1; i++) {
            int a, b;
            T w = T(1);
            cin >> a >> b;
            if (weighted) cin >> w;
            if (one_indexed) a--, b--;
            add_edge(a, b, w);
        }
    }
    // read_parents(one_indexed) : 「p_2 p_3 … p_n」（頂点 i の親が p_i）の形の入力を読んで辺を足す（頂点 1 が根）
    void read_parents(bool one_indexed = true) {
        for (int v = 1; v < n; v++) {
            int p;
            cin >> p;
            if (one_indexed) p--;
            add_edge(p, v);
        }
    }
    // --- 削除可ここまで ---

    // build(root) : root : 根（省略で 0）。木がつながっていないと、つながっていない頂点の情報は壊れる
    void build(int root = 0) {
        par.assign(n, -1), dep.assign(n, 0), sz.assign(n, 1), height.assign(n, 0), wdep.assign(n, T(0)), children.assign(n, {});
        order = {root};
        for (int i = 0; i < (int)order.size(); i++) {  // ★ 根から BFS（再帰を使わないので深い木でも安全）
            int v = order[i];
            for (auto [to, w] : adj[v]) {
                if (to == par[v]) continue;
                par[to] = v, dep[to] = dep[v] + 1, wdep[to] = wdep[v] + w;
                children[v].push_back(to);
                order.push_back(to);
            }
        }
        for (int i = n - 1; i > 0; i--) {  // ★ 葉の方から親に集める
            int v = order[i];
            sz[par[v]] += sz[v];
            height[par[v]] = max(height[par[v]], height[v] + 1);
        }
        up.clear(), LOG = 0;  // LCA の表は使うときに作り直す
    }

    // prepare_lca() : LCA の表を作る（作ってあれば何もしない）。kth_ancestor・lca が自動で呼ぶので、普通は呼ばなくてよい
    void prepare_lca() const {
        // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree.cpp
        if (!up.empty()) return;
        LOG = 1;
        while ((1 << LOG) <= n) LOG++;
        up.assign(LOG, vector<int>(n));
        for (int v = 0; v < n; v++) up[0][v] = par[v] == -1 ? v : par[v];  // 根の親は根自身にしておく
        for (int k = 0; k + 1 < LOG; k++)
            for (int v = 0; v < n; v++) up[k + 1][v] = up[k][up[k][v]];  // ★ 2^k 上の 2^k 上 = 2^(k+1) 上
    }

    // kth_ancestor(v, k) : v : 頂点、k : 何個上か（0 なら v 自身。深さを超えると −1）
    long long kth_ancestor(int v, int k) const {  // 存在しなければ -1
        if (k < 0 || k > dep[v]) return -1;
        prepare_lca();
        for (int i = 0; i < LOG; i++)
            if (k >> i & 1) v = up[i][v];  // ★ k を 2 進数で分解して飛ぶ
        return v;
    }
    // lca(a, b) : a, b : 頂点（build 済みであること）
    long long lca(int a, int b) const {
        prepare_lca();
        if (dep[a] < dep[b]) swap(a, b);
        a = kth_ancestor(a, dep[a] - dep[b]);  // ★ まず深さをそろえる
        if (a == b) return a;
        for (int k = LOG - 1; k >= 0; k--)
            if (up[k][a] != up[k][b]) a = up[k][a], b = up[k][b];  // ★ 合流しない範囲で大きく飛ぶ
        return up[0][a];                                          // ★ その 1 つ上が合流点
    }
    // dist(a, b) : 辺の本数
    long long dist(int a, int b) const { return dep[a] + dep[b] - 2 * dep[lca(a, b)]; }
    // wdist(a, b) : 辺の重みの和
    T wdist(int a, int b) const { return wdep[a] + wdep[b] - 2 * wdep[lca(a, b)]; }

    // --- 削除可: パス関連 ---
    // jump(a, b, k) : k : 進む歩数（0 なら a、dist(a, b) なら b、それより大きいと −1）
    long long jump(int a, int b, int k) const {  // a から b へ k 歩進んだ頂点（はみ出したら -1）
        int l = lca(a, b), da = dep[a] - dep[l], db = dep[b] - dep[l];
        if (k <= da) return kth_ancestor(a, k);
        if (k <= da + db) return kth_ancestor(b, da + db - k);
        return -1;
    }
    // path(a, b) : a から b への頂点の並び（a と b を含む）
    vector<long long> path(int a, int b) const {  // a から b へのパスの頂点列
        int l = lca(a, b);
        vector<long long> left, right;
        for (int v = a; v != l; v = par[v]) left.push_back(v);
        left.push_back(l);
        for (int v = b; v != l; v = par[v]) right.push_back(v);
        left.insert(left.end(), right.rbegin(), right.rend());
        return left;
    }
    // is_on_path(a, b, c) : c が a-b パス上（端点を含む）にあるか
    bool is_on_path(int a, int b, int c) const { return dist(a, c) + dist(c, b) == dist(a, b); }
    // --- 削除可ここまで ---

    // --- 削除可: 木の直径・木の重心（どちらも build 不要）。tree_hash.cpp を使うなら centroids は消さない ---
    // diameter() : build しなくても呼べる。重みは add_edge の w（負の重みがあると正しくない）
    pair<T, vector<long long>> diameter() const {
        auto far = [&](int s, vector<int>& pre) {  // s から一番遠い頂点
            vector<T> d(n, T(0));
            pre.assign(n, -1);
            vector<int> st = {s};
            vector<bool> seen(n, false);
            seen[s] = true;
            int best = s;
            while (!st.empty()) {
                int v = st.back();
                st.pop_back();
                if (d[v] > d[best]) best = v;
                for (auto [to, w] : adj[v])
                    if (!seen[to]) seen[to] = true, d[to] = d[v] + w, pre[to] = v, st.push_back(to);
            }
            return make_pair(best, d[best]);
        };
        vector<int> pre;
        int u = far(0, pre).first;   // ★ 適当な点から一番遠い点 u
        auto [v, len] = far(u, pre);  // ★ u から一番遠い点 v。u-v が直径
        vector<long long> p;
        for (int x = v; x != -1; x = pre[x]) p.push_back(x);
        return {len, p};
    }

    // centroids() : 2 個のときは隣どうし（その間の辺で木がちょうど半分に分かれる）
    vector<long long> centroids() const {
        // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree.cpp
        vector<int> p(n, -1), ord = {0}, s(n, 1);
        for (size_t i = 0; i < ord.size(); i++)
            for (auto [to, w] : adj[ord[i]]) if (to != p[ord[i]]) p[to] = ord[i], ord.push_back(to);
        for (int i = n - 1; i > 0; i--) s[p[ord[i]]] += s[ord[i]];  // 頂点 0 を根にした部分木の大きさ
        vector<long long> res;
        for (int v = 0; v < n; v++) {
            int mx = n - s[v];  // ★ v を取り除いたときの、親の側の大きさ
            for (auto [to, w] : adj[v]) if (to != p[v]) mx = max(mx, s[to]);
            if (mx * 2 <= n) res.push_back(v);
        }
        return res;
    }
    // --- 削除可ここまで ---
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// 他の木のファイルのテストは、ここより上を #include して使う（TREE_NO_MAIN を定義してから）
#ifndef TREE_NO_MAIN
// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    //       0
    //    2/   \3
    //    1     2
    //  4/ \1    \10
    //  3   4     5
    Tree<long long> t(6);
    t.add_edge(0, 1, 2), t.add_edge(0, 2, 3), t.add_edge(1, 3, 4), t.add_edge(1, 4, 1), t.add_edge(2, 5, 10);
    t.build(0);
    assert(t.up.empty());  // build しただけでは LCA の表はまだない
    assert(t.lca(3, 4) == 1 && t.lca(3, 5) == 0 && t.dist(3, 5) == 4 && t.wdist(3, 5) == 19);
    assert(t.kth_ancestor(3, 2) == 0 && t.kth_ancestor(3, 3) == -1 && t.sz[1] == 3);
    assert(t.height[0] == 2 && t.height[1] == 1 && t.height[5] == 0 && t.height[2] == 1);
    assert(t.jump(3, 5, 3) == 2 && t.jump(3, 5, 5) == -1);
    assert((t.path(4, 5) == vector<long long>{4, 1, 0, 2, 5}) && t.is_on_path(4, 5, 0));
    assert(t.diameter().first == 19);
    assert(!t.up.empty());
    t.build(5);  // 根を変えて build し直すと表も作り直される
    assert(t.up.empty() && t.lca(3, 4) == 1 && t.lca(0, 3) == 0 && t.lca(3, 2) == 2 && t.kth_ancestor(3, 4) == 5);
    t.build(0);
    assert((t.centroids() == vector<long long>{0, 1}));
    {   // 入力を読む
        istringstream in("1 2\n1 3\n3 4\n1 1 3\n");
        auto* old = cin.rdbuf(in.rdbuf());
        Tree<> r(4), q(4);
        r.read(), q.read_parents();
        cin.rdbuf(old);
        r.build(0), q.build(0);
        assert(r.par[3] == 2 && r.dist(1, 3) == 3 && q.par[1] == 0 && q.par[2] == 0 && q.par[3] == 2);
    }  // 0 か 1 を取ると残りは 3 個以下
    {   // ランダムな木で、重心を「取り除いて最大の成分が N/2 以下」の愚直と比べる
        mt19937 rc(777);
        for (int it = 0; it < 500; it++) {
            int n = rc() % 30 + 1;
            Tree<> u(n);
            vector<vector<int>> h(n);
            for (int v = 1; v < n; v++) { int p = rc() % v; u.add_edge(p, v), h[p].push_back(v), h[v].push_back(p); }
            vector<long long> want;
            for (int c = 0; c < n; c++) {
                vector<int> seen(n, 0);
                seen[c] = 1;
                int mx = 0;
                for (int s0 = 0; s0 < n; s0++) {
                    if (seen[s0]) continue;
                    vector<int> q = {s0};
                    seen[s0] = 1;
                    for (size_t i = 0; i < q.size(); i++) for (int to : h[q[i]]) if (!seen[to]) seen[to] = 1, q.push_back(to);
                    mx = max(mx, (int)q.size());
                }
                if (mx * 2 <= n) want.push_back(c);
            }
            assert(u.centroids() == want);
            int r = rc() % n;
            u.build(r);
            for (int v = 0; v < n; v++) {  // 高さ: v の子孫のうち一番深いもの（愚直）
                int best = 0;
                for (int x = 0; x < n; x++) {
                    int y = x;
                    while (y != -1 && y != v) y = u.par[y];
                    if (y == v) best = max(best, u.dep[x] - u.dep[v]);
                }
                assert(u.height[v] == best);
            }
        }
    }

    // 木 DP の例: 部分木に含まれる重みの合計（葉から順に処理）
    vector<long long> sub(6, 0);
    for (int i = 5; i >= 0; i--) {
        int v = t.order[i];
        for (int c : t.children[v]) sub[v] += sub[c] + (t.wdep[c] - t.wdep[v]);
    }
    assert(sub[0] == 20);

    // ランダムテスト: 親をたどる愚直な方法と、すべての機能を比較
    {
        mt19937 rr(119);
        for (int it = 0; it < 300; it++) {
            int n = rr() % 15 + 1;
            Tree<long long> tt(n);
            for (int v = 1; v < n; v++) tt.add_edge(rr() % v, v, rr() % 10);
            int root = rr() % n;
            tt.build(root);
            vector<int> P(n, -1), D(n, 0), q = {root};
            vector<long long> WD(n, 0);
            vector<bool> seen(n, false);
            seen[root] = true;
            for (size_t i = 0; i < q.size(); i++)
                for (auto [to, w] : tt.adj[q[i]])
                    if (!seen[to]) seen[to] = true, P[to] = q[i], D[to] = D[q[i]] + 1, WD[to] = WD[q[i]] + w, q.push_back(to);
            auto is_desc = [&](int u, int v) { while (u != -1 && u != v) u = P[u]; return u == v; };  // u は v の子孫か
            for (int v = 0; v < n; v++) {
                assert(tt.par[v] == P[v] && tt.dep[v] == D[v] && tt.wdep[v] == WD[v]);
                int s = 0;
                for (int u = 0; u < n; u++) s += is_desc(u, v);
                assert(tt.sz[v] == s);
                for (int k = -1; k <= n; k++) {
                    int x = v;
                    for (int j = 0; j < k && x != -1; j++) x = P[x];
                    assert(tt.kth_ancestor(v, k) == (k < 0 ? -1 : x));
                }
            }
            auto naive_lca = [&](int a, int b) {
                while (D[a] > D[b]) a = P[a];
                while (D[b] > D[a]) b = P[b];
                while (a != b) a = P[a], b = P[b];
                return a;
            };
            long long dm = 0;
            for (int a = 0; a < n; a++)
                for (int b = 0; b < n; b++) {
                    int l = naive_lca(a, b);
                    assert(tt.lca(a, b) == l && tt.dist(a, b) == D[a] + D[b] - 2 * D[l] && tt.wdist(a, b) == WD[a] + WD[b] - 2 * WD[l]);
                    dm = max(dm, WD[a] + WD[b] - 2 * WD[l]);
                    vector<long long> path, tail;
                    for (int x = a; x != l; x = P[x]) path.push_back(x);
                    path.push_back(l);
                    for (int x = b; x != l; x = P[x]) tail.push_back(x);
                    path.insert(path.end(), tail.rbegin(), tail.rend());
                    assert(tt.path(a, b) == path);
                    for (int k = 0; k <= (int)path.size(); k++) assert(tt.jump(a, b, k) == (k < (int)path.size() ? path[k] : -1));
                    for (int c = 0; c < n; c++) assert(tt.is_on_path(a, b, c) == (find(path.begin(), path.end(), c) != path.end()));
                }
            auto [len, dp] = tt.diameter();
            assert(len == dm && tt.wdist(dp.front(), dp.back()) == len);
        }
    }
    puts("OK");
}
#endif
