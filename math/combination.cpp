// =====================================================================
// math/combination.cpp  二項係数（mod 素数）
//   削除可: n が巨大で k が小さいとき、mod なしのパスカルの三角形、Lucas の定理（mod が小さい素数 p のとき）
//   階乗と階乗の逆元を前計算して nCk を O(1)。足りなければ自動でテーブルを伸ばす
// 依存: ACL の modint（AtCoder では #include <atcoder/modint> が使える）
// ABC / ARC での使用例:
//   ABC145 D - Knight  https://atcoder.jp/contests/abc145/tasks/abc145_d
//     → 2 種類の移動の回数を求めて C(n, k)
//   ABC202 D - aab aba baa  https://atcoder.jp/contests/abc202/tasks/abc202_d
//     → mod なしの二項係数表（pascal）で K 番目を決める
//   ARC117 C - Tricolor Pyramid  https://atcoder.jp/contests/arc117/tasks/arc117_c
//     → 色を 0, 1, 2 にすると上の色 = −(左 + 右) mod 3。一番上 = (−1)^(N−1) Σ C(N−1, i) c_i mod 3 で、二項係数は Lucas(3)
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
#include <atcoder/modint>
using namespace std;
using mint = atcoder::modint998244353;  // 問題に合わせて modint1000000007 などに

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// 【自分で定義するもの】mint 型（上の using mint = ...）。mod は素数、n < mod
// Combination<mint> cb(200000);  // 最大の n（省略しても自動拡張）
// cb.C(n, k) 組合せ（範囲外は 0）、cb.P(n, k) 順列、cb.H(n, k) 重複組合せ
// cb.fact(n) n!、cb.ifact(n) 1/n!、cb.inv(n) 1/n
// よく使う: 格子経路 (0,0)->(h,w) = C(h+w, h)
//           n 個を k 箱（空OK）= C(n+k-1, n)、（空NG）= C(n-1, k-1)
//           カタラン数 = C(2n, n) - C(2n, n+1)
template <class mint>
struct Combination {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/combination.cpp
    vector<mint> f, fi;  // f[i] = i!, fi[i] = 1 / i!
    Combination(int n = 0) : f(1, mint(1)), fi(1, mint(1)) { extend(n); }
    void extend(int n) {
        int m = f.size();
        if (n < m) return;
        assert(n < (int)mint::mod());  // n は mod 未満であること（n >= mod だと n! ≡ 0 になる。そのときは Lucas を使う）
        int nm = max(n + 1, (int)min<long long>(2LL * m, mint::mod()));  // ★ 倍々に伸ばすが、mod を超えないように（超えると逆元が取れない）
        f.resize(nm), fi.resize(nm);
        for (int i = m; i < nm; i++) f[i] = f[i - 1] * mint(i);    // ★ i! = (i-1)! * i
        fi[nm - 1] = f[nm - 1].inv();                              // ★ 最後だけ逆元を計算し
        for (int i = nm - 1; i > m; i--) fi[i - 1] = fi[i] * mint(i);  // ★ 1/(i-1)! = 1/i! * i で戻る
    }
    mint fact(int n) { extend(n); return f[n]; }
    mint ifact(int n) { extend(n); return fi[n]; }
    mint inv(int n) { extend(n); return fi[n] * f[n - 1]; }
    mint C(int n, int k) {
        if (n < 0 || k < 0 || k > n) return mint(0);
        extend(n);
        return f[n] * fi[k] * fi[n - k];  // ★ n! / (k! (n-k)!)
    }
    mint P(int n, int k) {
        if (n < 0 || k < 0 || k > n) return mint(0);
        extend(n);
        return f[n] * fi[n - k];
    }
    mint H(int n, int k) { return n == 0 ? mint(k == 0) : C(n + k - 1, k); }
};

// ---------- 削除可: n が巨大（~1e18）で k が小さいときの nCk。O(k) ----------
template <class mint>
mint comb_large_n(long long n, int k) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/combination.cpp
    if (k < 0 || n < k) return mint(0);
    mint num = 1, den = 1;
    for (int i = 0; i < k; i++) num *= mint(n - i), den *= mint(i + 1);  // ★ n(n-1)...(n-k+1) / k!
    return num / den;
}

// ---------- 削除可: mod なしのパスカルの三角形（n <= 66 で long long に収まる。ACL 不要） ----------
vector<vector<long long>> pascal(int n) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/combination.cpp
    vector<vector<long long>> c(n + 1, vector<long long>(n + 1, 0));
    for (int i = 0; i <= n; i++) {
        c[i][0] = 1;
        for (int j = 1; j <= i; j++) c[i][j] = c[i - 1][j - 1] + c[i - 1][j];  // ★ 上 2 つの和
    }
    return c;
}

