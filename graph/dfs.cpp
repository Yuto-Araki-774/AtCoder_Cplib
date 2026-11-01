// =====================================================================
// graph/dfs.cpp  深さ優先探索（DFS）。再帰なし（深いグラフでもスタックオーバーフローしない）
//   本体A: 汎用 DFS。「頂点に入るとき」「頂点から出るとき」に好きな処理を挟める
//   本体B: 連結成分の番号付け
//   本体C: 閉路検出・復元（有向・無向どちらでも。多重辺・自己ループもよい）
//   A・B・C は互いに独立。O(N + M)
// 依存: graph/graph.cpp の Graph（先に貼る）
// ABC / ARC での使用例:
//   ABC285 D - Change Usernames  https://atcoder.jp/contests/abc285/tasks/abc285_d
//     → 名前の変更を有向辺（add_arc）にして閉路があるか（find_cycle）
//   ABC288 C - Don't be cycle  https://atcoder.jp/contests/abc288/tasks/abc288_c
//     → 答え = M − (N − 連結成分数)（connected_components）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;
// テスト用: graph.cpp の Graph を読み込む（提出するときは、graph.cpp のコピー範囲をこのファイルのコピー範囲より先に貼る）
#define GRAPH_NO_MAIN
#include "graph.cpp"

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 汎用 DFS（graph.cpp の Graph が必要） ----------
// Graph<> g(n); g.add_edge(a, b); ...
// s     : 始点
// seen  : 訪問済みフラグ（サイズ n）。s から行ける頂点が true になる
//         複数の始点から順に呼ぶと、前回までに訪れた頂点は飛ばされる（森・連結成分ごとの処理）
// 【自分で定義するもの】（main の中にラムダで書く。何もしないなら [](int, int) {} ）
//   on_enter(v, p) : v に初めて入ったとき。p は v に来る直前の頂点（始点は -1）  = 行きがけ
//   on_exit(v, p)  : v から行ける未訪問の頂点をすべて調べ終わったとき           = 帰りがけ
// 例: 行きがけ順・帰りがけ順・DFS 木の親・部分木サイズなど（使用例あり）
// ◆ dfs_walk(g, s, seen, on_enter, on_exit) : s から深さ優先探索（再帰なし）。O(N + M)
//   g        : Graph（有向辺は向きどおりにだけ進む）
//   s        : 始点。seen[s] が true ならすぐ終わる
//   seen     : 訪問済みフラグ（大きさ n、最初は全部 false）。呼んだ後は s から行けた頂点が true
//              同じ seen で別の始点から呼び直すと、もう訪れた頂点は飛ばす（連結成分ごとの処理）
//   on_enter : void on_enter(int v, int p)。v に初めて来たときに呼ぶ（行きがけ）。p は来る前の頂点（始点は −1）
//   on_exit  : void on_exit(int v, int p)。v から先を全部調べ終わったときに呼ぶ（帰りがけ）
//   注意 : on_enter / on_exit の中で seen を書き換えないこと
template <class T, class Enter, class Exit>
void dfs_walk(const Graph<T>& g, int s, vector<bool>& seen, Enter on_enter, Exit on_exit) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dfs.cpp
    if (seen[s]) return;
    // （内部用）スタックに積む情報
    struct Frame { int v, p, i; };  // 頂点、親、次に見る辺の位置
    vector<Frame> st;
    seen[s] = true;
    on_enter(s, -1);
    st.push_back({s, -1, 0});
    while (!st.empty()) {
        Frame& f = st.back();
        if (f.i < (int)g.adj[f.v].size()) {
            int to = g.adj[f.v][f.i++].to;
            if (seen[to]) continue;
            seen[to] = true;
            on_enter(to, f.v);           // ★ 行きがけ: 初めて来たときの処理
            st.push_back({to, f.v, 0});  // ★ 再帰呼び出しの代わりにスタックに積む（f はこの後使わない）
        } else {
            on_exit(f.v, f.p);  // ★ 帰りがけ: 子をすべて処理し終えた
            st.pop_back();      // ★ 再帰から戻る代わりにスタックから降ろす
        }
    }
}

