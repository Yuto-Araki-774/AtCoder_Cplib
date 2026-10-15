// =====================================================================
// math/gauss.cpp  ガウスの消去法（連立一次方程式・行列の階数・行列式）
//   本体A（mod p）: 係数が mint の連立方程式 Ax = b を解く。解がないか・解の自由度（解の個数 = p^(自由度)）も。O(N M min(N, M))
//   本体B（F2 = 0/1 と xor）: 「スイッチを押すとランプが反転」型。bitset で 64 倍速い
//   削除可: 行列式（mod p）、行列木定理（全域木の個数・有向全域木の個数。行列式を使う）
//   A・B は互いに独立
// 依存: 本体A は ACL の modint（mint）。B は依存なし
// ABC / ARC での使用例:
//   ABC253 Ex - We Love Forest  https://atcoder.jp/contests/abc253/tasks/abc253_h
//     → 頂点の部分集合 S ごとに、S の中の辺だけでの全域木の個数を count_spanning_trees で求め、部分集合 DP で森の数を数える（N ≤ 14）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/modint>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: mod p の連立一次方程式 ----------
// T : mint（mod は素数。割り算を使うので）
// A : N 行 M 列（N 本の式、M 個の未知数）、b : 長さ N
// auto r = gauss_solve(A, b);
//   r.ok      解があるか
//   r.x       解の 1 つ（自由に決められる未知数は 0 にしたもの）
//   r.rank    A の階数（独立な式の本数）
//   r.basis   Ax = 0 の解の基底（M − rank 本）。全部の解 = x + (basis の好きな線形結合)
//   解の個数 = p^(M − rank)（ok のとき）
// ◆ GaussResult<T> : gauss_solve の結果。ok : 解があるか、x : 解の 1 つ、rank : 階数、basis : Ax = 0 の解の基底
template <class T>
struct GaussResult {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/gauss.cpp
    bool ok;
    vector<T> x;
    int rank;
    vector<vector<T>> basis;
};
// ◆ gauss_solve(A, b) : 連立一次方程式 Ax = b（mod 素数）。O(N M min(N, M))
//   A : N 行 M 列の係数（vector<vector<mint>>。各行の長さは M でそろえる）
//   b : 長さ N の右辺
//   T : mint（mod は素数であること。割り算を使う）
//   返り値 : ok, x（自由な未知数は 0 にした解）, rank, basis。解の個数 = p^(M − rank)
template <class T>
GaussResult<T> gauss_solve(vector<vector<T>> A, vector<T> b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/gauss.cpp
    int n = A.size(), m = n ? A[0].size() : 0;
    for (int i = 0; i < n; i++) A[i].push_back(b[i]);  // 右辺をくっつけた拡大係数行列
    vector<int> piv;  // piv[k] = k 行目の先頭の 1 がある列
    int r = 0;
    for (int c = 0; c < m && r < n; c++) {
        int p = -1;
        for (int i = r; i < n; i++) if (A[i][c] != 0) { p = i; break; }
        if (p == -1) continue;  // この列は全部 0 → この未知数は自由に決められる
        swap(A[p], A[r]);
        T inv = A[r][c].inv();
        for (int j = c; j <= m; j++) A[r][j] *= inv;  // ★ 先頭を 1 にする
        for (int i = 0; i < n; i++) {                  // ★ 他の行のこの列を 0 にする（上の行も消す = 掃き出し法）
            if (i == r || A[i][c] == 0) continue;
            T f = A[i][c];
            for (int j = c; j <= m; j++) A[i][j] -= f * A[r][j];
        }
        piv.push_back(c), r++;
    }
    GaussResult<T> res{true, vector<T>(m, T(0)), r, {}};
    for (int i = r; i < n; i++)
        if (A[i][m] != 0) { res.ok = false; return res; }  // ★ 0 = (0 でない数) の式が残った → 解なし
    for (int k = 0; k < r; k++) res.x[piv[k]] = A[k][m];
    vector<bool> is_piv(m, false);
    for (int c : piv) is_piv[c] = true;
    for (int f = 0; f < m; f++) {  // 自由な未知数 f を 1、他の自由な未知数を 0 にした Ax = 0 の解
        if (is_piv[f]) continue;
        vector<T> v(m, T(0));
        v[f] = 1;
        for (int k = 0; k < r; k++) v[piv[k]] = -A[k][f];
        res.basis.push_back(v);
    }
    return res;
}

