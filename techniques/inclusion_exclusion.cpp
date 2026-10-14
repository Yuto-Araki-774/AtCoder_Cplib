// =====================================================================
// techniques/inclusion_exclusion.cpp  包除原理
//   「条件のどれか 1 つ以上を満たすものの個数」を、「いくつかの条件を全部満たすものの個数」の足し引きで求める
//     |A ∪ B ∪ C| = |A| + |B| + |C| - |A∩B| - |A∩C| - |B∩C| + |A∩B∩C|
//   選ぶ条件の集合を bit で全部回す。条件が m 個なら 2^m 通り（m <= 20 程度）
//   「どれも満たさない個数」= 全体 - 「どれか 1 つ以上を満たす個数」
//   本体A: 汎用（「この条件たちを全部満たす個数」を自分で書く）
//   本体B: 1..N のうち、a_0..a_{m-1} のどれかで割り切れる数の個数
//   A と B は互いに独立
// 依存: なし
// ABC / ARC での使用例:
//   ABC131 C - Anti-Division  https://atcoder.jp/contests/abc131/tasks/abc131_c
//     → (B − A + 1) − (count_divisible_any(B, {C, D}) − count_divisible_any(A − 1, {C, D}))
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 汎用 ----------
// m 個の条件。【自分で定義するもの】f(mask) : mask に含まれる条件を「全部」満たすものの個数（ラムダ）
// 返り値: 条件のどれか 1 つ以上を満たすものの個数
template <class T, class F>
T union_count(int m, F f) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/inclusion_exclusion.cpp
    T res = 0;
    for (int mask = 1; mask < (1 << m); mask++) {
        T v = f(mask);
        if (__builtin_popcount(mask) & 1) res += v;  // ★ 奇数個の条件の共通部分は足す
        else res -= v;                               // ★ 偶数個は引く
    }
    return res;
}

// ---------- 本体B: どれかで割り切れる数の個数 ----------
// 返り値: 1 以上 N 以下で、a のどれか 1 つ以上で割り切れる数の個数（a の要素は 1 以上）
// 「L 以上 R 以下」なら count_divisible_any(R, a) - count_divisible_any(L - 1, a)
long long count_divisible_any(long long N, const vector<long long>& a) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/inclusion_exclusion.cpp
    int m = a.size();
    long long res = 0;
    for (int mask = 1; mask < (1 << m); mask++) {
        __int128 l = 1;  // 選んだ数の最小公倍数（N を超えたら打ち切り。あふれ防止）
        for (int i = 0; i < m && l <= N; i++)
            if (mask >> i & 1) l = l / std::gcd((long long)l, a[i]) * a[i];
        if (l > N) continue;
        long long cnt = N / (long long)l;  // ★ 選んだ数すべてで割り切れる = 最小公倍数で割り切れる
        res += (__builtin_popcount(mask) & 1) ? cnt : -cnt;
    }
    return res;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 1..30 で 2, 3, 5 のどれかで割り切れる: 15 + 10 + 6 - 5 - 3 - 2 + 1 = 22
    assert(count_divisible_any(30, {2, 3, 5}) == 22);
    // 4 と 6 は最小公倍数 12（積 24 ではない）: 1..24 で 6 + 4 - 2 = 8
    assert(count_divisible_any(24, {4, 6}) == 8);
    // 大きな数: 1e18 自身と 999999999999999989 自身の 2 個。2 つの最小公倍数は N を超えるので打ち切られる
    assert(count_divisible_any(1000000000000000000LL, {1000000000000000000LL, 999999999999999989LL}) == 2);

    // 汎用版で同じもの: f(mask) = N / lcm(選んだ数)
    vector<long long> a = {2, 3, 5};
    long long N = 30;
    auto f = [&](int mask) {
        long long l = 1;
        for (int i = 0; i < 3; i++) if (mask >> i & 1) l = lcm(l, a[i]);
        return N / l;
    };
    assert(union_count<long long>(3, f) == 22);

    // ランダムテスト: 1..N を全部調べたものと比較
    mt19937 rr(136);
    for (int it = 0; it < 3000; it++) {
        int m = rr() % 6;
        vector<long long> b(m);
        for (auto& x : b) x = rr() % 12 + 1;
        long long n = rr() % 500;
        long long ref = 0;
        for (long long x = 1; x <= n; x++) {
            bool any = false;
            for (long long y : b) any |= x % y == 0;
            ref += any;
        }
        assert(count_divisible_any(n, b) == ref);
        auto g = [&](int mask) { long long c = 0; for (long long x = 1; x <= n; x++) { bool all = true; for (int i = 0; i < m; i++) if (mask >> i & 1) all &= x % b[i] == 0; c += all; } return c; };
        assert(union_count<long long>(m, g) == ref);
    }
    puts("OK");
}