// ---------- 削除可: Lucas の定理（mod が小さい素数 p のときの nCk。n, k は 1e18 まで OK） ----------
// mod が小さい（2, 3 など）と n >= p で n! が 0 になり、本体の Combination が使えない。そのときはこちら
// Lucas lu(p);   p は素数（1e7 くらいまで。前計算 O(p)）
//   lu.C(n, k)   nCk mod p（int）。O(log_p n)
// ★ n, k を p 進数で書いて、桁ごとの二項係数を掛ける: C(n, k) ≡ Π C(n の i 桁目, k の i 桁目) (mod p)
//   特に p = 2: nCk が奇数 ⇔ (n & k) == k
struct Lucas {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/combination.cpp
    int p;
    vector<long long> f, fi;
    Lucas(int p_) : p(p_), f(p_), fi(p_) {
        f[0] = 1;
        for (int i = 1; i < p; i++) f[i] = f[i - 1] * i % p;
        long long a = f[p - 1], e = p - 2, r = 1;  // フェルマーの小定理で逆元
        for (; e; e >>= 1, a = a * a % p) if (e & 1) r = r * a % p;
        fi[p - 1] = r;
        for (int i = p - 1; i > 0; i--) fi[i - 1] = fi[i] * i % p;
    }
    long long small(int n, int k) const { return k > n ? 0 : f[n] * fi[k] % p * fi[n - k] % p; }  // n, k < p
    int C(long long n, long long k) const {
        if (k < 0 || n < k) return 0;
        long long r = 1;
        while (n > 0 || k > 0) {
            r = r * small(n % p, k % p) % p;  // ★ 一番下の桁どうしの二項係数を掛ける
            if (r == 0) return 0;
            n /= p, k /= p;
        }
        return r;
    }
};

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    Combination<mint> cb;
    assert(cb.C(5, 2).val() == 10 && cb.C(3, 5).val() == 0 && cb.P(5, 2).val() == 20 && cb.H(3, 2).val() == 6);
    assert(cb.C(300000, 3) == comb_large_n<mint>(300000, 3));
    assert((cb.inv(3) * 3).val() == 1);
    assert(pascal(60)[60][30] == 118264581564861424LL);
    // ランダムテスト: パスカルの三角形・直接の掛け算・数え上げと比較
    {
        mt19937 rr(116);
        Combination<mint> c2;
        auto pc = pascal(60);
        for (int n = 0; n <= 60; n++)
            for (int k = -1; k <= n + 1; k++) {
                mint ref = (k < 0 || k > n) ? mint(0) : mint(pc[n][k] % 998244353);
                assert(c2.C(n, k) == ref);
                if (k < 0 || k > n) continue;
                mint pr = 1;
                for (int i = 0; i < k; i++) pr *= n - i;
                assert(c2.P(n, k) == pr);
            }
        for (int n = 1; n <= 8; n++)
            for (int k = 0; k <= 8; k++) {  // H(n, k) = 0..n-1 から重複を許して k 個選ぶ方法の数
                long long cnt = 0;
                function<void(int, int)> rec = [&](int left, int lo) {
                    if (left == 0) { cnt++; return; }
                    for (int x = lo; x < n; x++) rec(left - 1, x);
                };
                rec(k, 0);
                assert(c2.H(n, k) == mint(cnt));
            }
        for (int it = 0; it < 300; it++) {
            int n = rr() % 200000, k = rr() % 50;
            assert(comb_large_n<mint>(n, k) == c2.C(n, k));
        }
        for (int n = 1; n < 2000; n++) assert(c2.inv(n) * n == 1 && c2.fact(n) * c2.ifact(n) == 1);
    }
    // mod が小さめ（1000003）で、テーブルを倍々に伸ばしても mod を超えないか（以前はここで落ちていた）
    {
        Combination<atcoder::static_modint<1000003>> cb2;
        cb2.C(600000, 3);
        assert(cb2.C(700000, 3) == atcoder::static_modint<1000003>(700000LL * 699999 / 2 % 1000003 * 699998 % 1000003) / 3);
        assert(cb2.C(1000002, 1) == 1000002);
    }
    // Lucas の定理
    {
        Lucas l3(3);
        // C(5, 2) = 10 ≡ 1 (mod 3)、C(6, 3) = 20 ≡ 2 (mod 3)、C(9, 3) = 84 ≡ 0 (mod 3)
        assert(l3.C(5, 2) == 1 && l3.C(6, 3) == 2 && l3.C(9, 3) == 0 && l3.C(3, 5) == 0);
        auto pc = pascal(60);
        for (int p : {2, 3, 5, 7, 11, 13, 97}) {  // n <= 60 はパスカルの三角形（mod なし）と比較
            Lucas lu(p);
            for (int n = 0; n <= 60; n++) for (int k = -1; k <= n + 1; k++) assert(lu.C(n, k) == (k < 0 || k > n ? 0 : pc[n][k] % p));
        }
        mt19937_64 rr(173);
        Lucas l2(2);
        for (int it = 0; it < 100000; it++) {  // p = 2: 奇数 ⇔ (n & k) == k
            long long n = rr() % 1000000000000000000LL, k = rr() % (n + 1);
            if (it % 2) k &= n;
            assert(l2.C(n, k) == (((n & k) == k) ? 1 : 0));
        }
        // p が大きい素数で n < p なら本体の Combination（mod p）と一致
        Lucas lb(1000003);
        Combination<atcoder::static_modint<1000003>> cbp;
        for (int it = 0; it < 2000; it++) {
            int n = rr() % 1000003, k = rr() % (n + 1);
            assert(lb.C(n, k) == (int)cbp.C(n, k).val());
        }
        // n >= p のとき: C(p^2 + 5, p + 2) は桁が (1, 0, 5) と (0, 1, 2) → C(1,0) C(0,1) C(5,2) = 0
        Lucas l7(7);
        assert(l7.C(49 + 5, 7 + 2) == 0 && l7.C(49 + 7 + 5, 7 + 2) == (int)(pc[5][2] % 7));  // 後者は C(1,0) C(1,1) C(5,2) = 10 ≡ 3
    }
    puts("OK");
}
