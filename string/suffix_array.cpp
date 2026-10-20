// =====================================================================
// string/suffix_array.cpp  接尾辞配列（Suffix Array）の使い方まとめ
//   ACL の suffix_array / lcp_array の上に、よく使う機能をまとめた
//   sa[i] = 辞書順で i 番目の接尾辞の開始位置、lcp[i] = sa[i] と sa[i+1] の接尾辞の最長共通接頭辞
//   → 種類の違う部分文字列の数、2 回以上現れる最長の部分文字列、k 番目の部分文字列、パターンの出現回数、
//      任意の 2 つの接尾辞の LCP・部分文字列の大小比較 O(1)、2 つの文字列の最長共通部分文字列
//   ★ 部分文字列 = 「ある接尾辞の接頭辞」。辞書順に並べた接尾辞の隣どうしの LCP を見ると、重複が数えられる
// 依存: ACL の suffix_array, lcp_array（AtCoder ではテンプレートの atcoder/all に含まれる）
// ABC / ARC での使用例:
//   ALPC I - Number of Substrings  https://atcoder.jp/contests/practice2/tasks/practice2_i
//     → SuffixArray(s).count_distinct() がそのまま答え（ABC ではないが ACL 公式の練習問題）
//   ABC213 F - Common Prefixes  https://atcoder.jp/contests/abc213/tasks/abc213_f
//     → 答え(k) = Σ_i lcp_of(k, i)。sa の順に並べると LCP は間の lcp[] の最小なので、
//        前から・後ろからそれぞれスタックで「最小値の和」を持って O(N)
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/string>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// ◆ struct SuffixArray<Seq> : 接尾辞配列と LCP。前計算 O(N log N)
//   SuffixArray sa(s)       : s は string か vector<int> / vector<long long>（型は自動で決まる）
//   sa[i], rnk[p], lcp[i]   : 辞書順 i 番目の接尾辞の開始位置 / 開始位置 p の接尾辞の順位 / sa[i] と sa[i+1] の LCP
//   lcp_of(i, j)            : s[i..) と s[j..) の最長共通接頭辞の長さ。O(1)
//   compare(l1, r1, l2, r2) : s[l1..r1) と s[l2..r2) の辞書順の比較（小さい −1、同じ 0、大きい 1）。O(1)
//   count_distinct()        : 種類の違う（空でない）部分文字列の数
//   longest_repeated()      : 2 回以上現れる最長の部分文字列 {開始位置, 長さ}（重なってもよい。なければ長さ 0）
//   kth_substring(k)        : 種類の違う部分文字列を辞書順に並べた k 番目（0 始まり）{開始位置, 長さ}。O(log N)
//   find(t)                 : t で始まる接尾辞の範囲 [lo, hi)（sa の添字）。出現回数 = hi − lo、出現位置 = sa[lo..hi)。O(|t| log N)
template <class Seq = string>
struct SuffixArray {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/suffix_array.cpp
    Seq s;
    int n;
    vector<int> sa, rnk, lcp;
    vector<vector<int>> mn;    // lcp の Sparse Table
    vector<long long> cum;     // cum[i] = sa[0..i) の接尾辞が新しく生む部分文字列の数の合計
    // s_ : 元の列（コピーして持つ）
    SuffixArray(const Seq& s_) : s(s_), n(s_.size()), rnk(n), cum(n + 1, 0) {
        if (n == 0) return;
        sa = atcoder::suffix_array(s);
        lcp = atcoder::lcp_array(s, sa);
        for (int i = 0; i < n; i++) rnk[sa[i]] = i;
        for (int i = 0; i < n; i++) cum[i + 1] = cum[i] + (n - sa[i]) - (i ? lcp[i - 1] : 0);  // ★ 前の接尾辞と共通の分は数え済み
        mn.push_back(lcp);
        for (int k = 1; (1 << k) <= n - 1; k++) {
            mn.push_back(vector<int>(n - (1 << k)));
            for (int i = 0; i + (1 << k) <= n - 1; i++) mn[k][i] = min(mn[k - 1][i], mn[k - 1][i + (1 << (k - 1))]);
        }
    }
    // lcp_of(i, j) : 0 <= i, j <= N（N は空の接尾辞）
    long long lcp_of(int i, int j) const {
        if (i == n || j == n) return 0;
        if (i == j) return n - i;
        int a = rnk[i], b = rnk[j];
        if (a > b) swap(a, b);
        int k = __lg(b - a);
        return min(mn[k][a], mn[k][b - (1 << k)]);  // ★ 順位 a と b の間の lcp の最小
    }
    // compare(l1, r1, l2, r2) : 2 つの区間 [l1, r1), [l2, r2)
    long long compare(int l1, int r1, int l2, int r2) const {
        long long len1 = r1 - l1, len2 = r2 - l2;
        long long c = min({lcp_of(l1, l2), len1, len2});
        if (c == min(len1, len2)) return len1 < len2 ? -1 : (len1 > len2 ? 1 : 0);  // 一方がもう一方の接頭辞
        return s[l1 + c] < s[l2 + c] ? -1 : 1;  // ★ 最初に違う文字で決まる
    }
    // count_distinct() : N(N+1)/2 − Σ lcp
    long long count_distinct() const { return cum[n]; }
    // longest_repeated() : 同じ長さなら辞書順で最小のもの
    pair<long long, long long> longest_repeated() const {
        int best = 0, st = 0;
        for (int i = 0; i + 1 < n; i++)
            if (lcp[i] > best) best = lcp[i], st = sa[i];
        return {st, best};
    }
    // kth_substring(k) : k が count_distinct() 以上なら {−1, −1}
    pair<long long, long long> kth_substring(long long k) const {
        if (k < 0 || k >= cum[n]) return {-1, -1};
        int i = upper_bound(cum.begin(), cum.end(), k) - cum.begin() - 1;  // ★ sa[i] の接尾辞が生む部分文字列の中にある
        return {sa[i], (i ? lcp[i - 1] : 0) + 1 + int(k - cum[i])};        // 長さ lcp[i−1]+1, +2, ... の順に並ぶ
    }
    // find(t) : t は s と同じ型。t が空なら [0, N)
    pair<long long, long long> find(const Seq& t) const {
        int m = t.size();
        auto cmp = [&](int i) {  // s[sa[i]..) の先頭 m 文字と t の比較（−1, 0, 1）
            int p = sa[i];
            for (int j = 0; j < m; j++) {
                if (p + j == n) return -1;  // 接尾辞の方が短い = 小さい
                if (s[p + j] != t[j]) return s[p + j] < t[j] ? -1 : 1;
            }
            return 0;
        };
        int lo = 0, hi = n;  // 最初の cmp >= 0
        while (lo < hi) { int mid = (lo + hi) / 2; if (cmp(mid) < 0) lo = mid + 1; else hi = mid; }
        int L = lo;
        hi = n;              // 最初の cmp > 0
        while (lo < hi) { int mid = (lo + hi) / 2; if (cmp(mid) <= 0) lo = mid + 1; else hi = mid; }
        return {L, lo};
    }
};

