// =====================================================================
// dp/monotone_minima.cpp  Monotone Minima（各行の最小値の位置が右下がりに並ぶ行列の、全行の最小値）
//   H × W の行列 A の各行の最小値を、全部見る O(HW) ではなく O((H + W) log H) で求める
//   条件: 行 i の最小値の位置（一番左のもの）を pos(i) とすると、pos(0) <= pos(1) <= ... <= pos(H−1)
//   ★ 真ん中の行の最小値の位置 p を求めると、上の行は p 以下、下の行は p 以上だけ見ればよい → 分割統治
//   ★ 使いどころ: 「凸な列との (min, +) 畳み込み」や、Monge な遷移の DP（dp[j] = min_i (dp[i] + cost(i, j)) で cost が Monge）
// 依存: なし
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// 【自分で定義するもの】f(i, j) : 行列の (i, j) 成分（ラムダ。行列を作らずに計算で返せばよい）
// auto pos = monotone_minima<T>(H, W, f);   pos[i] = 行 i の最小値の列（同じ値が複数あれば一番左）
//   最小値そのものは f(i, pos[i])。最大値がほしいなら f を −1 倍する
// ◆ monotone_minima<T>(H, W, f) : 各行の最小値の位置が単調な行列で、全行の最小値の位置を求める。O((H + W) log H)
//   T : 値の型（必ず書く）
//   H : 行数、W : 列数
//   f : T f(int i, int j)。行列の (i, j) 成分（行列を作らず計算で返す）
//   注意 : 行 i の最小値の位置（一番左）が i について単調増加であること（Monge なら成り立つ）
//   返り値 : pos[i] = 行 i の最小値の列
template <class T, class F>
vector<int> monotone_minima(int H, int W, F f) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/monotone_minima.cpp
    vector<int> pos(H, 0);
    if (H == 0 || W == 0) return pos;
    vector<array<int, 4>> st = {{0, H, 0, W}};  // {行の範囲 [t, b), 列の範囲 [l, r)}
    while (!st.empty()) {
        auto [t, b, l, r] = st.back();
        st.pop_back();
        if (t >= b) continue;
        int m = (t + b) / 2, p = l;
        T best = f(m, l);
        for (int j = l + 1; j < r; j++) {  // ★ 真ん中の行だけ、許された列の範囲を全部見る
            T v = f(m, j);
            if (v < best) best = v, p = j;
        }
        pos[m] = p;
        st.push_back({t, m, l, p + 1});  // 上の行は列 p 以下
        st.push_back({m + 1, b, p, r});  // 下の行は列 p 以上
    }
    return pos;
}

// ---------- 削除可: 凸な列との (min, +) 畳み込み O((N + M) log(N + M)) ----------
// c[k] = min_{i + j = k} (a[i] + b[j])（k = 0..N+M−2）。b は下に凸（b[j+1] − b[j] が増えていく）であること。a は何でもよい
// 例: 「品物 j 個で b[j]（凸）」と「別の種類で a[i]」を合わせて、合計 k 個の最小コスト
// ◆ min_plus_convex(a, b) : c[k] = min_{i+j=k} (a[i] + b[j])（長さ N + M − 1）
//   a : 何でもよい列
//   b : 下に凸な列（b[j+1] − b[j] が増えていく）。凸でないと答えが壊れる
template <class T>
vector<T> min_plus_convex(const vector<T>& a, const vector<T>& b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/monotone_minima.cpp
    int n = a.size(), m = b.size();
    if (n == 0 || m == 0) return {};
    // 行 k、列 i の成分 = a[i] + b[k − i]。b が凸なら、範囲内の部分で各行の最小の位置が単調になる
    // 範囲外（k − i が 0..m−1 にない）は「とても大きい値」にしておけば最小に選ばれない
    const T BIG = numeric_limits<T>::max() / 2;
    auto f = [&](int k, int i) -> T {
        int j = k - i;
        if (j < 0 || j >= m) return BIG;
        return a[i] + b[j];  // ★
    };
    auto pos = monotone_minima<T>(n + m - 1, n, f);
    vector<T> c(n + m - 1);
    for (int k = 0; k < n + m - 1; k++) c[k] = f(k, pos[k]);
    return c;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // A[i][j] = (j − 2i)^2（行 i の最小値は列 2i 付近 → 位置が右下がり）。3 × 6: pos = 0, 2, 4
    {
        auto f = [](int i, int j) { return (long long)(j - 2 * i) * (j - 2 * i); };
        assert((monotone_minima<long long>(3, 6, f) == vector<int>{0, 2, 4}));
    }
    // 凸との畳み込み: a = {0, 5, 1}, b = {0, 1, 4}（凸）→ c[0] = 0, c[1] = 1, c[2] = min(4, 6, 1) = 1, c[3] = min(5+4, 1+1) = 2, c[4] = 1 + 4 = 5
    {
        auto c = min_plus_convex<long long>({0, 5, 1}, {0, 1, 4});
        assert((c == vector<long long>{0, 1, 1, 2, 5}));
    }
    // ランダムテスト
    {
        mt19937 rr(183);
        for (int it = 0; it < 3000; it++) {
            // (1) Monge 行列（f(i, j) = g(i) + h(j) + (x_i − y_j)^2、x, y は昇順）→ 最小の位置が単調
            int H = rr() % 8 + 1, W = rr() % 8 + 1;
            vector<long long> x(H), y(W), g(H), hh(W);
            for (auto& v : x) v = rr() % 20;
            for (auto& v : y) v = rr() % 20;
            sort(x.begin(), x.end()), sort(y.begin(), y.end());
            for (auto& v : g) v = rr() % 50;
            for (auto& v : hh) v = rr() % 50;
            auto f = [&](int i, int j) { return g[i] + hh[j] + (x[i] - y[j]) * (x[i] - y[j]); };
            auto pos = monotone_minima<long long>(H, W, f);
            for (int i = 0; i < H; i++) {
                int want = 0;
                for (int j = 1; j < W; j++) if (f(i, j) < f(i, want)) want = j;
                assert(pos[i] == want);
            }
            // (2) 凸との畳み込みを O(NM) と比較
            int n = rr() % 8 + 1, m = rr() % 8 + 1;
            vector<long long> a(n), b(m);
            for (auto& v : a) v = (long long)(rr() % 41) - 20;
            vector<long long> d(m);  // b の差分を増加列にする
            for (auto& v : d) v = (long long)(rr() % 21) - 10;
            sort(d.begin(), d.end());
            b[0] = (long long)(rr() % 21) - 10;
            for (int j = 1; j < m; j++) b[j] = b[j - 1] + d[j];
            vector<long long> want(n + m - 1, LLONG_MAX);
            for (int i = 0; i < n; i++) for (int j = 0; j < m; j++) want[i + j] = min(want[i + j], a[i] + b[j]);
            assert(min_plus_convex(a, b) == want);
        }
    }
    puts("OK");
}
