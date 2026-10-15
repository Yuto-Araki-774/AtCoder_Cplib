// =====================================================================
// dp/expected_value.cpp  期待値 DP・確率 DP（よく出る形の部品 + 例題集）
//   期待値 DP は「状態」と「遷移」が問題ごとに全然違うので、関数にできるのは計算の枠だけ。
//   ここでは状態に 0..n-1 の番号をつけてもらい、遷移をラムダで受け取る形にした。例題（main）の書き方を真似して使う
//   本体A（期待値・後ろから）: ゴールまでの回数などの期待値。遷移は「番号が同じか大きい状態へ」だけ（自己ループ OK）。O(遷移の数)
//   本体B（確率・前から）    : スタートから各状態にたどり着く確率。遷移の条件は A と同じ
//   削除可（閉路あり）       : 遷移が自由（行ったり戻ったり）なとき。連立方程式をガウスの消去法で解く。O(n^3)
//   T は double（小数で出力）か mint（mod で出力。確率 p/q は mint(p) / q で作る）
// 依存: なし
// ABC / ARC での使用例:
//   ABC280 E - Critical Hit  https://atcoder.jp/contests/abc280/tasks/abc280_e
//     → 状態 = 与えたダメージ i（N 以上は N にまとめてゴール）。i → min(i+2, N) が P/100、i → i+1 が 1 − P/100 で ev_backward<mint>
//   ABC263 E - Sugoroku 3  https://atcoder.jp/contests/abc263/tasks/abc263_e
//     → 出目 0 = 自己ループ。遷移が合計 O(N^2) 本あるので ev_backward は使わず、同じ式 E[i] = (1 + Σ E[j] / (A_i+1)) / (1 − 1/(A_i+1)) を、Σ E[j] を後ろからの累積和で持って手で書く
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/modint>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 期待値（後ろから） ----------
// 【自分で定義するもの】
//   adj(i, emit) : 状態 i からの遷移を emit(行き先 j, 確率 p) で全部呼ぶ（j >= i。j == i は「何も変わらない」）
//                  遷移が 1 つもない状態はゴール（期待値 0）
//   cost(i)      : 状態 i から 1 回動くときにかかる量（回数の期待値なら常に 1）
// auto E = ev_backward<T>(n, adj, cost);   E[i] = 状態 i からゴールまでにかかる量の期待値
// ★ E[i] = cost(i) + Σ p × E[j]。j == i（自己ループ、確率 q）があると E[i] が両辺に出るので
//    E[i] = (cost(i) + Σ_{j > i} p × E[j]) / (1 − q) と移項してから計算する
// ◆ ev_backward<T>(n, adj, cost) : 各状態からゴールまでの量の期待値（後ろから）。O(遷移の数)
//   T    : double か mint（必ず書く）
//   n    : 状態の数（状態は 0..n−1）
//   adj  : void adj(int i, auto emit)。状態 i からの遷移ごとに emit(行き先 j, 確率 p) を呼ぶ
//          j >= i であること（j == i は「何も変わらない」= 自己ループ）。遷移がない状態はゴール
//   cost : cost(i)。状態 i から 1 回動くときにかかる量（回数なら常に 1）
//   注意 : 確率の合計は 1 にしておく。自己ループの確率が 1 だと 0 で割る
template <class T, class F, class C>
vector<T> ev_backward(int n, F adj, C cost) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/expected_value.cpp
    vector<T> E(n, T(0));
    for (int i = n - 1; i >= 0; i--) {  // ★ 後ろ（ゴールに近い方）から
        T stay = 0, rest = 0;
        bool any = false;
        adj(i, [&](int j, T p) {
            assert(j >= i);
            any = true;
            if (j == i) stay += p;
            else rest += p * E[j];
        });
        if (!any) continue;  // ゴール
        E[i] = (cost(i) + rest) / (T(1) - stay);  // ★ 自己ループの移項
    }
    return E;
}

// ---------- 本体B: 確率（前から） ----------
// adj は本体A と同じ。P[i] = start から始めて、状態 i にたどり着く確率（ゴールなら「そこで終わる確率」）
// ◆ prob_forward<T>(n, start, adj) : start から各状態にたどり着く確率（前から）
//   n, adj : ev_backward と同じ（遷移は番号が同じか大きい状態へ）
//   start  : 出発する状態
template <class T, class F>
vector<T> prob_forward(int n, int start, F adj) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/expected_value.cpp
    vector<T> P(n, T(0));
    P[start] = 1;
    for (int i = start; i < n; i++) {  // ★ 前（スタートに近い方）から
        if (P[i] == T(0)) continue;
        T stay = 0;
        vector<pair<int, T>> out;
        adj(i, [&](int j, T p) {
            assert(j >= i);
            if (j == i) stay += p;
            else out.push_back({j, p});
        });
        T q = P[i] / (T(1) - stay);  // ★ 自己ループで何回とどまっても、いずれ出ていく → 出ていく確率を 1 / (1 − stay) 倍
        for (auto [j, p] : out) P[j] += q * p;
    }
    return P;
}

