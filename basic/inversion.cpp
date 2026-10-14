// =====================================================================
// basic/inversion.cpp  転倒数
//   i < j かつ a[i] > a[j] となる組 (i, j) の個数。O(N log N)
//   典型: 「隣り合う要素の交換だけで昇順に並べる最小回数」= 転倒数
//         「順列 P を順列 Q にする隣接交換の最小回数」= Q での位置に置き換えた列の転倒数
//   値が大きくても（~1e9）、負でもよい（内部で座標圧縮する）
//   求め方は 2 通り: BIT 版（本体。各位置ごとの個数も出せる）と、マージソート版（削除可。ACL も BIT も不要）
//   どんな問題で転倒数が出てくるかは techniques/patterns.md の「転倒数」にまとめてある
// 依存: ACL の fenwick_tree（AtCoder では #include <atcoder/fenwicktree>、テンプレートの atcoder/all に含まれる）
// ABC / ARC での使用例:
//   ABC190 F - Shift and Inversions  https://atcoder.jp/contests/abc190/tasks/abc190_f
//     → 最初の転倒数を求め、先頭を末尾に回すごとに差分で更新
//   ABC261 F - Sorting Color Balls  https://atcoder.jp/contests/abc261/tasks/abc261_f
//     → 同じ色どうしの入れ替えはタダ。答え = 全体の転倒数 − 色ごとの転倒数の和
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/fenwicktree>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体: 転倒数 ----------
template <class T>
long long inversion_count(const vector<T>& a) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/inversion.cpp
    vector<T> xs(a);
    sort(xs.begin(), xs.end());
    xs.erase(unique(xs.begin(), xs.end()), xs.end());  // 座標圧縮
    atcoder::fenwick_tree<long long> bit(xs.size());
    long long res = 0;
    for (int i = 0; i < (int)a.size(); i++) {
        int x = lower_bound(xs.begin(), xs.end(), a[i]) - xs.begin();
        res += i - bit.sum(0, x + 1);  // ★ 前にある i 個のうち a[i] 以下でないもの = a[i] より大きいもの
        bit.add(x, 1);                 // ★ a[i] を「出てきた値」として記録
    }
    return res;
}

// ---------- 削除可: 各位置ごとの個数 ----------
// 返り値: res[i] = j < i かつ a[j] < a[i] となる j の個数（自分より前にある、自分より小さいもの）
// 「より大きいもの」が欲しいなら、全要素を -1 倍して呼ぶ
template <class T>
vector<long long> smaller_before(const vector<T>& a) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/inversion.cpp
    vector<T> xs(a);
    sort(xs.begin(), xs.end());
    xs.erase(unique(xs.begin(), xs.end()), xs.end());
    atcoder::fenwick_tree<long long> bit(xs.size());
    vector<long long> res(a.size());
    for (int i = 0; i < (int)a.size(); i++) {
        int x = lower_bound(xs.begin(), xs.end(), a[i]) - xs.begin();
        res[i] = bit.sum(0, x);  // ★ 番号が x 未満 = 値が a[i] より小さい
        bit.add(x, 1);
    }
    return res;
}

// ---------- 削除可: マージソート版の転倒数（ACL・BIT 不要） O(N log N) ----------
// 並べ替えながら数える。左半分と右半分を合わせるとき、右の要素を先に取るたびに
// 「左に残っている要素の数」だけ転倒が見つかる
template <class T>
long long inversion_count_merge(vector<T> a) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/inversion.cpp
    int n = a.size();
    vector<T> buf(n);
    long long res = 0;
    for (int w = 1; w < n; w *= 2)  // 長さ 1, 2, 4, ... のソート済みの塊を 2 つずつ合わせる（再帰なし）
        for (int l = 0; l + w < n; l += 2 * w) {
            int m = l + w, r = min(l + 2 * w, n), i = l, j = m, k = l;
            while (i < m && j < r) {
                if (a[j] < a[i]) res += m - i, buf[k++] = a[j++];  // ★ 右の a[j] は、左に残る m - i 個すべてより小さい
                else buf[k++] = a[i++];                            // 同じ値は左を先に取る（同じ値は数えない）
            }
            while (i < m) buf[k++] = a[i++];
            while (j < r) buf[k++] = a[j++];
            copy(buf.begin() + l, buf.begin() + r, a.begin() + l);
        }
    return res;
}

