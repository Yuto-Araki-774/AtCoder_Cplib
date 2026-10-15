// =====================================================================
// tree/tree.cpp  木の基本情報 + LCA（ダブリング）+ 木の直径
//   親・深さ・部分木サイズ・BFS 順を前計算。LCA・距離・k 個上の祖先を O(log N)、直径、木上のいもす法
//   前提: 頂点 0..n-1 の連結な木
// 依存: なし
// ABC / ARC での使用例:
//   ABC014 D - 閉路  https://atcoder.jp/contests/abc014/tasks/abc014_4
//     → 答え = dist(a, b) + 1（LCA）
//   ABC361 E - Tree and Hamilton Path 2  https://atcoder.jp/contests/abc361/tasks/abc361_e
//     → 答え = 2 × (辺の重みの和) − 直径（diameter）
//   ABC138 D - Ki  https://atcoder.jp/contests/abc138/tasks/abc138_d
//     → 部分木に加算（TreeImos の add_subtree）
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
//   add_edge(a, b, w)    : 無向辺を足す（w 省略で 1）
//   build(root)          : 根を決めて前計算する。O(N log N)。辺を全部足してから呼ぶ
//   kth_ancestor(v, k)   : v の k 個上の祖先（なければ −1）。O(log N)
//   lca(a, b)            : 最小共通祖先。O(log N)
//   dist(a, b)           : 辺の本数での距離、wdist(a, b) : 重みでの距離
//   jump(a, b, k)        : a から b へ k 歩進んだ頂点（はみ出したら −1）
//   path(a, b)           : a から b への頂点の並び。O(パスの長さ)
//   is_on_path(a, b, c)  : c が a-b パス上にあるか
//   diameter()           : 直径 {長さ, 端から端への頂点の並び}（build 不要）
//   T : 辺の重みの型（既定 long long）
template <class T = long long>
struct Tree {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree.cpp
    int n, LOG = 1;
    vector<vector<pair<int, T>>> adj;
    vector<int> par, dep, sz, order;
    vector<T> wdep;
    vector<vector<int>> children, up;  // up[k][v] = v の 2^k 個上

    // n_ : 頂点数
    Tree(int n_) : n(n_), adj(n_) {}
    // add_edge(a, b, w) : a, b : 端点（0 始まり）、w : 重み（省略で 1。wdep / wdist / diameter で使う）
    void add_edge(int a, int b, T w = T(1)) { adj[a].push_back({b, w}), adj[b].push_back({a, w}); }

    // build(root) : root : 根（省略で 0）。木がつながっていないと、つながっていない頂点の情報は壊れる
    void build(int root = 0) {
        par.assign(n, -1), dep.assign(n, 0), sz.assign(n, 1), wdep.assign(n, T(0)), children.assign(n, {});
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
        for (int i = n - 1; i > 0; i--) sz[par[order[i]]] += sz[order[i]];  // ★ 葉の方から親に足す
        while ((1 << LOG) <= n) LOG++;
        up.assign(LOG, vector<int>(n));
        for (int v = 0; v < n; v++) up[0][v] = par[v] == -1 ? v : par[v];  // 根の親は根自身にしておく
        for (int k = 0; k + 1 < LOG; k++)
            for (int v = 0; v < n; v++) up[k + 1][v] = up[k][up[k][v]];  // ★ 2^k 上の 2^k 上 = 2^(k+1) 上
    }

    // kth_ancestor(v, k) : v : 頂点、k : 何個上か（0 なら v 自身。深さを超えると −1）
    int kth_ancestor(int v, int k) const {  // 存在しなければ -1
        if (k < 0 || k > dep[v]) return -1;
        for (int i = 0; i < LOG; i++)
            if (k >> i & 1) v = up[i][v];  // ★ k を 2 進数で分解して飛ぶ
        return v;
    }
    // lca(a, b) : a, b : 頂点（build 済みであること）
    int lca(int a, int b) const {
        if (dep[a] < dep[b]) swap(a, b);
        a = kth_ancestor(a, dep[a] - dep[b]);  // ★ まず深さをそろえる
        if (a == b) return a;
        for (int k = LOG - 1; k >= 0; k--)
            if (up[k][a] != up[k][b]) a = up[k][a], b = up[k][b];  // ★ 合流しない範囲で大きく飛ぶ
        return up[0][a];                                          // ★ その 1 つ上が合流点
    }
    // dist(a, b) : 辺の本数
    int dist(int a, int b) const { return dep[a] + dep[b] - 2 * dep[lca(a, b)]; }
    // wdist(a, b) : 辺の重みの和
    T wdist(int a, int b) const { return wdep[a] + wdep[b] - 2 * wdep[lca(a, b)]; }

    // --- 削除可: パス関連 ---
    // jump(a, b, k) : k : 進む歩数（0 なら a、dist(a, b) なら b、それより大きいと −1）
    int jump(int a, int b, int k) const {  // a から b へ k 歩進んだ頂点（はみ出したら -1）
        int l = lca(a, b), da = dep[a] - dep[l], db = dep[b] - dep[l];
        if (k <= da) return kth_ancestor(a, k);
        if (k <= da + db) return kth_ancestor(b, da + db - k);
        return -1;
    }
    // path(a, b) : a から b への頂点の並び（a と b を含む）
    vector<int> path(int a, int b) const {  // a から b へのパスの頂点列
        int l = lca(a, b);
        vector<int> left, right;
        for (int v = a; v != l; v = par[v]) left.push_back(v);
        left.push_back(l);
        for (int v = b; v != l; v = par[v]) right.push_back(v);
        left.insert(left.end(), right.rbegin(), right.rend());
        return left;
    }
    // is_on_path(a, b, c) : c が a-b パス上（端点を含む）にあるか
    bool is_on_path(int a, int b, int c) const { return dist(a, c) + dist(c, b) == dist(a, b); }
    // --- 削除可ここまで ---

