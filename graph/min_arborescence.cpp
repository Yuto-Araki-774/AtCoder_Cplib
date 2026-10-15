// =====================================================================
// graph/min_arborescence.cpp  最小全域有向木（Chu-Liu/Edmonds 法）
//   有向グラフで、根 root から全頂点へ行ける「有向の全域木」のうち、辺の重みの合計が最小のものの重み。O(NM)
//   （無向グラフの最小全域木はクラスカル・プリムでよいが、有向だと貪欲では解けない）
//   ★ 各頂点（根以外）について「入ってくる一番安い辺」を選ぶ。閉路ができなければそれで最小。
//      閉路ができたら、その閉路を 1 頂点に縮め、入ってくる辺の重みを「閉路の中の辺と取り替えたときの差」にして繰り返す
// 依存: なし
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// edges = 有向辺 {a, b, 重み}（a → b。負の重みも OK）
// auto [ok, cost] = min_arborescence<long long>(n, edges, root);
//   ok = root から全頂点へ行けるか（行けない頂点があれば false）、cost = 最小の重みの合計
// 各頂点の親（どの辺を使ったか）までは求めない
// ◆ min_arborescence<T>(n, edges, root) : 最小全域有向木（root から全頂点へ行ける有向の木の最小の重み）。O(NM)
//   T     : 重みの型（必ず書く）
//   n     : 頂点数
//   edges : 有向辺 {a, b, 重み}（a → b）の一覧。負の重み・多重辺もよい（自己ループは無視）
//   root  : 根
//   返り値 : {root から全頂点へ行けるか, 最小の重みの合計}
template <class T>
pair<bool, T> min_arborescence(int n, vector<tuple<int, int, T>> edges, int root) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/min_arborescence.cpp
    const T INF = numeric_limits<T>::max() / 4;
    T res = 0;
    while (true) {
        vector<T> in(n, INF);
        vector<int> pre(n, -1);
        for (auto [a, b, w] : edges)  // ★ 各頂点に入ってくる一番安い辺
            if (a != b && w < in[b]) in[b] = w, pre[b] = a;
        for (int v = 0; v < n; v++)
            if (v != root && in[v] == INF) return {false, 0};  // 入ってくる辺がない → 行けない頂点がある
        int cnt = 0;
        vector<int> id(n, -1), vis(n, -1);
        in[root] = 0;
        for (int v = 0; v < n; v++) {
            res += in[v];  // ★ 選んだ辺の重みを足す（閉路を縮めた後は「差」を足していることになる）
            int x = v;
            while (vis[x] != v && id[x] == -1 && x != root) vis[x] = v, x = pre[x];  // pre をたどる
            if (x != root && id[x] == -1) {  // ★ 今回たどった中で閉路に入った → 閉路の頂点に同じ番号を振る
                for (int y = pre[x]; y != x; y = pre[y]) id[y] = cnt;
                id[x] = cnt++;
            }
        }
        if (cnt == 0) break;  // 閉路がない → 選んだ辺が最小全域有向木
        for (int v = 0; v < n; v++)
            if (id[v] == -1) id[v] = cnt++;
        vector<tuple<int, int, T>> ne;
        for (auto [a, b, w] : edges)
            if (id[a] != id[b]) ne.push_back({id[a], id[b], w - in[b]});  // ★ b に入る辺を取り替えたときの差
        edges = ne, n = cnt, root = id[root];
    }
    return {true, res};
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // root = 0。0 → 1 (10), 0 → 2 (10), 1 → 2 (1), 2 → 1 (1)
    //   入ってくる一番安い辺は 2 → 1 と 1 → 2 で閉路 → どちらか 1 本を 0 からの辺に替える: 10 + 1 = 11
    {
        auto [ok, c] = min_arborescence<long long>(3, {{0, 1, 10}, {0, 2, 10}, {1, 2, 1}, {2, 1, 1}}, 0);
        assert(ok && c == 11);
        auto [ok2, c2] = min_arborescence<long long>(3, {{0, 1, 1}, {2, 1, 1}}, 0);  // 2 には入れない
        assert(!ok2);
        assert(min_arborescence<long long>(1, {}, 0).second == 0);
    }
    // ランダムテスト: 各頂点の「入ってくる辺」を全部の組合せで選び、根から全部に行けるものの最小と比較
    {
        mt19937 rr(198);
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 5 + 1, m = rr() % 10, root = rr() % n;
            vector<tuple<int, int, long long>> es;
            for (int i = 0; i < m; i++) es.push_back({(int)(rr() % n), (int)(rr() % n), (long long)(rr() % 21) - 5});
            vector<vector<int>> inE(n);
            for (int i = 0; i < m; i++) { auto [a, b, w] = es[i]; if (a != b && b != root) inE[b].push_back(i); }
            long long best = LLONG_MAX;
            vector<int> pick(n, -1);
            function<void(int)> rec = [&](int v) {
                if (v == n) {
                    vector<vector<int>> ch(n);
                    long long w = 0;
                    for (int u = 0; u < n; u++) if (u != root) { auto [a, b, c] = es[pick[u]]; ch[a].push_back(b), w += c; }
                    vector<bool> seen(n, false);
                    vector<int> st = {root};
                    seen[root] = true;
                    while (!st.empty()) { int x = st.back(); st.pop_back(); for (int y : ch[x]) if (!seen[y]) seen[y] = true, st.push_back(y); }
                    if (count(seen.begin(), seen.end(), true) == n) best = min(best, w);
                    return;
                }
                if (v == root) { rec(v + 1); return; }
                for (int i : inE[v]) pick[v] = i, rec(v + 1);
            };
            rec(0);
            auto [ok, c] = min_arborescence<long long>(n, es, root);
            assert(ok == (best != LLONG_MAX));
            if (ok) assert(c == best);
        }
    }
    puts("OK");
}
