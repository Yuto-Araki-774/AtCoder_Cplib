// =====================================================================
// graph/graph.cpp  グラフの土台（辺の張り方・入力）
//   Tree と同じく「作る → 辺を足す」でグラフを作る。graph フォルダのアルゴリズムはすべてこの Graph を受け取る
//   無向辺は add_edge、有向辺は add_arc（1 つのグラフに混ぜてもよい）。辺には足した順に番号 0, 1, 2, ... が付く
//   AtCoder でよくある入力「a_1 b_1 / ... / a_M b_M」（1 始まり）を読む関数つき
// 依存: なし
// ABC / ARC での使用例:
//   ABC168 D - .. (Double Dots)  https://atcoder.jp/contests/abc168/tasks/abc168_d
//     → Graph<> g(N); g.read_edges(M); で読み、bfs.cpp の bfs(g, 0) の prev が答え
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// Graph<> g(n);              頂点 0..n−1（<> の中は辺の重みの型。省略で long long）
//   g.add_edge(a, b, w);     無向辺 a — b（w 省略で 1）
//   g.add_arc(a, b, w);      有向辺 a → b（w 省略で 1）
//   g.read_edges(m);         「a b」が m 行（1 始まり）を無向辺として読む。重み付き「a b c」なら g.read_edges(m, true)
//   g.read_arcs(m);          同じく有向辺として読む
//   for (auto& e : g.adj[v]) { e.to, e.cost, e.id }    v から出る辺を回す（無向辺は両方の端点から見える）
// ◆ struct Graph<T> : グラフ（隣接リスト＋辺の一覧）
//   Graph(n)                         : 頂点 0..n−1 のグラフを作る（辺はまだない）
//   Graph(g)                         : 隣接リスト g（g[v] = v から行ける頂点）から作る。g[v] の各要素を有向辺 v → to として入れる
//                                      （無向グラフの隣接リストは両向きが入っているので、そのまま同じグラフになる）
//   add_edge(a, b, w)                : 無向辺を足す。返り値は辺の番号
//   add_arc(a, b, w)                 : 有向辺 a → b を足す。返り値は辺の番号
//   read_edges(m, weighted, one)     : m 本の無向辺を cin から読む。weighted = true なら「a b c」、one = true（既定）なら 1 始まりの入力
//   read_arcs(m, weighted, one)      : 同じく有向辺を読む
//   【変数】
//   n          : 頂点数
//   adj[v]     : v から出る辺 {from = v, to, cost, id} の一覧。無向辺は a にも b にも入る（同じ id）
//   edges[id]  : 足した順の辺 {from, to, cost, id}。無向辺も 1 本として入る
//   is_arc[id] : その辺が有向辺か
//   T : 辺の重みの型（既定 long long）
template <class T = long long>
struct Graph {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/graph.cpp
    struct Edge {
        int from, to;  // 辺の始点と終点（adj[v] の中では from == v）
        T cost;        // 重み
        int id;        // 辺の番号（足した順に 0, 1, 2, ...。無向辺は両向きで同じ番号）
    };
    int n;                        // 頂点数
    vector<vector<Edge>> adj;     // adj[v] = v から出る辺
    vector<Edge> edges;           // edges[id] = 足した順の辺
    vector<bool> is_arc;          // is_arc[id] = 有向辺か
    // n_ : 頂点数
    Graph(int n_) : n(n_), adj(n_) {}
    // g : 隣接リスト（g[v] = v から行ける頂点。重みはすべて 1）
    Graph(const vector<vector<int>>& g) : n(g.size()), adj(g.size()) {
        for (int v = 0; v < n; v++) for (int to : g[v]) add_arc(v, to);
    }
    // add_edge(a, b, w) : a, b : 端点（0 始まり）、w : 重み（省略で 1）
    long long add_edge(int a, int b, T w = T(1)) {
        int id = edges.size();
        edges.push_back({a, b, w, id}), is_arc.push_back(false);
        adj[a].push_back({a, b, w, id});
        adj[b].push_back({b, a, w, id});  // ★ 無向辺は逆向きにも入れる（同じ番号）
        return id;
    }
    // add_arc(a, b, w) : a → b の向きだけ
    long long add_arc(int a, int b, T w = T(1)) {
        int id = edges.size();
        edges.push_back({a, b, w, id}), is_arc.push_back(true);
        adj[a].push_back({a, b, w, id});
        return id;
    }
    // read_edges(m, weighted, one_indexed) : m : 読む行数、weighted : 重みも読むか、one_indexed : 入力が 1 始まりか
    void read_edges(int m, bool weighted = false, bool one_indexed = true) { read(m, weighted, one_indexed, false); }
    // read_arcs(m, weighted, one_indexed) : read_edges の有向辺版
    void read_arcs(int m, bool weighted = false, bool one_indexed = true) { read(m, weighted, one_indexed, true); }
    // （内部用）
    void read(int m, bool weighted, bool one_indexed, bool arc) {
        for (int i = 0; i < m; i++) {
            int a, b;
            T w = T(1);
            cin >> a >> b;
            if (weighted) cin >> w;
            if (one_indexed) a--, b--;  // ★ 1 始まり → 0 始まり
            arc ? add_arc(a, b, w) : add_edge(a, b, w);
        }
    }
};