// ---------- 削除可: 最長共通部分文字列 ----------
// ◆ longest_common_substring(s, t) : s と t の両方に現れる最長の部分文字列 {s での開始位置, t での開始位置, 長さ}。O((N + M) log)
//   s, t : 同じ型の string か vector（vector の値に LLONG_MIN があってはいけない）。共通部分がなければ長さ 0
//   ★ s + 区切り + t の接尾辞配列で、隣どうしが「s 側と t 側」になっている所の lcp の最大
template <class Seq>
tuple<long long, long long, long long> longest_common_substring(const Seq& s, const Seq& t) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/suffix_array.cpp
    int n = s.size();
    vector<long long> u;
    for (auto& x : s) u.push_back((long long)x);
    u.push_back(LLONG_MIN);  // どちらにも出てこない区切り（lcp がここをまたがない）
    for (auto& x : t) u.push_back((long long)x);
    auto sa = atcoder::suffix_array(u);
    auto lcp = atcoder::lcp_array(u, sa);
    int best = 0, ps = 0, pt = 0;
    for (int i = 0; i + 1 < (int)u.size(); i++) {
        int a = sa[i], b = sa[i + 1];
        if ((a < n) == (b < n) || lcp[i] <= best) continue;  // ★ 片方が s 側、もう片方が t 側の組だけ
        best = lcp[i], ps = min(a, b), pt = max(a, b) - n - 1;
    }
    return {ps, pt, best};
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // ALPC I のサンプル
    assert(SuffixArray(string("abcbcba")).count_distinct() == 21);
    assert(SuffixArray(string("mississippi")).count_distinct() == 53);
    assert(SuffixArray(string("ababacaca")).count_distinct() == 33);
    assert(SuffixArray(string("aaaaa")).count_distinct() == 5);
    {
        SuffixArray sa(string("banana"));
        assert((sa.sa == vector<int>{5, 3, 1, 0, 4, 2}));
        assert(sa.lcp_of(1, 3) == 3 && sa.lcp_of(0, 1) == 0 && sa.lcp_of(2, 2) == 4 && sa.lcp_of(6, 1) == 0);
        assert((sa.longest_repeated() == pair<long long, long long>(3, 3)));  // "ana"（位置 1 と 3 に現れる。返すのは sa で先に来る方）
        auto [lo, hi] = sa.find(string("an"));
        assert(hi - lo == 2);
        assert((sa.kth_substring(0) == pair<long long, long long>(5, 1)));    // "a"
        assert(sa.compare(1, 3, 3, 5) == 0 && sa.compare(0, 1, 1, 2) == 1 && sa.compare(1, 2, 1, 4) == -1);
    }
    {
        auto [ps, pt, len] = longest_common_substring(string("xabcdy"), string("zzbcdab"));
        assert(len == 3 && ps == 2 && pt == 2);
        assert(get<2>(longest_common_substring(string("ab"), string("cd"))) == 0);
    }
    assert((SuffixArray(vector<int>{3, 1, 3, 1}).count_distinct() == 7));
    assert(SuffixArray(string("")).count_distinct() == 0);

    // ランダムテスト: 愚直と比較
    mt19937 rng(334);
    for (int it = 0; it < 1500; it++) {
        int n = rng() % 12, sig = rng() % 3 + 1;
        string s;
        for (int i = 0; i < n; i++) s += char('a' + rng() % sig);
        SuffixArray sa(s);
        set<string> subs;
        for (int l = 0; l < n; l++)
            for (int r = l + 1; r <= n; r++) subs.insert(s.substr(l, r - l));
        assert(sa.count_distinct() == (long long)subs.size());
        vector<string> v(subs.begin(), subs.end());
        for (int k = 0; k <= (int)v.size(); k++) {
            auto [p, len] = sa.kth_substring(k);
            if (k == (int)v.size()) assert(p == -1);
            else assert(s.substr(p, len) == v[k]);
        }
        // longest_repeated: 2 回以上（重なりも可）現れる最長
        {
            int best = 0;
            string bs;
            for (auto& u : v) {
                int c = 0;
                for (int i = 0; i + (int)u.size() <= n; i++) c += s.compare(i, u.size(), u) == 0;
                if (c >= 2 && ((int)u.size() > best)) best = u.size(), bs = u;
            }
            auto [st, len] = sa.longest_repeated();
            assert(len == best && s.substr(st, len) == bs);
        }
        for (int i = 0; i <= n; i++)
            for (int j = 0; j <= n; j++) {
                int c = 0;
                while (i + c < n && j + c < n && s[i + c] == s[j + c]) c++;
                assert(sa.lcp_of(i, j) == c);
            }
        for (int q = 0; q < 20 && n > 0; q++) {
            int l1 = rng() % (n + 1), r1 = l1 + rng() % (n - l1 + 1), l2 = rng() % (n + 1), r2 = l2 + rng() % (n - l2 + 1);
            string a = s.substr(l1, r1 - l1), b = s.substr(l2, r2 - l2);
            assert(sa.compare(l1, r1, l2, r2) == (a < b ? -1 : (a > b ? 1 : 0)));
            string t;
            for (int j = rng() % 4; j > 0; j--) t += char('a' + rng() % sig);
            auto [lo, hi] = sa.find(t);
            vector<int> pos, ref;
            for (int i = lo; i < hi; i++) pos.push_back(sa.sa[i]);
            sort(pos.begin(), pos.end());
            for (int i = 0; i + (int)t.size() <= n; i++) if (s.compare(i, t.size(), t) == 0) ref.push_back(i);
            if (t.empty()) ref.pop_back();  // 空の t は位置 n（空の接尾辞）を数えない
            assert(pos == ref);
        }
        // 最長共通部分文字列
        {
            string t;
            for (int j = rng() % 10; j > 0; j--) t += char('a' + rng() % sig);
            int best = 0;
            for (int i = 0; i < n; i++)
                for (int j = 0; j < (int)t.size(); j++) {
                    int c = 0;
                    while (i + c < n && j + c < (int)t.size() && s[i + c] == t[j + c]) c++;
                    best = max(best, c);
                }
            auto [ps, pt, len] = longest_common_substring(s, t);
            assert(len == best && s.substr(ps, len) == t.substr(pt, len));
        }
    }
    puts("OK");
}
