// =====================================================================
// dp/alien_dp.cpp  Alien DP（ラグランジュ緩和。「ちょうど K 個」の制約を外す）
//   「ちょうど K 個選ぶ（K 個に分ける）ときの最小コスト g(K)」を求めたいが、K を DP の状態に入れると遅い
//   → 1 個選ぶごとにペナルティ λ を払うことにして、個数の制約なしの最小値 h(λ) = min_k (g(k) + λ k) を DP で求める
//   → g(K) = max_λ (h(λ) − λ K)。λ について二分探索するので、DP を O(log(λ の範囲)) 回するだけ
//   ★ 条件: g(k) が k について下に凸（g(k+1) − g(k) が増えていく）。最大化なら上に凸（−1 倍して最小化に直す）
//   凸かどうかは、小さいケースで g(k) を全探索して差分を見て確かめるのが安全
// 依存: なし
// ABC / ARC での使用例:
//   ABC218 H - Red and Blue Lamps  https://atcoder.jp/contests/abc218/tasks/abc218_h
//     → 赤をちょうど R 個（R > N/2 なら色を入れ替える）。赤 1 個ごとにペナルティを引く「個数自由」の DP は O(N) の 2 状態 DP。最大化なので −1 倍して alien_dp
//   ABC305 Ex - Shojin  https://atcoder.jp/contests/abc305/tasks/abc305_h
//     → （難しい）日数 K ごとの最小コストが K について凸。最小の日数 D を探したあと、ちょうど D 日の最小コストを Alien DP で
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// 【自分で定義するもの】h(λ) : 「1 個ごとに λ を足す」として、個数自由で最小にしたときの値（DP を書く）（ラムダ）
// T ans = alien_dp<T>(K, lo, hi, h);   g(K)（ちょうど K 個のときの最小値）
//   λ は [lo, hi] の整数で探す。g の傾き g(k+1) − g(k) の符号を変えたもの（−傾き）が、K の前後で入る範囲にすること
//   迷ったら lo = −(コストの最大の変化量)、hi = +(同じ) のように広めに。g が整数なら λ も整数で十分
//   h(λ) の中では λ × 個数 が足されるので、オーバーフローに注意（λ の範囲 × 最大の個数 が T に収まること）
template <class T, class F>
T alien_dp(long long K, T lo, T hi, F h) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/alien_dp.cpp
    auto phi = [&](T lam) { return h(lam) - lam * (T)K; };  // ★ φ(λ) = h(λ) − λK は λ について上に凸 → 山の頂上を二分探索
    while (hi - lo > 1) {
        T mid = lo + (hi - lo) / 2;
        if (phi(mid) <= phi(mid + 1)) lo = mid;  // ★ まだ上り坂（右の方が大きい）→ 頂上は右
        else hi = mid;
    }
    return max(phi(lo), phi(hi));
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 例 1: 数列をちょうど K 個の区間に分けて、Σ (区間の和)^2 を最小にする（区間の和の 2 乗は Monge → g は凸）
    //   a = {1, 2, 3, 4}、K = 2 → {1, 2, 3} {4}: 36 + 16 = 52、{1, 2} {3, 4}: 9 + 49 = 58、{1} {2, 3, 4}: 1 + 81 = 82 → 52
    //   h(λ): dp[i] = min_j dp[j] + (s[i] − s[j])^2 + λ（区間を 1 個作るごとに λ）O(N^2)
    {
        vector<long long> a = {1, 2, 3, 4};
        int n = a.size();
        vector<long long> s(n + 1, 0);
        for (int i = 0; i < n; i++) s[i + 1] = s[i] + a[i];
        auto h = [&](long long lam) {
            vector<long long> dp(n + 1, LLONG_MAX / 4);
            dp[0] = 0;
            for (int i = 1; i <= n; i++)
                for (int j = 0; j < i; j++) dp[i] = min(dp[i], dp[j] + (s[i] - s[j]) * (s[i] - s[j]) + lam);
            return dp[n];
        };
        long long B = s[n] * s[n];  // 区間を 1 個増やしたときのコストの変化はこれ以下
        assert(alien_dp<long long>(2, -B, B, h) == 52);
        assert(alien_dp<long long>(1, -B, B, h) == 100 && alien_dp<long long>(4, -B, B, h) == 30);  // 1 個: 10^2、4 個: 1+4+9+16
    }
    // ランダムテスト
    {
        mt19937 rr(184);
        for (int it = 0; it < 500; it++) {
            // (1) 区間分割 Σ (区間の和)^2 を、K を状態に入れた O(N^2 K) の DP と比較
            int n = rr() % 8 + 1;
            vector<long long> a(n), s(n + 1, 0);
            for (auto& v : a) v = rr() % 10 + 1;
            for (int i = 0; i < n; i++) s[i + 1] = s[i] + a[i];
            vector<vector<long long>> G(n + 1, vector<long long>(n + 1, LLONG_MAX / 4));  // G[k][i]
            G[0][0] = 0;
            for (int k = 1; k <= n; k++)
                for (int i = 1; i <= n; i++)
                    for (int j = 0; j < i; j++) G[k][i] = min(G[k][i], G[k - 1][j] + (s[i] - s[j]) * (s[i] - s[j]));
            auto h = [&](long long lam) {
                vector<long long> dp(n + 1, LLONG_MAX / 4);
                dp[0] = 0;
                for (int i = 1; i <= n; i++)
                    for (int j = 0; j < i; j++) dp[i] = min(dp[i], dp[j] + (s[i] - s[j]) * (s[i] - s[j]) + lam);
                return dp[n];
            };
            long long B = s[n] * s[n];
            for (int K = 1; K <= n; K++) assert(alien_dp<long long>(K, -B, B, h) == G[K][n]);
            // (2) 最大化: 隣り合わない K 個を選んで和を最大（g(k) は上に凸）→ −1 倍して最小化
            int m = rr() % 9 + 1;
            vector<long long> b(m);
            for (auto& v : b) v = (long long)(rr() % 21) - 5;
            int maxk = (m + 1) / 2;
            vector<long long> best(maxk + 1, LLONG_MIN);  // 全探索
            for (int mask = 0; mask < (1 << m); mask++) {
                if (mask & (mask >> 1)) continue;
                int c = __builtin_popcount(mask);
                long long t = 0;
                for (int i = 0; i < m; i++) if (mask >> i & 1) t += b[i];
                best[c] = max(best[c], t);
            }
            auto h2 = [&](long long lam) {  // 最小化に直した −(和) + λ × 個数 の最小値（隣り合わない、個数自由）
                long long take = LLONG_MAX / 4, skip = 0;  // take: 直前を選んだ、skip: 選んでいない
                for (int i = 0; i < m; i++) {
                    long long nt = skip - b[i] + lam, ns = min(take, skip);
                    take = nt, skip = ns;
                }
                return min(take, skip);
            };
            for (int K = 0; K <= maxk; K++) assert(-alien_dp<long long>(K, -1000, 1000, h2) == best[K]);
        }
    }
    puts("OK");
}
