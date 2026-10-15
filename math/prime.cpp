// =====================================================================
// math/prime.cpp  素数判定・素因数分解・約数列挙・区間篩・オイラーの φ 関数・1e18 までの素因数分解
//   本体A（篩）: N 以下の数をたくさん処理するとき（N <= 1e7）
//   本体B（単発）: 1 個の大きい数（~1e12）を O(√x) で処理するとき
//   本体C（区間篩）: [L, R) の素数判定。L, R は 1e12 程度まで、R - L は 1e6 程度まで
//   本体D（φ 関数）: 1 以上 n 以下で n と互いに素な数の個数
//   本体E（ミラー・ラビン + ポラード・ロー）: 1e18 までの素数判定・素因数分解を高速に（√x では間に合わないとき）
//   A・B・C・D・E は互いに独立
// 依存: なし
// ABC / ARC での使用例:
//   ABC142 D - Disjoint Set of Common Divisors  https://atcoder.jp/contests/abc142/tasks/abc142_d
//     → gcd(A, B) の素因数の個数 + 1（factorize）
//   ABC177 E - Coprime  https://atcoder.jp/contests/abc177/tasks/abc177_e
//     → 篩の最小素因数で全要素を素因数分解（Sieve）
//   AOJ NTL_1_D - オイラーのφ関数  https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=NTL_1_D&lang=ja
//     → n ≤ 10^9 の φ(n)。euler_phi そのもの（ABC ではないが練習用の定番）
//   （区間篩は ABC での出題を確認できていないので例なし）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 線形篩（最小素因数テーブル） 構築 O(N)、分解 O(log x) ----------
// Sieve sv(N);  sv.primes（N 以下の素数）、sv.is_prime(x)、sv.factorize(x)、sv.divisors(x)
// ◆ struct Sieve : 線形篩（N 以下の素数と、各数の最小素因数の表）
//   Sieve(N)        : 前計算 O(N)。メモリ 4N バイト（N は 1e7 くらいまで）
//   primes          : N 以下の素数の一覧（昇順）
//   is_prime(x)     : x が素数か（0 <= x <= N）
//   factorize(x)    : 素因数分解 {{素数, 指数}, ...}（1 <= x <= N）。O(log x)
//   divisors(x)     : 約数の一覧（昇順。1 <= x <= N）
struct Sieve {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp
    int n;
    vector<int> spf, primes;  // spf[x] = x の最小素因数
    // n_ : 調べる数の上限 N
    Sieve(int n_) : n(n_), spf(n_ + 1, 0) {
        for (int i = 2; i <= n; i++) {
            if (spf[i] == 0) spf[i] = i, primes.push_back(i);  // ★ 誰にも割られていない = 素数
            for (int p : primes) {
                if (p > spf[i] || (long long)i * p > n) break;
                spf[i * p] = p;  // ★ i*p の最小素因数は p（各数を 1 回だけ塗る）
            }
        }
    }
    // is_prime(x) : x : 0..N
    bool is_prime(int x) const { return x >= 2 && spf[x] == x; }
    // factorize(x) : x : 1..N（1 なら空）
    vector<pair<int, int>> factorize(int x) const {  // {{素数, 指数}, ...}
        vector<pair<int, int>> res;
        while (x > 1) {
            int p = spf[x], e = 0;  // ★ 最小素因数で割れるだけ割る
            while (x % p == 0) x /= p, e++;
            res.push_back({p, e});
        }
        return res;
    }
    // --- 削除可: 約数列挙（昇順） ---
    // divisors(x) : x : 1..N
    vector<int> divisors(int x) const {
        vector<int> res = {1};
        for (auto [p, e] : factorize(x)) {
            int sz = res.size(), pk = 1;
            for (int k = 1; k <= e; k++) {
                pk *= p;
                for (int i = 0; i < sz; i++) res.push_back(res[i] * pk);  // ★ 既存の約数 × p^k
            }
        }
        sort(res.begin(), res.end());
        return res;
    }
    // --- 削除可ここまで ---
};

