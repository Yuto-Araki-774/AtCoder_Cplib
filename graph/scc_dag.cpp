// =====================================================================
// graph/scc_dag.cpp  強連結成分分解（SCC）と縮約 DAG
//   有向グラフで「お互いに行き来できる頂点」をまとめて 1 つの頂点にすると、閉路のないグラフ（DAG）になる
//   → 閉路があるグラフでも、縮約 DAG の上ならトポロジカル順に DP ができる
//   SCC 自体は ACL の scc_graph を使い、成分の番号付け・縮約 DAG の辺（重複なし）を作るところまでをまとめた。O(N + M)
// 依存: ACL の scc_graph（AtCoder ではテンプレートの atcoder/all に含まれる）
// ABC / ARC での使用例:
//   ABC245 F - Endless Walk  https://atcoder.jp/contests/abc245/tasks/abc245_f
//     → 使用例と同じ。後ろの成分から「ぐるぐる回れる成分にたどり着けるか」を伝える
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/scc>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// SCCDag sd(n, edges);          edges = 有向辺 {a, b} の一覧（a → b）
//   sd.k             成分の数
//   sd.comp[v]       頂点 v の成分番号。トポロジカル順（辺 a → b があれば comp[a] <= comp[b]）
//   sd.groups[c]     成分 c の頂点の一覧
//   sd.dag[c]        縮約 DAG で c から出る辺の行き先（重複なし、自分への辺なし）
//   sd.self_loop[c]  成分 c の中に辺があるか（頂点 2 個以上、または自己ループ）= c の中をぐるぐる回れるか
// DP の書き方: 「後ろから」なら for (int c = k - 1; c >= 0; c--) for (int d : dag[c]) ...（d は c より後ろ）
struct SCCDag {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/scc_dag.cpp
    int n, k;
    vector<int> comp;
    vector<vector<int>> groups, dag;
    vector<bool> self_loop;
    SCCDag(int n_, const vector<pair<int, int>>& edges) : n(n_), comp(n_) {
        atcoder::scc_graph g(n);
        for (auto [a, b] : edges) g.add_edge(a, b);
        groups = g.scc();  // ★ ACL はトポロジカル順に成分を返す
        k = groups.size();
        for (int c = 0; c < k; c++)
            for (int v : groups[c]) comp[v] = c;
        dag.assign(k, {}), self_loop.assign(k, false);
        for (auto [a, b] : edges) {
            if (comp[a] != comp[b]) dag[comp[a]].push_back(comp[b]);  // ★ 成分をまたぐ辺だけが DAG の辺になる
            else self_loop[comp[a]] = true;                           // ★ 成分の中の辺（頂点 2 個以上の成分には必ずある。自己ループもここ）
        }
        for (auto& d : dag) sort(d.begin(), d.end()), d.erase(unique(d.begin(), d.end()), d.end());  // 重複をなくす
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 0 → 1 → 2 → 0（閉路）、2 → 3、3 → 4、4 → 3（閉路）、5 → 5（自己ループ）、5 → 0
    vector<pair<int, int>> es = {{0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 3}, {5, 5}, {5, 0}};
    SCCDag sd(6, es);
    assert(sd.k == 3);  // {5}, {0,1,2}, {3,4}
    assert(sd.comp[0] == sd.comp[1] && sd.comp[1] == sd.comp[2] && sd.comp[3] == sd.comp[4]);
    assert(sd.comp[5] < sd.comp[0] && sd.comp[0] < sd.comp[3]);  // トポロジカル順
    assert(sd.self_loop[sd.comp[5]] && sd.self_loop[sd.comp[0]]);
    assert((sd.dag[sd.comp[5]] == vector<int>{sd.comp[0]}));

    // 使用例: 無限に歩き続けられる頂点の数（ぐるぐる回れる成分に、たどり着ける頂点）
    vector<bool> good(sd.k);
    for (int c = sd.k - 1; c >= 0; c--) {  // 後ろの成分から
        good[c] = sd.self_loop[c];
        for (int d : sd.dag[c]) if (good[d]) good[c] = true;
    }
    int cnt = 0;
    for (int v = 0; v < 6; v++) cnt += good[sd.comp[v]];
    assert(cnt == 6);

    // ランダムテスト: 到達可能性（ワーシャルフロイド）と比較
    mt19937 rr(138);
    for (int it = 0; it < 3000; it++) {
        int n = rr() % 8 + 1, m = rr() % 14;
        vector<pair<int, int>> e;
        vector<vector<bool>> R(n, vector<bool>(n, false));  // R[a][b] = a から b に 1 歩以上で行ける
        for (int i = 0; i < m; i++) { int a = rr() % n, b = rr() % n; e.push_back({a, b}), R[a][b] = true; }
        for (int x = 0; x < n; x++) for (int a = 0; a < n; a++) for (int b = 0; b < n; b++) if (R[a][x] && R[x][b]) R[a][b] = true;
        SCCDag s(n, e);
        for (int a = 0; a < n; a++)
            for (int b = 0; b < n; b++) {
                bool same = a == b || (R[a][b] && R[b][a]);
                assert((s.comp[a] == s.comp[b]) == same);
                if (R[a][b] && !same) assert(s.comp[a] < s.comp[b]);
            }
        for (int c = 0; c < s.k; c++) {
            assert(s.self_loop[c] == R[s.groups[c][0]][s.groups[c][0]]);  // 自分に戻れる
            set<int> want;
            for (auto [a, b] : e) if (s.comp[a] == c && s.comp[b] != c) want.insert(s.comp[b]);
            assert(vector<int>(want.begin(), want.end()) == s.dag[c]);
        }
    }
    puts("OK");
}
