// =====================================================================
// graph/bipartite_matching.cpp  二部マッチング（Hopcroft-Karp 法）
//   左側 L 個・右側 R 個の頂点と、左右をつなぐ辺があるとき、
//   端点を共有しないように選べる辺の最大本数を O(E √V) で求める
//   ※ ACL の mf_graph（最大流）でも解けるが、こちらの方が速く、使うときに書く量も少ない
//   最小頂点被覆（König の定理）・最大独立集合も求まる
// 依存: なし
// ABC / ARC での使用例:
//   ABC091 C - 2D Plane 2N Points  https://atcoder.jp/contests/abc091/tasks/arc092_a
//     → 赤（左）と青（右）を、条件を満たす組で辺にして最大マッチング（貪欲でも解ける）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/maxflow>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// BipartiteMatching bm(L, R);   左 0..L-1、右 0..R-1（左右で番号は別）
//   bm.add_edge(l, r)            左 l と右 r をつなぐ
//   int k = bm.solve()           最大マッチングの本数
//   bm.match_l[l]                左 l の相手の右の番号（いなければ -1）
//   bm.match_r[r]                右 r の相手の左の番号（いなければ -1）
// 典型: 「人と仕事」「赤い点と青い点」をペアにする最大数。
//       グリッドの隣り合うマスのペア（ドミノ）は、(i + j) の偶奇で左右に分ける
// ◆ struct BipartiteMatching : 二部グラフの最大マッチング（Hopcroft-Karp）。O(M √N)
//   BipartiteMatching(L, R) : 左 L 個・右 R 個の頂点で作る（左右で番号は別々に 0 から）
//   add_edge(l, r)          : 左 l と右 r をつなぐ
//   solve()                 : 最大マッチングの本数を返す。match_l / match_r に相手が入る
//   min_vertex_cover()      : 最小頂点被覆 {左の頂点, 右の頂点}（solve の後に呼ぶ）
struct BipartiteMatching {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bipartite_matching.cpp
    int L, R;
    vector<vector<int>> g;
    vector<int> match_l, match_r, dist;
    // L_ : 左の頂点数、R_ : 右の頂点数
    BipartiteMatching(int L_, int R_) : L(L_), R(R_), g(L_), match_l(L_, -1), match_r(R_, -1) {}
    // add_edge(l, r) : l は左の番号 0..L−1、r は右の番号 0..R−1（同じ組を 2 回入れても答えは変わらない）
    void add_edge(int l, int r) { g[l].push_back(r); }

    // （内部用）増加道を探すための距離を求める
    bool bfs() {  // ★ 未マッチの左頂点から、交互に「マッチしていない辺 → している辺」で進む距離を求める
        dist.assign(L, -1);
        queue<int> q;
        for (int l = 0; l < L; l++)
            if (match_l[l] == -1) dist[l] = 0, q.push(l);
        bool found = false;
        while (!q.empty()) {
            int l = q.front();
            q.pop();
            for (int r : g[l]) {
                int nl = match_r[r];
                if (nl == -1) found = true;  // ★ 未マッチの右に着いた = 増やせる道がある
                else if (dist[nl] == -1) dist[nl] = dist[l] + 1, q.push(nl);
            }
        }
        return found;
    }
    // （内部用）距離に沿って増加道を探す。再帰の深さは道の長さまで
    bool dfs(int l, vector<int>& it) {  // ★ 距離が 1 ずつ増える道だけを使って、増やせる道を探す
        for (int& i = it[l]; i < (int)g[l].size(); i++) {
            int r = g[l][i], nl = match_r[r];
            if (nl == -1 || (dist[nl] == dist[l] + 1 && dfs(nl, it))) {
                match_l[l] = r, match_r[r] = l;  // ★ 道に沿ってマッチングを入れ替える
                return true;
            }
        }
        dist[l] = -1;  // この頂点からはもう道がない
        return false;
    }
    // solve() : 最大マッチングの本数。何回呼んでもよい（辺を足してから呼び直すと続きから増やす）
    int solve() {
        int res = 0;
        for (int l = 0; l < L; l++) res += match_l[l] != -1;
        while (bfs()) {  // ★ 1 回の BFS で、最短の増加道をまとめて全部使う（√V 回で終わる）
            vector<int> it(L, 0);
            for (int l = 0; l < L; l++)
                if (match_l[l] == -1 && dfs(l, it)) res++;
        }
        return res;
    }

