// =====================================================================
// ds/wavelet_matrix.cpp  Wavelet Matrix（区間の k 番目に小さい値・区間で x 未満の個数 など）
//   数列を前計算 O(N log σ) しておくと、区間 [l, r) について次が O(log σ) で答えられる（σ = 値の種類数）
//     k 番目に小さい値、値が [lo, hi) に入る個数、x 未満で最大の値、x 以上で最小の値
//   ★ 値を 2 進数で見て、上の桁から「0 のものを前に、1 のものを後ろに」安定に並べ替えた列を桁ごとに持つ。
//      各桁で「区間に 0 が何個あるか」がわかれば、k 番目がどちら側にあるか決められる（二分探索を値の桁で行う）
//   更新はできない（値の変更があるならセグ木や BIT を使う）
// 依存: なし
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// WaveletMatrix<long long> wm(a);   a は何でもよい（中で座標圧縮する）
//   wm.access(i)                a[i]
//   wm.kth_smallest(l, r, k)    [l, r) の中で k 番目（0 始まり）に小さい値。k < r − l
//   wm.kth_largest(l, r, k)     k 番目（0 始まり）に大きい値
//   wm.range_freq(l, r, lo, hi) [l, r) の中で lo <= 値 < hi の個数
//   wm.prev_value(l, r, x)      [l, r) の中で x 未満の最大の値（なければ nullopt）
//   wm.next_value(l, r, x)      [l, r) の中で x 以上の最小の値（なければ nullopt）
// ◆ struct WaveletMatrix : 区間の k 番目に小さい値・範囲に入る個数など（更新なし）。前計算 O(N log σ)、各クエリ O(log σ)
//   WaveletMatrix(a)              : a は何でもよい（中で座標圧縮する）
//   access(i)                     : a[i]
//   kth_smallest(l, r, k)         : [l, r) で k 番目（0 始まり）に小さい値（0 <= k < r − l）
//   kth_largest(l, r, k)          : k 番目に大きい値
//   range_freq(l, r, lo, hi)      : [l, r) で lo <= 値 < hi の個数
//   count_less(l, r, x)           : [l, r) で x 未満の個数
//   prev_value(l, r, x)           : x 未満で最大の値（なければ nullopt）
//   next_value(l, r, x)           : x 以上で最小の値（なければ nullopt）
template <class T>
struct WaveletMatrix {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/wavelet_matrix.cpp
    // （内部用）rank が O(1) のビット列
    struct BitVec {  // rank（[0, i) の 1 の個数）を O(1) で求められるビット列
        vector<unsigned long long> b;
        vector<int> cum;
        // （内部用）n ビットのビット列
        BitVec(int n = 0) : b(n / 64 + 1, 0), cum(n / 64 + 2, 0) {}
        // （内部用）i 番目のビットを立てる
        void set(int i) { b[i >> 6] |= 1ULL << (i & 63); }
        // （内部用）rank のための累積を作る
        void build() { for (size_t i = 0; i < b.size(); i++) cum[i + 1] = cum[i] + __builtin_popcountll(b[i]); }
        // （内部用）i 番目のビット
        bool get(int i) const { return b[i >> 6] >> (i & 63) & 1; }
        // （内部用）[0, i) の 1 の個数
        int rank1(int i) const { return cum[i >> 6] + __builtin_popcountll(b[i >> 6] & ((1ULL << (i & 63)) - 1)); }
        // （内部用）[0, i) の 0 の個数
        int rank0(int i) const { return i - rank1(i); }
    };
    int n, LOG;
    vector<T> vals;  // 座標圧縮した値の一覧
    vector<BitVec> bv;
    vector<int> zeros;  // zeros[d] = 桁 d で 0 だったものの個数
    // a : 元の数列
    WaveletMatrix(const vector<T>& a) : n(a.size()) {
        vals = a;
        sort(vals.begin(), vals.end());
        vals.erase(unique(vals.begin(), vals.end()), vals.end());
        LOG = 1;
        while ((1 << LOG) < (int)vals.size()) LOG++;
        vector<int> cur(n);
        for (int i = 0; i < n; i++) cur[i] = lower_bound(vals.begin(), vals.end(), a[i]) - vals.begin();
        bv.assign(LOG, BitVec(n)), zeros.assign(LOG, 0);
        for (int d = LOG - 1; d >= 0; d--) {  // ★ 上の桁から
            vector<int> zs, os;
            for (int i = 0; i < n; i++) {
                if (cur[i] >> d & 1) bv[d].set(i), os.push_back(cur[i]);
                else zs.push_back(cur[i]);
            }
            bv[d].build();
            zeros[d] = zs.size();
            cur = zs;
            cur.insert(cur.end(), os.begin(), os.end());  // ★ 0 を前、1 を後ろに（順番は保つ）
        }
    }
    // access(i) : i : 0..N−1
    T access(int i) const {
        int res = 0;
        for (int d = LOG - 1; d >= 0; d--) {
            if (bv[d].get(i)) res |= 1 << d, i = zeros[d] + bv[d].rank1(i);
            else i = bv[d].rank0(i);
        }
        return vals[res];
    }
    // kth_smallest(l, r, k) : 区間 [l, r)、k : 0..r−l−1（範囲外は assert）
    T kth_smallest(int l, int r, int k) const {
        assert(0 <= k && k < r - l);
        int res = 0;
        for (int d = LOG - 1; d >= 0; d--) {
            int c0 = bv[d].rank0(r) - bv[d].rank0(l);  // この桁が 0 のものの個数
            if (k < c0) l = bv[d].rank0(l), r = bv[d].rank0(r);  // ★ k 番目は 0 の側にある
            else k -= c0, res |= 1 << d, l = zeros[d] + bv[d].rank1(l), r = zeros[d] + bv[d].rank1(r);
        }
        return vals[res];
    }
    // kth_largest(l, r, k) : k : 0..r−l−1
    T kth_largest(int l, int r, int k) const { return kth_smallest(l, r, r - l - 1 - k); }
    // （内部用）圧縮後の値が x 未満の個数
    int count_less_id(int l, int r, int x) const {  // 圧縮後の値が x 未満の個数
        if (x >= (1 << LOG)) return r - l;
        if (x <= 0) return 0;
        int res = 0;
        for (int d = LOG - 1; d >= 0; d--) {
            if (x >> d & 1) {
                res += bv[d].rank0(r) - bv[d].rank0(l);  // ★ x の桁が 1 なら、0 の側は全部 x 未満
                l = zeros[d] + bv[d].rank1(l), r = zeros[d] + bv[d].rank1(r);
            } else {
                l = bv[d].rank0(l), r = bv[d].rank0(r);
            }
        }
        return res;
    }
    // count_less(l, r, x) : x : 境目の値（入っていない値でもよい）
    long long count_less(int l, int r, T x) const { return count_less_id(l, r, lower_bound(vals.begin(), vals.end(), x) - vals.begin()); }
    // range_freq(l, r, lo, hi) : lo >= hi なら 0
    long long range_freq(int l, int r, T lo, T hi) const { return lo >= hi ? 0 : count_less(l, r, hi) - count_less(l, r, lo); }
    // prev_value(l, r, x) : x 未満で最大の値
    optional<T> prev_value(int l, int r, T x) const {
        int c = count_less(l, r, x);
        if (c == 0) return nullopt;
        return kth_smallest(l, r, c - 1);
    }
    // next_value(l, r, x) : x 以上で最小の値
    optional<T> next_value(int l, int r, T x) const {
        int c = count_less(l, r, x);
        if (c == r - l) return nullopt;
        return kth_smallest(l, r, c);
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // a = 5 3 8 3 1 9。[1, 5) = 3 8 3 1 → 小さい順 1 3 3 8
    {
        WaveletMatrix<long long> wm({5, 3, 8, 3, 1, 9});
        assert(wm.access(2) == 8 && wm.access(4) == 1);
        assert(wm.kth_smallest(1, 5, 0) == 1 && wm.kth_smallest(1, 5, 2) == 3 && wm.kth_largest(1, 5, 0) == 8);
        assert(wm.range_freq(0, 6, 3, 6) == 3 && wm.range_freq(1, 5, 4, 100) == 1);  // [3, 6) は 5, 3, 3
        assert(*wm.prev_value(0, 6, 5) == 3 && *wm.next_value(0, 6, 6) == 8 && !wm.prev_value(1, 5, 1).has_value() && !wm.next_value(0, 3, 9).has_value());
        WaveletMatrix<long long> one({-7});  // 1 個・負の値
        assert(one.kth_smallest(0, 1, 0) == -7 && one.range_freq(0, 1, -7, -6) == 1);
    }
    // ランダムテスト: 区間を取り出してソートしたものと比較
    {
        mt19937 rr(195);
        for (int it = 0; it < 500; it++) {
            int n = rr() % 40 + 1, V = (it % 2) ? 5 : 1000000000;
            vector<long long> a(n);
            for (auto& x : a) x = (long long)(rr() % (2 * V + 1)) - V;
            WaveletMatrix<long long> wm(a);
            for (int i = 0; i < n; i++) assert(wm.access(i) == a[i]);
            for (int q = 0; q < 100; q++) {
                int l = rr() % n, r = rr() % n;
                if (l > r) swap(l, r);
                r++;
                vector<long long> b(a.begin() + l, a.begin() + r);
                sort(b.begin(), b.end());
                int k = rr() % (r - l);
                assert(wm.kth_smallest(l, r, k) == b[k] && wm.kth_largest(l, r, k) == b[r - l - 1 - k]);
                long long lo = (long long)(rr() % (2 * V + 3)) - V - 1, hi = (long long)(rr() % (2 * V + 3)) - V - 1;
                if (rr() % 3 == 0) lo = a[rr() % n], hi = a[rr() % n];  // 値ちょうどの境界も
                int f = 0;
                for (long long x : b) f += lo <= x && x < hi;
                assert(wm.range_freq(l, r, lo, hi) == f);
                auto pv = wm.prev_value(l, r, lo), nv = wm.next_value(l, r, lo);
                auto it1 = lower_bound(b.begin(), b.end(), lo);
                assert(pv.has_value() == (it1 != b.begin()) && (!pv || *pv == *prev(it1)));
                assert(nv.has_value() == (it1 != b.end()) && (!nv || *nv == *it1));
            }
        }
    }
    puts("OK");
}
