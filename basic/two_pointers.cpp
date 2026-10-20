// =====================================================================
// basic/two_pointers.cpp  尺取り法
//   各 l について「区間 [l, r) が条件を満たす最大の r」を、全体 O(N) で求める
//   条件は「区間を縮めても条件を満たし続ける」（単調）であること
//   例: 和が K 以下、種類数が K 以下、最大値 - 最小値が K 以下 など
// 依存: なし
// ABC / ARC での使用例:
//   ABC032 C - 列  https://atcoder.jp/contests/abc032/tasks/abc032_c
//     → 積が K 以下の最長区間（要素に 0 があるときは別扱い）
//   ABC098 D - Xor Sum 2  https://atcoder.jp/contests/abc098/tasks/arc098_b
//     → xor と和が等しい区間の個数 = Σ(R[l] − l)
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// n : 配列の長さ
// 【自分で定義するもの】（main の中にラムダで書く。区間の状態（和、個数など）はラムダの外の変数に持つ）
//   can_push(r) -> bool : 今の区間 [l, r) に a[r] を加えても条件を満たすか
//   push(r)             : 区間に a[r] を加える（状態を更新）
//   pop(l)              : 区間から a[l] を取り除く（状態を更新）
// 返り値: R[l] = [l, R[l]) が条件を満たす最大の右端（条件を満たす区間が空しかなければ R[l] = l）
//
// よく使う形:
//   条件を満たす区間の個数         = Σ (R[l] - l)
//   条件を満たす最長の区間の長さ   = max (R[l] - l)
//   「和が K 以上」になる最短区間  → 条件を「和が K 未満」にして、R[l] < n なら [l, R[l] + 1) が最短
// ◆ two_pointers(n, can_push, push, pop) : 尺取り法。各左端 l について、条件を満たす最大の右端を求める。O(N)
//   n        : 配列の長さ
//   can_push : bool can_push(int r)。今の区間に a[r] を加えても条件を満たすか（区間の状態はラムダの外の変数に持つ）
//   push     : void push(int r)。区間に a[r] を加えたとして状態を更新
//   pop      : void pop(int l)。区間から a[l] を取り除いたとして状態を更新
//   注意 : 「区間を縮めても条件は満たされたまま」（単調性）が成り立つこと。そうでないと尺取り法は使えない
//   返り値 : R[l] = [l, R[l]) が条件を満たす最大の右端
template <class CanPush, class Push, class Pop>
vector<long long> two_pointers(int n, CanPush can_push, Push push, Pop pop) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/two_pointers.cpp
    vector<long long> R(n);
    int r = 0;
    for (int l = 0; l < n; l++) {
        while (r < n && can_push(r)) push(r), r++;  // ★ 右端を伸ばせるだけ伸ばす
        R[l] = r;
        if (r == l) r++;  // 区間が空（a[l] 1 個でも条件を満たさない）→ 右端も一緒に進める
        else pop(l);      // ★ 左端を 1 つ縮める（右端は戻さない → 全体で O(N)）
    }
    return R;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 例1: 和が K 以下の区間の個数（a は正の値）
    {
        vector<long long> a = {1, 2, 3, 4, 5};
        long long K = 6, sum = 0;
        auto R = two_pointers(a.size(),
                              [&](int r) { return sum + a[r] <= K; },
                              [&](int r) { sum += a[r]; },
                              [&](int l) { sum -= a[l]; });
        long long cnt = 0;
        for (int l = 0; l < (int)a.size(); l++) cnt += R[l] - l;
        assert(cnt == 8);  // [1] [2] [3] [4] [5] [1,2] [2,3] [1,2,3]
    }
    // 例2: 種類数が K 以下の最長区間
    {
        vector<int> a = {1, 2, 1, 3, 3, 3, 2, 2};
        int K = 2, kinds = 0;
        map<int, int> cnt;
        auto R = two_pointers(a.size(),
                              [&](int r) { return kinds + (cnt[a[r]] == 0) <= K; },
                              [&](int r) { if (cnt[a[r]]++ == 0) kinds++; },
                              [&](int l) { if (--cnt[a[l]] == 0) kinds--; });
        long long best = 0;
        for (int l = 0; l < (int)a.size(); l++) best = max(best, R[l] - l);
        assert(best == 5);  // [3, 3, 3, 2, 2]
    }
    // 例3: 1 個でも条件を満たさない要素があるとき（K より大きい要素）
    {
        vector<long long> a = {1, 10, 2};
        long long K = 5, sum = 0;
        auto R = two_pointers(a.size(),
                              [&](int r) { return sum + a[r] <= K; },
                              [&](int r) { sum += a[r]; },
                              [&](int l) { sum -= a[l]; });
        assert((R == vector<long long>{1, 1, 3}));
    }
    // ランダムテスト: 和が K 以下の最大右端を愚直解と比較
    mt19937 rng(1);
    for (int it = 0; it < 2000; it++) {
        int n = rng() % 10 + 1;
        vector<long long> a(n);
        for (auto& x : a) x = rng() % 6;
        long long K = rng() % 12, sum = 0;
        auto R = two_pointers(n, [&](int r) { return sum + a[r] <= K; }, [&](int r) { sum += a[r]; },
                              [&](int l) { sum -= a[l]; });
        for (int l = 0; l < n; l++) {
            int r = l;
            long long s = 0;
            while (r < n && s + a[r] <= K) s += a[r++];
            assert(R[l] == r);
        }
    }
    puts("OK");
}
