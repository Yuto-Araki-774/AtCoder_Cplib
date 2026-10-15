// =====================================================================
// graph/graph_input.cpp  グラフの入力
//   AtCoder でよくある入力形式をそのまま読み、0-indexed の隣接リストにする
//   「N M / a_1 b_1 / ... / a_M b_M」（1-indexed）を毎回書かずに済ませる
// 依存: なし（cin から読む）
// ABC / ARC での使用例:
//   ABC168 D - .. (Double Dots)  https://atcoder.jp/contests/abc168/tasks/abc168_d
//     → N M と辺のリストを read_graph で読む
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 重みなしグラフ ----------
// 入力の各行が「a b」の m 行のとき
// directed    : 有向なら true（a -> b だけ）。無向なら false（両方向）
// one_indexed : 入力の頂点番号が 1 から始まるなら true（ほとんどの問題）→ 内部では 0 から
// 例: int n, m; cin >> n >> m; auto g = read_graph(n, m);
// ◆ read_graph(n, m, directed, one_indexed) : 「a b」が m 行の入力を読み、重みなしの隣接リストを返す
//   n           : 頂点数（隣接リストの大きさ）
//   m           : 辺の本数（読む行数）
//   directed    : true なら a → b だけ入れる。false（既定）なら両方向に入れる
//   one_indexed : true（既定）なら入力の番号から 1 引いて 0 始まりにする。入力が 0 始まりなら false
//   注意 : n, m を先に cin で読んでから呼ぶ（この関数は辺の行だけを読む）
vector<vector<int>> read_graph(int n, int m, bool directed = false, bool one_indexed = true) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/graph_input.cpp
    vector<vector<int>> g(n);
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        if (one_indexed) a--, b--;  // ★ 1-indexed → 0-indexed
        g[a].push_back(b);
        if (!directed) g[b].push_back(a);  // ★ 無向なら逆向きも
    }
    return g;
}

// ---------- 削除可: 木（辺が n-1 本。本体A が必要） ----------
// 例: int n; cin >> n; auto g = read_tree(n);
// ◆ read_tree(n, one_indexed) : 頂点 n 個・辺 n−1 本の木を読む（無向の隣接リスト）
//   n : 頂点数（読むのは n−1 行）、one_indexed : read_graph と同じ
vector<vector<int>> read_tree(int n, bool one_indexed = true) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/graph_input.cpp
    return read_graph(n, n - 1, false, one_indexed);
}

// ---------- 削除可: 親の列で与えられる木「p_2 p_3 ... p_n」（頂点 i の親が p_i） ----------
// 返り値は無向の隣接リスト（tree.cpp や rerooting.cpp にそのまま渡せる形）
// ◆ read_parents(n, one_indexed) : 「p_2 p_3 … p_n」（頂点 i の親が p_i）の形の木を読む（無向の隣接リスト）
//   n : 頂点数（読むのは n−1 個の数）。頂点 1（0 始まりでは 0）が根
//   one_indexed : 親の番号が 1 始まりなら true（既定）
vector<vector<int>> read_parents(int n, bool one_indexed = true) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/graph_input.cpp
    vector<vector<int>> g(n);
    for (int v = 1; v < n; v++) {
        int p;
        cin >> p;
        if (one_indexed) p--;
        g[p].push_back(v), g[v].push_back(p);
    }
    return g;
}

// ---------- 本体B: 重み付きグラフ ----------
// 入力の各行が「a b c」（c が重み）の m 行のとき
// T : 重みの型
// E : 辺の型。省略すると pair<int, T>（{行き先, 重み}）
//     dijkstra.cpp の WGraph に入れるなら E = WEdge<T> を指定する
// 例: auto g = read_wgraph<long long>(n, m);                    // g[a] に {b, c}
//     auto g = read_wgraph<long long, WEdge<long long>>(n, m);  // そのまま dijkstra(g, s) に渡せる
// ◆ read_wgraph<T, E>(n, m, directed, one_indexed) : 「a b c」が m 行の入力を読み、重み付き隣接リストを返す
//   T : 重みの型（long long など。必ず書く）
//   E : 辺の型。省略すると pair<int, T>（{行き先, 重み}）。dijkstra に渡すなら WEdge<T>
//   n, m, directed, one_indexed : read_graph と同じ
template <class T, class E = pair<int, T>>
vector<vector<E>> read_wgraph(int n, int m, bool directed = false, bool one_indexed = true) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/graph_input.cpp
    vector<vector<E>> g(n);
    for (int i = 0; i < m; i++) {
        int a, b;
        T c;
        cin >> a >> b >> c;
        if (one_indexed) a--, b--;
        g[a].push_back(E{b, c});
        if (!directed) g[b].push_back(E{a, c});
    }
    return g;
}

