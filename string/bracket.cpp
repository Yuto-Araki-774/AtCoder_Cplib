// =====================================================================
// string/bracket.cpp  括弧列
//   本体A: 正しい括弧列か（何種類の括弧でも）、足りない括弧を最小個数で補う方法
//   本体B: 最長の正しい括弧列になっている部分文字列
//   本体C: 区間 s[l..r) が正しい括弧列か O(1)（累積和 + Sparse Table）
//   本体D: '?' を '(' か ')' にして正しい括弧列になる置き方の数 O(N^2)
//   A〜D は互いに独立（使うものだけコピーすればよい）
//   ★ 括弧列の基本: '(' を +1、')' を −1 とした累積和が「途中で一度も負にならず、最後に 0」⇔ 正しい括弧列
//   長さ 2n の正しい括弧列の個数はカタラン数 C(2n, n) / (n + 1)（math/combination.cpp）
// 依存: なし
// ABC / ARC での使用例:
//   ABC394 D - Colorful Bracket Sequence  https://atcoder.jp/contests/abc394/tasks/abc394_d
//     → is_balanced(s, "([<", ")]>") がそのまま答え
//   ABC064 D - Insertion  https://atcoder.jp/contests/abc064/tasks/abc064_d
//     → bracket_fix(s) = {a, b} で、'(' を a 個 + s + ')' を b 個
//   ABC312 D - Count Bracket Sequences  https://atcoder.jp/contests/abc312/tasks/abc312_d
//     → count_bracket_fill<atcoder::modint998244353>(s)
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 判定・最小の補い方 ----------
// ◆ is_balanced(s, open, close) : s が正しい括弧列か（スタック）。O(N)
//   s     : 調べる文字列。open にも close にもない文字は無視する
//   open  : 開き括弧の一覧（既定 "("）。close[i] が open[i] に対応する閉じ括弧
//   close : 閉じ括弧の一覧（既定 ")"）。例: is_balanced(s, "([<", ")]>")
//   注意 : open と close に同じ文字を入れない（"|" のように開きと閉じが同じ文字の括弧は扱えない）
bool is_balanced(const string& s, const string& open = "(", const string& close = ")") {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/bracket.cpp
    vector<int> st;  // まだ閉じていない開き括弧の種類
    for (char c : s) {
        size_t o = open.find(c), k = close.find(c);
        if (o != string::npos) st.push_back(o);
        else if (k != string::npos) {
            if (st.empty() || st.back() != (int)k) return false;  // ★ 一番最後に開いた括弧と同じ種類でないとだめ
            st.pop_back();
        }
    }
    return st.empty();  // ★ 開いたままの括弧が残っていたらだめ
}

// ◆ bracket_fix(s, open, close) : 文字を足して正しい括弧列にするとき、最小で何個足すか {a, b}。O(N)
//   返り値 : 先頭に open を a 個、末尾に close を b 個足すと正しくなる（足す個数 a + b は最小）
//            この足し方は、最短のものの中で辞書順最小でもある（'(' < ')' なので）
//   s : 調べる文字列（open, close 以外の文字は無視）
pair<long long, long long> bracket_fix(const string& s, char open = '(', char close = ')') {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/bracket.cpp
    int bal = 0, need = 0;  // bal = 今開いている数、need = 相手のいなかった閉じ括弧の数
    for (char c : s) {
        if (c == open) bal++;
        else if (c == close) {
            if (bal == 0) need++;  // ★ 累積和が負になる所 = 前に開き括弧が足りない
            else bal--;
        }
    }
    return {need, bal};  // ★ 最後に開いたまま残った分を閉じる
}

// ---------- 本体B: 最長の正しい括弧列（部分文字列） ----------
// ◆ longest_balanced(s, open, close) : 正しい括弧列になっている部分文字列のうち最長のもの {開始位置, 長さ}。O(N)
//   s : 調べる文字列。open, close 以外の文字は「壁」（それをまたぐ部分文字列は数えない）
//   返り値 : 同じ長さが複数あれば一番左。1 つもなければ {0, 0}
pair<long long, long long> longest_balanced(const string& s, char open = '(', char close = ')') {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/bracket.cpp
    vector<int> st = {-1};  // ★ 一番下は「ここより右から始められる」という区切りの位置
    int best = 0, start = 0;
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == open) st.push_back(i);
        else if (s[i] == close) {
            st.pop_back();
            if (st.empty()) st.push_back(i);  // ★ 相手のいない閉じ括弧 → ここが新しい区切り
            else if (i - st.back() > best) best = i - st.back(), start = st.back() + 1;  // ★ 区切りの次から i までが正しい
        } else {
            st.assign(1, i);  // 括弧以外の文字は区切り
        }
    }
    return {start, best};
}