// ---------- 本体B: 1 個の数に対する O(√x) の関数 ----------
// ◆ is_prime(x) : x が素数か。O(√x)（x は 1e12 くらいまで。もっと大きいなら is_prime_mr）
//   x : 判定する数（2 未満なら false）
bool is_prime(long long x) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp
    if (x < 2) return false;
    for (long long i = 2; i * i <= x; i++)  // ★ √x まで調べれば十分
        if (x % i == 0) return false;
    return true;
}
// ◆ factorize(x) : 素因数分解 {{素数, 指数}, ...}（素数の昇順）。O(√x)
//   x : 1 以上（1 なら空）。1e12 くらいまで（もっと大きいなら factorize_fast）
vector<pair<long long, int>> factorize(long long x) {  // {{素数, 指数}, ...}
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp
    vector<pair<long long, int>> res;
    for (long long p = 2; p * p <= x; p++) {
        if (x % p) continue;
        int e = 0;
        while (x % p == 0) x /= p, e++;
        res.push_back({p, e});
    }
    if (x > 1) res.push_back({x, 1});  // ★ 最後に残ったものは √x より大きい素数
    return res;
}
// ◆ divisors(x) : 約数の一覧（昇順）。O(√x)
//   x : 1 以上
vector<long long> divisors(long long x) {  // 昇順
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp
    vector<long long> small, large;
    for (long long i = 1; i * i <= x; i++) {
        if (x % i) continue;
        small.push_back(i);                         // ★ i が約数なら
        if (i != x / i) large.push_back(x / i);     // ★ x / i も約数
    }
    small.insert(small.end(), large.rbegin(), large.rend());
    return small;
}

// ---------- 本体C: 区間篩 O((R - L) log log R + √R) ----------
// 返り値: res[i] = (L + i が素数か)（i = 0 .. R-L-1）
// 例: auto ok = segment_sieve(L, R); for (long long x = L; x < R; x++) if (ok[x - L]) ...
// ◆ segment_sieve(L, R) : [L, R) の各数が素数か。O((R − L) log log R + √R)
//   L, R : 0 <= L <= R。R は 1e12 くらいまで、R − L は 1e6〜1e7 くらいまで（その大きさの配列を作る）
//   返り値 : res[i] = L + i が素数か
vector<bool> segment_sieve(long long L, long long R) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp
    long long s = 1;
    while ((s + 1) * (s + 1) < R) s++;  // s = ceil(√R) 程度
    vector<bool> small(s + 1, true), res(max(0LL, R - L), true);
    for (long long x = L; x < min(R, 2LL); x++) res[x - L] = false;  // 0, 1 は素数でない
    for (long long p = 2; p <= s; p++) {
        if (!small[p]) continue;
        for (long long q = p * p; q <= s; q += p) small[q] = false;  // ★ √R までの素数を普通の篩で求める
        for (long long q = max(p * p, (L + p - 1) / p * p); q < R; q += p) res[q - L] = false;  // ★ その倍数を [L, R) で消す
    }
    return res;
}

// ---------- 本体D: オイラーの φ 関数 ----------
// euler_phi(n)   : φ(n) を O(√n)
// phi_table(N)   : φ(0..N) を全部 O(N log log N)
// φ(n) = n × Π (1 - 1/p)（p は n の素因数）。φ(1) = 1、φ(p) = p - 1
// ◆ euler_phi(n) : φ(n) = 1..n で n と互いに素な数の個数。O(√n)
//   n : 1 以上
long long euler_phi(long long n) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp
    long long res = n;
    for (long long p = 2; p * p <= n; p++) {
        if (n % p) continue;
        res -= res / p;  // ★ 素因数 p ごとに × (1 - 1/p)
        while (n % p == 0) n /= p;
    }
    if (n > 1) res -= res / n;
    return res;
}
// ◆ phi_table(N) : φ(0..N) の表（φ(0) = 0）。O(N log log N)
//   N : 上限（1e7 くらいまで）
vector<int> phi_table(int N) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp
    vector<int> phi(N + 1);
    iota(phi.begin(), phi.end(), 0);
    for (int p = 2; p <= N; p++)
        if (phi[p] == p)  // ★ まだ変わっていない = 素数
            for (int q = p; q <= N; q += p) phi[q] -= phi[q] / p;
    return phi;
}

