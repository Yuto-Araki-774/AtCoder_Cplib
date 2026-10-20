// =====================================================================
// ds/sqrt_decomposition.cpp  平方分割
//   配列を長さ約 √N のブロックに分け、ブロック全体は「まとめた情報」で、はみ出した端は 1 つずつ処理する
//   セグ木より遅い（1 回 O(√N)）が、セグ木に載せにくい処理も書ける
//   本体A: 区間加算・区間和・区間最小（遅延評価のセグ木と同じことを平方分割で）  O(√N)
//   本体B: 点の更新 + 「区間の中で x 未満の値の個数」                             O(√N log N)
//          （セグ木では難しい。ブロックごとにソート済みの列を持つ）
//   A と B は互いに独立
// 依存: なし
//   （ABC で平方分割が想定解の問題は確認できていないので例なし）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 区間加算・区間和・区間最小 ----------
// SqrtRangeAdd sd(a);
//   sd.add(l, r, x)    [l, r) に x を足す
//   sd.sum(l, r)       [l, r) の和
//   sd.get_min(l, r)   [l, r) の最小（l < r であること）
// ※ これだけなら ACL の lazy_segtree の方が速い。平方分割の書き方の見本として、また中身を書き換えて使う土台として
// ◆ struct SqrtRangeAdd : 平方分割（区間加算・区間和・区間最小）。各操作 O(√N)
//   SqrtRangeAdd(a)    : 前計算 O(N)
//   add(l, r, x)       : [l, r) に x を足す
//   sum(l, r)          : [l, r) の和
//   get_min(l, r)      : [l, r) の最小（l < r であること）
struct SqrtRangeAdd {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/sqrt_decomposition.cpp
    int n, B;
    vector<long long> a, lazy, bsum, bmin;  // lazy[k] = ブロック k 全体にまとめて足した値
    // v : 初期値（空でもよい）
    SqrtRangeAdd(const vector<long long>& v) : n(v.size()), a(v) {
        B = max(1, (int)sqrt(n));
        int nb = (n + B - 1) / B;
        lazy.assign(nb, 0), bsum.assign(nb, 0), bmin.assign(nb, LLONG_MAX);
        for (int k = 0; k < nb; k++) rebuild(k);
    }
    // （内部用）ブロック k のまとめた情報を作り直す
    void rebuild(int k) {  // ブロック k のまとめた情報を作り直す
        bsum[k] = 0, bmin[k] = LLONG_MAX;
        for (int i = k * B; i < std::min(n, (k + 1) * B); i++) bsum[k] += a[i], bmin[k] = std::min(bmin[k], a[i]);
    }
    // add(l, r, x) : 区間 [l, r)、x : 足す値
    void add(int l, int r, long long x) {
        while (l < r && l % B != 0) a[l] += x, rebuild(l / B), l++;    // ★ 左端のはみ出し: 1 つずつ（rebuild は下でまとめた方が速いが簡単さ優先）
        while (l + B <= r) lazy[l / B] += x, l += B;                    // ★ ブロック全体: lazy に足すだけ O(1)
        while (l < r) a[l] += x, rebuild(l / B), l++;                    // ★ 右端のはみ出し
    }
    // sum(l, r) : 区間 [l, r)（空なら 0）
    long long sum(int l, int r) const {
        long long s = 0;
        while (l < r && l % B != 0) s += a[l] + lazy[l / B], l++;
        while (l + B <= r) s += bsum[l / B] + lazy[l / B] * B, l += B;  // ★ ブロックの和 + まとめて足した分 × 長さ
        while (l < r) s += a[l] + lazy[l / B], l++;
        return s;
    }
    // get_min(l, r) : 区間 [l, r)（空なら LLONG_MAX）
    long long get_min(int l, int r) const {  // min という名前にすると、この構造体の中で std::min と取り違えるので避ける
        long long m = LLONG_MAX;
        while (l < r && l % B != 0) m = std::min(m, a[l] + lazy[l / B]), l++;
        while (l + B <= r) m = std::min(m, bmin[l / B] + lazy[l / B]), l += B;
        while (l < r) m = std::min(m, a[l] + lazy[l / B]), l++;
        return m;
    }
};

