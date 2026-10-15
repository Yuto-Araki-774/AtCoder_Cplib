// =====================================================================
// graph/euler_path.cpp  オイラー路・オイラー閉路（すべての辺をちょうど 1 回ずつ通る道）
//   「一筆書きできるか」と、その書き順を O(N + M) で求める（Hierholzer 法）。多重辺・自己ループも OK
//   ★ できる条件（辺のある頂点が全部つながっていることが前提）
//     無向: 次数が奇数の頂点が 0 個（閉路）か 2 個（その 2 つが始点と終点）
//     有向: 全頂点で 入次数 = 出次数（閉路）か、出 − 入 = 1 の頂点（始点）と 入 − 出 = 1 の頂点（終点）が 1 つずつ
// 依存: なし
// ABC / ARC での使用例:
//   ABC286 G - Unique Walk  https://atcoder.jp/contests/abc286/tasks/abc286_g
//     → S 以外の辺で連結成分をまとめ（dsu）、成分を頂点・S の辺を辺にした多重グラフで euler_path(...).ok を見る（次数の奇数の個数が 0 か 2 か、でも判定できる）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// edges = 辺 {a, b} の一覧（有向なら a → b）。頂点は 0..n-1
// auto r = euler_path(n, edges, directed);          始点はおまかせ（奇数次数の頂点があればそこ）
// auto r = euler_path(n, edges, directed, s);       始点を s に決める（s から始められないなら ok = false）
//   r.ok    一筆書きできるか
//   r.vs    通る頂点の順番（長さ M + 1）。閉路なら vs.front() == vs.back()
//   r.es    通る辺の番号の順番（長さ M）
// 辺が 0 本なら ok = true、vs = {始点}（s 未指定なら 0）
// ◆ EulerResult : euler_path の結果。ok : 一筆書きできるか、vs : 通る頂点の順（M + 1 個）、es : 通る辺の番号の順（M 個）
struct EulerResult {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/euler_path.cpp
    bool ok;
    vector<int> vs, es;
};
// ◆ euler_path(n, edges, directed, s) : すべての辺をちょうど 1 回ずつ通る道（一筆書き）を求める。O(N + M)
//   n        : 頂点数（辺のない頂点があってもよい）
//   edges    : 辺 {a, b} の一覧（辺の番号 = この配列の添字）。多重辺・自己ループもよい
//   directed : true なら有向（a → b だけ）、false なら無向
//   s        : 始点を決めたいとき（既定 −1 = おまかせ）。その頂点から始められなければ ok = false
//   注意 : 辺のある頂点が全部つながっていないと ok = false
EulerResult euler_path(int n, const vector<pair<int, int>>& edges, bool directed, int s = -1) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/euler_path.cpp
    int m = edges.size();
    vector<vector<pair<int, int>>> g(n);  // g[v] = {{行き先, 辺番号}, ...}
    vector<int> deg(n, 0);                // 無向: 次数、有向: 出次数 − 入次数
    for (int i = 0; i < m; i++) {
        auto [a, b] = edges[i];
        g[a].push_back({b, i});
        if (directed) deg[a]++, deg[b]--;
        else g[b].push_back({a, i}), deg[a]++, deg[b]++;
    }
    if (m == 0) return {true, {s == -1 ? 0 : s}, {}};
    // ★ 始点を決める: 無向なら奇数次数の頂点、有向なら 出 − 入 = 1 の頂点
    vector<int> sp;  // 始点になれる「特別な」頂点
    int bad = 0;
    for (int v = 0; v < n; v++) {
        if (directed) {
            if (deg[v] == 1) sp.push_back(v);
            else if (deg[v] != 0 && deg[v] != -1) bad++;
        } else if (deg[v] % 2 == 1) sp.push_back(v);
    }
    int minus = 0;
    if (directed) for (int v = 0; v < n; v++) minus += deg[v] == -1;
    bool path_type;  // true: 始点と終点が違う道、false: 閉路
    if (directed) {
        if (bad || sp.size() != (size_t)minus || sp.size() > 1) return {false, {}, {}};
        path_type = sp.size() == 1;
    } else {
        if (sp.size() != 0 && sp.size() != 2) return {false, {}, {}};
        path_type = sp.size() == 2;
    }
    int start = -1;
    if (s != -1) {
        if (path_type && find(sp.begin(), sp.end(), s) == sp.end()) return {false, {}, {}};  // 道なら端からしか始められない
        if (!path_type && g[s].empty()) return {false, {}, {}};                                  // 閉路なら辺のある頂点から
        start = s;
    } else if (path_type) start = sp[0];
    else for (int v = 0; v < n && start == -1; v++) if (!g[v].empty()) start = v;
    // ★ Hierholzer 法: 進めるだけ進み、行き止まったら戻りながら記録する（記録は逆順になる）
    vector<int> ptr(n, 0), vs, es;
    vector<bool> used(m, false);
    vector<pair<int, int>> st = {{start, -1}};  // {頂点, そこへ来た辺}
    while (!st.empty()) {
        int v = st.back().first;
        while (ptr[v] < (int)g[v].size() && used[g[v][ptr[v]].second]) ptr[v]++;  // 使った辺を飛ばす
        if (ptr[v] == (int)g[v].size()) {  // ★ v から出る辺がもうない → 確定させて戻る
            vs.push_back(v);
            if (st.back().second != -1) es.push_back(st.back().second);
            st.pop_back();
        } else {
            auto [to, id] = g[v][ptr[v]++];
            used[id] = true;
            st.push_back({to, id});
        }
    }
    if ((int)es.size() != m) return {false, {}, {}};  // ★ 使い切れなかった辺がある = 辺のある頂点がつながっていない
    reverse(vs.begin(), vs.end()), reverse(es.begin(), es.end());
    return {true, vs, es};
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 無向: 0-1, 1-2, 2-0, 2-3 → 奇数次数は 2 と 3 → 2 か 3 から始まる一筆書き
    {
        vector<pair<int, int>> es = {{0, 1}, {1, 2}, {2, 0}, {2, 3}};
        auto r = euler_path(4, es, false);
        assert(r.ok && r.vs.size() == 5 && r.es.size() == 4);
        assert((r.vs.front() == 2 && r.vs.back() == 3) || (r.vs.front() == 3 && r.vs.back() == 2));
        assert(euler_path(4, es, false, 3).ok && euler_path(4, es, false, 3).vs[0] == 3);
        assert(!euler_path(4, es, false, 0).ok);  // 次数が偶数の 0 からは始められない
        // 有向: 0 → 1 → 2 → 0 は閉路
        auto c = euler_path(3, {{0, 1}, {1, 2}, {2, 0}}, true);
        assert(c.ok && c.vs.front() == c.vs.back());
        // 有向: 0 → 1、0 → 2 は無理（0 の出次数が 2）
        assert(!euler_path(3, {{0, 1}, {0, 2}}, true).ok);
        // 2 つに分かれている: 0-1 と 2-3 は無理
        assert(!euler_path(4, {{0, 1}, {2, 3}}, false).ok);
    }
    // ランダムテスト: 辺の順番を全部試す（バックトラック）ものと、できるかどうかを比較し、返した道が正しいか確認
    {
        mt19937 rr(162);
        for (int it = 0; it < 20000; it++) {
            int n = rr() % 5 + 1, m = rr() % 7;
            bool directed = rr() % 2;
            vector<pair<int, int>> es;
            for (int i = 0; i < m; i++) es.push_back({(int)(rr() % n), (int)(rr() % n)});
            int s = (rr() % 3 == 0) ? (int)(rr() % n) : -1;
            vector<bool> used(m, false);
            function<bool(int, int)> bt = [&](int v, int k) {  // 頂点 v にいて、k 本使った
                if (k == m) return true;
                for (int i = 0; i < m; i++) {
                    if (used[i]) continue;
                    auto [a, b] = es[i];
                    int to = -1;
                    if (a == v) to = b;
                    else if (!directed && b == v) to = a;
                    if (to == -1) continue;
                    used[i] = true;
                    bool ok = bt(to, k + 1);
                    used[i] = false;
                    if (ok) return true;
                }
                return false;
            };
            bool want = false;
            if (m == 0) want = true;
            else for (int v = 0; v < n; v++) if ((s == -1 || v == s) && bt(v, 0)) want = true;
            auto r = euler_path(n, es, directed, s);
            assert(r.ok == want);
            if (!r.ok) continue;
            assert((int)r.vs.size() == m + 1 && (int)r.es.size() == m);
            if (s != -1) assert(r.vs[0] == s);
            vector<int> cnt(m, 0);
            for (int i = 0; i < m; i++) {
                auto [a, b] = es[r.es[i]];
                cnt[r.es[i]]++;
                bool fwd = a == r.vs[i] && b == r.vs[i + 1];
                bool bwd = !directed && b == r.vs[i] && a == r.vs[i + 1];
                assert(fwd || bwd);
            }
            for (int c : cnt) assert(c == 1);
        }
    }
    puts("OK");
}