// ---------- 本体B: 連結成分の番号付け（graph.cpp の Graph が必要。無向グラフ用） ----------
// 返り値 comp[v] = v の連結成分の番号（0 から）。成分の個数は *max_element + 1
// ◆ connected_components(g) : 無向グラフの連結成分の番号付け。O(N + M)
//   g : Graph（add_edge の無向辺で作ること。有向辺 add_arc だと成分が正しく分かれない）
//   返り値 : comp[v] = v の成分の番号（0 から、頂点番号の小さい順に振る）
template <class T>
vector<long long> connected_components(const Graph<T>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dfs.cpp
    int n = g.n, c = 0;
    vector<long long> comp(n, -1);
    for (int s = 0; s < n; s++) {
        if (comp[s] != -1) continue;
        vector<int> st = {s};
        comp[s] = c;
        while (!st.empty()) {  // ★ s から行ける頂点に同じ番号
            int v = st.back();
            st.pop_back();
            for (auto& e : g.adj[v]) if (comp[e.to] == -1) comp[e.to] = c, st.push_back(e.to);
        }
        c++;
    }
    return comp;
}

// ---------- 本体C: 閉路検出・復元（graph.cpp の Graph が必要。有向・無向どちらでも） ----------
// 返り値: 閉路の頂点列 {v0, v1, ..., vk}（v0 → v1 → ... → vk → v0 が閉路）。閉路がなければ空
// 有向辺（add_arc）は向きどおり、無向辺（add_edge）は同じ辺を 2 回通らない閉路を探す（混ぜてもよい）
// ◆ find_cycle(g) : 閉路を 1 つ見つける。O(N + M)
//   g : Graph。有向グラフなら有向の閉路、無向グラフなら無向の閉路
//   返り値 : 閉路の頂点の並び（最後の頂点から最初の頂点へ辺がある）。閉路がなければ空
//   自己ループは {v}、無向の多重辺（同じ 2 頂点を結ぶ辺が 2 本）は長さ 2 の閉路 {a, b} として見つかる
template <class T>
vector<long long> find_cycle(const Graph<T>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dfs.cpp
    int n = g.n;
    vector<int> state(n, 0), pos(n, -1);  // state: 0 未訪問、1 探索中（スタックに乗っている）、2 探索済み
    // （内部用）スタックに積む情報
    struct Frame { int v, in_id, i; };  // 頂点、この頂点に来るのに使った辺の番号（始点は −1）、次に見る辺の位置
    vector<Frame> st;
    for (int s = 0; s < n; s++) {
        if (state[s]) continue;
        st.push_back({s, -1, 0}), state[s] = 1, pos[s] = 0;
        while (!st.empty()) {
            Frame& f = st.back();
            if (f.i < (int)g.adj[f.v].size()) {
                auto& e = g.adj[f.v][f.i++];
                if (e.id == f.in_id) continue;  // ★ 来た辺（無向辺）をそのまま戻るのは閉路ではない
                if (state[e.to] == 1) {         // ★ 探索中の頂点に戻ってきた = 閉路
                    vector<long long> cyc;
                    for (int k = pos[e.to]; k < (int)st.size(); k++) cyc.push_back(st[k].v);  // ★ スタック上の e.to から今まで
                    return cyc;
                }
                if (state[e.to] == 0) {
                    int to = e.to, id = e.id;  // push_back で f, e が無効になる前に取り出す
                    pos[to] = st.size(), state[to] = 1, st.push_back({to, id, 0});
                }
            } else {
                state[f.v] = 2;  // ★ ここから先に閉路はなかった
                st.pop_back();
            }
        }
    }
    return {};
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    //   0 - 1 - 3
    //   |   |
    //   2   4      5 - 6
    Graph<> g(7);
    g.add_edge(0, 1), g.add_edge(0, 2), g.add_edge(1, 3), g.add_edge(1, 4), g.add_edge(5, 6);

    // 行きがけ順・帰りがけ順・親・部分木サイズを一度に求める
    int n = g.n;
    vector<int> pre, post, par(n, -1), sz(n, 1);
    vector<bool> seen(n, false);
    dfs_walk(g, 0, seen,
             [&](int v, int p) { pre.push_back(v), par[v] = p; },
             [&](int v, int p) { post.push_back(v); if (p != -1) sz[p] += sz[v]; });
    assert((pre == vector<int>{0, 1, 3, 4, 2}));
    assert((post == vector<int>{3, 4, 1, 2, 0}));
    assert(par[3] == 1 && sz[0] == 5 && sz[1] == 3 && !seen[5]);

    // 連結成分
    auto comp = connected_components(g);
    assert(comp[0] == comp[4] && comp[5] == comp[6] && comp[0] != comp[5]);
    assert(*max_element(comp.begin(), comp.end()) + 1 == 2);

    // 有向グラフの閉路: 0→1→2→0、3→1
    {
        Graph<> d(4);
        d.add_arc(0, 1), d.add_arc(1, 2), d.add_arc(2, 0), d.add_arc(3, 1);
        assert((find_cycle(d) == vector<long long>{0, 1, 2}));
        Graph<> d2(3);
        d2.add_arc(0, 1), d2.add_arc(1, 2);
        assert(find_cycle(d2).empty());
        d2.add_arc(2, 2);
        assert((find_cycle(d2) == vector<long long>{2}));  // 自己ループ
    }
    // 無向グラフの閉路
    assert(find_cycle(g).empty());  // 森なので閉路なし
    g.add_edge(3, 4);               // 1-3-4-1 ができる
    assert(find_cycle(g).size() == 3);
    {
        Graph<> m(2);
        m.add_edge(0, 1), m.add_edge(0, 1);
        assert(find_cycle(m).size() == 2);  // 多重辺
        Graph<> l(1);
        l.add_edge(0, 0);
        assert((find_cycle(l) == vector<long long>{0}));  // 無向の自己ループ
    }
    // ランダムテスト: 閉路検出（入次数で判定・辺の本数で判定）、連結成分（UnionFind）、DFS の順番の正しさ
    {
        mt19937 rr(109);
        auto has_dir = [&](const vector<vector<int>>& h) {
            int n = h.size();
            vector<int> in(n);
            for (auto& a : h) for (int t : a) in[t]++;
            queue<int> q;
            for (int v = 0; v < n; v++) if (!in[v]) q.push(v);
            int c = 0;
            while (!q.empty()) { int v = q.front(); q.pop(); c++; for (int t : h[v]) if (--in[t] == 0) q.push(t); }
            return c != n;
        };
        for (int it = 0; it < 5000; it++) {
            int n = rr() % 7 + 1, m = rr() % 9;
            vector<vector<int>> dg(n);
            Graph<> D(n), U(n);
            set<pair<int, int>> dset;
            multiset<pair<int, int>> uset;
            int loops = 0;
            for (int i = 0; i < m; i++) {
                int a = rr() % n, b = rr() % n;
                dg[a].push_back(b), dset.insert({a, b}), D.add_arc(a, b);
                if (a != b || it % 3 == 0) U.add_edge(a, b), uset.insert({min(a, b), max(a, b)}), loops += a == b;
            }
            auto c = find_cycle(D);
            assert(c.empty() == !has_dir(dg));
            for (size_t k = 0; k < c.size(); k++) assert(dset.count({c[k], c[(k + 1) % c.size()]}));
            vector<int> p(n);
            iota(p.begin(), p.end(), 0);
            function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
            for (auto [a, b] : uset) p[f(a)] = f(b);
            auto cc = connected_components(U);
            int C = 0;
            for (int v = 0; v < n; v++) C += f(v) == v;
            for (int a = 0; a < n; a++) for (int b = 0; b < n; b++) assert((cc[a] == cc[b]) == (f(a) == f(b)));
            auto cu = find_cycle(U);
            assert(cu.empty() == ((int)uset.size() == n - C));  // 森 ⇔ 辺の本数 = N − 成分数（自己ループも辺として数える）
            if (!cu.empty()) {
                set<int> vs(cu.begin(), cu.end());
                assert(vs.size() == cu.size());
                if (cu.size() == 1) assert(uset.count({cu[0], cu[0]}));
                else if (cu.size() == 2) assert(uset.count({min(cu[0], cu[1]), max(cu[0], cu[1])}) >= 2);
                else for (size_t k = 0; k < cu.size(); k++) { int a = cu[k], b = cu[(k + 1) % cu.size()]; assert(uset.count({min(a, b), max(a, b)})); }
            }
            int s = rr() % n, t1 = 0, t2 = 0;
            vector<bool> seen(n, false);
            vector<int> pre(n, -1), post(n, -1), par(n, -2);
            dfs_walk(U, s, seen, [&](int v, int pp) { pre[v] = t1++, par[v] = pp; }, [&](int v, int) { post[v] = t2++; });
            for (int v = 0; v < n; v++) {
                assert(seen[v] == (f(v) == f(s)));
                if (!seen[v]) continue;
                if (par[v] >= 0) assert(pre[par[v]] < pre[v] && post[v] < post[par[v]]);
                else assert(v == s);
            }
            for (auto [a, b] : uset)  // DFS なら、どの辺も「祖先と子孫」を結ぶ（区間が入れ子）
                if (seen[a] && a != b) assert((pre[a] < pre[b] && post[b] < post[a]) || (pre[b] < pre[a] && post[a] < post[b]));
        }
    }
    puts("OK");
}
