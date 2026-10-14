// =====================================================================
// graph/lowlink.cpp  LowLink（橋・関節点）+ 二重辺連結成分分解 + 二重頂点連結成分分解・Block-Cut Tree
//   橋     : 取り除くとグラフが分断される辺
//   関節点 : 取り除くとグラフが分断される頂点
//   二重頂点連結成分（ブロック）: どの 1 頂点を取り除いてもつながったままの、極大な部分グラフ。関節点で分けた「かたまり」
//   O(N + M)。無向グラフ用。多重辺・非連結でもよい。再帰なし（深いグラフでも安全）
// 依存: なし
// ABC / ARC での使用例:
//   ABC075 C - Bridge  https://atcoder.jp/contests/abc075/tasks/abc075_c
//     → 橋の本数（bridges.size()）
//   ABC334 G - Christmas Color Grid 2  https://atcoder.jp/contests/abc334/tasks/abc334_g
//     → 緑のマスを頂点にする。マス v を赤にした後の成分数 = C − 1 + (v を含むブロックの数 = block_cut_tree の t[v].size())。ただし孤立したマスは C − 1
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// LowLink ll(n);  ll.add_edge(a, b);（辺番号が返る、0 から順番）  ll.build();
//   ll.bridges       橋の辺番号の一覧
//   ll.is_bridge[e]  辺 e が橋か
//   ll.is_art[v]     頂点 v が関節点か
//   ll.articulation  関節点の一覧
//   ll.edges[e]      辺 e の両端 {a, b}
struct LowLink {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/lowlink.cpp
    int n;
    vector<vector<pair<int, int>>> g;  // g[v] = {行き先, 辺番号}
    vector<pair<int, int>> edges;
    vector<int> ord, low, bridges, articulation;
    vector<bool> is_bridge, is_art;

    LowLink(int n_) : n(n_), g(n_) {}
    int add_edge(int a, int b) {
        int id = edges.size();
        g[a].push_back({b, id}), g[b].push_back({a, id});
        edges.push_back({a, b});
        return id;
    }

    void build() {
        // ord[v] : DFS で訪れた順番
        // low[v] : v から「DFS 木の子へ下る辺」を何本か + 「後退辺」を 1 本まで使って戻れる最小の ord
        ord.assign(n, -1), low.assign(n, 0);
        is_bridge.assign(edges.size(), false), is_art.assign(n, false);
        vector<int> pe(n, -1), it(n, 0);  // pe[v] = 親から来た辺の番号、it[v] = 次に見る隣接の位置
        int k = 0;
        for (int s = 0; s < n; s++) {
            if (ord[s] != -1) continue;
            int root_child = 0;
            vector<int> st = {s};
            ord[s] = low[s] = k++;
            while (!st.empty()) {
                int v = st.back();
                if (it[v] < (int)g[v].size()) {
                    auto [to, id] = g[v][it[v]++];
                    if (id == pe[v]) continue;  // 来た辺そのものは戻らない（多重辺は別の辺なので OK）
                    if (ord[to] == -1) {        // 未訪問 → DFS 木の子
                        pe[to] = id, ord[to] = low[to] = k++;
                        st.push_back(to);
                        if (v == s) root_child++;
                    } else {
                        low[v] = min(low[v], ord[to]);  // ★ 後退辺: 祖先に戻れる
                    }
                } else {  // v の探索が終わった → 親 p に low を伝える
                    st.pop_back();
                    if (st.empty()) break;
                    int p = st.back();
                    low[p] = min(low[p], low[v]);
                    if (low[v] > ord[p]) is_bridge[pe[v]] = true;  // ★ v 側から p より上に戻れない → 辺 p-v は橋
                    if (p != s && low[v] >= ord[p]) is_art[p] = true;  // ★ v 側が p を通らないと上に行けない → p は関節点
                }
            }
            if (root_child >= 2) is_art[s] = true;  // ★ 根は「DFS 木の子が 2 つ以上」なら関節点
        }
        for (int e = 0; e < (int)edges.size(); e++)
            if (is_bridge[e]) bridges.push_back(e);
        for (int v = 0; v < n; v++)
            if (is_art[v]) articulation.push_back(v);
    }

