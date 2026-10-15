// =====================================================================
// dp/digit_dp.cpp  桁 DP のテンプレート
//   「0 以上 N 以下の整数のうち、条件を満たすものはいくつか」を、N が 10^100000 桁でも数える
//   数を上の桁から 1 桁ずつ読む「状態」を自分で決めれば、あとは数えるだけ。O(桁数 × 状態数 × 10)
// 依存: なし（mod で数えるなら ACL の modint を T に使う）
// ABC / ARC での使用例:
//   ABC029 D - 1  https://atcoder.jp/contests/abc029/tasks/abc029_d
//     → 状態 = ここまでの 1 の個数、答え = Σ s × cnt[s]（使用例と同じ）
//   EDPC S - Digit Sum  https://atcoder.jp/contests/dp/tasks/dp_s
//     → 状態 = 桁和 mod D、答え = cnt[0] − 1（0 を除く）。K は 10000 桁なので T = mint
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/modint>
using namespace std;
using mint = atcoder::modint1000000007;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// T : 個数の型（long long か mint）
// auto cnt = digit_dp<T>(N, S, init, trans);
//   N     : 上限（10 進数の文字列。long long なら to_string(N)）
//   返り値 cnt[s] = 0 以上 N 以下の整数 x のうち、読み終えたときの状態が s になるものの個数
// 【自分で定義するもの】
//   S              : 状態の数（状態は 0..S-1 の整数）
//   init           : 最初の状態
//   trans(s, d)    : 状態 s で次の桁 d（0..9）を読んだときの次の状態。-1 を返すとその数は数えない（ラムダ）
// 注意: x は N と同じ桁数になるよう上に 0 を詰めて読む（N = 345 なら x = 7 は "007"）
//       「先頭の 0」を普通の 0 と区別したいとき（「0 を含まない」など）は、状態に「もう 0 以外の桁が出たか」を入れる
// L 以上 R 以下は cnt(R) - cnt(L - 1)（L = 0 なら cnt(R) だけ）
//
// 例: 各桁の和が D の倍数の個数   S = D, init = 0, trans(s, d) = (s + d) % D, 答え = cnt[0]（0 を含む）
// 例: 3 を含まない数の個数        S = 1, init = 0, trans(s, d) = (d == 3 ? -1 : 0), 答え = cnt[0]
// ◆ digit_dp<T>(N, S, init, trans) : 桁 DP の枠。0 以上 N 以下の整数を、状態ごとに数える。O(桁数 × S × 10)
//   T     : 個数の型（long long か mint。必ず書く）
//   N     : 上限（10 進数の文字列。先頭に余分な 0 がないこと。long long なら to_string(N)）
//   S     : 状態の数（状態は 0..S−1）
//   init  : 最初の状態（0..S−1）
//   trans : int trans(int s, int d)。状態 s で次の桁 d（0..9）を読んだ後の状態。−1 を返すとその数は数えない
//           S 以上を返すと assert で止まる
//   返り値 : cnt[s] = 読み終わったとき状態が s になる数の個数（x は N と同じ桁数になるよう上に 0 を詰めて読む）
template <class T, class F>
vector<T> digit_dp(const string& N, int S, int init, F trans) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/digit_dp.cpp
    vector<T> loose(S, T(0));  // loose[s] = すでに N より小さいと決まった数のうち、状態が s のものの個数
    vector<char> live(S, 0);   // live[s] = 状態 s になる数が実際にあるか（ない状態からは遷移を試さない）
    int tight = init;          // ★ ここまで N とまったく同じ数は 1 つだけなので、その状態だけ持つ（-1 なら条件から外れた）
    for (char ch : N) {
        int nd = ch - '0';
        vector<T> nxt(S, T(0));
        vector<char> nlive(S, 0);
        auto go = [&](int s, int d, T c) {
            int t = trans(s, d);
            assert(t < S);  // trans が S 以上を返した = 状態の数 S の決め方が間違っている
            if (t >= 0) nxt[t] += c, nlive[t] = 1;
        };
        for (int s = 0; s < S; s++)
            if (live[s])
                for (int d = 0; d <= 9; d++) go(s, d, loose[s]);  // ★ すでに小さい数は、次の桁に何を置いてもよい
        if (tight >= 0) {
            for (int d = 0; d < nd; d++) go(tight, d, T(1));  // ★ N と同じ数は、N の桁より小さい d を置くとここで「小さい」側に移る
            tight = trans(tight, nd);                         // ★ N と同じ桁を置けば、まだ N と同じ
            assert(tight < S);
        }
        loose.swap(nxt), live.swap(nlive);
    }
    if (tight >= 0) loose[tight] += T(1);  // N 自身
    return loose;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    // 0..20 で各桁の和が 3 の倍数: 0, 3, 6, 9, 12, 15, 18 の 7 個
    auto c = digit_dp<long long>("20", 3, 0, [](int s, int d) { return (s + d) % 3; });
    assert(c[0] == 7);

    // 0..1000 で 3 を含まない数: 各桁 9 通りで 000..999 が 9^3 = 729 個、1000 も含まないので 730 個
    auto c2 = digit_dp<long long>("1000", 1, 0, [](int, int d) { return d == 3 ? -1 : 0; });
    assert(c2[0] == 730);

    // 1..N の数を全部書いたときの数字 1 の個数: 状態 = ここまでの 1 の個数（0..桁数）、答え = Σ s × cnt[s]
    string N = "13";  // 1, 10, 11(2個), 12, 13 → 1 + 1 + 2 + 1 + 1 = 6
    int L = N.size();
    auto c3 = digit_dp<long long>(N, L + 1, 0, [](int s, int d) { return s + (d == 1); });
    long long ones = 0;
    for (int s = 0; s <= L; s++) ones += s * c3[s];
    assert(ones == 6);

    // mod で数える（1000 桁の N）: 全状態の個数を足すと「0 以上 N 以下の個数」= 10^1000 になるはず
    auto cm = digit_dp<mint>(string(1000, '9'), 9, 0, [](int s, int d) { return (s + d) % 9; });
    mint total = 0;
    for (auto x : cm) total += x;
    assert(total == mint(10).pow(1000));

    // ランダムテスト: 愚直に 0..N を全部調べたものと比較（いろいろな状態の作り方で）
    mt19937 rr(128);
    for (int it = 0; it < 300; it++) {
        long long n = rr() % 3000;
        string s = to_string(n);
        int D = rr() % 7 + 1;
        auto a = digit_dp<long long>(s, D, 0, [&](int st, int d) { return (st + d) % D; });  // 桁和 mod D
        auto b = digit_dp<long long>(s, 2, 0, [&](int st, int d) { return d == 7 ? -1 : (st ^ (d % 2)); });  // 7 を含まない、奇数の桁の個数の偶奇
        // 先頭の 0 を区別: 状態 = {まだ始まっていない: 0, 始まった: 1}、0 を含む数を除く（ただし x = 0 は数える）
        auto z = digit_dp<long long>(s, 2, 0, [&](int st, int d) { if (st == 0) return d == 0 ? 0 : 1; return d == 0 ? -1 : 1; });
        vector<long long> ra(D, 0), rb(2, 0);
        long long rz = 0;
        for (long long x = 0; x <= n; x++) {
            int sum = 0, odd = 0;
            bool has7 = false, has0 = false;
            string t = to_string(x);
            for (char ch : t) { sum += ch - '0', odd ^= (ch - '0') % 2, has7 |= ch == '7'; has0 |= ch == '0'; }
            ra[sum % D]++;
            if (!has7) rb[odd]++;
            if (x == 0 || !has0) rz++;
        }
        assert(a == ra && b == rb && z[0] + z[1] == rz);
    }
    puts("OK");
}
