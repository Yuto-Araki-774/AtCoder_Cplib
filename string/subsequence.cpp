// =====================================================================
// string/subsequence.cpp  部分列（飛び飛びに選んだ文字の列）
//   本体A: 次の文字の位置の表 nxt[i][c]（i 以降で文字 c が最初に出る位置）
//          → t が部分列か O(|t|)、t を部分列として含む最短の区間、部分列でない最短の文字列
//   本体B: 種類の違う部分列の数 O(N log N)
//   本体C: 長さ K の部分列で辞書順最小（最大）のもの O(N)
//   A〜C は互いに独立
//   ★ 部分列の判定は「前から貪欲に、一番早く出てくる位置を使う」でよい（早く使うほど後の選択肢が広い）
//   部分列ではなく部分文字列（連続）を探すなら kmp.cpp。2 つの列の最長共通部分列は dp/lcs.cpp
// 依存: なし
// ABC / ARC での使用例:
//   ARC081 E - Don't Be a Subsequence  https://atcoder.jp/contests/arc081/tasks/arc081_c
//     → Subsequence<>(a).shortest_absent() がそのまま答え
//   ABC452 D - No-Subsequence Substring  https://atcoder.jp/contests/abc452/tasks/abc452_d
//     → 各開始位置 l で e = sub.end_of(t, l)（t を部分列として含む最短の区間 [l, e)）。
//        t を含む区間は [l, e), [l, e+1), ..., [l, N) の N − e + 1 個なので、全体 N(N+1)/2 から引く
//   ABC138 E - Strings of Impurity  https://atcoder.jp/contests/abc138/tasks/abc138_e
//     → s を 2 回つなげたもので nxt を作り、t の文字ごとに nxt(i % |s|, c) で次の位置へ進んで、何周したかを数える
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 次の文字の位置の表 ----------
// ◆ struct Subsequence<SIGMA, BASE> : 部分列の判定用の表。前計算 O(N × SIGMA)、メモリ O(N × SIGMA)
//   Subsequence<> sub(s)  : s は文字 BASE .. BASE + SIGMA − 1 だけからなる文字列（既定は英小文字）
//   nxt(i, c)             : i 以上で s[j] == c となる最小の j（なければ N）。0 <= i <= N
//   is_subseq(t)          : t が s の部分列か。O(|t|)
//   match(t, i)           : s[i..) の部分列として、t の先頭から最大何文字とれるか。O(|t|)
//   end_of(t, i)          : t が s[i..e) の部分列になる最小の e（なければ −1）。O(|t|)
//   shortest_absent()     : s の部分列でない最短の文字列のうち辞書順最小。O(N × SIGMA)
//   SIGMA, BASE : 文字の種類数と最初の文字（英大文字なら Subsequence<26, 'A'>）
template <int SIGMA = 26, char BASE = 'a'>
struct Subsequence {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/subsequence.cpp
    int n;
    vector<array<int, SIGMA>> nx;  // nx[i][c] = i 以上で文字 c が最初に出る位置（なければ n）
    // s : 元の文字列
    Subsequence(const string& s) : n(s.size()), nx(n + 1) {
        nx[n].fill(n);
        for (int i = n - 1; i >= 0; i--) {
            nx[i] = nx[i + 1];         // ★ 後ろから作る: i + 1 の表をコピーして、
            nx[i][s[i] - BASE] = i;    // ★ s[i] の所だけ i に書き換える
        }
    }
    // nxt(i, c) : c は文字そのもの（'a' など）
    int nxt(int i, char c) const { return nx[i][c - BASE]; }
    // match(t, i) : i : 探し始める位置（0 <= i <= N、省略で 0）
    int match(const string& t, int i = 0) const {
        int k = 0;
        for (char c : t) {
            int j = nx[i][c - BASE];
            if (j == n) break;
            i = j + 1, k++;  // ★ 一番早く出てくる所を使う
        }
        return k;
    }
    // is_subseq(t) : 空の t は常に true
    bool is_subseq(const string& t) const { return match(t) == (int)t.size(); }
    // end_of(t, i) : i : 探し始める位置（0 <= i <= N）。t が空なら i
    int end_of(const string& t, int i = 0) const {
        for (char c : t) {
            int j = nx[i][c - BASE];
            if (j == n) return -1;
            i = j + 1;
        }
        return i;
    }
    // shortest_absent() : s が空なら BASE 1 文字
    string shortest_absent() const {
        // d[i] = s[i..) の部分列でない最短の長さ。d[n + 1] = 0（「もう文字がない」を表す番兵）
        vector<int> d(n + 2, 0);
        auto go = [&](int i, int c) { return (i >= n ? n : nx[i][c]) + 1; };  // 文字 c を使った後の位置（使えなければ n + 1）
        for (int i = n; i >= 0; i--) {
            d[i] = INT_MAX;
            for (int c = 0; c < SIGMA; c++) d[i] = min(d[i], d[go(i, c)] + 1);  // ★ 次の文字 c を決めると、残りは go(i, c) からの問題
        }
        string res;
        for (int i = 0; i <= n;) {
            for (int c = 0; c < SIGMA; c++)
                if (d[go(i, c)] + 1 == d[i]) {  // ★ 最短を保てる一番小さい文字を選ぶ
                    res += char(BASE + c), i = go(i, c);
                    break;
                }
        }
        return res;
    }
};

