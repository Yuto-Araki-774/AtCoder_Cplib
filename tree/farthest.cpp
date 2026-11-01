// =====================================================================
// tree/farthest.cpp  各頂点から一番遠い頂点（直径の両端を使う）
//   ★ どの頂点 v についても、一番遠い頂点は「直径の両端 a, b のどちらか」になる
//     → a からの距離と b からの距離を 1 回ずつ求めれば、全頂点の答えが O(N) でわかる（重み付きでもよい）
//   全方位木 DP（rerooting.cpp）でも求まるが、こちらは定義するものがない
// 依存: tree/tree.cpp の Tree（先に貼る）。build はしなくてよい
// ABC / ARC での使用例:
//   ABC222 F - Expensive Expense  https://atcoder.jp/contests/abc222/tasks/abc222_f
//     → 各町 j に重み D_j の辺で葉 N + j を足した木で TreeFarthest。答え(i) = 一番遠い端までの距離だが、
//        その端が i 自身の葉 N + i なら、もう一方の端までの距離を使う
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;
// テスト用: tree.cpp の Tree を読み込む（提出するときは、tree.cpp のコピー範囲をこのファイルのコピー範囲より先に貼る）
#define TREE_NO_MAIN
#include "tree.cpp"

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体（tree.cpp の Tree が必要） ----------
// Tree<> t(n); t.add_edge(a, b, w); ...（build はしなくてよい）
// TreeFarthest f(t);
//   f.d[v]       v から一番遠い頂点までの距離（重みの和）
//   f.far[v]     その頂点（どちらの端も同じ距離なら a）
//   f.a, f.b     直径の両端。直径の長さは f.da[f.b]
//   f.da[v]      a から v までの距離、f.db[v] : b から v までの距離（d[v] = max(da[v], db[v])）
// ◆ struct TreeFarthest<T> : 全頂点について「一番遠い頂点までの距離」と「その頂点」。O(N)
//   TreeFarthest(t) : t は辺を入れた Tree（build 不要。TreeDP など Tree を継承したものでもよい）
//   d[v], far[v]    : v から一番遠い頂点までの距離、その頂点
//   a, b, da, db    : 直径の両端、それぞれからの距離
//   注意 : 辺の重みは 0 以上であること
template <class T>
struct TreeFarthest {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/farthest.cpp
    int a = 0, b = 0;
    vector<T> da, db, d;  // a からの距離、b からの距離、一番遠い頂点までの距離
    vector<int> far;      // 一番遠い頂点
    // t : 辺を入れた Tree（build 不要）
    TreeFarthest(const Tree<T>& t) {
        int n = t.n;
        auto from = [&](int s) {  // s から全頂点への距離（重み付き、再帰なし）
            vector<T> dist(n, T(0));
            vector<int> st = {s};
            vector<bool> seen(n, false);
            seen[s] = true;
            while (!st.empty()) {
                int v = st.back();
                st.pop_back();
                for (auto [to, w] : t.adj[v])
                    if (!seen[to]) seen[to] = true, dist[to] = dist[v] + w, st.push_back(to);
            }
            return dist;
        };
        auto argmax = [&](const vector<T>& v) { return int(max_element(v.begin(), v.end()) - v.begin()); };
        a = argmax(from(0));   // ★ 適当な点から一番遠い点 a は直径の端
        da = from(a);
        b = argmax(da);        // ★ a から一番遠い点 b がもう一方の端
        db = from(b);
        d.resize(n), far.resize(n);
        for (int v = 0; v < n; v++) {
            if (da[v] >= db[v]) d[v] = da[v], far[v] = a;  // ★ 一番遠いのは a か b のどちらか
            else d[v] = db[v], far[v] = b;
        }
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    //       0
    //    2/   \3
    //    1     2
    //  4/ \1    \10
    //  3   4     5
    Tree<long long> t(6);
    t.add_edge(0, 1, 2), t.add_edge(0, 2, 3), t.add_edge(1, 3, 4), t.add_edge(1, 4, 1), t.add_edge(2, 5, 10);
    TreeFarthest f(t);
    assert(f.da[f.b] == 19);  // 直径 3-1-0-2-5
    assert((f.d == vector<long long>{13, 15, 10, 19, 16, 19}));
    assert(f.far[0] == 5 && f.far[5] == 3 && f.far[4] == 5);

    // ABC222 F の形: 道 1-2-3（重み 2, 3）、D = {1, 2, 3}。手計算で答えは 8, 6, 6
    {
        int N = 3;
        vector<long long> D = {1, 2, 3};
        Tree<long long> g(2 * N);
        g.add_edge(0, 1, 2), g.add_edge(1, 2, 3);
        for (int j = 0; j < N; j++) g.add_edge(j, N + j, D[j]);  // 町 j に葉 N + j を足す
        TreeFarthest F(g);
        vector<long long> ans;
        for (int i = 0; i < N; i++) ans.push_back(F.far[i] != N + i ? F.d[i] : (F.far[i] == F.a ? F.db[i] : F.da[i]));
        assert((ans == vector<long long>{8, 6, 6}));
    }

    // ランダムテスト: 全点対の距離を求めて比べる
    mt19937 rng(2024);
    for (int it = 0; it < 2000; it++) {
        int n = rng() % 12 + 1;
        Tree<long long> u(n);
        for (int v = 1; v < n; v++) u.add_edge(rng() % v, v, rng() % 5);
        TreeFarthest F(u);
        u.build(0);
        for (int v = 0; v < n; v++) {
            long long mx = 0;
            for (int w = 0; w < n; w++) mx = max(mx, u.wdist(v, w));
            assert(F.d[v] == mx && u.wdist(v, F.far[v]) == mx);
            assert(F.da[v] == u.wdist(F.a, v) && F.db[v] == u.wdist(F.b, v));
        }
        long long diam = 0;
        for (int v = 0; v < n; v++) for (int w = 0; w < n; w++) diam = max(diam, u.wdist(v, w));
        assert(F.da[F.b] == diam);
    }
    puts("OK");
}
