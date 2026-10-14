// =====================================================================
// string/aho_corasick.cpp  Aho-Corasick（たくさんのパターン文字列をまとめて検索する）
//   パターンを Trie に入れ、「一致しなくなったらどこへ戻るか（fail）」を付けたオートマトンを作る
//   テキストを 1 文字ずつ読むだけで、全パターンの出現を O(|テキスト| + Σ|パターン| × 文字の種類) で数えられる
//   ★ もう 1 つの使い方: 状態（ノード番号）を DP の状態にして「どのパターンも含まない長さ N の文字列の数」などを数える
// 依存: なし
// ABC / ARC での使用例:
//   ABC268 Ex - Taboo  https://atcoder.jp/contests/abc268/tasks/abc268_h
//     → S を前から読み、cnt[v] > 0 の状態（どれかの T_i で終わった）に来たらその文字を * にして根に戻す貪欲
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// K : 文字の種類数、B : 最初の文字（英小文字なら 26, 'a'。大文字なら 26, 'A'。数字なら 10, '0'）
// AhoCorasick<26, 'a'> ac;
//   int id = ac.add(s);         パターンを追加（0, 1, 2, ... の番号が返る）
//   ac.build();                 全部追加してから 1 回呼ぶ
//   ac.count_each(t)            各パターンが t に何回出てくるか（重なりも数える）
//   ac.count_all(t)             全パターンの出現回数の合計
//   ac.next(v, c)               状態 v で文字 c（'a' など）を読んだ後の状態（DP 用。根は 0）
//   ac.cnt[v]                   状態 v に来たとき「ここで終わるパターン」の個数（fail 先の分も含む）
//   ac.size()                   状態の個数
template <int K = 26, char B = 'a'>
struct AhoCorasick {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/aho_corasick.cpp
    vector<array<int, K>> to;
    vector<int> fail, cnt, end_node, bfs;
    AhoCorasick() : to(1), cnt(1, 0) { to[0].fill(-1); }
    int add(const string& s) {
        int v = 0;
        for (char ch : s) {
            int c = ch - B;
            assert(0 <= c && c < K);
            if (to[v][c] == -1) {
                to[v][c] = to.size();
                to.push_back({}), to.back().fill(-1), cnt.push_back(0);
            }
            v = to[v][c];
        }
        cnt[v]++;
        end_node.push_back(v);
        return end_node.size() - 1;
    }
    void build() {
        int n = to.size();
        fail.assign(n, 0);
        bfs = {0};
        for (int i = 0; i < (int)bfs.size(); i++) {  // ★ 浅い（短い）ノードから順に fail を決める
            int v = bfs[i];
            if (v != 0) cnt[v] += cnt[fail[v]];  // ★ fail 先で終わるパターンも、ここで終わっている（接尾辞なので）
            for (int c = 0; c < K; c++) {
                int u = to[v][c];
                if (u == -1) {
                    to[v][c] = v == 0 ? 0 : to[fail[v]][c];  // ★ 子がないなら fail 先の遷移を借りる（完全なオートマトンにする）
                } else {
                    fail[u] = v == 0 ? 0 : to[fail[v]][c];   // ★ u の fail = 「v の fail から c で進んだ先」
                    bfs.push_back(u);
                }
            }
        }
    }
    int next(int v, char ch) const { return to[v][ch - B]; }
    int size() const { return to.size(); }
    long long count_all(const string& t) const {
        long long res = 0;
        int v = 0;
        for (char ch : t) v = to[v][ch - B], res += cnt[v];
        return res;
    }
    vector<long long> count_each(const string& t) const {
        vector<long long> visit(to.size(), 0);
        int v = 0;
        for (char ch : t) v = to[v][ch - B], visit[v]++;  // 各状態に何回来たか
        for (int i = (int)bfs.size() - 1; i > 0; i--) visit[fail[bfs[i]]] += visit[bfs[i]];  // ★ 深い方から fail 先へ回数を流す
        vector<long long> res(end_node.size());
        for (int i = 0; i < (int)end_node.size(); i++) res[i] = visit[end_node[i]];
        return res;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // パターン he, she, his, hers をテキスト ushers で数える: he 1, she 1, his 0, hers 1
    {
        AhoCorasick<> ac;
        for (string p : {"he", "she", "his", "hers"}) ac.add(p);
        ac.build();
        assert((ac.count_each("ushers") == vector<long long>{1, 1, 0, 1}) && ac.count_all("ushers") == 3);
        // 重なりも数える: aa は aaaa に 3 回
        AhoCorasick<> b;
        b.add("aa"), b.add("a");
        b.build();
        assert((b.count_each("aaaa") == vector<long long>{3, 4}));
    }
    // 使用例（DP）: a, b からなる長さ 4 の文字列で、ab も bb も含まないものの数
    //   b の前に文字があると ab か bb になるので、b は先頭にしか置けない → aaaa, baaa の 2 個
    {
        AhoCorasick<2, 'a'> ac;
        ac.add("ab"), ac.add("bb");
        ac.build();
        vector<long long> dp(ac.size(), 0);
        dp[0] = 1;
        for (int i = 0; i < 4; i++) {
            vector<long long> nd(ac.size(), 0);
            for (int v = 0; v < ac.size(); v++)
                for (char c : {'a', 'b'}) {
                    int u = ac.next(v, c);
                    if (ac.cnt[u] == 0) nd[u] += dp[v];  // ★ パターンで終わる状態には行かない
                }
            dp = nd;
        }
        assert(accumulate(dp.begin(), dp.end(), 0LL) == 2);
    }
    // ランダムテスト: 部分文字列を全部見て数えたものと比較（同じパターンが重複して入ることもある）
    {
        mt19937 rr(171);
        for (int it = 0; it < 3000; it++) {
            int k = rr() % 6 + 1;
            vector<string> ps;
            AhoCorasick<3, 'a'> ac;
            for (int i = 0; i < k; i++) {
                string p;
                for (int j = rr() % 4 + 1; j > 0; j--) p += (char)('a' + rr() % 3);
                ps.push_back(p), ac.add(p);
            }
            ac.build();
            string t;
            for (int j = rr() % 30; j > 0; j--) t += (char)('a' + rr() % 3);
            auto got = ac.count_each(t);
            long long all = 0;
            for (int i = 0; i < k; i++) {
                long long c = 0;
                for (size_t s = 0; s + ps[i].size() <= t.size(); s++) c += t.compare(s, ps[i].size(), ps[i]) == 0;
                assert(got[i] == c);
                all += c;
            }
            assert(ac.count_all(t) == all);
            // 状態遷移: 読んだ文字列の「一番長い、Trie にある接尾辞」のノードになっているか（cnt で確認）
            int v = 0;
            for (size_t i = 0; i < t.size(); i++) {
                v = ac.next(v, t[i]);
                long long ends = 0;
                for (auto& p : ps) ends += p.size() <= i + 1 && t.compare(i + 1 - p.size(), p.size(), p) == 0;
                assert(ac.cnt[v] == ends);
            }
        }
    }
    puts("OK");
}
