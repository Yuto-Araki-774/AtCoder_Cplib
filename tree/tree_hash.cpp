// =====================================================================
// tree/tree_hash.cpp  木の同型判定（形が同じ木に同じ番号をつける）
//   根付き木: 「子の番号を並べた列」ごとに番号を振る（AHU 法）。乱数ハッシュではないので衝突しない。O(N log N)
//   根なし木: 重心（1 個か 2 個）を根にして、小さい方の番号を使う
//   tree.cpp の Tree を継承していて、使い方は Tree と同じ（add_edge → build）
// 依存: tree/tree.cpp の Tree（先に貼る）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;
// テスト用: tree.cpp の Tree を読み込む（提出するときは、tree.cpp のコピー範囲をこのファイルのコピー範囲より先に貼る）
#define TREE_NO_MAIN
#include "tree.cpp"

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体（tree.cpp の Tree が必要） ----------
// TreeHash<> th(n);  th.add_edge(a, b);  th.build(root);     Tree と同じ使い方（Tree の関数 lca・dist なども使える）
//   th.id[v]        根を root にしたとき、v の部分木の形の番号。id[u] == id[v] ⇔ u と v の部分木が同じ形（根付きで）
//   th.unrooted()   根なし木としての形の番号（2 つの木が同型 ⇔ 番号が同じ）。build しなくてよい
//   別の木どうしでも番号を比べてよい（番号の表は全部の TreeHash で共有している）
// 隣接リスト g がもうあるなら TreeHash<> th(g); でもよい（add_edge をまとめてやる。build は自分で）
// ◆ struct TreeHash<T> : 木の同型判定（形が同じ木・部分木に同じ番号を振る。乱数を使わないので衝突しない）。Tree<T> を継承
//   TreeHash(n), TreeHash(g) : 頂点 0..n−1 の木を作る準備 / 隣接リスト g から辺を入れる
//   add_edge(a, b, w)        : Tree と同じ（重みは見ない）
//   build(root)              : Tree の前計算＋ id[v]（根を root にしたときの部分木の形の番号）。O(N log N)
//   unrooted()               : 根なし木としての形の番号。O(N log N)
//   ★ 根付き木: 「子の番号をソートした列」ごとに番号を振る（AHU 法）。根なし木: 重心（1 個か 2 個）を根にした番号の小さい方
template <class T = long long>
struct TreeHash : Tree<T> {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree_hash.cpp
    using B = Tree<T>;
    inline static map<vector<int>, int> table;  // ★ 子の番号の列（ソート済み）→ 番号。全部の TreeHash で共有
    vector<long long> id;  // id[v] = build(root) の根で見たときの、v の部分木の形の番号
    // n_ : 頂点数
    TreeHash(int n_) : B(n_) {}
    // g : 木の隣接リスト（両方向）
    TreeHash(const vector<vector<int>>& g) : B(g) {}
    // build(root) : root : 根（省略で 0）。辺を全部足してから呼ぶ
    void build(int root = 0) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree_hash.cpp
        B::build(root);
        id = calc(root);
    }
    // unrooted() : build していなくても呼べる
    long long unrooted() const {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree_hash.cpp
        long long best = LLONG_MAX;
        for (long long c : this->centroids()) best = min(best, calc(c)[c]);
        return best;
    }
    // （内部用）root を根にしたときの各頂点の番号（このオブジェクトの状態は変えない）
    vector<long long> calc(int root) const {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree_hash.cpp
        int n = this->n;
        vector<int> par(n, -1), ord = {root};
        for (size_t i = 0; i < ord.size(); i++)
            for (auto [to, w] : this->adj[ord[i]]) if (to != par[ord[i]]) par[to] = ord[i], ord.push_back(to);
        vector<long long> res(n, -1);
        for (int i = n - 1; i >= 0; i--) {  // 葉の方から
            int v = ord[i];
            vector<int> ch;
            for (auto [to, w] : this->adj[v]) if (to != par[v]) ch.push_back(res[to]);  // 子の番号を集める
            sort(ch.begin(), ch.end());  // ★ 子の順番を無視するためにソート
            auto it = table.find(ch);
            if (it == table.end()) it = table.emplace(ch, (int)table.size()).first;  // ★ 初めて見る形なら新しい番号
            res[v] = it->second;
        }
        return res;
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
        TreeHash<> a(3), b(3);  // Tree と同じく add_edge
        a.add_edge(0, 1), a.add_edge(1, 2);
        b.add_edge(1, 0), b.add_edge(0, 2);
        assert(a.unrooted() == b.unrooted());
        TreeHash<> star(make_tree(4, {{0, 1}, {0, 2}, {0, 3}})), path(make_tree(4, {{0, 1}, {1, 2}, {2, 3}}));
        assert(star.unrooted() != path.unrooted());
        // 根付き: 星形を 0 で根付け → 葉どうしは同じ番号、根は違う
        star.build(0);
        assert(star.id[1] == star.id[2] && star.id[2] == star.id[3] && star.id[0] != star.id[1]);
        assert(star.lca(1, 2) == 0 && star.dist(1, 3) == 2);  // Tree の関数も使える
        TreeHash<> one(1);
        one.build(0);
        assert(one.unrooted() == one.id[0]);  // 1 頂点
    }
    // ランダムテスト: 頂点の対応を全部試す方法と比較（根なし・根付き・部分木どうし）
    {
        mt19937 rr(163);
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
            TreeHash<> t1(g1), t2(g2);
            assert((t1.unrooted() == t2.unrooted()) == brute_iso(g1, g2));
            int r1 = rr() % n, r2 = rr() % n;
            t1.build(r1), t2.build(r2);
            assert((t1.id[r1] == t2.id[r2]) == brute_iso(g1, g2, r1, r2));
        }
        // 部分木どうし: 1 つの木で、id[u] == id[v] ⇔ 部分木が同型（部分木を取り出して比べる）
        for (int it = 0; it < 500; it++) {
            int n = rr() % 8 + 1;
            auto g = rnd_tree(n);
            TreeHash<> th(g);
            th.build(0);
            auto& id = th.id;
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
