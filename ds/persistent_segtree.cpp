// =====================================================================
// ds/persistent_segtree.cpp  永続セグ木（更新しても昔の版が残るセグ木）
//   1 点を更新するたびに、根から葉までの O(log N) 個のノードだけを新しく作り、残りは前の版と共有する
//   → どの版に対しても区間の積（op）が O(log N)。メモリ O(N + Q log N)
//   ★ 典型: 版 i = 「a[0..i) を値の位置に足した BIT」とすると、版 r − 版 l で「区間 [l, r) の中の値の分布」が見られる
//      （区間の k 番目・x 未満の個数など。Wavelet Matrix の代わりにもなる）
// 依存: なし
// ABC / ARC での使用例:
//   ABC339 G - Smaller Sum  https://atcoder.jp/contests/abc339/tasks/abc339_g
//     → 前の答えで次のクエリが決まる（オンライン）。値を座標圧縮し、版 i = A[0..i) の値の位置に A を足したもの。答え = 版 R の [0, X 以下) の和 − 版 L−1 の同じ和
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// 【自分で定義するもの】ACL の segtree と同じ S, op（グローバル関数）, e（グローバル関数）
// PersistentSegTree<S, op, e> seg(n);
//   int r0 = seg.build(v);          初期の版（v は長さ n。省略すると全部 e()）。版は「根の番号」(int) で表す
//   int r1 = seg.set(r0, p, x);     版 r0 で a[p] = x にした新しい版（r0 はそのまま残る）
//   seg.get(r, p)                   版 r の a[p]
//   seg.prod(r, l, x)               版 r の op(a[l], ..., a[x−1])
// 版の一覧は自分で vector<int> roots などに入れて持つ
// ◆ struct PersistentSegTree : 永続セグ木（更新しても昔の版が残る）。各操作 O(log N)、メモリ O(N + Q log N)
//   PersistentSegTree(n)   : 長さ n
//   build(v)               : 最初の版を作り、その根の番号を返す（v 省略で全部 e()）
//   set(root, p, x)        : 版 root で a[p] = x にした新しい版の根の番号を返す（root はそのまま残る）
//   get(root, p)           : 版 root の a[p]
//   prod(root, l, r)       : 版 root の op(a[l], …, a[r−1])
//   S, op, e : ACL の segtree と同じ
template <class S, S (*op)(S, S), S (*e)()>
struct PersistentSegTree {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/persistent_segtree.cpp
    struct Node {
        S val;
        int l, r;  // 子のノード番号（葉なら -1）
    };
    int n;
    vector<Node> nd;
    // n_ : 配列の長さ（1 以上）
    PersistentSegTree(int n_) : n(n_) {}
    // build(v) : v : 長さ n の初期値（省略で全部 e()）。何回呼んでもよい（別の版ができる）
    int build(const vector<S>& v = {}) {
        // 再帰の深さは log N なので再帰で書いている
        auto rec = [&](auto self, int lo, int hi) -> int {
            if (hi - lo == 1) {
                nd.push_back({v.empty() ? e() : v[lo], -1, -1});
                return nd.size() - 1;
            }
            int m = (lo + hi) / 2;
            int a = self(self, lo, m), b = self(self, m, hi);
            nd.push_back({op(nd[a].val, nd[b].val), a, b});
            return nd.size() - 1;
        };
        assert(n >= 1);
        return rec(rec, 0, n);
    }
    // set(root, p, x) : root : 元にする版、p : 0..n−1、x : 新しい値
    int set(int root, int p, S x) {
        auto rec = [&](auto self, int k, int lo, int hi) -> int {
            if (hi - lo == 1) {
                nd.push_back({x, -1, -1});
                return nd.size() - 1;
            }
            int m = (lo + hi) / 2;
            int a = nd[k].l, b = nd[k].r;
            if (p < m) a = self(self, a, lo, m);  // ★ 通り道のノードだけ作り直す（反対側の子は前の版と共有）
            else b = self(self, b, m, hi);
            nd.push_back({op(nd[a].val, nd[b].val), a, b});
            return nd.size() - 1;
        };
        assert(0 <= p && p < n);
        return rec(rec, root, 0, n);
    }
    // get(root, p) : p : 0..n−1
    S get(int root, int p) const {
        int k = root, lo = 0, hi = n;
        while (hi - lo > 1) {
            int m = (lo + hi) / 2;
            if (p < m) k = nd[k].l, hi = m;
            else k = nd[k].r, lo = m;
        }
        return nd[k].val;
    }
    // prod(root, l, r) : 0 <= l <= r <= n
    S prod(int root, int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        auto rec = [&](auto self, int k, int lo, int hi) -> S {
            if (r <= lo || hi <= l) return e();
            if (l <= lo && hi <= r) return nd[k].val;
            int m = (lo + hi) / 2;
            return op(self(self, nd[k].l, lo, m), self(self, nd[k].r, m, hi));
        };
        return rec(rec, root, 0, n);
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
long long op_sum(long long a, long long b) { return a + b; }
long long e_sum() { return 0; }
string op_cat(string a, string b) { return a + b; }
string e_cat() { return ""; }
int main() {
    // 版 0: 1 2 3 4、版 1: 版 0 で a[1] = 10、版 2: 版 0 で a[3] = 0（版 1 とは別の枝）
    {
        PersistentSegTree<long long, op_sum, e_sum> seg(4);
        int r0 = seg.build({1, 2, 3, 4});
        int r1 = seg.set(r0, 1, 10), r2 = seg.set(r0, 3, 0);
        assert(seg.prod(r0, 0, 4) == 10 && seg.prod(r1, 0, 4) == 18 && seg.prod(r2, 0, 4) == 6);
        assert(seg.get(r1, 1) == 10 && seg.get(r2, 1) == 2 && seg.prod(r1, 1, 1) == 0);
    }
    // 使用例: 区間 [l, r) の中で x 未満の個数（値を座標として、前から 1 個ずつ足した版を作る）
    //   a = 3 1 4 1 5、[1, 4) = 1 4 1 の中で 4 未満 → 2 個
    {
        vector<int> a = {3, 1, 4, 1, 5};
        PersistentSegTree<long long, op_sum, e_sum> seg(6);  // 値は 0..5
        vector<int> roots = {seg.build()};
        for (int x : a) roots.push_back(seg.set(roots.back(), x, seg.get(roots.back(), x) + 1));
        auto count_less = [&](int l, int r, int x) { return seg.prod(roots[r], 0, x) - seg.prod(roots[l], 0, x); };
        assert(count_less(1, 4, 4) == 2 && count_less(0, 5, 6) == 5 && count_less(2, 3, 4) == 0);
    }
    // ランダムテスト: 版ごとに配列を丸ごと保存した素朴な方法と比較（文字列の連結で順番も確認）
    {
        mt19937 rr(196);
        for (int it = 0; it < 300; it++) {
            int n = rr() % 12 + 1;
            PersistentSegTree<string, op_cat, e_cat> seg(n);
            vector<string> init(n);
            for (auto& s : init) s = string(1, (char)('a' + rr() % 26));
            vector<int> roots = {seg.build(init)};
            vector<vector<string>> arr = {init};
            for (int q = 0; q < 60; q++) {
                int ver = rr() % roots.size();
                if (rr() % 2) {
                    int p = rr() % n;
                    string x(1, (char)('a' + rr() % 26));
                    roots.push_back(seg.set(roots[ver], p, x));
                    arr.push_back(arr[ver]);
                    arr.back()[p] = x;
                } else {
                    int l = rr() % (n + 1), r = rr() % (n + 1);
                    if (l > r) swap(l, r);
                    string want;
                    for (int i = l; i < r; i++) want += arr[ver][i];
                    assert(seg.prod(roots[ver], l, r) == want);
                    int p = rr() % n;
                    assert(seg.get(roots[ver], p) == arr[ver][p]);
                }
            }
            for (size_t v = 0; v < roots.size(); v++)  // 最後に全部の版が壊れていないか
                for (int i = 0; i < n; i++) assert(seg.get(roots[v], i) == arr[v][i]);
        }
    }
    puts("OK");
}