// ---------- 削除可: グリッドの入力 ----------
// ◆ read_grid(h) : h 行の文字列（空白を含まない）を読む。グリッドの BFS は bfs.cpp の grid_bfs
//   h : 行数（h, w は先に読んでおく）
vector<string> read_grid(int h) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/graph.cpp
    vector<string> s(h);
    for (auto& row : s) cin >> row;
    return s;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// 他のグラフのファイルのテストは、ここより上を #include して使う（GRAPH_NO_MAIN を定義してから）
#ifndef GRAPH_NO_MAIN
// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    Graph<> g(4);
    assert(g.add_edge(0, 1) == 0 && g.add_arc(1, 2, 5) == 1 && g.add_edge(2, 3, 7) == 2);
    assert(g.adj[0].size() == 1 && g.adj[1].size() == 2 && g.adj[2].size() == 1 && g.adj[3].size() == 1);  // 有向辺 1 → 2 は 2 からは見えない
    assert(g.adj[1][1].to == 2 && g.adj[1][1].cost == 5 && g.adj[1][1].id == 1 && g.adj[1][1].from == 1);
    assert(g.adj[3][0].to == 2 && g.adj[3][0].id == 2 && g.is_arc[1] && !g.is_arc[2]);
    assert(g.edges[2].from == 2 && g.edges[2].to == 3 && g.edges[2].cost == 7);

    // 入力を読む: 1 始まりの「a b c」
    {
        istringstream in("1 2 10\n2 3 20\n");
        auto* old = cin.rdbuf(in.rdbuf());
        Graph<> h(3);
        h.read_edges(2, true);
        cin.rdbuf(old);
        assert(h.edges.size() == 2 && h.edges[1].from == 1 && h.edges[1].to == 2 && h.edges[1].cost == 20 && h.adj[1].size() == 2);
    }
    {
        istringstream in("0 1\n1 2\n");
        auto* old = cin.rdbuf(in.rdbuf());
        Graph<int> h(3);
        h.read_arcs(2, false, false);
        cin.rdbuf(old);
        assert(h.adj[0].size() == 1 && h.adj[1].size() == 1 && h.adj[2].empty() && h.edges[0].cost == 1);
    }
    {   // 隣接リストから作る
        vector<vector<int>> a = {{1, 2}, {0}, {0}};
        Graph<> h(a);
        assert(h.adj[0].size() == 2 && h.adj[1][0].to == 0 && h.edges.size() == 4);
    }
    {
        istringstream in("ab\n.#\n");
        auto* old = cin.rdbuf(in.rdbuf());
        auto s = read_grid(2);
        cin.rdbuf(old);
        assert(s[0] == "ab" && s[1] == ".#");
    }
    puts("OK");
}
#endif