// ---------- 本体C: グリッド ----------
// 入力が h 行の文字列のとき
// 例: int h, w; cin >> h >> w; auto S = read_grid(h);
// ◆ read_grid(h) : h 行の文字列（空白を含まない）を読む
//   h : 行数（h, w は先に読んでおく）
vector<string> read_grid(int h) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/graph_input.cpp
    vector<string> s(h);
    for (auto& row : s) cin >> row;
    return s;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
template <class T> struct WEdge { int to; T cost; };  // dijkstra.cpp と同じ形（動作確認用）
int main() {
    // cin を文字列から読むように差し替えて確認する
    istringstream in(
        "4 3\n1 2\n2 3\n3 4\n"      // 無向グラフ
        "3 2\n1 2\n1 3\n"           // 有向グラフ
        "3\n1 2\n2 3\n"             // 木
        "4\n1 1 2\n"                // 親の列
        "3 2\n1 2 10\n2 3 5\n"      // 重み付き
        "2 1\n1 2 7\n"              // 重み付き（WEdge）
        "2 3\n#.#\n...\n");         // グリッド
    auto* old = cin.rdbuf(in.rdbuf());

    int n, m;
    cin >> n >> m;
    auto g = read_graph(n, m);
    assert(g[1] == vector<int>({0, 2}) && g[3] == vector<int>({2}));
    cin >> n >> m;
    auto d = read_graph(n, m, true);
    assert(d[0] == vector<int>({1, 2}) && d[1].empty());
    cin >> n;
    auto t = read_tree(n);
    assert(t[1].size() == 2);
    cin >> n;
    auto p = read_parents(n);
    assert(p[0] == vector<int>({1, 2}) && p[3] == vector<int>({1}));
    cin >> n >> m;
    auto wg = read_wgraph<long long>(n, m);
    assert(wg[1].size() == 2 && wg[2][0] == make_pair(1, 5LL));
    cin >> n >> m;
    auto we = read_wgraph<long long, WEdge<long long>>(n, m);
    assert(we[1][0].to == 0 && we[1][0].cost == 7);
    int h, w;
    cin >> h >> w;
    auto S = read_grid(h);
    assert(S[0] == "#.#" && S[1] == "...");

    cin.rdbuf(old);
    // ランダムテスト: ランダムなグラフを文字列にして読み込み、隣接リストが一致するか
    {
        mt19937 rr(111);
        for (int it = 0; it < 1000; it++) {
            int nn = rr() % 6 + 1, mm = rr() % 8;
            bool dir = rr() % 2;
            vector<pair<int, int>> es;
            vector<long long> ws;
            for (int i = 0; i < mm; i++) es.push_back({(int)(rr() % nn), (int)(rr() % nn)}), ws.push_back(rr() % 100);
            string txt;
            for (int i = 0; i < mm; i++) txt += to_string(es[i].first + 1) + " " + to_string(es[i].second + 1) + "\n";
            for (int i = 0; i < mm; i++) txt += to_string(es[i].first + 1) + " " + to_string(es[i].second + 1) + " " + to_string(ws[i]) + "\n";
            istringstream in2(txt);
            auto* o = cin.rdbuf(in2.rdbuf());
            auto gg = read_graph(nn, mm, dir);
            auto wgg = read_wgraph<long long>(nn, mm, dir);
            cin.rdbuf(o);
            vector<multiset<int>> ref(nn);
            vector<multiset<pair<int, long long>>> wref(nn);
            for (int i = 0; i < mm; i++) {
                auto [a, b] = es[i];
                ref[a].insert(b), wref[a].insert({b, ws[i]});
                if (!dir) ref[b].insert(a), wref[b].insert({a, ws[i]});
            }
            for (int v = 0; v < nn; v++)
                assert((multiset<int>(gg[v].begin(), gg[v].end()) == ref[v] && multiset<pair<int, long long>>(wgg[v].begin(), wgg[v].end()) == wref[v]));
        }
    }
    puts("OK");
}
