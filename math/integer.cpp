// =====================================================================
// math/integer.cpp  整数の小物（繰り返し二乗・拡張ユークリッド・整数平方根・負数対応の割り算・進数変換）
//   本体A: 累乗（mod なし・上限付き / 64 bit mod）
//   本体B: 拡張ユークリッドの互除法
//   本体C: 整数平方根・切り捨て / 切り上げ除算・負数対応の mod
//   本体D: 進数変換（10 進 ↔ b 進。負の進数も）
//   A・B・C・D は互いに独立
//   ※ mod が int に収まるときの累乗・逆元は ACL の pow_mod / inv_mod（atcoder/math）でもよい
// 依存: なし
// ABC / ARC での使用例:
//   ABC239 B - Integer Division  https://atcoder.jp/contests/abc239/tasks/abc239_b
//     → 負の数の切り捨て除算（floor_div）
//   ABC400 C - 2^a b^2  https://atcoder.jp/contests/abc400/tasks/abc400_c
//     → a = 1, 2 の場合に分けて isqrt で数える
//   ABC340 F - S = 1  https://atcoder.jp/contests/abc340/tasks/abc340_f
//     → |X·B − Y·A| = 2 を拡張ユークリッドで解く（ext_gcd）
//   ABC105 C - Base -2 Number  https://atcoder.jp/contests/abc105/tasks/abc105_c
//     → to_base_str(N, -2) がそのまま答え（N = 0 は "0"）
//   ABC156 B - Digits  https://atcoder.jp/contests/abc156/tasks/abc156_b
//     → to_base(N, K).size()
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 繰り返し二乗法 O(log n) ----------
// ipow(a, n)          : a^n（mod なし）。long long に収まる前提
// ipow_cap(a, n, cap) : min(a^n, cap)。a >= 0。途中でオーバーフローしない
//                       「a^n が X を超えるか」の判定に使う: ipow_cap(a, n, X + 1) > X
// pow_mod64(a, n, m)  : a^n mod m。m が 1e18 程度まで OK（128 bit で掛け算）
// ◆ ipow(a, n) : a^n（mod なし）。O(log n)
//   a : 底（負でもよい）、n : 指数（0 以上。負は不可）
//   注意 : 答えが long long に収まること（収まらないなら ipow_cap か pow_mod64）
long long ipow(long long a, long long n) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp
    long long r = 1;
    while (n > 0) {
        if (n & 1) r *= a;  // ★ n の 2 進数で 1 の桁だけ掛ける
        n >>= 1;
        if (n > 0) a *= a;  // ★ a, a^2, a^4, ...（最後の 1 回は不要。答えが収まっても a*a があふれることがあるので掛けない）
    }
    return r;
}
// ◆ ipow_cap(a, n, cap) : min(a^n, cap)。途中であふれない。O(min(n, 63))
//   a : 底（0 以上）、n : 指数（0 以上）、cap : 上限（1 以上）
long long ipow_cap(long long a, long long n, long long cap) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp
    __int128 r = 1;
    for (long long i = 0; i < n; i++) {
        r *= a;
        if (r >= cap) return cap;  // ★ 超えた時点で打ち切る（a >= 2 なら高々 63 回で止まる）
        if (a <= 1) break;         // a = 0, 1 のときは値が変わらないので打ち切り
    }
    return (long long)r;
}
// ◆ pow_mod64(a, n, m) : a^n mod m（128 bit で掛け算するので m は 1e18 程度まで）。O(log n)
//   a : 底（負でもよい）、n : 指数（0 以上）、m : mod（1 以上）
long long pow_mod64(long long a, long long n, long long m) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp
    __int128 r = 1 % m, x = ((a % m) + m) % m;
    while (n > 0) {
        if (n & 1) r = r * x % m;
        x = x * x % m;  // ★ 128 bit なので m が 1e18 でも積があふれない
        n >>= 1;
    }
    return (long long)r;
}