// ---------- 削除可: 行列式（mod p） O(N^3) ----------
// ◆ determinant(A) : 行列式（mod 素数）。O(N^3)
//   A : N × N の正方行列
template <class T>
T determinant(vector<vector<T>> A) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/gauss.cpp
    int n = A.size();
    T det = 1;
    for (int c = 0; c < n; c++) {
        int p = -1;
        for (int i = c; i < n; i++) if (A[i][c] != 0) { p = i; break; }
        if (p == -1) return 0;
        if (p != c) swap(A[p], A[c]), det = -det;  // ★ 行を入れ替えると符号が反転
        det *= A[c][c];
        T inv = A[c][c].inv();
        for (int i = c + 1; i < n; i++) {
            T f = A[i][c] * inv;
            for (int j = c; j < n; j++) A[i][j] -= f * A[c][j];  // 下の行から引いても行列式は変わらない
        }
    }
    return det;
}

// ---------- 本体B: F2（xor）の連立一次方程式 ----------
// W : 列数の上限（未知数の個数 M より大きく。例: 2048）
// A[i] : i 本目の式の係数（bitset。A[i][j] = 1 なら未知数 j を含む）、b[i] : 右辺（0 / 1）
// auto r = gauss_f2<W>(A, b, M);
//   r.ok, r.x（解の 1 つ。vector<int> の 0 / 1）, r.rank。解の個数 = 2^(M − rank)
// ランプの問題: 「スイッチ j を押すとランプ i が反転」→ A[i][j] = 1、「ランプ i を最終的に反転させたいか」→ b[i]
// ◆ GaussF2Result : gauss_f2 の結果。ok, x（0 / 1 の解の 1 つ）, rank
struct GaussF2Result {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/gauss.cpp
    bool ok;
    vector<int> x;
    int rank;
};
// ◆ gauss_f2<W>(A, b, m) : F2（0 / 1 と xor）の連立一次方程式。bitset で速い。O(N M min(N, M) / 64)
//   W : bitset の大きさ（m より大きい定数。例 2048）
//   A : N 本の式（A[i][j] = 1 なら式 i に未知数 j が入る）
//   b : 右辺（0 / 1、長さ N）
//   m : 未知数の個数（m < W であること。assert あり）
template <size_t W>
GaussF2Result gauss_f2(vector<bitset<W>> A, vector<int> b, int m) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/gauss.cpp
    assert((size_t)m < W);
    int n = A.size();
    for (int i = 0; i < n; i++) A[i][m] = b[i];  // 右辺を m 列目に入れる
    vector<int> piv;
    int r = 0;
    for (int c = 0; c < m && r < n; c++) {
        int p = -1;
        for (int i = r; i < n; i++) if (A[i][c]) { p = i; break; }
        if (p == -1) continue;
        swap(A[p], A[r]);
        for (int i = 0; i < n; i++)
            if (i != r && A[i][c]) A[i] ^= A[r];  // ★ F2 では引き算 = xor。行ごと bitset でまとめて
        piv.push_back(c), r++;
    }
    GaussF2Result res{true, vector<int>(m, 0), r};
    for (int i = r; i < n; i++)
        if (A[i][m]) { res.ok = false; return res; }
    for (int k = 0; k < r; k++) res.x[piv[k]] = A[k][m];
    return res;
}