// ---------- 削除可: 期待値（閉路あり。ガウスの消去法） O(n^3) ----------
// adj・cost は本体A と同じだが、行き先 j に制限なし（戻ってもよい）。ゴールにいずれ必ずたどり着くこと
// ◆ ev_general<T>(n, adj, cost) : 遷移が自由（行ったり戻ったり）でも期待値を求める（連立方程式）。O(n^3)
//   n, adj, cost : ev_backward と同じだが、行き先 j に制限なし
//   注意 : どの状態からもいつか必ずゴールに着くこと（着けない状態があると assert で止まる）。n <= 500 程度
template <class T, class F, class C>
vector<T> ev_general(int n, F adj, C cost) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/expected_value.cpp
    vector<vector<T>> A(n, vector<T>(n + 1, T(0)));  // ★ 式: E[i] − Σ p E[j] = cost(i)（ゴールは E[i] = 0）
    for (int i = 0; i < n; i++) {
        A[i][i] = 1;
        bool any = false;
        adj(i, [&](int j, T p) { any = true, A[i][j] -= p; });
        if (any) A[i][n] = cost(i);
    }
    for (int c = 0; c < n; c++) {
        int piv = c;
        for (int r = c; r < n; r++) {
            if constexpr (is_floating_point_v<T>) { if (abs(A[r][c]) > abs(A[piv][c])) piv = r; }  // 小数は一番大きいものを選ぶ（誤差対策）
            else if (A[piv][c] == T(0) && A[r][c] != T(0)) piv = r;
        }
        swap(A[c], A[piv]);
        assert(!(A[c][c] == T(0)));  // 0 なら「ゴールにたどり着けない状態がある」
        T inv = T(1) / A[c][c];
        for (int j = c; j <= n; j++) A[c][j] *= inv;
        for (int r = 0; r < n; r++) {
            if (r == c || A[r][c] == T(0)) continue;
            T f = A[r][c];
            for (int j = c; j <= n; j++) A[r][j] -= f * A[c][j];
        }
    }
    vector<T> E(n);
    for (int i = 0; i < n; i++) E[i] = A[i][n];
    return E;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
