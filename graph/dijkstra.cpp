// =====================================================================
// graph/dijkstra.cpp  ダイクストラ法（非負重みの最短路）
//   多始点・経路復元。拡張ダイクストラ（頂点に状態を持たせる）にも対応。最短路の本数・最短路 DAG・k 番目最短路も
//   O((N + M) log N)。負の重みがあるなら使えない（ベルマンフォードへ）
// 依存: なし
// ABC / ARC での使用例:
//   ABC035 D - トレジャーハント  https://atcoder.jp/contests/abc035/tasks/abc035_d
//     → 行きと帰り（辺を逆向きにしたグラフ）でダイクストラ
//   ABC395 E - Flip Edge  https://atcoder.jp/contests/abc395/tasks/abc395_e
//     → 状態 (頂点, 反転しているか) の拡張ダイクストラ（dijkstra_func）
//   ABC211 D - Number of Shortest paths  https://atcoder.jp/contests/abc211/tasks/abc211_d
//     → 重み 1 の無向グラフで count_shortest_paths<atcoder::modint1000000007>（mod が 998244353 でないので注意）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// WGraph<long long> g(n);  g[a].push_back({b, c});   // a -> b コスト c（無向なら逆も）
// auto r = dijkstra(g, s);   r.dist[t]（到達不能は INF）、r.reachable(t)、r.path(t)
// ◆ WEdge<T> : 重み付きの辺 {to: 行き先, cost: 重み}。WGraph<T> = vector<vector<WEdge<T>>> が隣接リスト
template <class T> struct WEdge {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dijkstra.cpp
    int to; T cost;
};
template <class T> using WGraph = vector<vector<WEdge<T>>>;

// ◆ struct DijkstraResult : ダイクストラの結果
//   dist[v]        : 最短距離（行けなければ INF = T の最大値の半分）
//   prev[v]        : 最短路で v の 1 つ前の頂点（始点・行けない頂点は −1）
//   reachable(v)   : v に行けるか
//   path(t)        : 始点から t への最短路の頂点の並び（行けなければ空）
template <class T>
struct DijkstraResult {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dijkstra.cpp
    static constexpr T INF = numeric_limits<T>::max() / 2;
    vector<T> dist;
    vector<int> prev;
    // reachable(v) : v に行けるか。v : 頂点番号
    bool reachable(int v) const { return dist[v] != INF; }
    // --- 削除可: 経路復元 ---
    // path(t) : t までの最短路（始点 → … → t）。t : 終点
    vector<long long> path(int t) const {
        if (!reachable(t)) return {};
        vector<long long> res;
        for (int v = t; v != -1; v = prev[v]) res.push_back(v);
        reverse(res.begin(), res.end());
        return res;
    }
    // --- 削除可ここまで ---
};

// 汎用版: 頂点数 n と「隣接を列挙する関数」を渡す
// adj(v, relax) の中で、v から行ける先ごとに relax(行き先, コスト) を呼ぶ
// ◆ dijkstra_func<T>(n, starts, adj) : 頂点を自分で定義するダイクストラ（拡張ダイクストラ）。O((N + M) log N)
//   T      : 距離の型（long long など。必ず書く）
//   n      : 状態の数。状態は 0..n−1 の番号にしておく（例: (頂点, 使ったクーポンの数) → v * (K + 1) + k）
//   starts : 始点の一覧（多始点）
//   adj    : void adj(int v, auto relax)。v から行ける先ごとに relax(行き先, コスト) を呼ぶ
//   注意 : コストは 0 以上であること（負の辺があると正しくない → shortest_path.cpp）
template <class T, class F>
DijkstraResult<T> dijkstra_func(int n, const vector<int>& starts, F adj) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dijkstra.cpp
    const T INF = DijkstraResult<T>::INF;
    DijkstraResult<T> r{vector<T>(n, INF), vector<int>(n, -1)};
    using P = pair<T, int>;
    priority_queue<P, vector<P>, greater<P>> pq;  // ★ 距離が小さい順に取り出す
    for (int s : starts)
        if (r.dist[s] != 0) r.dist[s] = 0, pq.push({0, s});
    while (!pq.empty()) {
        auto [d, v] = pq.top();
        pq.pop();
        if (d != r.dist[v]) continue;  // ★ 古い情報（もっと短い距離で確定済み）は捨てる
        adj(v, [&](int to, T cost) {
            if (d + cost < r.dist[to]) {  // ★ 今の頂点経由の方が短ければ更新（緩和）
                r.dist[to] = d + cost;
                r.prev[to] = v;
                pq.push({r.dist[to], to});
            }
        });
    }
    return r;
}

