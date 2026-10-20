// =====================================================================
// string/rolling_hash.cpp  ローリングハッシュ（mod 2^61 - 1）
//   前計算 O(N) の後、任意の部分文字列のハッシュを O(1)
//   「部分文字列どうしが等しいか」を O(1) で判定できる。LCP（最長共通接頭辞）も O(log N)
//   基数は実行ごとにランダム（ハック対策）。衝突確率は 1 回あたり約 N / 2^61
// 依存: なし
// ABC / ARC での使用例:
//   ABC141 E - Who Says a Pun?  https://atcoder.jp/contests/abc141/tasks/abc141_e
//     → 長さを二分探索し、重ならない同じ部分文字列があるかをハッシュで判定
//   ABC284 F - ABCBAC  https://atcoder.jp/contests/abc284/tasks/abc284_f
//     → T と反転した T のハッシュで、各 i について O(1) で判定
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// RollingHash rh(s);   （s は string。vector<int> なども可）
//   rh.get(l, r)   s[l..r) のハッシュ値（uint64_t）
//   rh.get(l1, r1) == rh.get(l2, r2) なら、その 2 つの部分文字列は（ほぼ確実に）等しい
// 別の文字列どうしの比較もできる（基数を全インスタンスで共有しているため）
//   RollingHash a(s), b(t);  a.get(...) == b.get(...)
// 回文判定: 反転文字列 rev の RollingHash も作り、s[l..r) と rev[n-r..n-l) を比較
// ◆ struct RollingHash : ローリングハッシュ（mod 2^61−1。部分文字列の一致を O(1) で判定）
//   RollingHash(s) : 前計算 O(N)
//   get(l, r)      : s[l..r) のハッシュ値。2 つのハッシュ値が同じなら（ほぼ確実に）同じ文字列
//   lcp(a, b)      : s[a..) と s[b..) の最長共通接頭辞の長さ。O(log N)
//   注意 : 基数は全部の RollingHash で共通なので、別の文字列どうしの get も比べられる
struct RollingHash {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/rolling_hash.cpp
    static constexpr uint64_t MOD = (1ULL << 61) - 1;
    // （内部用）a × b mod 2^61−1
    static uint64_t mul(uint64_t a, uint64_t b) {  // a * b mod 2^61-1（128 bit で計算）
        __uint128_t t = (__uint128_t)a * b;
        uint64_t r = (uint64_t)(t >> 61) + (uint64_t)(t & MOD);  // ★ 2^61 ≡ 1 なので上位と下位を足すだけ
        return r >= MOD ? r - MOD : r;
    }
    // （内部用）基数（実行ごとに乱数で決める）
    static uint64_t base() {
        static uint64_t b = mt19937_64(chrono::steady_clock::now().time_since_epoch().count())() % (MOD - 2) + 2;
        return b;
    }

    vector<uint64_t> h, pw;  // h[i] = s[0..i) のハッシュ、pw[i] = base^i
    // s : string か vector<int> など（要素は 0 以上の整数として扱える型）
    template <class Seq>
    RollingHash(const Seq& s) : h(s.size() + 1, 0), pw(s.size() + 1, 1) {
        uint64_t b = base();
        for (int i = 0; i < (int)s.size(); i++) {
            h[i + 1] = mul(h[i], b) + (uint64_t)s[i] + 1;  // ★ h = h * base + 文字（+1 は 0 を避けるため）
            if (h[i + 1] >= MOD) h[i + 1] -= MOD;
            pw[i + 1] = mul(pw[i], b);
        }
    }
    // get(l, r) : 区間 [l, r)。0 <= l <= r <= N（空区間は 0）
    uint64_t get(int l, int r) const {
        uint64_t res = h[r] + MOD - mul(h[l], pw[r - l]);  // ★ h[r] - h[l] * base^(r-l)
        return res >= MOD ? res - MOD : res;
    }

    // --- 削除可: LCP（s[a..) と s[b..) の最長共通接頭辞の長さ）O(log N) ---
    // lcp(a, b) : a, b : 開始位置（0..N）
    long long lcp(int a, int b) const {
        int ok = 0, ng = min(h.size() - a, h.size() - b);  // 長さ ng はとれない
        while (ng - ok > 1) {
            int mid = (ok + ng) / 2;
            (get(a, a + mid) == get(b, b + mid) ? ok : ng) = mid;  // ★ 長さで二分探索
        }
        return ok;
    }
    // --- 削除可ここまで ---
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    string s = "abcabcx";
    RollingHash rh(s);
    assert(rh.get(0, 3) == rh.get(3, 6));  // "abc" == "abc"
    assert(rh.get(0, 3) != rh.get(1, 4));  // "abc" != "bca"
    assert(rh.lcp(0, 3) == 3 && rh.lcp(1, 4) == 2 && rh.lcp(0, 1) == 0);

    // 別の文字列との比較: t が s の中に何回出てくるか
    string t = "bc";
    RollingHash rt(t);
    int cnt = 0;
    for (int i = 0; i + (int)t.size() <= (int)s.size(); i++) cnt += rh.get(i, i + t.size()) == rt.get(0, t.size());
    assert(cnt == 2);

    // 回文判定
    string p = "xabcbay", rev(p.rbegin(), p.rend());
    RollingHash hp(p), hr(rev);
    int n = p.size();
    auto is_pal = [&](int l, int r) { return hp.get(l, r) == hr.get(n - r, n - l); };
    assert(is_pal(1, 6) && !is_pal(0, 6));

    // ランダムテスト: ハッシュが等しい ⇔ 文字列が等しい
    mt19937 rng(1);
    string u;
    for (int i = 0; i < 300; i++) u += char('a' + rng() % 2);
    RollingHash hu(u);
    for (int it = 0; it < 20000; it++) {
        int l1 = rng() % 300, l2 = rng() % 300, len = rng() % (300 - max(l1, l2) + 1);
        assert((hu.get(l1, l1 + len) == hu.get(l2, l2 + len)) == (u.substr(l1, len) == u.substr(l2, len)));
    }
    puts("OK");
}
