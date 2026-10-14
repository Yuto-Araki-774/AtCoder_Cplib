// =====================================================================
// dp/interval_dp.cpp  区間 DP のテンプレート
//   「区間 [l, r) の答えを、2 つに分けた [l, m) と [m, r) の答えから作る」DP を O(N^3) で計算する
//   短い区間から順に埋めるので、長い区間を計算するときには必ず中の短い区間が計算済み
//   典型: 隣り合うものを合体させるコストの最小（スライム）、行列の積の順番、括弧の付け方
// 依存: なし
// ABC / ARC での使用例:
//   EDPC N - Slimes  https://atcoder.jp/contests/dp/tasks/dp_n
//     → 使用例のスライムの合体そのもの（ABC ではないが区間 DP の定番）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// T : 値の型
// auto dp = interval_dp<T>(n, leaf, cost, minimize);
//   dp[l][r] = 区間 [l, r)（0 <= l < r <= n）の答え。全体の答えは dp[0][n]
// 【自分で定義するもの】（main の中にラムダで書く）
//   leaf(i)          : 長さ 1 の区間 [i, i+1) の答え
//   cost(l, m, r)    : [l, m) と [m, r) を 1 つにまとめるときに足す値
//   minimize         : true なら最小（省略時）、false なら最大
//   dp[l][r] = min_{l < m < r} ( dp[l][m] + dp[m][r] + cost(l, m, r) )
//
// 例: スライムの合体（隣り合う 2 匹を合体するとき大きさの和を払う。全部 1 匹にする最小コスト）
//     leaf(i) = 0, cost(l, m, r) = a[l] + ... + a[r-1]（累積和で O(1)）
// 例: 行列の積の順番（行列 i は d[i] × d[i+1]）  leaf(i) = 0, cost(l, m, r) = d[l] * d[m] * d[r]
// この形にならない区間 DP（両端から取り除くゲームなど）は、下のループの形（len を 1 ずつ増やす）だけ真似して自分で書く
template <class T, class Leaf, class Cost>
vector<vector<T>> interval_dp(int n, Leaf leaf, Cost cost, bool minimize = true) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/interval_dp.cpp
    vector<vector<T>> dp(n + 1, vector<T>(n + 1, T(0)));
    for (int i = 0; i < n; i++) dp[i][i + 1] = leaf(i);
    for (int len = 2; len <= n; len++)          // ★ 短い区間から順に
        for (int l = 0; l + len <= n; l++) {
            int r = l + len;
            T best = dp[l][l + 1] + dp[l + 1][r] + cost(l, l + 1, r);
            for (int m = l + 2; m < r; m++) {     // ★ 分け目 m を全部試す
                T v = dp[l][m] + dp[m][r] + cost(l, m, r);
                if (minimize ? v < best : v > best) best = v;
            }
            dp[l][r] = best;
        }
    return dp;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // スライム {10, 20, 30, 40}: (10+20)=30、(30+30)=60、(60+40)=100 → 30 + 60 + 100 = 190
    //   他の順番 (10+20), (30+40), (30+70) → 30 + 70 + 100 = 200 なので 190 が最小
    vector<long long> a = {10, 20, 30, 40};
    vector<long long> pre(a.size() + 1, 0);
    for (size_t i = 0; i < a.size(); i++) pre[i + 1] = pre[i] + a[i];
    auto dp = interval_dp<long long>(a.size(), [](int) { return 0LL; }, [&](int l, int, int r) { return pre[r] - pre[l]; });
    assert(dp[0][4] == 190);

    // 行列の積: 10×30, 30×5, 5×60 → (AB)C = 10·30·5 + 10·5·60 = 1500 + 3000 = 4500（A(BC) は 9000 + 18000）
    vector<long long> d = {10, 30, 5, 60};
    auto mc = interval_dp<long long>(3, [](int) { return 0LL; }, [&](int l, int m, int r) { return d[l] * d[m] * d[r]; });
    assert(mc[0][3] == 4500);

    // ランダムテスト1: スライムを「隣り合う 2 匹を選んで合体」の全順番で試したものと比較
    mt19937 rr(130);
    function<long long(vector<long long>)> brute = [&](vector<long long> v) {
        if (v.size() <= 1) return 0LL;
        long long best = LLONG_MAX;
        for (size_t i = 0; i + 1 < v.size(); i++) {
            vector<long long> w(v.begin(), v.begin() + i);
            w.push_back(v[i] + v[i + 1]);
            w.insert(w.end(), v.begin() + i + 2, v.end());
            best = min(best, v[i] + v[i + 1] + brute(w));
        }
        return best;
    };
    for (int it = 0; it < 300; it++) {
        int n = rr() % 6 + 1;
        vector<long long> b(n), p(n + 1, 0);
        for (int i = 0; i < n; i++) b[i] = rr() % 20 + 1, p[i + 1] = p[i] + b[i];
        auto t = interval_dp<long long>(n, [](int) { return 0LL; }, [&](int l, int, int r) { return p[r] - p[l]; });
        assert(t[0][n] == brute(b));
    }
    // ランダムテスト2: ランダムな leaf・cost で、すべての区間を再帰（メモなし）で計算したものと比較（最小・最大）
    for (int it = 0; it < 300; it++) {
        int n = rr() % 7 + 1;
        vector<long long> lf(n);
        for (auto& x : lf) x = (long long)(rr() % 21) - 10;
        vector<vector<vector<long long>>> C(n + 1, vector<vector<long long>>(n + 1, vector<long long>(n + 1)));
        for (auto& x : C) for (auto& y : x) for (auto& z : y) z = (long long)(rr() % 21) - 10;
        for (bool mn : {true, false}) {
            function<long long(int, int)> rec = [&](int l, int r) {
                if (r - l == 1) return lf[l];
                long long best = mn ? LLONG_MAX : LLONG_MIN;
                for (int m = l + 1; m < r; m++) {
                    long long v = rec(l, m) + rec(m, r) + C[l][m][r];
                    best = mn ? min(best, v) : max(best, v);
                }
                return best;
            };
            auto t = interval_dp<long long>(n, [&](int i) { return lf[i]; }, [&](int l, int m, int r) { return C[l][m][r]; }, mn);
            for (int l = 0; l < n; l++) for (int r = l + 1; r <= n; r++) assert(t[l][r] == rec(l, r));
        }
    }
    puts("OK");
}