// 普通のグラフ用
// ◆ dijkstra(g, starts) / dijkstra(g, s) : ダイクストラ法。O((N + M) log N)
//   g      : WGraph<T>（g[a].push_back({b, c}) で a → b 重み c。無向なら逆向きも入れる）
//   starts : 始点の一覧（多始点）、s : 始点が 1 つのとき
//   注意 : 重みは 0 以上。距離の合計が T の最大値の半分を超えないこと
template <class T>
DijkstraResult<T> dijkstra(const WGraph<T>& g, const vector<int>& starts) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dijkstra.cpp
    return dijkstra_func<T>(g.size(), starts, [&](int v, auto&& relax) {
        for (auto& e : g[v]) relax(e.to, e.cost);
    });
}
// dijkstra(g, s) : 始点が 1 つのとき（s : 始点）
template <class T>
DijkstraResult<T> dijkstra(const WGraph<T>& g, int s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dijkstra.cpp
    return dijkstra(g, vector<int>{s});
}

// ---------- 削除可: 最短路の本数・最短路 DAG（本体が必要。重みは正であること） ----------
// C : 本数の型（long long か mint。本数は指数的に増えるので、たいてい mint）
// auto [r, cnt] = count_shortest_paths<mint>(g, s);   r = dijkstra の結果、cnt[v] = s から v への最短路の本数
// shortest_path_dag(g, r.dist)                       最短路 DAG: 「その辺を通る最短路がある」辺だけの隣接リスト
//   ★ 辺 u → v（重み w）が最短路に使える ⇔ dist[u] + w == dist[v]
//   重み 0 の辺があると、同じ距離の頂点の間で順番が決まらない（0 の閉路なら本数が無限）ので使えない
//   重みなしグラフは全部の重みを 1 にして渡す
// ◆ shortest_path_dag(g, dist) : 最短路に使われうる辺だけを残した隣接リスト（最短路 DAG）
//   g    : dijkstra に渡したグラフ
//   dist : dijkstra の結果の dist
template <class T>
vector<vector<int>> shortest_path_dag(const WGraph<T>& g, const vector<T>& dist) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dijkstra.cpp
    const T INF = DijkstraResult<T>::INF;
    vector<vector<int>> dag(g.size());
    for (int v = 0; v < (int)g.size(); v++) {
        if (dist[v] == INF) continue;
        for (auto& e : g[v]) if (dist[v] + e.cost == dist[e.to]) dag[v].push_back(e.to);
    }
    return dag;
}
// ◆ count_shortest_paths<C>(g, s) : s から各頂点への最短路の本数
//   C : 本数の型（mint か long long。本数はすぐ大きくなるのでたいてい mint）
//   g : WGraph<T>。重みはすべて正（0 があると assert で止まる）
//   s : 始点
//   返り値 : {dijkstra の結果, cnt}。cnt[v] = 最短路の本数（行けなければ 0）
template <class C, class T>
pair<DijkstraResult<T>, vector<C>> count_shortest_paths(const WGraph<T>& g, int s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dijkstra.cpp
    auto r = dijkstra(g, s);
    int n = g.size();
    vector<int> ord;
    for (int v = 0; v < n; v++) if (r.reachable(v)) ord.push_back(v);
    sort(ord.begin(), ord.end(), [&](int a, int b) { return r.dist[a] < r.dist[b]; });  // ★ 距離の近い順 = 最短路 DAG のトポロジカル順（重みが正なので）
    vector<C> cnt(n, C(0));
    cnt[s] = C(1);
    for (int v : ord)
        for (auto& e : g[v]) {
            assert(e.cost > 0);  // 重みは正であること
            if (r.dist[v] + e.cost == r.dist[e.to]) cnt[e.to] += cnt[v];  // ★ 最短路の最後の辺が v → to のものを足す
        }
    return {r, cnt};
}

