// =====================================================================
// basic/run_length.cpp  ランレングス圧縮
//   同じ値が続く部分を {値, 個数} にまとめる。例: "aaabcc" → {a,3} {b,1} {c,2}
//   O(N)。string でも vector でも使える
// 依存: なし
// ABC / ARC での使用例:
//   ABC019 B - 高橋くんと文字列圧縮  https://atcoder.jp/contests/abc019/tasks/abc019_2
//     → そのままランレングス圧縮
//   ABC329 C - Count xxx  https://atcoder.jp/contests/abc329/tasks/abc329_c
//     → 文字ごとに一番長い塊の長さを足す
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// auto rl = run_length(s);    for (auto [c, k] : rl) { ... }   // c が k 個続いている
// 型は自動で決まる（string なら pair<char, int>、vector<long long> なら pair<long long, int>）
// ◆ run_length(s) : 同じ値が続く塊ごとに {値, 個数} にまとめる。O(N)
//   s : string か vector など（== で比べられる要素の列）
template <class Seq>
vector<pair<typename Seq::value_type, long long>> run_length(const Seq& s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/run_length.cpp
    vector<pair<typename Seq::value_type, long long>> res;
    for (const auto& x : s) {
        if (!res.empty() && res.back().first == x) res.back().second++;  // ★ 直前と同じなら個数を増やす
        else res.push_back({x, 1});                                     // ★ 違えば新しい塊を始める
    }
    return res;
}

// ---------- 削除可: 塊ごとの位置付き。{値, 開始位置, 終了位置（含まない）} ----------
// 「各塊が元の配列のどこにあるか」が必要なとき（塊をまたぐ処理、塊の長さで区間を作るなど）
// ◆ run_length_range(s) : 塊ごとに {値, 開始位置, 終了位置（含まない）}
//   s : string か vector など
template <class Seq>
vector<tuple<typename Seq::value_type, long long, long long>> run_length_range(const Seq& s) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/run_length.cpp
    vector<tuple<typename Seq::value_type, long long, long long>> res;
    int n = s.size();
    for (int l = 0, r = 0; l < n; l = r) {
        while (r < n && s[r] == s[l]) r++;  // ★ 同じ値が続く限り右へ
        res.push_back({s[l], l, r});
    }
    return res;
}

// ---------- 削除可: 復元（文字列用） ----------
// ◆ run_length_decode(rl) : {文字, 個数} の列を文字列に戻す
//   rl : run_length(string) の返り値の形
string run_length_decode(const vector<pair<char, long long>>& rl) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/run_length.cpp
    string s;
    for (auto [c, k] : rl) s += string(k, c);
    return s;
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    string s = "aaabccdddd";
    auto rl = run_length(s);
    assert((rl == vector<pair<char, long long>>{{'a', 3}, {'b', 1}, {'c', 2}, {'d', 4}}));
    assert(run_length_decode(rl) == s);

    // 圧縮した文字列を "a3b1c2d4" の形で出力
    string out;
    for (auto [c, k] : rl) out += c + to_string(k);
    assert(out == "a3b1c2d4");

    vector<long long> v = {5, 5, 1, 1, 1, 5};
    auto rv = run_length(v);
    assert((rv.size() == 3 && rv[1] == pair<long long, long long>(1LL, 3)));

    auto rr = run_length_range(s);
    assert(rr.size() == 4 && get<1>(rr[3]) == 6 && get<2>(rr[3]) == 10);
    assert(run_length(string("")).empty());
    // ランダムテスト: 復元すると元に戻るか、塊の切れ目が正しいか
    {
        mt19937 rr(106);
        for (int it = 0; it < 3000; it++) {
            string s;
            for (int i = rr() % 12; i > 0; i--) s += char('a' + rr() % 3);
            auto rl = run_length(s);
            assert(run_length_decode(rl) == s);
            for (size_t i = 0; i < rl.size(); i++) assert(rl[i].second > 0 && (i == 0 || rl[i].first != rl[i - 1].first));
            auto rg = run_length_range(s);
            assert(rg.size() == rl.size());
            int pos = 0;
            for (size_t i = 0; i < rg.size(); i++) {
                auto [c, l, r] = rg[i];
                assert(l == pos && r - l == rl[i].second && c == rl[i].first);
                pos = r;
            }
            assert(pos == (int)s.size());
        }
    }
    puts("OK");
}
