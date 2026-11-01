// =====================================================================
// graph/topological_sort.cpp  トポロジカルソート（Kahn 法）
//   有向グラフを「辺 u->v なら u が先」になるよう並べる。閉路検出、DAG 上の DP に使う
//   削除可: DAG の最長路（辺の本数・最長の道そのもの）、DAG 上の重み付き最短路・最長路（負の重みも OK、経路復元つき）
//   O(N + M)（辞書順最小版は O((N + M) log N)）
// 依存: graph/graph.cpp の Graph（先に貼る）
// ABC / ARC での使用例:
//   ABC223 D - Restricted Permutation  https://atcoder.jp/contests/abc223/tasks/abc223_d
//     → 辞書順最小のトポロジカルソート（add_arc で入れて topological_sort(g, true)）
//   ABC139 E - League  https://atcoder.jp/contests/abc139/tasks/abc139_e
//     → 試合を頂点にした DAG の最長路（dag_longest_path）。閉路なら -1
//   ※ トポロジカルソートの結果は「全頂点の並び」で、最長の道ではない。最長の道そのものは dag_longest_path_vertices
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;
// テスト用: graph.cpp の Graph を読み込む（提出するときは、graph.cpp のコピー範囲をこのファイルのコピー範囲より先に貼る）
#define GRAPH_NO_MAIN
#include "graph.cpp"

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// Graph<> g(n); g.add_arc(a, b); ...（有向グラフ。重みは見ない）
// auto ord = topological_sort(g);     頂点の並び。閉路があれば空
// lex_min = true で辞書順最小の並び
// ◆ topological_sort(g, lex_min) : 辺 u → v なら u が先になるように頂点を並べる。O(N + M)（lex_min なら O((N + M) log N)）
//   g       : Graph（有向辺 add_arc で作る。無向辺は両向きの辺なので閉路になる）
//   lex_min : true なら並べ方のうち辞書順最小のものを返す（既定 false = どれか 1 つ）
//   返り値  : 頂点の並び。閉路があれば空
template <class T>
vector<long long> topological_sort(const Graph<T>& g, bool lex_min = false) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/topological_sort.cpp
    int n = g.n;
    vector<int> indeg(n, 0);
    vector<long long> order;
    for (int v = 0; v < n; v++)
        for (auto& e : g.adj[v]) indeg[e.to]++;  // ★ 入次数（自分に向かってくる辺の数）を数える
    priority_queue<int, vector<int>, greater<int>> pq;  // lex_min 用
    queue<int> q;
    auto push = [&](int v) { lex_min ? pq.push(v) : q.push(v); };
    for (int v = 0; v < n; v++)
        if (indeg[v] == 0) push(v);  // ★ 入次数 0 = もう前に来るべき頂点がない
    while (lex_min ? !pq.empty() : !q.empty()) {
        int v;
        if (lex_min) v = pq.top(), pq.pop();
        else v = q.front(), q.pop();
        order.push_back(v);
        for (auto& e : g.adj[v])
            if (--indeg[e.to] == 0) push(e.to);  // ★ v を取り除いたら入次数 0 になった頂点を追加
    }
    if ((int)order.size() != n) return {};  // ★ 全部並ばない = 閉路がある
    return order;
}

// ---------- 削除可: DAG の最長路 ----------
// res[v] = v で終わる最長パスの辺数（閉路があれば空）
// ◆ dag_longest_path(g) : 各頂点で終わる最長の道の辺の本数（重みなし）
//   g : Graph（有向。閉路があれば空を返す）
template <class T>
vector<long long> dag_longest_path(const Graph<T>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/topological_sort.cpp
    auto ord = topological_sort(g);
    if (ord.empty()) return {};
    vector<long long> dp(g.n, 0);
    for (int v : ord)
        for (auto& e : g.adj[v]) dp[e.to] = max(dp[e.to], dp[v] + 1);  // ★ トポロジカル順なら前が確定済み
    return dp;
}

