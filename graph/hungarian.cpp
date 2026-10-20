// =====================================================================
// graph/hungarian.cpp  割当問題（ハンガリアン法）
//   N 人に M 個の仕事（N <= M）を 1 人 1 つずつ、同じ仕事は 1 人まで割り当て、コストの合計を最小にする。O(N^2 M)
//   最小費用流（ACL の mcf_graph）でも解けるが、コスト行列が密（全員が全部の仕事をできる）ならこちらが速くて簡単
//   ★ 各人・各仕事にポテンシャル u[i], v[j] を持ち、「u[i] + v[j] <= cost[i][j]」を保ちながら、
//      等号の成り立つ辺だけで人を 1 人ずつ増やして割り当てる（足りなければポテンシャルを動かして等号の辺を増やす）
// 依存: なし
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// a : N × M のコスト行列（N <= M）。a[i][j] = 人 i に仕事 j を割り当てるコスト（負も OK）
// auto [cost, match] = hungarian(a);   cost = 最小の合計、match[i] = 人 i の仕事
// 最大化したいときは a を −1 倍して、答えも −1 倍する
// ◆ hungarian(a) : 割当問題（N 人に 1 つずつ違う仕事を割り当てて、コストの合計を最小にする）。O(N^2 M)
//   a : N × M のコスト行列（N <= M。人の方が多いなら転置して渡す）。負のコストもよい
//   返り値 : {最小の合計, match}。match[i] = 人 i の仕事の番号
//   注意 : 最大化したいなら a を −1 倍して、合計も −1 倍する
template <class T>
pair<T, vector<long long>> hungarian(const vector<vector<T>>& a) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/hungarian.cpp
    int n = a.size(), m = n ? a[0].size() : 0;
    assert(n <= m);
    const T INF = numeric_limits<T>::max() / 4;
    vector<T> u(n + 1, 0), v(m + 1, 0);  // ポテンシャル（1 から始まる番号。0 は番兵）
    vector<int> p(m + 1, 0), way(m + 1, 0);  // p[j] = 仕事 j の担当（0 ならなし）
    for (int i = 1; i <= n; i++) {  // ★ 人 i を加える
        p[0] = i;
        int j0 = 0;
        vector<T> minv(m + 1, INF);
        vector<bool> used(m + 1, false);
        do {  // ★ 担当のいない仕事が見つかるまで、ダイクストラのように広げる
            used[j0] = true;
            int i0 = p[j0], j1 = 0;
            T delta = INF;
            for (int j = 1; j <= m; j++) {
                if (used[j]) continue;
                T cur = a[i0 - 1][j - 1] - u[i0] - v[j];  // 余裕（0 なら等号の辺）
                if (cur < minv[j]) minv[j] = cur, way[j] = j0;
                if (minv[j] < delta) delta = minv[j], j1 = j;
            }
            for (int j = 0; j <= m; j++) {  // ★ ポテンシャルを delta だけ動かして、新しい等号の辺を作る
                if (used[j]) u[p[j]] += delta, v[j] -= delta;
                else minv[j] -= delta;
            }
            j0 = j1;
        } while (p[j0] != 0);
        do {  // ★ 見つけた道に沿って担当を入れ替える
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
    }
    vector<long long> match(n);
    for (int j = 1; j <= m; j++) if (p[j]) match[p[j] - 1] = j - 1;
    T cost = 0;
    for (int i = 0; i < n; i++) cost += a[i][match[i]];
    return {cost, match};
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 3 人 × 3 仕事: 4 1 3 / 2 0 5 / 3 2 2 → 人 0 に仕事 1 (1)、人 1 に仕事 0 (2)、人 2 に仕事 2 (2) = 5
    {
        auto [c, mt] = hungarian<long long>({{4, 1, 3}, {2, 0, 5}, {3, 2, 2}});
        assert(c == 5 && (mt == vector<long long>{1, 0, 2}));
        // 2 人 × 3 仕事（余る仕事がある）: 1 9 9 / 9 9 1 → 1 + 1 = 2
        auto [c2, m2] = hungarian<long long>({{1, 9, 9}, {9, 9, 1}});
        assert(c2 == 2 && (m2 == vector<long long>{0, 2}));
    }
    // ランダムテスト: 仕事の選び方（順列）を全部試す
    {
        mt19937 rr(194);
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 6 + 1, m = n + rr() % 3;
            vector<vector<long long>> a(n, vector<long long>(m));
            for (auto& row : a) for (auto& x : row) x = (long long)(rr() % 41) - 20;
            auto [c, mt] = hungarian(a);
            // 割当が正しい形か（同じ仕事が 2 回出てこない）、合計が c か
            set<int> js(mt.begin(), mt.end());
            assert((int)js.size() == n);
            long long s = 0;
            for (int i = 0; i < n; i++) s += a[i][mt[i]];
            assert(s == c);
            vector<int> p(m);
            iota(p.begin(), p.end(), 0);
            long long best = LLONG_MAX;
            do {  // 先頭 n 個を割当とみなす（同じ割当が何回も出るが、最小値は変わらない）
                long long t = 0;
                for (int i = 0; i < n; i++) t += a[i][p[i]];
                best = min(best, t);
            } while (next_permutation(p.begin(), p.end()));
            assert(c == best);
        }
    }
    puts("OK");
}