// ---------- 削除可: 列 a を列 b にする隣接交換の最小回数（本体が必要） ----------
// a と b は同じ値を同じ個数ずつ含むこと（そうでなければ -1）。同じ値があってもよい
// ★ 同じ値は「a の k 番目の x」を「b の k 番目の x」に移すのが最適 → b での位置の列の転倒数になる
template <class T>
long long min_adjacent_swaps(const vector<T>& a, const vector<T>& b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/inversion.cpp
    if (a.size() != b.size()) return -1;
    map<T, vector<int>> pos;  // 値 → b での位置（後ろから使うので逆順に入れる）
    for (int i = (int)b.size() - 1; i >= 0; i--) pos[b[i]].push_back(i);
    vector<int> target(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        auto it = pos.find(a[i]);
        if (it == pos.end() || it->second.empty()) return -1;
        target[i] = it->second.back(), it->second.pop_back();
    }
    return inversion_count(target);
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    assert(inversion_count(vector<int>{3, 1, 2}) == 2);
    assert(inversion_count(vector<int>{1, 2, 3}) == 0);
    assert(inversion_count(vector<long long>{(long long)1e18, -5, (long long)1e18, -5}) == 3);  // 同じ値は数えない
    assert((smaller_before(vector<int>{3, 1, 2, 5}) == vector<long long>{0, 0, 1, 3}));

    // 順列 P を Q にする隣接交換の最小回数
    vector<int> P = {2, 0, 1}, Q = {1, 2, 0}, posQ(3), b(3);
    for (int i = 0; i < 3; i++) posQ[Q[i]] = i;
    for (int i = 0; i < 3; i++) b[i] = posQ[P[i]];  // P の各要素が Q のどこに行くか
    assert(inversion_count(b) == 2);                // {2,0,1} → {2,1,0} → {1,2,0} の 2 回

    mt19937 rng(7);
    for (int it = 0; it < 3000; it++) {
        int n = rng() % 12;
        vector<int> a(n);
        for (auto& x : a) x = (int)(rng() % 7) - 3;
        long long ref = 0;
        vector<long long> sb(n, 0);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < i; j++) ref += a[j] > a[i], sb[i] += a[j] < a[i];
        assert(inversion_count(a) == ref && smaller_before(a) == sb);
        assert(inversion_count_merge(a) == ref);
    }

    // マージソート版・隣接交換の例
    assert(inversion_count_merge(vector<int>{3, 1, 2}) == 2 && inversion_count_merge(vector<int>{}) == 0);
    // "aab" → "baa": b を左へ 2 回動かす
    assert(min_adjacent_swaps(vector<char>{'a', 'a', 'b'}, vector<char>{'b', 'a', 'a'}) == 2);
    assert(min_adjacent_swaps(vector<int>{1, 2}, vector<int>{1, 3}) == -1);

    // ランダムテスト: 隣接交換の最小回数を BFS（隣を交換する操作で最短何回か）で求めたものと比較
    for (int it = 0; it < 500; it++) {
        int n = rng() % 6 + 1;
        vector<int> a(n);
        for (auto& x : a) x = rng() % 3;
        vector<int> b = a;
        shuffle(b.begin(), b.end(), rng);
        map<vector<int>, int> dist = {{a, 0}};
        queue<vector<int>> q;
        q.push(a);
        while (!q.empty()) {
            auto v = q.front();
            q.pop();
            if (v == b) break;
            for (int i = 0; i + 1 < n; i++) {
                auto w = v;
                swap(w[i], w[i + 1]);
                if (!dist.count(w)) dist[w] = dist[v] + 1, q.push(w);
            }
        }
        assert(min_adjacent_swaps(a, b) == dist[b]);
    }
    puts("OK");
}
