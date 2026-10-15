// =====================================================================
// graph/kruskal.cpp  最小全域木（クラスカル法 / プリム法）・最大全域木
//   本体: クラスカル法。辺を重みの小さい順に見て、閉路ができない辺だけ採用する。O(M log M)。ふつうはこちら
//   削除可: プリム法。頂点 i, j の間の重みを関数で渡す。全頂点の間に辺がある「密な」グラフ用（O(N^2)、辺のリストを作らない）
//   削除可: ブルーフカ法。「各頂点から、別の成分への一番安い辺」を自分で速く求められる、辺が多すぎるグラフ用（O(log N) 回の呼び出し）
// 依存: ACL の dsu（AtCoder では #include <atcoder/dsu> が使える）
// ABC / ARC での使用例:
//   ABC065 D - Built?  https://atcoder.jp/contests/abc065/tasks/arc076_b
//     → x 順・y 順で隣り合う町だけを辺にしてクラスカル
//   ABC218 E - Destruction  https://atcoder.jp/contests/abc218/tasks/abc218_e
//     → 重みを max(c, 0) にして（負の辺は残すほど得）、重みの合計 − 最小全域木の重み
//   AOJ ALDS1_12_A - Minimum Spanning Tree  https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ALDS1_12_A&lang=ja
//     → 隣接行列で与えられる。-1 を PRIM_INF にして prim_dense（ABC ではないが練習用の定番）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/dsu>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// T : 重みの型（long long など）
// vector<KEdge<long long>> es;  es.push_back({a, b, w});   // 無向辺 a-b 重み w
// auto r = kruskal(n, es);
//   r.cost       採用した辺の重みの合計
//   r.used       採用した辺の番号（es の添字）
//   r.connected  全頂点がつながったか（false なら最小全域「森」の結果）
// kruskal(n, es, true) で最大全域木
// ◆ KEdge<T> : 無向辺 {u, v, w: 重み}
template <class T> struct KEdge {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/kruskal.cpp
    int u, v;
    T w;
};
// ◆ KruskalResult<T> : kruskal の結果
//   cost : 採用した辺の重みの合計、used : 採用した辺の番号（es の添字）、connected : 全部つながったか
template <class T> struct KruskalResult {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/kruskal.cpp
    T cost;
    vector<int> used;
    bool connected;
};
// ◆ kruskal(n, es, maximize) : 最小全域木（maximize = true なら最大全域木）。O(M log M)
//   n        : 頂点数
//   es       : 辺の一覧（vector<KEdge<T>>）
//   maximize : true なら重みの大きい順に採用（既定 false）
//   注意 : つながらないグラフでは connected = false で、最小全域森の結果になる
template <class T>
KruskalResult<T> kruskal(int n, const vector<KEdge<T>>& es, bool maximize = false) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/kruskal.cpp
    vector<int> idx(es.size());
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&](int a, int b) {  // ★ 重みの小さい順（最大全域木なら大きい順）
        return maximize ? es[a].w > es[b].w : es[a].w < es[b].w;
    });
    atcoder::dsu uf(n);
    KruskalResult<T> r{T(0), {}, false};
    for (int i : idx) {
        if (uf.same(es[i].u, es[i].v)) continue;  // ★ すでにつながっている = 入れると閉路になるので捨てる
        uf.merge(es[i].u, es[i].v);
        r.cost += es[i].w;
        r.used.push_back(i);
    }
    r.connected = (int)r.used.size() == n - 1;  // ★ 木なら辺は n-1 本
    return r;
}

