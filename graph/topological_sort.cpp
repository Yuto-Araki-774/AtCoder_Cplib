// =====================================================================
// graph/topological_sort.cpp  トポロジカルソート（Kahn 法）
//   有向グラフを「辺 u->v なら u が先」になるよう並べる。閉路検出、DAG 上の DP に使う
//   削除可: DAG の最長路（辺の本数）、DAG 上の重み付き最短路・最長路（負の重みも OK、経路復元つき）
//   O(N + M)（辞書順最小版は O((N + M) log N)）
// 依存: なし
// ABC / ARC での使用例:
//   ABC223 D - Restricted Permutation  https://atcoder.jp/contests/abc223/tasks/abc223_d
//     → 辞書順最小のトポロジカルソート（lex_min = true）
//   ABC139 E - League  https://atcoder.jp/contests/abc139/tasks/abc139_e
//     → 試合を頂点にした DAG の最長路（dag_longest_path）。閉路なら -1
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// g : 有向グラフ g[v] = {v -> to の to}
// 返り値 : 頂点の並び。閉路があれば空
// lex_min = true で辞書順最小の並び
vector<int> topological_sort(const vector<vector<int>>& g, bool lex_min = false) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/topological_sort.cpp
    int n = g.size();
    vector<int> indeg(n, 0), order;
    for (int v = 0; v < n; v++)
        for (int to : g[v]) indeg[to]++;  // ★ 入次数（自分に向かってくる辺の数）を数える
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
        for (int to : g[v])
            if (--indeg[to] == 0) push(to);  // ★ v を取り除いたら入次数 0 になった頂点を追加
    }
    if ((int)order.size() != n) return {};  // ★ 全部並ばない = 閉路がある
    return order;
}

// ---------- 削除可: DAG の最長路 ----------
// res[v] = v で終わる最長パスの辺数（閉路があれば空）
vector<int> dag_longest_path(const vector<vector<int>>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/topological_sort.cpp
    auto ord = topological_sort(g);
    if (ord.empty()) return {};
    vector<int> dp(g.size(), 0);
    for (int v : ord)
        for (int to : g[v]) dp[to] = max(dp[to], dp[v] + 1);  // ★ トポロジカル順なら前が確定済み
    return dp;
}

// ---------- 削除可: DAG 上の重み付き最短路・最長路 O(N + M)（本体が必要） ----------
// g[v] = {{to, w}, ...}（重み付き有向グラフ。負の重みも OK。閉路があってはいけない）
// auto r = dag_path<long long>(g, s);         s から各頂点への最短路
// auto r = dag_path<long long>(g, s, true);   最長路
//   s = -1 なら「どの頂点から始めてもよい」（その頂点だけの長さ 0 の道も含む）
//   r.ok            閉路がなかったか（false なら他は使えない）
//   r.reachable(v)  s から v へ行けるか
//   r.dist[v]       最短（最長）の重みの和
//   r.path(t)       その道の頂点の並び
// ふつうのグラフの最長路は難しいが、DAG ならトポロジカル順に DP するだけで求まる
template <class T>
struct DagPathResult {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/topological_sort.cpp
    bool ok;
    vector<T> dist;
    vector<int> prev;
    vector<bool> reach;
    bool reachable(int v) const { return reach[v]; }
    vector<int> path(int t) const {
        if (!reach[t]) return {};
        vector<int> p;
        for (int v = t; v != -1; v = prev[v]) p.push_back(v);
        reverse(p.begin(), p.end());
        return p;
    }
};
template <class T>
DagPathResult<T> dag_path(const vector<vector<pair<int, T>>>& g, int s, bool maximize = false) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/topological_sort.cpp
    int n = g.size();
    vector<vector<int>> h(n);
    for (int v = 0; v < n; v++)
        for (auto& [to, w] : g[v]) h[v].push_back(to);
    auto ord = topological_sort(h);
    DagPathResult<T> r{false, vector<T>(n, T(0)), vector<int>(n, -1), vector<bool>(n, s == -1)};
    if ((int)ord.size() != n) return r;  // 閉路がある
    r.ok = true;
    if (s != -1) r.reach[s] = true;
    for (int v : ord) {  // ★ トポロジカル順なら、v に入ってくる辺はすべて処理済み → dist[v] は確定している
        if (!r.reach[v]) continue;
        for (auto& [to, w] : g[v]) {
            T nd = r.dist[v] + w;
            if (!r.reach[to] || (maximize ? nd > r.dist[to] : nd < r.dist[to]))
                r.dist[to] = nd, r.prev[to] = v, r.reach[to] = true;  // ★ ダイクストラと同じ緩和を、トポロジカル順に 1 回ずつ
        }
    }
    return r;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    vector<vector<int>> g = {{2}, {2}, {3}, {}};  // 0->2, 1->2, 2->3
    assert((topological_sort(g, true) == vector<int>{0, 1, 2, 3}));
    assert(topological_sort({{1}, {0}}).empty());  // 閉路
    assert(dag_longest_path(g)[3] == 2);
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
            vector<int> p(n), first;
            iota(p.begin(), p.end(), 0);
            do {
                vector<int> pos(n);
                for (int i = 0; i < n; i++) pos[p[i]] = i;
                bool ok = true;
                for (auto [a, b] : es) if (pos[a] >= pos[b]) ok = false;
                if (ok) { first = p; break; }
            } while (next_permutation(p.begin(), p.end()));
            auto o = topological_sort(h), lo = topological_sort(h, true);
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
            auto lp = dag_longest_path(h);
            for (int v = 0; v < n; v++) assert(lp[v] == longest_to(v));
        }
    }
    // 重み付き: 0 → 1 (5), 0 → 2 (-3), 2 → 1 (4), 1 → 3 (2), 2 → 3 (11)
    //   0 から 3 へ: 0 → 1 → 3 = 7、0 → 2 → 1 → 3 = 3、0 → 2 → 3 = 8 → 最短 3、最長 8
    {
        vector<vector<pair<int, long long>>> wg = {{{1, 5}, {2, -3}}, {{3, 2}}, {{1, 4}, {3, 11}}, {}};
        auto mn = dag_path<long long>(wg, 0), mx = dag_path<long long>(wg, 0, true);
        assert(mn.ok && mn.dist[3] == 3 && (mn.path(3) == vector<int>{0, 2, 1, 3}));
        assert(mx.dist[3] == 8 && (mx.path(3) == vector<int>{0, 2, 3}) && mx.dist[1] == 5);
        auto any = dag_path<long long>(wg, -1, true);  // どこから始めてもよい最長路: 3 で終わるなら 2 → 3 = 11、1 で終わるなら 0 → 1 = 5
        assert(any.dist[3] == 11 && (any.path(3) == vector<int>{2, 3}) && any.dist[1] == 5 && any.dist[0] == 0);
        vector<vector<pair<int, long long>>> cyc = {{{1, 1}}, {{0, 1}}};
        assert(!dag_path<long long>(cyc, 0).ok);
        auto un = dag_path<long long>({{}, {{0, 1}}}, 0);  // 0 から 1 へは行けない
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
                assert(!dag_path<long long>(wg, 0).ok);
                continue;
            }
            int s = (int)(rr() % (n + 1)) - 1;
            for (int mxm = 0; mxm < 2; mxm++) {
                auto r = dag_path<long long>(wg, s, mxm);
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
