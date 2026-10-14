// =====================================================================
// basic/ternary_search.cpp  三分探索（凸関数の最小値を探す）
//   f が「下に凸」（谷が 1 つ。減って、増える）のとき、f が最小になる x を求める
//   最大値を探すとき（山が 1 つ）は、f の代わりに -f を渡す
//   本体A: 整数版（凸関数の差が単調になることを使った二分探索。平らな区間があっても正しい）
//   本体B: 実数版（三分探索）
//   本体C: 実数版（黄金分割探索。f の呼び出し回数が少ない。f が重いとき用）
//   どれも互いに独立
// 依存: なし
// ABC / ARC での使用例:
//   ABC279 D - Freefall  https://atcoder.jp/contests/abc279/tasks/abc279_d
//     → 操作回数 x の関数 Bx + A/√(1+x) は凸 → 整数版（argmin_convex_int）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 整数版 O(log(hi - lo)) ----------
// [lo, hi] の整数 x のうち f(x) が最小になるもの（最小が複数あれば一番左）を返す
// f は凸（f(x+1) - f(x) が x について増えていく）であること
// 【自分で定義するもの】f(x)（main の中にラムダで書く）
// 例: long long x = argmin_convex_int(0LL, (long long)1e9, [&](long long x) { return cost(x); });
template <class F>
long long argmin_convex_int(long long lo, long long hi, F f) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/ternary_search.cpp
    // ★ 凸なら「f(x) <= f(x+1)」は x について false...false true...true と変わる → その境目が最小
    long long ng = lo - 1, ok = hi;  // f(hi) <= f(hi+1) は成り立つとみなす（hi より右は見ない）
    while (ok - ng > 1) {
        long long mid = ng + (ok - ng) / 2;
        (f(mid) <= f(mid + 1) ? ok : ng) = mid;  // ★ 右が下がっていなければ、最小はこれより左（か、ここ）
    }
    return ok;
}

// ---------- 本体B: 実数版（三分探索） ----------
// [lo, hi] で f が最小になる x を返す（iter 回で区間は (2/3)^iter に縮む。200 回で十分）
template <class F>
double argmin_convex_real(double lo, double hi, F f, int iter = 200) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/ternary_search.cpp
    for (int i = 0; i < iter; i++) {
        double m1 = (2 * lo + hi) / 3, m2 = (lo + 2 * hi) / 3;  // ★ 区間を 3 等分する 2 点
        if (f(m1) < f(m2)) hi = m2;  // ★ 左の方が小さい → 最小は m2 より右にはない
        else lo = m1;                // ★ そうでなければ m1 より左にはない
    }
    return (lo + hi) / 2;
}

// ---------- 本体C: 実数版（黄金分割探索） ----------
// 本体B と同じ結果を、f の呼び出し 1 回ずつで求める（三分探索は 1 回あたり 2 回）
template <class F>
double golden_section_min(double lo, double hi, F f, int iter = 200) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/ternary_search.cpp
    const double r = (sqrt(5.0) - 1) / 2;  // 0.618...
    double m1 = hi - r * (hi - lo), m2 = lo + r * (hi - lo);
    double f1 = f(m1), f2 = f(m2);
    for (int i = 0; i < iter; i++) {
        if (f1 < f2) hi = m2, m2 = m1, f2 = f1, m1 = hi - r * (hi - lo), f1 = f(m1);  // ★ 古い m1 が新しい m2 になる
        else lo = m1, m1 = m2, f1 = f2, m2 = lo + r * (hi - lo), f2 = f(m2);         // ★ 古い m2 が新しい m1 になる
    }
    return (lo + hi) / 2;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 整数: f(x) = (x - 37)^2 の最小
    assert(argmin_convex_int(-100, 100, [](long long x) { return (x - 37) * (x - 37); }) == 37);
    // 平らな区間がある: f(x) = max(0, |x - 50| - 10) → 40..60 が最小 0、一番左の 40 を返す
    assert(argmin_convex_int(0, 100, [](long long x) { return max(0LL, llabs(x - 50) - 10); }) == 40);
    // 端が最小
    assert(argmin_convex_int(5, 9, [](long long x) { return x; }) == 5);
    assert(argmin_convex_int(5, 9, [](long long x) { return -x; }) == 9);
    // 典型: 「a + b * x 回の操作で…」のような f(x) = A * x + B / (x + 1) の最小（AtCoder でよく出る形）
    long long A = 3, B = 1000;
    auto g = [&](long long x) { return (double)A * x + (double)B / (x + 1); };
    long long best = argmin_convex_int(0, 1000, g);
    for (long long x = 0; x <= 1000; x++) assert(g(best) <= g(x) + 1e-9);

    // 実数
    auto h = [](double x) { return (x - 1.2345) * (x - 1.2345) + 3; };
    assert(abs(argmin_convex_real(-10, 10, h) - 1.2345) < 1e-6);
    assert(abs(golden_section_min(-10, 10, h) - 1.2345) < 1e-6);
    // 最大値は -f で: f(x) = -(x-2)^2 の最大
    assert(abs(argmin_convex_real(0, 5, [](double x) { return (x - 2) * (x - 2); }) - 2) < 1e-6);

    // ランダムテスト: ランダムな凸関数（差が単調増加）を整数版で
    mt19937 rng(6);
    for (int it = 0; it < 3000; it++) {
        int n = rng() % 30 + 1;
        vector<long long> diff(n);
        for (auto& d : diff) d = (long long)(rng() % 11) - 5;
        sort(diff.begin(), diff.end());  // 差が単調 → 凸
        vector<long long> f(n + 1, 0);
        for (int i = 0; i < n; i++) f[i + 1] = f[i] + diff[i];
        long long x = argmin_convex_int(0, n, [&](long long t) { return t > n ? LLONG_MAX : f[t]; });
        long long mn = *min_element(f.begin(), f.end());
        assert(f[x] == mn && find(f.begin(), f.end(), mn) - f.begin() == x);
    }
    puts("OK");
}
