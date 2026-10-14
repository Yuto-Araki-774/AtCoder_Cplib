// =====================================================================
// ds/erasable_pq.cpp  削除できる優先度付きキュー・上位 K 個の和・中央値
//   本体A: 削除できる優先度付きキュー（priority_queue 2 本。好きな要素を後から消せる）
//   本体B: 上位 K 個の和（要素の追加・削除があっても、大きい方から K 個の和を O(log N) で保つ）
//   本体C: 中央値（追加だけ。小さい半分と大きい半分のヒープ 2 本）
//   A・B・C は互いに独立
// 依存: なし
// ABC / ARC での使用例:
//   ABC253 C - Max - Min Query  https://atcoder.jp/contests/abc253/tasks/abc253_c
//     → 最大用・最小用の ErasablePQ に同じ値を入れる。消す個数は値ごとの個数を map で数えて min(c, 個数)
//   ABC306 E - Best Performances  https://atcoder.jp/contests/abc306/tasks/abc306_e
//     → TopKSum で、値の変更を erase(古い値) と insert(新しい値) にする
//   ABC281 E - Least Elements  https://atcoder.jp/contests/abc281/tasks/abc281_e
//     → 長さ M の区間をずらしながら -A を TopKSum に入れ、小さい方から K 個の和 = -sum
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体A: 削除できる優先度付きキュー ----------
// ErasablePQ<long long> q;                        top が最大（最小にするなら ErasablePQ<long long, greater<long long>>）
//   q.push(x), q.erase(x), q.top(), q.pop(), q.size(), q.empty()   どれもならし O(log N)
//   erase(x) は x が入っているときだけ呼ぶこと（入っていない値を消すと壊れる）
// 例: 最大値と最小値を両方見たいなら、最大用と最小用の 2 つに同じ値を入れる
template <class T, class Cmp = less<T>>
struct ErasablePQ {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/erasable_pq.cpp
    priority_queue<T, vector<T>, Cmp> q, del;  // del = 「消す予定」の値
    void push(const T& x) { q.push(x); }
    void erase(const T& x) { del.push(x); }  // ★ すぐには消さず、メモだけしておく
    void clean() {
        Cmp c;
        while (!del.empty() && !c(q.top(), del.top()) && !c(del.top(), q.top())) q.pop(), del.pop();  // ★ 先頭が「消す予定」なら、ここでまとめて消す
    }
    const T& top() { clean(); return q.top(); }
    void pop() { clean(); q.pop(); }
    size_t size() const { return q.size() - del.size(); }
    bool empty() const { return size() == 0; }
};

// ---------- 本体B: 上位 K 個の和 ----------
// TopKSum<long long> tk(K);
//   tk.insert(x), tk.erase(x)   値の追加・削除 O(log N)（erase は入っている値だけ）
//   tk.sum                       大きい方から K 個の和（全部で K 個未満なら全部の和）
// 小さい方から K 個の和が欲しいなら、-x を入れて -tk.sum を使う
template <class T>
struct TopKSum {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/erasable_pq.cpp
    int K;
    multiset<T> top, rest;  // top = 大きい方から K 個、rest = 残り
    T sum = T(0);
    TopKSum(int k) : K(k) {}
    void insert(const T& x) {
        top.insert(x), sum += x;
        if ((int)top.size() > K) {  // ★ K 個を超えたら、top の一番小さいものを rest へ
            auto it = top.begin();
            sum -= *it, rest.insert(*it), top.erase(it);
        }
    }
    void erase(const T& x) {
        auto it = rest.find(x);
        if (it != rest.end()) { rest.erase(it); return; }  // ★ rest にあれば和は変わらない
        top.erase(top.find(x)), sum -= x;
        if (!rest.empty()) {  // ★ top が 1 つ減ったので、rest の一番大きいものを top へ
            auto jt = prev(rest.end());
            sum += *jt, top.insert(*jt), rest.erase(jt);
        }
    }
};

// ---------- 本体C: 中央値（追加のみ） ----------
// MedianHeap<long long> mh;  mh.push(x);  mh.median()  （要素が偶数個なら小さい方の中央値）
template <class T>
struct MedianHeap {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/erasable_pq.cpp
    priority_queue<T> lo;                             // 小さい半分（最大が先頭）
    priority_queue<T, vector<T>, greater<T>> hi;      // 大きい半分（最小が先頭）
    void push(const T& x) {
        if (lo.empty() || x <= lo.top()) lo.push(x);
        else hi.push(x);
        if (lo.size() > hi.size() + 1) hi.push(lo.top()), lo.pop();  // ★ 個数を lo = hi か hi + 1 に保つ
        else if (hi.size() > lo.size()) lo.push(hi.top()), hi.pop();
    }
    T median() const { return lo.top(); }  // ★ 小さい半分の最大 = 中央値
    size_t size() const { return lo.size() + hi.size(); }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    ErasablePQ<int> q;
    for (int x : {5, 1, 8, 3}) q.push(x);
    q.erase(8);
    assert(q.top() == 5 && q.size() == 3);  // 8 を消したので最大は 5
    q.pop();
    assert(q.top() == 3);

    TopKSum<long long> tk(2);
    for (long long x : {5, 1, 8, 3}) tk.insert(x);
    assert(tk.sum == 13);  // 8 + 5
    tk.erase(8);
    assert(tk.sum == 8);   // 5 + 3

    MedianHeap<int> mh;
    for (int x : {5, 1, 8}) mh.push(x);
    assert(mh.median() == 5);  // {1, 5, 8}
    mh.push(3);
    assert(mh.median() == 3);  // {1, 3, 5, 8} の小さい方の中央値

    // ランダムテスト: multiset / ソートした配列と比較
    mt19937 rr(131);
    for (int it = 0; it < 500; it++) {
        ErasablePQ<int> mx;
        ErasablePQ<int, greater<int>> mn;
        int K = rr() % 5 + 1;
        TopKSum<long long> t(K);
        MedianHeap<int> m;
        multiset<int> ref, all;
        for (int op = 0; op < 100; op++) {
            if (ref.empty() || rr() % 3) {
                int x = rr() % 10;
                mx.push(x), mn.push(x), t.insert(x), ref.insert(x);
            } else {
                auto itr = ref.begin();
                advance(itr, rr() % ref.size());
                int x = *itr;
                mx.erase(x), mn.erase(x), t.erase(x), ref.erase(itr);
            }
            assert(mx.size() == ref.size() && mn.size() == ref.size());
            if (!ref.empty()) {
                assert(mx.top() == *ref.rbegin() && mn.top() == *ref.begin());
                if (rr() % 4 == 0) {  // pop も混ぜる
                    int y = *ref.rbegin();
                    mx.pop(), mn.erase(y), t.erase(y), ref.erase(prev(ref.end()));
                }
            }
            long long s = 0;
            int c = 0;
            for (auto it2 = ref.rbegin(); it2 != ref.rend() && c < K; it2++, c++) s += *it2;
            assert(t.sum == s);
            int z = rr() % 10;  // 中央値は追加だけ
            m.push(z), all.insert(z);
            auto it3 = all.begin();
            advance(it3, (all.size() - 1) / 2);
            assert(m.median() == *it3);
        }
    }
    puts("OK");
}
