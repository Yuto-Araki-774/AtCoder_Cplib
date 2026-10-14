// =====================================================================
// string/kmp.cpp  KMP 法（パターン検索・周期）
//   文字列 t の中のパターン p の出現位置をすべて O(|t| + |p|) で求める
//   失敗関数（prefix function）から、文字列の最小周期もわかる
//   ※ Z-algorithm は ACL の z_algorithm（atcoder/string）にある。ハッシュで比べたいなら rolling_hash.cpp
// 依存: なし
// ABC / ARC での使用例:
//   ABC150 F - Xor Shift  https://atcoder.jp/contests/abc150/tasks/abc150_f
//     → 隣り合う要素の xor の列に変換し、b の列を a の列 2 周分の中から kmp_search で探す
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体: 失敗関数 ----------
// 返り値: pi[i] = s[0..i] の「接頭辞かつ接尾辞」になる最長の長さ（全体は除く）
// 例: s = "abcabd" → pi = {0, 0, 0, 1, 2, 0}
// string でも vector でも使える
template <class Seq>
vector<int> prefix_function(const Seq& s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/kmp.cpp
    int n = s.size();
    vector<int> pi(n, 0);
    for (int i = 1; i < n; i++) {
        int k = pi[i - 1];                             // ★ 1 つ前で一致していた長さから始める
        while (k > 0 && s[i] != s[k]) k = pi[k - 1];   // ★ 合わなければ、より短い「接頭辞かつ接尾辞」に戻る
        if (s[i] == s[k]) k++;
        pi[i] = k;
    }
    return pi;
}

// ---------- 削除可: パターン検索（本体が必要） ----------
// 返り値: t の中で p が始まる位置の一覧（重なりも含む）
template <class Seq>
vector<int> kmp_search(const Seq& t, const Seq& p) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/kmp.cpp
    vector<int> res;
    if (p.empty()) return res;
    auto pi = prefix_function(p);
    int k = 0;  // 今 p の先頭 k 文字が一致している
    for (int i = 0; i < (int)t.size(); i++) {
        while (k > 0 && t[i] != p[k]) k = pi[k - 1];  // ★ t を戻らずに、p の方だけずらす
        if (t[i] == p[k]) k++;
        if (k == (int)p.size()) res.push_back(i - k + 1), k = pi[k - 1];  // ★ 全部一致 → 次の候補へ
    }
    return res;
}

// ---------- 削除可: 最小周期（本体が必要） ----------
// 返り値: s[i] = s[i + d] がすべての i で成り立つ最小の d（d が |s| を割り切るとは限らない）
// 「s がある文字列の繰り返しか」は (n % d == 0 && d < n) で判定できる
template <class Seq>
int min_period(const Seq& s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/kmp.cpp
    if (s.empty()) return 0;
    return s.size() - prefix_function(s).back();  // ★ 全体の「接頭辞かつ接尾辞」を除いた長さが周期
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    assert((prefix_function(string("abcabd")) == vector<int>{0, 0, 0, 1, 2, 0}));
    assert((kmp_search(string("abababa"), string("aba")) == vector<int>{0, 2, 4}));
    assert(kmp_search(string("abc"), string("d")).empty());
    assert(min_period(string("abcabcab")) == 3 && min_period(string("aaaa")) == 1 && min_period(string("abcd")) == 4);
    // 数列でも使える
    assert((kmp_search(vector<int>{1, 2, 1, 2, 1}, vector<int>{1, 2, 1}) == vector<int>{0, 2}));

    // ランダムテスト: 愚直な検索と比較
    mt19937 rng(14);
    for (int it = 0; it < 3000; it++) {
        string t, p;
        for (int i = rng() % 20; i > 0; i--) t += char('a' + rng() % 2);
        for (int i = rng() % 4 + 1; i > 0; i--) p += char('a' + rng() % 2);
        vector<int> ref;
        for (int i = 0; i + (int)p.size() <= (int)t.size(); i++) if (t.compare(i, p.size(), p) == 0) ref.push_back(i);
        assert(kmp_search(t, p) == ref);
        if (!t.empty()) {
            int d = 1;
            while (true) { bool ok = true; for (int i = 0; i + d < (int)t.size(); i++) if (t[i] != t[i + d]) ok = false; if (ok) break; d++; }
            assert(min_period(t) == d);
        }
    }
    puts("OK");
}
