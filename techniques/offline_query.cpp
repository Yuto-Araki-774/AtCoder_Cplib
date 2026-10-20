// =====================================================================
// techniques/offline_query.cpp  オフラインクエリ（クエリを先に全部読んで、並べ替えて処理する）
//   「x 以下のものだけ使ったら」という形のクエリは、x の小さい順に並べ替えると
//   データを「追加するだけ」で済む（削除がいらない）ので、UnionFind や BIT で答えられる
//   本体A: 重みが x 以下の辺だけを使ったときの連結判定・成分の大きさ（UnionFind）  O((M + Q) log)
//   本体B: 区間 [l, r) の中で x 以下の値の個数（BIT）                                 O((N + Q) log N)
//   A と B は互いに独立。答えは入力のクエリの順番で返す
// 依存: ACL の dsu、fenwick_tree（AtCoder ではテンプレートの atcoder/all に含まれる）
// ABC / ARC での使用例:
//   ABC040 D - 道路の老朽化対策について  https://atcoder.jp/contests/abc040/tasks/abc040_d
//     → 「w 年より後」の道路だけ。年を −1 倍して重み −y、クエリ x = −(w+1) で offline_connectivity の成分の大きさ
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/dsu>
#include <atcoder/fenwicktree>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 重み x 以下の辺だけでの連結性 ----------
// edges[i] = {u, v, w}、queries[j] = {a, b, x}
// 返り値[j] = {a と b が連結か, a の連結成分の頂点数}（重みが x 以下の辺だけを使ったとき）
// 「x 以上の辺だけ」なら、w と x を両方 -1 倍して呼ぶ。「x より大きい」なら x+1 以上として扱う
// ◆ offline_connectivity(n, edges, queries) : 重み x 以下の辺だけを使ったときの連結性（クエリを先読み）。O((M + Q) log)
//   n       : 頂点数
//   edges   : 辺 {u, v, w} の一覧（無向）
//   queries : クエリ {a, b, x} の一覧
//   返り値[j] : {a と b がつながっているか, a の成分の頂点数}
template <class T>
vector<pair<bool, long long>> offline_connectivity(int n, vector<tuple<int, int, T>> edges, const vector<tuple<int, int, T>>& queries) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/offline_query.cpp
    sort(edges.begin(), edges.end(), [](auto& p, auto& q) { return get<2>(p) < get<2>(q); });
    vector<int> ord(queries.size());
    iota(ord.begin(), ord.end(), 0);
    sort(ord.begin(), ord.end(), [&](int i, int j) { return get<2>(queries[i]) < get<2>(queries[j]); });  // ★ クエリを x の小さい順に
    atcoder::dsu uf(n);
    vector<pair<bool, long long>> ans(queries.size());
    size_t e = 0;
    for (int j : ord) {
        auto [a, b, x] = queries[j];
        while (e < edges.size() && get<2>(edges[e]) <= x) uf.merge(get<0>(edges[e]), get<1>(edges[e])), e++;  // ★ x 以下の辺を追加するだけ
        ans[j] = {uf.same(a, b), uf.size(a)};
    }
    return ans;
}

// ---------- 本体B: 区間の中で x 以下の値の個数 ----------
// queries[j] = {l, r, x}。返り値[j] = a[l..r) の中で x 以下の値の個数
// 「x 未満」なら x - 1 以下（整数のとき）、「x 以上」なら (r - l) - (x - 1 以下の個数)
// ◆ offline_count_le(a, queries) : 区間 a[l..r) の中で x 以下の値の個数（クエリを先読み）。O((N + Q) log N)
//   a       : 数列
//   queries : {l, r, x} の一覧（0 <= l <= r <= N）
template <class T>
vector<long long> offline_count_le(const vector<T>& a, const vector<tuple<int, int, T>>& queries) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/offline_query.cpp
    int n = a.size();
    vector<int> idx(n), ord(queries.size());
    iota(idx.begin(), idx.end(), 0), iota(ord.begin(), ord.end(), 0);
    sort(idx.begin(), idx.end(), [&](int i, int j) { return a[i] < a[j]; });  // 値の小さい順
    sort(ord.begin(), ord.end(), [&](int i, int j) { return get<2>(queries[i]) < get<2>(queries[j]); });
    atcoder::fenwick_tree<int> bit(n);
    vector<long long> ans(queries.size());
    int p = 0;
    for (int j : ord) {
        auto [l, r, x] = queries[j];
        while (p < n && a[idx[p]] <= x) bit.add(idx[p++], 1);  // ★ x 以下の値の位置に 1 を立てる
        ans[j] = bit.sum(l, r);                                  // ★ 区間の中に立っている 1 の数
    }
    return ans;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 辺 0-1 (重み 5)、1-2 (重み 3)、2-3 (重み 8)
    vector<tuple<int, int, long long>> es = {{0, 1, 5}, {1, 2, 3}, {2, 3, 8}};
    auto r = offline_connectivity<long long>(4, es, {{0, 2, 4}, {0, 2, 5}, {0, 3, 7}, {3, 3, 0}});
    assert((r == vector<pair<bool, long long>>{{false, 1}, {true, 3}, {false, 3}, {true, 1}}));

    vector<int> a = {5, 1, 4, 2, 3};
    auto c = offline_count_le<int>(a, {{0, 5, 3}, {1, 4, 1}, {2, 3, 10}, {0, 0, 5}});
    assert((c == vector<long long>{3, 1, 1, 0}));  // {1,2,3} / {1} / {4} / 空

    // ランダムテスト: 愚直と比較
    mt19937 rr(134);
    for (int it = 0; it < 2000; it++) {
        int n = rr() % 8 + 1, m = rr() % 12, q = rr() % 10;
        vector<tuple<int, int, long long>> E;
        for (int i = 0; i < m; i++) E.push_back({(int)(rr() % n), (int)(rr() % n), (long long)(rr() % 10)});
        vector<tuple<int, int, long long>> Q;
        for (int i = 0; i < q; i++) Q.push_back({(int)(rr() % n), (int)(rr() % n), (long long)(rr() % 12) - 1});
        auto got = offline_connectivity<long long>(n, E, Q);
        for (int j = 0; j < q; j++) {
            auto [qa, qb, x] = Q[j];
            atcoder::dsu d(n);
            for (auto [u, v, w] : E) if (w <= x) d.merge(u, v);
            assert(got[j].first == d.same(qa, qb) && got[j].second == d.size(qa));
        }
        vector<long long> b(n);
        for (auto& y : b) y = (long long)(rr() % 10) - 3;
        vector<tuple<int, int, long long>> Q2;
        for (int i = 0; i < q; i++) {
            int l = rr() % (n + 1), rr2 = rr() % (n + 1);
            if (l > rr2) swap(l, rr2);
            Q2.push_back({l, rr2, (long long)(rr() % 12) - 4});
        }
        auto got2 = offline_count_le<long long>(b, Q2);
        for (int j = 0; j < q; j++) {
            auto [l, r2, x] = Q2[j];
            int cnt = 0;
            for (int i = l; i < r2; i++) cnt += b[i] <= x;
            assert(got2[j] == cnt);
        }
    }
    puts("OK");
}