// ---------- 本体B: 点の更新 + 区間で x 未満の個数 ----------
// SqrtCountLess sc(a);
//   sc.set(i, x)            a[i] = x                    O(√N)
//   sc.count_less(l, r, x)  [l, r) の中で x 未満の個数   O(√N log N)
// 「x 以下」は count_less(l, r, x + 1)、「x 以上」は (r - l) - count_less(l, r, x)
// ◆ struct SqrtCountLess : 平方分割（1 点の更新、区間で x 未満の個数）
//   SqrtCountLess(a)        : 前計算 O(N log N)
//   set(i, x)               : a[i] = x。O(√N)
//   count_less(l, r, x)     : [l, r) の中で x 未満の個数。O(√(N log N))
struct SqrtCountLess {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/sqrt_decomposition.cpp
    int n, B;
    vector<long long> a;
    vector<vector<long long>> sorted_blk;  // ブロックごとのソート済みの値
    // v : 初期値
    SqrtCountLess(const vector<long long>& v) : n(v.size()), a(v) {
        B = max(1, (int)sqrt(n * max(1.0, log2(max(n, 2)))));  // 二分探索の log の分だけブロックを大きめに
        int nb = (n + B - 1) / B;
        sorted_blk.resize(nb);
        for (int k = 0; k < nb; k++) {
            sorted_blk[k].assign(a.begin() + k * B, a.begin() + std::min(n, (k + 1) * B));
            sort(sorted_blk[k].begin(), sorted_blk[k].end());
        }
    }
    // set(i, x) : i : 位置 0..N−1、x : 新しい値
    void set(int i, long long x) {
        auto& s = sorted_blk[i / B];
        s.erase(lower_bound(s.begin(), s.end(), a[i]));  // ★ 古い値を消して
        s.insert(lower_bound(s.begin(), s.end(), x), x);  // ★ 新しい値を正しい位置に入れる（ブロック内 O(√N)）
        a[i] = x;
    }
    // count_less(l, r, x) : 区間 [l, r)、x : 境目（x 以下なら x + 1 を渡す）
    long long count_less(int l, int r, long long x) const {
        int c = 0;
        while (l < r && l % B != 0) c += a[l] < x, l++;
        while (l + B <= r) {
            auto& s = sorted_blk[l / B];
            c += lower_bound(s.begin(), s.end(), x) - s.begin();  // ★ ブロック全体はソート済みの列で二分探索
            l += B;
        }
        while (l < r) c += a[l] < x, l++;
        return c;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    SqrtRangeAdd sd({5, 1, 4, 2, 3});
    sd.add(1, 4, 10);  // {5, 11, 14, 12, 3}
    assert(sd.sum(0, 5) == 45 && sd.get_min(0, 5) == 3 && sd.get_min(1, 4) == 11);
    SqrtCountLess sc({5, 1, 4, 2, 3});
    assert(sc.count_less(0, 5, 4) == 3);
    sc.set(1, 9);  // {5, 9, 4, 2, 3}
    assert(sc.count_less(0, 5, 4) == 2 && sc.count_less(1, 3, 10) == 2);

    // ランダムテスト: 愚直な配列と比較（ブロックの境目をまたぐ区間が多く出るよう小さい n も含める）
    mt19937 rr(123);
    for (int it = 0; it < 500; it++) {
        int n = rr() % 30 + 1;
        vector<long long> v(n);
        for (auto& x : v) x = (long long)(rr() % 21) - 10;
        SqrtRangeAdd A(v);
        SqrtCountLess C(v);
        vector<long long> ref = v;
        for (int q = 0; q < 100; q++) {
            int l = rr() % (n + 1), r = rr() % (n + 1);
            if (l > r) swap(l, r);
            long long x = (long long)(rr() % 21) - 10;
            int t = rr() % 5;
            if (t == 0) {
                A.add(l, r, x);
                for (int i = l; i < r; i++) ref[i] += x;
                for (int i = 0; i < n; i++) C.set(i, ref[i]);
            } else if (t == 1) {
                int i = rr() % n;
                ref[i] = x, C.set(i, x);
                A.add(i, i + 1, x - A.sum(i, i + 1));  // A にも同じ値を入れる
            } else if (t == 2) {
                long long s = 0;
                for (int i = l; i < r; i++) s += ref[i];
                assert(A.sum(l, r) == s);
            } else if (t == 3 && l < r) {
                assert(A.get_min(l, r) == *min_element(ref.begin() + l, ref.begin() + r));
            } else {
                int c = 0;
                for (int i = l; i < r; i++) c += ref[i] < x;
                assert(C.count_less(l, r, x) == c);
            }
        }
    }
    puts("OK");
}