// ---------- 削除可: 行列木定理（determinant が必要） O(N^3) ----------
// count_spanning_trees<mint>(n, edges)        無向グラフの全域木の個数（多重辺は別々に数える。自己ループは無視）
// count_arborescences<mint>(n, edges, root)   有向グラフで、root から全頂点へ向かう有向全域木の個数（辺 a → b）
// ★ ラプラシアン行列 L（対角 = 次数、(a, b) = −(a と b の間の辺の本数)）から 1 行 1 列を消した行列式 = 全域木の個数
//    有向なら 対角 = 入次数、(a, b) = −(a → b の本数) にして root の行と列を消す
// ◆ count_spanning_trees<mint>(n, edges) : 無向グラフの全域木の個数（行列木定理）。O(N^3)
//   n     : 頂点数
//   edges : 無向辺 {a, b} の一覧（多重辺は別々に数える。自己ループは無視）
template <class T>
T count_spanning_trees(int n, const vector<pair<int, int>>& edges) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/gauss.cpp
    if (n <= 1) return T(1);
    vector<vector<T>> L(n - 1, vector<T>(n - 1, T(0)));  // 頂点 0 の行と列を消したもの（頂点 v → 添字 v − 1）
    for (auto [a, b] : edges) {
        if (a == b) continue;
        if (a > 0) L[a - 1][a - 1] += 1;
        if (b > 0) L[b - 1][b - 1] += 1;
        if (a > 0 && b > 0) L[a - 1][b - 1] -= 1, L[b - 1][a - 1] -= 1;
    }
    return determinant(L);
}
// ◆ count_arborescences<mint>(n, edges, root) : 有向グラフで root から全頂点へ向かう有向全域木の個数。O(N^3)
//   n     : 頂点数
//   edges : 有向辺 {a, b}（a → b）の一覧
//   root  : 根
template <class T>
T count_arborescences(int n, const vector<pair<int, int>>& edges, int root) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/gauss.cpp
    if (n <= 1) return T(1);
    auto id = [&](int v) { return v < root ? v : v - 1; };  // root を消した添字
    vector<vector<T>> L(n - 1, vector<T>(n - 1, T(0)));
    for (auto [a, b] : edges) {
        if (a == b || b == root) continue;  // root に入る辺は使わない
        L[id(b)][id(b)] += 1;                // ★ 入次数
        if (a != root) L[id(a)][id(b)] -= 1;
    }
    return determinant(L);
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
using mint = atcoder::modint998244353;
using m5 = atcoder::static_modint<5>;  // テスト用（全部の x を試せるよう小さい素数）
int main() {
    // x + y = 3, x − y = 1 → x = 2, y = 1
    {
        auto r = gauss_solve<mint>({{1, 1}, {1, -1}}, {3, 1});
        assert(r.ok && r.rank == 2 && r.x[0] == 2 && r.x[1] == 1 && r.basis.empty());
        // x + y = 1, 2x + 2y = 3 → 解なし
        assert(!gauss_solve<mint>({{1, 1}, {2, 2}}, {1, 3}).ok);
        // x + y + z = 6 だけ → rank 1、自由度 2
        auto s = gauss_solve<mint>({{1, 1, 1}}, {6});
        assert(s.ok && s.rank == 1 && s.basis.size() == 2);
        // 行列式: [[2, 1], [7, 4]] → 8 − 7 = 1、[[0, 1], [1, 0]] → −1
        assert(determinant<mint>({{2, 1}, {7, 4}}) == 1 && determinant<mint>({{0, 1}, {1, 0}}) == -1);
    }
    // F2: ランプ 3 個、スイッチ 3 個。スイッチ j は ランプ j と j+1 を反転。全部つけたい
    //   x0 = 1、x0 + x1 = 1、x1 + x2 = 1 → x = (1, 0, 1)
    {
        vector<bitset<8>> A(3);
        A[0][0] = 1, A[1][0] = A[1][1] = 1, A[2][1] = A[2][2] = 1;
        auto r = gauss_f2<8>(A, {1, 1, 1}, 3);
        assert(r.ok && r.rank == 3 && (r.x == vector<int>{1, 0, 1}));
    }
    // ランダムテスト（mod 5）: x を全部試して、解があるか・解の個数・行列式を比較
    {
        mt19937 rr(160);
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 4 + 1, m = rr() % 4 + 1;
            vector<vector<m5>> A(n, vector<m5>(m));
            vector<m5> b(n);
            for (auto& row : A) for (auto& v : row) v = (rr() % 3 == 0) ? 0 : rr() % 5;
            for (auto& v : b) v = rr() % 5;
            auto r = gauss_solve(A, b);
            int total = 1, cnt = 0;
            for (int i = 0; i < m; i++) total *= 5;
            for (int code = 0; code < total; code++) {
                vector<m5> x(m);
                for (int i = 0, c = code; i < m; i++, c /= 5) x[i] = c % 5;
                bool good = true;
                for (int i = 0; i < n; i++) {
                    m5 s = 0;
                    for (int j = 0; j < m; j++) s += A[i][j] * x[j];
                    if (s != b[i]) good = false;
                }
                cnt += good;
            }
            assert(r.ok == (cnt > 0));
            if (r.ok) {
                int expect = 1;
                for (int i = 0; i < m - r.rank; i++) expect *= 5;
                assert(cnt == expect && (int)r.basis.size() == m - r.rank);
                for (int i = 0; i < n; i++) {  // x が本当に解か、basis が Ax = 0 を満たすか
                    m5 s = 0;
                    for (int j = 0; j < m; j++) s += A[i][j] * r.x[j];
                    assert(s == b[i]);
                    for (auto& v : r.basis) {
                        m5 t = 0;
                        for (int j = 0; j < m; j++) t += A[i][j] * v[j];
                        assert(t == 0);
                    }
                }
            }
            if (n == m) {  // 行列式を置換の和（定義どおり）で
                vector<int> p(n);
                iota(p.begin(), p.end(), 0);
                m5 d = 0;
                do {
                    m5 t = 1;
                    for (int i = 0; i < n; i++) t *= A[i][p[i]];
                    int inv = 0;
                    for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) inv += p[i] > p[j];
                    d += (inv % 2) ? -t : t;
                } while (next_permutation(p.begin(), p.end()));
                assert(determinant(A) == d);
                assert((d != 0) == (r.rank == n));
            }
        }
    }
    // ランダムテスト（F2）: 2^m 通り全部試す
    {
        mt19937 rr(161);
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 7 + 1, m = rr() % 7 + 1;
            vector<bitset<16>> A(n);
            vector<int> b(n);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) A[i][j] = rr() % 2;
                b[i] = rr() % 2;
            }
            auto r = gauss_f2<16>(A, b, m);
            int cnt = 0;
            for (int x = 0; x < (1 << m); x++) {
                bool good = true;
                for (int i = 0; i < n; i++) {
                    int s = 0;
                    for (int j = 0; j < m; j++) s ^= A[i][j] & (x >> j & 1);
                    if (s != b[i]) good = false;
                }
                cnt += good;
            }
            assert(r.ok == (cnt > 0));
            if (r.ok) {
                assert(cnt == (1 << (m - r.rank)));
                for (int i = 0; i < n; i++) {
                    int s = 0;
                    for (int j = 0; j < m; j++) s ^= A[i][j] & r.x[j];
                    assert(s == b[i]);
                }
            }
        }
    }
    // 行列木定理: 4 頂点の完全グラフの全域木は 4^(4−2) = 16 個（ケーリーの公式）。三角形 + 多重辺 1 本（0-1 が 2 本）は 2 + 2 + 1 = 5 個
    {
        vector<pair<int, int>> k4;
        for (int a = 0; a < 4; a++) for (int b = a + 1; b < 4; b++) k4.push_back({a, b});
        assert(count_spanning_trees<mint>(4, k4) == 16);
        assert(count_spanning_trees<mint>(3, {{0, 1}, {0, 1}, {1, 2}, {0, 2}}) == 5);
        assert(count_spanning_trees<mint>(3, {{0, 1}}) == 0);  // つながっていない
        // 有向: 0 → 1, 0 → 2, 1 → 2, 2 → 1 で root = 0 → {0→1, 0→2}, {0→1, 1→2}, {0→2, 2→1} の 3 個
        assert(count_arborescences<mint>(3, {{0, 1}, {0, 2}, {1, 2}, {2, 1}}, 0) == 3);
    }
    // ランダムテスト: 辺の選び方を全部試して数える
    {
        mt19937 r9(191);
        for (int it = 0; it < 2000; it++) {
            int n = r9() % 5 + 1, m = r9() % 9;
            vector<pair<int, int>> es;
            for (int i = 0; i < m; i++) es.push_back({(int)(r9() % n), (int)(r9() % n)});
            long long und = 0, dir = 0;
            int root = r9() % n;
            for (int mask = 0; mask < (1 << m); mask++) {
                if (__builtin_popcount(mask) != n - 1) continue;
                // 無向: n − 1 本で全部つながる = 全域木（自己ループを含むとつながらないので自動的に除かれる）
                vector<int> p(n);
                iota(p.begin(), p.end(), 0);
                function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
                int comps = n;
                for (int i = 0; i < m; i++) if (mask >> i & 1) { int a = f(es[i].first), b = f(es[i].second); if (a != b) p[a] = b, comps--; }
                und += comps == 1;
                // 有向: root 以外の各頂点にちょうど 1 本入り、root から全頂点に行ける
                vector<int> in(n, 0);
                bool ok = true;
                for (int i = 0; i < m; i++) if (mask >> i & 1) { in[es[i].second]++; if (es[i].first == es[i].second || es[i].second == root) ok = false; }
                for (int v = 0; v < n; v++) if (v != root && in[v] != 1) ok = false;
                if (ok) {
                    vector<bool> seen(n, false);
                    vector<int> st = {root};
                    seen[root] = true;
                    while (!st.empty()) {
                        int v = st.back();
                        st.pop_back();
                        for (int i = 0; i < m; i++) if ((mask >> i & 1) && es[i].first == v && !seen[es[i].second]) seen[es[i].second] = true, st.push_back(es[i].second);
                    }
                    for (int v = 0; v < n; v++) if (!seen[v]) ok = false;
                }
                dir += ok;
            }
            assert(count_spanning_trees<mint>(n, es) == und && count_arborescences<mint>(n, es, root) == dir);
        }
    }
    puts("OK");
}
