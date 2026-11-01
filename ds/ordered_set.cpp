// =====================================================================
// ds/ordered_set.cpp  順序付き set / multiset（k 番目の値・x より小さい値の個数が O(log N)）
//   std::set にできないこと「k 番目に小さい値」「x より小さい値はいくつあるか」を、値の追加・削除をしながら O(log N) で答える
//   中身は GCC の拡張（pb_ds の tree）。AtCoder（GCC）で使える。座標圧縮 + BIT と違って、値を先に全部知らなくてよい
//   ※ Clang（AtCoder の C++ (Clang)）では使えない。速さは std::set の 2〜3 倍遅いくらい（N = 2×10^5 なら問題ない）
// 依存: なし（#include <ext/pb_ds/...> をコピー範囲の中に入れてある）
// ABC / ARC での使用例:
//   ABC234 D - Prefix K-th Max  https://atcoder.jp/contests/abc234/tasks/abc234_d
//     → P_i を順に insert し、i >= K なら kth_largest(K − 1)
//   ABC241 D - Sequence Query  https://atcoder.jp/contests/abc241/tasks/abc241_d
//     → OrderedMultiset。「x 以下で大きい方から k 番目」は c = count_leq(x) として c >= k なら kth(c − k)、
//       「x 以上で小さい方から k 番目」は c = count_less(x) として c + k <= size() なら kth(c + k − 1)
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
#include <ext/pb_ds/assoc_container.hpp>  // ★ この 2 行も一緒に貼る（bits/stdc++.h には入っていない）
#include <ext/pb_ds/tree_policy.hpp>
// OrderedSet<long long> s;          重複なし（std::set と同じ）
// OrderedMultiset<long long> s;     重複あり（std::multiset と同じ）
//   s.insert(x), s.erase(x)         追加・1 個削除。O(log N)
//   s.kth(k)                        小さい方から k 番目（0 始まり）。s.kth_largest(k) は大きい方から k 番目
//   s.count_less(x)                 x より小さい値の個数（= x を入れたら何番目になるか）。count_leq(x) は x 以下
//   s.count_range(l, r)             l <= 値 < r の個数
//   s.min_ge(x, def), s.max_le(x, def)   x 以上で最小・x 以下で最大（なければ def）
// 「x より大きい最小」は整数なら min_ge(x + 1, def)、「x より小さい最大」は max_le(x − 1, def)
// ◆ struct OrderedSet<T, MULTI> : 順序付き set（MULTI = true で multiset）。各操作 O(log N)
//   insert(x)          : x を入れる。入れたら true（重複なしで x がもうあれば何もせず false）
//   erase(x)           : x を 1 個消す。消したら true（なければ false）
//   contains(x)        : x が入っているか
//   count(x)           : x の個数
//   size(), empty()    : 個数、空か
//   kth(k)             : 小さい方から k 番目の値（k = 0 が最小）。0 <= k < size() であること
//   kth_largest(k)     : 大きい方から k 番目の値（k = 0 が最大）
//   count_less(x)      : x より小さい値の個数
//   count_leq(x)       : x 以下の値の個数
//   count_range(l, r)  : l 以上 r 未満の値の個数
//   min_ge(x, def)     : x 以上の最小の値（なければ def）
//   max_le(x, def)     : x 以下の最大の値（なければ def）
//   to_vector()        : 中身を小さい順に並べた vector（デバッグ用）
//   T : 値の型（比較できる型。pair なども OK）
template <class T, bool MULTI = false>
struct OrderedSet {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/ordered_set.cpp
    // ★ 同じ値を区別するため {値, 通し番号} の組で入れる（重複なしでも同じ作り）
    using P = pair<T, int>;
    __gnu_pbds::tree<P, __gnu_pbds::null_type, less<P>, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update> s;
    int cnt = 0;  // 次に使う通し番号
    // insert(x) : x : 入れる値
    bool insert(const T& x) {
        if (!MULTI && contains(x)) return false;
        s.insert({x, cnt++});
        return true;
    }
    // erase(x) : x : 消す値（1 個だけ消す）
    bool erase(const T& x) {
        auto it = s.lower_bound({x, INT_MIN});  // ★ 値が x の組のうち最初のもの
        if (it == s.end() || it->first != x) return false;
        s.erase(it);
        return true;
    }
    bool contains(const T& x) const { return count(x) > 0; }
    long long count(const T& x) const { return count_leq(x) - count_less(x); }
    long long size() const { return s.size(); }
    bool empty() const { return s.empty(); }
    // kth(k) : k : 0 始まりの順位（0 <= k < size()）
    T kth(long long k) const {
        assert(0 <= k && k < size());
        return s.find_by_order(k)->first;  // ★ k 番目の要素（pb_ds の機能）
    }
    // kth_largest(k) : k : 大きい方からの 0 始まりの順位
    T kth_largest(long long k) const { return kth(size() - 1 - k); }
    // count_less(x) : x : 基準の値
    long long count_less(const T& x) const { return s.order_of_key({x, INT_MIN}); }  // ★ {x, 最小の番号} より小さい組の個数 = x より小さい値の個数
    // count_leq(x) : x : 基準の値
    long long count_leq(const T& x) const { return s.order_of_key({x, INT_MAX}); }
    // count_range(l, r) : l 以上 r 未満
    long long count_range(const T& l, const T& r) const { return l < r ? count_less(r) - count_less(l) : 0; }
    // min_ge(x, def) : x : 基準の値、def : なかったときに返す値
    T min_ge(const T& x, const T& def) const {
        long long c = count_less(x);
        return c < size() ? kth(c) : def;
    }
    // max_le(x, def) : x : 基準の値、def : なかったときに返す値
    T max_le(const T& x, const T& def) const {
        long long c = count_leq(x);
        return c > 0 ? kth(c - 1) : def;
    }
    // to_vector() : 小さい順の中身
    vector<T> to_vector() const {
        vector<T> res;
        for (auto& p : s) res.push_back(p.first);
        return res;
    }
};
// ◆ OrderedMultiset<T> : 重複ありの順序付き set（OrderedSet<T, true> と同じ）
template <class T>
using OrderedMultiset = OrderedSet<T, true>;

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    {  // 重複なし
        OrderedSet<long long> s;
        for (long long x : {5, 1, 9, 3, 5}) s.insert(x);  // 5 は 1 回だけ入る
        assert(s.size() == 4 && (s.to_vector() == vector<long long>{1, 3, 5, 9}));
        assert(s.kth(0) == 1 && s.kth(2) == 5 && s.kth_largest(0) == 9);
        assert(s.count_less(5) == 2 && s.count_leq(5) == 3 && s.count_range(2, 9) == 2);
        assert(s.min_ge(4, -1) == 5 && s.min_ge(10, -1) == -1 && s.max_le(4, -1) == 3 && s.max_le(0, -1) == -1);
        assert(!s.insert(3) && s.erase(3) && !s.erase(3) && !s.contains(3) && s.size() == 3);
    }
    {  // 重複あり
        OrderedMultiset<int> m;
        for (int x : {2, 2, 7, 2, 4}) m.insert(x);
        assert(m.size() == 5 && m.count(2) == 3 && m.kth(2) == 2 && m.kth(3) == 4);
        assert(m.erase(2) && m.count(2) == 2 && m.size() == 4);
        assert(m.count_less(4) == 2 && m.count_leq(4) == 3 && m.max_le(3, -1) == 2);
    }
    {  // ABC234 D の例: P = 1 2 3, K = 2 → 1, 2
        OrderedSet<int> s;
        vector<int> P = {1, 2, 3}, out;
        for (int i = 0; i < 3; i++) {
            s.insert(P[i]);
            if (i + 1 >= 2) out.push_back(s.kth_largest(1));
        }
        assert((out == vector<int>{1, 2}));
    }
    {  // pair も入れられる
        OrderedSet<pair<int, int>> s;
        s.insert({1, 5}), s.insert({1, 2}), s.insert({0, 9});
        assert((s.kth(1) == pair<int, int>{1, 2}) && s.count_less({1, 3}) == 2);
    }
    // ランダムテスト: 並べた vector（全部見る）と比較
    mt19937 rr(301);
    for (int multi = 0; multi < 2; multi++) {
        for (int it = 0; it < 300; it++) {
            OrderedSet<long long> s;
            OrderedMultiset<long long> m;
            vector<long long> v;  // 小さい順
            for (int q = 0; q < 200; q++) {
                long long x = (long long)(rr() % 21) - 10, y = (long long)(rr() % 21) - 10;
                int t = rr() % 4;
                if (t == 0 || t == 1) {
                    bool want = multi || !binary_search(v.begin(), v.end(), x);
                    bool got = multi ? m.insert(x) : s.insert(x);
                    assert(got == want);
                    if (want) v.insert(upper_bound(v.begin(), v.end(), x), x);
                } else if (t == 2) {
                    auto p = lower_bound(v.begin(), v.end(), x);
                    bool want = p != v.end() && *p == x;
                    bool got = multi ? m.erase(x) : s.erase(x);
                    assert(got == want);
                    if (want) v.erase(p);
                }
                auto chk = [&](auto& o) {
                    long long n = v.size();
                    assert(o.size() == n && o.empty() == (n == 0) && o.to_vector() == v);
                    long long lt = lower_bound(v.begin(), v.end(), x) - v.begin(), le = upper_bound(v.begin(), v.end(), x) - v.begin();
                    assert(o.count_less(x) == lt && o.count_leq(x) == le && o.count(x) == le - lt && o.contains(x) == (le > lt));
                    long long rl = lower_bound(v.begin(), v.end(), y) - v.begin();
                    assert(o.count_range(x, y) == max(0LL, rl - lt));
                    assert(o.min_ge(x, 99) == (lt < n ? v[lt] : 99));
                    assert(o.max_le(x, 99) == (le > 0 ? v[le - 1] : 99));
                    if (n) {
                        long long k = rr() % n;
                        assert(o.kth(k) == v[k] && o.kth_largest(k) == v[n - 1 - k]);
                    }
                };
                if (multi) chk(m);
                else chk(s);
            }
        }
    }
    puts("OK");
}
