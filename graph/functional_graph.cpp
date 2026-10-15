// =====================================================================
// graph/functional_graph.cpp  Functional Graph（各頂点から出る辺がちょうど 1 本のグラフ）とダブリング
//   「頂点 v の次は f[v]」で決まる移動。例: テレポーター、置換を K 回適用、x → (x^2 + 1) mod M
//   本体A: 1 つの始点から K 回移動した先（閉路を見つけて K を割る）  O(N)、K は 1e18 でもよい
//   本体B: ダブリング（全頂点から K 回移動した先、通った値の和）    前計算 O(N log K)、各問い O(log K)
//   本体C: 閉路分解（各頂点が閉路上か、閉路までの距離、どの閉路に入るか） O(N)
//   A・B・C は互いに独立
// 依存: なし
// ABC / ARC での使用例:
//   ABC167 D - Teleporter  https://atcoder.jp/contests/abc167/tasks/abc167_d
//     → K ≤ 10^18 回移動した先（walk_k）
//   ABC241 E - Putting Candies  https://atcoder.jp/contests/abc241/tasks/abc241_e
//     → x mod N を頂点にしたダブリングで和を求める（jump_sum）
//   ABC357 E - Reachability in Functional Graph  https://atcoder.jp/contests/abc357/tasks/abc357_e
//     → 閉路の長さ + 閉路までの距離（FunctionalGraph）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 1 つの始点から K 回移動した先 ----------
// f[v] : v の次の頂点（0-indexed）
// 返り値: s から K 回移動した頂点
// ◆ walk_k(f, s, K) : 「v の次は f[v]」で s から K 回進んだ頂点。O(N)
//   f : 次の頂点の表（0 始まり。全部の頂点に次がちょうど 1 つあること）
//   s : 出発する頂点
//   K : 進む回数（0 以上。1e18 でもよい）
int walk_k(const vector<int>& f, int s, long long K) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/functional_graph.cpp
    vector<long long> first(f.size(), -1);  // first[v] = 初めて v に来た回数
    int v = s;
    for (long long t = 0;; t++) {
        if (t == K) return v;
        if (first[v] != -1) {  // ★ 2 回目に来た = 閉路に入った。閉路の長さは t - first[v]
            long long len = t - first[v];
            long long rest = (K - t) % len;  // ★ 残りの移動は閉路の長さで割った余りだけでよい
            while (rest--) v = f[v];
            return v;
        }
        first[v] = t;
        v = f[v];
    }
}

// ---------- 本体B: ダブリング ----------
// Doubling db(f, maxK);                 maxK = 移動回数の最大（1e18 まで）
//   db.jump(v, k)                        v から k 回移動した頂点（k <= maxK）
// 重み付き: Doubling db(f, maxK, w);     w[v] = v を出発するときにもらえる値
//   ※ 表は 2^j ≤ maxK の段まで作るので、maxK × (w の最大) が long long（約 9e18）に収まること
//     （例: ABC241 E は K ≤ 1e12、A ≤ 1e6 なので 1e18 で収まる。w を使わないなら制限なし）
//   db.jump_sum(v, k)                    {k 回移動した先, 通った k 頂点（出発点を含み到着点を含まない）の w の和}
// ◆ struct Doubling : ダブリング（2^j 回先を前計算して、何回先でも O(log K) で求める）
//   Doubling(f, maxK, w) : 前計算 O(N log maxK)
//   jump(v, k)           : v から k 回進んだ頂点
//   jump_sum(v, k)       : {k 回進んだ頂点, 通った k 個の頂点（出発点を含み、到着点を含まない）の w の和}
struct Doubling {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/functional_graph.cpp
    int n, LOG = 1;
    vector<vector<int>> nxt;        // nxt[j][v] = v から 2^j 回移動した頂点
    vector<vector<long long>> sum;  // sum[j][v] = その間にもらえる値の和
    // f    : 次の頂点の表（0 始まり）
    // maxK : 進む回数の最大（これを超える k で jump を呼ばないこと）
    // w    : 各頂点の値（省略可。jump_sum を使うときだけ必要）。maxK × w の最大 が long long に収まること
    Doubling(const vector<int>& f, long long maxK, const vector<long long>& w = {}) : n(f.size()) {
        while ((1LL << LOG) <= maxK) LOG++;
        nxt.assign(LOG, f);
        sum.assign(LOG, w.empty() ? vector<long long>(n, 0) : w);
        for (int j = 0; j + 1 < LOG; j++)
            for (int v = 0; v < n; v++) {
                nxt[j + 1][v] = nxt[j][nxt[j][v]];               // ★ 2^j 回の後にさらに 2^j 回
                sum[j + 1][v] = sum[j][v] + sum[j][nxt[j][v]];  // ★ 値は前半と後半の和
            }
    }
    // jump_sum(v, k) : v : 出発点、k : 進む回数（0 <= k <= maxK）
    pair<int, long long> jump_sum(int v, long long k) const {
        long long s = 0;
        for (int j = 0; j < LOG; j++)
            if (k >> j & 1) s += sum[j][v], v = nxt[j][v];  // ★ k を 2 進数に分解して飛ぶ
        return {v, s};
    }
    // jump(v, k) : v : 出発点、k : 進む回数（0 <= k <= maxK）
    int jump(int v, long long k) const { return jump_sum(v, k).first; }
};

