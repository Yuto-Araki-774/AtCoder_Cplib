// =====================================================================
// tree/tree_imos.cpp  木の上のいもす法
//   「部分木の全頂点に x を足す」「u-v パス上の全頂点に x を足す」をたくさんやってから、最後に各頂点の値をまとめて求める
//   部分木への加算は根に印を付けて下へ流す、パスへの加算は両端に +x・LCA と LCA の親に −x を付けて上へ流す
//   加算 1 回 O(1)（パスは LCA の O(log N)）、最後に O(N)
// 依存: tree/tree.cpp の Tree（先に貼る）。Tree は build 済みであること
// ABC / ARC での使用例:
//   ABC138 D - Ki  https://atcoder.jp/contests/abc138/tasks/abc138_d
//     → 部分木に加算（add_subtree）して、最後に build
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;
// テスト用: tree.cpp の Tree を読み込む（提出するときは、tree.cpp のコピー範囲をこのファイルのコピー範囲より先に貼る）
#define TREE_NO_MAIN
#include "tree.cpp"

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体（tree.cpp の Tree が必要。build 済みであること） ----------
// 頂点に値を足す操作をまとめて行い、最後に各頂点の値を一括で得る
// U : 足す値の型
// TreeImos<long long, long long> im(t);
//   im.add_subtree(v, x)  v の部分木全体に x を足す      O(1)
//   im.add_path(u, v, x)  u-v パス上の全頂点に x を足す   O(log N)（LCA を使う）
//   im.build()            各頂点の最終的な値           O(N)
// 例: 「パス u-v を通る回数」= 各パスに add_path(u, v, 1)
// ◆ struct TreeImos : 木の上のいもす法（部分木やパスへの加算をまとめて行い、最後に各頂点の値を求める）
//   TreeImos(t)          : t は build 済みの Tree（参照で持つので t を先に作って残しておく）
//   add_subtree(v, x)    : v の部分木の全頂点に x を足す。O(1)
//   add_path(u, v, x)    : u-v パス上の全頂点に x を足す。O(log N)
//   build()              : 各頂点の値を返す。O(N)
//   U : 足す値の型、T : Tree の重みの型
template <class U, class T>
struct TreeImos {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree_imos.cpp
    const Tree<T>& t;
    vector<U> ds, dp;  // ds: 部分木加算用、dp: パス加算用
    // t_ : build 済みの Tree
    TreeImos(const Tree<T>& t_) : t(t_), ds(t_.n, U(0)), dp(t_.n, U(0)) {}
    // add_subtree(v, x) : v : 部分木の根、x : 足す値
    void add_subtree(int v, U x) { ds[v] += x; }  // ★ 根の印だけ付けて、後で下に流す
    // add_path(u, v, x) : u, v : パスの両端（u == v なら u だけ）、x : 足す値
    void add_path(int u, int v, U x) {
        int l = t.lca(u, v);
        dp[u] += x, dp[v] += x;                // ★ 両端に +x
        dp[l] -= x;                            // ★ LCA で 2 回分になるので 1 回引く
        if (t.par[l] != -1) dp[t.par[l]] -= x;  // ★ LCA の親より上には流さない
    }
    // build() : 返り値 res[v] = 頂点 v に足された値の合計
    vector<U> build() const {
        auto s = ds, p = dp;
        for (int v : t.order)
            if (t.par[v] != -1) s[v] += s[t.par[v]];  // ★ 部分木加算: 根 → 葉 に累積
        for (int i = t.n - 1; i > 0; i--) {
            int v = t.order[i];
            p[t.par[v]] += p[v];  // ★ パス加算: 葉 → 根 に累積
        }
        vector<U> res(t.n);
        for (int v = 0; v < t.n; v++) res[v] = s[v] + p[v];
        return res;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    //       0
    //    2/   \3
    //    1     2
    //  4/ \1    \10
    //  3   4     5
    Tree<long long> t(6);
    t.add_edge(0, 1, 2), t.add_edge(0, 2, 3), t.add_edge(1, 3, 4), t.add_edge(1, 4, 1), t.add_edge(2, 5, 10);
    t.build(0);
    // 部分木 1 に +1、パス 3-5 に +10、パス 4-4 に +100
    TreeImos<long long, long long> im(t);
    im.add_subtree(1, 1);
    im.add_path(3, 5, 10);
    im.add_path(4, 4, 100);
    assert((im.build() == vector<long long>{10, 11, 10, 11, 101, 10}));

    // ランダムテスト: 親をたどる愚直な方法と比べる（根もいろいろ）
    mt19937 rr(119);
    for (int it = 0; it < 1000; it++) {
        int n = rr() % 15 + 1;
        Tree<long long> tt(n);
        for (int v = 1; v < n; v++) tt.add_edge(rr() % v, v, rr() % 10);
        tt.build(rr() % n);
        auto& P = tt.par;
        auto is_desc = [&](int u, int v) { while (u != -1 && u != v) u = P[u]; return u == v; };  // u は v の子孫か
        TreeImos<long long, long long> im2(tt);
        vector<long long> ref(n, 0);
        for (int k = 0; k < 10; k++) {
            long long x = (long long)(rr() % 11) - 5;
            if (rr() % 2) {
                int v = rr() % n;
                im2.add_subtree(v, x);
                for (int u = 0; u < n; u++) if (is_desc(u, v)) ref[u] += x;
            } else {
                int a = rr() % n, b = rr() % n;
                im2.add_path(a, b, x);
                for (auto y : tt.path(a, b)) ref[y] += x;
            }
        }
        assert(im2.build() == ref);
    }
    puts("OK");
}
