// =====================================================================
// util/template.cpp  提出用テンプレート（小物ユーティリティ）
//   提出コードの土台。新しい問題を解くときはこのコピー範囲を丸ごと貼り、solve() の中に書く
//   ライブラリは「ここにライブラリを貼る」の位置に貼る
//   入っているもの: ライブラリが使うヘッダ全部（ACL を含む）と mint、高速入出力、ll / INF、
//                   chmin / chmax、グリッドの方向と範囲判定、配列の出力、Yes / No、
//                   デバッグ出力（手元だけで表示、提出時は自動で消える）
// 依存: ACL（AtCoder ではそのまま使える。手元では -I で ACL の場所を指定する）
// 貼る場所: 提出コードの一番上から（このファイルだけは main を含めて丸ごと使う）
// =====================================================================

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
// Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
// ---------- ヘッダ（ライブラリが使うものを全部） ----------
// ライブラリに新しく別のヘッダを使うものを追加したら、ここにも追加する
#include <iostream>
#include <vector>
#include <algorithm>
#include <atcoder/all>
#include <string>
#include <map>
#include <cmath>
#include <climits>
#include <list>
#include <set>
#include <tuple>
#include <ranges>
// ↓ ライブラリが使うので追加
#include <queue>          // queue, priority_queue（bfs, dijkstra, topological_sort など）
#include <deque>          // deque（01-BFS）
#include <stack>
#include <numeric>        // iota, accumulate, gcd, lcm
#include <functional>     // greater, function
#include <utility>        // pair, swap
#include <array>
#include <bitset>
#include <unordered_map>
#include <unordered_set>
#include <limits>         // numeric_limits（dijkstra, shortest_path）
#include <cstdint>        // uint64_t（rolling_hash）
#include <random>         // mt19937_64（rolling_hash）
#include <chrono>         // 乱数の種（rolling_hash）
#include <iomanip>        // setprecision
#include <sstream>
#include <cassert>
#include <optional>       // optional（wavelet_matrix の prev_value / next_value）
using namespace std;
using mint = atcoder::modint998244353;  // 問題の mod に合わせる（1e9+7 なら atcoder::modint1000000007）
using ll = long long;
const ll INF = 1LL << 60;  // ≈ 1.15e18。2 つ足してもあふれない
const int IINF = 1 << 30;  // ≈ 1.07e9

// ---------- chmin / chmax ----------
// a より b が小さければ（大きければ）a を b に更新して true を返す
// 例: chmin(dp[j], dp[i] + c);   if (chmax(best, x)) arg = i;
template <class T, class U> bool chmin(T& a, const U& b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    if (b < a) { a = b; return true; }
    return false;
}
template <class T, class U> bool chmax(T& a, const U& b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    if (a < b) { a = b; return true; }
    return false;
}

// ---------- グリッド ----------
// 4 方向は d = 0..3、8 方向は d = 0..7（最初の 4 つが上下左右）
// 例: for (int d = 0; d < 4; d++) { int ni = i + DI[d], nj = j + DJ[d]; if (!in_grid(ni, nj, H, W)) continue; ... }
const int DI[8] = {1, 0, -1, 0, 1, 1, -1, -1};
const int DJ[8] = {0, 1, 0, -1, 1, -1, 1, -1};
bool in_grid(int i, int j, int h, int w) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    return 0 <= i && i < h && 0 <= j && j < w;
}

// ---------- 出力 ----------
// print_vec(v)        : 空白区切りで 1 行（sep="\n" で 1 行ずつ）
// yes_no(cond)        : "Yes" / "No"（AtCoder の大文字小文字に合わせて書き換える）
template <class T> void print_vec(const vector<T>& v, const string& sep = " ") {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    for (size_t i = 0; i < v.size(); i++) cout << (i ? sep : "") << v[i];
    cout << '\n';
}
void yes_no(bool cond) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    cout << (cond ? "Yes" : "No") << '\n';
}

// ---------- デバッグ出力 ----------
// debug(x, v, ...) で変数名と値を標準エラーに出す。vector / pair / set / map も表示できる
// 手元で g++ -DLOCAL でコンパイルしたときだけ表示され、AtCoder（LOCAL なし）では何もしない
#ifdef LOCAL
template <class T> ostream& operator<<(ostream& os, const vector<T>& v);
template <class A, class B> ostream& operator<<(ostream& os, const pair<A, B>& p) { return os << '(' << p.first << ", " << p.second << ')'; }
template <class T> ostream& operator<<(ostream& os, const vector<T>& v) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    os << '[';
    for (size_t i = 0; i < v.size(); i++) os << (i ? ", " : "") << v[i];
    return os << ']';
}
template <class T> ostream& operator<<(ostream& os, const set<T>& s) { return os << vector<T>(s.begin(), s.end()); }
template <class K, class V> ostream& operator<<(ostream& os, const map<K, V>& m) { return os << vector<pair<K, V>>(m.begin(), m.end()); }
void debug_out(const char*) { cerr << endl; }
template <class H, class... T> void debug_out(const char* names, const H& h, const T&... t) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    const char* comma = names;
    int depth = 0;
    while (*comma && !(*comma == ',' && depth == 0)) {  // 括弧の中のカンマは区切りとみなさない
        if (*comma == '(' || *comma == '[' || *comma == '{' || *comma == '<') depth++;
        if (*comma == ')' || *comma == ']' || *comma == '}' || *comma == '>') depth--;
        comma++;
    }
    cerr.write(names, comma - names) << " = " << h;
    if (*comma) { cerr << " | "; debug_out(comma + 1 + (comma[1] == ' '), t...); }
    else debug_out(comma, t...);
}
#define debug(...) debug_out(#__VA_ARGS__, __VA_ARGS__)
#else
#define debug(...) (void)0
#endif

// ============ ここにライブラリを貼る ============

void solve() {
    // ここに解答を書く
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);  // ★ 入出力の高速化（scanf / printf と混ぜない）
    cout << fixed << setprecision(15);  // 小数を出すときの桁数
    solve();
}
// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
