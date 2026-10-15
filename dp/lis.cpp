// =====================================================================
// dp/lis.cpp  LIS（最長増加部分列）
//   列から順番を保って取り出した、増加している部分列のうち最長のもの。O(N log N)
//   strict = true  : 狭義単調増加（a < b < c）
//   strict = false : 広義単調増加（a <= b <= c）
//   減少列が欲しいときは、全要素を -1 倍するか列を反転して使う
// 依存: なし
// ABC / ARC での使用例:
//   ABC038 D - プレゼント  https://atcoder.jp/contests/abc038/tasks/abc038_d
//     → 幅で昇順・同じ幅は高さで降順にソートし、高さの LIS
//   ABC354 F - Useless for LIS  https://atcoder.jp/contests/abc354/tasks/abc354_f
//     → 前からの len と後ろからの len の和 − 1 が LIS の長さになる位置（lis_lengths）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体: 各位置で終わる LIS の長さ ----------
// 返り値: len[i] = a[i] で終わる LIS の長さ。全体の LIS の長さは *max_element(len)
// 典型: 「a[i] を必ず含む LIS の長さ」= (a[i] で終わる長さ) + (a[i] から始まる長さ) - 1
//       a[i] から始まる長さは、列を反転し -1 倍したものの len を逆順に読めば求まる
// ◆ lis_lengths(a, strict) : 各位置で終わる最長増加部分列の長さ。O(N log N)
//   a      : 数列（< で比べられる型）
//   strict : true（既定）なら狭義単調増加（a < b）、false なら広義（a <= b）
//   返り値 : len[i] = a[i] で終わる LIS の長さ
template <class T>
vector<int> lis_lengths(const vector<T>& a, bool strict = true) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/lis.cpp
    vector<T> tail;  // tail[k] = 長さ k+1 の増加列の末尾として最小の値
    vector<int> len(a.size());
    for (int i = 0; i < (int)a.size(); i++) {
        auto it = strict ? lower_bound(tail.begin(), tail.end(), a[i])   // ★ a[i] 以上の最初の位置（狭義）
                         : upper_bound(tail.begin(), tail.end(), a[i]);  // ★ a[i] より大きい最初の位置（広義）
        len[i] = it - tail.begin() + 1;  // ★ その位置の長さの列を a[i] で終わらせられる
        if (it == tail.end()) tail.push_back(a[i]);
        else *it = a[i];  // ★ 同じ長さなら末尾は小さい方が後で伸ばしやすい
    }
    return len;
}

// ---------- 削除可: LIS の復元（本体が必要） ----------
// 返り値: LIS をなす添字の列（昇順）。値は a[idx[k]]。長さは .size()
// ◆ lis(a, strict) : 最長増加部分列を 1 つ返す（添字の昇順の列）
//   a, strict : lis_lengths と同じ
//   返り値 : LIS をなす添字の列。値は a[idx[k]]
template <class T>
vector<int> lis(const vector<T>& a, bool strict = true) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/lis.cpp
    auto len = lis_lengths(a, strict);
    int need = len.empty() ? 0 : *max_element(len.begin(), len.end());
    vector<int> idx;
    for (int i = (int)a.size() - 1; i >= 0 && need > 0; i--) {  // ★ 後ろから「長さ need で終わり、次の要素につながる」ものを拾う
        if (len[i] != need) continue;
        if (!idx.empty() && !(strict ? a[i] < a[idx.back()] : a[i] <= a[idx.back()])) continue;
        idx.push_back(i), need--;
    }
    reverse(idx.begin(), idx.end());
    return idx;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    vector<int> a = {3, 1, 4, 1, 5, 9, 2, 6};
    auto len = lis_lengths(a);
    assert(*max_element(len.begin(), len.end()) == 4);  // 1 4 5 9 / 3 4 5 6 など
    assert((len == vector<int>{1, 1, 2, 1, 3, 4, 2, 4}));
    auto idx = lis(a);
    assert(idx.size() == 4);
    for (int k = 1; k < 4; k++) assert(idx[k - 1] < idx[k] && a[idx[k - 1]] < a[idx[k]]);

    vector<int> b = {2, 2, 2, 1, 3};
    auto ls = lis_lengths(b, true);
    assert(*max_element(ls.begin(), ls.end()) == 2);  // 2 3
    auto lb = lis_lengths(b, false);
    assert(*max_element(lb.begin(), lb.end()) == 4);  // 2 2 2 3

    // ランダムテスト: bit 全探索と比較
    mt19937 rng(5);
    for (int it = 0; it < 3000; it++) {
        int n = rng() % 10;
        vector<int> c(n);
        for (auto& x : c) x = rng() % 5;
        for (bool strict : {true, false}) {
            int best = 0;
            for (int m = 0; m < (1 << n); m++) {
                int prev = INT_MIN, cnt = 0;
                bool ok = true;
                for (int i = 0; i < n; i++)
                    if (m >> i & 1) {
                        if (cnt > 0 && (strict ? c[i] <= prev : c[i] < prev)) ok = false;
                        prev = c[i], cnt++;
                    }
                if (ok) best = max(best, cnt);
            }
            auto r = lis(c, strict);
            assert((int)r.size() == best);
            for (int k = 1; k < (int)r.size(); k++) assert(r[k - 1] < r[k] && (strict ? c[r[k - 1]] < c[r[k]] : c[r[k - 1]] <= c[r[k]]));
        }
    }
    puts("OK");
}
