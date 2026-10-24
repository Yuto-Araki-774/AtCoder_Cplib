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
// debug(x, v, ...) で変数名と値を標準エラーに出す。手元で -DLOCAL を付けたときだけ動き、AtCoder では何もしない
// 表示できる型: 数値・文字列・bitset・mint・pair・tuple・vector・deque・list・array・set・multiset・map・multimap・
//   unordered_set・unordered_map・queue・stack・priority_queue・atcoder::dsu と、それらの入れ子（vector<map<...>> など）
// queue は先頭から、stack と priority_queue は top から（取り出す順に）表示する。中身は変わらない
// unordered_set / unordered_map の並びはバラバラ（中の順のまま）。dsu は連結成分ごとの頂点の一覧
// ※ cout / cerr の << は増やしていない（手元だけ cout << v が通って AtCoder でコンパイルエラー、を防ぐため）
#ifdef LOCAL
// ★ 先に全部宣言しておく（入れ子にしたとき、後で定義した型も表示できるように）
template <class T> void dprint(ostream& os, const T& x);
template <class A, class B> void dprint(ostream& os, const pair<A, B>& p);
template <class... Ts> void dprint(ostream& os, const tuple<Ts...>& t);
template <class T, class A> void dprint(ostream& os, const vector<T, A>& v);
template <class T, class A> void dprint(ostream& os, const deque<T, A>& v);
template <class T, class A> void dprint(ostream& os, const list<T, A>& v);
template <class T, size_t N> void dprint(ostream& os, const array<T, N>& v);
template <class T, class C, class A> void dprint(ostream& os, const set<T, C, A>& v);
template <class T, class C, class A> void dprint(ostream& os, const multiset<T, C, A>& v);
template <class T, class H, class E, class A> void dprint(ostream& os, const unordered_set<T, H, E, A>& v);
template <class T, class H, class E, class A> void dprint(ostream& os, const unordered_multiset<T, H, E, A>& v);
template <class K, class V, class C, class A> void dprint(ostream& os, const map<K, V, C, A>& v);
template <class K, class V, class C, class A> void dprint(ostream& os, const multimap<K, V, C, A>& v);
template <class K, class V, class H, class E, class A> void dprint(ostream& os, const unordered_map<K, V, H, E, A>& v);
template <class T, class C> void dprint(ostream& os, const queue<T, C>& q);
template <class T, class C> void dprint(ostream& os, const stack<T, C>& st);
template <class T, class C, class Cmp> void dprint(ostream& os, const priority_queue<T, C, Cmp>& pq);
template <int M> void dprint(ostream& os, const atcoder::static_modint<M>& x);
template <int I> void dprint(ostream& os, const atcoder::dynamic_modint<I>& x);
void dprint(ostream& os, const atcoder::dsu& d);
// （内部用）[a, b, c] の形で出す
template <class It> void dprint_range(ostream& os, It first, It last) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    os << '[';
    for (It it = first; it != last; ++it) os << (it == first ? "" : ", "), dprint(os, *it);
    os << ']';
}
template <class T> void dprint(ostream& os, const T& x) { os << x; }  // 数値・文字列など、もともと << で出せるもの
template <class A, class B> void dprint(ostream& os, const pair<A, B>& p) { os << '(', dprint(os, p.first), os << ", ", dprint(os, p.second), os << ')'; }
template <class... Ts> void dprint(ostream& os, const tuple<Ts...>& t) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    os << '(';
    apply([&](const auto&... xs) { int i = 0; ((os << (i++ ? ", " : ""), dprint(os, xs)), ...); }, t);
    os << ')';
}
template <class T, class A> void dprint(ostream& os, const vector<T, A>& v) { dprint_range(os, v.begin(), v.end()); }
template <class T, class A> void dprint(ostream& os, const deque<T, A>& v) { dprint_range(os, v.begin(), v.end()); }
template <class T, class A> void dprint(ostream& os, const list<T, A>& v) { dprint_range(os, v.begin(), v.end()); }
template <class T, size_t N> void dprint(ostream& os, const array<T, N>& v) { dprint_range(os, v.begin(), v.end()); }
template <class T, class C, class A> void dprint(ostream& os, const set<T, C, A>& v) { dprint_range(os, v.begin(), v.end()); }
template <class T, class C, class A> void dprint(ostream& os, const multiset<T, C, A>& v) { dprint_range(os, v.begin(), v.end()); }
template <class T, class H, class E, class A> void dprint(ostream& os, const unordered_set<T, H, E, A>& v) { dprint_range(os, v.begin(), v.end()); }
template <class T, class H, class E, class A> void dprint(ostream& os, const unordered_multiset<T, H, E, A>& v) { dprint_range(os, v.begin(), v.end()); }
template <class K, class V, class C, class A> void dprint(ostream& os, const map<K, V, C, A>& v) { dprint_range(os, v.begin(), v.end()); }
template <class K, class V, class C, class A> void dprint(ostream& os, const multimap<K, V, C, A>& v) { dprint_range(os, v.begin(), v.end()); }
template <class K, class V, class H, class E, class A> void dprint(ostream& os, const unordered_map<K, V, H, E, A>& v) { dprint_range(os, v.begin(), v.end()); }
// queue / stack / priority_queue は中を順に見る方法がないので、コピーを取り出しながら並べる（元の中身は変わらない）
template <class T, class C> void dprint(ostream& os, const queue<T, C>& q) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    vector<T> v;
    for (auto c = q; !c.empty(); c.pop()) v.push_back(c.front());
    dprint(os, v);
}
template <class T, class C> void dprint(ostream& os, const stack<T, C>& st) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    vector<T> v;
    for (auto c = st; !c.empty(); c.pop()) v.push_back(c.top());
    dprint(os, v);
}
template <class T, class C, class Cmp> void dprint(ostream& os, const priority_queue<T, C, Cmp>& pq) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    vector<T> v;
    for (auto c = pq; !c.empty(); c.pop()) v.push_back(c.top());
    dprint(os, v);
}
template <int M> void dprint(ostream& os, const atcoder::static_modint<M>& x) { os << x.val(); }
template <int I> void dprint(ostream& os, const atcoder::dynamic_modint<I>& x) { os << x.val(); }
void dprint(ostream& os, const atcoder::dsu& d) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp
    atcoder::dsu c = d;  // groups() は const で呼べないのでコピーして使う
    dprint(os, c.groups());
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
    cerr.write(names, comma - names) << " = ";
    dprint(cerr, h);
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
