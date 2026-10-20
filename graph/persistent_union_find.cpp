// =====================================================================
// graph/persistent_union_find.cpp  部分永続 UnionFind（過去の時点の連結性を聞ける）
//   unite を呼ぶたびに時刻が 1 進む。「時刻 t（t 回目の unite の直後）の時点で a と b はつながっていたか」
//   「その時点での成分の大きさは」に答えられる。更新は最新の時刻にだけできる（部分永続）
//   経路圧縮をしないので find は O(log N)（大きさで併合するので木の高さが log N 以下）
//   ★ 典型: 「a と b が初めてつながるのは何回目の操作か」→ same(a, b, t) が t について単調なので二分探索
// 依存: なし
// ABC / ARC での使用例:
//   AGC002 D - Stamp Rally  https://atcoder.jp/contests/agc002/tasks/agc002_d
//     → 辺を番号順に unite。クエリごとに「x と y の成分の大きさの合計（同じ成分なら 1 つ分）が z 以上になる最小の時刻 t」を二分探索（AGC だが部分永続 UnionFind の定番）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// PersistentUnionFind uf(n);
//   uf.unite(a, b)       時刻を 1 進めて a と b をつなぐ（すでに同じでも時刻は進む）。新しくつないだら true
//   uf.now               今の時刻（unite を呼んだ回数）
//   uf.find(x, t)        時刻 t での x の根
//   uf.same(a, b, t)     時刻 t で a と b がつながっていたか
//   uf.size(x, t)        時刻 t での x の成分の大きさ
//   uf.first_time(a, b)  a と b が初めてつながった時刻（まだなら -1）。O(log Q log N)
// ◆ struct PersistentUnionFind : 部分永続 UnionFind（過去の時刻のつながりも聞ける）。各操作 O(log N)
//   PersistentUnionFind(n) : 頂点 0..n−1、時刻 0
//   unite(a, b)            : 時刻を 1 進めて a と b をつなぐ。新しくつないだら true
//   find(x, t)             : 時刻 t での x の根
//   same(a, b, t)          : 時刻 t でつながっていたか
//   size(x, t)             : 時刻 t での x の成分の頂点数
//   first_time(a, b)       : a と b が初めてつながった時刻（まだなら −1）
//   now                    : 今の時刻（unite を呼んだ回数）
struct PersistentUnionFind {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/persistent_union_find.cpp
    int now = 0;
    vector<int> par, when;               // when[x] = x が根でなくなった時刻（根なら INT_MAX）
    vector<vector<pair<int, int>>> hist;  // hist[r] = {時刻, その時刻からの成分の大きさ} の履歴（r が根の間）
    // n : 頂点数
    PersistentUnionFind(int n) : par(n), when(n, INT_MAX), hist(n, {{0, 1}}) { iota(par.begin(), par.end(), 0); }
    // find(x, t) : x : 頂点、t : 時刻（0 = 何もつないでいない状態、t = t 回目の unite の直後）
    long long find(int x, int t) const {
        while (when[x] <= t) x = par[x];  // ★ 時刻 t までに親ができていたら上へ（それより後の併合は見ない）
        return x;
    }
    // unite(a, b) : 今の時刻に対してだけつなげる（過去は変えられない）。同じ成分でも時刻は 1 進む
    bool unite(int a, int b) {
        now++;
        a = find(a, now), b = find(b, now);
        if (a == b) return false;
        if (hist[a].back().second < hist[b].back().second) swap(a, b);  // ★ 大きい方を根に（木の高さ log N）
        par[b] = a, when[b] = now;
        hist[a].push_back({now, hist[a].back().second + hist[b].back().second});
        return true;
    }
    // same(a, b, t) : t : 時刻（0..now）
    bool same(int a, int b, int t) const { return find(a, t) == find(b, t); }
    // size(x, t) : t : 時刻（0..now）
    long long size(int x, int t) const {
        int r = find(x, t);
        auto it = upper_bound(hist[r].begin(), hist[r].end(), make_pair(t, INT_MAX));  // 時刻 t 以前の最後の記録
        return prev(it)->second;
    }
    // first_time(a, b) : 二分探索で求める。a == b なら 0
    long long first_time(int a, int b) const {
        if (!same(a, b, now)) return -1;
        int lo = -1, hi = now;  // same(a, b, hi) は true、same(a, b, lo) は false（lo = -1 は番兵）
        while (hi - lo > 1) {
            int mid = (lo + hi) / 2;
            (same(a, b, mid) ? hi : lo) = mid;
        }
        return hi;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 時刻 1: 0-1、時刻 2: 2-3、時刻 3: 1-2、時刻 4: 0-3（すでに同じ）
    {
        PersistentUnionFind uf(5);
        assert(uf.unite(0, 1) && uf.unite(2, 3) && uf.unite(1, 2) && !uf.unite(0, 3));
        assert(uf.now == 4);
        assert(!uf.same(0, 3, 2) && uf.same(0, 3, 3) && uf.same(0, 1, 1) && !uf.same(0, 1, 0));
        assert(uf.size(0, 0) == 1 && uf.size(0, 1) == 2 && uf.size(3, 2) == 2 && uf.size(3, 3) == 4);
        assert(uf.first_time(0, 3) == 3 && uf.first_time(1, 0) == 1 && uf.first_time(0, 4) == -1 && uf.first_time(2, 2) == 0);
    }
    // ランダムテスト: 各時刻の状態を全部保存した素朴な UnionFind と比較
    {
        mt19937 rr(182);
        for (int it = 0; it < 500; it++) {
            int n = rr() % 10 + 1, Q = rr() % 30;
            PersistentUnionFind uf(n);
            vector<vector<int>> snap;  // snap[t][v] = 時刻 t での成分のラベル
            vector<int> lab(n);
            iota(lab.begin(), lab.end(), 0);
            snap.push_back(lab);
            for (int q = 0; q < Q; q++) {
                int a = rr() % n, b = rr() % n;
                bool merged = lab[a] != lab[b];
                assert(uf.unite(a, b) == merged);
                if (merged) { int x = lab[b]; for (int& l : lab) if (l == x) l = lab[a]; }
                snap.push_back(lab);
            }
            for (int t = 0; t <= Q; t++)
                for (int a = 0; a < n; a++) {
                    int sz = count(snap[t].begin(), snap[t].end(), snap[t][a]);
                    assert(uf.size(a, t) == sz);
                    for (int b = 0; b < n; b++) assert(uf.same(a, b, t) == (snap[t][a] == snap[t][b]));
                }
            for (int v = 0; v < n; v++) {  // 大きさで併合しているので、根までの段数は log2 N 以下
                int depth = 0;
                for (int x = v; uf.when[x] <= uf.now; x = uf.par[x]) depth++;
                assert((1 << depth) <= n);
            }
            for (int a = 0; a < n; a++)
                for (int b = 0; b < n; b++) {
                    int want = -1;
                    for (int t = 0; t <= Q && want == -1; t++) if (snap[t][a] == snap[t][b]) want = t;
                    assert(uf.first_time(a, b) == want);
                }
        }
    }
    puts("OK");
}
