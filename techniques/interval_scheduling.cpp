// =====================================================================
// techniques/interval_scheduling.cpp  区間スケジューリング（終わりでソートする貪欲法）
//   本体A: 重ならないように選べる区間の最大数（半開区間 [l, r)。端どうしが触れるのはよい）
//   本体B: すべての区間に少なくとも 1 本刺さるように点を置くときの最小本数（閉区間 [l, r]）
//   どちらも「終わりが早い順に見て、取れるなら取る」で最適になる。O(N log N)
//   ※ A の答えと B の答えは、同じ区間（閉区間どうし）なら等しくなる
//   A と B は互いに独立
// 依存: なし
// ABC / ARC での使用例:
//   ABC103 D - Islands War  https://atcoder.jp/contests/abc103/tasks/abc103_d
//     → a と b の間の橋 = 閉区間 [a, b−1] に 1 本刺す。min_stabbing_points の個数
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 重ならない区間の最大数 ----------
// iv[i] = {l, r}（半開区間 [l, r)）。返り値: 選んだ区間の番号（個数が答え）
// 閉区間 [l, r] で「端が同じ点で触れるのもダメ」なら {l, r + 1} として渡す
template <class T>
vector<int> max_disjoint_intervals(const vector<pair<T, T>>& iv) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/interval_scheduling.cpp
    vector<int> ord(iv.size());
    iota(ord.begin(), ord.end(), 0);
    sort(ord.begin(), ord.end(), [&](int a, int b) { return iv[a].second < iv[b].second; });  // ★ 終わりが早い順
    vector<int> chosen;
    bool any = false;
    T last = T();
    for (int i : ord)
        if (!any || iv[i].first >= last) chosen.push_back(i), last = iv[i].second, any = true;  // ★ 前に選んだ区間と重ならなければ取る
    return chosen;
}

// ---------- 本体B: すべての区間を刺す最小の点の数 ----------
// iv[i] = {l, r}（閉区間 [l, r]）。返り値: 置く点の座標の一覧（個数が答え）
// 例: 「a 番目と b 番目の島の間のどこかの橋を壊す」→ 橋 a..b-1 のどれか → 閉区間 [a, b - 1]
template <class T>
vector<T> min_stabbing_points(const vector<pair<T, T>>& iv) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/interval_scheduling.cpp
    vector<int> ord(iv.size());
    iota(ord.begin(), ord.end(), 0);
    sort(ord.begin(), ord.end(), [&](int a, int b) { return iv[a].second < iv[b].second; });
    vector<T> pts;
    for (int i : ord)
        if (pts.empty() || iv[i].first > pts.back()) pts.push_back(iv[i].second);  // ★ まだ刺さっていなければ、一番右端に刺す（後の区間にも刺さりやすい）
    return pts;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // [1,4) [3,5) [0,6) [5,7) [3,9) [5,9) [6,10) [8,11) [8,12) [2,14) [12,16): 選べるのは 4 個
    //   終わり順に [1,4) → [5,7)（4 以上で最初に終わる）→ [8,11) → [12,16)
    vector<pair<int, int>> iv = {{1, 4}, {3, 5}, {0, 6}, {5, 7}, {3, 9}, {5, 9}, {6, 10}, {8, 11}, {8, 12}, {2, 14}, {12, 16}};
    auto ch = max_disjoint_intervals(iv);
    assert(ch.size() == 4 && iv[ch[0]] == make_pair(1, 4) && iv[ch[1]] == make_pair(5, 7) && iv[ch[2]] == make_pair(8, 11) && iv[ch[3]] == make_pair(12, 16));

    // 閉区間 [1,3] [2,5] [4,6] [7,8]: 点 3 で前 2 つ、点 6 で 3 つ目、点 8 で 4 つ目 → 3 本
    auto pts = min_stabbing_points<int>({{1, 3}, {2, 5}, {4, 6}, {7, 8}});
    assert((pts == vector<int>{3, 6, 8}));

    // ランダムテスト: 部分集合・点の置き方を全部試したものと比較
    mt19937 rr(135);
    for (int it = 0; it < 3000; it++) {
        int n = rr() % 9;
        vector<pair<int, int>> v(n);
        for (auto& [l, r] : v) { l = rr() % 10, r = l + rr() % 5; }  // 閉区間用（l <= r）
        // A: 半開区間 [l, r+1) として、重ならない最大数
        vector<pair<int, int>> h(n);
        for (int i = 0; i < n; i++) h[i] = {v[i].first, v[i].second + 1};
        int bestA = 0;
        for (int m = 0; m < (1 << n); m++) {
            bool ok = true;
            for (int i = 0; i < n && ok; i++)
                for (int j = i + 1; j < n && ok; j++)
                    if ((m >> i & 1) && (m >> j & 1) && max(h[i].first, h[j].first) < min(h[i].second, h[j].second)) ok = false;
            if (ok) bestA = max(bestA, __builtin_popcount(m));
        }
        auto ca = max_disjoint_intervals(h);
        assert((int)ca.size() == bestA);
        for (size_t i = 0; i < ca.size(); i++)
            for (size_t j = i + 1; j < ca.size(); j++)
                assert(max(h[ca[i]].first, h[ca[j]].first) >= min(h[ca[i]].second, h[ca[j]].second));
        // B: 点の位置 0..14 の部分集合を全部試す（小さい集合から）
        auto pb = min_stabbing_points(v);
        for (auto [l, r] : v) {
            bool hit = false;
            for (int p : pb) hit |= l <= p && p <= r;
            assert(hit);
        }
        int bestB = n == 0 ? 0 : 100;
        for (int m = 0; m < (1 << 15) && n > 0; m++) {
            int pc = __builtin_popcount(m);
            if (pc >= bestB) continue;
            bool ok = true;
            for (auto [l, r] : v) {
                bool hit = false;
                for (int p = l; p <= r; p++) hit |= m >> p & 1;
                ok &= hit;
            }
            if (ok) bestB = pc;
        }
        assert((int)pb.size() == bestB && bestB == bestA);
    }
    puts("OK");
}
