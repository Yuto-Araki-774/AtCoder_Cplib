// =====================================================================
// tree/euler_tour.cpp  オイラーツアー（行きがけ順の番号付け）
//   各頂点に「入った時刻 in[v]」「出た時刻 out[v]」を振ると、
//   v の部分木 = 時刻 [in[v], out[v]) の区間 になる → 部分木のクエリを列のクエリ（BIT・セグ木）にできる
//   O(N)。再帰なし。tree/tree.cpp とは独立
// 依存: なし（部分木の和などは ACL の fenwick_tree / segtree と組み合わせる）
// ABC / ARC での使用例:
//   ABC202 E - Count Descendants  https://atcoder.jp/contests/abc202/tasks/abc202_e
//     → 深さごとに in[v] を並べ、[in[U], out[U]) に入る個数を二分探索
//   ABC294 G - Distance Queries on a Tree  https://atcoder.jp/contests/abc294/tasks/abc294_g
//     → 辺の重みを子の頂点に乗せ、根からのパスの和を BIT で（+ LCA）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/fenwicktree>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// g : 木の隣接リスト（両方向）
// EulerTour et(g, root);
//   et.in[v], et.out[v]  v の部分木 = 時刻 [in[v], out[v])。部分木のサイズは out[v] - in[v]
//   et.order[t]          時刻 t に入った頂点（order[in[v]] == v）
//   et.par[v], et.dep[v] 親（根は -1）、深さ
//   et.is_ancestor(u, v) u が v の祖先か（u == v も true）
//
// よく使う形（BIT は ACL の fenwick_tree<long long> bit(n)）:
//   部分木の和 : 頂点 v の値は bit.add(in[v], x)、部分木の和は bit.sum(in[v], out[v])
//   根からのパスの和（頂点の値が変わる）:
//       v の値に x を足す → bit.add(in[v], x)、bit.add(out[v], -x)（out[v] < n のとき）
//       根から u までの和 → bit.sum(0, in[u] + 1)
//       ★ v の値は「v の部分木の中の頂点」すべての根からの和に効く、を区間加算で表している
// ◆ struct EulerTour : オイラーツアー（部分木を連続した区間 [in[v], out[v]) にする）。O(N)、再帰なし
//   EulerTour(g, root)   : 前計算
//   in[v], out[v]        : v の部分木 = 時刻 [in[v], out[v])
//   order[t]             : 時刻 t に入った頂点
//   par[v], dep[v]       : 親（根は −1）、深さ
//   is_ancestor(u, v)    : u が v の祖先か（u == v も true）。O(1)
struct EulerTour {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/euler_tour.cpp
    int n;
    vector<int> in, out, order, par, dep;
    // g : 木の隣接リスト（両方向）、root : 根（省略で 0）
    EulerTour(const vector<vector<int>>& g, int root = 0)
        : n(g.size()), in(n), out(n), par(n, -1), dep(n, 0) {
        vector<int> it(n, 0), st = {root};
        int t = 0;
        in[root] = t++, order.push_back(root);
        while (!st.empty()) {
            int v = st.back();
            if (it[v] < (int)g[v].size()) {
                int to = g[v][it[v]++];
                if (to == par[v]) continue;
                par[to] = v, dep[to] = dep[v] + 1;
                in[to] = t++, order.push_back(to);  // ★ 入ったときに時刻を振る
                st.push_back(to);
            } else {
                out[v] = t;  // ★ 部分木をすべて回り終えた時刻。部分木の頂点は in[v] 〜 out[v]-1 に並ぶ
                st.pop_back();
            }
        }
    }
    // is_ancestor(u, v) : u, v : 頂点
    bool is_ancestor(int u, int v) const { return in[u] <= in[v] && out[v] <= out[u]; }  // ★ 区間の包含
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 木の形（0 の子が 1, 2、1 の子が 3, 4、2 の子が 5）:
    //       0
    //      / \ .
    //     1   2
    //    / \   \ .
    //   3   4   5
    vector<vector<int>> g(6);
    auto add = [&](int a, int b) { g[a].push_back(b), g[b].push_back(a); };
    add(0, 1), add(0, 2), add(1, 3), add(1, 4), add(2, 5);
    EulerTour et(g, 0);
    assert((et.order == vector<int>{0, 1, 3, 4, 2, 5}));
    assert(et.in[1] == 1 && et.out[1] == 4 && et.out[0] == 6 && et.out[1] - et.in[1] == 3);
    assert(et.is_ancestor(0, 5) && et.is_ancestor(1, 4) && !et.is_ancestor(1, 5) && et.is_ancestor(2, 2));

    // 部分木の和（点の値の更新あり）
    vector<long long> val = {1, 2, 3, 4, 5, 6};
    atcoder::fenwick_tree<long long> bit(6);
    for (int v = 0; v < 6; v++) bit.add(et.in[v], val[v]);
    assert(bit.sum(et.in[1], et.out[1]) == 2 + 4 + 5);
    bit.add(et.in[4], 10);  // 頂点 4 の値を +10
    assert(bit.sum(et.in[1], et.out[1]) == 21 && bit.sum(et.in[0], et.out[0]) == 31);

    // 根からのパスの和
    atcoder::fenwick_tree<long long> pb(7);  // out[v] == n の場合に備えて n+1
    for (int v = 0; v < 6; v++) pb.add(et.in[v], val[v]), pb.add(et.out[v], -val[v]);
    assert(pb.sum(0, et.in[4] + 1) == 1 + 2 + 5 && pb.sum(0, et.in[5] + 1) == 1 + 3 + 6);

    // ランダムな木で、部分木の頂点集合を愚直と比較
    mt19937 rng(10);
    for (int it = 0; it < 500; it++) {
        int n = rng() % 30 + 1;
        vector<vector<int>> h(n);
        vector<int> p(n, -1);
        for (int v = 1; v < n; v++) p[v] = rng() % v, h[p[v]].push_back(v), h[v].push_back(p[v]);
        EulerTour e(h, 0);
        for (int u = 0; u < n; u++)
            for (int v = 0; v < n; v++) {
                bool anc = false;
                for (int x = v; x != -1; x = p[x]) anc |= x == u;
                assert(e.is_ancestor(u, v) == anc && (anc == (e.in[u] <= e.in[v] && e.in[v] < e.out[u])));
            }
    }
    puts("OK");
}