    // --- 削除可: 最小頂点被覆（solve の後に呼ぶ） ---
    // すべての辺の少なくとも一方の端点を含む、最小の頂点集合。大きさ = 最大マッチング（König の定理）
    // 返り値: {左側で選ぶ頂点, 右側で選ぶ頂点}
    // 最大独立集合（どの 2 頂点も辺でつながらない最大の集合）= 最小頂点被覆の補集合
    // min_vertex_cover() : solve の後に呼ぶ。返り値 = {左側で選ぶ頂点, 右側で選ぶ頂点}
    pair<vector<int>, vector<int>> min_vertex_cover() const {
        vector<bool> vl(L, false), vr(R, false);
        queue<int> q;
        for (int l = 0; l < L; l++)
            if (match_l[l] == -1) vl[l] = true, q.push(l);
        while (!q.empty()) {  // ★ 未マッチの左から交互道でたどれる頂点に印をつける
            int l = q.front();
            q.pop();
            for (int r : g[l])
                if (!vr[r]) {
                    vr[r] = true;
                    int nl = match_r[r];
                    if (nl != -1 && !vl[nl]) vl[nl] = true, q.push(nl);
                }
        }
        vector<int> cl, cr;
        for (int l = 0; l < L; l++) if (!vl[l]) cl.push_back(l);  // ★ 左は「たどれなかった」頂点
        for (int r = 0; r < R; r++) if (vr[r]) cr.push_back(r);   // ★ 右は「たどれた」頂点
        return {cl, cr};
    }
    // --- 削除可ここまで ---
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    BipartiteMatching bm(3, 3);
    bm.add_edge(0, 0), bm.add_edge(0, 1), bm.add_edge(1, 0), bm.add_edge(2, 1), bm.add_edge(2, 2);
    assert(bm.solve() == 3);
    for (int l = 0; l < 3; l++) assert(bm.match_r[bm.match_l[l]] == l);

    // ランダムテスト: ACL の最大流と比較、最小頂点被覆が辺をすべて覆うか
    mt19937 rng(16);
    for (int it = 0; it < 2000; it++) {
        int L = rng() % 8 + 1, R = rng() % 8 + 1, E = rng() % 20;
        BipartiteMatching b(L, R);
        atcoder::mf_graph<int> mf(L + R + 2);
        int s = L + R, t = s + 1;
        vector<pair<int, int>> es;
        for (int l = 0; l < L; l++) mf.add_edge(s, l, 1);
        for (int r = 0; r < R; r++) mf.add_edge(L + r, t, 1);
        for (int i = 0; i < E; i++) {
            int l = rng() % L, r = rng() % R;
            b.add_edge(l, r), mf.add_edge(l, L + r, 1), es.push_back({l, r});
        }
        int k = b.solve();
        assert(k == mf.flow(s, t));
        set<int> used;
        for (int l = 0; l < L; l++)
            if (b.match_l[l] != -1) assert(b.match_r[b.match_l[l]] == l && used.insert(b.match_l[l]).second);
        auto [cl, cr] = b.min_vertex_cover();
        assert((int)(cl.size() + cr.size()) == k);
        set<int> SL(cl.begin(), cl.end()), SR(cr.begin(), cr.end());
        for (auto [l, r] : es) assert(SL.count(l) || SR.count(r));
    }
    puts("OK");
}
