// =====================================================================
// dp/tsp.cpp  bit DP の巡回セールスマン問題（TSP）
//   N 個の頂点をすべて 1 回ずつ訪れる最短の道（または最短の一周）を O(2^N N^2) で求める。N <= 16〜18 程度
//   dp[mask][v] = 「訪れた頂点の集合が mask で、今 v にいる」ときの最小コスト
//   ★ 「どの順番で回ったか」は忘れて「どこを回ったか（集合）と今どこにいるか」だけ覚える → N! が 2^N N になる
// 依存: なし
// ABC / ARC での使用例:
//   ABC180 E - Traveling Salesman among Aerial Cities  https://atcoder.jp/contests/abc180/tasks/abc180_e
//     → d[i][j] = abs(a−a') + abs(b−b') + max(0, c'−c) で tsp_cycle(d, dp, 0)
//   ABC190 E - Magical Ornament  https://atcoder.jp/contests/abc190/tasks/abc190_e
//     → K 個の宝石から BFS して K×K の距離表 → 始点自由なので、全頂点へ 0 で行けるダミー頂点を足して tsp_path。答え +1
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// T : コストの型（long long）
// d[i][j] : i から j へ直接行くコスト（行けないなら TSP_INF<T>）。対称でなくてよい
//           直接の辺がない問題なら、先にワーシャルフロイドや BFS で「最短距離」の表にしておく
// auto dp = tsp_dp(d, s);          s から出発
//   tsp_cycle(d, dp, s)            全頂点を回って s に戻る最小コスト（無理なら TSP_INF<T>）
//   tsp_path(d, dp)                全頂点を回る最小コスト（終わりはどこでもよい）
//   tsp_restore(d, dp, s, end)     end で終わる最小の回り方の頂点の順番（削除可）
template <class T> constexpr T TSP_INF = numeric_limits<T>::max() / 4;
template <class T>
vector<vector<T>> tsp_dp(const vector<vector<T>>& d, int s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/tsp.cpp
    int n = d.size();
    vector<vector<T>> dp(1 << n, vector<T>(n, TSP_INF<T>));
    dp[1 << s][s] = 0;
    for (int mask = 0; mask < (1 << n); mask++)  // ★ mask の小さい順 = 訪れた集合が小さい方から
        for (int v = 0; v < n; v++) {
            if (dp[mask][v] == TSP_INF<T>) continue;
            for (int to = 0; to < n; to++) {
                if (mask >> to & 1 || d[v][to] == TSP_INF<T>) continue;  // まだ訪れていない頂点へだけ進む
                T nv = dp[mask][v] + d[v][to];
                int nm = mask | 1 << to;
                if (nv < dp[nm][to]) dp[nm][to] = nv;  // ★ 集合に to を加えて、今 to にいる
            }
        }
    return dp;
}
template <class T>
T tsp_cycle(const vector<vector<T>>& d, const vector<vector<T>>& dp, int s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/tsp.cpp
    int n = d.size(), full = (1 << n) - 1;
    if (n == 1) return 0;
    T best = TSP_INF<T>;
    for (int v = 0; v < n; v++)
        if (v != s && dp[full][v] != TSP_INF<T> && d[v][s] != TSP_INF<T>) best = min(best, dp[full][v] + d[v][s]);  // ★ 最後に s へ戻る
    return best;
}
template <class T>
T tsp_path(const vector<vector<T>>& d, const vector<vector<T>>& dp) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/tsp.cpp
    int n = d.size();
    return *min_element(dp[(1 << n) - 1].begin(), dp[(1 << n) - 1].end());
}

// ---------- 削除可: 回り方の復元 ----------
// 返り値: s から始まり end で終わる、全頂点を回る最小の順番（dp[full][end] が TSP_INF なら空）
template <class T>
vector<int> tsp_restore(const vector<vector<T>>& d, const vector<vector<T>>& dp, int s, int end) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/tsp.cpp
    int n = d.size(), mask = (1 << n) - 1, v = end;
    if (dp[mask][v] == TSP_INF<T>) return {};
    vector<int> order = {v};
    while (v != s || mask != (1 << s)) {  // ★ 後ろから「どこから来たか」をたどる
        int pm = mask ^ (1 << v);
        for (int u = 0; u < n; u++)
            if ((pm >> u & 1) && dp[pm][u] != TSP_INF<T> && d[u][v] != TSP_INF<T> && dp[pm][u] + d[u][v] == dp[mask][v]) {
                v = u, mask = pm;
                break;
            }
        order.push_back(v);
    }
    reverse(order.begin(), order.end());
    return order;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 4 頂点の正方形 0-1-2-3（隣は 1、対角は 10）
    const long long X = 10;
    vector<vector<long long>> d = {{0, 1, X, 1}, {1, 0, 1, X}, {X, 1, 0, 1}, {1, X, 1, 0}};
    auto dp = tsp_dp(d, 0);
    assert(tsp_cycle(d, dp, 0) == 4 && tsp_path(d, dp) == 3);  // 一周は 0-1-2-3-0、道は 0-1-2-3
    auto ord = tsp_restore(d, dp, 0, 3);
    assert((ord == vector<int>{0, 1, 2, 3}));

    // ランダムテスト: 順列を全部試したものと比較（非対称、行けない辺あり）
    mt19937 rr(139);
    const long long INF = TSP_INF<long long>;
    for (int it = 0; it < 1000; it++) {
        int n = rr() % 7 + 1, s = rr() % n;
        vector<vector<long long>> w(n, vector<long long>(n));
        for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) w[i][j] = (i != j && rr() % 5 == 0) ? INF : (long long)(rr() % 20);
        auto t = tsp_dp(w, s);
        vector<int> p;
        for (int i = 0; i < n; i++) if (i != s) p.push_back(i);
        long long bc = INF, bp = INF;
        vector<long long> bend(n, INF);
        do {
            long long c = 0;
            int cur = s;
            bool ok = true;
            for (int v : p) { if (w[cur][v] == INF) { ok = false; break; } c += w[cur][v], cur = v; }
            if (!ok) continue;
            bp = min(bp, c), bend[cur] = min(bend[cur], c);
            if (n == 1) bc = 0;
            else if (w[cur][s] != INF) bc = min(bc, c + w[cur][s]);
        } while (next_permutation(p.begin(), p.end()));
        assert(tsp_cycle(w, t, s) == bc && tsp_path(w, t) == bp);
        for (int e = 0; e < n; e++) {
            auto o = tsp_restore(w, t, s, e);
            if (bend[e] == INF) { assert(o.empty()); continue; }
            assert((int)o.size() == n && o[0] == s && o.back() == e);
            long long c = 0;
            for (int i = 0; i + 1 < n; i++) c += w[o[i]][o[i + 1]];
            assert(c == bend[e] && set<int>(o.begin(), o.end()).size() == (size_t)n);
        }
    }
    puts("OK");
}
