// =====================================================================
// dp/knapsack.cpp  ナップサック問題
//   重さ w[i]・価値 v[i] の品物を、重さの合計 W 以下で選んで価値の合計を最大化する
//   本体A: 0-1 ナップサック（各品物 1 個まで）       O(N W)          W が小さいとき
//   本体B: 0-1 ナップサック（価値で DP）              O(N Σv)         W が大きく（~1e9）価値の合計が小さいとき
//   本体C: 個数制限なしナップサック（何個でも）       O(N W)
//   本体D: 個数制限ありナップサック（品物 i は c[i] 個まで） O(N W log c)
//   本体E: 部分和問題（合計がちょうど S になる選び方があるか） O(N S)
//   どれも互いに独立
// 依存: なし
// ABC / ARC での使用例:
//   ABC032 D - ナップサック問題  https://atcoder.jp/contests/abc032/tasks/abc032_d
//     → 制約で 3 通りに分岐: N 小 → 半分全列挙（basic/meet_in_the_middle.cpp の knapsack_mitm）、W 小 → knapsack_01、価値小 → knapsack_01_by_value
//   EDPC E - Knapsack 2  https://atcoder.jp/contests/dp/tasks/dp_e
//     → W ≤ 10^9、価値が小さい → knapsack_01_by_value（ABC ではないが典型）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 0-1 ナップサック（重さで DP） ----------
// 返り値: 重さ W 以下での価値の最大値
long long knapsack_01(const vector<int>& w, const vector<long long>& v, int W) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/knapsack.cpp
    vector<long long> dp(W + 1, 0);  // dp[j] = 重さ j 以下での価値の最大
    for (int i = 0; i < (int)w.size(); i++)
        for (int j = W; j >= w[i]; j--)  // ★ 重い方から回す → 同じ品物を 2 回使わない
            dp[j] = max(dp[j], dp[j - w[i]] + v[i]);  // ★ 品物 i を入れる / 入れない
    return dp[W];
}

// ---------- 本体B: 0-1 ナップサック（価値で DP。W が大きいとき） ----------
// 返り値: 重さ W 以下での価値の最大値
long long knapsack_01_by_value(const vector<long long>& w, const vector<int>& v, long long W) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/knapsack.cpp
    int V = accumulate(v.begin(), v.end(), 0);
    const long long INF = LLONG_MAX / 2;
    vector<long long> dp(V + 1, INF);  // ★ dp[x] = 価値をちょうど x にするのに必要な最小の重さ（重さと価値を入れ替える）
    dp[0] = 0;
    for (int i = 0; i < (int)w.size(); i++)
        for (int x = V; x >= v[i]; x--)
            if (dp[x - v[i]] != INF) dp[x] = min(dp[x], dp[x - v[i]] + w[i]);
    for (int x = V; x >= 0; x--)
        if (dp[x] <= W) return x;  // ★ 重さ W 以下で達成できる最大の価値
    return 0;
}

// ---------- 本体C: 個数制限なしナップサック ----------
long long knapsack_unbounded(const vector<int>& w, const vector<long long>& v, int W) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/knapsack.cpp
    vector<long long> dp(W + 1, 0);
    for (int i = 0; i < (int)w.size(); i++)
        for (int j = w[i]; j <= W; j++)  // ★ 軽い方から回す → 同じ品物を何回でも使える（本体A との違いはここだけ）
            dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
    return dp[W];
}

// ---------- 本体D: 個数制限ありナップサック ----------
// c[i] : 品物 i を使える個数
long long knapsack_bounded(const vector<int>& w, const vector<long long>& v, const vector<long long>& c, int W) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/knapsack.cpp
    vector<long long> dp(W + 1, 0);
    for (int i = 0; i < (int)w.size(); i++) {
        long long rest = c[i];
        for (long long k = 1; rest > 0; k *= 2) {  // ★ c 個を 1, 2, 4, ..., 余り の束に分ける（どの個数も束の組合せで作れる）
            long long take = min(k, rest);
            rest -= take;
            long long tw = take * w[i], tv = take * v[i];
            if (tw > W) continue;
            for (int j = W; j >= tw; j--) dp[j] = max(dp[j], dp[j - tw] + tv);  // ★ 各束を 0-1 ナップサックの品物として扱う
        }
    }
    return dp[W];
}

// ---------- 本体E: 部分和問題 ----------
// 返り値: ok[s] = 「いくつか選んで合計をちょうど s にできるか」（0 <= s <= S）
vector<bool> subset_sum(const vector<int>& a, int S) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/knapsack.cpp
    vector<bool> ok(S + 1, false);
    ok[0] = true;
    for (int x : a)
        for (int s = S; s >= x; s--)
            if (ok[s - x]) ok[s] = true;  // ★ s - x が作れるなら x を足して s が作れる
    return ok;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    vector<int> w = {3, 4, 5};
    vector<long long> v = {30, 50, 60};
    assert(knapsack_01(w, v, 8) == 90);          // 3 + 5
    assert(knapsack_unbounded(w, v, 8) == 100);  // 4 + 4
    assert(knapsack_bounded(w, v, {1, 1, 1}, 8) == 90);
    assert(knapsack_bounded(w, v, {3, 2, 0}, 10) == 110);  // 重さ 3 を 2 個 + 重さ 4 を 1 個
    assert(knapsack_01_by_value({3, 4, 5}, {30, 50, 60}, 8) == 90);
    auto ok = subset_sum({3, 5, 7}, 15);
    assert(ok[0] && ok[8] && ok[15] && !ok[9] && !ok[1]);

    // ランダムテスト: bit 全探索と比較
    mt19937 rng(1);
    for (int it = 0; it < 3000; it++) {
        int n = rng() % 6 + 1, W = rng() % 25;
        vector<int> wi(n), vi(n);
        vector<long long> wl(n), vl(n), cnt(n);
        for (int i = 0; i < n; i++) wi[i] = rng() % 8 + 1, vi[i] = rng() % 20, wl[i] = wi[i], vl[i] = vi[i], cnt[i] = rng() % 4;
        long long best = 0;
        for (int m = 0; m < (1 << n); m++) {
            long long sw = 0, sv = 0;
            for (int i = 0; i < n; i++) if (m >> i & 1) sw += wi[i], sv += vi[i];
            if (sw <= W) best = max(best, sv);
        }
        assert(knapsack_01(wi, vl, W) == best);
        assert(knapsack_01_by_value(wl, vi, W) == best);
        // 個数制限あり: 各品物の個数を全部試す
        long long bb = 0;
        function<void(int, long long, long long)> rec = [&](int i, long long sw, long long sv) {
            if (sw > W) return;
            if (i == n) { bb = max(bb, sv); return; }
            for (long long k = 0; k <= cnt[i]; k++) rec(i + 1, sw + k * wi[i], sv + k * vi[i]);
        };
        rec(0, 0, 0);
        assert(knapsack_bounded(wi, vl, cnt, W) == bb);
        // 個数制限なし = 個数制限を十分大きくしたもの
        vector<long long> big(n, 30);
        assert(knapsack_unbounded(wi, vl, W) == knapsack_bounded(wi, vl, big, W));
    }
    puts("OK");
}