// ---------- 本体C: 区間が正しい括弧列か O(1) ----------
// ◆ struct BracketRange : 区間 s[l..r) が正しい括弧列かを O(1) で答える。前計算 O(N log N)
//   BracketRange(s, open, close) : 前計算（open, close 以外の文字は無視 = 0 として扱う）
//   ok(l, r)    : s[l..r) が正しい括弧列か（空の区間は true）
//   pre[i]      : s[0..i) の累積和（open は +1、close は −1）
//   注意 : 文字の書き換えがあるなら、{和, 累積和の最小} を持つ ACL の segtree を使う
struct BracketRange {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/bracket.cpp
    vector<int> pre;
    vector<vector<int>> mn;  // mn[k][i] = pre[i..i + 2^k) の最小（Sparse Table）
    // s : 括弧列、open / close : 開き・閉じ括弧の文字
    BracketRange(const string& s, char open = '(', char close = ')') {
        int n = s.size();
        pre.assign(n + 1, 0);
        for (int i = 0; i < n; i++) pre[i + 1] = pre[i] + (s[i] == open) - (s[i] == close);
        mn.push_back(pre);
        for (int k = 1; (1 << k) <= n + 1; k++) {
            mn.push_back(vector<int>(n + 2 - (1 << k)));
            for (int i = 0; i + (1 << k) <= n + 1; i++) mn[k][i] = min(mn[k - 1][i], mn[k - 1][i + (1 << (k - 1))]);
        }
    }
    // ok(l, r) : 0 <= l <= r <= N
    bool ok(int l, int r) const {
        if (pre[l] != pre[r]) return false;             // ★ 開きと閉じの数が同じ
        int k = __lg(r - l + 1);
        int m = min(mn[k][l], mn[k][r + 1 - (1 << k)]);  // pre[l..r] の最小
        return m >= pre[l];                             // ★ 途中で pre[l] を下回らない = 負にならない
    }
};