// ---------- 削除可: プリム法（密なグラフ用） O(N^2) ----------
// 全頂点の組に辺がある（完全グラフ）など、辺が N^2 本ある問題で、辺のリストを作らずに最小全域木を求める
// 【自分で定義するもの】cost(i, j) : 頂点 i と j を結ぶ辺の重み（辺がないなら PRIM_INF<T>）（ラムダ）
// auto r = prim_dense<long long>(n, cost);
//   r.cost       最小全域木の重み（つながらなければ connected = false で、それまでの和）
//   r.par[v]     木での v の親（頂点 0 が根。根とつながらない頂点は -1）
// 例: 平面上の点を全部つなぐ最小コスト（コスト = 2 点のマンハッタン距離など）
template <class T> constexpr T PRIM_INF = numeric_limits<T>::max() / 4;
// ◆ PrimResult<T> : prim_dense の結果。cost : 重みの合計、par[v] : 木での親（根 0 と、つながらない頂点は −1）、connected
template <class T> struct PrimResult {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/kruskal.cpp
    T cost;
    vector<int> par;
    bool connected;
};
// ◆ prim_dense<T>(n, cost) : 辺のリストを作らずに最小全域木を求める（プリム法）。O(N^2) 回 cost を呼ぶ
//   T    : 重みの型（必ず書く）
//   n    : 頂点数（N <= 5000 くらい）
//   cost : T cost(int i, int j)。i と j を結ぶ辺の重み。辺がなければ PRIM_INF<T> を返す
template <class T, class F>
PrimResult<T> prim_dense(int n, F cost) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/kruskal.cpp
    vector<T> best(n, PRIM_INF<T>);  // best[v] = 今の木から v へつなぐ一番安い辺
    vector<int> par(n, -1);
    vector<bool> used(n, false);
    PrimResult<T> r{T(0), {}, true};
    if (n == 0) return r;
    best[0] = 0;
    for (int it = 0; it < n; it++) {
        int v = -1;
        for (int u = 0; u < n; u++)  // ★ 木に入っていない頂点のうち、一番安くつなげるものを選ぶ
            if (!used[u] && (v == -1 || best[u] < best[v])) v = u;
        if (best[v] == PRIM_INF<T>) { r.connected = false; break; }
        used[v] = true, r.cost += best[v];
        for (int u = 0; u < n; u++) {  // ★ v が木に入ったので、v からの辺で best を更新
            if (used[u]) continue;
            T c = cost(v, u);
            if (c < best[u]) best[u] = c, par[u] = v;
        }
    }
    for (int v = 0; v < n; v++) if (!used[v]) par[v] = -1;
    r.par = par;
    return r;
}

