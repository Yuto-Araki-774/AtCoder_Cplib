// =====================================================================
// math/divisor_transform.cpp  メビウス関数・約数 / 倍数のゼータ変換・メビウス変換
//   「gcd がちょうど d になる組の数」を「gcd が d の倍数になる組の数（数えやすい）」から引き算で求める道具
//   本体A（メビウス関数）: mu(n) = 0（平方数で割り切れる）/ (-1)^(素因数の個数)
//   本体B（ゼータ / メビウス変換）: 約数の和・倍数の和と、その逆。O(N log log N)
//   A・B は互いに独立
// 依存: なし
// ABC / ARC での使用例:
//   ABC162 E - Sum of gcd of Tuples (Hard)  https://atcoder.jp/contests/abc162/tasks/abc162_e
//     → f[d] = (K / d)^N（全部 d の倍数）→ multiple_mobius → Σ d × f[d]。mod は 1e9+7 なので modint1000000007
//   ABC206 E - Divide Both  https://atcoder.jp/contests/abc206/tasks/abc206_e
//     → f[g] = (L 以上 R 以下の g の倍数の個数)^2 → multiple_mobius で gcd がちょうど g の組。そこから x や y が g そのものの組を引く
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/modint>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: メビウス関数 ----------
// mobius_table(N) : mu[0..N]（mu[0] = 0）。線形篩で O(N)
// mobius(n)       : 1 個だけ O(√n)
vector<int> mobius_table(int N) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/divisor_transform.cpp
    vector<int> mu(N + 1, 0), primes;
    vector<bool> comp(N + 1, false);
    if (N >= 1) mu[1] = 1;
    for (int i = 2; i <= N; i++) {
        if (!comp[i]) primes.push_back(i), mu[i] = -1;
        for (int p : primes) {
            if ((long long)i * p > N) break;
            comp[i * p] = true;
            if (i % p == 0) { mu[i * p] = 0; break; }  // ★ p^2 で割り切れる → 0
            mu[i * p] = -mu[i];                          // ★ 素因数が 1 つ増える → 符号が反転
        }
    }
    return mu;
}
int mobius(long long n) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/divisor_transform.cpp
    int res = 1;
    for (long long p = 2; p * p <= n; p++) {
        if (n % p) continue;
        n /= p;
        if (n % p == 0) return 0;
        res = -res;
    }
    if (n > 1) res = -res;
    return res;
}

// ---------- 本体B: 約数・倍数のゼータ変換 / メビウス変換 O(N log log N) ----------
// a は a[0..N] の vector（a[0] は使わない）。T は long long か mint。a をその場で書き換える
//   divisor_zeta(a)    : a[n] ← Σ_{d | n} a[d]          （n の約数での和）
//   divisor_mobius(a)  : divisor_zeta の逆
//   multiple_zeta(a)   : a[d] ← Σ_{d | n, n <= N} a[n]  （d の倍数での和）
//   multiple_mobius(a) : multiple_zeta の逆
// ★ よく使う形: f[d] = 「全部 d の倍数」になる組の数（数えやすい）→ multiple_mobius(f) → f[d] = 「gcd がちょうど d」の組の数
vector<int> dt_primes(int N) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/divisor_transform.cpp
    vector<bool> comp(N + 1, false);
    vector<int> ps;
    for (int i = 2; i <= N; i++) {
        if (comp[i]) continue;
        ps.push_back(i);
        for (long long j = (long long)i * i; j <= N; j += i) comp[j] = true;
    }
    return ps;
}
template <class T>
void divisor_zeta(vector<T>& a) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/divisor_transform.cpp
    int N = (int)a.size() - 1;
    for (int p : dt_primes(N))
        for (int i = 1; (long long)i * p <= N; i++) a[i * p] += a[i];  // ★ 素数 p ごとに「p の指数方向の累積和」
}
template <class T>
void divisor_mobius(vector<T>& a) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/divisor_transform.cpp
    int N = (int)a.size() - 1;
    for (int p : dt_primes(N))
        for (int i = N / p; i >= 1; i--) a[i * p] -= a[i];  // ★ 累積和の逆 = 差分（大きい方から）
}
template <class T>
void multiple_zeta(vector<T>& a) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/divisor_transform.cpp
    int N = (int)a.size() - 1;
    for (int p : dt_primes(N))
        for (int i = N / p; i >= 1; i--) a[i] += a[i * p];  // ★ 倍数の方から足していく（後ろからの累積和）
}
template <class T>
void multiple_mobius(vector<T>& a) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/divisor_transform.cpp
    int N = (int)a.size() - 1;
    for (int p : dt_primes(N))
        for (int i = 1; (long long)i * p <= N; i++) a[i] -= a[i * p];  // ★ 後ろからの累積和の逆（小さい方から）
}