// ---------- 本体E: ミラー・ラビン素数判定・ポラード・ロー素因数分解（x < 2^63） ----------
// is_prime_mr(x)      x が素数か。決まった 7 個の底で確かめるので、long long の範囲では必ず正しい  O(log x)
// factorize_fast(x)   {{素数, 指数}, ...}（素数の昇順）  だいたい O(x^(1/4))。1e18 でも一瞬
// 本体B の is_prime / factorize（O(√x)）は x が 1e12 くらいまで。それより大きいときはこちら
// （内部用）a × b mod m を 128 bit で
unsigned long long mr_mul(unsigned long long a, unsigned long long b, unsigned long long m) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp
    return (unsigned long long)((__uint128_t)a * b % m);
}
// （内部用）a^e mod m
unsigned long long mr_pow(unsigned long long a, unsigned long long e, unsigned long long m) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp
    unsigned long long r = 1 % m;
    a %= m;
    for (; e; e >>= 1, a = mr_mul(a, a, m)) if (e & 1) r = mr_mul(r, a, m);
    return r;
}
// ◆ is_prime_mr(x) : x が素数か（ミラー・ラビン。long long の範囲で必ず正しい）。O(log x)
//   x : 判定する数（2 未満なら false）
bool is_prime_mr(long long x) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp
    if (x < 2) return false;
    for (long long p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) {
        if (x % p == 0) return x == p;
    }
    unsigned long long n = x, d = n - 1;
    int s = 0;
    while (d % 2 == 0) d /= 2, s++;  // n - 1 = d × 2^s（d は奇数）
    for (unsigned long long a : {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL, 9780504ULL, 1795265022ULL}) {  // ★ この 7 個で 2^64 未満は確定する
        if (a % n == 0) continue;
        unsigned long long y = mr_pow(a, d, n);
        if (y == 1 || y == n - 1) continue;
        bool ok = false;
        for (int i = 1; i < s && !ok; i++) {
            y = mr_mul(y, y, n);
            if (y == n - 1) ok = true;  // ★ 2 乗していって -1 が出れば、この底では「素数っぽい」
        }
        if (!ok) return false;  // ★ 素数なら必ず出るはずの -1 が出ない → 合成数
    }
    return true;
}
// ◆ pollard_rho(x) : 合成数 x の、1 と x 以外の約数を 1 つ返す（素数とは限らない）
//   x : 合成数であること（素数を渡すと終わらない）。普通は factorize_fast から使う
long long pollard_rho(long long x) {  // x は合成数。1 より大きく x 未満の約数を 1 つ返す
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp
    if (x % 2 == 0) return 2;
    unsigned long long n = x;
    static mt19937_64 rng(20241014);
    while (true) {
        unsigned long long c = rng() % (n - 1) + 1, y = rng() % n, ys = 0, xx = 0, q = 1, g = 1;
        auto f = [&](unsigned long long v) { return (mr_mul(v, v, n) + c) % n; };  // ★ 乱数のような列 v → v^2 + c
        const unsigned long long M = 128;
        for (unsigned long long r = 1; g == 1; r <<= 1) {  // ★ 列がいつか周期に入る（mod p で先に）ことを使い、差の gcd で p を見つける
            xx = y;
            for (unsigned long long i = 0; i < r; i++) y = f(y);
            for (unsigned long long k = 0; k < r && g == 1; k += M) {
                ys = y;
                for (unsigned long long i = 0; i < M && i < r - k; i++) y = f(y), q = mr_mul(q, xx > y ? xx - y : y - xx, n);
                g = std::gcd(q, n);  // 差をまとめて掛けてから gcd を 1 回（速くするため）
            }
        }
        if (g == n) {  // まとめすぎて n になったら、1 つずつやり直す
            do ys = f(ys), g = std::gcd(xx > ys ? xx - ys : ys - xx, n);
            while (g == 1);
        }
        if (g != n) return (long long)g;  // それでも n なら c を変えてやり直し
    }
}
// ◆ factorize_fast(x) : 素因数分解 {{素数, 指数}, ...}（素数の昇順）。1e18 でも一瞬
//   x : 1 以上（1 なら空）
vector<pair<long long, int>> factorize_fast(long long x) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp
    vector<long long> ps, st;
    for (long long p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37})  // 小さい素数は先に割っておく
        while (x % p == 0) ps.push_back(p), x /= p;
    if (x > 1) st.push_back(x);
    while (!st.empty()) {
        long long v = st.back();
        st.pop_back();
        if (is_prime_mr(v)) { ps.push_back(v); continue; }
        long long d = pollard_rho(v);  // ★ 約数を 1 つ見つけて 2 つに分ける
        st.push_back(d), st.push_back(v / d);
    }
    sort(ps.begin(), ps.end());
    vector<pair<long long, int>> res;
    for (long long p : ps) {
        if (!res.empty() && res.back().first == p) res.back().second++;
        else res.push_back({p, 1});
    }
    return res;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    Sieve sv(100);
    assert(sv.primes.size() == 25 && sv.is_prime(97) && !sv.is_prime(91));
    assert((sv.factorize(12) == vector<pair<int, int>>{{2, 2}, {3, 1}}));
    assert((sv.divisors(12) == vector<int>{1, 2, 3, 4, 6, 12}));
    assert(is_prime(1000000007) && !is_prime(1));
    assert((factorize(2000000014LL) == vector<pair<long long, int>>{{2, 1}, {1000000007, 1}}));
    assert((divisors(36) == vector<long long>{1, 2, 3, 4, 6, 9, 12, 18, 36}));

    // 区間篩: 愚直な is_prime と比較（小さい区間、大きい区間）
    for (auto [L, R] : vector<pair<long long, long long>>{{0, 200}, {1, 2}, {97, 98}, {1000000000000LL, 1000000001000LL}, {5, 5}}) {
        auto ok = segment_sieve(L, R);
        assert((long long)ok.size() == R - L);
        for (long long x = L; x < R; x++) assert(ok[x - L] == is_prime(x));
    }
    // φ 関数: gcd で数えた値と比較
    auto ph = phi_table(300);
    for (int n = 1; n <= 300; n++) {
        int c = 0;
        for (int k = 1; k <= n; k++) c += gcd(k, n) == 1;
        assert(ph[n] == c && euler_phi(n) == c);
    }
    assert(euler_phi(1000000007) == 1000000006 && euler_phi(1000000000000LL) == 400000000000LL);
    // ランダムテスト: 篩と単発版の一致、素因数分解を掛け戻すと元の数か、約数を全部割って調べたものと一致するか
    {
        mt19937_64 rr(118);
        Sieve s2(5000);
        for (int x = 0; x <= 5000; x++) {
            assert(s2.is_prime(x) == is_prime(x));
            if (x < 2) continue;
            long long prod = 1;
            int prevp = 0;
            for (auto [p, e] : s2.factorize(x)) {
                assert(is_prime(p) && p > prevp && e > 0);
                prevp = p;
                for (int i = 0; i < e; i++) prod *= p;
            }
            assert(prod == x);
            vector<int> dv;
            for (int d = 1; d <= x; d++) if (x % d == 0) dv.push_back(d);
            assert(s2.divisors(x) == dv);
        }
        for (int it = 0; it < 300; it++) {
            long long x = rr() % 10000000000LL + 1;
            long long prod = 1, prevp = 0;
            for (auto [p, e] : factorize(x)) {
                assert(is_prime(p) && p > prevp);
                prevp = p;
                for (int i = 0; i < e; i++) prod *= p;
            }
            assert(prod == x);
            long long y = rr() % 100000 + 1;
            vector<long long> dv;
            for (long long t = 1; t <= y; t++) if (y % t == 0) dv.push_back(t);
            assert(divisors(y) == dv);
        }
    }
    // ミラー・ラビン・ポラード・ロー
    {
        assert(is_prime_mr(2) && is_prime_mr(999999999999999989LL) && is_prime_mr(2305843009213693951LL));  // 2^61 - 1
        assert(!is_prime_mr(1) && !is_prime_mr(561) && !is_prime_mr(3825123056546413051LL));  // カーマイケル数、強擬素数
        assert((factorize_fast(3825123056546413051LL) == vector<pair<long long, int>>{{149491, 1}, {747451, 1}, {34233211, 1}}));
        assert((factorize_fast(1000000000000000000LL) == vector<pair<long long, int>>{{2, 18}, {5, 18}}));
        assert(factorize_fast(1).empty());
        Sieve big(1000000);
        for (int v = 0; v <= 1000000; v++) assert(is_prime_mr(v) == big.is_prime(v));
        mt19937_64 r3(141);
        for (int it = 0; it < 300; it++) {  // 1e12 までは O(√x) 版と完全に一致するか
            long long v = r3() % 1000000000000LL + 1;
            assert(is_prime_mr(v) == is_prime(v) && factorize_fast(v) == factorize(v));
        }
        for (int it = 0; it < 300; it++) {  // 1e18 近く: 掛け戻すと元に戻り、出てくる数が全部素数か
            long long v = (long long)(r3() % 1000000000000000000LL) + 1;
            if (it % 3 == 0) {  // 大きい素数 2 つの積（ポラード・ローが一番がんばる形）も混ぜる
                long long p = r3() % 1000000000 + 1, q = r3() % 1000000000 + 1;
                while (!is_prime_mr(p)) p++;
                while (!is_prime_mr(q)) q++;
                v = p * q;
            }
            __int128 prod = 1;
            long long prevp = 0;
            for (auto [p, e] : factorize_fast(v)) {
                assert(is_prime_mr(p) && p > prevp);
                prevp = p;
                for (int i = 0; i < e; i++) prod *= p;
            }
            assert(prod == v);
        }
    }
    puts("OK");
}