    // --- 削除可: 二重辺連結成分分解（build の後に呼ぶ） ---
    // 橋をすべて取り除いたときの連結成分。comp[v] = v の成分番号（0 から）
    // 成分を頂点、橋を辺にすると木（森）になる（橋木）
    vector<int> two_edge_cc() const {
        vector<int> comp(n, -1);
        int c = 0;
        for (int s = 0; s < n; s++) {
            if (comp[s] != -1) continue;
            vector<int> st = {s};
            comp[s] = c;
            while (!st.empty()) {
                int v = st.back();
                st.pop_back();
                for (auto [to, id] : g[v])
                    if (!is_bridge[id] && comp[to] == -1) comp[to] = c, st.push_back(to);  // ★ 橋は渡らない
            }
            c++;
        }
        return comp;
    }
    // --- 削除可: 二重頂点連結成分分解・Block-Cut Tree（build は不要。単独で呼べる） ---
    // bcc()            : 各ブロックの頂点の一覧。関節点は複数のブロックに入る。辺のない頂点は 1 頂点だけのブロック
    //                    （自己ループは無視する）
    // block_cut_tree() : 頂点 0..n-1 = 元の頂点、n.. = ブロック。元の頂点とそれを含むブロックを辺で結んだ森（隣接リスト）
    //   ★ 関節点 v を取り除くと、v を含んでいた成分は「v を含むブロックの数」個に分かれる
    //   ★ 2 頂点 a, b の間のどの道も必ず通る頂点 = 木の上の a-b のパスにある関節点
    vector<vector<int>> bcc() const {
        vector<int> ord2(n, -1), low2(n, 0), pe(n, -1), it(n, 0), vst;
        vector<vector<int>> res;
        int k = 0;
        for (int s = 0; s < n; s++) {
            if (ord2[s] != -1) continue;
            ord2[s] = low2[s] = k++;
            vector<int> st = {s};
            bool has_child = false;
            while (!st.empty()) {
                int v = st.back();
                if (it[v] < (int)g[v].size()) {
                    auto [to, id] = g[v][it[v]++];
                    if (id == pe[v]) continue;
                    if (ord2[to] == -1) {
                        pe[to] = id, ord2[to] = low2[to] = k++;
                        st.push_back(to), vst.push_back(to);
                        if (v == s) has_child = true;
                    } else {
                        low2[v] = min(low2[v], ord2[to]);
                    }
                } else {
                    st.pop_back();
                    if (st.empty()) break;
                    int p = st.back();
                    low2[p] = min(low2[p], low2[v]);
                    if (low2[v] >= ord2[p]) {  // ★ v の側は p を通らないと外に出られない → p と「v 以降に積んだ頂点」で 1 ブロック
                        vector<int> comp = {p};
                        while (true) {
                            int x = vst.back();
                            vst.pop_back();
                            comp.push_back(x);
                            if (x == v) break;
                        }
                        res.push_back(comp);
                    }
                }
            }
            if (!has_child) res.push_back({s});  // 辺のない頂点（自己ループだけの頂点も）
        }
        return res;
    }
    vector<vector<int>> block_cut_tree() const {
        auto b = bcc();
        vector<vector<int>> t(n + b.size());
        for (int i = 0; i < (int)b.size(); i++)
            for (int v : b[i]) t[v].push_back(n + i), t[n + i].push_back(v);
        return t;
    }
    // --- 削除可ここまで ---
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    //  0 - 1
    //   \ /
    //    2 - 3 - 4     5 = 6（多重辺）
    LowLink ll(7);
    ll.add_edge(0, 1), ll.add_edge(1, 2), ll.add_edge(2, 0);
    int e3 = ll.add_edge(2, 3), e4 = ll.add_edge(3, 4);
    ll.add_edge(5, 6), ll.add_edge(5, 6);
    ll.build();
    assert((ll.bridges == vector<int>{e3, e4}));
    assert((ll.articulation == vector<int>{2, 3}));
    auto comp = ll.two_edge_cc();
    assert(comp[0] == comp[1] && comp[1] == comp[2] && comp[2] != comp[3] && comp[3] != comp[4] && comp[5] == comp[6]);
    // ランダムテスト: 辺・頂点を 1 つずつ取り除いて連結成分が増えるかを愚直に調べて比較
    {
        mt19937 rr(113);
        auto comps = [&](int n, const vector<pair<int, int>>& es, int skipV, int skipE) {
            vector<int> p(n);
            iota(p.begin(), p.end(), 0);
            function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
            for (int i = 0; i < (int)es.size(); i++) {
                if (i == skipE) continue;
                auto [a, b] = es[i];
                if (a == skipV || b == skipV) continue;
                p[f(a)] = f(b);
            }
            int c = 0;
            for (int v = 0; v < n; v++) if (v != skipV && f(v) == v) c++;
            return c;
        };
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 8 + 1, m = rr() % 12;
            LowLink L(n);
            vector<pair<int, int>> es;
            for (int i = 0; i < m; i++) {
                int a = rr() % n, b = rr() % n;
                if (a == b) continue;
                L.add_edge(a, b), es.push_back({a, b});
            }
            L.build();
            int base = comps(n, es, -1, -1);
            for (int e = 0; e < (int)es.size(); e++) assert(L.is_bridge[e] == (comps(n, es, -1, e) > base));
            for (int v = 0; v < n; v++) {
                bool iso = true;
                for (auto [a, b] : es) if (a == v || b == v) iso = false;
                assert(L.is_art[v] == (comps(n, es, v, -1) > base - (iso ? 1 : 0)));
            }
            auto cc = L.two_edge_cc();
            vector<int> p(n);
            iota(p.begin(), p.end(), 0);
            function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
            for (int e = 0; e < (int)es.size(); e++) if (!L.is_bridge[e]) p[f(es[e].first)] = f(es[e].second);
            for (int a = 0; a < n; a++) for (int b = 0; b < n; b++) assert((cc[a] == cc[b]) == (f(a) == f(b)));
        }
    }
    // 二重頂点連結成分: 上のグラフ（0-1-2 の三角形、2-3、3-4、5 = 6）→ {0,1,2}, {2,3}, {3,4}, {5,6}
    {
        auto b = ll.bcc();
        set<vector<int>> got;
        for (auto c : b) sort(c.begin(), c.end()), got.insert(c);
        assert((got == set<vector<int>>{{0, 1, 2}, {2, 3}, {3, 4}, {5, 6}}));
        auto t = ll.block_cut_tree();
        assert(t.size() == 7 + 4 && t[2].size() == 2 && t[3].size() == 2 && t[0].size() == 1);  // 関節点 2, 3 は 2 つのブロックに入る
        LowLink one(3);  // 辺なし → 1 頂点ずつ
        assert(one.bcc().size() == 3);
    }
    // ランダムテスト: 「2 本の辺が同じブロック ⇔ どの頂点 x を取り除いても、2 本の辺の（x 以外の）端点がつながっている」と比較
    {
        mt19937 rr(172);
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 7 + 1, m = rr() % 10;
            LowLink L(n);
            vector<pair<int, int>> es;
            for (int i = 0; i < m; i++) {
                int a = rr() % n, b = rr() % n;
                if (a == b) continue;
                L.add_edge(a, b), es.push_back({a, b});
            }
            int M = es.size();
            auto connected = [&](int skip, int a, int b) {  // 頂点 skip を除いたグラフで a と b がつながっているか
                vector<int> p(n);
                iota(p.begin(), p.end(), 0);
                function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
                for (auto [u, v] : es) if (u != skip && v != skip) p[f(u)] = f(v);
                return f(a) == f(b);
            };
            vector<int> cls(M);
            iota(cls.begin(), cls.end(), 0);
            function<int(int)> fc = [&](int x) { return cls[x] == x ? x : cls[x] = fc(cls[x]); };
            for (int e = 0; e < M; e++)
                for (int f = e + 1; f < M; f++) {
                    bool same = connected(-1, es[e].first, es[f].first);
                    for (int x = 0; x < n && same; x++) {
                        vector<int> ends;
                        for (int y : {es[e].first, es[e].second, es[f].first, es[f].second}) if (y != x) ends.push_back(y);
                        for (int y : ends) if (!connected(x, ends[0], y)) same = false;
                    }
                    if (same) cls[fc(e)] = fc(f);
                }
            map<int, set<int>> blocks;
            vector<bool> has_edge(n, false);
            for (int e = 0; e < M; e++) blocks[fc(e)].insert(es[e].first), blocks[fc(e)].insert(es[e].second), has_edge[es[e].first] = has_edge[es[e].second] = true;
            set<vector<int>> want;
            for (auto& [k, s] : blocks) want.insert(vector<int>(s.begin(), s.end()));
            for (int v = 0; v < n; v++) if (!has_edge[v]) want.insert({v});
            auto b = L.bcc();
            set<vector<int>> got;
            for (auto c : b) sort(c.begin(), c.end()), got.insert(c);
            assert(got == want && b.size() == want.size());
            // Block-Cut Tree は森で、関節点 ⇔ 2 つ以上のブロックに入る（辺のある頂点について）
            L.build();
            auto t = L.block_cut_tree();
            int V = t.size(), E = 0;
            for (auto& a : t) E += a.size();
            E /= 2;
            vector<int> p(V);
            iota(p.begin(), p.end(), 0);
            function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
            int comps = V;
            for (int v = 0; v < V; v++) for (int u : t[v]) if (f(u) != f(v)) p[f(u)] = f(v), comps--;
            assert(E == V - comps);  // 森 ⇔ 辺の数 = 頂点数 − 連結成分数
            for (int v = 0; v < n; v++) assert(L.is_art[v] == (t[v].size() >= 2));
        }
    }
    puts("OK");
}
