// =====================================================================
// tree/centroid.cpp  重心分解
//   木の重心（取り除くと残りの部分木がどれも半分以下になる頂点）を根にして分け、
//   分かれた部分でまた重心を取る…を繰り返して「重心木」を作る。重心木の深さは log N 以下
//   → u-v パスは必ず、重心木での u と v の共通の祖先（重心）c を通る。
//     「各頂点から祖先の重心までの距離」を前計算しておけば、パスに関するクエリが O(log N)
//   前計算 O(N log N)。再帰なし。tree.cpp の Tree を継承していて、使い方は Tree と同じ（cd(n) → add_edge → build）
//   木の重心（1 個か 2 個）だけ欲しいときは Tree の centroids()（このファイルは要らない）
//   削除可: 距離 k 以内の頂点の数（重み付きなら重みの和）を 1 回 O(log N) で答える（前計算 O(N log N)）
// 依存: tree/tree.cpp の Tree（先に貼る）
// ABC / ARC での使用例:
//   ABC291 Ex - Balanced Tree  https://atcoder.jp/contests/abc291/tasks/abc291_h
//     → 求める木がそのまま重心木。cpar を 1-indexed で出力（根は -1）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;
// テスト用: tree.cpp の Tree を読み込む（提出するときは、tree.cpp のコピー範囲をこのファイルのコピー範囲より先に貼る）
#define TREE_NO_MAIN
#include "tree.cpp"

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体（tree.cpp の Tree が必要） ----------
// CentroidDecomposition<> cd(n);  cd.add_edge(a, b);  cd.build();    Tree と同じ使い方（Tree の関数 lca・dist なども使える）
//   cd.root          重心木の根
//   cd.cpar[v]       重心木での親（根は -1）
//   cd.level[v]      重心木での深さ（根が 0）
//   cd.cdist[v][k]   v から「重心木で v の祖先のうち深さ k のもの」までの元の木での距離（辺の本数。k = 0..level[v]）
//   cd.centroids()   木の重心（1 個か 2 個）。Tree の関数なので build 不要
// 祖先をたどる書き方:
//   for (int c = v; c != -1; c = cd.cpar[c]) { int d = cd.cdist[v][cd.level[c]]; ... }   // c は v の祖先の重心、d = dist(v, c)
// 典型（使用例にあり）: 「頂点を赤く塗る」「v から一番近い赤い頂点までの距離」が交互に来る → 1 回 O(log N)
// 隣接リスト g がもうあるなら CentroidDecomposition<> cd(g); でもよい（add_edge と build をまとめてやる）
// ◆ struct CentroidDecomposition<T> : 重心分解（重心木を作る）。O(N log N)、再帰なし。Tree<T> を継承
//   CentroidDecomposition(n) : 頂点 0..n−1 の木を作る準備（辺はまだない。n >= 1）
//   CentroidDecomposition(g) : 隣接リスト g（両方向）から作って、すぐ build する
//   add_edge(a, b, w)        : Tree と同じ（重みは重心分解では見ない）
//   build(r)                 : Tree の前計算（r は Tree の lca などで使う根。省略で 0）＋重心木。O(N log N)
//   root                     : 重心木の根
//   cpar[v], level[v]        : 重心木での親（根は −1）、深さ（根が 0）
//   cdist[v][k]              : v から「重心木での v の祖先のうち深さ k のもの」までの元の木での距離
template <class T = long long>
struct CentroidDecomposition : Tree<T> {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/centroid.cpp
    using B = Tree<T>;
    int root = -1;               // 重心木の根（木全体の重心）
    vector<int> cpar, level;     // cpar[v] = 重心木での親（根は −1）、level[v] = 重心木での深さ（根が 0）
    vector<vector<int>> cdist;   // cdist[v][k] = v から「重心木で深さ k の v の祖先」までの、元の木での辺の本数
    // n_ : 頂点数
    CentroidDecomposition(int n_) : B(n_) {}
    // g : 木の隣接リスト（両方向）。作ったらすぐ build する
    CentroidDecomposition(const vector<vector<int>>& g) : B(g) { build(); }
    // build(r) : 木がつながっていること（辺が N − 1 本）。辺を全部足してから呼ぶ
    void build(int r = 0) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/centroid.cpp
        B::build(r);
        int n = this->n;
        auto g = [&](int v) -> const vector<pair<int, T>>& { return this->adj[v]; };
        root = -1;
        cpar.assign(n, -1), level.assign(n, 0), cdist.assign(n, {});
        vector<bool> removed(n, false);
        vector<int> sz(n), par(n);
        vector<pair<int, int>> st = {{0, -1}};  // {成分の中の頂点 1 つ, その成分の親になる重心}
        while (!st.empty()) {
            auto [s, pc] = st.back();
            st.pop_back();
            vector<int> order = {s};  // 成分の頂点を BFS で集める
            par[s] = -1;
            for (size_t i = 0; i < order.size(); i++)
                for (auto [to, w] : g(order[i]))
                    if (!removed[to] && to != par[order[i]]) par[to] = order[i], order.push_back(to);
            int tot = order.size(), c = s;
            for (int i = tot - 1; i >= 0; i--) {
                int v = order[i];
                sz[v] = 1;
                for (auto [to, w] : g(v)) if (!removed[to] && to != par[v]) sz[v] += sz[to];
            }
            for (int v : order) {  // ★ 重心: 子の部分木も、親の側（tot - sz[v]）も tot/2 以下
                int mx = tot - sz[v];
                for (auto [to, w] : g(v)) if (!removed[to] && to != par[v]) mx = max(mx, sz[to]);
                if (mx * 2 <= tot) { c = v; break; }
            }
            cpar[c] = pc, level[c] = (pc == -1 ? 0 : level[pc] + 1);
            if (pc == -1) root = c;
            vector<int> q = {c};  // ★ 重心 c から成分内の各頂点までの距離を記録（cdist[v] の level[c] 番目に入る）
            par[c] = -1;
            cdist[c].push_back(0);
            for (size_t i = 0; i < q.size(); i++) {
                int v = q[i];
                for (auto [to, w] : g(v))
                    if (!removed[to] && to != par[v]) par[to] = v, cdist[to].push_back(cdist[v].back() + 1), q.push_back(to);
            }
            removed[c] = true;  // ★ c を取り除くと、成分は c の隣ごとに分かれる
            for (auto [to, w] : g(c)) if (!removed[to]) st.push_back({to, c});
        }
    }
};