using mint = atcoder::modint998244353;
bool near(double a, double b) { return fabs(a - b) < 1e-9; }
int main() {
    auto one = [](int) { return 1; };
    // 例題 1（すごろく型・後ろから）: マス 0 にいて、毎回 1/2 で 1 マス、1/2 で 2 マス進む。N 以上でゴール
    //   状態 = 今のマス（N 以上はまとめて N）。N = 2: E[1] = 1、E[0] = 1 + (1/2) E[1] + (1/2) E[2] = 1.5
    {
        int N = 2;
        auto adj = [&](int i, auto emit) {
            if (i >= N) return;  // ゴール
            emit(min(i + 1, N), 0.5), emit(min(i + 2, N), 0.5);
        };
        auto E = ev_backward<double>(N + 1, adj, one);
        assert(near(E[0], 1.5) && near(E[1], 1.0) && near(E[2], 0.0));
        // mod で出力するなら mint に変えるだけ（確率 1/2 は mint(1) / 2）
        auto adjm = [&](int i, auto emit) {
            if (i >= N) return;
            emit(min(i + 1, N), mint(1) / 2), emit(min(i + 2, N), mint(1) / 2);
        };
        auto Em = ev_backward<mint>(N + 1, adjm, one);
        assert(Em[0] == mint(3) / 2);
    }
    // 例題 2（自己ループ・コンプガチャ）: N 種類のカードが等確率で出る。全種類集めるまでの回数の期待値
    //   状態 = 集めた種類数 k。確率 k/N で「持っているカード」（自己ループ）、(N−k)/N で k+1 へ
    //   N = 3: 3 × (1 + 1/2 + 1/3) = 5.5
    {
        int N = 3;
        auto adj = [&](int k, auto emit) {
            if (k == N) return;
            emit(k, (double)k / N), emit(k + 1, (double)(N - k) / N);
        };
        auto E = ev_backward<double>(N + 1, adj, one);
        assert(near(E[0], 5.5));
        // 例題 2'（期待値の線形性で DP なし）: k 種類 → k+1 種類に 1 回で進む確率は (N−k)/N
        //   → 平均 N/(N−k) 回かかる（幾何分布）。全体の期待値 = その和（techniques/patterns.md の「期待値の線形性」）
        double s = 0;
        for (int k = 0; k < N; k++) s += (double)N / (N - k);
        assert(near(s, E[0]));
    }
    // 例題 3（確率・前から）: 0 から 1/3 で 1、2/3 で 2 へ。1 からは 1/2 で 2、1/2 で 3 へ。2, 3 はゴール
    //   2 で終わる確率 = 2/3 + 1/3 × 1/2 = 5/6、3 で終わる確率 = 1/6
    {
        auto adj = [&](int i, auto emit) {
            if (i == 0) emit(1, mint(1) / 3), emit(2, mint(2) / 3);
            if (i == 1) emit(2, mint(1) / 2), emit(3, mint(1) / 2);
        };
        auto P = prob_forward<mint>(4, 0, adj);
        assert(P[2] == mint(5) / 6 && P[3] == mint(1) / 6 && P[1] == mint(1) / 3);
    }
    // 例題 4（閉路あり）: 数直線 0..3。0 からは必ず 1 へ、1・2 からは 1/2 で左右へ。3 がゴール
    //   0 から 3 にたどり着くまでの回数の期待値 = 3^2 = 9（はね返りのある酔歩。0 から N までは N^2）
    {
        int N = 3;
        auto adj = [&](int i, auto emit) {
            if (i == N) return;
            if (i == 0) emit(1, 1.0);
            else emit(i - 1, 0.5), emit(i + 1, 0.5);
        };
        auto E = ev_general<double>(N + 1, adj, one);
        assert(near(E[0], 9) && near(E[1], 8) && near(E[2], 5));
    }
    // ランダムテスト（mint）
    {
        mt19937 rr(165);
        for (int it = 0; it < 2000; it++) {
            int n = rr() % 6 + 1;
            // 前向きの遷移（自己ループあり）をランダムに作る。最後の状態は必ずゴール
            vector<vector<pair<int, mint>>> tr(n);
            vector<int> c(n);
            for (int i = 0; i + 1 < n; i++) {
                c[i] = rr() % 5 + 1;
                if (rr() % 4 == 0) continue;  // ゴール
                int k = rr() % 3 + 1;
                vector<int> w(k);
                int tot = 0;
                for (auto& x : w) x = rr() % 4 + 1, tot += x;
                for (int t = 0; t < k; t++) {
                    int j = (t == 0) ? i + 1 + rr() % (n - 1 - i) : i + rr() % (n - i);  // 1 本は必ず先へ（自己ループだけにならない）
                    tr[i].push_back({j, mint(w[t]) / tot});
                }
            }
            auto adj = [&](int i, auto emit) { for (auto [j, p] : tr[i]) emit(j, p); };
            auto cost = [&](int i) { return mint(c[i]); };
            auto E1 = ev_backward<mint>(n, adj, cost), E2 = ev_general<mint>(n, adj, cost);
            assert(E1 == E2);  // 2 つの方法が一致
            // 確率: 自己ループを除いた道をすべてたどり、Π p / (1 − stay) の和を足したものと比較
            vector<mint> stay(n, 0), want(n, 0);
            for (int i = 0; i < n; i++) for (auto [j, p] : tr[i]) if (j == i) stay[i] += p;
            function<void(int, mint)> dfs = [&](int v, mint pr) {
                want[v] += pr;
                for (auto [j, p] : tr[v]) if (j != v) dfs(j, pr * p / (1 - stay[v]));
            };
            int s = rr() % n;
            dfs(s, 1);
            assert(prob_forward<mint>(n, s, adj) == want);
            // 閉路ありの一般の遷移: 解が式 E[i] = cost(i) + Σ p E[j] を満たすか
            vector<vector<pair<int, mint>>> tg(n);
            for (int i = 0; i + 1 < n; i++) {
                if (rr() % 4 == 0) continue;
                int k = rr() % 3 + 1;
                for (int t = 0; t < k; t++) tg[i].push_back({(t == 0) ? n - 1 : (int)(rr() % n), mint(1) / k});  // 1 本はゴールへ → 必ずいつか終わる
            }
            auto adjg = [&](int i, auto emit) { for (auto [j, p] : tg[i]) emit(j, p); };
            auto Eg = ev_general<mint>(n, adjg, cost);
            for (int i = 0; i < n; i++) {
                if (tg[i].empty()) { assert(Eg[i] == 0); continue; }
                mint r = c[i];
                for (auto [j, p] : tg[i]) r += p * Eg[j];
                assert(r == Eg[i]);
            }
        }
    }
    // ランダムテスト（double・閉路あり）: シミュレーションではなく、十分たくさん反復した値と比較
    {
        mt19937 rr(166);
        for (int it = 0; it < 300; it++) {
            int n = rr() % 6 + 2;
            vector<vector<pair<int, double>>> tg(n);
            for (int i = 0; i + 1 < n; i++) {
                int k = rr() % 3 + 1;
                for (int t = 0; t < k; t++) tg[i].push_back({(t == 0) ? n - 1 : (int)(rr() % n), 1.0 / k});
            }
            auto adjg = [&](int i, auto emit) { for (auto [j, p] : tg[i]) emit(j, p); };
            auto E = ev_general<double>(n, adjg, one);
            vector<double> V(n, 0);  // 価値反復: V ← 1 + Σ p V（ゴールに行く確率が 1/3 以上なので速く収束する）
            for (int rep = 0; rep < 200; rep++) {
                vector<double> W(n, 0);
                for (int i = 0; i < n; i++) {
                    if (tg[i].empty()) continue;
                    W[i] = 1;
                    for (auto [j, p] : tg[i]) W[i] += p * V[j];
                }
                V = W;
            }
            for (int i = 0; i < n; i++) assert(fabs(E[i] - V[i]) < 1e-6);
        }
    }
    puts("OK");
}
