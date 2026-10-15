// =====================================================================
// techniques/event_sort.cpp  イベントソート（1 次元の平面走査）
//   区間の「始まり」と「終わり」をイベントとして座標順に並べ、左から順に見ていく
//   → 重なりの最大数、塗られた長さ、ちょうど k 個に覆われている長さ、点 x が何個の区間に入っているか
//   座標が大きくても（1e18 でも）よい。いもす法の「座標が大きい版」。O(N log N)
//   ★ 間違えやすいのは「同じ座標で始まりと終わりが重なったとき」の扱い
//     ここでは区間を半開区間 [l, r) に統一し、同じ座標のイベントは全部まとめて処理してから数を記録する
//     → [1, 3) と [3, 5) は重ならない（座標 3 は後ろの区間だけに入る）
//     自分でイベントソートを書くときは、(1) 同じ座標をまとめて処理するか、(2) 同じ座標では「終わり」を先に並べるか、
//     のどちらかが必要（両方ないと、触れているだけの区間が一瞬重なったと数えて最大値が 1 多くなる）。このライブラリは両方している
//     閉区間 [l, r]（整数）を扱うときは add(l, r + 1) と入れる（日付・時刻の問題はたいていこちら）
// 依存: なし
// ABC / ARC での使用例:
//   ABC221 D - Online games  https://atcoder.jp/contests/abc221/tasks/abc221_d
//     → A 日目から B 日間 = [A, A + B)。length_exactly() の ans[k] がそのまま答え
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// T : 座標の型（long long など）
// EventSort<long long> es;
//   es.add(l, r)               半開区間 [l, r) を追加（l >= r なら無視）
//   es.build()                 全部 add してから 1 回呼ぶ
//   es.profile                 {x, c} の列: 座標 x から次の x までは c 個の区間に覆われている（c が変わる点だけ）
//   es.max_overlap()           重なりの最大数
//   es.length_at_least(k)      k 個以上に覆われている長さの合計（k = 1 で「どれかに覆われている長さ」）
//   es.length_exactly()        ans[k] = ちょうど k 個に覆われている長さ（k = 1..区間の数。0 個の部分は数えない）
//   es.count_at(x)             点 x を含む区間の個数 O(log N)
// ◆ struct EventSort : イベントソート（区間の始まり +1、終わり −1 を座標順に処理する）
//   add(l, r)            : 半開区間 [l, r) を追加（l >= r なら無視）
//   build()              : 全部 add してから 1 回呼ぶ。O(N log N)
//   max_overlap()        : 重なりの最大数
//   length_at_least(k)   : k 個以上に覆われている長さの合計
//   length_exactly()     : ans[k] = ちょうど k 個に覆われている長さ
//   count_at(x)          : 点 x を含む区間の個数。O(log N)
//   注意 : 閉区間 [l, r] の整数（日付など）は add(l, r + 1) として入れる
template <class T>
struct EventSort {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/event_sort.cpp
    vector<pair<T, int>> ev;        // {座標, +1 なら始まり / -1 なら終わり}
    vector<pair<T, int>> profile;   // {座標, その座標から先の被覆数}
    int n = 0;
    // add(l, r) : l, r : 区間 [l, r) の両端
    void add(T l, T r) {
        if (l >= r) return;
        ev.push_back({l, +1}), ev.push_back({r, -1}), n++;
    }
    // build() : add を全部終えてから呼ぶ（build の後に add しても反映されない）
    void build() {
        sort(ev.begin(), ev.end());  // ★ 座標順。同じ座標では -1（終わり）が +1（始まり）より先に来る
        profile.clear();
        int c = 0;
        for (size_t i = 0; i < ev.size();) {
            T x = ev[i].first;
            while (i < ev.size() && ev[i].first == x) c += ev[i++].second;  // ★ 同じ座標のイベントは全部まとめて処理してから記録
            if (profile.empty() || profile.back().second != c) profile.push_back({x, c});
        }
    }
    // max_overlap() : 一番多く重なっている所の区間の個数
    int max_overlap() const {
        int m = 0;
        for (auto& p : profile) m = max(m, p.second);
        return m;
    }
    // length_at_least(k) : k : 1 以上（k = 1 で「どれかに覆われている長さ」）
    T length_at_least(int k) const {
        T s = 0;
        for (size_t i = 0; i + 1 < profile.size(); i++)
            if (profile[i].second >= k) s += profile[i + 1].first - profile[i].first;  // ★ 次の変化点までの長さ
        return s;
    }
    // length_exactly() : 長さ（区間の数 + 1）の配列。ans[0] は 0（隙間の長さは数えない）
    vector<T> length_exactly() const {
        vector<T> ans(n + 1, 0);
        for (size_t i = 0; i + 1 < profile.size(); i++) ans[profile[i].second] += profile[i + 1].first - profile[i].first;
        ans[0] = 0;  // 区間と区間の隙間（0 個の部分）は長さが決まらないので数えない
        return ans;
    }
    // count_at(x) : x : 調べる点
    int count_at(T x) const {
        auto it = upper_bound(profile.begin(), profile.end(), make_pair(x, INT_MAX));  // ★ x 以下で一番右の変化点
        return it == profile.begin() ? 0 : prev(it)->second;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // [1, 5), [3, 8), [5, 6), [10, 12)
    //   座標 1..3: 1 個、3..5: 2 個、5..6: 2 個（[1,5) が終わり [5,6) が始まる）、6..8: 1 個、10..12: 1 個
    EventSort<long long> es;
    es.add(1, 5), es.add(3, 8), es.add(5, 6), es.add(10, 12);
    es.build();
    assert(es.max_overlap() == 2);
    assert(es.length_at_least(1) == 9 && es.length_at_least(2) == 3);  // 1..8 の 7 と 10..12 の 2 / 3..6 の 3
    auto ex = es.length_exactly();
    assert(ex[1] == 6 && ex[2] == 3 && ex[3] == 0);  // 1 個: 1..3 の 2 + 6..8 の 2 + 10..12 の 2
    assert(es.count_at(5) == 2 && es.count_at(8) == 0 && es.count_at(0) == 0 && es.count_at(11) == 1);

    // 閉区間 [l, r] の整数（日付など）: 1 日目〜3 日目と 3 日目〜4 日目 → 3 日目は 2 人
    EventSort<long long> d;
    d.add(1, 3 + 1), d.add(3, 4 + 1);
    d.build();
    assert(d.count_at(3) == 2 && d.max_overlap() == 2);

    // ランダムテスト: 小さい座標で、各点ごとに覆う区間を数えた愚直と比較
    mt19937 rr(133);
    for (int it = 0; it < 3000; it++) {
        int m = rr() % 8;
        EventSort<long long> e;
        vector<int> cov(25, 0);
        for (int i = 0; i < m; i++) {
            int l = rr() % 22, r = rr() % 22;
            e.add(l, r);
            for (int x = l; x < r; x++) cov[x]++;
        }
        e.build();
        int real_n = 0;
        for (auto& p : e.ev) real_n += p.second == 1;
        int mx = 0;
        long long ge1 = 0, ge2 = 0;
        vector<long long> exact(real_n + 1, 0);
        for (int x = 0; x < 25; x++) {
            mx = max(mx, cov[x]), ge1 += cov[x] >= 1, ge2 += cov[x] >= 2;
            if (cov[x] > 0) exact[cov[x]]++;
            assert(e.count_at(x) == cov[x]);
        }
        assert(e.max_overlap() == mx && e.length_at_least(1) == ge1 && e.length_at_least(2) == ge2 && e.length_exactly() == exact);
    }
    puts("OK");
}
