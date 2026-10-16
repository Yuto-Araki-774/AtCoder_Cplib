// =====================================================================
// string/manacher.cpp  Manacher 法（回文）
//   すべての中心について「そこを中心とする最長の回文」の長さを O(N) で求める
//   → 最長回文部分文字列、任意の区間が回文かの判定 O(1)、回文の個数
//   削除可: 回文分割（回文に分ける最小個数・分け方の数）O(N^2)
//   ※ ハッシュで回文を判定したいなら rolling_hash.cpp。書き換え・並べ替え・最長回文部分列（飛び飛び）は palindrome.cpp
// 依存: なし
// ABC / ARC での使用例:
//   ABC398 F - ABCBA  https://atcoder.jp/contests/abc398/tasks/abc398_f
//     → is_pal(i, n) が true になる最小の i を探し、s[0..i) を反転して後ろに付ける
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// Palindrome pal(s);         s は string でも vector でも可
//   pal.is_pal(l, r)          s[l..r) が回文か O(1)（空文字列は回文）
//   pal.longest()             最長回文部分文字列の {開始位置, 長さ}
//   pal.count()               回文になっている部分文字列（位置が違えば別）の個数
//   pal.rad[i]                文字の間に区切りを入れた列（長さ 2n-1）での半径（下の説明）
// 内部では s の文字の間に区切りを入れて 2n-1 個の中心を見る:
//   中心 i が偶数 → 文字 s[i/2] が中心（奇数長の回文）、奇数 → s[i/2] と s[i/2+1] の間（偶数長）
// ◆ struct Palindrome : Manacher（各中心の最長回文）。前計算 O(N)
//   Palindrome(s)   : s は string でも vector でもよい
//   is_pal(l, r)    : s[l..r) が回文か。O(1)（空・1 文字は回文）
//   longest()       : 最長回文の {開始位置, 長さ}
//   count()         : 回文の部分文字列の個数（位置が違えば別に数える）
//   rad[i]          : 中心 i（文字の間を含めて 2N−1 個）の最長回文の長さ
struct Palindrome {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/manacher.cpp
    int n;
    vector<int> rad;  // rad[i] = 中心 i の最長回文の長さ（元の文字列での文字数）
    // s : 回文を調べる列
    template <class Seq>
    Palindrome(const Seq& s) : n(s.size()) {
        if (n == 0) return;
        int m = 2 * n - 1;
        vector<int> r(m, 0);  // r[i] = 区切り入りの列で、中心 i から左右に何マス一致するか
        auto same = [&](int a, int b) { return (a % 2 == 1) || s[a / 2] == s[b / 2]; };  // 区切りどうしは常に一致
        for (int i = 0, c = 0, R = 0; i < m; i++) {  // [c - R + 1, c + R - 1] が今わかっている一番右まで届く回文
            int k = (i < c + R) ? min(r[2 * c - i], c + R - i) : 1;  // ★ 対称な位置の結果を使い回す（はみ出す分は切る）
            while (i - k >= 0 && i + k < m && same(i - k, i + k)) k++;  // ★ その先だけ実際に比べる
            r[i] = k;
            if (i + k > c + R) c = i, R = k;
        }
        rad.resize(m);
        for (int i = 0; i < m; i++) {
            // 区切り入りの列で半径 r[i]（中心を含む）→ 元の文字数に直す
            int len = r[i] - 1;  // 中心から片側に伸びたマス数
            rad[i] = (i % 2 == 0) ? (len / 2) * 2 + 1 : ((len + 1) / 2) * 2;
        }
    }
    // is_pal(l, r) : 区間 [l, r)。0 <= l <= r <= N
    bool is_pal(int l, int r) const {
        if (r - l <= 1) return true;
        return rad[l + r - 1] >= r - l;  // ★ 区間 [l, r) の中心は区切り入りの列の l + r - 1 番目
    }
    // longest() : 同じ長さが複数あれば一番左
    pair<int, int> longest() const {
        int best = 0, start = 0;
        for (int i = 0; i < (int)rad.size(); i++)
            if (rad[i] > best) best = rad[i], start = (i + 1) / 2 - rad[i] / 2;
        return {start, best};
    }
    // count() : 長さ 1 も含めた回文の個数
    long long count() const {
        long long c = 0;
        for (int i = 0; i < (int)rad.size(); i++) c += (rad[i] + 1) / 2;  // ★ 中心ごとに、長さ rad, rad-2, ... の回文がある
        return c;
    }
};

