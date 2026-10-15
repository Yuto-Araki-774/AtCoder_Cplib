// =====================================================================
// basic/sliding_window.cpp  スライド最小値（スライド最大値）
//   長さ K の区間を 1 つずつずらしたときの最小値を、全体 O(N) で求める
//   本体: 「末尾に追加・先頭から削除」ができて、中の最小値が O(1) でわかるキュー
//         → 区間を自由に伸び縮みさせる尺取り法や、「直前 K 個の dp の最小値」を使う DP にも使える
// 依存: なし
// ABC / ARC での使用例:
//   ABC352 D - Permutation Subsequence  https://atcoder.jp/contests/abc352/tasks/abc352_d
//     → 値 i の位置の列で、長さ K の区間の max − min の最小（sliding_max / sliding_min）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体: 最小値つきキュー（単調 deque） ----------
// MinQueue<long long> q;            最小値（MinQueue<long long, greater<long long>> で最大値）
//   q.push(x)   末尾に x を追加      ならし O(1)
//   q.pop()     先頭を 1 つ削除      O(1)
//   q.get()     中の最小値           O(1)（空のときに呼ばないこと）
//   q.size(), q.empty()
// 例: 直前 K 個の dp の最小を使う DP（dp[i] = min(dp[i-K..i-1]) + c[i]）
//   q.push(dp[0]); for i: { if (q.size() > K) q.pop(); dp[i] = q.get() + c[i]; q.push(dp[i]); }
// ◆ struct MinQueue : 最小値（Cmp = greater<T> なら最大値）がすぐわかるキュー
//   push(x)   : 末尾に x を追加。ならし O(1)
//   pop()     : 先頭（一番古いもの）を 1 つ削除。O(1)。空のときに呼ばないこと
//   get()     : 中の最小値。O(1)。空のときに呼ばないこと
//   size(), empty() : 個数、空か
//   T : 値の型、Cmp : 比べ方（省略で最小、greater<T> で最大）
template <class T, class Cmp = less<T>>
struct MinQueue {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/sliding_window.cpp
    deque<pair<long long, T>> dq;  // {追加した番号, 値}。値は先頭から単調に並ぶ
    long long head = 0, tail = 0;  // 先頭の番号、次に追加する番号
    Cmp cmp;
    // x : 末尾に入れる値
    void push(const T& x) {
        while (!dq.empty() && !cmp(dq.back().second, x)) dq.pop_back();  // ★ x 以上の値は、x がいる間は最小になれないので捨てる
        dq.push_back({tail++, x});
    }
    // 先頭を 1 つ取り除く（キューなので一番古いもの）
    void pop() {
        if (!dq.empty() && dq.front().first == head) dq.pop_front();  // ★ 削除する要素が deque に残っていれば取り除く
        head++;
    }
    // 今入っている中の最小値（空なら未定義）
    T get() const { return dq.front().second; }  // ★ 先頭が最小
    // size() : 入っている個数
    long long size() const { return tail - head; }
    // empty() : 空か
    bool empty() const { return tail == head; }
};

// ---------- 削除可: スライド最小値・最大値（本体が必要） ----------
// 返り値: res[i] = min(a[i], ..., a[i+k-1])（i = 0 .. n-k）
// ◆ sliding_min(a, k) / sliding_max(a, k) : 長さ k の区間をずらしたときの最小値 / 最大値。O(N)
//   a : 数列
//   k : 区間の長さ（1 <= k <= N。k > N なら空の配列）
//   返り値 : res[i] = min(a[i..i+k))（i = 0..N−k）
template <class T>
vector<T> sliding_min(const vector<T>& a, int k) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/sliding_window.cpp
    MinQueue<T> q;
    vector<T> res;
    for (int i = 0; i < (int)a.size(); i++) {
        q.push(a[i]);
        if (q.size() > k) q.pop();  // ★ 区間の長さを k に保つ
        if (i >= k - 1) res.push_back(q.get());
    }
    return res;
}
// （引数は sliding_min と同じ。最大値を返す）
template <class T>
vector<T> sliding_max(const vector<T>& a, int k) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/sliding_window.cpp
    MinQueue<T, greater<T>> q;
    vector<T> res;
    for (int i = 0; i < (int)a.size(); i++) {
        q.push(a[i]);
        if (q.size() > k) q.pop();
        if (i >= k - 1) res.push_back(q.get());
    }
    return res;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    vector<int> a = {4, 2, 12, 3, 8, 1, 7};
    assert((sliding_min(a, 3) == vector<int>{2, 2, 3, 1, 1}));
    assert((sliding_max(a, 3) == vector<int>{12, 12, 12, 8, 8}));
    assert((sliding_min(a, 1) == a) && sliding_min(a, 8).empty());

    // 直前 K 個の dp の最小を使う DP: 1 マスから K マス先までジャンプ、着地したマスのコストを払う
    vector<long long> c = {0, 5, 1, 9, 9, 2, 8, 3};
    int K = 2, n = c.size();
    vector<long long> dp(n);
    MinQueue<long long> q;
    dp[0] = c[0], q.push(dp[0]);
    for (int i = 1; i < n; i++) {
        if (q.size() > K) q.pop();
        dp[i] = q.get() + c[i];
        q.push(dp[i]);
    }
    vector<long long> ref(n, LLONG_MAX);
    ref[0] = c[0];
    for (int i = 1; i < n; i++)
        for (int j = max(0, i - K); j < i; j++) ref[i] = min(ref[i], ref[j] + c[i]);
    assert(dp == ref);

    // ランダムテスト: 自由に push / pop して最小値を愚直と比較
    mt19937 rng(8);
    for (int it = 0; it < 200; it++) {
        MinQueue<int> mq;
        MinQueue<int, greater<int>> Mq;
        deque<int> ref2;
        for (int op = 0; op < 300; op++) {
            if (!ref2.empty() && rng() % 3 == 0) mq.pop(), Mq.pop(), ref2.pop_front();
            else { int x = rng() % 10; mq.push(x), Mq.push(x), ref2.push_back(x); }
            assert(mq.size() == (long long)ref2.size());
            if (!ref2.empty()) {
                assert(mq.get() == *min_element(ref2.begin(), ref2.end()));
                assert(Mq.get() == *max_element(ref2.begin(), ref2.end()));
            }
        }
    }
    puts("OK");
}