// ---------- 削除可: gcd 畳み込み ----------
// c[k] = Σ_{gcd(i, j) = k} a[i] b[j]（1 <= i, j <= N）。a, b は同じ長さ N + 1
template <class T>
vector<T> gcd_convolution(vector<T> a, vector<T> b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/divisor_transform.cpp
    multiple_zeta(a), multiple_zeta(b);  // ★ 「両方 k の倍数」の和は掛け算で求まる
    for (int i = 0; i < (int)a.size(); i++) a[i] *= b[i];
    multiple_mobius(a);  // ★ 「gcd がちょうど k」に戻す
    return a;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
using mint = atcoder::modint998244353;
int main() {
    // メビウス関数: mu(1..12) = 1, -1, -1, 0, -1, 1, -1, 0, 0, 1, -1, 0
    {
        auto mu = mobius_table(12);
        assert((mu == vector<int>{0, 1, -1, -1, 0, -1, 1, -1, 0, 0, 1, -1, 0}));
        for (int n = 1; n <= 12; n++) assert(mobius(n) == mu[n]);
    }
    // 使用例: 1..K の数を N 個並べる（K^N 通り）ときの gcd の和（ABC162 E と同じ形）
    //   f[d] = 全部 d の倍数 = (K / d)^N 通り → multiple_mobius → f[d] = gcd がちょうど d の通り数
    //   N = 3, K = 2: 並べ方 8 通りのうち gcd = 2 は (2,2,2) の 1 通りだけ → 和 = 7 × 1 + 1 × 2 = 9
    {
        int N = 3, K = 2;
        vector<long long> f(K + 1, 0);
        for (int d = 1; d <= K; d++) { f[d] = 1; for (int i = 0; i < N; i++) f[d] *= K / d; }
        multiple_mobius(f);
        long long s = 0;
        for (int d = 1; d <= K; d++) s += d * f[d];
        assert(f[1] == 7 && f[2] == 1 && s == 9);
    }
    // ランダムテスト: 4 つの変換を定義どおりの O(N^2) と比較し、逆変換で戻るかも確認
    {
        mt19937 rr(150);
        for (int it = 0; it < 300; it++) {
            int N = rr() % 60 + 1;
            vector<long long> a(N + 1, 0);
            for (int i = 1; i <= N; i++) a[i] = (long long)(rr() % 21) - 10;
            vector<long long> dz(N + 1, 0), mz(N + 1, 0);
            for (int n = 1; n <= N; n++)
                for (int d = 1; d <= N; d++) {
                    if (n % d == 0) dz[n] += a[d];
                    if (d % n == 0) mz[n] += a[d];
                }
            auto b = a; divisor_zeta(b); assert(b == dz);
            divisor_mobius(b); assert(b == a);
            auto c = a; multiple_zeta(c); assert(c == mz);
            multiple_mobius(c); assert(c == a);
            // メビウス関数で書いた逆変換の式 a[n] = Σ_{d | n} mu(n / d) dz[d] とも一致するか
            auto mu = mobius_table(N);
            for (int n = 1; n <= N; n++) {
                long long t = 0;
                for (int d = 1; d <= N; d++) if (n % d == 0) t += mu[n / d] * dz[d];
                assert(t == a[n] && mobius(n) == mu[n]);
            }
            // gcd 畳み込み
            vector<long long> x(N + 1, 0), y(N + 1, 0), want(N + 1, 0);
            for (int i = 1; i <= N; i++) x[i] = rr() % 7, y[i] = rr() % 7;
            for (int i = 1; i <= N; i++) for (int j = 1; j <= N; j++) want[gcd(i, j)] += x[i] * y[j];
            want[0] = 0;
            auto got = gcd_convolution(x, y);
            got[0] = 0;
            assert(got == want);
        }
        // mobius(n) の大きい値: 30 = 2·3·5 → -1、1e12 = 2^12·5^12 → 0、999999000001 は素数 → -1
        assert(mobius(30) == -1 && mobius(1000000000000LL) == 0 && mobius(999999000001LL) == -1);
        // mint でも使える
        vector<mint> m(11, 0);
        for (int i = 1; i <= 10; i++) m[i] = i;
        auto m2 = m;
        divisor_zeta(m2), divisor_mobius(m2);
        for (int i = 1; i <= 10; i++) assert(m2[i] == m[i]);
    }
    puts("OK");
}
