// Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
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
// debug(x, v, ...) で変数名と値を標準エラーに出す。pair / tuple / vector / deque / set / multiset / map とその入れ子も表示できる
// queue は先頭から、stack は top から、priority_queue は top から（取り出す順に）表示する。中身は変わらない
// mint は debug(x.val()) と書く
#ifdef LOCAL
template <class A, class B> ostream& operator<<(ostream& os, const pair<A, B>& p);
template <class... Ts> ostream& operator<<(ostream& os, const tuple<Ts...>& t);
template <class T> ostream& operator<<(ostream& os, const vector<T>& v);
template <class T> ostream& operator<<(ostream& os, const set<T>& s);
template <class T> ostream& operator<<(ostream& os, const multiset<T>& s);
template <class K, class V> ostream& operator<<(ostream& os, const map<K, V>& m);
template <class T> ostream& operator<<(ostream& os, const deque<T>& d);
template <class T, class C> ostream& operator<<(ostream& os, queue<T, C> q);
template <class T, class C> ostream& operator<<(ostream& os, stack<T, C> st);
template <class T, class C, class Cmp> ostream& operator<<(ostream& os, priority_queue<T, C, Cmp> pq);
template <class It> ostream& print_range(ostream& os, It first, It last) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    os << '[';
    for (It it = first; it != last; ++it) os << (it == first ? "" : ", ") << *it;
    return os << ']';
}
template <class A, class B> ostream& operator<<(ostream& os, const pair<A, B>& p) { return os << '(' << p.first << ", " << p.second << ')'; }
template <class... Ts> ostream& operator<<(ostream& os, const tuple<Ts...>& t) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    os << '(';
    apply([&](const auto&... xs) { int i = 0; ((os << (i++ ? ", " : "") << xs), ...); }, t);
    return os << ')';
}
template <class T> ostream& operator<<(ostream& os, const vector<T>& v) { return print_range(os, v.begin(), v.end()); }
template <class T> ostream& operator<<(ostream& os, const set<T>& s) { return print_range(os, s.begin(), s.end()); }
template <class T> ostream& operator<<(ostream& os, const multiset<T>& s) { return print_range(os, s.begin(), s.end()); }
template <class K, class V> ostream& operator<<(ostream& os, const map<K, V>& m) { return print_range(os, m.begin(), m.end()); }
template <class T> ostream& operator<<(ostream& os, const deque<T>& d) { return print_range(os, d.begin(), d.end()); }
// queue / stack / priority_queue は中を順に見る方法がないので、コピーを取り出しながら並べる（元の中身は変わらない）
template <class T, class C> ostream& operator<<(ostream& os, queue<T, C> q) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    vector<T> v;
    while (!q.empty()) v.push_back(q.front()), q.pop();
    return print_range(os, v.begin(), v.end());
}
template <class T, class C> ostream& operator<<(ostream& os, stack<T, C> st) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    vector<T> v;
    while (!st.empty()) v.push_back(st.top()), st.pop();
    return print_range(os, v.begin(), v.end());
}
template <class T, class C, class Cmp> ostream& operator<<(ostream& os, priority_queue<T, C, Cmp> pq) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    vector<T> v;
    while (!pq.empty()) v.push_back(pq.top()), pq.pop();
    return print_range(os, v.begin(), v.end());
}
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
    
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);
    cout << fixed << setprecision(15);
    solve();
}
