// =====================================================================
// string/trie.cpp  Trie（トライ木）
//   文字列を木に登録し、共通の接頭辞を 1 本の道にまとめる。登録・検索とも O(文字列の長さ)
//   → 「接頭辞が p の文字列の個数」「他の文字列との最長共通接頭辞」「全ペアの最長共通接頭辞の和」
// 依存: なし
// ABC / ARC での使用例:
//   ABC287 E - Karuta  https://atcoder.jp/contests/abc287/tasks/abc287_e
//     → 全部 insert してから、各文字列の lcp_with_others
//   ABC353 E - Yet Another Sigma Problem  https://atcoder.jp/contests/abc353/tasks/abc353_e
//     → 全部 insert して sum_lcp_pairs
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// K, BASE : 文字の種類数と最初の文字（英小文字なら省略で 26, 'a'。英大文字なら Trie<26, 'A'>、数字なら Trie<10, '0'>）
// Trie<> tr;
//   tr.insert(s)           s を登録（同じ文字列を何回でも登録できる）。s の終わりの頂点番号を返す
//   tr.erase(s)            s を 1 つ削除（登録されていなければ何もせず false）
//   tr.count(s)            s が登録されている個数
//   tr.count_prefix(p)     p で始まる登録済みの文字列の個数（重複も数える）
//   tr.nodes[v]            頂点 v の情報 { nxt[c]: 子（なければ -1）, pass: v を通る文字列の数, end: v で終わる数, depth }
// ◆ struct Trie : 文字列の集合を木で持つ（接頭辞の個数などを O(|s|) で）
//   insert(s)          : s を登録（同じ文字列を何回でも）。s の終わりの頂点番号を返す
//   erase(s)           : s を 1 つ削除（登録されていなければ false）
//   find(s)            : s に対応する頂点番号（なければ −1）
//   count(s)           : s が登録されている個数
//   count_prefix(p)    : p で始まる登録済みの文字列の個数
//   lcp_with_others(s) : s と他の登録済み文字列との最長共通接頭辞の長さ
//   sum_lcp_pairs()    : 全ペアの最長共通接頭辞の和
//   K, BASE : 文字の種類数と最初の文字（既定 26, 'a'）。範囲外の文字を入れると壊れる
template <int K = 26, char BASE = 'a'>
struct Trie {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/trie.cpp
    struct Node {
        array<int, K> nxt;
        int pass = 0, end = 0, depth = 0;
        Node(int d = 0) : depth(d) { nxt.fill(-1); }
    };
    vector<Node> nodes = {Node(0)};  // 頂点 0 が根（空文字列）

    // insert(s) : s : 登録する文字列（空文字列もよい）
    int insert(const string& s) {
        int v = 0;
        nodes[0].pass++;
        for (char ch : s) {
            int c = ch - BASE;
            if (nodes[v].nxt[c] == -1) {  // ★ 子がなければ作る
                nodes[v].nxt[c] = nodes.size();
                nodes.push_back(Node(nodes[v].depth + 1));
            }
            v = nodes[v].nxt[c];
            nodes[v].pass++;  // ★ 通った頂点の数を数えておく → 接頭辞の個数になる
        }
        nodes[v].end++;
        return v;
    }
    // find(s) : s : 調べる文字列
    int find(const string& s) const {  // s の頂点番号（なければ -1）
        int v = 0;
        for (char ch : s) {
            v = nodes[v].nxt[ch - BASE];
            if (v == -1) return -1;
        }
        return v;
    }
    // count(s) : s が登録されている個数
    int count(const string& s) const { int v = find(s); return v == -1 ? 0 : nodes[v].end; }
    // count_prefix(p) : p : 接頭辞（空なら登録数の合計）
    int count_prefix(const string& p) const { int v = find(p); return v == -1 ? 0 : nodes[v].pass; }
    // erase(s) : s : 消す文字列（1 つだけ消す）
    bool erase(const string& s) {
        if (count(s) == 0) return false;
        int v = 0;
        nodes[0].pass--;
        for (char ch : s) v = nodes[v].nxt[ch - BASE], nodes[v].pass--;
        nodes[v].end--;
        return true;
    }

    // --- 削除可: 他の文字列との最長共通接頭辞（s は登録済みであること） ---
    // 返り値: s と、s 以外の登録済み文字列（同じ文字列がもう 1 つあればそれも）との最長共通接頭辞の長さ
    // lcp_with_others(s) : s は登録済みであること
    int lcp_with_others(const string& s) const {
        int v = 0, res = 0;
        for (char ch : s) {
            v = nodes[v].nxt[ch - BASE];
            if (nodes[v].pass >= 2) res = nodes[v].depth;  // ★ 2 本以上通っている = 他の文字列もここまで同じ
        }
        return res;
    }
    // --- 削除可: 全ペアの最長共通接頭辞の和 Σ_{i<j} LCP(s_i, s_j) ---
    // sum_lcp_pairs() : Σ_{i<j} LCP(s_i, s_j)
    long long sum_lcp_pairs() const {
        long long res = 0;
        for (size_t v = 1; v < nodes.size(); v++) res += (long long)nodes[v].pass * (nodes[v].pass - 1) / 2;  // ★ 各頂点で、そこを通るペアの数
        return res;
    }
    // --- 削除可ここまで ---
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    Trie<> tr;
    for (string s : {"abc", "abd", "ab", "b", "abc"}) tr.insert(s);
    // "ab" で始まるのは abc, abd, ab, abc の 4 個。"abc" は 2 個登録
    assert(tr.count_prefix("ab") == 4 && tr.count("abc") == 2 && tr.count("a") == 0 && tr.count_prefix("") == 5);
    // abd と他との最長共通接頭辞は "ab" の 2、b は 0、abc は同じ abc がもう 1 つあるので 3
    assert(tr.lcp_with_others("abd") == 2 && tr.lcp_with_others("b") == 0 && tr.lcp_with_others("abc") == 3);
    assert(tr.erase("abc") && tr.count("abc") == 1 && !tr.erase("zz"));

    // ランダムテスト: 文字列の多重集合を持って愚直に計算したものと比較
    mt19937 rr(125);
    auto lcp = [](const string& a, const string& b) { size_t k = 0; while (k < a.size() && k < b.size() && a[k] == b[k]) k++; return (int)k; };
    for (int it = 0; it < 500; it++) {
        Trie<3, 'a'> t;
        multiset<string> ms;
        auto rnd = [&]() { string s; for (int i = rr() % 5; i > 0; i--) s += char('a' + rr() % 3); return s; };
        for (int q = 0; q < 40; q++) {
            string s = rnd();
            int op = rr() % 3;
            if (op == 0) t.insert(s), ms.insert(s);
            else if (op == 1) { bool had = ms.count(s) > 0; assert(t.erase(s) == had); if (had) ms.erase(ms.find(s)); }
            int pre = 0;
            for (auto& x : ms) pre += x.compare(0, s.size(), s) == 0;
            assert(t.count(s) == (int)ms.count(s) && t.count_prefix(s) == pre);
            if (ms.count(s)) {
                int best = 0;
                bool skipped = false;  // s 自身を 1 つだけ除く
                for (auto& x : ms) { if (x == s && !skipped) { skipped = true; continue; } best = max(best, lcp(x, s)); }
                assert(t.lcp_with_others(s) == best);
            }
        }
        vector<string> v(ms.begin(), ms.end());
        long long sum = 0;
        for (size_t i = 0; i < v.size(); i++) for (size_t j = i + 1; j < v.size(); j++) sum += lcp(v[i], v[j]);
        assert(t.sum_lcp_pairs() == sum);
    }
    puts("OK");
}
