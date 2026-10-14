// =====================================================================
// basic/meet_in_the_middle.cpp  半分全列挙
//   N 個から選ぶ 2^N 通りを、前半 2^(N/2) 通りと後半 2^(N/2) 通りに分けて調べ、組み合わせる
//   N <= 40 くらいまで（2^20 ≈ 1e6 が 2 つ）。bit 全探索（N <= 20）では足りないときに使う
//   本体A: 部分和の列挙（ソート済み）          O(2^N)
//   本体B: 和が K 以下の最大値・個数          O(2^(N/2) N)
//   本体C: ナップサック（W が大きく N が小さい）O(2^(N/2) N)
//   本体A は B・C から使う。B と C は互いに独立
// 依存: なし
// ABC / ARC での使用例:
//   ABC184 F - Programming Contest  https://atcoder.jp/contests/abc184/tasks/abc184_f
//     → N ≤ 40 で和が T 以下の最大（max_subset_sum_le）
//   ABC032 D - ナップサック問題  https://atcoder.jp/contests/abc032/tasks/abc032_d
//     → N ≤ 30 のケースを knapsack_mitm で（他のケースは dp/knapsack.cpp）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 部分和の列挙 ----------
// 返り値: a の部分集合（空集合を含む）の和 2^n 個を昇順に並べたもの
vector<long long> subset_sums(const vector<long long>& a) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/meet_in_the_middle.cpp
    vector<long long> s = {0};
    for (long long x : a) {
        vector<long long> t(s.size());
        for (size_t i = 0; i < s.size(); i++) t[i] = s[i] + x;  // ★ 今までの和それぞれに x を足したもの
        vector<long long> m(s.size() * 2);
        merge(s.begin(), s.end(), t.begin(), t.end(), m.begin());  // ★ 2 つのソート済み列をまとめる（ソートし直さない）
        s.swap(m);
    }
    return s;
}

// ---------- 本体B: 和が K 以下（本体A が必要） ----------
// max_subset_sum_le(a, K)   : 和が K 以下になる部分集合の和の最大（空集合の 0 を含む。K < 0 なら -1）
// count_subset_sum_le(a, K) : 和が K 以下になる部分集合の個数（空集合を含む）
long long max_subset_sum_le(const vector<long long>& a, long long K) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/meet_in_the_middle.cpp
    int h = a.size() / 2;
    auto L = subset_sums(vector<long long>(a.begin(), a.begin() + h));
    auto R = subset_sums(vector<long long>(a.begin() + h, a.end()));
    long long best = -1;
    int j = R.size() - 1;
    for (long long x : L) {  // ★ L を小さい順に見ると、組める R の上限は大きい順に下がる（尺取り）
        while (j >= 0 && x + R[j] > K) j--;
        if (j < 0) break;
        best = max(best, x + R[j]);
    }
    return best;
}
long long count_subset_sum_le(const vector<long long>& a, long long K) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/meet_in_the_middle.cpp
    int h = a.size() / 2;
    auto L = subset_sums(vector<long long>(a.begin(), a.begin() + h));
    auto R = subset_sums(vector<long long>(a.begin() + h, a.end()));
    long long cnt = 0;
    int j = R.size();  // R[0..j) が組める
    for (long long x : L) {
        while (j > 0 && x + R[j - 1] > K) j--;
        cnt += j;
    }
    return cnt;
}

// ---------- 本体C: ナップサック（N <= 40、W が大きい） ----------
// 重さ w、価値 v、重さの合計 W 以下での価値の最大
// （W が小さいなら dp/knapsack.cpp の knapsack_01、価値の合計が小さいなら knapsack_01_by_value）
long long knapsack_mitm(const vector<long long>& w, const vector<long long>& v, long long W) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/meet_in_the_middle.cpp
    int n = w.size(), h = n / 2;
    auto enumerate = [&](int lo, int hi) {  // [lo, hi) の品物の選び方すべての {重さ, 価値}
        vector<pair<long long, long long>> res = {{0, 0}};
        for (int i = lo; i < hi; i++) {
            int sz = res.size();
            for (int k = 0; k < sz; k++) res.push_back({res[k].first + w[i], res[k].second + v[i]});
        }
        return res;
    };
    auto L = enumerate(0, h), R = enumerate(h, n);
    sort(R.begin(), R.end());
    for (size_t i = 1; i < R.size(); i++) R[i].second = max(R[i].second, R[i - 1].second);  // ★ 重さ以下で取れる価値の最大にしておく
    long long best = 0;
    for (auto [lw, lv] : L) {
        if (lw > W) continue;
        auto it = upper_bound(R.begin(), R.end(), make_pair(W - lw, LLONG_MAX));  // ★ 残りの重さ W - lw 以下で最大の価値
        if (it != R.begin()) best = max(best, lv + prev(it)->second);
    }
    return best;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    assert((subset_sums({1, 2, 4}) == vector<long long>{0, 1, 2, 3, 4, 5, 6, 7}));
    // 和が 10 以下の最大: {3, 5, 2} → 10
    assert(max_subset_sum_le({3, 5, 2, 7}, 10) == 10 && max_subset_sum_le({3, 5}, 2) == 0 && max_subset_sum_le({3}, -1) == -1);
    // 和が 3 以下: {}, {1}, {2}, {1, 2} の 4 個
    assert(count_subset_sum_le({1, 2, 4}, 3) == 4);
    assert(knapsack_mitm({3, 4, 5}, {30, 50, 60}, 8) == 90);

    // ランダムテスト: bit 全探索と比較
    mt19937_64 rr(120);
    for (int it = 0; it < 3000; it++) {
        int n = rr() % 12;
        vector<long long> a(n), w(n), v(n);
        for (int i = 0; i < n; i++) a[i] = (long long)(rr() % 21) - 5, w[i] = rr() % 1000000000, v[i] = rr() % 100;
        long long K = (long long)(rr() % 41) - 10, W = rr() % 3000000000LL;
        vector<long long> all;
        long long mx = -1, cnt = 0, kn = 0;
        for (int m = 0; m < (1 << n); m++) {
            long long s = 0, sw = 0, sv = 0;
            for (int i = 0; i < n; i++) if (m >> i & 1) s += a[i], sw += w[i], sv += v[i];
            all.push_back(s);
            if (s <= K) mx = max(mx, s), cnt++;
            if (sw <= W) kn = max(kn, sv);
        }
        sort(all.begin(), all.end());
        assert(subset_sums(a) == all && max_subset_sum_le(a, K) == mx && count_subset_sum_le(a, K) == cnt && knapsack_mitm(w, v, W) == kn);
    }
    puts("OK");
}
