// =====================================================================
// tree/tree_hash.cpp  木の同型判定（形が同じ木に同じ番号をつける）
//   根付き木: 「子の番号を並べた列」ごとに番号を振る（AHU 法）。乱数ハッシュではないので衝突しない。O(N log N)
//   根なし木: 中心（直径の真ん中、1 個か 2 個）を根にして、小さい方の番号を使う
// 依存: なし
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// TreeCanon tc;                     比べたい木はすべて同じ tc で番号を振ること（番号の表を共有するため）
//   auto id = tc.rooted(g, r);      根を r にしたとき、各頂点 v の部分木の形の番号 id[v]
//                                   id[u] == id[v] ⇔ u の部分木と v の部分木が同じ形（根付きで）
//   int t = tc.unrooted(g);         根なし木としての形の番号（2 つの木が同型 ⇔ 番号が同じ）
// g : 木の隣接リスト（頂点 0..n-1）
// ◆ struct TreeCanon : 木の同型判定（形が同じ木・部分木に同じ番号を振る。衝突なし）
//   rooted(g, r)  : 根を r にしたときの、各頂点の部分木の形の番号の配列。O(N log N)
//   unrooted(g)   : 根なし木としての形の番号
//   注意 : 比べたい木はすべて同じ TreeCanon で番号を振ること（番号の表を共有するため）
struct TreeCanon {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree_hash.cpp
    map<vector<int>, int> table;  // ★ 子の番号の列（ソート済み）→ 番号
    // rooted(g, root) : g : 木の隣接リスト（両方向、0..n−1）、root : 根
    vector<int> rooted(const vector<vector<int>>& g, int root) {
        int n = g.size();
        vector<int> id(n, -1), par(n, -1), ord;
        ord.reserve(n);
        vector<int> st = {root};
        par[root] = root;
        while (!st.empty()) {  // DFS で順番を作り、逆順（葉から）に処理する
            int v = st.back();
            st.pop_back();
            ord.push_back(v);
            for (int to : g[v]) if (par[to] == -1) par[to] = v, st.push_back(to);
        }
        for (int i = n - 1; i >= 0; i--) {
            int v = ord[i];
            vector<int> ch;
            for (int to : g[v]) if (to != root && par[to] == v) ch.push_back(id[to]);  // 子の番号を集める
            sort(ch.begin(), ch.end());  // ★ 子の順番を無視するためにソート
            auto it = table.find(ch);
            if (it == table.end()) it = table.emplace(ch, (int)table.size()).first;  // ★ 初めて見る形なら新しい番号
            id[v] = it->second;
        }
        return id;
    }
    // unrooted(g) : g : 木の隣接リスト。中心（1 個か 2 個）を根にしたときの番号の小さい方を返す
    int unrooted(const vector<vector<int>>& g) {
        int n = g.size();
        // ★ 葉を外側から 1 枚ずつはがして、最後に残った 1 〜 2 頂点が中心
        vector<int> deg(n), cur;
        for (int v = 0; v < n; v++) {
            deg[v] = g[v].size();
            if (deg[v] <= 1) cur.push_back(v);
        }
        int left = n;
        while (left > 2) {
            vector<int> nxt;
            left -= cur.size();
            for (int v : cur)
                for (int to : g[v])
                    if (--deg[to] == 1) nxt.push_back(to);
            cur = nxt;
        }
        int best = INT_MAX;
        for (int c : cur) best = min(best, rooted(g, c)[c]);
        return best;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
vector<vector<int>> make_tree(int n, const vector<pair<int, int>>& es) {
    vector<vector<int>> g(n);
    for (auto [a, b] : es) g[a].push_back(b), g[b].push_back(a);
    return g;
}
// 素朴な同型判定: 頂点の対応（順列）を全部試す。root1, root2 >= 0 なら根どうしを対応させる
bool brute_iso(const vector<vector<int>>& g1, const vector<vector<int>>& g2, int root1 = -1, int root2 = -1) {
    int n = g1.size();
    if ((int)g2.size() != n) return false;
    vector<int> p(n);
    iota(p.begin(), p.end(), 0);
    do {
        if (root1 >= 0 && p[root1] != root2) continue;
        bool ok = true;
        for (int v = 0; v < n && ok; v++) {
            if (g1[v].size() != g2[p[v]].size()) { ok = false; break; }
            for (int to : g1[v]) if (find(g2[p[v]].begin(), g2[p[v]].end(), p[to]) == g2[p[v]].end()) ok = false;
        }
        if (ok) return true;
    } while (next_permutation(p.begin(), p.end()));
    return false;
}
int main() {
    // 長さ 3 の道 0-1-2 と 1-0-2（真ん中が 0）は同型。星形 0-1, 0-2, 0-3 と道 0-1-2-3 は違う
    {
        TreeCanon tc;
        auto a = make_tree(3, {{0, 1}, {1, 2}}), b = make_tree(3, {{1, 0}, {0, 2}});
        assert(tc.unrooted(a) == tc.unrooted(b));
        auto star = make_tree(4, {{0, 1}, {0, 2}, {0, 3}}), path = make_tree(4, {{0, 1}, {1, 2}, {2, 3}});
        assert(tc.unrooted(star) != tc.unrooted(path));
        // 根付き: 道 0-1-2 を 0 で根付け → 部分木 {2} と {1,2} と {0,1,2} は全部違う形。葉どうしは同じ番号
        auto id = tc.rooted(star, 0);
        assert(id[1] == id[2] && id[2] == id[3] && id[0] != id[1]);
        assert(tc.unrooted(make_tree(1, {})) == tc.rooted(make_tree(1, {}), 0)[0]);  // 1 頂点
    }
    // ランダムテスト: 頂点の対応を全部試す方法と比較（根なし・根付き・部分木どうし）
    {
        mt19937 rr(163);
        TreeCanon tc;
        auto rnd_tree = [&](int n) {
            vector<pair<int, int>> es;
            for (int v = 1; v < n; v++) es.push_back({(int)(rr() % v), v});
            vector<int> p(n);  // 番号を混ぜる
            iota(p.begin(), p.end(), 0);
            shuffle(p.begin(), p.end(), rr);
            for (auto& [a, b] : es) a = p[a], b = p[b];
            return make_tree(n, es);
        };
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 7 + 1;
            auto g1 = rnd_tree(n), g2 = rnd_tree(n);
            assert((tc.unrooted(g1) == tc.unrooted(g2)) == brute_iso(g1, g2));
            int r1 = rr() % n, r2 = rr() % n;
            assert((tc.rooted(g1, r1)[r1] == tc.rooted(g2, r2)[r2]) == brute_iso(g1, g2, r1, r2));
        }
        // 部分木どうし: 1 つの木で、id[u] == id[v] ⇔ 部分木が同型（部分木を取り出して比べる）
        for (int it = 0; it < 500; it++) {
            int n = rr() % 8 + 1;
            auto g = rnd_tree(n);
            auto id = tc.rooted(g, 0);
            vector<int> par(n, -1);
            vector<int> st = {0};
            par[0] = 0;
            while (!st.empty()) { int v = st.back(); st.pop_back(); for (int to : g[v]) if (par[to] == -1) par[to] = v, st.push_back(to); }
            auto sub = [&](int r) {  // r の部分木を取り出して番号を振り直す（r は 0 番）
                vector<int> vs = {r}, idx(n, -1);
                idx[r] = 0;
                for (int i = 0; i < (int)vs.size(); i++)
                    for (int to : g[vs[i]]) if (par[to] == vs[i] && to != vs[i] && to != 0) idx[to] = vs.size(), vs.push_back(to);
                vector<vector<int>> h(vs.size());
                for (int v : vs) for (int to : g[v]) if (idx[to] != -1 && par[to] == v && to != 0) h[idx[v]].push_back(idx[to]), h[idx[to]].push_back(idx[v]);
                return h;
            };
            for (int u = 0; u < n; u++)
                for (int v = 0; v < n; v++) assert((id[u] == id[v]) == brute_iso(sub(u), sub(v), 0, 0));
        }
    }
    puts("OK");
}