// ---------- 本体D: '?' の置き方の数 ----------
// ◆ count_bracket_fill<T>(s, open, close, any) : any の文字を open か close にして、正しい括弧列になる置き方の数。O(N^2)
//   T   : 数の型（mint 推奨。必ず書く）
//   s   : 文字列（N <= 5000 程度）。open, close, any 以外の文字は無視
//   any : どちらにもできる文字（既定 '?'）
template <class T>
T count_bracket_fill(const string& s, char open = '(', char close = ')', char any = '?') {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/bracket.cpp
    int n = s.size();
    vector<T> dp(n + 2, T(0));  // dp[h] = ここまでで累積和が h になる置き方の数（負になったものは捨てる）
    dp[0] = T(1);
    for (char c : s) {
        if (c != open && c != close && c != any) continue;
        vector<T> nd(n + 2, T(0));
        for (int h = 0; h <= n; h++) {
            if (c != close) nd[h + 1] += dp[h];             // open にする
            if (c != open && h > 0) nd[h - 1] += dp[h];     // ★ close にする（h = 0 からは負になるので不可）
        }
        dp.swap(nd);
    }
    return dp[0];  // ★ 最後に 0 に戻ったもの
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 手計算の例
    assert(is_balanced("(()())") && !is_balanced("())(") && !is_balanced("(") && is_balanced(""));
    assert(is_balanced("([])<>()", "([<", ")]>") && !is_balanced("([<)]>", "([<", ")]>") && !is_balanced("())", "([<", ")]>"));
    assert(is_balanced("(a(b)c)"));  // 括弧以外は無視
    assert((bracket_fix("())") == pair<long long, long long>(1, 0)));         // "(())"
    assert((bracket_fix(")))())") == pair<long long, long long>(4, 0)));      // "(((()))())"
    assert((bracket_fix("))))((((") == pair<long long, long long>(4, 4)));    // "(((())))(((())))"
    assert((longest_balanced(")()())") == pair<long long, long long>(1, 4)));
    assert((longest_balanced("(()") == pair<long long, long long>(1, 2)));
    assert((longest_balanced("((((") == pair<long long, long long>(0, 0)));
    assert((longest_balanced("()x(())") == pair<long long, long long>(3, 4)));
    {
        BracketRange br("(()())");
        assert(br.ok(0, 6) && br.ok(1, 3) && br.ok(1, 5) && !br.ok(0, 3) && !br.ok(2, 4) && br.ok(3, 3));
    }
    assert(count_bracket_fill<long long>("(??" "?(?") == 2);  // "(???(?"（?? と ( が続くとトライグラフの警告が出るので分けて書く）
    assert(count_bracket_fill<long long>(")))))") == 0);
    assert(count_bracket_fill<long long>("??????") == 5);  // カタラン数 C_3
    assert(count_bracket_fill<long long>("(x?") == 1);     // 括弧以外は無視

    // ランダムテスト: 愚直と比較
    mt19937 rng(331);
    auto valid = [](const string& t) {  // 累積和で判定（'(' ')' のみ数える）
        int h = 0;
        for (char c : t) {
            if (c == '(') h++;
            else if (c == ')') h--;
            if (h < 0) return false;
        }
        return h == 0;
    };
    for (int it = 0; it < 3000; it++) {
        int n = rng() % 13;
        string s;
        const string al = (it % 4 == 0) ? "()x" : "()";
        for (int i = 0; i < n; i++) s += al[rng() % al.size()];
        assert(is_balanced(s) == valid(s));
        // bracket_fix: 補った結果が正しい & 個数が最小（愚直: 足す個数 k を増やしながら、全部の挿入位置を試すのは重いので、
        //   「最小の足す数 = 相手のいない開き + 閉じの数」を、1 文字ずつ消していく愚直で確かめる）
        auto [a, b] = bracket_fix(s);
        assert(valid(string(a, '(') + s + string(b, ')')));
        {
            string t;  // () を繰り返し消すと、残りは ")))(((" の形。その長さが足す最小数
            for (char c : s) {
                if (c == 'x') continue;
                if (c == ')' && !t.empty() && t.back() == '(') t.pop_back();
                else t += c;
            }
            assert(a + b == (int)t.size());
        }
        // longest_balanced と BracketRange: 全区間を愚直に
        int best = 0, st = 0;
        BracketRange br(s);
        for (int l = 0; l <= n; l++)
            for (int r = l; r <= n; r++) {
                string u = s.substr(l, r - l);
                bool v = valid(u);
                assert(br.ok(l, r) == v);
                bool wall = u.find('x') != string::npos;
                if (v && !wall && r - l > best) best = r - l, st = l;
            }
        auto [ls, ll] = longest_balanced(s);
        assert(ll == best && (best == 0 || ls == st));
        // 複数種類の括弧: 愚直に「隣り合うペアを消す」を繰り返す
        {
            string u;
            const string al2 = "()[]";
            for (int i = 0; i < n; i++) u += al2[rng() % 4];
            string w = u;
            bool changed = true;
            while (changed) {
                changed = false;
                for (int i = 0; i + 1 < (int)w.size(); i++)
                    if ((w[i] == '(' && w[i + 1] == ')') || (w[i] == '[' && w[i + 1] == ']')) {
                        w.erase(i, 2), changed = true;
                        break;
                    }
            }
            assert(is_balanced(u, "([", ")]") == w.empty());
        }
        // count_bracket_fill: '?' の置き方を全部試す
        {
            string u;
            const string al3 = "()?";
            int m = rng() % 13;
            for (int i = 0; i < m; i++) u += al3[rng() % 3];
            vector<int> q;
            for (int i = 0; i < m; i++) if (u[i] == '?') q.push_back(i);
            long long cnt = 0;
            for (int mask = 0; mask < (1 << q.size()); mask++) {
                string w = u;
                for (int j = 0; j < (int)q.size(); j++) w[q[j]] = (mask >> j & 1) ? ')' : '(';
                cnt += valid(w);
            }
            assert(count_bracket_fill<long long>(u) == cnt);
        }
    }
    puts("OK");
}
