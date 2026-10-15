// =====================================================================
// ds/interval_set.cpp  区間を set（map）で管理
//   数直線上の「塗られた部分」を、重ならない区間 [l, r) の集まりとして持つ
//   区間の追加（重なり・隣接は 1 つにまとめる）・削除・x を含むか・x 以上で塗られていない最小の値 を O(log N)（ならし）
//   座標は long long の範囲なら何でもよい（座標圧縮いらない）
// 依存: なし
// ABC / ARC での使用例:
//   ABC256 D - Union of Interval  https://atcoder.jp/contests/abc256/tasks/abc256_d
//     → 全部 insert して mp をそのまま出力
//   ABC228 D - Linear Probing  https://atcoder.jp/contests/abc228/tasks/abc228_d
//     → 埋まった位置 x を [x, x+1) として入れ、空き位置を mex で探す（末尾を越えたら mex(0)）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// IntervalSet st;
//   st.insert(l, r)     [l, r) を塗る（l >= r なら何もしない）
//   st.erase(l, r)      [l, r) を消す
//   st.contains(x)      x が塗られているか
//   st.get(x)           x を含む区間 {l, r}（含まれなければ {x, x}）
//   st.mex(x)           x 以上で塗られていない最小の値
//   st.total            塗られている長さの合計
//   st.mp               区間の一覧（map: 左端 → 右端）。for (auto [l, r] : st.mp) で小さい順に見られる
// 整数の点を扱うなら、点 x を [x, x + 1) として入れる
// ◆ struct IntervalSet : 区間の集合（塗った範囲を、重ならない区間の map で管理）。各操作ならし O(log N)
//   insert(l, r)   : [l, r) を塗る
//   erase(l, r)    : [l, r) を消す
//   get(x)         : x を含む区間 {l, r}（なければ {x, x}）
//   contains(x)    : x が塗られているか
//   mex(x)         : x 以上で塗られていない最小の値
//   total          : 塗られている長さの合計
//   mp             : 区間の一覧（左端 → 右端）
struct IntervalSet {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/interval_set.cpp
    map<long long, long long> mp;  // 重ならず、隣り合ってもいない区間 [l, r)
    long long total = 0;

    // insert(l, r) : l >= r なら何もしない。隣り合う区間はつなげて 1 つにする
    void insert(long long l, long long r) {
        if (l >= r) return;
        auto it = mp.upper_bound(l);
        if (it != mp.begin() && prev(it)->second >= l) it = prev(it);  // ★ l の左から始まり l に届く区間があれば、そこから
        while (it != mp.end() && it->first <= r) {                     // ★ [l, r) と重なる・隣り合う区間を全部吸収
            l = min(l, it->first), r = max(r, it->second);
            total -= it->second - it->first;
            it = mp.erase(it);
        }
        mp[l] = r, total += r - l;
    }
    // erase(l, r) : l >= r なら何もしない
    void erase(long long l, long long r) {
        if (l >= r) return;
        auto it = mp.upper_bound(l);
        if (it != mp.begin() && prev(it)->second > l) it = prev(it);
        while (it != mp.end() && it->first < r) {  // ★ [l, r) と重なる区間を取り出し、はみ出した部分だけ戻す
            auto [a, b] = *it;
            total -= b - a;
            it = mp.erase(it);
            if (a < l) mp[a] = l, total += l - a;
            if (b > r) mp[r] = b, total += b - r;
        }
    }
    // get(x) : x : 調べる点
    pair<long long, long long> get(long long x) const {
        auto it = mp.upper_bound(x);  // ★ 左端が x 以下で一番右の区間だけ見ればよい
        if (it != mp.begin() && prev(it)->second > x) return *prev(it);
        return {x, x};
    }
    // contains(x) : x : 調べる点（整数の点 x は [x, x+1) として入れておく）
    bool contains(long long x) const { auto [l, r] = get(x); return l < r; }
    // mex(x) : x : ここから右を探す
    long long mex(long long x) const { return get(x).second; }  // ★ x を含む区間があればその右端、なければ x
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    IntervalSet st;
    st.insert(1, 3), st.insert(5, 7), st.insert(3, 5);  // [1,3) と [3,5) と [5,7) は隣り合うので [1,7) になる
    assert(st.mp.size() == 1 && st.get(4) == make_pair(1LL, 7LL) && st.total == 6);
    st.erase(2, 4);  // [1,2) と [4,7)
    assert(st.mp.size() == 2 && !st.contains(3) && st.contains(4) && st.mex(1) == 2 && st.mex(4) == 7 && st.mex(8) == 8 && st.total == 4);
    st.insert(-1000000000000LL, -999999999999LL);  // 大きな座標もそのまま
    assert(st.contains(-1000000000000LL) && st.total == 5);

    // ランダムテスト: 0..29 の点を bool 配列で持ち、愚直と比較
    mt19937 rr(127);
    for (int it = 0; it < 2000; it++) {
        IntervalSet s;
        vector<bool> on(32, false);
        for (int q = 0; q < 30; q++) {
            int l = rr() % 31, r = rr() % 31;
            if (rr() % 2) { s.insert(l, r); for (int x = l; x < r; x++) on[x] = true; }
            else { s.erase(l, r); for (int x = l; x < r; x++) on[x] = false; }
            int tot = 0;
            for (int x = 0; x < 30; x++) tot += on[x];
            assert(s.total == tot);
            long long prev_r = LLONG_MIN;  // 区間どうしが重ならず、隣り合ってもいない
            for (auto [a, b] : s.mp) assert(a < b && a > prev_r), prev_r = b;
            for (int x = 0; x < 31; x++) {
                assert(s.contains(x) == on[x]);
                int m = x;
                while (on[m]) m++;
                assert(s.mex(x) == m);
                auto [a, b] = s.get(x);
                if (on[x]) { assert(a <= x && x < b && (a == 0 || !on[a - 1]) && !on[b]); for (long long y = a; y < b; y++) assert(on[y]); }
                else assert(a == x && b == x);
            }
        }
    }
    puts("OK");
}
