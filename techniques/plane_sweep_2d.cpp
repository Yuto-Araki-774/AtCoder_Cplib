// =====================================================================
// techniques/plane_sweep_2d.cpp  2 次元の平面走査（座標圧縮 + BIT）
//   動かない点が N 個（重み付きでもよい）、長方形の質問が Q 個あるとき、
//   各長方形に入る点の重みの合計を O((N + Q) log N) で求める。座標は大きくてよい（1e18 でも）
//   ★ やり方: x の小さい順に点を BIT（y 方向）へ入れていき、長方形 [x1, x2) × [y1, y2) を
//     「x < x2 までの和」-「x < x1 までの和」に分けて、それぞれ x がそこまで進んだときに答える
//   グリッドが小さく更新もないなら basic/prefix_sum.cpp の PrefixSum2D、
//   点の追加と質問が混ざるなら ds/bit2d.cpp（座標が小さいとき）
// 依存: ACL の fenwick_tree（AtCoder ではテンプレートの atcoder/all に含まれる）
// ABC / ARC での使用例:
//   ABC106 D - AtCoder Express 2  https://atcoder.jp/contests/abc106/tasks/abc106_d
//     → 列車 (L, R) を点、「p ≤ L かつ R ≤ q」を長方形 [p, N+1) × [1, q+1) に（制約が小さいので 2 次元累積和でも解ける）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/fenwicktree>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// T : 座標の型、W : 重みの型
// pts[i]   = {x, y, w}           点（重み w。個数を数えるなら w = 1）
// rects[j] = {x1, y1, x2, y2}    長方形 [x1, x2) × [y1, y2)（半開区間）
// 返り値[j] = 長方形 j に入る点の重みの合計
// 閉区間 [x1, x2] × [y1, y2]（整数）なら {x1, y1, x2 + 1, y2 + 1} として渡す
// ◆ rect_sum_offline(pts, rects) : 長方形の中の点の重みの合計（平面走査 + BIT）。O((N + Q) log N)
//   pts   : 点 {x, y, 重み} の一覧（個数を数えるなら重み 1）
//   rects : 長方形 {x1, y1, x2, y2}（[x1, x2) × [y1, y2) の半開区間）
//   T : 座標の型、W : 重みの型（どちらも自動で決まる）
template <class T, class W>
vector<W> rect_sum_offline(const vector<tuple<T, T, W>>& pts, const vector<array<T, 4>>& rects) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/plane_sweep_2d.cpp
    vector<T> ys;
    for (auto& [x, y, w] : pts) ys.push_back(y);
    sort(ys.begin(), ys.end()), ys.erase(unique(ys.begin(), ys.end()), ys.end());  // y を座標圧縮
    auto yid = [&](T y) { return int(lower_bound(ys.begin(), ys.end(), y) - ys.begin()); };  // y 以上の最初の番号
    vector<int> po(pts.size());
    iota(po.begin(), po.end(), 0);
    sort(po.begin(), po.end(), [&](int a, int b) { return get<0>(pts[a]) < get<0>(pts[b]); });
    // （内部用）長方形をばらしたイベント
    struct Ev { T x; int j, sign; };  // 「x 未満の点だけでの [y1, y2) の和」を sign 倍して答え j に足す
    vector<Ev> ev;
    for (int j = 0; j < (int)rects.size(); j++) {
        auto [x1, y1, x2, y2] = rects[j];
        if (x1 >= x2 || y1 >= y2) continue;
        ev.push_back({x2, j, +1}), ev.push_back({x1, j, -1});  // ★ [x1, x2) = (x < x2) − (x < x1)
    }
    sort(ev.begin(), ev.end(), [](const Ev& a, const Ev& b) { return a.x < b.x; });
    atcoder::fenwick_tree<W> bit(ys.size());
    vector<W> ans(rects.size(), W(0));
    size_t p = 0;
    for (auto& e : ev) {
        while (p < po.size() && get<0>(pts[po[p]]) < e.x) {  // ★ x が e.x 未満の点を全部 BIT に入れる
            bit.add(yid(get<1>(pts[po[p]])), get<2>(pts[po[p]]));
            p++;
        }
        auto [x1, y1, x2, y2] = rects[e.j];
        W s = bit.sum(yid(y1), yid(y2));  // ★ y が [y1, y2) の点の重み
        ans[e.j] += e.sign > 0 ? s : -s;
    }
    return ans;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 点 (1,1) (2,5) (4,3) (1000000000000,2)、長方形 [0,3)×[0,6) には (1,1) (2,5) の 2 個
    vector<tuple<long long, long long, long long>> pts = {{1, 1, 1}, {2, 5, 1}, {4, 3, 1}, {1000000000000LL, 2, 1}};
    auto r = rect_sum_offline<long long, long long>(pts, {{0, 0, 3, 6}, {2, 2, 5, 6}, {0, 0, 2000000000000LL, 3}, {5, 5, 5, 9}});
    assert((r == vector<long long>{2, 2, 2, 0}));  // [2,5)×[2,6): (2,5) (4,3) / [0,2e12)×[0,3): (1,1) と (1e12,2) / 空の長方形

    // ランダムテスト: 全点を見る愚直と比較（重みは負もあり、同じ座標の点もあり）
    mt19937 rr(137);
    for (int it = 0; it < 2000; it++) {
        int n = rr() % 12, q = rr() % 10;
        vector<tuple<long long, long long, long long>> P;
        for (int i = 0; i < n; i++) P.push_back({(long long)(rr() % 8), (long long)(rr() % 8), (long long)(rr() % 11) - 5});
        vector<array<long long, 4>> R;
        for (int j = 0; j < q; j++) R.push_back({(long long)(rr() % 10) - 1, (long long)(rr() % 10) - 1, (long long)(rr() % 10) - 1, (long long)(rr() % 10) - 1});
        auto got = rect_sum_offline<long long, long long>(P, R);
        for (int j = 0; j < q; j++) {
            long long s = 0;
            for (auto [x, y, w] : P) if (R[j][0] <= x && x < R[j][2] && R[j][1] <= y && y < R[j][3]) s += w;
            assert(got[j] == s);
        }
    }
    puts("OK");
}
