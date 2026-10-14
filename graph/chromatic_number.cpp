// =====================================================================
// graph/chromatic_number.cpp  彩色数（隣り合う頂点を違う色にするのに必要な最小の色数）
//   一般のグラフでは難しい問題（NP 困難）なので N <= 20 程度。O(2^N N)
//   ★ 包除原理: 「独立集合（どの 2 頂点も隣り合わない集合）を k 個選んで全頂点を覆う選び方の数」
//      = Σ_{S ⊆ V} (−1)^{N − |S|} × (S の中の独立集合の個数)^k。これが 0 でない最小の k が答え
//      数が大きくなるので mod で計算する（2 つの素数で計算して、どちらかが 0 でなければ 0 でないと判断）
// 依存: なし
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// g : 無向グラフの隣接リスト（頂点 0..n-1、n <= 25 程度まで。メモリ 2^n × 8 バイト × 2）
// int k = chromatic_number(g);   彩色数（頂点が 0 個なら 0、辺がなければ 1）
//   自己ループがあると彩色できないので -1
int chromatic_number(const vector<vector<int>>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/chromatic_number.cpp
    int n = g.size();
    if (n == 0) return 0;
    vector<unsigned> adj(n, 0);
    for (int v = 0; v < n; v++)
        for (int to : g[v]) {
            if (to == v) return -1;
            adj[v] |= 1u << to, adj[to] |= 1u << v;
        }
    int full = 1 << n;
    vector<int> I(full);  // I[S] = S の中の独立集合の個数（空集合も含む）。n <= 25 なら 2^25 以下で int に収まる
    I[0] = 1;
    for (int S = 1; S < full; S++) {
        int v = __builtin_ctz(S);
        I[S] = I[S & ~(1 << v)] + I[S & ~(1 << v) & ~(int)adj[v]];  // ★ v を使わない独立集合 + v を使う独立集合（v の隣は使えない）
    }
    const long long MODS[2] = {1000000007, 998244353};
    vector<long long> pw[2] = {vector<long long>(full), vector<long long>(full)};
    for (int t = 0; t < 2; t++)
        for (int S = 0; S < full; S++) pw[t][S] = (__builtin_popcount(S) % 2 == n % 2) ? 1 : MODS[t] - 1;  // 符号 (−1)^{n − |S|}
    for (int k = 1; k <= n; k++) {
        bool nonzero = false;
        for (int t = 0; t < 2; t++) {
            long long sum = 0;
            for (int S = 0; S < full; S++) {
                pw[t][S] = pw[t][S] * I[S] % MODS[t];  // ★ (−1)^{n−|S|} I[S]^k を k ごとに 1 回掛けて更新
                sum += pw[t][S];
            }
            if (sum % MODS[t] != 0) nonzero = true;
        }
        if (nonzero) return k;  // ★ k 色で塗れる
    }
    return n;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    auto make = [](int n, vector<pair<int, int>> es) {
        vector<vector<int>> g(n);
        for (auto [a, b] : es) g[a].push_back(b), g[b].push_back(a);
        return g;
    };
    // 奇数長の閉路（5 角形）は 3 色、偶数長（4 角形）は 2 色、完全グラフ K4 は 4 色、辺なしは 1 色
    assert(chromatic_number(make(5, {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}})) == 3);
    assert(chromatic_number(make(4, {{0, 1}, {1, 2}, {2, 3}, {3, 0}})) == 2);
    assert(chromatic_number(make(4, {{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}})) == 4);
    assert(chromatic_number(make(3, {})) == 1 && chromatic_number(make(0, {})) == 0);
    assert(chromatic_number(make(2, {{1, 1}})) == -1);
    // ピーターセングラフ（10 頂点）は 3 色
    {
        vector<pair<int, int>> es;
        for (int i = 0; i < 5; i++) es.push_back({i, (i + 1) % 5}), es.push_back({i, i + 5}), es.push_back({i + 5, (i + 2) % 5 + 5});
        assert(chromatic_number(make(10, es)) == 3);
    }
    // ランダムテスト: 色の塗り方を全部試す
    {
        mt19937 rr(192);
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 7 + 1;
            vector<pair<int, int>> es;
            int p = rr() % 100;
            for (int a = 0; a < n; a++) for (int b = a + 1; b < n; b++) if ((int)(rr() % 100) < p) es.push_back({a, b});
            int want = n;
            for (int k = 1; k <= n && want == n; k++) {
                int tot = 1;
                for (int i = 0; i < n; i++) tot *= k;
                for (int code = 0; code < tot; code++) {
                    vector<int> col(n);
                    for (int i = 0, c = code; i < n; i++, c /= k) col[i] = c % k;
                    bool ok = true;
                    for (auto [a, b] : es) if (col[a] == col[b]) ok = false;
                    if (ok) { want = k; break; }
                }
            }
            assert(chromatic_number(make(n, es)) == want);
        }
    }
    puts("OK");
}
