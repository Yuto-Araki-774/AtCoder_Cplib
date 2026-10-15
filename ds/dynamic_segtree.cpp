// =====================================================================
// ds/dynamic_segtree.cpp  動的セグ木（添字が巨大でも使えるセグ木）
//   添字の範囲 [L, R) が 1e18 などでも、値を入れた場所の近くだけノードを作る。1 回の操作 O(log(R − L))、メモリ O(Q log(R − L))
//   クエリが全部先にわかるなら「座標圧縮 + ACL の segtree」の方が速い。先にわからない（オンライン）ときに使う
// 依存: なし
// ABC / ARC での使用例:
//   ABC403 G - Odd Position Sum Query  https://atcoder.jp/contests/abc403/tasks/abc403_g
//     → 前の答えで次の値が決まる（オンライン）ので座標圧縮できない。値 x を添字に、{個数, 奇数番目の和, 偶数番目の和} を持つ動的セグ木（左の個数の偶奇で右の奇偶を入れ替えてつなぐ）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// 【自分で定義するもの】ACL の segtree と同じ
//   S       : 値の型
//   op(a,b) : 結合的な演算（グローバル関数）
//   e()     : 単位元（グローバル関数）。値を入れていない場所は e()
// DynamicSegTree<S, op, e> seg(L, R);   添字は L 以上 R 未満（long long）
//   seg.set(p, x)       a[p] = x
//   seg.get(p)          a[p]
//   seg.prod(l, r)      op(a[l], ..., a[r-1])（l == r なら e()）
//   seg.all_prod()      全体
//   1 点に足したいとき: seg.set(p, seg.get(p) + x)
// ◆ struct DynamicSegTree : 添字が巨大（1e18 など）でも使えるセグ木。使う場所だけノードを作る
//   DynamicSegTree(L, R)  : 添字の範囲 [L, R)（long long）
//   set(p, x)             : a[p] = x。O(log(R − L))
//   get(p)                : a[p]（入れていなければ e()）
//   prod(l, r)            : op(a[l], …, a[r−1])
//   all_prod()            : 全体
//   S, op, e : ACL の segtree と同じ（op と e はグローバルな関数）
template <class S, S (*op)(S, S), S (*e)()>
struct DynamicSegTree {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/dynamic_segtree.cpp
    struct Node {
        S val;
        int ch[2];
    };
    long long L, R;
    vector<Node> nd;
    // L_, R_ : 添字の範囲 [L_, R_)。R_ − L_ が long long に収まること
    DynamicSegTree(long long L_, long long R_) : L(L_), R(R_) { nd.push_back({e(), {-1, -1}}); }  // 0 番が根 [L, R)
    // set(p, x) : p : L <= p < R、x : 新しい値
    void set(long long p, S x) {
        assert(L <= p && p < R);
        vector<int> path;
        int v = 0;
        long long lo = L, hi = R;
        while (hi - lo > 1) {  // ★ 根から p の葉まで降りる。ないノードはその場で作る
            path.push_back(v);
            long long mid = lo + (hi - lo) / 2;
            int d = p >= mid;
            if (d) lo = mid;
            else hi = mid;
            if (nd[v].ch[d] == -1) {
                nd[v].ch[d] = nd.size();
                nd.push_back({e(), {-1, -1}});
            }
            v = nd[v].ch[d];
        }
        nd[v].val = x;
        for (int i = (int)path.size() - 1; i >= 0; i--) {  // ★ 下から値を計算し直す（ない子は e()）
            int u = path[i];
            S a = nd[u].ch[0] == -1 ? e() : nd[nd[u].ch[0]].val;
            S b = nd[u].ch[1] == -1 ? e() : nd[nd[u].ch[1]].val;
            nd[u].val = op(a, b);
        }
    }
    // get(p) : p : L <= p < R
    S get(long long p) const {
        assert(L <= p && p < R);
        int v = 0;
        long long lo = L, hi = R;
        while (hi - lo > 1) {
            long long mid = lo + (hi - lo) / 2;
            int d = p >= mid;
            if (d) lo = mid;
            else hi = mid;
            v = nd[v].ch[d];
            if (v == -1) return e();  // ★ ノードがない = 一度も値を入れていない
        }
        return nd[v].val;
    }
    // prod(l, r) : L <= l <= r <= R（l == r なら e()）
    S prod(long long l, long long r) const {
        assert(L <= l && l <= r && r <= R);
        // 左から順に op するために、区間を左から並べて取り出す（スタックに右の子を先に積む）
        S res = e();
        vector<tuple<int, long long, long long>> st = {{0, L, R}};
        while (!st.empty()) {
            auto [v, lo, hi] = st.back();
            st.pop_back();
            if (v == -1 || hi <= l || r <= lo) continue;
            if (l <= lo && hi <= r) { res = op(res, nd[v].val); continue; }  // ★ 完全に含まれるノードはそのまま使う
            long long mid = lo + (hi - lo) / 2;
            st.push_back({nd[v].ch[1], mid, hi});
            st.push_back({nd[v].ch[0], lo, mid});
        }
        return res;
    }
    // all_prod() : 全体の積
    S all_prod() const { return nd[0].val; }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
long long op_sum(long long a, long long b) { return a + b; }
long long e_sum() { return 0; }
string op_cat(string a, string b) { return a + b; }  // 順番が大事な演算（左から順に op しているかの確認用）
string e_cat() { return ""; }
int main() {
    // 添字が 1e18 まで: a[5] = 3, a[1e17] = 10, a[-7] = 1 → [0, 1e18) の和 = 13、[-10, 6) の和 = 4
    {
        DynamicSegTree<long long, op_sum, e_sum> seg(-1000000000000000000LL, 1000000000000000000LL);
        seg.set(5, 3), seg.set(100000000000000000LL, 10), seg.set(-7, 1);
        assert(seg.prod(0, 1000000000000000000LL) == 13 && seg.prod(-10, 6) == 4 && seg.all_prod() == 14);
        assert(seg.get(5) == 3 && seg.get(6) == 0 && seg.prod(6, 6) == 0);
        seg.set(5, seg.get(5) + 2);  // 1 点に足す
        assert(seg.prod(-10, 6) == 6);
    }
    // ランダムテスト: map で持った素朴な配列と比較（和、文字列の連結）
    {
        mt19937_64 rr(170);
        for (int it = 0; it < 300; it++) {
            long long L = (long long)(rr() % 2000000000000000000ULL) - 1000000000000000000LL;
            long long W = (it % 3 == 0) ? (long long)(rr() % 50 + 1) : (long long)(rr() % 1000000000000000000ULL + 1);
            long long R = L + W;
            DynamicSegTree<long long, op_sum, e_sum> s1(L, R);
            DynamicSegTree<string, op_cat, e_cat> s2(L, R);
            map<long long, long long> m1;
            map<long long, string> m2;
            vector<long long> pts;  // 近い場所に集まるように、使う添字の候補を少しだけ作る
            for (int i = 0; i < 6; i++) pts.push_back(L + (long long)(rr() % W));
            pts.push_back(L), pts.push_back(R - 1);
            auto pick = [&]() { long long p = pts[rr() % pts.size()] + (long long)(rr() % 3) - 1; return min(max(p, L), R - 1); };
            for (int q = 0; q < 200; q++) {
                int t = rr() % 3;
                if (t == 0) {
                    long long p = pick(), x = (long long)(rr() % 100) - 50;
                    string c(1, (char)('a' + rr() % 26));
                    s1.set(p, x), s2.set(p, c), m1[p] = x, m2[p] = c;
                } else if (t == 1) {
                    long long p = pick();
                    assert(s1.get(p) == (m1.count(p) ? m1[p] : 0) && s2.get(p) == (m2.count(p) ? m2[p] : ""));
                } else {
                    long long l = pick(), r = pick();
                    if (l > r) swap(l, r);
                    if (rr() % 2) r++;
                    long long w1 = 0;
                    string w2;
                    for (auto it2 = m1.lower_bound(l); it2 != m1.end() && it2->first < r; it2++) w1 += it2->second;
                    for (auto it2 = m2.lower_bound(l); it2 != m2.end() && it2->first < r; it2++) w2 += it2->second;
                    assert(s1.prod(l, r) == w1 && s2.prod(l, r) == w2);
                }
            }
            long long tot = 0;
            for (auto [k, v] : m1) tot += v;
            assert(s1.all_prod() == tot);
        }
    }
    puts("OK");
}
