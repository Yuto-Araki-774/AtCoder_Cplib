// =====================================================================
// ds/li_chao_tree.cpp  Li Chao Tree（直線・線分を追加して、ある x での最小値を求める）
//   「dp[i] = min_j (dp[j] + a_j × x_i + b_j)」のような DP を、直線 y = a_j x + b_j の集まりへの最小値クエリにして速くする
//   （Convex Hull Trick と同じことができ、傾きや x の順番に条件がいらない）
//   直線の追加 O(log N)、線分の追加 O(log^2 N)、最小値 O(log N)。N = 使う x 座標の個数
// 依存: なし
// ABC / ARC での使用例:
//   EDPC Z - Frog 3  https://atcoder.jp/contests/dp/tasks/dp_z
//     → 使用例と同じ。直線 (−2 h_i, dp[i] + h_i^2) を入れて x = h_j の最小値（ABC ではないが Li Chao / CHT の定番）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// T : 座標・値の型（long long）。a × x + b が T に収まること
// LiChaoTree<long long> lc(xs);   xs = クエリで使う x 座標の一覧（先に全部わかっている必要がある。重複・順不同 OK）
//   lc.add_line(a, b)             直線 y = a x + b を追加
//   lc.add_segment(a, b, l, r)    l <= x < r の範囲だけにある線分 y = a x + b を追加（削除可）
//   lc.get(x)                     x での最小値（x は xs に入っている値。まだ何もないなら LICHAO_INF<T>）
// 最大値がほしいとき: add_line(-a, -b) で入れて、-get(x) を答えにする
template <class T> constexpr T LICHAO_INF = numeric_limits<T>::max();
template <class T>
struct LiChaoTree {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/li_chao_tree.cpp
    int sz;
    vector<T> xs, A, B;
    vector<bool> has;
    LiChaoTree(vector<T> xs_) : xs(xs_) {
        sort(xs.begin(), xs.end());
        xs.erase(unique(xs.begin(), xs.end()), xs.end());
        if (xs.empty()) xs.push_back(0);
        sz = 1;
        while (sz < (int)xs.size()) sz <<= 1;
        xs.resize(sz, xs.back());  // 2 冪にそろえる（余りは最後の x で埋める）
        A.assign(2 * sz, 0), B.assign(2 * sz, 0), has.assign(2 * sz, false);
    }
    T eval(T a, T b, T x) const { return a * x + b; }
    // ノード k（x の番号 [l, r) を担当）から下へ、直線 (a, b) を入れる
    void add_at(int k, int l, int r, T a, T b) {
        while (true) {
            if (!has[k]) { A[k] = a, B[k] = b, has[k] = true; return; }
            int m = (l + r) / 2;
            bool lef = eval(a, b, xs[l]) < eval(A[k], B[k], xs[l]);
            bool rig = eval(a, b, xs[r - 1]) < eval(A[k], B[k], xs[r - 1]);
            if (lef && rig) { A[k] = a, B[k] = b; return; }  // ★ 区間全体で新しい直線の方が小さい → 置き換え
            if (!lef && !rig) return;                          // ★ 区間全体で古い方が小さい → 捨てる
            bool mid = eval(a, b, xs[m]) < eval(A[k], B[k], xs[m]);
            if (mid) swap(A[k], a), swap(B[k], b), lef = !lef;  // ★ 真ん中で小さい方をノードに残す
            // 残った直線が勝つのは片側だけ（2 本の直線は 1 回しか交わらない）→ そちらへ降りる
            if (lef) k = 2 * k, r = m;
            else k = 2 * k + 1, l = m;
        }
    }
    void add_line(T a, T b) { add_at(1, 0, sz, a, b); }
    void add_segment(T a, T b, T xl, T xr) {  // 削除可（線分を使わないなら不要）
        int li = lower_bound(xs.begin(), xs.end(), xl) - xs.begin();
        int ri = lower_bound(xs.begin(), xs.end(), xr) - xs.begin();
        vector<array<int, 3>> st = {{1, 0, sz}};  // ★ セグ木と同じように [li, ri) をノードに分けて、それぞれに直線として入れる
        while (!st.empty()) {
            auto [k, l, r] = st.back();
            st.pop_back();
            if (r <= li || ri <= l) continue;
            if (li <= l && r <= ri) { add_at(k, l, r, a, b); continue; }
            int m = (l + r) / 2;
            st.push_back({2 * k, l, m}), st.push_back({2 * k + 1, m, r});
        }
    }
    T get(T x) const {
        int i = lower_bound(xs.begin(), xs.end(), x) - xs.begin();
        assert(i < sz && xs[i] == x);  // x は最初に渡した xs の中の値であること
        T res = LICHAO_INF<T>;
        for (int k = i + sz; k >= 1; k >>= 1)  // ★ 葉から根まで、通るノードの直線をすべて見る
            if (has[k]) res = min(res, eval(A[k], B[k], x));
        return res;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // y = x、y = -x + 4、y = 3 の最小値: x = 0 → 0、x = 2 → 2、x = 5 → -1
    {
        LiChaoTree<long long> lc({0, 2, 5});
        assert(lc.get(2) == LICHAO_INF<long long>);
        lc.add_line(1, 0), lc.add_line(-1, 4), lc.add_line(0, 3);
        assert(lc.get(0) == 0 && lc.get(2) == 2 && lc.get(5) == -1);
        // 線分 y = -10（2 <= x < 5 だけ）: x = 2 は -10、x = 5 は範囲外なので -1 のまま
        lc.add_segment(0, -10, 2, 5);
        assert(lc.get(0) == 0 && lc.get(2) == -10 && lc.get(5) == -1);
    }
    // 使用例: カエルの DP（EDPC Z と同じ形）
    //   dp[j] = min_{i<j} dp[i] + (h_j − h_i)^2 + C = h_j^2 + C + min_i (−2 h_i × h_j + dp[i] + h_i^2)
    //   → 直線 (−2 h_i, dp[i] + h_i^2) を追加しながら、x = h_j の最小値を取る
    //   h = 1 2 3 4 5、C = 6 → 1 → 3 → 5 と 2 つずつ飛ぶのが最小で (4 + 6) × 2 = 20（1 → 5 は 16 + 6 = 22、1 つずつは 7 × 4 = 28）
    {
        vector<long long> h = {1, 2, 3, 4, 5};
        long long C = 6;
        int n = h.size();
        LiChaoTree<long long> lc(h);
        vector<long long> dp(n);
        dp[0] = 0;
        lc.add_line(-2 * h[0], dp[0] + h[0] * h[0]);
        for (int j = 1; j < n; j++) {
            dp[j] = lc.get(h[j]) + h[j] * h[j] + C;
            lc.add_line(-2 * h[j], dp[j] + h[j] * h[j]);
        }
        assert(dp[n - 1] == 20);
    }
    // ランダムテスト: 直線・線分の追加と最小値を、全部の直線を見る素朴な方法と比較
    {
        mt19937 rr(151);
        for (int it = 0; it < 3000; it++) {
            int nx = rr() % 12 + 1;
            vector<long long> xs;
            for (int i = 0; i < nx; i++) xs.push_back((long long)(rr() % 41) - 20);
            LiChaoTree<long long> lc(xs);
            vector<array<long long, 4>> segs;  // a, b, l, r
            for (int q = 0; q < 30; q++) {
                int t = rr() % 3;
                if (t == 0) {
                    long long a = (long long)(rr() % 21) - 10, b = (long long)(rr() % 201) - 100;
                    lc.add_line(a, b), segs.push_back({a, b, LLONG_MIN, LLONG_MAX});
                } else if (t == 1) {
                    long long a = (long long)(rr() % 21) - 10, b = (long long)(rr() % 201) - 100;
                    long long l = (long long)(rr() % 45) - 22, r = (long long)(rr() % 45) - 22;
                    if (l > r) swap(l, r);
                    lc.add_segment(a, b, l, r), segs.push_back({a, b, l, r});
                } else {
                    long long x = xs[rr() % nx], want = LICHAO_INF<long long>;
                    for (auto [a, b, l, r] : segs) if (l <= x && x < r) want = min(want, a * x + b);
                    assert(lc.get(x) == want);
                }
            }
            for (long long x : xs) {
                long long want = LICHAO_INF<long long>;
                for (auto [a, b, l, r] : segs) if (l <= x && x < r) want = min(want, a * x + b);
                assert(lc.get(x) == want);
            }
        }
    }
    puts("OK");
}
