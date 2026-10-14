// =====================================================================
// basic/binary_search.cpp  二分探索
//   「条件を満たすギリギリの値」を O(log 範囲) で求める。答えの二分探索にも使う
// 依存: なし
// ABC / ARC での使用例:
//   ABC146 C - Buy an Integer  https://atcoder.jp/contests/abc146/tasks/abc146_c
//     → 買える最大の N を答えで二分探索（bin_search）
//   ABC077 C - Snuke Festival  https://atcoder.jp/contests/abc077/tasks/arc084_a
//     → 中段を固定し、上段・下段の個数を count_lt / count_gt で数える
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体: 整数の二分探索（めぐる式） ----------
// ok : f(ok) == true が保証される値
// ng : f(ng) == false が保証される値（探索範囲の 1 つ外側でよい）
// f  : bool f(T x)。ok 側から ng 側に向かって true...true false...false と単調であること
// 返り値: f が true になる値のうち ng に最も近いもの
//   ok < ng なら「f が true の最大値」、ok > ng なら「f が true の最小値」
// T, F は自分で定義しない（T は ok/ng の型、F はラムダの型で、どちらも自動で決まる）
//   ok と ng の型が違うとき（例: 0 と 1e18）だけ bin_search<long long>(...) と T を明示する
// f はラムダで main / solve の中に書く: [&](long long x) { return 条件; }
template <class T, class F>
T bin_search(T ok, T ng, F f) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/binary_search.cpp
    while ((ok > ng ? ok - ng : ng - ok) > 1) {  // ★ ok と ng が隣り合うまで
        T mid = ok + (ng - ok) / 2;
        (f(mid) ? ok : ng) = mid;                // ★ 真ん中の結果で片方を寄せる
    }
    return ok;
}

// ---------- 削除可: 実数の二分探索（回数固定） ----------
template <class F>
double bin_search_real(double ok, double ng, F f, int iter = 100) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/binary_search.cpp
    for (int i = 0; i < iter; i++) {
        double mid = (ok + ng) / 2;
        (f(mid) ? ok : ng) = mid;
    }
    return ok;
}

// ---------- 削除可: ソート済み配列の個数カウント（v は昇順ソート済み） ----------
template <class T> int count_lt(const vector<T>& v, T x) {  // x 未満
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/binary_search.cpp
    return lower_bound(v.begin(), v.end(), x) - v.begin();
}
template <class T> int count_le(const vector<T>& v, T x) {  // x 以下
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/binary_search.cpp
    return upper_bound(v.begin(), v.end(), x) - v.begin();
}
template <class T> int count_gt(const vector<T>& v, T x) {  // x より大
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/binary_search.cpp
    return (int)v.size() - count_le(v, x);
}
template <class T> int count_ge(const vector<T>& v, T x) {  // x 以上
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/binary_search.cpp
    return (int)v.size() - count_lt(v, x);
}
template <class T> int count_range(const vector<T>& v, T l, T r) {  // [l, r)
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/binary_search.cpp
    return l >= r ? 0 : count_lt(v, r) - count_lt(v, l);
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    vector<int> a = {1, 3, 3, 5, 8};
    int n = a.size();
    // a[i] >= 4 となる最小の i（なければ n）: ok = n, ng = -1
    assert(bin_search<int>(n, -1, [&](int m) { return a[m] >= 4; }) == 3);
    // 答えの二分探索: m <= 100 を満たす最大の m（ok = 0, ng = 大きい値）
    assert(bin_search<long long>(0, (long long)4e18, [&](long long m) { return m <= 100; }) == 100);
    // 実数: x^2 <= 2 を満たす最大の x
    assert(abs(bin_search_real(0, 2, [](double x) { return x * x <= 2; }) - sqrt(2)) < 1e-9);
    assert(count_range(a, 3, 6) == 3 && count_ge(a, 4) == 2 && count_le(a, 3) == 3);
    // ランダムテスト: 線形探索・愚直な数え上げと比較
    {
        mt19937 rr(101);
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 10;
            vector<int> v(n);
            for (auto& x : v) x = rr() % 10;
            sort(v.begin(), v.end());
            int x = (int)(rr() % 12) - 1;
            int ref = n;
            for (int i = n - 1; i >= 0; i--) if (v[i] >= x) ref = i;
            assert(bin_search<int>(n, -1, [&](int m) { return v[m] >= x; }) == ref);
            int lt = 0, le = 0;
            for (int y : v) lt += y < x, le += y <= x;
            assert(count_lt(v, x) == lt && count_le(v, x) == le && count_gt(v, x) == n - le && count_ge(v, x) == n - lt);
            int l = (int)(rr() % 12) - 1, r = (int)(rr() % 12) - 1, rg = 0;
            for (int y : v) rg += l <= y && y < r;
            assert(count_range(v, l, r) == rg);
            long long X = rr() % 1000, mx = 0;
            while ((mx + 1) * (mx + 1) <= X) mx++;
            assert(bin_search<long long>(0, X + 1, [&](long long m) { return m * m <= X; }) == mx);
            assert(bin_search<long long>(X + 1, -1, [&](long long m) { return m * m >= X; }) == (mx * mx == X ? mx : mx + 1));
        }
    }
    puts("OK");
}