// ---------- 削除可: ブルーフカ法（辺を全部は列挙できない「暗黙のグラフ」の最小全域木。ACL の dsu を使う） ----------
// 【自分で定義するもの】cheapest(comp) : comp[v] = 頂点 v の今の成分の番号（同じ成分なら同じ値）。
//     各頂点 v について「comp が違う頂点 u への一番安い辺」{重み, u} を返す（なければ {何でも, -1}）（ラムダ）
//     例: 重み = |a_u − a_v| + … のような式なら、ソートやセグ木で「自分と違う成分の中で一番よいもの」を速く探す
// auto r = boruvka<T>(n, cheapest);   r.cost, r.connected, r.edges（採用した辺 {u, v}）
// ★ 成分ごとに「外へ出る一番安い辺」を全部採用 → 成分の数が毎回半分以下になるので、cheapest を呼ぶのは O(log N) 回
// ◆ BoruvkaResult<T> : boruvka の結果。cost : 重みの合計、connected、edges : 採用した辺 {u, v}
template <class T> struct BoruvkaResult {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/kruskal.cpp
    T cost;
    bool connected;
    vector<pair<int, int>> edges;
};
// ◆ boruvka<T>(n, cheapest) : 辺が多すぎて列挙できないグラフの最小全域木（ブルーフカ法）
//   T        : 重みの型（必ず書く）
//   n        : 頂点数
//   cheapest : vector<pair<T, int>> cheapest(const vector<int>& comp)。comp[v] = v の今の成分の番号。
//              各頂点 v について、成分が違う頂点 u への一番安い辺 {重み, u}（なければ {何でも, −1}）を返す
//   注意 : cheapest が同じ成分の頂点を返すと assert で止まる。呼ばれる回数は O(log N) 回
template <class T, class F>
BoruvkaResult<T> boruvka(int n, F cheapest) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/kruskal.cpp
    atcoder::dsu uf(n);
    BoruvkaResult<T> r{T(0), true, {}};
    int comps = n;
    while (comps > 1) {
        vector<int> comp(n);
        for (int v = 0; v < n; v++) comp[v] = uf.leader(v);
        vector<pair<T, int>> c = cheapest(comp);
        vector<tuple<T, int, int>> best(n, {T(0), -1, -1});  // best[成分] = {重み, v, u}
        for (int v = 0; v < n; v++) {
            auto [w, u] = c[v];
            if (u == -1) continue;
            assert(comp[u] != comp[v]);
            auto& b = best[comp[v]];
            if (get<1>(b) == -1 || w < get<0>(b)) b = {w, v, u};  // ★ 成分から外へ出る一番安い辺
        }
        vector<tuple<T, int, int>> cand;
        for (int x = 0; x < n; x++) if (get<1>(best[x]) != -1) cand.push_back(best[x]);
        if (cand.empty()) { r.connected = false; break; }  // どこにも出られない → つながらない
        // ★ 候補の辺で閉路ができるのは、閉路の辺が全部同じ重みのときだけ（各成分の一番安い辺なので）。
        //    なので、閉路になる辺を dsu で飛ばすだけでよい（どれを飛ばしても重みの合計は同じ）
        for (auto [w, v, u] : cand)
            if (!uf.same(v, u)) uf.merge(v, u), r.cost += w, r.edges.push_back({v, u}), comps--;
    }
    return r;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    //  0 --1-- 1
    //  |  \    |
    //  4   3   2
    //  |     \ |
    //  3 --5-- 2
    vector<KEdge<long long>> es = {{0, 1, 1}, {1, 2, 2}, {0, 2, 3}, {0, 3, 4}, {3, 2, 5}};
    auto r = kruskal(4, es);
    assert(r.cost == 7 && r.connected && (r.used == vector<int>{0, 1, 3}));

    auto mx = kruskal(4, es, true);  // 最大全域木
    assert(mx.cost == 11);  // 5 (3-2), 4 (0-3), 0-2 は閉路になるので飛ばす, 2 (1-2)

    auto f = kruskal(5, es);  // 頂点 4 は孤立 → 森
    assert(!f.connected && f.cost == 7);

    // 採用した辺だけで木（隣接リスト）を作る
    vector<vector<int>> tree(4);
    for (int i : r.used) tree[es[i].u].push_back(es[i].v), tree[es[i].v].push_back(es[i].u);
    assert(tree[0].size() == 2);
    // ランダムテスト: 辺の選び方を全部試して、最小・最大の全域森と比較（負の重みも含む）
    {
        mt19937 rr(112);
        for (int it = 0; it < 2000; it++) {
            int n = rr() % 5 + 1, m = rr() % 8;
            vector<KEdge<long long>> e2;
            for (int i = 0; i < m; i++) e2.push_back({(int)(rr() % n), (int)(rr() % n), (long long)(rr() % 21) - 10});
            auto comps = [&](int mask) {
                atcoder::dsu d(n);
                int c = n;
                bool acyc = true;
                for (int i = 0; i < m; i++)
                    if (mask >> i & 1) {
                        if (d.same(e2[i].u, e2[i].v)) acyc = false;
                        else d.merge(e2[i].u, e2[i].v), c--;
                    }
                return make_pair(acyc, c);
            };
            int best_c = comps((1 << m) - 1).second;
            long long mn = LLONG_MAX, mx = LLONG_MIN;
            for (int mask = 0; mask < (1 << m); mask++) {
                auto [acyc, c] = comps(mask);
                if (!acyc || c != best_c) continue;
                long long s = 0;
                for (int i = 0; i < m; i++) if (mask >> i & 1) s += e2[i].w;
                mn = min(mn, s), mx = max(mx, s);
            }
            auto r1 = kruskal(n, e2), r2 = kruskal(n, e2, true);
            assert(r1.cost == mn && r2.cost == mx && r1.connected == (best_c == 1) && (int)r1.used.size() == n - best_c);
        }
    }
    // プリム法: 点 (0,0) (3,0) (0,4) (10,10) をマンハッタン距離でつなぐ → 3 + 4 + (0,4)-(10,10) の 16 = 23
    {
        vector<pair<long long, long long>> pt = {{0, 0}, {3, 0}, {0, 4}, {10, 10}};
        auto r = prim_dense<long long>(4, [&](int i, int j) { return llabs(pt[i].first - pt[j].first) + llabs(pt[i].second - pt[j].second); });
        assert(r.cost == 23 && r.connected && r.par[0] == -1);
    }
    // ランダムテスト: プリム法とクラスカル法（ランダムな完全グラフ・辺がない組あり）を比較
    {
        mt19937 r2(140);
        for (int it = 0; it < 2000; it++) {
            int n = r2() % 8 + 1;
            vector<vector<long long>> w(n, vector<long long>(n, PRIM_INF<long long>));
            vector<KEdge<long long>> e;
            for (int i = 0; i < n; i++)
                for (int j = i + 1; j < n; j++)
                    if (r2() % 4) { long long c = (long long)(r2() % 21) - 5; w[i][j] = w[j][i] = c, e.push_back({i, j, c}); }
            auto pr = prim_dense<long long>(n, [&](int i, int j) { return w[i][j]; });
            auto kr = kruskal(n, e);
            assert(pr.connected == kr.connected);
            if (kr.connected) {
                assert(pr.cost == kr.cost);
                long long s = 0;  // par でできる木の重みの和が cost と一致し、木になっている
                atcoder::dsu d(n);
                for (int v = 1; v < n; v++) { assert(pr.par[v] != -1 && w[v][pr.par[v]] != PRIM_INF<long long>); s += w[v][pr.par[v]]; d.merge(v, pr.par[v]); }
                assert(s == pr.cost && d.size(0) == n);
            }
        }
    }
    // ブルーフカ法: 数直線上の点 a = {1, 4, 6, 10}、重み = |a_i − a_j| の完全グラフ → 隣どうしをつなぐ 3 + 2 + 4 = 9
    {
        vector<long long> a = {1, 4, 6, 10};
        auto cheapest = [&](const vector<int>& comp) {  // 素朴に O(N^2)（本番ではソートなどで速くする）
            int n = a.size();
            vector<pair<long long, int>> res(n, {0, -1});
            for (int v = 0; v < n; v++)
                for (int u = 0; u < n; u++)
                    if (comp[u] != comp[v] && (res[v].second == -1 || llabs(a[u] - a[v]) < res[v].first)) res[v] = {llabs(a[u] - a[v]), u};
            return res;
        };
        auto r = boruvka<long long>(4, cheapest);
        assert(r.connected && r.cost == 9 && r.edges.size() == 3);
    }
    // ランダムテスト: クラスカル法と比較（重みが同じ辺がたくさんある場合も）
    {
        mt19937 rb(200);
        for (int it = 0; it < 3000; it++) {
            int n = rb() % 9 + 1;
            int W = (it % 2) ? 3 : 100;  // 半分は重みの種類を少なくして、同じ重みを多くする
            vector<vector<long long>> w(n, vector<long long>(n, -1));  // -1 は辺なし
            vector<KEdge<long long>> es;
            for (int i = 0; i < n; i++)
                for (int j = i + 1; j < n; j++)
                    if (rb() % 3) w[i][j] = w[j][i] = rb() % W, es.push_back({i, j, w[i][j]});
            auto cheapest = [&](const vector<int>& comp) {
                vector<pair<long long, int>> res(n, {0, -1});
                for (int v = 0; v < n; v++)
                    for (int u = 0; u < n; u++)
                        if (w[v][u] >= 0 && comp[u] != comp[v] && (res[v].second == -1 || w[v][u] < res[v].first || (w[v][u] == res[v].first && rb() % 2))) res[v] = {w[v][u], u};  // 同じ重みはランダムに選ぶ
                return res;
            };
            auto r = boruvka<long long>(n, cheapest);
            auto k = kruskal(n, es);
            assert(r.connected == k.connected);
            if (k.connected) {
                assert(r.cost == k.cost && (int)r.edges.size() == n - 1);
                atcoder::dsu d(n);
                long long s = 0;
                for (auto [u, v] : r.edges) { assert(w[u][v] >= 0 && !d.same(u, v)); d.merge(u, v), s += w[u][v]; }
                assert(s == r.cost);
            }
        }
    }
    puts("OK");
}