    // --- 削除可: 木の直径（build 不要）。{長さ, 端点から端点への頂点列} ---
    // diameter() : build しなくても呼べる。重みは add_edge の w（負の重みがあると正しくない）
    pair<T, vector<int>> diameter() const {
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
        vector<int> p;
        for (int x = v; x != -1; x = pre[x]) p.push_back(x);
        return {len, p};
    }
    // --- 削除可ここまで ---
};

// ---------- 削除可: 木上のいもす法（上の Tree が必要。build 済みであること） ----------
// 頂点に値を足す操作をまとめて行い、最後に各頂点の値を一括で得る
// U : 足す値の型
// TreeImos<long long, long long> im(t);
//   im.add_subtree(v, x)  v の部分木全体に x を足す      O(1)
//   im.add_path(u, v, x)  u-v パス上の全頂点に x を足す   O(log N)（LCA を使う）
//   im.build()            各頂点の最終的な値           O(N)
// 例: 「パス u-v を通る回数」= 各パスに add_path(u, v, 1)
// ◆ struct TreeImos : 木の上のいもす法（部分木やパスへの加算をまとめて行い、最後に各頂点の値を求める）
//   TreeImos(t)          : t は build 済みの Tree（参照で持つので t を先に作って残しておく）
//   add_subtree(v, x)    : v の部分木の全頂点に x を足す。O(1)
//   add_path(u, v, x)    : u-v パス上の全頂点に x を足す。O(log N)
//   build()              : 各頂点の値を返す。O(N)
//   U : 足す値の型、T : Tree の重みの型
template <class U, class T>
struct TreeImos {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree.cpp
    const Tree<T>& t;
    vector<U> ds, dp;  // ds: 部分木加算用、dp: パス加算用
    // t_ : build 済みの Tree
    TreeImos(const Tree<T>& t_) : t(t_), ds(t_.n, U(0)), dp(t_.n, U(0)) {}
    // add_subtree(v, x) : v : 部分木の根、x : 足す値
    void add_subtree(int v, U x) { ds[v] += x; }  // ★ 根の印だけ付けて、後で下に流す
    // add_path(u, v, x) : u, v : パスの両端（u == v なら u だけ）、x : 足す値
    void add_path(int u, int v, U x) {
        int l = t.lca(u, v);
        dp[u] += x, dp[v] += x;                // ★ 両端に +x
        dp[l] -= x;                            // ★ LCA で 2 回分になるので 1 回引く
        if (t.par[l] != -1) dp[t.par[l]] -= x;  // ★ LCA の親より上には流さない
    }
    // build() : 返り値 res[v] = 頂点 v に足された値の合計
    vector<U> build() const {
        auto s = ds, p = dp;
        for (int v : t.order)
            if (t.par[v] != -1) s[v] += s[t.par[v]];  // ★ 部分木加算: 根 → 葉 に累積
        for (int i = t.n - 1; i > 0; i--) {
            int v = t.order[i];
            p[t.par[v]] += p[v];  // ★ パス加算: 葉 → 根 に累積
        }
        vector<U> res(t.n);
        for (int v = 0; v < t.n; v++) res[v] = s[v] + p[v];
        return res;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

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
    assert(t.lca(3, 4) == 1 && t.lca(3, 5) == 0 && t.dist(3, 5) == 4 && t.wdist(3, 5) == 19);
    assert(t.kth_ancestor(3, 2) == 0 && t.kth_ancestor(3, 3) == -1 && t.sz[1] == 3);
    assert(t.jump(3, 5, 3) == 2 && t.jump(3, 5, 5) == -1);
    assert((t.path(4, 5) == vector<int>{4, 1, 0, 2, 5}) && t.is_on_path(4, 5, 0));
    assert(t.diameter().first == 19);

    // 木 DP の例: 部分木に含まれる重みの合計（葉から順に処理）
    vector<long long> sub(6, 0);
    for (int i = 5; i >= 0; i--) {
        int v = t.order[i];
        for (int c : t.children[v]) sub[v] += sub[c] + (t.wdep[c] - t.wdep[v]);
    }
    assert(sub[0] == 20);

    // 木上のいもす: 部分木 1 に +1、パス 3-5 に +10、パス 4-4 に +100
    TreeImos<long long, long long> im(t);
    im.add_subtree(1, 1);
    im.add_path(3, 5, 10);
    im.add_path(4, 4, 100);
    assert((im.build() == vector<long long>{10, 11, 10, 11, 101, 10}));
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
                    vector<int> path, tail;
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
            TreeImos<long long, long long> im(tt);
            vector<long long> ref(n, 0);
            for (int k = 0; k < 10; k++) {
                long long x = (long long)(rr() % 11) - 5;
                if (rr() % 2) {
                    int v = rr() % n;
                    im.add_subtree(v, x);
                    for (int u = 0; u < n; u++) if (is_desc(u, v)) ref[u] += x;
                } else {
                    int a = rr() % n, b = rr() % n, l = naive_lca(a, b);
                    im.add_path(a, b, x);
                    for (int y = a; y != l; y = P[y]) ref[y] += x;
                    for (int y = b; y != l; y = P[y]) ref[y] += x;
                    ref[l] += x;
                }
            }
            assert(im.build() == ref);
        }
    }
    puts("OK");
}
