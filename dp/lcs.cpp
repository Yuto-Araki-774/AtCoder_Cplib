// =====================================================================
// dp/lcs.cpp  LCS（最長共通部分列）と編集距離
//   LCS      : 2 つの列から、順番を保ったまま共通して取り出せる最長の列（連続でなくてよい）
//   編集距離 : 1 文字の挿入・削除・置換を何回すれば一致するか（レーベンシュタイン距離）
//   どちらも O(|a| |b|)。string でも vector でも使える。2 つのブロックは互いに独立
// 依存: なし
// ABC / ARC での使用例:
//   EDPC F - LCS  https://atcoder.jp/contests/dp/tasks/dp_f
//     → LCS の復元（lcs）（ABC ではないが典型）
//   ABC185 E - Sequence Matching  https://atcoder.jp/contests/abc185/tasks/abc185_e
//     → 編集距離そのもの（edit_distance）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: LCS（復元付き） ----------
// 返り値: LCS そのもの（a と同じ型）。長さだけなら .size()
// 例: lcs(string("axyb"), string("abyxb")) → "axb" など（長さ 3。最長のものが 1 つ返る）
// ◆ lcs(a, b) : 最長共通部分列（LCS）を 1 つ返す。O(NM)、メモリ O(NM)
//   a, b : string か vector（同じ型）。N × M が 1e7〜1e8 程度まで
//   返り値 : LCS（a と同じ型）。長さだけなら .size()
template <class Seq>
Seq lcs(const Seq& a, const Seq& b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/lcs.cpp
    int n = a.size(), m = b.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));  // dp[i][j] = a の先頭 i 個と b の先頭 j 個の LCS の長さ
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            dp[i + 1][j + 1] = (a[i] == b[j]) ? dp[i][j] + 1                    // ★ 末尾が同じなら両方使う
                                              : max(dp[i][j + 1], dp[i + 1][j]);  // ★ 違えばどちらかの末尾を捨てる
    Seq res;
    for (int i = n, j = m; i > 0 && j > 0;) {  // ★ 復元: 表を右下から逆にたどる
        if (a[i - 1] == b[j - 1]) res.push_back(a[i - 1]), i--, j--;
        else if (dp[i][j] == dp[i - 1][j]) i--;
        else j--;
    }
    reverse(res.begin(), res.end());
    return res;
}

// ---------- 本体B: 編集距離 ----------
// ◆ edit_distance(a, b) : 編集距離（1 文字の挿入・削除・置換で a を b にする最小回数）。O(NM)
//   a, b : string か vector（同じ型）
template <class Seq>
long long edit_distance(const Seq& a, const Seq& b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/lcs.cpp
    int n = a.size(), m = b.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1));  // dp[i][j] = a の先頭 i 個を b の先頭 j 個にする最小回数
    for (int i = 0; i <= n; i++) dp[i][0] = i;  // 全部削除
    for (int j = 0; j <= m; j++) dp[0][j] = j;  // 全部挿入
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            dp[i + 1][j + 1] = min({dp[i][j + 1] + 1,                     // ★ a[i] を削除
                                    dp[i + 1][j] + 1,                     // ★ b[j] を挿入
                                    dp[i][j] + (a[i] == b[j] ? 0 : 1)});  // ★ 置換（同じ文字なら 0 回）
    return dp[n][m];
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
bool is_subseq(const string& s, const string& t) {  // s が t の部分列か
    size_t k = 0;
    for (char c : t) if (k < s.size() && s[k] == c) k++;
    return k == s.size();
}
int main() {
    string a = "axyb", b = "abyxb";
    string c = lcs(a, b);
    assert(c.size() == 3 && is_subseq(c, a) && is_subseq(c, b));
    assert(lcs(string("abc"), string("def")).empty());
    vector<int> x = {1, 3, 4, 1, 2}, y = {3, 1, 2, 4};
    assert(lcs(x, y).size() == 3);  // {3, 1, 2} など

    assert(edit_distance(string("kitten"), string("sitting")) == 3);
    assert(edit_distance(string(""), string("abc")) == 3);
    assert(edit_distance(string("abc"), string("abc")) == 0);

    // ランダムテスト: LCS の長さを全部分列で確認
    mt19937 rng(2);
    for (int it = 0; it < 500; it++) {
        int n = rng() % 8, m = rng() % 8;
        string s, t;
        for (int i = 0; i < n; i++) s += char('a' + rng() % 3);
        for (int i = 0; i < m; i++) t += char('a' + rng() % 3);
        int best = 0;
        for (int mask = 0; mask < (1 << n); mask++) {
            string sub;
            for (int i = 0; i < n; i++) if (mask >> i & 1) sub += s[i];
            if (is_subseq(sub, t)) best = max(best, (int)sub.size());
        }
        string r = lcs(s, t);
        assert((int)r.size() == best && is_subseq(r, s) && is_subseq(r, t));
        assert(edit_distance(s, t) >= max(n, m) - best && edit_distance(s, t) <= n + m - 2 * best);
    }
    puts("OK");
}