// ◆ dag_longest_path_vertices(g) : 一番長い道（辺の本数が最大）の頂点の並びを 1 つ返す（重みなし）。O(N + M)
//   g : Graph（有向。閉路があれば空を返す）
//   返り値 : 始点から終点までの頂点の並び（辺の本数 = 返り値の長さ − 1）。辺がなければ頂点 1 個だけ
//   例: 辺 1→2, 3→4, 4→5（0 始まりなら 0→1, 2→3, 3→4）なら {2, 3, 4}
template <class T>
vector<long long> dag_longest_path_vertices(const Graph<T>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/topological_sort.cpp
    auto ord = topological_sort(g);
    if (ord.empty()) return {};
    int n = g.n;
    vector<long long> dp(n, 0);  // dp[v] = v で終わる最長の道の辺の本数
    vector<int> prev(n, -1);     // prev[v] = その道で v の 1 つ前の頂点
    for (int v : ord)
        for (auto& e : g.adj[v])
            if (dp[v] + 1 > dp[e.to]) dp[e.to] = dp[v] + 1, prev[e.to] = v;  // ★ 伸ばせたら「どこから来たか」も覚える
    int t = max_element(dp.begin(), dp.end()) - dp.begin();          // ★ 一番長い道の終点
    vector<long long> path;
    for (int v = t; v != -1; v = prev[v]) path.push_back(v);         // ★ 終点から prev を逆にたどる
    reverse(path.begin(), path.end());
    return path;
}

// ---------- 削除可: DAG 上の重み付き最短路・最長路 O(N + M)（本体が必要） ----------
// Graph<> g(n); g.add_arc(a, b, w); ...（重み付き有向グラフ。負の重みも OK。閉路があってはいけない）
// auto r = dag_path(g, s);         s から各頂点への最短路
// auto r = dag_path(g, s, true);   最長路
//   s = -1 なら「どの頂点から始めてもよい」（その頂点だけの長さ 0 の道も含む）
//   r.ok            閉路がなかったか（false なら他は使えない）
//   r.reachable(v)  s から v へ行けるか
//   r.dist[v]       最短（最長）の重みの和
//   r.path(t)       その道の頂点の並び
// ふつうのグラフの最長路は難しいが、DAG ならトポロジカル順に DP するだけで求まる
// ◆ struct DagPathResult : dag_path の結果
//   ok           : 閉路がなかったか（false なら他は使えない）
//   dist[v]      : 最短（最長）の重みの和
//   reachable(v) : 始点から v に行けるか
//   path(t)      : その道の頂点の並び（行けなければ空）
template <class T>
struct DagPathResult {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/topological_sort.cpp
    bool ok;
    vector<T> dist;
    vector<int> prev;
    vector<bool> reach;
    // reachable(v) : v に行けるか
    bool reachable(int v) const { return reach[v]; }
    // path(t) : t で終わる最短（最長）の道の頂点の並び。t : 終点
    vector<long long> path(int t) const {
        if (!reach[t]) return {};
        vector<long long> p;
        for (int v = t; v != -1; v = prev[v]) p.push_back(v);
        reverse(p.begin(), p.end());
        return p;
    }
};
// ◆ dag_path(g, s, maximize) : DAG（閉路のない有向グラフ）の重み付き最短路・最長路。O(N + M)
//   g        : Graph<T>（有向）。負の重みもよい
//   s        : 始点。−1 なら「どの頂点から始めてもよい」（各頂点だけの長さ 0 の道も含む）
//   maximize : true なら最長路、false（既定）なら最短路
template <class T>
DagPathResult<T> dag_path(const Graph<T>& g, int s, bool maximize = false) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/topological_sort.cpp
    int n = g.n;
    auto ord = topological_sort(g);
    DagPathResult<T> r{false, vector<T>(n, T(0)), vector<int>(n, -1), vector<bool>(n, s == -1)};
    if ((int)ord.size() != n) return r;  // 閉路がある
    r.ok = true;
    if (s != -1) r.reach[s] = true;
    for (int v : ord) {  // ★ トポロジカル順なら、v に入ってくる辺はすべて処理済み → dist[v] は確定している
        if (!r.reach[v]) continue;
        for (auto& e : g.adj[v]) {
            int to = e.to;
            T nd = r.dist[v] + e.cost;
            if (!r.reach[to] || (maximize ? nd > r.dist[to] : nd < r.dist[to]))
                r.dist[to] = nd, r.prev[to] = v, r.reach[to] = true;  // ★ ダイクストラと同じ緩和を、トポロジカル順に 1 回ずつ
        }
    }
    return r;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