// ---------- 削除可: 回文分割（本体が必要） ----------
// ◆ pal_partition_min(s) : s を回文（連続した部分文字列）に区切るときの最小個数。O(N^2)（N <= 5000 程度）
//   s : string か vector。空なら 0
template <class Seq>
int pal_partition_min(const Seq& s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/manacher.cpp
    int n = s.size();
    Palindrome pal(s);
    vector<int> dp(n + 1, INT_MAX);  // dp[r] = s[0..r) を区切る最小個数
    dp[0] = 0;
    for (int r = 1; r <= n; r++)
        for (int l = 0; l < r; l++)
            if (pal.is_pal(l, r)) dp[r] = min(dp[r], dp[l] + 1);  // ★ 最後の 1 個が s[l..r)
    return dp[n];
}

// ◆ pal_partition_count<T>(s) : s を回文に区切る方法の数。O(N^2)
//   T : 数の型（mint 推奨。必ず書く）
//   s : string か vector。空なら 1（何も区切らない 1 通り）
template <class T, class Seq>
T pal_partition_count(const Seq& s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/manacher.cpp
    int n = s.size();
    Palindrome pal(s);
    vector<T> dp(n + 1, T(0));  // dp[r] = s[0..r) の区切り方の数
    dp[0] = T(1);
    for (int r = 1; r <= n; r++)
        for (int l = 0; l < r; l++)
            if (pal.is_pal(l, r)) dp[r] += dp[l];
    return dp[n];
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    string s = "abacabadd";
    Palindrome pal(s);
    assert(pal.is_pal(0, 7) && !pal.is_pal(0, 8) && pal.is_pal(7, 9) && pal.is_pal(3, 4));
    assert(pal.longest() == make_pair(0, 7));  // "abacaba"
    assert(Palindrome(string("aaaa")).count() == 10);

    // ランダムテスト: 全区間を愚直に比較
    mt19937 rng(15);
    for (int it = 0; it < 2000; it++) {
        int n = rng() % 15 + 1;
        string t;
        for (int i = 0; i < n; i++) t += char('a' + rng() % 2);
        Palindrome p(t);
        long long cnt = 0;
        int best = 0;
        for (int l = 0; l < n; l++)
            for (int r = l + 1; r <= n; r++) {
                string u = t.substr(l, r - l);
                bool ok = equal(u.begin(), u.end(), u.rbegin());
                assert(p.is_pal(l, r) == ok);
                cnt += ok;
                if (ok) best = max(best, r - l);
            }
        auto [st, len] = p.longest();
        assert(p.count() == cnt && len == best && p.is_pal(st, st + len));
        // 回文分割: 区切り方 2^(n−1) 通りを全部試す
        int mn = INT_MAX;
        long long ways = 0;
        for (int mask = 0; mask < (1 << (n - 1)); mask++) {
            int l = 0, pieces = 0;
            bool ok = true;
            for (int i = 1; i <= n; i++)
                if (i == n || (mask >> (i - 1) & 1)) {
                    string u = t.substr(l, i - l);
                    ok &= equal(u.begin(), u.end(), u.rbegin());
                    l = i, pieces++;
                }
            if (ok) mn = min(mn, pieces), ways++;
        }
        assert(pal_partition_min(t) == mn && pal_partition_count<long long>(t) == ways);
    }
    assert(pal_partition_min(string("aab")) == 2 && pal_partition_count<long long>(string("aab")) == 2);  // aa|b, a|a|b
    assert(pal_partition_min(string("")) == 0 && pal_partition_count<long long>(string("")) == 1);
    puts("OK");
}
