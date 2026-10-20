// =====================================================================
// string/kmp.cpp  KMP 法（パターン検索・周期）
//   文字列 t の中のパターン p の出現位置をすべて O(|t| + |p|) で求める
//   失敗関数（prefix function）から、文字列の最小周期もわかる
//   削除可: KMP オートマトン（「パターンを含まない長さ N の文字列の数」などの DP の状態遷移）
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
// ◆ prefix_function(s) : 失敗関数。pi[i] = s[0..i] の「接頭辞かつ接尾辞」の最長の長さ（全体は除く）。O(N)
//   s : string か vector
template <class Seq>
vector<long long> prefix_function(const Seq& s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/kmp.cpp
    int n = s.size();
    vector<long long> pi(n, 0);
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
// ◆ kmp_search(t, p) : t の中で p が現れる開始位置の一覧（重なりも含む）。O(|t| + |p|)
//   t : 探される側の文字列、p : パターン（空なら空の結果）。t と p は同じ型
template <class Seq>
vector<long long> kmp_search(const Seq& t, const Seq& p) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/kmp.cpp
    vector<long long> res;
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
// ◆ min_period(s) : 最小周期 d（全部の i で s[i] = s[i + d]）。空なら 0
//   s : string か vector。「繰り返しでできているか」は n % d == 0 && d < n で判定
template <class Seq>
long long min_period(const Seq& s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/kmp.cpp
    if (s.empty()) return 0;
    return s.size() - prefix_function(s).back();  // ★ 全体の「接頭辞かつ接尾辞」を除いた長さが周期
}

// ---------- 削除可: KMP オートマトン（本体が必要） ----------
// 状態 k = 「今読んだ文字列の末尾が、p の先頭 k 文字と一致している最長の k」（0..m、m = |p|）
// 文字列を 1 文字ずつ読みながら状態を持つ DP で、「p を含む・含まない」「p が何回現れるか」を数えられる
// パターンが複数あるなら aho_corasick.cpp
// ◆ kmp_automaton(p, sigma, base) : nx[k][c] = 状態 k で文字 base + c を読んだ後の状態。O(m × sigma)
//   p     : パターン（1 文字以上）
//   sigma : 文字の種類数（既定 26）、base : 最初の文字（既定 'a'）
//   状態 m（p を読み終えた）からの遷移もある（重なりを含めて出現回数を数えるとき用）
vector<vector<long long>> kmp_automaton(const string& p, int sigma = 26, char base = 'a') {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/kmp.cpp
    int m = p.size();
    auto pi = prefix_function(p);
    vector<vector<long long>> nx(m + 1, vector<long long>(sigma));
    for (int k = 0; k <= m; k++)
        for (int c = 0; c < sigma; c++) {
            if (k < m && p[k] - base == c) nx[k][c] = k + 1;  // ★ 一致が 1 文字伸びる
            else nx[k][c] = (k == 0) ? 0 : nx[pi[k - 1]][c];  // ★ 合わなければ失敗関数で戻った状態と同じ（計算済み）
        }
    return nx;
}

// ◆ count_avoid<T>(p, N, sigma, base) : sigma 種類の文字からなる長さ N の文字列で、p を部分文字列として含まないものの数。O(N m sigma)
//   T : 数の型（mint 推奨。必ず書く）
//   含むものの数は sigma^N − count_avoid
template <class T>
T count_avoid(const string& p, long long N, int sigma = 26, char base = 'a') {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/kmp.cpp
    int m = p.size();
    auto nx = kmp_automaton(p, sigma, base);
    vector<T> dp(m, T(0));  // dp[k] = 状態 k にいる文字列の数（状態 m = p を含んだ、は捨てる）
    dp[0] = T(1);
    for (long long i = 0; i < N; i++) {
        vector<T> nd(m, T(0));
        for (int k = 0; k < m; k++)
            for (int c = 0; c < sigma; c++)
                if (nx[k][c] < m) nd[nx[k][c]] += dp[k];  // ★ 状態 m に行く遷移は使わない
        dp.swap(nd);
    }
    T res = T(0);
    for (auto& x : dp) res += x;
    return res;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    assert((prefix_function(string("abcabd")) == vector<long long>{0, 0, 0, 1, 2, 0}));
    assert((kmp_search(string("abababa"), string("aba")) == vector<long long>{0, 2, 4}));
    assert(kmp_search(string("abc"), string("d")).empty());
    assert(min_period(string("abcabcab")) == 3 && min_period(string("aaaa")) == 1 && min_period(string("abcd")) == 4);
    // 数列でも使える
    assert((kmp_search(vector<int>{1, 2, 1, 2, 1}, vector<int>{1, 2, 1}) == vector<long long>{0, 2}));

    // ランダムテスト: 愚直な検索と比較
    mt19937 rng(14);
    for (int it = 0; it < 3000; it++) {
        string t, p;
        for (int i = rng() % 20; i > 0; i--) t += char('a' + rng() % 2);
        for (int i = rng() % 4 + 1; i > 0; i--) p += char('a' + rng() % 2);
        vector<long long> ref;
        for (int i = 0; i + (int)p.size() <= (int)t.size(); i++) if (t.compare(i, p.size(), p) == 0) ref.push_back(i);
        assert(kmp_search(t, p) == ref);
        if (!t.empty()) {
            int d = 1;
            while (true) { bool ok = true; for (int i = 0; i + d < (int)t.size(); i++) if (t[i] != t[i + d]) ok = false; if (ok) break; d++; }
            assert(min_period(t) == d);
        }
    }
    // KMP オートマトン: 状態を持って読むと kmp_search と同じ回数だけ状態 m に来る
    {
        auto nx = kmp_automaton("aba", 2);
        assert((nx[3] == vector<long long>{1, 2}));  // "aba" の後に a → "a"（1）、b → "ab"（2）
        assert(count_avoid<long long>("aa", 3, 2) == 5);  // aba, abb, bab, bba, bbb
        assert(count_avoid<long long>("a", 4, 3) == 16);  // 2^4
    }
    for (int it = 0; it < 1500; it++) {
        int sig = rng() % 3 + 1;
        string p;
        for (int i = rng() % 4 + 1; i > 0; i--) p += char('a' + rng() % sig);
        auto nx = kmp_automaton(p, sig);
        string t;
        for (int i = rng() % 15; i > 0; i--) t += char('a' + rng() % sig);
        int k = 0, hits = 0;
        for (int i = 0; i < (int)t.size(); i++) {
            k = nx[k][t[i] - 'a'];
            // 状態 = 末尾と p の接頭辞の一致の最長（愚直に確かめる）
            int ref = 0;
            string u = t.substr(0, i + 1);
            for (int len = min(u.size(), p.size()); len > 0; len--)
                if (u.compare(u.size() - len, len, p, 0, len) == 0) { ref = len; break; }
            assert(k == ref);
            hits += k == (int)p.size();
        }
        assert(hits == (int)kmp_search(t, p).size());
        // count_avoid: 長さ N の文字列を全部試す
        int N = rng() % 7;
        long long cnt = 0, tot = 1;
        for (int j = 0; j < N; j++) tot *= sig;
        for (long long x = 0; x < tot; x++) {
            string w(N, 'a');
            for (int j = 0, y = x; j < N; j++, y /= sig) w[j] = 'a' + y % sig;
            cnt += w.find(p) == string::npos;
        }
        assert(count_avoid<long long>(p, N, sig) == cnt);
    }
    puts("OK");
}
