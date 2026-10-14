// =====================================================================
// basic/compress.cpp  座標圧縮
//   大きな値（~1e9）や飛び飛びの値を 0, 1, 2, ... の番号に振り直す
// 依存: なし
// ABC / ARC での使用例:
//   ABC036 C - 座圧  https://atcoder.jp/contests/abc036/tasks/abc036_c
//     → そのまま座標圧縮
//   ABC213 C - Reorder Cards  https://atcoder.jp/contests/abc213/tasks/abc213_c
//     → 行番号・列番号をそれぞれ圧縮
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// T : < と == ができる型（pair も可）
// Compress<T> cp(v)  または  cp.add(x) を繰り返して cp.build()   ※ add 後は必ず build
// get(x) : 値 → 番号（x が無いときは「x 以上の最小要素」の番号 = x 未満の種類数）
// cp[i]  : 番号 → 元の値
template <class T>
struct Compress {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/compress.cpp
    vector<T> xs;
    Compress() {}
    Compress(const vector<T>& v) : xs(v) { build(); }
    void add(const T& x) { xs.push_back(x); }
    void build() {
        sort(xs.begin(), xs.end());                        // ★ ソートして
        xs.erase(unique(xs.begin(), xs.end()), xs.end());  // ★ 重複を消す
    }
    int get(const T& x) const { return lower_bound(xs.begin(), xs.end(), x) - xs.begin(); }  // ★ 位置 = 番号
    const T& operator[](int i) const { return xs[i]; }
    int size() const { return xs.size(); }

    // --- 削除可: 便利関数 ---
    void add(const vector<T>& v) { xs.insert(xs.end(), v.begin(), v.end()); }
    vector<int> get(const vector<T>& v) const {
        vector<int> res(v.size());
        for (int i = 0; i < (int)v.size(); i++) res[i] = get(v[i]);
        return res;
    }
    bool contains(const T& x) const { return binary_search(xs.begin(), xs.end(), x); }
    // --- 削除可ここまで ---
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    vector<long long> a = {100, -5, 100, 7};
    Compress<long long> cp(a);
    assert(cp.size() == 3 && cp.get(7) == 1 && cp[2] == 100 && cp.get(8) == 2);
    assert((cp.get(a) == vector<int>{2, 0, 2, 1}));
    // ランダムテスト: 大小関係が保たれるか、番号から値に戻るか
    {
        mt19937 rr(103);
        for (int it = 0; it < 3000; it++) {
            int n = rr() % 10 + 1;
            vector<long long> v(n);
            for (auto& x : v) x = (long long)(rr() % 7) - 3;
            Compress<long long> c(v);
            set<long long> st(v.begin(), v.end());
            assert(c.size() == (int)st.size());
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++) assert((v[i] < v[j]) == (c.get(v[i]) < c.get(v[j])) && c[c.get(v[i])] == v[i]);
            long long x = (long long)(rr() % 9) - 4;
            int less = 0;
            for (long long y : st) less += y < x;
            assert(c.get(x) == less && c.contains(x) == (st.count(x) > 0));
            Compress<long long> c2;
            for (long long y : v) c2.add(y);
            c2.build();
            assert(c2.get(v) == c.get(v));
        }
    }
    puts("OK");
}
