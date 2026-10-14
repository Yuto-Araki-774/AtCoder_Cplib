// =====================================================================
// graph/dfs.cpp  深さ優先探索（DFS）。再帰なし（深いグラフでもスタックオーバーフローしない）
//   本体A: 汎用 DFS。「頂点に入るとき」「頂点から出るとき」に好きな処理を挟める
//   本体B: 有向グラフの閉路検出・復元
//   本体C: 無向グラフの閉路検出・復元（多重辺もよい）
//   A・B・C は互いに独立。O(N + M)
// 依存: なし
// ABC / ARC での使用例:
//   ABC285 D - Change Usernames  https://atcoder.jp/contests/abc285/tasks/abc285_d
//     → 名前の変更を有向辺にして閉路があるか（find_cycle_directed）
//   ABC288 C - Don't be cycle  https://atcoder.jp/contests/abc288/tasks/abc288_c
//     → 答え = M − (N − 連結成分数)（connected_components）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 汎用 DFS ----------
// g     : 隣接リスト（無向なら両方向に入れる）
// s     : 始点
// seen  : 訪問済みフラグ（サイズ n）。s から行ける頂点が true になる
//         複数の始点から順に呼ぶと、前回までに訪れた頂点は飛ばされる（森・連結成分ごとの処理）
// 【自分で定義するもの】（main の中にラムダで書く。何もしないなら [](int, int) {} ）
//   on_enter(v, p) : v に初めて入ったとき。p は v に来る直前の頂点（始点は -1）  = 行きがけ
//   on_exit(v, p)  : v から行ける未訪問の頂点をすべて調べ終わったとき           = 帰りがけ
// 例: 行きがけ順・帰りがけ順・DFS 木の親・部分木サイズなど（使用例あり）
template <class Enter, class Exit>
void dfs_walk(const vector<vector<int>>& g, int s, vector<bool>& seen, Enter on_enter, Exit on_exit) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dfs.cpp
    if (seen[s]) return;
    struct Frame { int v, p, i; };  // 頂点、親、次に見る隣接の位置
    vector<Frame> st;
    seen[s] = true;
    on_enter(s, -1);
    st.push_back({s, -1, 0});
    while (!st.empty()) {
        Frame& f = st.back();
        if (f.i < (int)g[f.v].size()) {
            int to = g[f.v][f.i++];
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

// ---------- 削除可: 連結成分の番号付け（本体A が必要。無向グラフ用） ----------
// 返り値 comp[v] = v の連結成分の番号（0 から）。成分の個数は *max_element + 1
vector<int> connected_components(const vector<vector<int>>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dfs.cpp
    int n = g.size(), c = 0;
    vector<int> comp(n, -1);
    vector<bool> seen(n, false);
    for (int s = 0; s < n; s++) {
        if (seen[s]) continue;
        dfs_walk(g, s, seen, [&](int v, int) { comp[v] = c; }, [](int, int) {});  // ★ s から行ける頂点に同じ番号
        c++;
    }
    return comp;
}

// ---------- 本体B: 有向グラフの閉路検出・復元 ----------
// 返り値: 閉路の頂点列 {v0, v1, ..., vk}（v0→v1→...→vk→v0 が閉路）。閉路がなければ空
// 自己ループ v→v は {v} を返す
vector<int> find_cycle_directed(const vector<vector<int>>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dfs.cpp
    int n = g.size();
    vector<int> state(n, 0), pos(n, -1);  // state: 0 未訪問、1 探索中（スタックに乗っている）、2 探索済み
    vector<pair<int, int>> st;            // {頂点, 次に見る隣接の位置}
    for (int s = 0; s < n; s++) {
        if (state[s]) continue;
        st.push_back({s, 0}), state[s] = 1, pos[s] = 0;
        while (!st.empty()) {
            auto& [v, i] = st.back();
            if (i < (int)g[v].size()) {
                int to = g[v][i++];
                if (state[to] == 1) {  // ★ 探索中の頂点に戻ってきた = 閉路
                    vector<int> cyc;
                    for (int k = pos[to]; k < (int)st.size(); k++) cyc.push_back(st[k].first);  // ★ スタック上の to から今まで
                    return cyc;
                }
                if (state[to] == 0) pos[to] = st.size(), state[to] = 1, st.push_back({to, 0});
            } else {
                state[v] = 2;  // ★ ここから先に閉路はなかった
                st.pop_back();
            }
        }
    }
    return {};
}

// ---------- 本体C: 無向グラフの閉路検出・復元 ----------
// g は両方向に辺を入れた隣接リスト。多重辺（同じ 2 頂点を結ぶ辺が 2 本）も長さ 2 の閉路として見つける
// 返り値: 閉路の頂点列 {v0, v1, ..., vk}（v0-v1-...-vk-v0 が閉路）。閉路がなければ空
vector<int> find_cycle_undirected(const vector<vector<int>>& g) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dfs.cpp
    int n = g.size();
    vector<int> state(n, 0), pos(n, -1);
    struct Frame { int v, p, i; bool skipped; };  // skipped: 親へ戻る辺を 1 本飛ばしたか
    vector<Frame> st;
    for (int s = 0; s < n; s++) {
        if (state[s]) continue;
        st.push_back({s, -1, 0, false}), state[s] = 1, pos[s] = 0;
        while (!st.empty()) {
            Frame& f = st.back();
            if (f.i < (int)g[f.v].size()) {
                int to = g[f.v][f.i++];
                if (to == f.p && !f.skipped) { f.skipped = true; continue; }  // ★ 来た辺を戻るのは閉路ではない（1 本だけ）
                if (state[to] == 1) {  // ★ 祖先に戻る辺 = 閉路
                    vector<int> cyc;
                    for (int k = pos[to]; k < (int)st.size(); k++) cyc.push_back(st[k].v);
                    return cyc;
                }
                if (state[to] == 0) {
                    int v = f.v;  // push_back で f が無効になる前に取り出す
                    pos[to] = st.size(), state[to] = 1, st.push_back({to, v, 0, false});
                }
            } else {
                state[f.v] = 2;
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
    vector<vector<int>> g(7);
    auto add = [&](int a, int b) { g[a].push_back(b), g[b].push_back(a); };
    add(0, 1), add(0, 2), add(1, 3), add(1, 4), add(5, 6);

    // 行きがけ順・帰りがけ順・親・部分木サイズを一度に求める
    int n = g.size();
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
    vector<vector<int>> d = {{1}, {2}, {0}, {1}};
    assert((find_cycle_directed(d) == vector<int>{0, 1, 2}));
    assert(find_cycle_directed({{1}, {2}, {}}).empty());

    // 無向グラフの閉路
    assert(find_cycle_undirected(g).empty());  // 森なので閉路なし
    add(3, 4);                                  // 1-3-4-1 ができる
    auto cyc = find_cycle_undirected(g);
    assert(cyc.size() == 3);
    assert(find_cycle_undirected({{1, 1}, {0, 0}}).size() == 2);  // 多重辺
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
            vector<vector<int>> dg(n), u(n);
            set<pair<int, int>> dset;
            multiset<pair<int, int>> uset;
            for (int i = 0; i < m; i++) {
                int a = rr() % n, b = rr() % n;
                dg[a].push_back(b), dset.insert({a, b});
                if (a != b) u[a].push_back(b), u[b].push_back(a), uset.insert({min(a, b), max(a, b)});
            }
            auto c = find_cycle_directed(dg);
            assert(c.empty() == !has_dir(dg));
            for (size_t k = 0; k < c.size(); k++) assert(dset.count({c[k], c[(k + 1) % c.size()]}));
            vector<int> p(n);
            iota(p.begin(), p.end(), 0);
            function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
            for (auto [a, b] : uset) p[f(a)] = f(b);
            auto cc = connected_components(u);
            int C = 0;
            for (int v = 0; v < n; v++) C += f(v) == v;
            for (int a = 0; a < n; a++) for (int b = 0; b < n; b++) assert((cc[a] == cc[b]) == (f(a) == f(b)));
            auto cu = find_cycle_undirected(u);
            assert(cu.empty() == ((int)uset.size() == n - C));
            if (!cu.empty()) {
                set<int> vs(cu.begin(), cu.end());
                assert(vs.size() == cu.size());
                if (cu.size() == 2) assert(uset.count({min(cu[0], cu[1]), max(cu[0], cu[1])}) >= 2);
                else for (size_t k = 0; k < cu.size(); k++) { int a = cu[k], b = cu[(k + 1) % cu.size()]; assert(uset.count({min(a, b), max(a, b)})); }
            }
            int s = rr() % n, t1 = 0, t2 = 0;
            vector<bool> seen(n, false);
            vector<int> pre(n, -1), post(n, -1), par(n, -2);
            dfs_walk(u, s, seen, [&](int v, int pp) { pre[v] = t1++, par[v] = pp; }, [&](int v, int) { post[v] = t2++; });
            for (int v = 0; v < n; v++) {
                assert(seen[v] == (f(v) == f(s)));
                if (!seen[v]) continue;
                if (par[v] >= 0) assert(pre[par[v]] < pre[v] && post[v] < post[par[v]]);
                else assert(v == s);
            }
            for (auto [a, b] : uset)  // DFS なら、どの辺も「祖先と子孫」を結ぶ（区間が入れ子）
                if (seen[a]) assert((pre[a] < pre[b] && post[b] < post[a]) || (pre[b] < pre[a] && post[a] < post[b]));
        }
    }
    puts("OK");
}