// ---------- 削除可: 距離 k 以内の頂点数（上の CentroidDecomposition が必要） ----------
// CentroidCount cc(cd, w);    w[v] = 頂点 v の重み（省略すると全部 1 = 頂点の個数を数える）。cd は参照で持つので先に作って残しておく
//   cc.count(v, k)            dist(v, u) <= k となる u の重みの和（u = v も含む）。O(log N)
// ★ v の祖先の重心 c ごとに「c の成分で c から距離 k − dist(v, c) 以内」を足す。
//    ただし v と同じ側（子の成分）の頂点は c を通らないので、その分を引く（同じ頂点を 2 回数えないように）
// ◆ struct CentroidCount : 距離 k 以内の頂点の数（重みの和）
//   CentroidCount(cd, w) : 前計算 O(N log N)
//   count(v, k)          : dist(v, u) <= k となる u の重みの和（u = v を含む）。O(log N)
template <class T>
struct CentroidCount {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/centroid.cpp
    const CentroidDecomposition<T>& cd;
    vector<vector<long long>> A, Bm;  // A[c][d] = c の成分で c から距離 d 以下の重みの和、Bm[ch][d] = ch の成分で「親の重心」から距離 d 以下の和
    // cd_ : 作ってある CentroidDecomposition（参照で持つので、cd_ を先に作って残しておく）
    // w   : 各頂点の重み（省略すると全部 1 = 頂点の個数）。作った後に重みは変えられない
    CentroidCount(const CentroidDecomposition<T>& cd_, vector<long long> w = {}) : cd(cd_), A(cd_.n), Bm(cd_.n) {
        int n = cd.n;
        if (w.empty()) w.assign(n, 1);
        for (int v = 0; v < n; v++) {
            int prev = -1;
            for (int c = v; c != -1; prev = c, c = cd.cpar[c]) {
                int d = cd.cdist[v][cd.level[c]];
                if ((int)A[c].size() <= d) A[c].resize(d + 1, 0);
                A[c][d] += w[v];
                if (prev != -1) {
                    if ((int)Bm[prev].size() <= d) Bm[prev].resize(d + 1, 0);
                    Bm[prev][d] += w[v];
                }
            }
        }
        for (auto& a : A) for (int i = 1; i < (int)a.size(); i++) a[i] += a[i - 1];  // 累積和にしておく
        for (auto& b : Bm) for (int i = 1; i < (int)b.size(); i++) b[i] += b[i - 1];
    }
    // （内部用）累積和の d 番目まで
    static long long upto(const vector<long long>& a, long long d) {  // a の d 番目まで（d < 0 なら 0）
        if (d < 0 || a.empty()) return 0;
        return a[min<long long>(d, (long long)a.size() - 1)];
    }
    // count(v, k) : v : 中心の頂点、k : 距離の上限（負なら 0 を返す）
    long long count(int v, long long k) const {
        long long res = 0;
        int prev = -1;
        for (int c = v; c != -1; prev = c, c = cd.cpar[c]) {
            long long rest = k - cd.cdist[v][cd.level[c]];
            res += upto(A[c], rest);
            if (prev != -1) res -= upto(Bm[prev], rest);  // ★ v と同じ子の成分にいる頂点は、もっと深い重心で数え済み
        }
        return res;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 一直線の木 0-1-2-3-4-5-6: 重心は 3、次は 1 と 5（Tree と同じく add_edge → build）
    CentroidDecomposition<> cd(7);
    for (int v = 0; v + 1 < 7; v++) cd.add_edge(v, v + 1);
    cd.build();
    assert((cd.centroids() == vector<long long>{3}));
    {
        CentroidDecomposition<> c2(4);  // 0-1-2-3 は真ん中の辺で半分に分かれるので重心は 1 と 2
        c2.add_edge(0, 1), c2.add_edge(1, 2), c2.add_edge(2, 3);
        assert((c2.centroids() == vector<long long>{1, 2}));
        assert((CentroidDecomposition<>(1).centroids() == vector<long long>{0}));
    }
    assert(cd.root == 3 && cd.cpar[1] == 3 && cd.cpar[5] == 3 && cd.cpar[0] == 1 && cd.level[6] == 2);
    assert(cd.cdist[0][0] == 3 && cd.cdist[0][1] == 1 && cd.cdist[0][2] == 0);  // 0 から 3, 1, 0 までの距離

    // 使用例: 赤く塗る / 一番近い赤い頂点までの距離
    vector<int> best(7, INT_MAX);  // best[c] = 重心 c から、c の成分の中で一番近い赤い頂点までの距離
    auto paint = [&](int v) { for (int c = v; c != -1; c = cd.cpar[c]) best[c] = min(best[c], cd.cdist[v][cd.level[c]]); };
    auto nearest = [&](int v) {
        int r = INT_MAX;
        for (int c = v; c != -1; c = cd.cpar[c]) if (best[c] != INT_MAX) r = min(r, best[c] + cd.cdist[v][cd.level[c]]);
        return r;
    };
    paint(0);
    assert(nearest(6) == 6 && nearest(2) == 2);
    paint(5);
    assert(nearest(6) == 1 && nearest(2) == 2 && nearest(3) == 2);

    // ランダムテスト: 重心木の性質と距離を BFS と比較、塗る / 一番近い赤 を愚直と比較
    mt19937 rr(126);
    for (int it = 0; it < 300; it++) {
        int n = rr() % 40 + 1;
        vector<vector<int>> h(n);
        for (int v = 1; v < n; v++) { int p = rr() % v; h[p].push_back(v), h[v].push_back(p); }
        CentroidDecomposition<> C(h);
        {   // add_edge → build で作っても、隣接リストから作ったものと同じになる
            CentroidDecomposition<> C2(n);
            for (int v = 0; v < n; v++) for (int to : h[v]) if (v < to) C2.add_edge(v, to);
            C2.build();
            // 辺を入れる順は違ってよいので、重心木の性質（下のテスト）を C2 でも確かめる
            for (int v = 0; v < n; v++) assert((int)C2.cdist[v].size() == C2.level[v] + 1);
            // 重心: 全頂点を試して「取り除いた後の最大の成分が n/2 以下」
            vector<long long> want;
            for (int c = 0; c < n; c++) {
                vector<int> seen(n, 0);
                seen[c] = 1;
                int mx = 0;
                for (int s = 0; s < n; s++) {
                    if (seen[s]) continue;
                    int cnt = 0;
                    vector<int> q = {s};
                    seen[s] = 1;
                    for (size_t i = 0; i < q.size(); i++, cnt++) for (int to : h[q[i]]) if (!seen[to]) seen[to] = 1, q.push_back(to);
                    mx = max(mx, cnt);
                }
                if (mx * 2 <= n) want.push_back(c);
            }
            assert(C2.centroids() == want && C.centroids() == want);
            assert(C.root == want[0] || C.root == want.back());  // 重心木の根は木の重心
        }
        vector<vector<int>> D(n, vector<int>(n, -1));  // 元の木での距離
        for (int s = 0; s < n; s++) {
            D[s][s] = 0;
            vector<int> q = {s};
            for (size_t i = 0; i < q.size(); i++) for (int to : h[q[i]]) if (D[s][to] < 0) D[s][to] = D[s][q[i]] + 1, q.push_back(to);
        }
        int maxlv = 0;
        for (int v = 0; v < n; v++) {
            maxlv = max(maxlv, C.level[v]);
            assert((int)C.cdist[v].size() == C.level[v] + 1);
            for (int c = v; c != -1; c = C.cpar[c]) assert(C.cdist[v][C.level[c]] == D[v][c]);
        }
        assert((1 << maxlv) <= n);  // 深さは log2 N 以下
        for (int u = 0; u < n; u++)
            for (int v = 0; v < n; v++) {  // 重心木での共通の祖先で一番深いもの c は u-v パス上にある
                set<int> au;
                for (int c = u; c != -1; c = C.cpar[c]) au.insert(c);
                int c = v;
                while (!au.count(c)) c = C.cpar[c];
                assert(D[u][c] + D[c][v] == D[u][v]);
            }
        vector<int> bst(n, INT_MAX);
        vector<bool> red(n, false);
        for (int q = 0; q < 30; q++) {
            int v = rr() % n;
            if (rr() % 2) {
                red[v] = true;
                for (int c = v; c != -1; c = C.cpar[c]) bst[c] = min(bst[c], C.cdist[v][C.level[c]]);
            } else {
                int r = INT_MAX, ref = INT_MAX;
                for (int c = v; c != -1; c = C.cpar[c]) if (bst[c] != INT_MAX) r = min(r, bst[c] + C.cdist[v][C.level[c]]);
                for (int u = 0; u < n; u++) if (red[u]) ref = min(ref, D[v][u]);
                assert(r == ref);
            }
        }
    }
    // 距離 k 以内の頂点数: 一直線の木 0-1-...-6 で、3 から距離 2 以内 = 1..5 の 5 個、0 から 2 以内 = 0, 1, 2 の 3 個
    {
        CentroidCount cc(cd);
        assert(cc.count(3, 2) == 5 && cc.count(0, 2) == 3 && cc.count(6, 0) == 1 && cc.count(6, 100) == 7);
        CentroidCount cw(cd, {1, 10, 100, 1000, 10000, 100000, 1000000});  // 重み付き: 2 から 1 以内 = 1, 2, 3 → 10 + 100 + 1000
        assert(cw.count(2, 1) == 1110);
    }
    // ランダムテスト: BFS で数えたものと比較
    {
        mt19937 r7(181);
        for (int it = 0; it < 300; it++) {
            int n = r7() % 40 + 1;
            vector<vector<int>> h(n);
            for (int v = 1; v < n; v++) { int p = r7() % v; h[p].push_back(v), h[v].push_back(p); }
            CentroidDecomposition<> C(h);
            vector<long long> w(n);
            for (auto& x : w) x = r7() % 10;
            CentroidCount cc(C, w);
            for (int s = 0; s < n; s++) {
                vector<int> D(n, -1);
                D[s] = 0;
                vector<int> q = {s};
                for (size_t i = 0; i < q.size(); i++) for (int to : h[q[i]]) if (D[to] < 0) D[to] = D[q[i]] + 1, q.push_back(to);
                for (int k = -1; k <= n; k++) {
                    long long want = 0;
                    for (int u = 0; u < n; u++) if (D[u] <= k) want += w[u];
                    assert(cc.count(s, k) == want);
                }
            }
        }
    }
    puts("OK");
}
