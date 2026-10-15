// =====================================================================
// basic/sparse_table.cpp  Sparse Table（静的な区間 min / max / gcd を O(1)）
//   構築 O(N log N)、クエリ O(1)。配列が途中で変わらないときに使う
//   値が変わる（更新がある）なら ACL の segtree を使う
// 依存: なし
// ABC / ARC での使用例:
//   ABC189 C - Mandarin Orange  https://atcoder.jp/contests/abc189/tasks/abc189_c
//     → N ≤ 10^4。全区間 (l, r) の min を O(1) で取って O(N^2)（左端を固定して min を更新しながら伸ばしても解ける）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// 【自分で定義するもの】
//   S op(S a, S b) : 結合則 op(op(a,b),c) == op(a,op(b,c)) と
//                    冪等性 op(a, a) == a の両方を満たす演算
//                    OK: min, max, gcd, and(&), or(|)   NG: 和, 積, xor（区間が重なると二重に数える）
//   ※ op は main の外（グローバル）に普通の関数として書く
//   例: int op(int a, int b) { return min(a, b); }
// SparseTable<int, op> st(a);
//   st.prod(l, r) : op(a[l], ..., a[r-1])。l < r であること（空区間は不可）
// ◆ struct SparseTable : 更新のない配列で、区間の min / max / gcd などを O(1) で求める
//   SparseTable(a)   : 前計算 O(N log N)
//   prod(l, r)       : op(a[l], …, a[r−1])。O(1)
template <class S, S (*op)(S, S)>
struct SparseTable {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/sparse_table.cpp
    vector<vector<S>> t;  // t[k][i] = op(a[i], ..., a[i + 2^k - 1])
    vector<int> lg;       // lg[x] = floor(log2(x))
    // a : 元の配列（後から変更できない）
    SparseTable(const vector<S>& a) {
        int n = a.size();
        lg.assign(n + 1, 0);
        for (int i = 2; i <= n; i++) lg[i] = lg[i / 2] + 1;
        t.assign(lg[n] + 1, a);
        for (int k = 0; k < lg[n]; k++)
            for (int i = 0; i + (2 << k) <= n; i++)
                t[k + 1][i] = op(t[k][i], t[k][i + (1 << k)]);  // ★ 長さ 2^(k+1) = 長さ 2^k を 2 つ
    }
    // l, r : 区間 [l, r)。l < r であること（空区間は未定義）
    S prod(int l, int r) const {
        int k = lg[r - l];
        return op(t[k][l], t[k][r - (1 << k)]);  // ★ 長さ 2^k の区間 2 つで [l, r) を覆う（重なってよい）
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int op_min(int a, int b) { return min(a, b); }
int op_gcd(int a, int b) { return gcd(a, b); }
int main() {
    vector<int> a = {5, 2, 8, 6, 3, 9, 1};
    SparseTable<int, op_min> st(a);
    for (int l = 0; l < 7; l++)
        for (int r = l + 1; r <= 7; r++)
            assert(st.prod(l, r) == *min_element(a.begin() + l, a.begin() + r));

    SparseTable<int, op_gcd> sg({12, 18, 24, 7});
    assert(sg.prod(0, 3) == 6 && sg.prod(2, 4) == 1);
    // ランダムテスト: すべての区間の min / gcd を愚直に計算して比較
    {
        mt19937 rr(107);
        for (int it = 0; it < 500; it++) {
            int n = rr() % 20 + 1;
            vector<int> v(n);
            for (auto& x : v) x = rr() % 30 + 1;
            SparseTable<int, op_min> s1(v);
            SparseTable<int, op_gcd> s2(v);
            for (int l = 0; l < n; l++) {
                int mn = INT_MAX, g = 0;
                for (int r = l + 1; r <= n; r++) {
                    mn = min(mn, v[r - 1]), g = gcd(g, v[r - 1]);
                    assert(s1.prod(l, r) == mn && s2.prod(l, r) == g);
                }
            }
        }
    }
    puts("OK");
}
