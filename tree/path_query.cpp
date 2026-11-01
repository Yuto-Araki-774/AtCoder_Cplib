// =====================================================================
// tree/path_query.cpp  木のパス上の値をまとめる（ダブリング。値の更新なし）
//   u-v パス上の辺の重み（または頂点の値）の最大・最小・和・gcd などを、1 回 O(log N) で答える
//   LCA のダブリングの表 up[k][v]（v の 2^k 個上）と同じ形で、「v から 2^k 本上までの値をまとめたもの」も持つ
//   値の更新があるなら hld.cpp（HL 分解）と ACL の segtree
// 依存: tree/tree.cpp の Tree（先に貼る）。Tree は build 済みであること
// ABC / ARC での使用例:
//   ABC235 E - MST + 1  https://atcoder.jp/contests/abc235/tasks/abc235_e
//     → 最小全域木（graph/kruskal.cpp）を Tree にして、パス上の辺の重みの最大を TreePathQuery で。
//        クエリの辺の重み w が「u-v パスの最大」より小さければ Yes（u == v なら No）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;
// テスト用: tree.cpp の Tree を読み込む（提出するときは、tree.cpp のコピー範囲をこのファイルのコピー範囲より先に貼る）
#define TREE_NO_MAIN
#include "tree.cpp"

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体（tree.cpp の Tree が必要） ----------
// S, op, e は ACL の segtree と同じ書き方（グローバルに関数を書く）。例: パス上の辺の重みの最大
//   long long op_max(long long a, long long b) { return max(a, b); }
//   long long e_max() { return LLONG_MIN; }
// Tree<> t(n); t.add_edge(a, b, w); ...; t.build(root);
// TreePathQuery<long long, op_max, e_max> pq(t);        辺の重みで（u-v パスの辺をまとめる）
// TreePathQuery<long long, op_max, e_max> pv(t, a);     頂点の値 a[v] で（u, v と途中の頂点をまとめる）
//   pq.prod(u, v)          u-v パス上の値をまとめたもの。パスに何もなければ e()（辺で u == v のとき）
// ◆ struct TreePathQuery<S, op, e, T> : 木のパス上の値をまとめる（値の更新なし）。前計算 O(N log N)、1 回 O(log N)
//   TreePathQuery(t)    : 辺の重みを使う。t は build 済みの Tree（Tree を継承したもの HLD などでもよい）
//   TreePathQuery(t, a) : 頂点の値 a[v]（長さ N）を使う
//   prod(u, v)          : u-v パス上の値を op でまとめたもの
//   注意 : op は順番によらない演算であること（max, min, +, gcd, xor など）。パスの向きは区別しない
//          辺の重み T から S に変換できること（S が T と同じなら気にしなくてよい）
//   T : Tree の辺の重みの型（既定 long long）
template <class S, S (*op)(S, S), S (*e)(), class T = long long>
struct TreePathQuery {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/path_query.cpp
    const Tree<T>& t;
    bool vertex;            // true なら頂点の値、false なら辺の重み
    vector<S> a;            // 頂点の値（vertex のときだけ）
    vector<vector<S>> agg;  // agg[k][v] = v から 2^k 個上へ進む間の値をまとめたもの（辺なら 2^k 本の辺、頂点なら v を含む 2^k 個）
    // t_ : build 済みの Tree（参照で持つので、t_ を先に作って残しておく）
    TreePathQuery(const Tree<T>& t_) : t(t_), vertex(false) {
        vector<S> base(t.n, e());
        for (int v = 0; v < t.n; v++)
            for (auto [to, w] : t.adj[v])
                if (to == t.par[v]) base[v] = S(w);  // ★ 頂点 v に「v と親をつなぐ辺」の重みを置く（根は e()）
        init(base);
    }
    // t_ : build 済みの Tree、a_ : 各頂点の値（長さ N）
    TreePathQuery(const Tree<T>& t_, const vector<S>& a_) : t(t_), vertex(true), a(a_) { init(a_); }
    // （内部用）ダブリングの表を作る
    void init(const vector<S>& base) {
        agg.assign(t.LOG, base);
        for (int k = 0; k + 1 < t.LOG; k++)
            for (int v = 0; v < t.n; v++) agg[k + 1][v] = op(agg[k][v], agg[k][t.up[k][v]]);  // ★ 2^k 上までと、その先の 2^k 上まで
    }
    // prod(u, v) : u, v : パスの両端
    S prod(int u, int v) const {
        S res = e();
        if (t.dep[u] < t.dep[v]) swap(u, v);
        for (int k = 0, diff = t.dep[u] - t.dep[v]; diff > 0; k++, diff >>= 1)
            if (diff & 1) res = op(res, agg[k][u]), u = t.up[k][u];  // ★ 深い方を同じ深さまで上げながらまとめる
        if (u != v) {
            for (int k = t.LOG - 1; k >= 0; k--)
                if (t.up[k][u] != t.up[k][v]) res = op(res, op(agg[k][u], agg[k][v])), u = t.up[k][u], v = t.up[k][v];
            res = op(res, op(agg[0][u], agg[0][v]));  // ★ LCA の 1 つ下から LCA へ（辺なら 2 本、頂点なら u と v 自身）
            u = t.up[0][u];                            // u = LCA
        }
        if (vertex) res = op(res, a[u]);  // 頂点なら LCA の値も入れる
        return res;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
long long op_max(long long a, long long b) { return max(a, b); }
long long e_max() { return LLONG_MIN; }
long long op_min(long long a, long long b) { return min(a, b); }
long long e_min() { return LLONG_MAX; }
long long op_sum(long long a, long long b) { return a + b; }
long long e_sum() { return 0; }
int main() {
    //       0
    //    2/   \3
    //    1     2
    //  4/ \1    \10
    //  3   4     5
    Tree<long long> t(6);
    t.add_edge(0, 1, 2), t.add_edge(0, 2, 3), t.add_edge(1, 3, 4), t.add_edge(1, 4, 1), t.add_edge(2, 5, 10);
    t.build(0);
    TreePathQuery<long long, op_max, e_max> mx(t);
    TreePathQuery<long long, op_min, e_min> mn(t);
    TreePathQuery<long long, op_sum, e_sum> sm(t);
    assert(mx.prod(3, 5) == 10 && mn.prod(3, 5) == 2 && sm.prod(3, 5) == 19 && sm.prod(3, 5) == t.wdist(3, 5));
    assert(mx.prod(3, 4) == 4 && mx.prod(4, 4) == LLONG_MIN);  // 辺で u == v なら e()
    vector<long long> val = {5, 1, 9, 2, 7, 3};
    TreePathQuery<long long, op_max, e_max> vx(t, val);
    assert(vx.prod(3, 4) == 7 && vx.prod(3, 5) == 9 && vx.prod(4, 4) == 7);

    // ランダムテスト: Tree::path で頂点の並びを出して愚直にまとめたものと比べる（根もいろいろ）
    mt19937 rng(235);
    for (int it = 0; it < 1500; it++) {
        int n = rng() % 20 + 1;
        Tree<long long> u(n);
        vector<vector<long long>> w(n, vector<long long>(n, 0));
        for (int v = 1; v < n; v++) {
            int p = rng() % v;
            long long c = rng() % 21 - 10;
            u.add_edge(p, v, c), w[p][v] = w[v][p] = c;
        }
        u.build(rng() % n);
        vector<long long> b(n);
        for (auto& x : b) x = rng() % 21 - 10;
        TreePathQuery<long long, op_max, e_max> qmx(u), vmx(u, b);
        TreePathQuery<long long, op_sum, e_sum> qsm(u), vsm(u, b);
        TreePathQuery<long long, op_min, e_min> vmn(u, b);
        for (int x = 0; x < n; x++)
            for (int y = 0; y < n; y++) {
                auto p = u.path(x, y);
                long long emx = LLONG_MIN, esm = 0, bmx = LLONG_MIN, bsm = 0, bmn = LLONG_MAX;
                for (int i = 0; i + 1 < (int)p.size(); i++) emx = max(emx, w[p[i]][p[i + 1]]), esm += w[p[i]][p[i + 1]];
                for (auto v : p) bmx = max(bmx, b[v]), bsm += b[v], bmn = min(bmn, b[v]);
                assert(qmx.prod(x, y) == emx && qsm.prod(x, y) == esm);
                assert(vmx.prod(x, y) == bmx && vsm.prod(x, y) == bsm && vmn.prod(x, y) == bmn);
            }
    }
    puts("OK");
}
