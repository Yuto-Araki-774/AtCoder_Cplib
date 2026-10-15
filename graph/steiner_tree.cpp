// =====================================================================
// graph/steiner_tree.cpp  最小シュタイナー木（指定した K 個の頂点をすべてつなぐ、重み最小の木）
//   K 個全部が「つなぐべき頂点（ターミナル）」なら最小全域木だが、途中で他の頂点を使ってもよいのが違い。一般には難しい
//   K が小さい（~10）とき、bit DP で O(3^K N + 2^K M log N)
//   ★ dp[S][v] = ターミナルの集合 S と頂点 v をつなぐ木の最小の重み
//      (1) 頂点 v で 2 つの木をくっつける: dp[S][v] = min_{T ⊂ S} dp[T][v] + dp[S − T][v]
//      (2) 木を辺に沿って伸ばす: dp[S][*] でダイクストラ
// 依存: なし
// ABC / ARC での使用例:
//   ABC364 G - Last Major City  https://atcoder.jp/contests/abc364/tasks/abc364_g
//     → ターミナルを都市 1..K−1 にして steiner_tree。都市 i の答えは dp[full][i]（「全ターミナルと頂点 i をつなぐ最小」がそのまま答え）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// edges = 無向辺 {a, b, 重み}（重みは 0 以上）、terms = つなぐべき頂点の一覧（重複 OK）
// auto dp = steiner_tree<long long>(n, edges, terms);
//   dp[full][v] の最小（full = (1 << K) − 1）が答え: steiner_min(dp)
//   つなげないなら STEINER_INF<T>
//   dp[S][v] も使える（例: 「ターミナルの一部 S と v をつなぐ最小」）
template <class T> constexpr T STEINER_INF = numeric_limits<T>::max() / 4;
// ◆ steiner_tree<T>(n, edges, terms) : 指定した頂点（ターミナル）をつなぐ最小の重みの木。O(3^K N + 2^K M log N)
//   T     : 重みの型（必ず書く）
//   n     : 頂点数
//   edges : 無向辺 {a, b, 重み} の一覧（重みは 0 以上）
//   terms : ターミナルの一覧（K 個。K <= 10 くらいまで。重複してもよい）
//   返り値 : dp[S][v] = ターミナルの集合 S（bit）と頂点 v をつなぐ木の最小の重み（つなげなければ STEINER_INF<T>）
template <class T>
vector<vector<T>> steiner_tree(int n, const vector<tuple<int, int, T>>& edges, const vector<int>& terms) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/steiner_tree.cpp
    int K = terms.size();
    vector<vector<pair<int, T>>> g(n);
    for (auto [a, b, w] : edges) g[a].push_back({b, w}), g[b].push_back({a, w});
    vector<vector<T>> dp(1 << K, vector<T>(n, STEINER_INF<T>));
    for (int i = 0; i < K; i++) dp[1 << i][terms[i]] = 0;
    for (int v = 0; v < n; v++) dp[0][v] = 0;
    using P = pair<T, int>;
    for (int S = 1; S < (1 << K); S++) {
        for (int sub = (S - 1) & S; sub > 0; sub = (sub - 1) & S) {  // ★ S を 2 つに分けて、同じ頂点 v でくっつける
            if (sub < (S ^ sub)) continue;  // 同じ分け方を 2 回見ない
            for (int v = 0; v < n; v++)
                if (dp[sub][v] != STEINER_INF<T> && dp[S ^ sub][v] != STEINER_INF<T>)
                    dp[S][v] = min(dp[S][v], dp[sub][v] + dp[S ^ sub][v]);
        }
        priority_queue<P, vector<P>, greater<P>> pq;  // ★ 辺に沿って木を伸ばす（多始点ダイクストラ）
        for (int v = 0; v < n; v++) if (dp[S][v] != STEINER_INF<T>) pq.push({dp[S][v], v});
        while (!pq.empty()) {
            auto [d, v] = pq.top();
            pq.pop();
            if (d != dp[S][v]) continue;
            for (auto [to, w] : g[v])
                if (d + w < dp[S][to]) dp[S][to] = d + w, pq.push({dp[S][to], to});
        }
    }
    return dp;
}
// ◆ steiner_min(dp) : ターミナルを全部つなぐ最小の重み（= dp[全部][v] の最小）
//   dp : steiner_tree の返り値
template <class T>
T steiner_min(const vector<vector<T>>& dp) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/steiner_tree.cpp
    return *min_element(dp.back().begin(), dp.back().end());
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 星形: 中心 0 から 1, 2, 3 へ重み 1。1-2, 2-3, 1-3 は重み 3。ターミナル {1, 2, 3}
    //   中心を使うと 1 + 1 + 1 = 3、使わないと 3 + 3 = 6 → 3
    {
        vector<tuple<int, int, long long>> es = {{0, 1, 1}, {0, 2, 1}, {0, 3, 1}, {1, 2, 3}, {2, 3, 3}, {1, 3, 3}};
        auto dp = steiner_tree<long long>(4, es, {1, 2, 3});
        assert(steiner_min(dp) == 3);
        assert(dp[0b011][0] == 2 && dp[0b011][1] == 2);  // {1, 2} と頂点 0 をつなぐ: 0-1, 0-2 で 2
        assert(steiner_min(steiner_tree<long long>(4, es, {2})) == 0);
        assert(steiner_min(steiner_tree<long long>(5, es, {1, 4})) == STEINER_INF<long long>);  // 4 は孤立
    }
    // ランダムテスト: 辺の部分集合を全部試して、ターミナルが全部つながる最小の重みと比較
    {
        mt19937 rr(193);
        for (int it = 0; it < 2000; it++) {
            int n = rr() % 6 + 1, m = rr() % 10, K = rr() % min(n, 4) + 1;
            vector<tuple<int, int, long long>> es;
            for (int i = 0; i < m; i++) es.push_back({(int)(rr() % n), (int)(rr() % n), (long long)(rr() % 10)});
            vector<int> terms;
            for (int i = 0; i < K; i++) terms.push_back(rr() % n);
            long long want = STEINER_INF<long long>;
            for (int mask = 0; mask < (1 << m); mask++) {
                vector<int> p(n);
                iota(p.begin(), p.end(), 0);
                function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
                long long w = 0;
                for (int i = 0; i < m; i++) if (mask >> i & 1) { auto [a, b, c] = es[i]; p[f(a)] = f(b), w += c; }
                bool ok = true;
                for (int t : terms) if (f(t) != f(terms[0])) ok = false;
                if (ok) want = min(want, w);
            }
            assert(steiner_min(steiner_tree<long long>(n, es, terms)) == want);
        }
    }
    puts("OK");
}
