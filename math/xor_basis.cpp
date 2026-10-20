// =====================================================================
// math/xor_basis.cpp  XOR 基底（線形基底）
//   数をいくつか選んで xor したときに作れる値の集合を、高々 60 個の「基底」で表す
//   → 作れるか判定、作れる最大値、作れる値の種類数（2^基底の数）が O(60) でわかる
//   値は 0 以上 2^60 未満（long long の範囲なら OK）
// 依存: なし
// ABC / ARC での使用例:
//   ABC141 F - Xor Sum 3  https://atcoder.jp/contests/abc141/tasks/abc141_f
//     → 全体の xor で 1 の桁は和に必ず入るので消し、残りで max_xor を 2 倍して足す
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// XorBasis xb;
//   xb.insert(x)       x を追加。今までの数で作れなかった（基底が増えた）なら true
//   xb.can_make(x)     いくつか選んで xor すると x になるか（何も選ばない 0 は常に作れる）
//   xb.max_xor(x0)     x0 と、いくつか選んだものの xor の最大値（x0 省略で 0）
//   xb.rank()          基底の数。作れる値の種類数は 2^rank
// 典型: 「xor の最大」「xor で x を作れるか」「作れる値の個数」、ガウスの消去法（F2）の代わり
// ◆ struct XorBasis : xor の基底（いくつか選んで xor して作れる値の集合を管理する）
//   insert(x)      : x を追加。今までの数で作れなかった（基底が増えた）なら true。O(60)
//   can_make(x)    : いくつか選んで xor すると x になるか（何も選ばない 0 は常に作れる）
//   max_xor(x0)    : x0 と、いくつか選んだものの xor の最大値（x0 省略で 0）
//   rank()         : 基底の数（作れる値の種類数 = 2^rank）
//   注意 : 値は 0 以上 2^60 未満であること
struct XorBasis {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/xor_basis.cpp
    static constexpr int B = 60;
    long long b[B] = {};  // b[k] = 最上位ビットが k の基底（なければ 0）
    int cnt = 0;
    // insert(x) : x : 0 以上 2^60 未満
    bool insert(long long x) {
        for (int k = B - 1; k >= 0; k--) {
            if (!(x >> k & 1)) continue;
            if (!b[k]) { b[k] = x, cnt++; return true; }  // ★ 最上位ビット k の基底がまだなければ x が新しい基底
            x ^= b[k];                                     // ★ あれば消して、より下のビットで続ける
        }
        return false;  // ★ 0 まで消えた = 既存の基底で作れる
    }
    // can_make(x) : x : 調べる値
    bool can_make(long long x) const {
        for (int k = B - 1; k >= 0; k--)
            if ((x >> k & 1) && b[k]) x ^= b[k];
        return x == 0;
    }
    // max_xor(x0) : x0 : 最初から持っている値（省略で 0）
    long long max_xor(long long x0 = 0) const {
        long long x = x0;
        for (int k = B - 1; k >= 0; k--) x = max(x, x ^ b[k]);  // ★ 上のビットから、立てられるなら立てる
        return x;
    }
    // rank() : 基底の数
    long long rank() const { return cnt; }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    XorBasis xb;
    assert(xb.insert(5) && xb.insert(3) && !xb.insert(6));  // 6 = 5 xor 3
    assert(xb.can_make(6) && xb.can_make(0) && !xb.can_make(1) && xb.max_xor() == 6 && xb.rank() == 2);
    assert(xb.max_xor(1) == 7);  // 1 xor 6

    // ランダムテスト: 部分集合を全部試して作れる値の集合を求め、比較
    mt19937_64 rr(121);
    for (int it = 0; it < 2000; it++) {
        int n = rr() % 10, bits = rr() % 6 + 1;
        vector<long long> a(n);
        for (auto& x : a) x = rr() % (1LL << bits);
        XorBasis X;
        set<long long> made = {0};
        for (long long x : a) {
            bool grew = !made.count(x);
            assert(X.insert(x) == grew);
            set<long long> nxt = made;
            for (long long y : made) nxt.insert(y ^ x);
            made = nxt;
        }
        assert((1LL << X.rank()) == (long long)made.size());
        for (long long x = 0; x < (1LL << bits); x++) assert(X.can_make(x) == (made.count(x) > 0));
        long long x0 = rr() % (1LL << bits), best = 0;
        for (long long y : made) best = max(best, x0 ^ y);
        assert(X.max_xor(x0) == best);
    }
    // 大きい値
    XorBasis big;
    big.insert((1LL << 59) + 7), big.insert((1LL << 59) + 1);
    assert(big.can_make(6) && big.max_xor() == (1LL << 59) + 7);
    puts("OK");
}
