// =====================================================================
// graph/rollback_union_find.cpp  ロールバック付き UnionFind
//   普通の UnionFind に「直前の merge を取り消す（undo）」「ある時点まで戻す（rollback）」を足したもの
//   経路圧縮をしないので 1 回 O(log N)
//   典型: 木の上を DFS しながら「行きで merge、帰りで undo」、辺の追加と削除が混ざる問題をオフラインで（分割統治）
//   取り消しが要らないなら ACL の dsu の方が速い
// 依存: なし
// ABC / ARC での使用例:
//   ABC302 Ex - Ball Collector  https://atcoder.jp/contests/abc302/tasks/abc302_h
//     → ボールの番号を頂点、各頂点の 2 個のボールを辺にする。木を DFS しながら行きで merge、帰りで undo。答え = 成分ごとの min(size, edges) の和
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// RollbackUnionFind uf(n);
//   uf.merge(a, b)       つなぐ（もともと同じなら何もせず false）。どちらの場合も「1 回」として記録される
//   uf.undo()            直前の merge を 1 回取り消す
//   int t = uf.snapshot(); ... uf.rollback(t);   t の時点まで戻す
//   uf.leader(a), uf.same(a, b), uf.size(a), uf.components（連結成分の数）
//   uf.edges(a)          a の連結成分の中で merge した回数（= 辺の本数。同じ成分どうしの merge も 1 本と数える）
struct RollbackUnionFind {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/rollback_union_find.cpp
    vector<int> par;  // 根なら -(サイズ)、それ以外は親
    vector<int> ecnt; // 根ごとの辺の本数
    int components;
    struct Rec { int a, pa, ea, b, pb, eb; bool merged; };  // 変更前の値
    vector<Rec> hist;
    RollbackUnionFind(int n) : par(n, -1), ecnt(n, 0), components(n) {}
    int leader(int x) const {
        while (par[x] >= 0) x = par[x];  // ★ 経路圧縮しない（すると取り消せなくなる）
        return x;
    }
    bool same(int a, int b) const { return leader(a) == leader(b); }
    int size(int a) const { return -par[leader(a)]; }
    int edges(int a) const { return ecnt[leader(a)]; }
    bool merge(int a, int b) {
        a = leader(a), b = leader(b);
        hist.push_back({a, par[a], ecnt[a], b, par[b], ecnt[b], a != b});  // ★ 書き換える前の値を覚えておく
        if (a == b) { ecnt[a]++; return false; }
        if (par[a] > par[b]) swap(a, b);  // ★ 大きい方に小さい方をつなぐ → 木の高さが log N 以下
        par[a] += par[b], par[b] = a, ecnt[a] += ecnt[b] + 1, components--;
        return true;
    }
    void undo() {
        Rec r = hist.back();
        hist.pop_back();
        par[r.a] = r.pa, par[r.b] = r.pb, ecnt[r.a] = r.ea, ecnt[r.b] = r.eb;  // ★ 覚えておいた値に戻すだけ
        if (r.merged) components++;
    }
    int snapshot() const { return hist.size(); }
    void rollback(int t) { while ((int)hist.size() > t) undo(); }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    RollbackUnionFind uf(5);
    uf.merge(0, 1);
    int t = uf.snapshot();
    uf.merge(1, 2), uf.merge(3, 4);
    assert(uf.same(0, 2) && uf.components == 2 && uf.size(0) == 3 && uf.edges(0) == 2 && uf.edges(3) == 1);
    uf.undo();  // 3-4 を取り消す
    assert(!uf.same(3, 4) && uf.components == 3);
    uf.rollback(t);  // 0-1 だけの状態に戻す
    assert(uf.same(0, 1) && !uf.same(1, 2) && uf.components == 4);
    assert(!uf.merge(0, 1));  // すでに同じ → false だが記録はされる
    uf.undo();
    assert(uf.same(0, 1) && uf.components == 4);  // 何も変わらない

    // ランダムテスト: 「今までに有効な merge の辺の一覧」を持ち、毎回 BFS で連結成分を求めたものと比較
    mt19937 rr(132);
    for (int it = 0; it < 1000; it++) {
        int n = rr() % 8 + 1;
        RollbackUnionFind u(n);
        vector<pair<int, int>> edges;  // merge を呼んだ順（取り消されていないもの）
        vector<int> snaps;
        for (int op = 0; op < 60; op++) {
            int k = rr() % 6;
            if (k <= 2) { int a = rr() % n, b = rr() % n; u.merge(a, b), edges.push_back({a, b}); }
            else if (k == 3 && !edges.empty()) { u.undo(), edges.pop_back(); }
            else if (k == 4) snaps.push_back(u.snapshot());
            else if (!snaps.empty()) {
                int s = snaps.back();
                snaps.pop_back();
                if (s <= (int)edges.size()) u.rollback(s), edges.resize(s);
            }
            vector<int> comp(n, -1);
            int C = 0;
            for (int s = 0; s < n; s++) {
                if (comp[s] != -1) continue;
                vector<int> st = {s};
                comp[s] = C;
                while (!st.empty()) {
                    int v = st.back();
                    st.pop_back();
                    for (auto [a, b] : edges) {
                        if (a == v && comp[b] == -1) comp[b] = C, st.push_back(b);
                        if (b == v && comp[a] == -1) comp[a] = C, st.push_back(a);
                    }
                }
                C++;
            }
            assert(u.components == C);
            for (int a = 0; a < n; a++) {
                int sz = 0;
                for (int b = 0; b < n; b++) { assert(u.same(a, b) == (comp[a] == comp[b])); sz += comp[a] == comp[b]; }
                int ec = 0;
                for (auto [x, y] : edges) ec += comp[x] == comp[a];
                assert(u.size(a) == sz && u.edges(a) == ec);
            }
        }
    }
    puts("OK");
}
