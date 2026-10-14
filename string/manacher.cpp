// =====================================================================
// string/manacher.cpp  Manacher 法（回文）
//   すべての中心について「そこを中心とする最長の回文」の長さを O(N) で求める
//   → 最長回文部分文字列、任意の区間が回文かの判定 O(1)、回文の個数
//   ※ ハッシュで回文を判定したいなら rolling_hash.cpp
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
struct Palindrome {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/manacher.cpp
    int n;
    vector<int> rad;  // rad[i] = 中心 i の最長回文の長さ（元の文字列での文字数）
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
    bool is_pal(int l, int r) const {
        if (r - l <= 1) return true;
        return rad[l + r - 1] >= r - l;  // ★ 区間 [l, r) の中心は区切り入りの列の l + r - 1 番目
    }
    pair<int, int> longest() const {
        int best = 0, start = 0;
        for (int i = 0; i < (int)rad.size(); i++)
            if (rad[i] > best) best = rad[i], start = (i + 1) / 2 - rad[i] / 2;
        return {start, best};
    }
    long long count() const {
        long long c = 0;
        for (int i = 0; i < (int)rad.size(); i++) c += (rad[i] + 1) / 2;  // ★ 中心ごとに、長さ rad, rad-2, ... の回文がある
        return c;
    }
};

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
    }
    puts("OK");
}
