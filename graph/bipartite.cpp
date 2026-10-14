// =====================================================================
// graph/bipartite.cpp  二部グラフ判定（2 彩色）
//   頂点を 2 色に塗り分けて、すべての辺の両端が違う色になるようにできるか。O(N + M)
//   できる ⇔ 奇数長の閉路がない。非連結でもよい（連結成分ごとに塗る）
//   ※ 辺が 1 本ずつ追加される問題なら、重み付き UnionFind（差を 1 とみて mod 2）でも判定できる
// 依存: なし
// ABC / ARC での使用例:
//   ABC282 D - Make Bipartite 2  https://atcoder.jp/contests/abc282/tasks/abc282_d
//     → 成分ごとの色の個数（bipartite_components）から数える
//   ABC126 D - Even Relation  https://atcoder.jp/contests/abc126/tasks/abc126_d
//     → 根からの距離の偶奇で 2 色に塗る（2 彩色の考え方。重み付きなので dfs_walk で親から偶奇を伝えて塗る）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// g : 無向グラフの隣接リスト（両方向に入れる）
// 返り値: color[v] = 0 か 1（二部グラフでなければ空のベクタ）
// 例: auto col = bipartite_coloring(g);  if (col.empty()) { /* 二部グラフでない */ }
vector<int> bipartite_coloring(const vector<vector<int>>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bipartite.cpp
    int n = g.size();
    vector<int> color(n, -1);
    for (int s = 0; s < n; s++) {
        if (color[s] != -1) continue;
        color[s] = 0;  // ★ 連結成分ごとに、最初の頂点を色 0 にする
        queue<int> q;
        q.push(s);
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int to : g[v]) {
                if (color[to] == -1) color[to] = color[v] ^ 1, q.push(to);  // ★ 隣は反対の色
                else if (color[to] == color[v]) return {};                   // ★ 隣が同じ色 = 矛盾（奇数長の閉路）
            }
        }
    }
    return color;
}

// ---------- 削除可: 連結成分ごとの色の個数（本体が必要） ----------
// 返り値: 成分ごとの {色 0 の頂点数, 色 1 の頂点数}（二部グラフでなければ空）
// 典型: 「各成分でどちらの色を "選ぶ" か決めて、選んだ頂点数の合計を K にできるか」→ この結果で部分和 DP
//      （各成分の 2 色は入れ替えてよいので、どちらの数を取るかを選べる）
vector<pair<int, int>> bipartite_components(const vector<vector<int>>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bipartite.cpp
    auto color = bipartite_coloring(g);
    if (color.empty() && !g.empty()) return {};
    int n = g.size();
    vector<bool> seen(n, false);
    vector<pair<int, int>> res;
    for (int s = 0; s < n; s++) {
        if (seen[s]) continue;
        pair<int, int> cnt = {0, 0};
        vector<int> st = {s};
        seen[s] = true;
        while (!st.empty()) {
            int v = st.back();
            st.pop_back();
            (color[v] == 0 ? cnt.first : cnt.second)++;
            for (int to : g[v])
                if (!seen[to]) seen[to] = true, st.push_back(to);
        }
        res.push_back(cnt);
    }
    return res;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 0-1-2-3-0（長さ 4 の閉路）+ 4-5、6 は孤立
    vector<vector<int>> g(7);
    auto add = [&](int a, int b) { g[a].push_back(b), g[b].push_back(a); };
    add(0, 1), add(1, 2), add(2, 3), add(3, 0), add(4, 5);
    auto col = bipartite_coloring(g);
    assert(!col.empty());
    for (int v = 0; v < 7; v++)
        for (int to : g[v]) assert(col[v] != col[to]);
    auto comps = bipartite_components(g);
    assert((comps == vector<pair<int, int>>{{2, 2}, {1, 1}, {1, 0}}));

    add(0, 2);  // 0-1-2-0 で長さ 3 の閉路 → 二部グラフでない
    assert(bipartite_coloring(g).empty() && bipartite_components(g).empty());
    // ランダムテスト: 2^n 通りの塗り方を全部試したものと比較
    {
        mt19937 rr(108);
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 8 + 1;
            vector<vector<int>> h(n);
            vector<pair<int, int>> es;
            for (int e = rr() % 10; e > 0; e--) {
                int a = rr() % n, b = rr() % n;
                if (a == b) continue;
                h[a].push_back(b), h[b].push_back(a), es.push_back({a, b});
            }
            bool ok = false;
            for (int m = 0; m < (1 << n) && !ok; m++) {
                bool good = true;
                for (auto [a, b] : es) if ((m >> a & 1) == (m >> b & 1)) good = false;
                ok |= good;
            }
            auto c = bipartite_coloring(h);
            assert(c.empty() == !ok);
            if (!ok) continue;
            for (auto [a, b] : es) assert(c[a] != c[b]);
            auto cs = bipartite_components(h);
            vector<int> p(n);
            iota(p.begin(), p.end(), 0);
            function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
            for (auto [a, b] : es) p[f(a)] = f(b);
            int C = 0, tot = 0;
            for (int v = 0; v < n; v++) C += f(v) == v;
            for (auto [x, y] : cs) tot += x + y;
            assert((int)cs.size() == C && tot == n);
        }
    }
    puts("OK");
}