// ---------- 本体B: 種類の違う部分列の数 ----------
// ◆ count_distinct_subseq<T>(s) : 空でない部分列の種類の数（位置が違っても同じ列なら 1 つ）。O(N log N)
//   T : 数の型（mint 推奨。必ず書く）
//   s : string か vector
template <class T, class Seq>
T count_distinct_subseq(const Seq& s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/subsequence.cpp
    T all = T(1);  // ここまでの部分列の種類の数（空を含む）
    map<typename Seq::value_type, T> last;  // last[x] = 前に x を読む直前の all
    for (auto& x : s) {
        T before = all;
        all = all * T(2);                       // ★ 今までの全部に x を付ける / 付けない
        auto it = last.find(x);
        if (it != last.end()) all -= it->second;  // ★ 前に x を読んだときに付けた分と重なるので引く
        last[x] = before;
    }
    return all - T(1);  // 空の列を除く
}

// ---------- 本体C: 辞書順最小の部分列 ----------
// ◆ min_subseq(s, k, largest) : 長さ k の部分列のうち辞書順最小（largest = true なら最大）のもの。O(N)
//   s : string か vector
//   k : 長さ（0 <= k <= N）。「K 文字消して辞書順最小」は min_subseq(s, N − K)
template <class Seq>
Seq min_subseq(const Seq& s, int k, bool largest = false) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/subsequence.cpp
    Seq res;
    int del = (int)s.size() - k;  // あと何文字捨ててよいか
    for (auto& x : s) {
        while (!res.empty() && del > 0 && (largest ? res.back() < x : x < res.back())) {
            res.pop_back(), del--;  // ★ 後ろの方が大きい（小さい）なら、捨てて x を前に出した方がよい
        }
        res.push_back(x);
    }
    res.resize(k);  // 捨て切れなかった分は後ろから捨てる
    return res;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    {
        Subsequence<> sub("abcab");
        assert(sub.nxt(0, 'b') == 1 && sub.nxt(2, 'b') == 4 && sub.nxt(2, 'z') == 5 && sub.nxt(5, 'a') == 5);
        assert(sub.is_subseq("acb") && sub.is_subseq("") && !sub.is_subseq("cc") && !sub.is_subseq("bca" "a"));
        assert(sub.match("abz") == 2 && sub.match("ab", 3) == 2 && sub.match("ab", 4) == 0);
        assert(sub.end_of("ca") == 4 && sub.end_of("ab", 2) == 5 && sub.end_of("cc") == -1 && sub.end_of("", 3) == 3);
    }
    // ARC081 E のサンプル
    assert(Subsequence<>("atcoderregularcontest").shortest_absent() == "b");
    assert(Subsequence<>("abcdefghijklmnopqrstuvwxyz").shortest_absent() == "aa");
    assert(Subsequence<>("").shortest_absent() == "a");
    // ABC452 D のサンプル 1（t を部分列として含まない区間の数）
    {
        string s = "abrakadabra", t = "aba";
        Subsequence<> sub(s);
        long long n = s.size(), ans = n * (n + 1) / 2;
        for (int l = 0; l < n; l++) {
            int e = sub.end_of(t, l);
            if (e != -1) ans -= n - e + 1;
        }
        assert(ans == 51);
    }
    assert(count_distinct_subseq<long long>(string("aba")) == 6);  // a b ab aa ba aba
    assert(count_distinct_subseq<long long>(string("aaa")) == 3);
    assert((count_distinct_subseq<long long>(vector<int>{1, 2, 1, 2}) == 11));
    assert(min_subseq(string("cbacdcbc"), 4) == "acbc");
    assert(min_subseq(string("bca"), 2, true) == "ca");
    assert((min_subseq(vector<int>{3, 1, 2}, 2) == vector<int>{1, 2}));

    // ランダムテスト: 全部の部分列を列挙する愚直と比較
    mt19937 rng(332);
    for (int it = 0; it < 1500; it++) {
        int n = rng() % 11, sig = rng() % 3 + 1;
        string s;
        for (int i = 0; i < n; i++) s += char('a' + rng() % sig);
        set<string> all;  // 空でない部分列すべて
        for (int mask = 1; mask < (1 << n); mask++) {
            string u;
            for (int i = 0; i < n; i++) if (mask >> i & 1) u += s[i];
            all.insert(u);
        }
        assert(count_distinct_subseq<long long>(s) == (long long)all.size());
        Subsequence<3> sub(s);
        // shortest_absent: 短い順・辞書順に {a, b, c} の文字列を作って、最初に部分列でないもの
        {
            string ans;
            for (int len = 1; ans.empty(); len++) {
                int tot = 1;
                for (int j = 0; j < len; j++) tot *= 3;
                for (int x = 0; x < tot && ans.empty(); x++) {
                    string u(len, 'a');
                    for (int j = len - 1, y = x; j >= 0; j--, y /= 3) u[j] = 'a' + y % 3;
                    if (!all.count(u)) ans = u;
                }
            }
            assert(sub.shortest_absent() == ans);
        }
        // is_subseq / match / end_of
        for (int q = 0; q < 10; q++) {
            string t;
            for (int j = rng() % 5; j > 0; j--) t += char('a' + rng() % 3);
            assert(sub.is_subseq(t) == (t.empty() || all.count(t)));
            int i = rng() % (n + 1);
            string suf = s.substr(i);
            int k = 0;
            while (k < (int)t.size()) {  // 愚直: t の先頭 k + 1 文字が s[i..) の部分列か
                string pre = t.substr(0, k + 1);
                size_t p = 0;
                for (char c : suf) if (p < pre.size() && pre[p] == c) p++;
                if (p < pre.size()) break;
                k++;
            }
            assert(sub.match(t, i) == k);
            int e = -1;
            for (int r = i; r <= n && e == -1; r++) {
                size_t p = 0;
                for (int j = i; j < r; j++) if (p < t.size() && t[p] == s[j]) p++;
                if (p == t.size()) e = r;
            }
            assert(sub.end_of(t, i) == e);
        }
        // min_subseq: 長さ k の部分列（重複を含めて全部）の最小・最大
        for (int k = 0; k <= n; k++) {
            string mn = "~", mx = "";
            for (int mask = 0; mask < (1 << n); mask++) {
                if (__builtin_popcount(mask) != k) continue;
                string u;
                for (int i = 0; i < n; i++) if (mask >> i & 1) u += s[i];
                mn = min(mn, u), mx = max(mx, u);
            }
            assert(min_subseq(s, k) == mn && min_subseq(s, k, true) == mx);
        }
    }
    puts("OK");
}