using G = Graph<long long>;
G from_wg(const vector<vector<pair<int, long long>>>& wg) {  // 重み付きの隣接リスト → Graph
    G g(wg.size());
    for (int v = 0; v < (int)wg.size(); v++) for (auto [to, w] : wg[v]) g.add_arc(v, to, w);
    return g;
}
int main() {
    vector<vector<int>> g = {{2}, {2}, {3}, {}};  // 0->2, 1->2, 2->3
    assert((topological_sort(G(g), true) == vector<long long>{0, 1, 2, 3}));
    assert(topological_sort(G(vector<vector<int>>{{1}, {0}})).empty());  // 閉路
    assert(dag_longest_path(G(g))[3] == 2);
    {   // Graph に直接 add_arc で入れる
        G d(4);
        d.add_arc(0, 2), d.add_arc(1, 2), d.add_arc(2, 3);
        assert((topological_sort(d, true) == vector<long long>{0, 1, 2, 3}));
        G u(2);
        u.add_edge(0, 1);  // 無向辺は両向きなので閉路
        assert(topological_sort(u).empty());
    }
    // 頂点 1..6、辺 2→3, 4→5, 5→6（0 始まりに直して入れる）→ 最長の道は 4 → 5 → 6
    {
        vector<vector<int>> h(6);
        for (auto [a, b] : vector<pair<int, int>>{{2, 3}, {4, 5}, {5, 6}}) h[a - 1].push_back(b - 1);
        assert((dag_longest_path_vertices(G(h)) == vector<long long>{3, 4, 5}));
        assert((dag_longest_path_vertices(G(3)) == vector<long long>{0}));  // 辺なし
        assert(dag_longest_path_vertices(G(vector<vector<int>>{{1}, {0}})).empty());                               // 閉路
    }
    // ランダムテスト: 順列を全部試して「辺の向きに矛盾しない最初の順列」と比較、最長路は全経路を再帰で
    {
        mt19937 rr(114);
        for (int it = 0; it < 2000; it++) {
            int n = rr() % 6 + 1;
            vector<vector<int>> h(n);
            vector<pair<int, int>> es;
            for (int e = rr() % 9; e > 0; e--) {
                int a = rr() % n, b = rr() % n;
                h[a].push_back(b), es.push_back({a, b});
            }
            vector<long long> p(n), first;
            iota(p.begin(), p.end(), 0);
            do {
                vector<int> pos(n);
                for (int i = 0; i < n; i++) pos[p[i]] = i;
                bool ok = true;
                for (auto [a, b] : es) if (pos[a] >= pos[b]) ok = false;
                if (ok) { first = p; break; }
            } while (next_permutation(p.begin(), p.end()));
            auto o = topological_sort(G(h)), lo = topological_sort(G(h), true);
            assert(o.empty() == first.empty() && lo == first);
            if (o.empty()) continue;
            vector<int> pos(n);
            for (int i = 0; i < n; i++) pos[o[i]] = i;
            for (auto [a, b] : es) assert(pos[a] < pos[b]);
            function<int(int)> longest_to = [&](int v) {
                int best = 0;
                for (auto [a, b] : es) if (b == v) best = max(best, longest_to(a) + 1);
                return best;
            };
            auto lp = dag_longest_path(G(h));
            for (int v = 0; v < n; v++) assert(lp[v] == longest_to(v));
            // 最長の道そのもの: 長さが最大で、隣どうしが本当に辺でつながっているか
            auto path = dag_longest_path_vertices(G(h));
            assert((long long)path.size() - 1 == *max_element(lp.begin(), lp.end()));
            for (int i = 0; i + 1 < (int)path.size(); i++)
                assert(find(h[path[i]].begin(), h[path[i]].end(), path[i + 1]) != h[path[i]].end());
        }
    }
    // 重み付き: 0 → 1 (5), 0 → 2 (-3), 2 → 1 (4), 1 → 3 (2), 2 → 3 (11)
    //   0 から 3 へ: 0 → 1 → 3 = 7、0 → 2 → 1 → 3 = 3、0 → 2 → 3 = 8 → 最短 3、最長 8
    {
        vector<vector<pair<int, long long>>> wg = {{{1, 5}, {2, -3}}, {{3, 2}}, {{1, 4}, {3, 11}}, {}};
        auto mn = dag_path(from_wg(wg), 0), mx = dag_path(from_wg(wg), 0, true);
        assert(mn.ok && mn.dist[3] == 3 && (mn.path(3) == vector<long long>{0, 2, 1, 3}));
        assert(mx.dist[3] == 8 && (mx.path(3) == vector<long long>{0, 2, 3}) && mx.dist[1] == 5);
        auto any = dag_path(from_wg(wg), -1, true);  // どこから始めてもよい最長路: 3 で終わるなら 2 → 3 = 11、1 で終わるなら 0 → 1 = 5
        assert(any.dist[3] == 11 && (any.path(3) == vector<long long>{2, 3}) && any.dist[1] == 5 && any.dist[0] == 0);
        vector<vector<pair<int, long long>>> cyc = {{{1, 1}}, {{0, 1}}};
        assert(!dag_path(from_wg(cyc), 0).ok);
        auto un = dag_path(from_wg({{}, {{0, 1}}}), 0);  // 0 から 1 へは行けない
        assert(un.ok && !un.reachable(1) && un.path(1).empty());
    }
    // ランダムテスト: 道を全部たどって最短・最長を求めたものと比較（閉路なしのグラフを作る）
    {
        mt19937 rr(152);
        for (int it = 0; it < 2000; it++) {
            int n = rr() % 7 + 1;
            vector<int> perm(n);
            iota(perm.begin(), perm.end(), 0);
            shuffle(perm.begin(), perm.end(), rr);  // perm の順に辺を向けて DAG にする
            vector<vector<pair<int, long long>>> wg(n);
            for (int e = rr() % 12; e > 0; e--) {
                int i = rr() % n, j = rr() % n;
                if (i == j) continue;
                if (i > j) swap(i, j);
                wg[perm[i]].push_back({perm[j], (long long)(rr() % 21) - 10});
            }
            if (rr() % 5 == 0 && n >= 2) {  // たまに閉路を入れる
                wg[perm[n - 1]].push_back({perm[0], 1}), wg[perm[0]].push_back({perm[n - 1], 1});
                assert(!dag_path(from_wg(wg), 0).ok);
                continue;
            }
            int s = (int)(rr() % (n + 1)) - 1;
            for (int mxm = 0; mxm < 2; mxm++) {
                auto r = dag_path(from_wg(wg), s, mxm);
                assert(r.ok);
                vector<bool> seen(n, false);
                vector<long long> best(n, 0);
                function<void(int, long long)> dfs = [&](int v, long long d) {
                    if (!seen[v] || (mxm ? d > best[v] : d < best[v])) best[v] = d;
                    seen[v] = true;
                    for (auto [to, w] : wg[v]) dfs(to, d + w);
                };
                for (int st = 0; st < n; st++) if (s == -1 || st == s) dfs(st, 0);
                for (int v = 0; v < n; v++) {
                    assert(r.reachable(v) == seen[v]);
                    if (!seen[v]) continue;
                    assert(r.dist[v] == best[v]);
                    auto p = r.path(v);  // 復元した道が本当に辺をたどっていて、重みの和が dist と一致するか
                    assert(p.back() == v && (s == -1 || p[0] == s));
                    long long sum = 0;
                    for (int i = 0; i + 1 < (int)p.size(); i++) {
                        long long w = mxm ? LLONG_MIN : LLONG_MAX;
                        for (auto [to, ww] : wg[p[i]]) if (to == p[i + 1]) w = mxm ? max(w, ww) : min(w, ww);
                        assert(w != LLONG_MIN && w != LLONG_MAX);
                        sum += w;
                    }
                    assert(sum == r.dist[v]);
                }
            }
        }
    }
    puts("OK");
}
