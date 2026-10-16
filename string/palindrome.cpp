// =====================================================================
// string/palindrome.cpp  回文の小物（ハッシュも Manacher も使わないもの）
//   本体A: 書き換えて回文にする最小回数、並べ替えて回文にできるか O(N)
//   本体B: 最長回文部分列（飛び飛びに選んでよい）の長さ O(N^2)、メモリ O(N)
//          → 「最小何文字消せば回文か」「最小何文字挿入すれば回文か」はどちらも N − 最長回文部分列
//   A, B は互いに独立
//   ★ 回文の数え上げ（「長さ N の回文で〇〇」）は、前半分 ceil(N/2) 文字を決めれば全体が決まる
//   部分文字列（連続）が回文か・最長回文・回文分割は manacher.cpp
// 依存: なし
// ABC / ARC での使用例:
//   ABC147 B - Palindrome-philia  https://atcoder.jp/contests/abc147/tasks/abc147_b
//     → pal_min_changes(s) がそのまま答え
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 書き換え・並べ替え ----------
// ◆ pal_min_changes(s) : 1 文字ずつ書き換えて回文にするときの最小回数。O(N)
//   s : string か vector
template <class Seq>
int pal_min_changes(const Seq& s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/palindrome.cpp
    int n = s.size(), c = 0;
    for (int i = 0; i < n / 2; i++) c += s[i] != s[n - 1 - i];  // ★ 対応する 2 文字が違えば、どちらかを 1 回書き換える
    return c;
}

// ◆ can_rearrange_pal(s) : 並べ替えて回文にできるか。O(N log N)
//   s : string か vector
//   ★ 奇数回出てくる値が 1 種類以下 ⇔ できる（真ん中に置けるのは 1 文字だけ）
template <class Seq>
bool can_rearrange_pal(const Seq& s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/palindrome.cpp
    map<typename Seq::value_type, int> cnt;
    for (auto& x : s) cnt[x]++;
    int odd = 0;
    for (auto& [x, c] : cnt) odd += c % 2;
    return odd <= 1;
}

// ---------- 本体B: 最長回文部分列 ----------
// ◆ longest_pal_subseq(s) : 回文になる部分列（飛び飛びでよい）の最長の長さ。O(N^2)（N <= 5000 程度）
//   s : string か vector
//   最小何文字消せば回文か = 最小何文字挿入すれば回文か = N − longest_pal_subseq(s)
template <class Seq>
int longest_pal_subseq(const Seq& s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/palindrome.cpp
    int n = s.size();
    if (n == 0) return 0;
    // dp[l][r] = s[l..r]（閉区間）の最長回文部分列。l を後ろから回し、1 行（prv = dp[l+1][*]）だけ持つ
    vector<int> prv(n, 0), cur(n, 0);
    for (int l = n - 1; l >= 0; l--) {
        cur.assign(n, 0);
        cur[l] = 1;
        for (int r = l + 1; r < n; r++) {
            if (s[l] == s[r]) cur[r] = prv[r - 1] + 2;  // ★ 両端が同じなら両方使う（prv[l] = 空の区間 = 0）
            else cur[r] = max(prv[r], cur[r - 1]);      // ★ 違えば、左端か右端のどちらかを使わない
        }
        swap(prv, cur);
    }
    return prv[n - 1];
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // ABC147 B のサンプル
    assert(pal_min_changes(string("redcoder")) == 1 && pal_min_changes(string("vvvvvv")) == 0 && pal_min_changes(string("abcdabc")) == 2);
    assert(can_rearrange_pal(string("aabbc")) && can_rearrange_pal(string("")) && !can_rearrange_pal(string("aabc")));
    assert((can_rearrange_pal(vector<int>{1, 2, 2, 1})));
    assert(longest_pal_subseq(string("bbbab")) == 4);  // bbbb
    assert(longest_pal_subseq(string("abcde")) == 1 && longest_pal_subseq(string("")) == 0);
    assert(longest_pal_subseq(string("character")) == 5);  // carac

    // ランダムテスト: 愚直と比較
    mt19937 rng(333);
    auto ispal = [](const string& u) { return equal(u.begin(), u.end(), u.rbegin()); };
    for (int it = 0; it < 2000; it++) {
        int n = rng() % 12, sig = rng() % 3 + 1;
        string s;
        for (int i = 0; i < n; i++) s += char('a' + rng() % sig);
        // 最長回文部分列: 全部の部分列を試す
        int best = 0;
        for (int mask = 0; mask < (1 << n); mask++) {
            string u;
            for (int i = 0; i < n; i++) if (mask >> i & 1) u += s[i];
            if (ispal(u)) best = max(best, (int)u.size());
        }
        assert(longest_pal_subseq(s) == best);
        // 並べ替え: n が小さいとき全順列を試す
        if (n <= 7) {
            string u = s;
            sort(u.begin(), u.end());
            bool ok = false;
            do ok |= ispal(u);
            while (next_permutation(u.begin(), u.end()));
            assert(can_rearrange_pal(s) == ok);
        }
        // 書き換え: 回文になるまでの最小回数（BFS 代わりに、全部の回文 t (文字は s に出るもの + 'z') との違いの最小）
        if (n <= 8) {
            int h = (n + 1) / 2, mn = INT_MAX;
            const string al = "abcz";
            int tot = 1;
            for (int j = 0; j < h; j++) tot *= 4;
            for (int x = 0; x < tot; x++) {
                string t(n, 'a');
                for (int j = 0, y = x; j < h; j++, y /= 4) t[j] = t[n - 1 - j] = al[y % 4];
                int d = 0;
                for (int j = 0; j < n; j++) d += s[j] != t[j];
                mn = min(mn, d);
            }
            assert(pal_min_changes(s) == mn);
        }
    }
    puts("OK");
}