// ---------- 本体B: 拡張ユークリッドの互除法 O(log min(a, b)) ----------
// ext_gcd(a, b, x, y) : g = gcd(a, b) を返し、a * x + b * y = g となる x, y を入れる
// 使い道: 「a * x ≡ c (mod m) を解く」「mod が素数でないときの逆元」「ax + by = c の整数解」
//   例: a * x + b * y = c の解は c % g == 0 のとき x0 = x * (c / g), y0 = y * (c / g)
//       一般解は x0 + k * (b / g), y0 - k * (a / g)（k は整数）
// ◆ ext_gcd(a, b, x, y) : g = gcd(a, b) を返し、a x + b y = g となる x, y を入れる。O(log)
//   a, b : 0 以上の整数（両方 0 なら g = 0）
//   x, y : 答えを入れる変数（参照）。|x| <= b, |y| <= a 程度の小さい解が入る
long long ext_gcd(long long a, long long b, long long& x, long long& y) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp
    if (b == 0) { x = 1, y = 0; return a; }
    long long g = ext_gcd(b, a % b, y, x);  // ★ b * y + (a % b) * x = g を先に解いて
    y -= a / b * x;                          // ★ a % b = a - (a / b) * b を代入して戻す
    return g;
}

// ---------- 本体C: 整数平方根・割り算の小物 ----------
// isqrt(n)        : floor(√n)（n >= 0）。sqrt() の誤差を補正済み
// floor_div(a, b) : floor(a / b)。負の数でも正しい（C++ の / は 0 方向に丸めるので負で 1 ずれる）
// ceil_div(a, b)  : ceil(a / b)。負の数でも正しい
// mod_pos(a, m)   : a mod m を 0 〜 m-1 で返す（C++ の % は負の数で負になる）
// ◆ isqrt(n) : floor(√n)。n : 0 以上（誤差なし）
long long isqrt(long long n) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp
    long long r = sqrtl((long double)n);
    while (r > 0 && (__int128)r * r > n) r--;          // ★ 浮動小数の誤差で大きすぎたら下げる
    while ((__int128)(r + 1) * (r + 1) <= n) r++;      // ★ 小さすぎたら上げる
    return r;
}
// ◆ floor_div(a, b) : floor(a / b)。a, b : 負でもよい（b ≠ 0）
long long floor_div(long long a, long long b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp
    return a / b - ((a % b != 0) && ((a < 0) != (b < 0)));  // ★ 割り切れず符号が違うと 0 方向に丸められている
}
// ◆ ceil_div(a, b) : ceil(a / b)。a, b : 負でもよい（b ≠ 0）
long long ceil_div(long long a, long long b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp
    return a / b + ((a % b != 0) && ((a < 0) == (b < 0)));
}
// ◆ mod_pos(a, m) : a mod m を 0..m−1 で返す。a : 負でもよい、m : 1 以上
long long mod_pos(long long a, long long m) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp
    long long r = a % m;
    return r < 0 ? r + m : r;
}