// ---------- 削除可: k 番目最短路（同じ頂点・辺を何度通ってもよい道） O(K (N + M) log(K M)) ----------
// auto res = kth_shortest(g, s, K);   res[v] = s から v への道の長さを、短い順に K 個（足りなければあるだけ）
//   辺の並びが違えば別の道として数える（長さが同じでも別々に入る）。s → s の長さ 0 の道も 1 個と数える
//   重みは正であること（0 の辺があると同じ長さの道が無限にできることがある）
// ★ ダイクストラで「各頂点を K 回まで取り出してよい」ことにするだけ。v を i 回目に取り出したときの距離が i 番目の長さ
// ◆ kth_shortest(g, s, K) : s から各頂点への道の長さを短い順に K 個。O(K (N + M) log)
//   g : WGraph<T>。重みは正であること
//   s : 始点（s → s の長さ 0 の道も 1 個に数える）
//   K : 何個まで求めるか。同じ頂点を何度通ってもよい道で、辺の並びが違えば別の道
//   返り値 : res[v] = 長さの昇順の列（K 個未満のこともある）
template <class T>
vector<vector<T>> kth_shortest(const WGraph<T>& g, int s, int K) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dijkstra.cpp
    vector<vector<T>> res(g.size());
    using P = pair<T, int>;
    priority_queue<P, vector<P>, greater<P>> pq;
    pq.push({0, s});
    while (!pq.empty()) {
        auto [d, v] = pq.top();
        pq.pop();
        if ((int)res[v].size() >= K) continue;  // ★ もう K 回取り出した頂点は無視
        res[v].push_back(d);
        for (auto& e : g[v])
            if ((int)res[e.to].size() < K) pq.push({d + e.cost, e.to});
    }
    return res;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    WGraph<long long> g(4);
    g[0].push_back({1, 5});
    g[0].push_back({2, 1});
    g[2].push_back({1, 2});
    auto r = dijkstra(g, 0);
    assert(r.dist[1] == 3 && !r.reachable(3) && (r.path(1) == vector<long long>{0, 2, 1}));

    // 拡張ダイクストラ: 「クーポン K 枚まで、辺を 1 本無料にできる」
    // 【自分で定義するもの】状態 → 整数 id の対応。ここでは (頂点 v, 残り枚数 k) → v * (K+1) + k
    int K = 1, n = 4;
    auto id = [&](int v, int k) { return v * (K + 1) + k; };
    auto r2 = dijkstra_func<long long>(n * (K + 1), {id(0, K)}, [&](int s, auto&& relax) {
        int v = s / (K + 1), k = s % (K + 1);
        for (auto [to, c] : g[v]) {
            relax(id(to, k), c);                 // 普通に払う
            if (k > 0) relax(id(to, k - 1), 0);  // クーポンを使う
        }
    });
    long long best = LLONG_MAX;
    for (int k = 0; k <= K; k++) best = min(best, r2.dist[id(1, k)]);  // 頂点 1 の全状態の最小
    assert(best == 0);
    // ランダムテスト: 愚直な緩和（n 回すべての辺を見る）と比較、復元した経路が本当にその長さか
    {
        mt19937 rr(110);
        const long long INF = DijkstraResult<long long>::INF;
        for (int it = 0; it < 3000; it++) {
            int nn = rr() % 8 + 1;
            WGraph<long long> h(nn);
            vector<tuple<int, int, long long>> es;
            for (int e = rr() % 15; e > 0; e--) {
                int a = rr() % nn, b = rr() % nn;
                long long c = rr() % 10;
                h[a].push_back({b, c}), es.push_back({a, b, c});
            }
            vector<int> st;
            for (int i = rr() % 2 + 1; i > 0; i--) st.push_back(rr() % nn);
            auto res = dijkstra(h, st);
            vector<long long> ref(nn, INF);
            for (int s : st) ref[s] = 0;
            for (int rep = 0; rep < nn; rep++)
                for (auto [a, b, c] : es) if (ref[a] != INF) ref[b] = min(ref[b], ref[a] + c);
            for (int v = 0; v < nn; v++) {
                assert(res.dist[v] == ref[v]);
                if (ref[v] == INF) continue;
                auto p = res.path(v);
                assert(p.back() == v && find(st.begin(), st.end(), p[0]) != st.end());
                long long sum = 0;
                for (size_t i = 0; i + 1 < p.size(); i++) {
                    long long best = INF;
                    for (auto [a, b, c] : es) if (a == p[i] && b == p[i + 1]) best = min(best, c);
                    assert(best != INF);
                    sum += best;
                }
                assert(sum == ref[v]);
            }
        }
    }
    // 最短路の本数: 0 → 1 (1), 0 → 2 (1), 1 → 3 (1), 2 → 3 (1), 0 → 3 (2) → 3 への最短路は 3 本（長さ 2）
    {
        WGraph<long long> h(4);
        h[0].push_back({1, 1}), h[0].push_back({2, 1}), h[1].push_back({3, 1}), h[2].push_back({3, 1}), h[0].push_back({3, 2});
        auto [r3, cnt] = count_shortest_paths<long long>(h, 0);
        assert(r3.dist[3] == 2 && cnt[3] == 3 && cnt[1] == 1);
        auto dg = shortest_path_dag(h, r3.dist);
        assert(dg[0].size() == 3 && dg[1].size() == 1);
    }
    // ランダムテスト: 単純な道を全部列挙して数えたものと比較（重み 1..4）
    {
        mt19937 r4(142);
        for (int it = 0; it < 1000; it++) {
            int n = r4() % 7 + 1;
            WGraph<long long> h(n);
            vector<vector<long long>> w(n, vector<long long>(n, 0));
            for (int e = r4() % 14; e > 0; e--) {
                int a = r4() % n, b = r4() % n;
                if (a == b) continue;
                long long c = r4() % 4 + 1;
                h[a].push_back({b, c});
            }
            auto [r5, cnt] = count_shortest_paths<long long>(h, 0);
            vector<long long> ways(n, 0);
            vector<bool> on(n, false);
            function<void(int, long long)> dfs = [&](int v, long long d) {  // 0 からの単純な道を全部たどる
                if (d == r5.dist[v]) ways[v]++;
                on[v] = true;
                for (auto& e : h[v]) if (!on[e.to]) dfs(e.to, d + e.cost);
                on[v] = false;
            };
            dfs(0, 0);
            for (int v = 0; v < n; v++) assert(cnt[v] == (r5.reachable(v) ? ways[v] : 0));
            auto dg = shortest_path_dag(h, r5.dist);
            vector<long long> viaDag(n, 0);  // 最短路 DAG の入ってくる辺の本数の和 = 本数
            viaDag[0] = 1;
            for (int v = 0; v < n; v++)
                for (int to : dg[v]) { assert(r5.reachable(to) && r5.dist[v] < r5.dist[to]); viaDag[to] += cnt[v]; }
            for (int v = 0; v < n; v++) assert(viaDag[v] == cnt[v]);
        }
    }
    // k 番目最短路: 0 → 1 (1), 0 → 2 (2), 1 → 2 (2), 2 → 0 (1)
    //   0 から 2 へ: 0→2 = 2、0→1→2 = 3、0→2→0→2 = 5、0→1→2→0→2 = 6、0→2→0→1→2 = 6 …
    {
        WGraph<long long> h(3);
        h[0].push_back({1, 1}), h[0].push_back({2, 2}), h[1].push_back({2, 2}), h[2].push_back({0, 1});
        auto ks = kth_shortest(h, 0, 5);
        assert((ks[2] == vector<long long>{2, 3, 5, 6, 6}));
        assert(ks[0][0] == 0 && ks[0][1] == 3);  // 0 → 0: 長さ 0、0→2→0 = 3
    }
    // ランダムテスト: 長さ B 以下の道を全部たどって集めたものと比較
    {
        mt19937 r6(180);
        for (int it = 0; it < 1000; it++) {
            int n = r6() % 5 + 1, K = r6() % 6 + 1;
            WGraph<long long> h(n);
            for (int e = r6() % 9; e > 0; e--) h[r6() % n].push_back({(int)(r6() % n), (long long)(r6() % 4 + 1)});
            int s = r6() % n;
            const long long B = 9;
            vector<vector<long long>> all(n);
            function<void(int, long long)> dfs = [&](int v, long long d) {
                all[v].push_back(d);
                for (auto& e : h[v]) if (d + e.cost <= B) dfs(e.to, d + e.cost);
            };
            dfs(s, 0);
            auto ks = kth_shortest(h, s, K);
            for (int v = 0; v < n; v++) {
                sort(all[v].begin(), all[v].end());
                vector<long long> want(all[v].begin(), all[v].begin() + min<size_t>(K, all[v].size()));
                vector<long long> got;
                for (long long x : ks[v]) if (x <= B) got.push_back(x);
                assert(got == want);
                assert((int)ks[v].size() <= K && is_sorted(ks[v].begin(), ks[v].end()));
                if ((int)all[v].size() >= K) assert((int)ks[v].size() == K);  // B 以下だけで K 個あるなら K 個そろう
            }
        }
    }
    puts("OK");
}