// ---------- 本体C: 閉路分解 ----------
// FunctionalGraph fg(f);
//   fg.cycle_id[v]   v が閉路上ならその閉路の番号、閉路上でなければ -1
//   fg.cycles[c]     閉路 c の頂点列（f でたどる順）
//   fg.dist[v]       v から閉路に入るまでの移動回数（閉路上なら 0）
//   fg.entry[v]      v から進んで最初に着く閉路上の頂点（閉路上なら v 自身）
// 例: v が入る閉路の長さ = fg.cycles[fg.cycle_id[fg.entry[v]]].size()
// ◆ struct FunctionalGraph : 「v の次は f[v]」のグラフを閉路と、閉路へ流れ込む木に分解する。O(N)
//   FunctionalGraph(f) : f は次の頂点の表（0 始まり）
//   cycle_id[v] : v が閉路上ならその閉路の番号、そうでなければ −1
//   cycles[c]   : 閉路 c の頂点の並び（f でたどる順）
//   dist[v]     : v から閉路に入るまでの回数（閉路上なら 0）
//   entry[v]    : v から進んで最初に着く閉路上の頂点
struct FunctionalGraph {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/functional_graph.cpp
    vector<int> cycle_id, dist, entry;
    vector<vector<int>> cycles;
    // f : 次の頂点の表（0 始まり。f[v] は 0..N−1）
    FunctionalGraph(const vector<int>& f) {
        int n = f.size();
        cycle_id.assign(n, -1), dist.assign(n, -1), entry.assign(n, -1);
        vector<int> state(n, 0), pos(n, -1);  // state: 0 未訪問、1 今たどっている途中、2 確定
        for (int s = 0; s < n; s++) {
            if (state[s]) continue;
            vector<int> path;
            int v = s;
            while (state[v] == 0) state[v] = 1, pos[v] = path.size(), path.push_back(v), v = f[v];  // ★ 訪問済みに当たるまで進む
            if (state[v] == 1) {  // ★ 今回の道の途中に戻った = 新しい閉路
                int c = cycles.size();
                cycles.push_back(vector<int>(path.begin() + pos[v], path.end()));
                for (int u : cycles[c]) cycle_id[u] = c, dist[u] = 0, entry[u] = u;
            }
            for (int i = (int)path.size() - 1; i >= 0; i--) {  // ★ 閉路に入る前の頂点は、次の頂点の情報から決まる
                int u = path[i];
                if (dist[u] == -1) dist[u] = dist[f[u]] + 1, entry[u] = entry[f[u]];
                state[u] = 2;
            }
        }
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 0 → 1 → 2 → 3 → 1（閉路 1-2-3）、4 → 0、5 → 5（自己ループ）
    vector<int> f = {1, 2, 3, 1, 0, 5};
    assert(walk_k(f, 4, 0) == 4 && walk_k(f, 4, 1) == 0 && walk_k(f, 4, 5) == 1 && walk_k(f, 4, (long long)1e18) == walk_k(f, 4, 2 + (1000000000000000000LL - 2) % 3));

    Doubling db(f, (long long)1e12, {10, 20, 30, 40, 50, 60});  // 1e12 × 60 は long long に収まる
    assert(db.jump(4, 5) == 1 && db.jump(0, 3) == 3);  // 4→0→1→2→3→1、0→1→2→3
    assert(db.jump_sum(4, 3) == make_pair(2, 50LL + 10 + 20));

    FunctionalGraph fg(f);
    assert(fg.cycles.size() == 2);
    assert(fg.cycle_id[0] == -1 && fg.cycle_id[1] != -1 && fg.cycle_id[1] == fg.cycle_id[3] && fg.cycle_id[5] != fg.cycle_id[1]);
    assert(fg.dist[4] == 2 && fg.dist[0] == 1 && fg.dist[2] == 0 && fg.entry[4] == 1);
    assert(fg.cycles[fg.cycle_id[fg.entry[4]]].size() == 3);

    // ランダムテスト: 愚直なシミュレーションと比較
    mt19937_64 rng(11);
    for (int it = 0; it < 2000; it++) {
        int n = rng() % 10 + 1;
        vector<int> g(n);
        vector<long long> w(n);
        for (int v = 0; v < n; v++) g[v] = rng() % n, w[v] = rng() % 100;
        Doubling d(g, 200, w);
        FunctionalGraph F(g);
        for (int s = 0; s < n; s++) {
            int v = s;
            long long sm = 0;
            for (int k = 0; k <= 200; k++) {
                assert(walk_k(g, s, k) == v && d.jump_sum(s, k) == make_pair(v, sm));
                sm += w[v], v = g[v];
            }
            // 閉路上 ⇔ n 回以内に自分に戻る
            bool on = false;
            for (int k = 1, x = g[s]; k <= n; k++, x = g[x]) on |= x == s;
            assert((F.cycle_id[s] != -1) == on);
            int x = s;
            for (int k = 0; k < F.dist[s]; k++) assert(F.cycle_id[x] == -1), x = g[x];
            assert(x == F.entry[s] && F.cycle_id[x] != -1);
        }
    }
    puts("OK");
}