// ---------- 本体D: 進数変換 ----------
// to_base(x, b)         x を b 進数にした桁の列（上の桁から。x = 0 なら {0}）
//                       b >= 2 なら x >= 0。b <= -2（負の進数、例: -2 進数）なら x は負でもよい
// from_base(digits, b)  桁の列（上の桁から）を数に戻す（long long に収まること）
// to_base_str(x, b)     文字列で返す（10 以上の桁は 'A', 'B', ...）。|b| <= 36
// ◆ to_base(x, b) : x を b 進数にした桁の列（上の桁から。x = 0 なら {0}）
//   x : 変換する数（b >= 2 なら 0 以上。b <= −2 なら負でもよい）
//   b : 基数（2 以上、または −2 以下。−1, 0, 1 は assert で止まる）
vector<long long> to_base(long long x, long long b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp
    assert(b >= 2 || b <= -2);
    assert(b < 0 || x >= 0);
    if (x == 0) return {0};
    vector<long long> d;
    while (x != 0) {
        long long r = x % b;  // ★ 一番下の桁 = b で割った余り
        x /= b;
        if (r < 0) r += (b > 0 ? b : -b), x += 1;  // ★ 負の進数では余りが負になることがあるので 0 以上 |b| 未満に直す
        d.push_back(r);
    }
    reverse(d.begin(), d.end());
    return d;
}
// ◆ from_base(d, b) : 桁の列（上の桁から）を数に戻す
//   d : 各桁（0..|b|−1）、b : 基数（負でもよい）。結果が long long に収まること
long long from_base(const vector<long long>& d, long long b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp
    long long x = 0;
    for (long long c : d) x = x * b + c;  // ★ 上の桁から「b 倍して足す」
    return x;
}
// ◆ to_base_str(x, b) : to_base の結果を文字列で（10 以上の桁は A, B, ...）
//   x, b : to_base と同じ（|b| <= 36）
string to_base_str(long long x, long long b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp
    string s;
    for (int c : to_base(x, b)) s += (char)(c < 10 ? '0' + c : 'A' + c - 10);
    return s;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    assert(ipow(3, 4) == 81 && ipow(2, 62) == (1LL << 62) && ipow(7, 0) == 1);
    assert(ipow_cap(10, 30, (long long)1e18) == (long long)1e18);  // 10^30 は 1e18 で頭打ち
    assert(ipow_cap(10, 5, (long long)1e18) == 100000);
    assert(ipow_cap(1, (long long)1e18, 100) == 1 && ipow_cap(0, 5, 100) == 0);
    // 「2^n > 1e18 となる最小の n」
    long long X = (long long)1e18;
    int n = 0;
    while (ipow_cap(2, n, X + 1) <= X) n++;
    assert(n == 60);
    assert(pow_mod64(2, 100, 1000000007) == 976371285);
    assert(pow_mod64(123456789012345678LL, 1000000000000LL, 999999999999999989LL) ==
           pow_mod64(123456789012345678LL, 1000000000000LL % (999999999999999989LL - 1), 999999999999999989LL));  // フェルマー

    long long x, y;
    assert(ext_gcd(240, 46, x, y) == 2 && 240 * x + 46 * y == 2);
    // 7x ≡ 1 (mod 40) → x = 23（40 は素数でない）
    ext_gcd(7, 40, x, y);
    assert(mod_pos(x, 40) == 23);

    mt19937_64 rng(1);
    for (int it = 0; it < 100000; it++) {
        long long v = rng() % (1LL << 62);
        long long r = isqrt(v);
        assert((__int128)r * r <= v && (__int128)(r + 1) * (r + 1) > v);
        long long a = (long long)(rng() % 2001) - 1000, b = (long long)(rng() % 41) - 20;
        if (b == 0) continue;
        assert(floor_div(a, b) == (long long)floorl((long double)a / b));
        assert(ceil_div(a, b) == (long long)ceill((long double)a / b));
        if (b > 0) assert(mod_pos(a, b) == a - floor_div(a, b) * b);
    }
    assert(isqrt(0) == 0 && isqrt(LLONG_MAX) == 3037000499LL);
    // ランダムテスト: ipow を 128 bit の愚直な掛け算と比較（答えが long long に収まる範囲すべて）
    for (long long a = -12; a <= 12; a++)
        for (long long e = 0; e <= 63; e++) {
            __int128 r = 1;
            bool fits = true;
            for (int i = 0; i < e && fits; i++) { r *= a; if (r > LLONG_MAX || r < LLONG_MIN) fits = false; }
            if (!fits) break;
            assert(ipow(a, e) == (long long)r);
            if (a >= 0) assert(ipow_cap(a, e, (long long)4e18) == (long long)min<__int128>(r, (__int128)4e18));
        }
    // 進数変換
    {
        assert((to_base(6, 2) == vector<long long>{1, 1, 0}) && from_base({1, 1, 0}, 2) == 6);
        assert(to_base_str(255, 16) == "FF" && to_base_str(0, 7) == "0" && to_base_str(35, 36) == "Z");
        assert(to_base_str(-9, -2) == "1011");  // -8 + 0 + (-2) + 1 = -9
        assert(to_base_str(6, -2) == "11010");  // 16 - 8 + 0 + (-2) + 0 = 6
        mt19937_64 rb(153);
        for (int it = 0; it < 20000; it++) {
            long long b = (long long)(rb() % 35) + 2;
            if (rb() % 2) b = -b;
            long long x = (long long)(rb() % 2000000000001LL) - 1000000000000LL;
            if (b > 0) x = llabs(x);
            auto d = to_base(x, b);
            assert(from_base(d, b) == x && (d.size() == 1 || d[0] != 0));
            for (int c : d) assert(0 <= c && c < llabs(b));
            if (b == 10) assert(to_base_str(x, b) == to_string(x));
        }
        // -2 進数: 長さ 10 までの 0/1 の列を全部作り、値ごとの表現がちょうど 1 つで to_base と一致するか
        map<long long, vector<long long>> rep;
        for (int len = 1; len <= 10; len++)
            for (int m = 0; m < (1 << len); m++) {
                vector<long long> d(len);
                for (int i = 0; i < len; i++) d[i] = m >> (len - 1 - i) & 1;
                if (len > 1 && d[0] == 0) continue;
                long long v = from_base(d, -2);
                assert(!rep.count(v));
                rep[v] = d;
            }
        for (auto& [v, d] : rep) assert(to_base(v, -2) == d);
    }
    puts("OK");
}
