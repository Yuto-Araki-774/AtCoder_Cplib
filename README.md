# 競プロライブラリ 一覧（コピペ用）

AtCoder（ABC / ARC）で使うアルゴリズム・データ構造・典型テクニックを、1 ファイルずつコピペで使える形にまとめた C++ ライブラリ。

- AtCoder は自作ヘッダの `#include` が使えないので、各 .cpp の「ここからコピー」〜「ここまでコピー」の範囲を、提出テンプレート（`util/template.cpp`）の決まった場所に貼って使う
- AtCoder Library（ACL）にあるもの（セグ木・UnionFind・最大流など）は含まない。ACL と一緒に使う前提
- 各関数・構造体の 1 行目に、そのファイルの GitHub の URL（出典）が書いてある
- 各ファイルの下半分は使用例とテスト（ランダムテストで素朴な解法と比較済み）。コピーは不要
- 区間はすべて半開区間 `[l, r)`、頂点・添字はすべて 0-indexed。C++17 以上

## 目次

- [使い方のルール](#使い方のルール)（提出コードの形・テンプレート引数・ファイルの中の区分け）
- [ABC / ARC での使用例](#abc--arc-での使用例)（各ライブラリを実際に使える問題）
- フォルダごとの説明:

| フォルダ | 何が入っているか | ファイル数 |
|---|---|---|
| [basic/](#basic--探索数列) | 探索・数列: 二分探索、累積和、いもす法、座標圧縮、尺取り法、全探索、転倒数、スライド最小値 など | 12 |
| [graph/](#graph--グラフ) | グラフ: BFS・DFS、ダイクストラ、最短路、トポロジカルソート、最小全域木、二部グラフ、SCC、橋・関節点、マッチング など | 22 |
| [tree/](#tree--木) | 木: LCA・直径、木 DP、全方位木 DP、オイラーツアー、HLD、重心分解、虚樹 など | 8 |
| [math/](#math--数学) | 数学: 累乗・拡張ユークリッド、行列累乗、二項係数、素数・素因数分解、メビウス変換、ガウスの消去法 など | 7 |
| [string/](#string--文字列) | 文字列: ローリングハッシュ、KMP、回文（Manacher など）、括弧列、部分列、接尾辞配列、Trie、Aho-Corasick | 9 |
| [dp/](#dp--dp) | DP: ナップサック、LCS、LIS、桁 DP、区間 DP、巡回セールスマン、期待値 DP、Alien DP など | 9 |
| [util/](#util--提出テンプレート) | 提出テンプレート: ヘッダ・mint・chmin/chmax・INF・デバッグ出力。コンパイル・テスト実行用のスクリプト `r`（Windows は `r.bat`） | 3 |
| [ds/](#ds--データ構造) | データ構造: 2 次元 BIT、平方分割、Mo、区間の集合、削除できるヒープ、Li Chao Tree、動的・永続セグ木、Wavelet Matrix | 9 |
| [geometry/](#geometry--幾何) | 幾何: 外積・内積、線分の交差、凸包、面積、内外判定、偏角ソート（整数で誤差なし） | 1 |
| [techniques/](#techniques--解法パターン) | 解法パターン: イベントソート、オフラインクエリ、区間スケジューリング、包除原理、平面走査 | 5 |

## フォルダ構成

```
basic/   探索・数列        binary_search, prefix_sum, imos, compress, sparse_table,
                           two_pointers, brute_force, run_length, ternary_search,
                           inversion, sliding_window, meet_in_the_middle
graph/   グラフ            graph_input, bfs, dfs, dijkstra, shortest_path, topological_sort,
                           kruskal, bipartite, bipartite_matching, functional_graph,
                           weighted_union_find, rollback_union_find, lowlink, scc_dag,
                           euler_path, persistent_union_find, chromatic_number, steiner_tree,
                           hungarian, general_matching, min_arborescence, dominator_tree
tree/    木                tree（LCA・直径・木上のいもす法）, tree_dp, rerooting, euler_tour, hld,
                           centroid, tree_hash, auxiliary_tree
dp/      DP                knapsack, lcs, lis, digit_dp, interval_dp, tsp, expected_value,
                           monotone_minima, alien_dp
math/    数学              integer, matrix, combination, prime, xor_basis, divisor_transform,
                           gauss
ds/      データ構造        bit2d, sqrt_decomposition, mo, interval_set, erasable_pq, li_chao_tree,
                           dynamic_segtree, wavelet_matrix, persistent_segtree
geometry/ 幾何            geometry（外積・内積・向き・線分の交差・凸包・面積・内外判定・偏角ソート）
techniques/ 解法パターン  event_sort, offline_query, interval_scheduling, inclusion_exclusion,
                           plane_sweep_2d、patterns.md（使いどころと考え方の解説）
string/  文字列            rolling_hash, kmp, manacher, palindrome, bracket, subsequence,
                           suffix_array, trie, aho_corasick
util/    提出テンプレート  template（全ヘッダ・mint・chmin/chmax・INF・方向配列・出力・デバッグ）、r / r.bat（コンパイル・実行スクリプト。r.bat は Windows 用）
```

Sparse Table は `basic/` に置いている（データ構造だが、累積和の仲間として使うことが多いため）。

---

## 使い方のルール

### 提出コードの形

新しい問題は `util/template.cpp` のコピー範囲を丸ごと貼るところから始め、ライブラリは「ここにライブラリを貼る」の位置に貼る。形は次のとおり。

```cpp
#include <bits/stdc++.h>
using namespace std;

// ↓ ライブラリのコピー範囲をここ（main の外）に貼る
template <class T> struct WEdge {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dijkstra.cpp
    ...
};
...

// ↓ op / e のように「関数として定義するもの」もここ（main の外）
int op(int a, int b) { return min(a, b); }
int e() { return INT_MAX; }

int main() {
    // ↓ ラムダ（二分探索の f、拡張ダイクストラの adj など）は main / solve の中
    auto r = bin_search<long long>(0, 2e18, [&](long long x) { return check(x); });
}
```

| 種類 | どこに書くか | 理由 |
|---|---|---|
| ライブラリ本体（`template` / `struct`） | main の外 | C++ ではテンプレートを関数の中で定義できない |
| `op`, `e`（`PrefixFold` の型引数に渡す関数） | main の外 | 型引数に渡す関数は普通のグローバル関数である必要がある |
| `f`, `adj` などのラムダ | main / solve の中 | 入力した配列などを `[&]` で参照できる |
| `using mint = ...` | main の外 | ライブラリより前に書いておく |

### `class T` / `class F` などのテンプレート引数

自分で定義するものではない。「型を後から決められる」という印で、使うときに決まる。

| 書いてあるもの | 中身 | 決め方 |
|---|---|---|
| `T`, `S`, `U` | 値の型（int, long long, mint など） | `PrefixSum<long long>` のように `<>` に書く。引数から分かる関数では省略可 |
| `F` | ラムダの型 | 常に自動。書かない |
| `op`, `e` | 関数 | 自分で関数を定義して `<>` に名前を渡す |
| `mint` | modint の型 | `using mint = atcoder::modint998244353;` |

### Git の URL

すべての関数・構造体の本体の 1 行目に `// Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/...`（そのファイルの GitHub の URL）がある。
関数や構造体を 1 つだけコピーしても、出典が必ず一緒に貼られる（貼り忘れ防止）。

```cpp
template <class T, class F>
T bin_search(T ok, T ng, F f) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/binary_search.cpp
    ...
}
```

リポジトリは [Yuto-Araki-774/AtCoder_Cplib](https://github.com/Yuto-Araki-774/AtCoder_Cplib)。すべての .cpp に、この URL を埋め込み済み。
新しく関数・構造体を追加するときも、本体の 1 行目に同じ形式（`// Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/フォルダ/ファイル名`）で URL を書く。
この README の各ファイルの見出しと表のファイル名も、GitHub のファイルへのリンクになっている。

### テストの方針

- すべてのファイルに、愚直解（全探索・単純なシミュレーション）とランダムな入力で比べるテストが入っている。ファイルを単体で実行して `OK` と出れば合格
- AddressSanitizer / UndefinedBehaviorSanitizer 付きでも全ファイルを実行している（範囲外アクセスやオーバーフローを見つけるため）
- 手で書いた使用例の期待値は、計算してから書く。テストが落ちたら、愚直解か手計算で確かめてから直し、理由をコメントに残す
- テストがバグを見つけられるか、ライブラリにわざとバグを入れて落ちることも確かめている

### ファイルの中の区分け

```
// ↓↓↓ ここからコピー ↓↓↓
// ---------- 本体 ----------            ← 必須。これだけで動く
// ---------- 本体A / 本体B ----------   ← 互いに独立。必要な方だけでよい
// ---------- 削除可: ○○ ----------     ← 便利機能。不要ならコピーしない
//     // --- 削除可: ○○ ---            ← 構造体の中の削除してよいメソッド
// ↑↑↑ ここまでコピー ↑↑↑
// ===== 使用例・動作確認（コピー不要） =====
```

- `// ★` の行がアルゴリズムの本質部分
- 関数・構造体のすぐ上に `// ◆ 名前(引数) : 何をするか` と、各引数の意味・注意点（範囲、前提、型の書き方など）を書いてある
  - 構造体は `◆ struct 名前` の下に、使える関数の一覧と、それぞれが何をするかをまとめてある
  - 構造体の中の各関数のすぐ上にも、`関数名(引数) : 引数の意味・注意点` を書いてある
  - `（内部用）` と書いた関数は中で使うだけなので、自分で呼ぶ必要はない
- どのファイルも単体で実行でき、`OK` と出れば動作確認済み
- 複数ファイルのコピー範囲を同じ提出コードに貼っても名前は衝突しない

---

## basic/ — 探索・数列

| ファイル | 何をするか | 自分で定義するもの |
|---|---|---|
| [`binary_search.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/binary_search.cpp) | 二分探索（整数・実数）、ソート済み配列の個数カウント | 判定ラムダ `f`、`ok` と `ng` の初期値 |
| [`prefix_sum.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/prefix_sum.cpp) | 累積和（1 次元・2 次元）、累積 min/max/gcd など | 累積 fold のみ `op` と `e` |
| [`imos.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/imos.cpp) | いもす法（1 次元・2 次元） | なし |
| [`compress.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/compress.cpp) | 座標圧縮 | なし |
| [`sparse_table.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/sparse_table.cpp) | 静的な配列の区間 min / max / gcd を O(1) | `op`（冪等な演算） |
| [`two_pointers.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/two_pointers.cpp) | 尺取り法。各 l について条件を満たす最大の r を全体 O(N) で | `can_push`, `push`, `pop`（ラムダ） |
| [`brute_force.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/brute_force.cpp) | bit 全探索・順列全探索・N 進数全探索・組合せ全探索・部分集合の列挙 | 各パターンで呼ぶ処理 `f`（ラムダ） |
| [`run_length.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/run_length.cpp) | ランレングス圧縮（{値, 個数} にまとめる） | なし |
| [`ternary_search.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/ternary_search.cpp) | 三分探索（凸関数の最小値。整数・実数・黄金分割） | 関数 `f`（ラムダ） |
| [`inversion.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/inversion.cpp) | 転倒数（BIT 版・マージソート版）、列 a を列 b にする隣接交換の最小回数 | なし（BIT 版は ACL の fenwick_tree を使う） |
| [`sliding_window.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/sliding_window.cpp) | スライド最小値・最大値、最小値つきキュー | なし |
| [`meet_in_the_middle.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/meet_in_the_middle.cpp) | 半分全列挙（部分和の列挙、和が K 以下の最大・個数、N ≤ 40 のナップサック） | なし |

### [binary_search.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/binary_search.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `bin_search(ok, ng, f)` | f が true の値のうち ng に最も近いもの。`ok < ng` なら最大値、`ok > ng` なら最小値 |
| 削除可 | `bin_search_real(ok, ng, f)` | 答えが実数のとき（平均の最大化、時刻・半径を求める問題など） |
| 削除可 | `count_lt / le / gt / ge(v, x)`, `count_range(v, l, r)` | ソート済み配列で x 未満・以下・より大・以上・`[l, r)` の個数。lower/upper_bound の取り違え防止 |

### [prefix_sum.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/prefix_sum.cpp)
| 区分 | 構造体 | 何をするか |
|---|---|---|
| 本体A | `PrefixSum<T>` | 区間和 `sum(l, r)` を O(1) |
| 本体B | `PrefixFold<S, op, e>` | 累積 min/max/gcd/xor。`prefix(r)`, `suffix(l)`, `except(i)`（i 番目以外）, `except_range(l, r)` |
| 本体C | `PrefixSum2D<T>` | 矩形和 `sum(r1, c1, r2, c2)` を O(1) |

`op`（結合則を満たす演算）と `e`（単位元）: min → `INT_MAX`、max → `INT_MIN`、gcd・xor → `0`、積 → `1`。

### [imos.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/imos.cpp)
| 区分 | 構造体 | 何をするか |
|---|---|---|
| 本体A | `Imos<T>(n)` | `add(l, r, x)` で `[l, r)` に加算、`build()` で配列 |
| 本体B | `Imos2D<T>(h, w)` | `add(r1, c1, r2, c2, x)` で矩形に加算、`build()` で 2 次元配列 |

木の上のいもす法は `tree/tree.cpp` の `TreeImos`。座標が大きいときは `compress.cpp` と併用。

### [compress.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/compress.cpp)
| 区分 | メソッド | 何をするか |
|---|---|---|
| 本体 | `Compress<T>(v)` / `add(x)` → `build()` | 値を登録して圧縮（add 後は必ず build） |
| 本体 | `get(x)`, `cp[i]`, `size()` | 値 → 番号、番号 → 値、種類数 |
| 削除可 | `add(v)`, `get(v)`, `contains(x)` | 配列ごと追加・変換、登録済みか |

使う場面: 値が大きい（~1e9）のに配列の添字にしたいとき（BIT・いもす・セグ木）、大小関係だけが大事なとき（順位付け・転倒数）、巨大なグリッドに点や長方形が少しだけあるとき。

### [sparse_table.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/sparse_table.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `SparseTable<S, op> st(a)` | 構築 O(N log N) |
| 本体 | `st.prod(l, r)` | `op(a[l], ..., a[r-1])` を O(1)（`l < r`） |

`op` は結合則に加えて `op(a, a) == a`（冪等）が必要。min, max, gcd, &, | は OK、和・積・xor は不可（それは累積和か ACL の segtree）。
配列が途中で変わるなら ACL の segtree を使う。

### [two_pointers.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/two_pointers.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `two_pointers(n, can_push, push, pop)` | `R[l]` = `[l, R[l])` が条件を満たす最大の右端 |

**定義**（main の中のラムダ。区間の状態はラムダの外の変数に持つ）: `can_push(r)` 区間に `a[r]` を加えても条件を満たすか、`push(r)` 加える、`pop(l)` 取り除く。
条件は「区間を縮めても満たし続ける」こと（和が K 以下、種類数が K 以下など）。区間の個数は `Σ(R[l] − l)`、最長は `max(R[l] − l)`。

### [brute_force.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/brute_force.cpp)
| 区分 | 関数 | 何をするか | 回数 |
|---|---|---|---|
| 本体A | `bit_search(n, f)` | `f(mask)`。i 番目を選ぶかは `mask >> i & 1` | 2^n（n ≤ 20） |
| 本体B | `perm_search(n, f)` | `f(p)`。p は 0..n−1 の並べ替え | n!（n ≤ 10） |
| 本体C | `product_search(base, f)` | `f(d)`。`d[i]` は 0..base[i]−1（3 グループ分け、重複順列など） | Π base[i] |
| 本体D | `comb_search(n, k, f)` | `f(c)`。c は選んだ k 個の番号（昇順） | nCk |
| 本体E | `subset_search(mask, f)` | `f(sub)`。mask の部分集合すべて（bit DP 用） | 全 mask で 3^n |

### [run_length.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/run_length.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `run_length(s)` | `{{値, 個数}, ...}`。string でも vector でも可 |
| 削除可 | `run_length_range(s)` | `{{値, 開始, 終了}, ...}`（元の位置付き） |
| 削除可 | `run_length_decode(rl)` | 文字列に戻す |

### [ternary_search.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/ternary_search.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `argmin_convex_int(lo, hi, f)` | `[lo, hi]` の整数で f が最小になる x（最小が複数なら一番左）。平らな区間があっても正しい |
| 本体B | `argmin_convex_real(lo, hi, f)` | 実数で f が最小になる x（三分探索） |
| 本体C | `golden_section_min(lo, hi, f)` | 本体B と同じ。f の呼び出し回数が半分（f が重いとき） |

f は下に凸（谷が 1 つ）であること。最大値を探すときは `-f` を渡す。

### [inversion.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/inversion.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `inversion_count(a)` | i < j かつ a[i] > a[j] の組の数。値が大きくても負でもよい |
| 削除可 | `inversion_count_merge(a)` | マージソート版（ACL・BIT 不要） |
| 削除可 | `min_adjacent_swaps(a, b)` | 列 a を列 b にする隣接交換の最小回数（同じ値があってもよい。並びが作れなければ −1） |
| 削除可 | `smaller_before(a)` | 各 i について、前にある a[i] より小さい要素の数 |

「順列 P を Q にする隣接交換の最小回数」は、P の各要素を Q での位置に置き換えた列の転倒数（使用例あり）。

### [sliding_window.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/sliding_window.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `MinQueue<T>`（最大値は `MinQueue<T, greater<T>>`） | `push(x)` 末尾に追加、`pop()` 先頭を削除、`get()` 中の最小値。すべて O(1) |
| 削除可 | `sliding_min(a, k)`, `sliding_max(a, k)` | 長さ k の区間ごとの最小値・最大値 |

「直前 K 個の dp の最小値を使う DP」にも `MinQueue` がそのまま使える（使用例あり）。

### [meet_in_the_middle.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/meet_in_the_middle.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `subset_sums(a)` | 部分集合の和 2^n 個（昇順） |
| 本体B | `max_subset_sum_le(a, K)`, `count_subset_sum_le(a, K)` | 和が K 以下の最大・個数。N ≤ 40 で O(2^(N/2) N) |
| 本体C | `knapsack_mitm(w, v, W)` | N ≤ 40、W が大きいナップサック |

---

## graph/ — グラフ

| ファイル | 何をするか | 自分で定義するもの |
|---|---|---|
| [`graph_input.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/graph_input.cpp) | よくある入力形式（辺のリスト、木、親の列、重み付き、グリッド）を読んで隣接リストにする | なし |
| [`bfs.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp) | BFS（多始点・経路復元）、グリッド BFS、01-BFS | 隣接リスト（グリッドなら壁の文字） |
| [`dfs.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dfs.cpp) | DFS（行きがけ・帰りがけ順）、連結成分、有向・無向グラフの閉路検出と復元 | 汎用 DFS のみ `on_enter`, `on_exit`（ラムダ） |
| [`dijkstra.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dijkstra.cpp) | ダイクストラ（経路復元）、拡張ダイクストラ、最短路の本数・最短路 DAG、k 番目最短路 | 拡張時のみ「状態 → 整数 id」と隣接の列挙ラムダ |
| [`topological_sort.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/topological_sort.cpp) | トポロジカルソート、閉路検出、DAG 上の最短路・最長路（重み付きも） | なし |
| [`kruskal.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/kruskal.cpp) | 最小全域木（クラスカル法・プリム法・ブルーフカ法）、最大全域木 | プリム法は `cost(i, j)`、ブルーフカ法は `cheapest(comp)`（ラムダ） |
| [`shortest_path.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/shortest_path.cpp) | ベルマンフォード（負の辺・負閉路）、ワーシャルフロイド（全点対）、Johnson 法（負の辺がある全点対をダイクストラで） | なし |
| [`bipartite.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bipartite.cpp) | 二部グラフ判定（2 彩色）、成分ごとの色の個数 | なし |
| [`bipartite_matching.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bipartite_matching.cpp) | 二部マッチング（Hopcroft-Karp）、最小頂点被覆 | なし |
| [`functional_graph.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/functional_graph.cpp) | 各頂点から辺が 1 本のグラフ。K 回移動した先、ダブリング（値の和も）、閉路分解 | なし |
| [`weighted_union_find.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/weighted_union_find.cpp) | 「b は a より w 大きい」を追加していき、矛盾判定と 2 点の差を求める | なし |
| [`lowlink.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/lowlink.cpp) | 橋・関節点、二重辺連結成分分解、二重頂点連結成分分解・Block-Cut Tree | なし |
| [`euler_path.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/euler_path.cpp) | オイラー路・オイラー閉路（すべての辺を 1 回ずつ通る一筆書き）。有向・無向 | なし |
| [`rollback_union_find.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/rollback_union_find.cpp) | ロールバック付き UnionFind（undo・ある時点まで戻す、成分ごとの辺の本数） | なし |
| [`persistent_union_find.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/persistent_union_find.cpp) | 部分永続 UnionFind（過去の時刻でつながっていたか・成分の大きさ、初めてつながった時刻） | なし |
| [`chromatic_number.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/chromatic_number.cpp) | 彩色数（隣り合う頂点を違う色にする最小の色数。N ≤ 20 程度） | なし |
| [`steiner_tree.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/steiner_tree.cpp) | 最小シュタイナー木（指定した K 個の頂点をつなぐ最小の木。K ≤ 10 程度） | なし |
| [`hungarian.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/hungarian.cpp) | 割当問題（ハンガリアン法。N 人に仕事を 1 つずつ、コスト最小） | なし |
| [`general_matching.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/general_matching.cpp) | 一般グラフの最大マッチング（Edmonds の花アルゴリズム） | なし |
| [`min_arborescence.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/min_arborescence.cpp) | 最小全域有向木（根から全頂点へ行ける有向の木の最小の重み） | なし |
| [`dominator_tree.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dominator_tree.cpp) | 支配木（根から v へのどの道も通る頂点） | なし |
| [`scc_dag.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/scc_dag.cpp) | 強連結成分分解（SCC）と縮約 DAG（成分はトポロジカル順） | なし（ACL の scc_graph を使う） |

### [graph_input.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/graph_input.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `read_graph(n, m, directed, one_indexed)` | 「a b」が m 行 → 隣接リスト（既定は無向・1-indexed 入力） |
| 削除可 | `read_tree(n)`, `read_parents(n)` | 辺 n−1 本の木 / 「p_2 … p_n」形式の木 |
| 本体B | `read_wgraph<T, E>(n, m, ...)` | 「a b c」が m 行 → 重み付き隣接リスト。`E = WEdge<T>` で dijkstra にそのまま渡せる |
| 本体C | `read_grid(h)` | h 行の文字列 |

### [bfs.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `bfs(g, s)` / `bfs(g, {s1, s2})` | 単一始点・多始点。`dist[v]`（到達不能 -1） |
| 削除可 | `path(t)` | 経路復元 |
| 本体B | `grid_bfs(S, {{r, c}}, wall, diag)` | グリッド上の BFS。壁の文字と 8 方向移動を指定可 |
| 本体C | `bfs01(g, s)` / `bfs01_func(状態数, 始点, adj)` | 01-BFS（辺の重みが 0 か 1）。`g[a]` に `{b, c}`。状態を自分で定義する版は拡張ダイクストラと同じ書き方 |

### [dfs.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dfs.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `dfs_walk(g, s, seen, on_enter, on_exit)` | 汎用 DFS（再帰なし）。入るとき・出るときに好きな処理を挟む |
| 削除可 | `connected_components(g)` | 連結成分の番号（本体A が必要） |
| 本体B | `find_cycle_directed(g)` | 有向グラフの閉路の頂点列（なければ空） |
| 本体C | `find_cycle_undirected(g)` | 無向グラフの閉路の頂点列（多重辺も可、なければ空） |

**定義**（本体A）: `on_enter(v, p)` は v に初めて入ったとき（行きがけ、p は直前の頂点、始点は −1）、`on_exit(v, p)` は v から先をすべて調べ終わったとき（帰りがけ）。何もしないなら `[](int, int) {}`。
`seen` を使い回して全頂点から順に呼ぶと、連結成分ごとの処理になる。使用例に「行きがけ順・帰りがけ順・親・部分木サイズを一度に求める」例あり。

### [dijkstra.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dijkstra.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `WGraph<T>`, `dijkstra(g, s)` | 重み付き最短路（重みは非負）。`dist[v]`（到達不能は INF）, `reachable(v)` |
| 本体 | `dijkstra_func<T>(状態数, 始点, adj)` | 拡張ダイクストラ。頂点を「(頂点, 状態)」に拡張したグラフで最短路 |
| 削除可 | `path(t)` | 経路復元 |
| 削除可 | `count_shortest_paths<C>(g, s)` | 最短路の本数（`C` は mint など）。`{dijkstra の結果, cnt}` を返す。重みは正 |
| 削除可 | `shortest_path_dag(g, dist)` | 最短路 DAG（`dist[u] + w == dist[v]` の辺 u → v だけの隣接リスト） |
| 削除可 | `kth_shortest(g, s, K)` | `res[v]` = s から v への道の長さを短い順に K 個。同じ頂点・辺を何度通ってもよい（辺の並びが違えば別の道）。重みは正 |

拡張時は「状態 → 整数 id」（例: `id = v * (K + 1) + k`）と、`adj(id, relax)` の中で行き先ごとに `relax(行き先id, コスト)` を呼ぶラムダを書く。使用例に「クーポン K 枚」の例あり。

### [topological_sort.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/topological_sort.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `topological_sort(g)` / `(g, true)` | 辺の向きに矛盾しない並び（true で辞書順最小）。閉路があれば空 |
| 削除可 | `dag_longest_path(g)` | 各頂点で終わる最長パスの辺数 |
| 削除可 | `dag_path<T>(g, s, maximize)` | 重み付き DAG の最短路（true で最長路）。負の重み OK。`ok`（閉路なし）, `dist`, `reachable(v)`, `path(t)`。`s = -1` で始点自由 |

`dag_path` の `g` は `vector<vector<pair<int, T>>>`（`g[v] = {{行き先, 重み}, ...}`）。ふつうのグラフの最長路は難しいが、DAG ならトポロジカル順に 1 回ずつ緩和するだけで求まる。

### [shortest_path.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/shortest_path.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `bellman_ford(n, es, s)` | 負の辺がある最短路 O(NM)。`dist[v]` は到達不能で `INF`、負閉路でいくらでも小さくなるなら `NEG_INF`。`neg_cycle` |
| 本体B | `wf_matrix<T>(n)` → `warshall_floyd(d)` | 全点対の最短路 O(N^3)。負閉路があれば true を返す |
| 削除可 | `wf_add_edge(d, a, b, c)` | 計算済みの d に辺を 1 本追加して O(N^2) で更新 |
| 本体C | `johnson(n, es)` | 全点対の最短路 O(NM log N)。負の辺 OK、負閉路があれば空。到達不能は `WF_INF` と同じ値 |

辺は `vector<BEdge<long long>> es; es.push_back({a, b, c});`（有向）。負の辺がないならダイクストラの方が速い。

Johnson 法の考え方（ポテンシャル）: 各頂点に値 h を決めて辺の重みを `w + h[u] − h[v]` に変えても、最短路そのものは変わらず、長さが `h[s] − h[t]` ずれるだけ。h をうまく選ぶと全部の重みが 0 以上になり、ダイクストラが使える。h が問題から最初にわかる（高さなど）なら、ベルマンフォードなしでダイクストラ 1 回で済む。

### [bipartite.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bipartite.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `bipartite_coloring(g)` | 各頂点の色 0 / 1。二部グラフでなければ空 |
| 削除可 | `bipartite_components(g)` | 連結成分ごとの {色 0 の数, 色 1 の数}（「片側を選ぶ」系の部分和 DP 用） |

### [bipartite_matching.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bipartite_matching.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `BipartiteMatching bm(L, R)`, `add_edge(l, r)`, `solve()` | 最大マッチングの本数 O(E√V)。`match_l[l]`, `match_r[r]` で相手 |
| 削除可 | `min_vertex_cover()` | 最小頂点被覆 {左, 右}（大きさ = 最大マッチング）。補集合が最大独立集合 |

グリッドの隣り合うマスの組（ドミノ）は、(i + j) の偶奇で左右に分ける。

### [functional_graph.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/functional_graph.cpp)
「頂点 v の次は f[v]」で決まる移動（テレポーター、置換を K 回など）。K は 1e18 でもよい。

| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `walk_k(f, s, K)` | s から K 回移動した先。閉路を見つけて K を割る O(N) |
| 本体B | `Doubling db(f, maxK, w)` | `jump(v, k)` 全頂点から K 回移動した先、`jump_sum(v, k)` 通った頂点の w の和も。各 O(log K) |
| 本体C | `FunctionalGraph fg(f)` | `cycle_id[v]`（閉路上か）、`cycles`、`dist[v]`（閉路までの距離）、`entry[v]`（最初に着く閉路の頂点） |

### [kruskal.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/kruskal.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `kruskal(n, es)` / `kruskal(n, es, true)` | 最小全域木 / 最大全域木。`cost`, `used`（採用した辺の番号）, `connected` |
| 削除可 | `prim_dense<T>(n, cost)` | プリム法 O(N^2)。全頂点の組に辺がある密なグラフ用。`cost`, `par[v]`（木での親、根は 0）, `connected` |
| 削除可 | `boruvka<T>(n, cheapest)` | ブルーフカ法。`cheapest(comp)` が「各頂点から別の成分への一番安い辺」を返せればよい。`cost`, `connected`, `edges` |

辺は `vector<KEdge<long long>> es; es.push_back({a, b, w});`。`#include <atcoder/dsu>` が必要。全頂点がつながらないときは `connected == false`（最小全域森の結果）。

使い分け: 辺のリストがあるならクラスカル。「全部の点の組をつなげる（コスト = 距離など）」で辺が N^2 本になるときは、辺を作らずに `prim_dense` に `cost(i, j)` を渡す（辺がない組は `PRIM_INF<T>`）。N ≤ 5000 くらいまで。N がもっと大きく、重みが式で決まっていて「自分と違う成分の中で一番安い相手」をソートやセグ木で速く探せるならブルーフカ法（cheapest を呼ぶのは O(log N) 回）。

### [scc_dag.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/scc_dag.cpp) — 強連結成分と縮約 DAG
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `SCCDag sd(n, edges)` | `k`（成分数）, `comp[v]`, `groups[c]`, `dag[c]`（重複なし）, `self_loop[c]`（成分の中をぐるぐる回れるか） |

成分番号はトポロジカル順（辺 a → b があれば `comp[a] <= comp[b]`）なので、縮約 DAG の DP は `for (c = k − 1; c >= 0; c--)` で「後ろから」書ける。閉路のある有向グラフで「到達できる頂点の数」「無限に歩けるか」などを求めるときに使う。`#include <atcoder/scc>` が必要。

### [weighted_union_find.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/weighted_union_find.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `WeightedUnionFind<T> uf(n)` | ポテンシャル付き UnionFind |
| 本体 | `merge(a, b, w)` | 「weight(b) − weight(a) = w」を追加。既存の情報と矛盾したら false |
| 本体 | `same(a, b)`, `diff(a, b)`, `size(a)` | 同じグループか、weight(b) − weight(a)、グループの大きさ |

典型: 「x_b − x_a = w」という条件が M 個あり、全部を同時に満たせるかを判定する。普通の連結判定だけなら ACL の `dsu` を使う。

### [rollback_union_find.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/rollback_union_find.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `RollbackUnionFind uf(n)`, `merge(a, b)` | つなぐ（同じ成分どうしでも 1 回として記録） O(log N) |
| 本体 | `undo()`, `snapshot()`, `rollback(t)` | 直前の merge を取り消す、時点 t まで戻す |
| 本体 | `leader`, `same`, `size`, `edges(a)`, `components` | 根、同じ成分か、成分の頂点数、成分の辺の本数、成分の数 |

木を DFS しながら「行きで merge、帰りで undo」が典型。取り消しが要らないなら ACL の dsu の方が速い。

### [persistent_union_find.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/persistent_union_find.cpp) — 部分永続 UnionFind
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `PersistentUnionFind uf(n)`, `unite(a, b)` | 時刻を 1 進めてつなぐ（`now` = unite を呼んだ回数）。O(log N) |
| 本体 | `find(x, t)`, `same(a, b, t)`, `size(x, t)` | 時刻 t（t 回目の unite の直後）での根・つながっていたか・成分の大きさ。O(log N) |
| 本体 | `first_time(a, b)` | a と b が初めてつながった時刻（まだなら −1）。O(log Q log N) |

「何回目の操作で初めて〇〇になるか」を、クエリごとに時刻で二分探索して答えるのが典型。ロールバック付きとの違い: こちらは過去を「見る」だけで戻せないが、どの時刻でもいつでも聞ける。

### [chromatic_number.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/chromatic_number.cpp) — 彩色数
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `chromatic_number(g)` | 彩色数。O(2^N N)、N ≤ 20 程度（自己ループがあれば −1） |

包除原理で「独立集合を k 個選んで全頂点を覆う選び方」を数え、0 でない最小の k を答える。数が大きいので 2 つの素数で mod を取って判定している。

### [steiner_tree.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/steiner_tree.cpp) — 最小シュタイナー木
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `steiner_tree<T>(n, edges, terms)` → `steiner_min(dp)` | ターミナル（K 個）を全部つなぐ最小の重み。O(3^K N + 2^K M log N)。つなげなければ `STEINER_INF<T>` |

`dp[S][v]` = 「ターミナルの集合 S と頂点 v をつなぐ最小」もそのまま使える（使用例の ABC364 G）。

### [hungarian.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/hungarian.cpp) — 割当問題
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `hungarian(a)` | N × M（N ≤ M）のコスト行列で、人ごとに違う仕事を割り当てる最小コスト `{cost, match}`。O(N^2 M) |

最大化は a を −1 倍。全員が全部の仕事をできる（密な）なら最小費用流より速くて簡単。

### [general_matching.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/general_matching.cpp) — 一般グラフの最大マッチング
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `GeneralMatching gm(n)`, `add_edge(a, b)`, `solve()` | 最大マッチングの大きさ。`mate[v]` = 相手（いなければ −1）。O(N^3) |

二部グラフなら `bipartite_matching.cpp` の方が速い。奇数長の閉路（花）があるグラフでも正しく動くのがこちら。

### [min_arborescence.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/min_arborescence.cpp) — 最小全域有向木
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `min_arborescence<T>(n, edges, root)` | `{ok, cost}`。root から全頂点へ行ける有向の木の最小の重み（Chu-Liu/Edmonds、O(NM)）。負の重み OK |

どの辺を使うか（各頂点の親）までは求めない。無向なら普通の最小全域木（クラスカル）でよい。

### [dominator_tree.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dominator_tree.cpp) — 支配木
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `dominator_tree(g, root)` | `idom[v]` = v の直接支配点（root は root、行けない頂点は −1）。O(M log N) |

支配木で u が v の祖先 ⇔ root から v へのどの道も u を通る（u を壊すと v に行けなくなる）。

### [lowlink.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/lowlink.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `LowLink ll(n)`, `add_edge(a, b)` → `build()` | 無向グラフ（多重辺・非連結 OK）。`add_edge` は辺番号を返す |
| 本体 | `bridges`, `is_bridge[e]` | 橋（取り除くと分断される辺） |
| 本体 | `articulation`, `is_art[v]` | 関節点（取り除くと分断される頂点） |
| 削除可 | `two_edge_cc()` | 二重辺連結成分分解（橋を全部取り除いたときの連結成分番号） |
| 削除可 | `bcc()` | 二重頂点連結成分（ブロック）ごとの頂点の一覧。関節点は複数のブロックに入る。build 不要 |
| 削除可 | `block_cut_tree()` | 頂点 0..n−1 = 元の頂点、n.. = ブロックの森。関節点 v を取り除くと、v のいた成分は `t[v].size()` 個に分かれる |

### [euler_path.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/euler_path.cpp) — オイラー路（一筆書き）
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `euler_path(n, edges, directed, s = -1)` | `ok`（一筆書きできるか）、`vs`（通る頂点、長さ M + 1）、`es`（通る辺の番号、長さ M）。`s` で始点を指定 |

できる条件: 辺のある頂点が全部つながっていて、無向なら次数が奇数の頂点が 0 個か 2 個、有向なら全頂点で入次数 = 出次数か、出 − 入 = 1 と 入 − 出 = 1 の頂点が 1 つずつ。閉路なら `vs.front() == vs.back()`。多重辺・自己ループ OK。O(N + M)。

---

## tree/ — 木

### [tree.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `add_edge(a, b, w)` → `build(root)` | 木を作って根から前計算 |
| 本体 | `par, dep, wdep, sz, children, order` | 親・深さ・重み付き深さ・部分木サイズ・子・BFS 順 |
| 本体 | `lca(a, b)`, `dist`, `wdist`, `kth_ancestor(v, k)` | 最小共通祖先、距離、k 個上の祖先 |
| 削除可 | `jump(a, b, k)`, `path(a, b)`, `is_on_path(a, b, c)` | パス上で k 歩進んだ頂点、パスの頂点列、c がパス上か |
| 削除可 | `diameter()` | 直径の長さと端点間の頂点列（build 不要） |
| 削除可 | `TreeImos<U, T>(t)` | 木上のいもす法。`add_subtree(v, x)` 部分木加算、`add_path(u, v, x)` パス加算、`build()` |

木 DP は `order` を逆順に回すと「子 → 親」の順になる（使用例あり）。

### [tree_dp.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree_dp.cpp) — 木 DP のテンプレート
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `TreeDP<S> td(n)`, `add_edge(a, b, w)`, `td.solve(root, e, merge, put_edge, put_vertex)` | `dp[v]` = v を根とする部分木の DP 値。再帰なし |

自分で決める 4 つは全方位木 DP（rerooting.cpp）と同じ。根が決まっている問題はこちら、全頂点を根にした答えが要るなら rerooting。使用例に「部分木のサイズ」「高さ」「独立集合の数え上げ」。

### [hld.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/hld.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `HLD hld(g, root)` | 頂点に番号 `pos[v]` を振り直す |
| 本体 | `path(u, v, edge)` | u-v パスを O(log N) 個の区間 `[l, r)` に。`edge = true` で LCA を除く（辺の値は子の `pos` に置く） |
| 本体 | `subtree(v)`, `lca(u, v)`, `dist(u, v)` | 部分木の区間、LCA、距離 |

区間は ACL の segtree / lazy_segtree / fenwick_tree で処理する（使用例はパスの最大値を segtree で）。

### [centroid.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/centroid.cpp) — 重心分解
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `CentroidDecomposition cd(g)` | 重心木を作る。`cpar[v]`（重心木の親）、`level[v]`（深さ、log N 以下）、`root` |
| 本体 | `dist[v][k]` | v から、重心木で深さ k の祖先の重心までの距離 |
| 削除可 | `CentroidCount cc(cd, w)`, `cc.count(v, k)` | v から距離 k 以内の頂点の数（重み w を渡せば重みの和）。前計算 O(N log N)、1 回 O(log N) |

u-v パスは重心木での共通の祖先を必ず通る。祖先を `for (int c = v; c != -1; c = cd.cpar[c])` でたどり、`dist[v][level[c]]` を使う。使用例に「赤く塗る / 一番近い赤い頂点までの距離」（1 回 O(log N)）。

### [euler_tour.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/euler_tour.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `EulerTour et(g, root)` | `in[v]`, `out[v]`（v の部分木 = 時刻 `[in[v], out[v])`）、`order`、`par`、`dep`、`is_ancestor(u, v)` |

部分木を列の区間にするので、ACL の fenwick_tree / segtree と組み合わせて「部分木の和」「根からのパスの和」を値の更新ありで求められる（使用例あり）。`tree.cpp` とは独立。

### [tree_hash.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree_hash.cpp) — 木の同型判定
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `TreeCanon tc;` `tc.rooted(g, r)` | 根を r にしたときの各頂点の部分木の形の番号。`id[u] == id[v]` ⇔ 部分木が同じ形 |
| 本体 | `tc.unrooted(g)` | 根なし木としての形の番号（中心を根にする） |

比べたい木はすべて同じ `tc` で番号を振ること。乱数ハッシュではなく「子の番号の列 → 番号」の表で番号を振るので、衝突はない（O(N log N)）。

### [auxiliary_tree.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/auxiliary_tree.cpp) — Auxiliary Tree（虚樹）
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `AuxiliaryTree at(g, root)` | 前計算 O(N log N)。`lca(u, v)`, `dist(u, v)`, `dep[v]` も使える |
| 本体 | `auto [vs, par] = at.build(S)` | S と、その LCA だけを残した木（2 × (S の個数) − 1 頂点以下）。`vs` は行きがけ順で `vs[0]` が根、`par[i]` は vs の中での親の位置 |

色ごと・クエリごとに指定された頂点の集合で木 DP をしたいときに使う。ΣK が N 程度なら全体で O(N log N)。DP は `i = vs.size() − 1` から 0 へ回すと子が先になる。辺の長さ（元の木での辺の本数）は `dep[vs[i]] − dep[vs[par[i]]]`。

### [rerooting.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/rerooting.cpp) — 全方位木 DP
すべての頂点 v について「v を根にしたときの木 DP の値」を O(N) でまとめて求める。`tree.cpp` とは独立。

| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `Rerooting<S> rr(n)`, `add_edge(a, b, w)` | 木を作る（重みなしなら w 省略） |
| 本体 | `rr.solve(e, merge, put_edge, put_vertex)` | `ans[v]` = v を根にしたときの DP 値 |

**定義**（すべて main の中のラムダでよい）。普通の木 DP `dp[v] = put_vertex(merge_{子 c}(put_edge(dp[c], c, v, w)), v)` の形に分解する。

| 名前 | 意味 | 例: 最遠距離 | 例: 距離の和（S = {和, 頂点数}） |
|---|---|---|---|
| `e` | 子が 0 個のときの「集めた結果」 | `0` | `{0, 0}` |
| `merge(a, b)` | 子からの値を集める（順序によらない演算） | `max(a, b)` | 成分ごとの和 |
| `put_edge(x, c, p, w)` | 子 c の部分木の値を、辺を通して親 p に渡す | `x + w` | `{x.和 + x.数 * w, x.数}` |
| `put_vertex(x, v)` | 集めた値から頂点 v の値を作る | `x` | `{x.和, x.数 + 1}` |

---

## math/ — 数学

| ファイル | 何をするか | 自分で定義するもの |
|---|---|---|
| [`integer.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp) | 繰り返し二乗（mod なし・上限付き・64 bit mod）、拡張ユークリッド、整数平方根、負数対応の割り算、進数変換 | なし |
| [`matrix.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/matrix.cpp) | 行列の積・累乗（漸化式の第 K 項、K 歩の経路数） | 要素の型（mint 推奨） |
| [`combination.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/combination.cpp) | 二項係数 nCk / nPk / nHk（mod 素数）、Lucas の定理（mod が小さい素数） | `mint` 型（ACL の modint） |
| [`prime.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp) | 篩、素数判定、素因数分解、約数列挙、区間篩、φ 関数、1e18 までの素数判定・素因数分解（ミラー・ラビン・ポラード・ロー） | なし |
| [`xor_basis.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/xor_basis.cpp) | XOR 基底（xor で作れるか、作れる最大値、作れる値の種類数） | なし |
| [`divisor_transform.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/divisor_transform.cpp) | メビウス関数、約数・倍数のゼータ / メビウス変換（gcd がちょうど d の組を数える）、gcd 畳み込み | なし |
| [`gauss.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/gauss.cpp) | ガウスの消去法: mod p の連立一次方程式（解の個数・基底・階数）、行列式、F2（xor）の連立方程式、行列木定理 | なし |

### [integer.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `ipow(a, n)`, `ipow_cap(a, n, cap)`, `pow_mod64(a, n, m)` | a^n（mod なし）、min(a^n, cap)（あふれない）、a^n mod m（m ~ 1e18 まで） |
| 本体B | `ext_gcd(a, b, x, y)` | gcd を返し、`a*x + b*y = gcd` となる x, y を求める |
| 本体C | `isqrt(n)`, `floor_div(a, b)`, `ceil_div(a, b)`, `mod_pos(a, m)` | 誤差なしの平方根の切り捨て、負数でも正しい切り捨て・切り上げ除算と mod |
| 本体D | `to_base(x, b)`, `from_base(digits, b)`, `to_base_str(x, b)` | 進数変換（桁の列は上の桁から）。b <= −2 の負の進数も |

「a^n が X を超えるか」は `ipow_cap(a, n, X + 1) > X` で判定できる。

### [matrix.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/matrix.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `Matrix<T> A(n, m)`, `identity(n)`, `A * B`, `A.pow(k)`, `A.apply(v)` | 行列の積・K 乗（O(N^3 log K)）・ベクトルとの積 |

漸化式 `a[i+2] = a[i+1] + a[i]` は `(a[k+1], a[k]) = A^k (a[1], a[0])`（A = [[1,1],[1,0]]）。使用例にフィボナッチと 3 項間漸化式あり。

### [divisor_transform.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/divisor_transform.cpp) — メビウス関数・ゼータ / メビウス変換
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `mobius_table(N)`, `mobius(n)` | メビウス関数 μ（平方数で割り切れるなら 0、そうでなければ (−1)^(素因数の個数)）。表 O(N) / 1 個 O(√n) |
| 本体B | `divisor_zeta(a)`, `divisor_mobius(a)` | `a[n] ← Σ_{d∣n} a[d]`（約数での和）とその逆。O(N log log N) |
| 本体B | `multiple_zeta(a)`, `multiple_mobius(a)` | `a[d] ← Σ_{d∣n} a[n]`（倍数での和）とその逆 |
| 削除可 | `gcd_convolution(a, b)` | `c[k] = Σ_{gcd(i, j) = k} a[i] b[j]` |

`a` は `a[0..N]`（`a[0]` は使わない）で、その場で書き換える。よく使う形: `f[d]` =「全部 d の倍数」の数（`(K / d)^N` など、数えやすい）を作って `multiple_mobius(f)` すると、`f[d]` =「gcd がちょうど d」の数になる。

### [xor_basis.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/xor_basis.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `XorBasis xb`, `insert(x)` | x を追加。基底が増えたら true |
| 本体 | `can_make(x)`, `max_xor(x0)`, `rank()` | x を作れるか、x0 と xor した最大値、基底の数（作れる値は 2^rank 種類） |

### [combination.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/combination.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `Combination<mint> cb(N)` | 階乗テーブル（自動拡張）。`C`, `P`, `H`, `fact`, `ifact`, `inv` |
| 削除可 | `comb_large_n<mint>(n, k)` | n が巨大（~1e18）で k が小さいときの nCk |
| 削除可 | `pascal(n)` | mod なしの二項係数表（n ≤ 66、ACL 不要） |
| 削除可 | `Lucas lu(p)`, `lu.C(n, k)` | mod が小さい素数 p（2, 3 など）のときの nCk。n, k は 1e18 まで。前計算 O(p) |

`Combination` は n < mod でしか使えない（n ≥ mod だと n! ≡ 0）。mod が小さいときは Lucas を使う。

### [prime.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体A | `Sieve sv(N)` | N 以下の篩。`primes`, `is_prime(x)`, `factorize(x)` を高速に |
| 削除可 | `sv.divisors(x)` | 約数列挙 |
| 本体B | `is_prime(x)`, `factorize(x)`, `divisors(x)` | 1 個の数に対して O(√x) |

| 本体C | `segment_sieve(L, R)` | 区間篩。[L, R) の素数判定（L, R ~ 1e12、R − L ~ 1e6） |
| 本体D | `euler_phi(n)`, `phi_table(N)` | オイラーの φ 関数（n と互いに素な 1..n の個数）。1 個 O(√n) / 表 |
| 本体E | `is_prime_mr(x)`, `factorize_fast(x)` | ミラー・ラビン素数判定（2^63 未満で必ず正しい）、ポラード・ロー素因数分解（1e18 でも一瞬） |

使い分け: たくさんの数（≤ 1e7）を処理するなら本体A、1 個の大きい数（≤ 1e12 程度）なら本体B、それより大きい数（≤ 1e18）や大きい数がたくさんなら本体E。

### [gauss.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/gauss.cpp) — ガウスの消去法
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `gauss_solve<mint>(A, b)` | Ax = b。`ok`（解があるか）, `x`（解の 1 つ）, `rank`, `basis`（Ax = 0 の解の基底）。解の個数 = p^(M − rank) |
| 削除可 | `determinant<mint>(A)` | 行列式 O(N^3) |
| 本体B | `gauss_f2<W>(A, b, M)` | F2（0/1 と xor）の連立方程式。`A` は `vector<bitset<W>>`（W > M）。`ok`, `x`, `rank`。解の個数 = 2^(M − rank) |
| 削除可 | `count_spanning_trees<mint>(n, edges)` | 行列木定理: 無向グラフの全域木の個数（多重辺は別々に数える） |
| 削除可 | `count_arborescences<mint>(n, edges, root)` | 有向グラフで root から全頂点へ向かう有向全域木の個数 |

F2 版は「スイッチ j を押すとランプ i が反転」型の問題（`A[i][j] = 1`、`b[i]` = ランプ i を反転させたいか）。xor の基底だけでよいなら `xor_basis.cpp` の方が軽い。

---

## string/ — 文字列

| ファイル | 何をするか | 自分で定義するもの |
|---|---|---|
| [`rolling_hash.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/rolling_hash.cpp) | 部分文字列のハッシュを O(1)、部分文字列の一致判定、LCP | なし |
| [`kmp.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/kmp.cpp) | KMP（パターンの出現位置をすべて）、失敗関数、最小周期、KMP オートマトン（パターンを含まない文字列の数え上げ） | なし |
| [`manacher.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/manacher.cpp) | Manacher（各中心の最長回文）、区間の回文判定 O(1)、最長回文、回文の個数、回文分割 | なし |
| [`palindrome.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/palindrome.cpp) | 回文の小物: 書き換えて回文にする最小回数、並べ替えて回文にできるか、最長回文部分列 | なし |
| [`bracket.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/bracket.cpp) | 括弧列: 正しいかの判定（何種類でも）、最小の補い方、最長の正しい部分、区間の判定 O(1)、'?' の置き方の数 | なし |
| [`subsequence.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/subsequence.cpp) | 部分列: 次の文字の位置の表（部分列か・部分列でない最短の文字列）、種類の違う部分列の数、辞書順最小の部分列 | なし（文字の種類を変えるなら `Subsequence<26, 'A'>` など） |
| [`suffix_array.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/suffix_array.cpp) | 接尾辞配列（ACL）の使い方: 部分文字列の種類数・k 番目・出現回数、2 つの接尾辞の LCP O(1)、最長共通部分文字列 | なし |
| [`trie.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/trie.cpp) | Trie（接頭辞の個数、他の文字列との最長共通接頭辞、全ペアの最長共通接頭辞の和） | なし（文字の種類を変えるなら `Trie<26, 'A'>` など） |
| [`aho_corasick.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/aho_corasick.cpp) | Aho-Corasick（たくさんのパターンをまとめて検索、パターンを含まない文字列の DP） | なし（文字の種類を変えるなら `AhoCorasick<26, 'A'>` など） |

### [rolling_hash.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/rolling_hash.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `RollingHash rh(s)` | 前計算 O(N)。`s` は `string` でも `vector<int>` でもよい |
| 本体 | `rh.get(l, r)` | `s[l..r)` のハッシュ値。値が等しければ部分文字列が等しい |
| 削除可 | `rh.lcp(a, b)` | `s[a..)` と `s[b..)` の最長共通接頭辞の長さ O(log N) |

- mod 2^61 − 1、基数は実行ごとにランダム（ハック対策）
- 基数は全インスタンスで共有しているので、別の文字列どうしのハッシュも比較できる
- 回文判定: 反転文字列 `rev` でも作り、`s[l..r)` と `rev[n−r..n−l)` を比較（使用例あり）

### [kmp.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/kmp.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `prefix_function(s)` | 失敗関数 `pi[i]`（s[0..i] の「接頭辞かつ接尾辞」の最長） |
| 削除可 | `kmp_search(t, p)` | t の中の p の出現位置すべて O(\|t\| + \|p\|) |
| 削除可 | `min_period(s)` | 最小周期。「s が繰り返しか」は `n % d == 0 && d < n` |
| 削除可 | `kmp_automaton(p, sigma, base)` | `nx[k][c]` = 「p の先頭 k 文字まで一致」の状態で文字 c を読んだ後の状態（0..\|p\|）。DP の状態遷移に使う |
| 削除可 | `count_avoid<mint>(p, N, sigma)` | 長さ N の文字列で p を含まないものの数 O(N \|p\| sigma)。含むものは sigma^N − これ |

パターンが複数なら `aho_corasick.cpp`。N が 1e18 などと大きいときは、`nx` から遷移行列を作って行列累乗（`math/matrix.cpp`）。

### [manacher.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/manacher.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `Palindrome pal(s)` | 前計算 O(N) |
| 本体 | `is_pal(l, r)`, `longest()`, `count()` | 区間の回文判定 O(1)、最長回文 {開始, 長さ}、回文の部分文字列の個数 |
| 削除可 | `pal_partition_min(s)`, `pal_partition_count<mint>(s)` | 回文（連続）に区切る最小個数・区切り方の数 O(N^2) |

### [palindrome.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/palindrome.cpp) — 回文の小物
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `pal_min_changes(s)` | 1 文字ずつ書き換えて回文にする最小回数（対応する 2 文字が違う組の数） |
| 本体A | `can_rearrange_pal(s)` | 並べ替えて回文にできるか（奇数回出る文字が 1 種類以下） |
| 本体B | `longest_pal_subseq(s)` | 最長回文部分列（飛び飛びでよい）の長さ O(N^2)、メモリ O(N) |

最小何文字「消す」または「挿入する」と回文になるかは、どちらも N − `longest_pal_subseq(s)`。「長さ N の回文で〇〇を満たすものの数」は、前半分 ⌈N/2⌉ 文字だけ決めればよいので、前半分で数える（桁 DP と組み合わせることも多い）。

### [bracket.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/bracket.cpp) — 括弧列
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `is_balanced(s, open, close)` | 正しい括弧列か。何種類でもよい（例: `is_balanced(s, "([<", ")]>")`） |
| 本体A | `bracket_fix(s)` | `{a, b}`: 先頭に `(` を a 個、末尾に `)` を b 個足すと正しくなる（最短・辞書順最小） |
| 本体B | `longest_balanced(s)` | 正しい括弧列になっている最長の部分文字列 {開始, 長さ} |
| 本体C | `BracketRange br(s)`, `br.ok(l, r)` | `s[l..r)` が正しい括弧列か O(1) |
| 本体D | `count_bracket_fill<mint>(s)` | `?` を `(` か `)` にして正しい括弧列になる置き方の数 O(N^2) |

基本: `(` を +1、`)` を −1 とした累積和が「途中で負にならず、最後に 0」なら正しい。文字の書き換えがあって区間を何度も聞かれるなら、{和, 累積和の最小} を持つ ACL の segtree にする。長さ 2n の正しい括弧列の数はカタラン数 C(2n, n) / (n + 1)。

### [subsequence.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/subsequence.cpp) — 部分列
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体A | `Subsequence<> sub(s)`, `nxt(i, c)` | i 以降で文字 c が最初に出る位置（なければ N）。前計算 O(26N) |
| 本体A | `is_subseq(t)`, `match(t, i)`, `end_of(t, i)` | t が部分列か、s[i..) で t の先頭から何文字とれるか、t を部分列として含む最短の区間 [i, e) の e |
| 本体A | `shortest_absent()` | s の部分列でない最短の文字列（辞書順最小） |
| 本体B | `count_distinct_subseq<mint>(s)` | 種類の違う（空でない）部分列の数 |
| 本体C | `min_subseq(s, k, largest)` | 長さ k の部分列で辞書順最小（`largest = true` で最大）。「K 文字消して最小」は `min_subseq(s, N − K)` |

部分列の判定は「前から貪欲に、一番早く出てくる位置を使う」でよい。2 つの列の最長共通部分列は `dp/lcs.cpp`。

### [suffix_array.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/suffix_array.cpp) — 接尾辞配列
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `SuffixArray sa(s)` | `sa.sa`, `sa.rnk`, `sa.lcp`（ACL の suffix_array・lcp_array）と Sparse Table。O(N log N) |
| 本体 | `lcp_of(i, j)`, `compare(l1, r1, l2, r2)` | 2 つの接尾辞の最長共通接頭辞、2 つの部分文字列の辞書順比較。どちらも O(1) |
| 本体 | `count_distinct()`, `longest_repeated()` | 種類の違う部分文字列の数、2 回以上現れる最長の部分文字列 |
| 本体 | `kth_substring(k)`, `find(t)` | 辞書順 k 番目の部分文字列（種類の違うもの）、t の出現範囲 [lo, hi)（出現回数 = hi − lo） |
| 削除可 | `longest_common_substring(s, t)` | 2 つの文字列の最長共通部分文字列 {s での開始, t での開始, 長さ} |

部分文字列の比較・一致判定だけならローリングハッシュでもできる。接尾辞配列は衝突がなく、「辞書順」が絡む問題に強い。

### [trie.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/trie.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `Trie<> tr`, `insert(s)`, `erase(s)` | 登録・削除 O(\|s\|)（同じ文字列を何回でも登録できる） |
| 本体 | `count(s)`, `count_prefix(p)`, `nodes[v]` | s の個数、p で始まる文字列の個数、頂点の情報（子・通る数・終わる数・深さ） |
| 削除可 | `lcp_with_others(s)` | s と他の文字列との最長共通接頭辞 |
| 削除可 | `sum_lcp_pairs()` | 全ペアの最長共通接頭辞の和 |

### [aho_corasick.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/aho_corasick.cpp) — Aho-Corasick
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `AhoCorasick<> ac`, `add(s)` → `build()` | パターンを全部入れてから build。O(Σ\|パターン\| × 文字の種類) |
| 本体 | `count_each(t)`, `count_all(t)` | 各パターンが t に何回出るか（重なりも数える）、その合計。O(\|t\|) |
| 本体 | `next(v, c)`, `cnt[v]`, `size()` | 状態 v で文字 c を読んだ次の状態（根は 0）、v で終わるパターンの個数。DP の状態に使う |

「どのパターンも含まない文字列を数える」は、`dp[状態]` を持って `next` で遷移し、`cnt[次の状態] > 0` の遷移を捨てる（使用例あり）。

---

## dp/ — DP

| ファイル | 何をするか | 自分で定義するもの |
|---|---|---|
| [`knapsack.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/knapsack.cpp) | ナップサック（0-1、価値で DP、個数制限なし、個数制限あり）、部分和問題 | なし |
| [`lcs.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/lcs.cpp) | LCS（復元付き）、編集距離 | なし |
| [`lis.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/lis.cpp) | LIS（最長増加部分列）。狭義・広義、各位置で終わる長さ、復元 | なし |
| [`digit_dp.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/digit_dp.cpp) | 桁 DP（0 以上 N 以下で条件を満たす数の個数。N は何桁でもよい） | 状態の数 `S`、`init`、遷移 `trans`（ラムダ） |
| [`interval_dp.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/interval_dp.cpp) | 区間 DP（区間を 2 つに分ける形。スライムの合体、行列の積の順番） | `leaf`, `cost`（ラムダ） |
| [`tsp.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/tsp.cpp) | bit DP の巡回セールスマン（全頂点を回る最短の一周・道、回り方の復元） | 距離の表 `d[i][j]` |
| [`expected_value.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/expected_value.cpp) | 期待値 DP・確率 DP の計算の枠（後ろから・前から・閉路あり）と例題集 | 遷移 `adj(i, emit)`、1 回のコスト `cost(i)`（ラムダ） |
| [`monotone_minima.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/monotone_minima.cpp) | Monotone Minima（各行の最小値の位置が単調な行列の全行の最小値）、凸な列との (min, +) 畳み込み | 行列の成分 `f(i, j)`（ラムダ） |
| [`alien_dp.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/alien_dp.cpp) | Alien DP（「ちょうど K 個」の制約をペナルティ λ に変えて二分探索） | 個数自由の DP `h(λ)`（ラムダ） |

### [knapsack.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/knapsack.cpp)
| 区分 | 関数 | いつ使うか | 計算量 |
|---|---|---|---|
| 本体A | `knapsack_01(w, v, W)` | 各品物 1 個まで、W が小さい（~1e5） | O(NW) |
| 本体B | `knapsack_01_by_value(w, v, W)` | 各品物 1 個まで、W が大きく（~1e9）価値の合計が小さい | O(NΣv) |
| 本体C | `knapsack_unbounded(w, v, W)` | 同じ品物を何個でも | O(NW) |
| 本体D | `knapsack_bounded(w, v, c, W)` | 品物 i は c[i] 個まで | O(NW log c) |
| 本体E | `subset_sum(a, S)` | 合計をちょうど s にできるか（`ok[s]`） | O(NS) |

本体A と本体C の違いは、重さのループを重い方から回すか軽い方から回すかだけ（コメントに★で説明あり）。

### [lcs.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/lcs.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `lcs(a, b)` | 最長共通部分列そのもの（長さは `.size()`）。string でも vector でも可 |
| 本体B | `edit_distance(a, b)` | 挿入・削除・置換の最小回数 |

### [lis.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/lis.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `lis_lengths(a, strict)` | `len[i]` = a[i] で終わる LIS の長さ（全体は max）。O(N log N) |
| 削除可 | `lis(a, strict)` | LIS をなす添字の列 |

`strict = true` で狭義増加（既定）、`false` で広義増加。減少列は −1 倍か反転で。

### [digit_dp.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/digit_dp.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `digit_dp<T>(N, S, init, trans)` | `cnt[s]` = 0 以上 N 以下で、上の桁から読み終えたとき状態 s になる数の個数 |

**定義**: 状態を 0..S−1 の整数で表し、`trans(s, d)` で「状態 s で次の桁 d を読んだら次の状態」を返す（−1 で除外）。x は N と同じ桁数になるよう上に 0 を詰めて読む。先頭の 0 を区別したいときは「もう始まったか」を状態に入れる（使用例あり）。L 以上 R 以下は cnt(R) − cnt(L − 1)。

### [interval_dp.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/interval_dp.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `interval_dp<T>(n, leaf, cost, minimize)` | `dp[l][r] = min_{l<m<r}(dp[l][m] + dp[m][r] + cost(l, m, r))`、`dp[i][i+1] = leaf(i)`。O(N^3) |

スライムの合体は `leaf = 0`、`cost(l, m, r) = 区間和`。行列の積の順番は `cost = d[l]·d[m]·d[r]`。この形にならない区間 DP は、ループの形（短い区間から順に）だけ真似して自分で書く。

### [tsp.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/tsp.cpp) — bit DP の巡回セールスマン
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `tsp_dp(d, s)` | `dp[mask][v]` = 訪れた集合が mask で今 v にいるときの最小コスト。O(2^N N^2)、N ≤ 16〜18 |
| 本体 | `tsp_cycle(d, dp, s)` / `tsp_path(d, dp)` | s に戻る一周の最小 / 終わりはどこでもよい道の最小（無理なら `TSP_INF<T>`） |
| 削除可 | `tsp_restore(d, dp, s, end)` | end で終わる最小の回り方（頂点の順番） |

`d[i][j]` は直接行くコスト（行けないなら `TSP_INF<T>`）。グラフ上の「K 個の特別な頂点を全部回る」問題は、先に K 個から BFS / ダイクストラして K × K の距離表を作ってから渡す。始点が自由なら、全頂点へコスト 0 で行けるダミー頂点を足してそこを s にする。

### [expected_value.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/expected_value.cpp) — 期待値 DP・確率 DP
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `ev_backward<T>(n, adj, cost)` | `E[i]` = 状態 i からゴールまでの期待値（後ろから）。遷移は番号が同じか大きい状態へ（自己ループ OK） |
| 本体B | `prob_forward<T>(n, start, adj)` | `P[i]` = start から状態 i にたどり着く確率（前から） |
| 削除可 | `ev_general<T>(n, adj, cost)` | 行ったり戻ったりする遷移でも期待値を求める（連立方程式、O(n^3)） |

期待値 DP は状態と遷移が問題ごとにばらばらなので、関数にしたのは計算の枠だけ。状態に 0..n−1 の番号をつけ、`adj(i, emit)` の中で `emit(行き先, 確率)` を呼ぶ。遷移がない状態はゴール。T は double か mint（確率 p/q は `mint(p) / q`）。自己ループ（確率 q で何も変わらない）は `E = (cost + Σ p E[次]) / (1 − q)` と移項して計算している。使用例に、すごろく・コンプガチャ（期待値の線形性で DP なしの解き方も）・確率を前から・閉路ありの酔歩の 4 題。

### [monotone_minima.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/monotone_minima.cpp) — Monotone Minima
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `monotone_minima<T>(H, W, f)` | `pos[i]` = 行 i の最小値の列（一番左）。行ごとの最小の位置が単調であること。O((H + W) log H) |
| 削除可 | `min_plus_convex(a, b)` | `c[k] = min_{i+j=k} a[i] + b[j]`。b が下に凸なら O((N + M) log) |

最小の位置が単調になる代表は Monge な行列（`f(i, j) + f(i+1, j+1) <= f(i, j+1) + f(i+1, j)`）。「凸なコストの品物と、普通の品物を合わせて合計 k 個」のような (min, +) 畳み込みに使う。

### [alien_dp.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/alien_dp.cpp) — Alien DP
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `alien_dp<T>(K, lo, hi, h)` | `h(λ)` = 1 個ごとに λ を足して個数自由で最小化した値。返り値 = ちょうど K 個のときの最小値 g(K) |

条件は g(k) が k について下に凸（最大化なら上に凸で、−1 倍して最小化に直す）。λ は [lo, hi] の整数で二分探索するので、h を O(log(hi − lo)) 回呼ぶ。凸かどうかは小さいケースで g(k) を全探索して確かめると安全。使用例に「ちょうど K 個の区間に分けて Σ(区間の和)^2 を最小」と「隣り合わない K 個の和の最大」。

---

## util/ — 提出テンプレート

### [template.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/template.cpp)
提出コードの土台。このファイルだけは main を含めて丸ごと貼る。

| 中身 | 何をするか |
|---|---|
| ヘッダ | `atcoder/all` と標準ヘッダを個別に include（`bits/stdc++.h` は使わない）。ライブラリが使うもの（queue, deque, numeric, random, optional など）は全部入れてある |
| `mint` | `atcoder::modint998244353`。問題の mod に合わせて書き換える |
| `ll`, `INF`（≈1.15e18）, `IINF`（≈1.07e9） | 型の別名と無限大。`INF` は 2 つ足してもあふれない |
| `chmin(a, b)`, `chmax(a, b)` | a を b で更新し、更新したら true |
| `DI`, `DJ`, `in_grid(i, j, h, w)` | グリッドの 4 / 8 方向と範囲判定 |
| `print_vec(v)`, `yes_no(cond)` | 配列を空白区切りで出力、Yes / No |
| `debug(x, v, ...)` | 変数名と値を標準エラーに出す。vector・pair・set・map も表示。手元で `g++ -DLOCAL` のときだけ動き、提出時は何もしない |
| `solve()` / `main()` | 高速入出力の設定済み。`solve()` の中に解答を書く |

### [r](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/r) — コンパイル・テスト実行用のスクリプト
解答を `A.cpp` に書き、同じフォルダに `r` を置いて使う（最初に 1 回 `chmod +x r`）。

| コマンド | 何をするか |
|---|---|
| `./r` | コンパイルせずに実行（前にコンパイルした `a.out` を使う） |
| `./r c` | `A.cpp` をコンパイル（`-std=gnu++20 -O2 -DLOCAL`）してから実行。`c` 以外でも何か付ければコンパイルする |

入力は `input.txt`、出力は `output.txt`、デバッグ出力（`cerr`・`debug(...)`）は `debug.txt`。実行時間と、実行時エラーなら終了コードを表示し、`output.txt` の先頭 30 行も画面に出す。
ACL（`atcoder/`）は g++ の include パスに入れておく前提（`-I` は付けていない）。

**Windows の場合は [`r.bat`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/util/r.bat) を使う。** `r` は bash 用なので、コマンドプロンプトや PowerShell では動かない。`A.cpp` と同じフォルダに `r.bat` を置き、使い方は同じ（`chmod` は不要）。

| コマンド | 何をするか |
|---|---|
| `r` / `r c`（コマンドプロンプト） | 実行だけ / コンパイルしてから実行 |
| `./r` / `./r c`（PowerShell） | 同上（`.\r c` でもよい） |

- 実行ファイルは `a.exe`。g++（MinGW など）に PATH が通っていること
- Windows はスタックが 1 MB しかなく深い再帰で落ちるので、`-Wl,--stack,268435456`（256 MB）を付けてコンパイルする
- 画面に出るメッセージは英語（.bat に日本語を書くと、文字コードの設定によっては壊れるため）


---

## ds/ — データ構造

| ファイル | 何をするか | 自分で定義するもの |
|---|---|---|
| [`bit2d.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/bit2d.cpp) | 2 次元 BIT（1 点に加算、長方形の和。更新と質問が混ざってよい） | なし |
| [`sqrt_decomposition.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/sqrt_decomposition.cpp) | 平方分割（区間加算・和・最小、点の更新 + 区間で x 未満の個数） | なし |
| [`mo.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/mo.cpp) | Mo's algorithm（更新のない区間クエリをまとめて処理） | `add`, `erase`, `out`（ラムダ） |
| [`interval_set.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/interval_set.cpp) | 区間を set（map）で管理（塗る・消す・含むか・mex） | なし |
| [`erasable_pq.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/erasable_pq.cpp) | 削除できる優先度付きキュー、上位 K 個の和、中央値 | なし |
| [`li_chao_tree.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/li_chao_tree.cpp) | Li Chao Tree（直線・線分を追加して、ある x での最小値）。DP の高速化（CHT の代わり） | なし（クエリで使う x 座標を先に渡す） |
| [`dynamic_segtree.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/dynamic_segtree.cpp) | 動的セグ木（添字が 1e18 などでも、使う場所だけノードを作る） | `S`, `op`, `e`（ACL の segtree と同じ） |
| [`wavelet_matrix.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/wavelet_matrix.cpp) | Wavelet Matrix（区間の k 番目に小さい値、値が範囲に入る個数、前後の値） | なし |
| [`persistent_segtree.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/persistent_segtree.cpp) | 永続セグ木（更新しても昔の版が残る。どの版にも区間の積） | `S`, `op`, `e`（ACL の segtree と同じ） |

### [bit2d.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/bit2d.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `BIT2D<T> bit(H, W)`, `add(i, j, x)`, `sum(r1, c1, r2, c2)` | 1 点への加算と長方形の和、どちらも O(log H log W) |

更新がないなら `basic/prefix_sum.cpp` の `PrefixSum2D`（O(1)）で十分。

### [sqrt_decomposition.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/sqrt_decomposition.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体A | `SqrtRangeAdd sd(a)`, `add(l, r, x)`, `sum(l, r)`, `get_min(l, r)` | 区間加算・区間和・区間最小 O(√N)（ACL の lazy_segtree の方が速い。書き換えて使う土台） |
| 本体B | `SqrtCountLess sc(a)`, `set(i, x)`, `count_less(l, r, x)` | 点の更新と「区間の中で x 未満の個数」 O(√N log N)。セグ木では難しい |

### [mo.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/mo.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `Mo mo(n)`, `add_query(l, r)`, `run(add, erase, out)` | 全クエリを O((N + Q)√N) で処理。左右で処理が違うなら `run4` |

**定義**: `add(i)` 位置 i が区間に入った、`erase(i)` 出た、`out(q)` 今の区間がクエリ q。区間の状態（個数など）はラムダの外の変数に持つ。値の更新があるクエリには使えない（オフライン専用）。


### [interval_set.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/interval_set.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `IntervalSet st`, `insert(l, r)`, `erase(l, r)` | [l, r) を塗る（重なり・隣接はまとめる）・消す。ならし O(log N) |
| 本体 | `contains(x)`, `get(x)`, `mex(x)`, `total`, `mp` | x が塗られているか、x を含む区間、x 以上で塗られていない最小の値、塗られた長さ、区間の一覧 |

座標は long long のまま使える（座標圧縮いらない）。整数の点 x は [x, x + 1) として入れる。

### [erasable_pq.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/erasable_pq.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体A | `ErasablePQ<T>`（最小なら `ErasablePQ<T, greater<T>>`） | `push`, `erase`（入っている値だけ）, `top`, `pop`, `size`。ならし O(log N) |
| 本体B | `TopKSum<T> tk(K)` | `insert`, `erase` で、大きい方から K 個の和 `sum` を保つ。小さい方からなら −x を入れる |
| 本体C | `MedianHeap<T>` | `push` と `median()`（偶数個なら小さい方）。追加のみ |

### [li_chao_tree.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/li_chao_tree.cpp) — Li Chao Tree
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `LiChaoTree<long long> lc(xs)` | `xs` = クエリで使う x 座標（先に全部わかっていること）。`add_line(a, b)` O(log N)、`get(x)` で x での最小値 O(log N) |
| 削除可 | `add_segment(a, b, l, r)` | l <= x < r の範囲だけの線分を追加 O(log^2 N) |

`dp[j] = min_i (dp[i] + (i だけで決まる係数) × (j だけで決まる x) + (i だけで決まる定数))` の形の DP を、直線の追加と最小値に分けて O(N log N) にする。最大値は `add_line(−a, −b)` で入れて `−get(x)`。何も入っていない x では `LICHAO_INF<T>` を返す。

### [dynamic_segtree.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/dynamic_segtree.cpp) — 動的セグ木
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `DynamicSegTree<S, op, e> seg(L, R)` | 添字 [L, R)（long long）。`set(p, x)`, `get(p)`, `prod(l, r)`, `all_prod()`。各 O(log(R − L)) |

クエリが全部先にわかるなら「座標圧縮 + ACL の segtree」の方が速い。前の答えで次のクエリが決まる（オンライン）ときに使う。1 点に足すのは `seg.set(p, seg.get(p) + x)`。

### [wavelet_matrix.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/wavelet_matrix.cpp) — Wavelet Matrix
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `WaveletMatrix<long long> wm(a)` | 前計算 O(N log σ)（中で座標圧縮）。更新はできない |
| 本体 | `kth_smallest(l, r, k)`, `kth_largest(l, r, k)`, `access(i)` | 区間 [l, r) の k 番目（0 始まり）。O(log σ) |
| 本体 | `range_freq(l, r, lo, hi)`, `prev_value(l, r, x)`, `next_value(l, r, x)` | lo ≤ 値 < hi の個数、x 未満の最大・x 以上の最小（なければ `nullopt`） |

### [persistent_segtree.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/persistent_segtree.cpp) — 永続セグ木
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `PersistentSegTree<S, op, e> seg(n)`, `build(v)` | 最初の版（根の番号 int）を作る |
| 本体 | `set(root, p, x)`, `get(root, p)`, `prod(root, l, r)` | 版 root から 1 点を変えた新しい版を作る（root は残る）、取得、区間の積。各 O(log N) |

典型: 版 i = 「A の先頭 i 個を、値の位置に足したもの」にすると、版 R と版 L の差で「区間 [L, R) の中で x 以下の個数・和」がオンラインで答えられる（使用例の ABC339 G）。

---

## geometry/ — 幾何

| ファイル | 何をするか | 自分で定義するもの |
|---|---|---|
| [`geometry.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp) | 点・ベクトル、内積・外積、向き（ccw）、点が線分上か、線分の交差、凸包、多角形の面積・内外判定、偏角ソート | なし |

### [geometry.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `Pt<long long>`（`+`, `-`, `*`）, `dot`, `cross`, `norm2` | ベクトルの計算。座標は整数で誤差なし（絶対値 1e9 程度まで） |
| 本体 | `ccw(a, b, c)` | a → b → c が反時計回り 1、時計回り −1、一直線 0 |
| 本体 | `on_segment(p, a, b)`, `segments_intersect(a, b, c, d)` | 点が線分上か、2 つの線分が共有点を持つか（端点で触れる・重なるも含む） |
| 削除可 | `convex_hull(pts)` | 凸包の頂点（反時計回り、一直線上の途中の点は除く）O(N log N) |
| 削除可 | `area2(poly)`, `point_in_polygon(p, poly)` | 面積の 2 倍（反時計回りなら正）、内部 2 / 辺上 1 / 外部 0 |
| 削除可 | `arg_less(a, b)` | 偏角ソートの比較関数。`sort(ps.begin(), ps.end(), arg_less<long long>)` で x 軸の正の向きから反時計回りの順（原点が先頭、同じ向きは同順位） |


---

## techniques/ — 解法パターン

アルゴリズムそのものではなく「問題をどう見れば使えるか」の型。**どんなときに使うか・見分け方は `techniques/patterns.md` にまとめてある**（主客転倒・期待値の線形性・鳩ノ巣原理・偶奇と不変量など、コードにならない考え方の解説もそこにある）。

| ファイル | 何をするか | 自分で定義するもの |
|---|---|---|
| [`event_sort.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/event_sort.cpp) | イベントソート（重なりの最大数、塗られた長さ、ちょうど k 個の長さ、点の被覆数。座標が大きくてよい） | なし |
| [`offline_query.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/offline_query.cpp) | オフラインクエリ（重み x 以下の辺での連結性、区間の x 以下の個数） | なし |
| [`interval_scheduling.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/interval_scheduling.cpp) | 区間スケジューリング（重ならない最大数、全部を刺す最小本数） | なし |
| [`inclusion_exclusion.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/inclusion_exclusion.cpp) | 包除原理（汎用、どれかで割り切れる数の個数） | 汎用版のみ `f(mask)`（ラムダ） |
| [`plane_sweep_2d.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/plane_sweep_2d.cpp) | 2 次元の平面走査（長方形に入る点の重みの合計。座標が大きくてよい） | なし |

### [event_sort.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/event_sort.cpp)
| 区分 | 機能 | 何をするか |
|---|---|---|
| 本体 | `EventSort<T> es`, `add(l, r)`, `build()` | 半開区間 [l, r) を登録して並べる（閉区間 [l, r] は `add(l, r + 1)`） |
| 本体 | `max_overlap()`, `length_at_least(k)`, `length_exactly()`, `count_at(x)`, `profile` | 重なりの最大、k 個以上の長さ、ちょうど k 個の長さ、点 x の被覆数、変化点の一覧 |

同じ座標のイベントはまとめて処理するので、「同じ座標で始まりと終わりが重なる」場合も正しく数える（自分で書くときの注意は patterns.md）。

### [offline_query.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/offline_query.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `offline_connectivity<T>(n, edges, queries)` | クエリ {a, b, x} ごとに、重み x 以下の辺だけでの {連結か, a の成分の大きさ} |
| 本体B | `offline_count_le<T>(a, queries)` | クエリ {l, r, x} ごとに、a[l..r) の中で x 以下の個数 |

「x 以上」「x より大きい」は −1 倍や x ± 1 で「x 以下」に直す（使用例の ABC040 D を参照）。

### [interval_scheduling.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/interval_scheduling.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `max_disjoint_intervals(iv)` | 半開区間のうち重ならずに選べる最大数（選んだ番号を返す） |
| 本体B | `min_stabbing_points(iv)` | 閉区間すべてに 1 本ずつ刺さる最小の点（座標を返す） |

### [inclusion_exclusion.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/inclusion_exclusion.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体A | `union_count<T>(m, f)` | `f(mask)` = mask の条件を全部満たす個数、を渡すと「どれか 1 つ以上を満たす個数」 |
| 本体B | `count_divisible_any(N, a)` | 1..N のうち a のどれかで割り切れる数の個数（最小公倍数があふれない） |

### [plane_sweep_2d.cpp](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/plane_sweep_2d.cpp)
| 区分 | 関数 | 何をするか |
|---|---|---|
| 本体 | `rect_sum_offline<T, W>(pts, rects)` | 点 {x, y, w}、長方形 {x1, y1, x2, y2}（半開区間）ごとに、入る点の重みの合計 O((N + Q) log N) |

---

## ABC / ARC での使用例

各ライブラリが実際に使える問題（リンクは問題ページ。すべて開いて問題文を確認済み）。第 16 弾から ARC の問題も入れている。
EDPC（Educational DP Contest）・AOJ（Aizu Online Judge）は ABC ではないが、そのまま使える典型なので入れている。各 .cpp ファイルの先頭にも同じものを書いてある。
主客転倒・期待値の線形性・鳩ノ巣原理・偶奇と不変量の例は `techniques/patterns.md` にある。区間篩・平方分割・凸包・ミラー・ラビン / ポラード・ロー・重み付き DAG 最長路・ガウスの消去法・木の同型判定・k 番目最短路・距離 k 以内の頂点数・Monotone Minima・彩色数・ハンガリアン法・一般グラフの最大マッチング・Wavelet Matrix・ブルーフカ法・最小全域有向木・支配木は、ABC / ARC で使う問題を確認できていないので載せていない。AGC の問題も、定番のものは入れている。

### basic/ — 探索・数列

| ファイル | 問題 | 使い方 |
|---|---|---|
| [`binary_search.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/binary_search.cpp) | [ABC146 C - Buy an Integer](https://atcoder.jp/contests/abc146/tasks/abc146_c) | 買える最大の N を答えで二分探索（bin_search） |
|  | [ABC077 C - Snuke Festival](https://atcoder.jp/contests/abc077/tasks/arc084_a) | 中段を固定し、上段・下段の個数を count_lt / count_gt で数える |
| [`prefix_sum.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/prefix_sum.cpp) | [ABC122 C - GeT AC](https://atcoder.jp/contests/abc122/tasks/abc122_c) | "AC" の出現位置の累積和で区間クエリ（PrefixSum） |
|  | [ABC125 C - GCD on Blackboard](https://atcoder.jp/contests/abc125/tasks/abc125_c) | 1 つを除いた gcd の最大（PrefixFold の except） |
|  | [ABC106 D - AtCoder Express 2](https://atcoder.jp/contests/abc106/tasks/abc106_d) | (L, R) の 2 次元累積和（PrefixSum2D） |
| [`imos.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/imos.cpp) | [ABC035 C - オセロ](https://atcoder.jp/contests/abc035/tasks/abc035_c) | 区間に +1 して最後に偶奇を見る |
|  | [ABC014 C - AtColor](https://atcoder.jp/contests/abc014/tasks/abc014_3) | 区間に +1 して最大値を取る |
| [`compress.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/compress.cpp) | [ABC036 C - 座圧](https://atcoder.jp/contests/abc036/tasks/abc036_c) | そのまま座標圧縮 |
|  | [ABC213 C - Reorder Cards](https://atcoder.jp/contests/abc213/tasks/abc213_c) | 行番号・列番号をそれぞれ圧縮 |
| [`sparse_table.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/sparse_table.cpp) | [ABC189 C - Mandarin Orange](https://atcoder.jp/contests/abc189/tasks/abc189_c) | N ≤ 10^4。全区間 (l, r) の min を O(1) で取って O(N^2)（左端を固定して min を更新しながら伸ばしても解ける） |
| [`two_pointers.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/two_pointers.cpp) | [ABC032 C - 列](https://atcoder.jp/contests/abc032/tasks/abc032_c) | 積が K 以下の最長区間（要素に 0 があるときは別扱い） |
|  | [ABC098 D - Xor Sum 2](https://atcoder.jp/contests/abc098/tasks/arc098_b) | xor と和が等しい区間の個数 = Σ(R[l] − l) |
| [`brute_force.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/brute_force.cpp) | [ABC128 C - Switches](https://atcoder.jp/contests/abc128/tasks/abc128_c) | スイッチの on/off を bit 全探索（bit_search） |
|  | [ABC145 C - Average Length](https://atcoder.jp/contests/abc145/tasks/abc145_c) | 町を回る順番を順列全探索（perm_search） |
|  | [ABC119 C - Synthetic Kadomatsu](https://atcoder.jp/contests/abc119/tasks/abc119_c) | 各竹を A / B / C / 使わない の 4 通りで N 進数全探索（product_search） |
| [`run_length.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/run_length.cpp) | [ABC019 B - 高橋くんと文字列圧縮](https://atcoder.jp/contests/abc019/tasks/abc019_2) | そのままランレングス圧縮 |
|  | [ABC329 C - Count xxx](https://atcoder.jp/contests/abc329/tasks/abc329_c) | 文字ごとに一番長い塊の長さを足す |
| [`ternary_search.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/ternary_search.cpp) | [ABC279 D - Freefall](https://atcoder.jp/contests/abc279/tasks/abc279_d) | 操作回数 x の関数 Bx + A/√(1+x) は凸 → 整数版（argmin_convex_int） |
| [`inversion.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/inversion.cpp) | [ABC190 F - Shift and Inversions](https://atcoder.jp/contests/abc190/tasks/abc190_f) | 最初の転倒数を求め、先頭を末尾に回すごとに差分で更新 |
|  | [ABC261 F - Sorting Color Balls](https://atcoder.jp/contests/abc261/tasks/abc261_f) | 同じ色どうしの入れ替えはタダ。答え = 全体の転倒数 − 色ごとの転倒数の和 |
| [`sliding_window.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/sliding_window.cpp) | [ABC352 D - Permutation Subsequence](https://atcoder.jp/contests/abc352/tasks/abc352_d) | 値 i の位置の列で、長さ K の区間の max − min の最小（sliding_max / sliding_min） |
| [`meet_in_the_middle.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/basic/meet_in_the_middle.cpp) | [ABC184 F - Programming Contest](https://atcoder.jp/contests/abc184/tasks/abc184_f) | N ≤ 40 で和が T 以下の最大（max_subset_sum_le） |
|  | [ABC032 D - ナップサック問題](https://atcoder.jp/contests/abc032/tasks/abc032_d) | N ≤ 30 のケースを knapsack_mitm で（他のケースは dp/knapsack.cpp） |

### graph/ — グラフ

| ファイル | 問題 | 使い方 |
|---|---|---|
| [`graph_input.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/graph_input.cpp) | [ABC168 D - .. (Double Dots)](https://atcoder.jp/contests/abc168/tasks/abc168_d) | N M と辺のリストを read_graph で読む |
| [`bfs.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bfs.cpp) | [ABC007 C - 幅優先探索](https://atcoder.jp/contests/abc007/tasks/abc007_3) | グリッド BFS（grid_bfs） |
|  | [ABC168 D - .. (Double Dots)](https://atcoder.jp/contests/abc168/tasks/abc168_d) | BFS の prev がそのまま答え |
|  | [ABC213 E - Stronger Takahashi](https://atcoder.jp/contests/abc213/tasks/abc213_e) | 歩く = 0、パンチ = 1 の 01-BFS（bfs01_func） |
| [`dfs.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dfs.cpp) | [ABC285 D - Change Usernames](https://atcoder.jp/contests/abc285/tasks/abc285_d) | 名前の変更を有向辺にして閉路があるか（find_cycle_directed） |
|  | [ABC288 C - Don't be cycle](https://atcoder.jp/contests/abc288/tasks/abc288_c) | 答え = M − (N − 連結成分数)（connected_components） |
| [`dijkstra.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/dijkstra.cpp) | [ABC035 D - トレジャーハント](https://atcoder.jp/contests/abc035/tasks/abc035_d) | 行きと帰り（辺を逆向きにしたグラフ）でダイクストラ |
|  | [ABC395 E - Flip Edge](https://atcoder.jp/contests/abc395/tasks/abc395_e) | 状態 (頂点, 反転しているか) の拡張ダイクストラ（dijkstra_func） |
|  | [ABC211 D - Number of Shortest paths](https://atcoder.jp/contests/abc211/tasks/abc211_d) | 重み 1 の無向グラフで count_shortest_paths<atcoder::modint1000000007>（mod が 998244353 でないので注意） |
| [`shortest_path.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/shortest_path.cpp) | [ABC061 D - Score Attack](https://atcoder.jp/contests/abc061/tasks/abc061_d) | スコアを −1 倍してベルマンフォード。dist[N] == NEG_INF なら inf |
|  | [ABC012 D - バスと避けられない運命](https://atcoder.jp/contests/abc012/tasks/abc012_4) | ワーシャルフロイドで全点対 |
|  | [ABC375 F - Road Blocked](https://atcoder.jp/contests/abc375/tasks/abc375_f) | クエリを逆順にして「道路の追加」にし、wf_add_edge で O(N^2) 更新 |
|  | [ABC237 E - Skiing](https://atcoder.jp/contests/abc237/tasks/abc237_e) | 幸福度を −1 倍すると負の辺がある最短路。ポテンシャルを高さ H にすると重みが 0 以上になるので、johnson の中身と同じくダイクストラ 1 回（全点対の johnson は N が大きいので使わない） |
| [`topological_sort.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/topological_sort.cpp) | [ABC223 D - Restricted Permutation](https://atcoder.jp/contests/abc223/tasks/abc223_d) | 辞書順最小のトポロジカルソート（lex_min = true） |
|  | [ABC139 E - League](https://atcoder.jp/contests/abc139/tasks/abc139_e) | 試合を頂点にした DAG の最長路（dag_longest_path）。閉路なら -1 |
| [`kruskal.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/kruskal.cpp) | [ABC065 D - Built?](https://atcoder.jp/contests/abc065/tasks/arc076_b) | x 順・y 順で隣り合う町だけを辺にしてクラスカル |
|  | [ABC218 E - Destruction](https://atcoder.jp/contests/abc218/tasks/abc218_e) | 重みを max(c, 0) にして（負の辺は残すほど得）、重みの合計 − 最小全域木の重み |
|  | [AOJ ALDS1_12_A - Minimum Spanning Tree](https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ALDS1_12_A&lang=ja) | 隣接行列で与えられる。-1 を PRIM_INF にして prim_dense（ABC ではないが練習用の定番） |
| [`bipartite.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bipartite.cpp) | [ABC282 D - Make Bipartite 2](https://atcoder.jp/contests/abc282/tasks/abc282_d) | 成分ごとの色の個数（bipartite_components）から数える |
|  | [ABC126 D - Even Relation](https://atcoder.jp/contests/abc126/tasks/abc126_d) | 根からの距離の偶奇で 2 色に塗る（2 彩色の考え方。重み付きなので dfs_walk で親から偶奇を伝えて塗る） |
| [`functional_graph.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/functional_graph.cpp) | [ABC167 D - Teleporter](https://atcoder.jp/contests/abc167/tasks/abc167_d) | K ≤ 10^18 回移動した先（walk_k） |
|  | [ABC241 E - Putting Candies](https://atcoder.jp/contests/abc241/tasks/abc241_e) | x mod N を頂点にしたダブリングで和を求める（jump_sum） |
|  | [ABC357 E - Reachability in Functional Graph](https://atcoder.jp/contests/abc357/tasks/abc357_e) | 閉路の長さ + 閉路までの距離（FunctionalGraph） |
| [`weighted_union_find.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/weighted_union_find.cpp) | [ABC087 D - People on a Line](https://atcoder.jp/contests/abc087/tasks/arc090_b) | x_R − x_L = D を merge し、矛盾したら No |
|  | [ABC328 F - Good Set Query](https://atcoder.jp/contests/abc328/tasks/abc328_f) | merge が true を返したクエリの番号を出力 |
| [`lowlink.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/lowlink.cpp) | [ABC075 C - Bridge](https://atcoder.jp/contests/abc075/tasks/abc075_c) | 橋の本数（bridges.size()） |
|  | [ABC334 G - Christmas Color Grid 2](https://atcoder.jp/contests/abc334/tasks/abc334_g) | 緑のマスを頂点にする。マス v を赤にした後の成分数 = C − 1 + (v を含むブロックの数 = block_cut_tree の t[v].size())。ただし孤立したマスは C − 1 |
| [`bipartite_matching.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/bipartite_matching.cpp) | [ABC091 C - 2D Plane 2N Points](https://atcoder.jp/contests/abc091/tasks/arc092_a) | 赤（左）と青（右）を、条件を満たす組で辺にして最大マッチング（貪欲でも解ける） |
| [`rollback_union_find.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/rollback_union_find.cpp) | [ABC302 Ex - Ball Collector](https://atcoder.jp/contests/abc302/tasks/abc302_h) | ボールの番号を頂点、各頂点の 2 個のボールを辺にする。木を DFS しながら行きで merge、帰りで undo。答え = 成分ごとの min(size, edges) の和 |
| [`scc_dag.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/scc_dag.cpp) | [ABC245 F - Endless Walk](https://atcoder.jp/contests/abc245/tasks/abc245_f) | 使用例と同じ。後ろの成分から「ぐるぐる回れる成分にたどり着けるか」を伝える |
| [`euler_path.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/euler_path.cpp) | [ABC286 G - Unique Walk](https://atcoder.jp/contests/abc286/tasks/abc286_g) | S 以外の辺で連結成分をまとめ（dsu）、成分を頂点・S の辺を辺にした多重グラフで euler_path(...).ok を見る（次数の奇数の個数が 0 か 2 か、でも判定できる） |
| [`persistent_union_find.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/persistent_union_find.cpp) | [AGC002 D - Stamp Rally](https://atcoder.jp/contests/agc002/tasks/agc002_d) | 辺を番号順に unite。クエリごとに「x と y の成分の大きさの合計（同じ成分なら 1 つ分）が z 以上になる最小の時刻 t」を二分探索（AGC だが部分永続 UnionFind の定番） |
| [`steiner_tree.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/graph/steiner_tree.cpp) | [ABC364 G - Last Major City](https://atcoder.jp/contests/abc364/tasks/abc364_g) | ターミナルを都市 1..K−1 にして steiner_tree。都市 i の答えは dp[full][i]（「全ターミナルと頂点 i をつなぐ最小」がそのまま答え） |

### tree/ — 木

| ファイル | 問題 | 使い方 |
|---|---|---|
| [`tree.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree.cpp) | [ABC014 D - 閉路](https://atcoder.jp/contests/abc014/tasks/abc014_4) | 答え = dist(a, b) + 1（LCA） |
|  | [ABC361 E - Tree and Hamilton Path 2](https://atcoder.jp/contests/abc361/tasks/abc361_e) | 答え = 2 × (辺の重みの和) − 直径（diameter） |
|  | [ABC138 D - Ki](https://atcoder.jp/contests/abc138/tasks/abc138_d) | 部分木に加算（TreeImos の add_subtree） |
| [`rerooting.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/rerooting.cpp) | [ABC220 F - Distance Sums 2](https://atcoder.jp/contests/abc220/tasks/abc220_f) | 各頂点からの距離の和（使用例の「例2」そのもの） |
|  | [ABC348 E - Minimize Sum of Distances](https://atcoder.jp/contests/abc348/tasks/abc348_e) | 例2 の頂点数を C_i の和に置き換える |
| [`euler_tour.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/euler_tour.cpp) | [ABC202 E - Count Descendants](https://atcoder.jp/contests/abc202/tasks/abc202_e) | 深さごとに in[v] を並べ、[in[U], out[U]) に入る個数を二分探索 |
|  | [ABC294 G - Distance Queries on a Tree](https://atcoder.jp/contests/abc294/tasks/abc294_g) | 辺の重みを子の頂点に乗せ、根からのパスの和を BIT で（+ LCA） |
| [`tree_dp.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/tree_dp.cpp) | [ABC036 D - 塗り絵](https://atcoder.jp/contests/abc036/tasks/abc036_d) | 隣り合う 2 つを両方黒にしない塗り方の数（使用例の「例3」を mint で） |
|  | [EDPC P - Independent Set](https://atcoder.jp/contests/dp/tasks/dp_p) | ABC036 D と同じ問題（ABC ではないが典型） |
| [`hld.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/hld.cpp) | [ABC294 G - Distance Queries on a Tree](https://atcoder.jp/contests/abc294/tasks/abc294_g) | 辺の重みを子の位置に置き、path(u, v, true) の区間の和を BIT で |
|  | [ABC014 D - 閉路](https://atcoder.jp/contests/abc014/tasks/abc014_4) | 答え = dist(a, b) + 1（HLD の lca でも求まる） |
| [`centroid.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/centroid.cpp) | [ABC291 Ex - Balanced Tree](https://atcoder.jp/contests/abc291/tasks/abc291_h) | 求める木がそのまま重心木。cpar を 1-indexed で出力（根は -1） |
| [`auxiliary_tree.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/tree/auxiliary_tree.cpp) | [ABC340 G - Leaf Color](https://atcoder.jp/contests/abc340/tasks/abc340_g) | 色ごとにその色の頂点で build し、虚樹の上で木 DP（Σ 頂点数 = O(N)） |
|  | [ABC359 G - Sum of Tree Distance](https://atcoder.jp/contests/abc359/tasks/abc359_g) | ラベルごとに build し、虚樹の各辺について「辺の長さ × 片側の個数 × 反対側の個数」を足す |

### dp/ — DP

| ファイル | 問題 | 使い方 |
|---|---|---|
| [`knapsack.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/knapsack.cpp) | [ABC032 D - ナップサック問題](https://atcoder.jp/contests/abc032/tasks/abc032_d) | 制約で 3 通りに分岐: N 小 → 半分全列挙（basic/meet_in_the_middle.cpp の knapsack_mitm）、W 小 → knapsack_01、価値小 → knapsack_01_by_value |
|  | [EDPC E - Knapsack 2](https://atcoder.jp/contests/dp/tasks/dp_e) | W ≤ 10^9、価値が小さい → knapsack_01_by_value（ABC ではないが典型） |
| [`lcs.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/lcs.cpp) | [EDPC F - LCS](https://atcoder.jp/contests/dp/tasks/dp_f) | LCS の復元（lcs）（ABC ではないが典型） |
|  | [ABC185 E - Sequence Matching](https://atcoder.jp/contests/abc185/tasks/abc185_e) | 編集距離そのもの（edit_distance） |
| [`lis.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/lis.cpp) | [ABC038 D - プレゼント](https://atcoder.jp/contests/abc038/tasks/abc038_d) | 幅で昇順・同じ幅は高さで降順にソートし、高さの LIS |
|  | [ABC354 F - Useless for LIS](https://atcoder.jp/contests/abc354/tasks/abc354_f) | 前からの len と後ろからの len の和 − 1 が LIS の長さになる位置（lis_lengths） |
| [`digit_dp.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/digit_dp.cpp) | [ABC029 D - 1](https://atcoder.jp/contests/abc029/tasks/abc029_d) | 状態 = ここまでの 1 の個数、答え = Σ s × cnt[s]（使用例と同じ） |
|  | [EDPC S - Digit Sum](https://atcoder.jp/contests/dp/tasks/dp_s) | 状態 = 桁和 mod D、答え = cnt[0] − 1（0 を除く）。K は 10000 桁なので T = mint |
| [`interval_dp.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/interval_dp.cpp) | [EDPC N - Slimes](https://atcoder.jp/contests/dp/tasks/dp_n) | 使用例のスライムの合体そのもの（ABC ではないが区間 DP の定番） |
| [`tsp.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/tsp.cpp) | [ABC180 E - Traveling Salesman among Aerial Cities](https://atcoder.jp/contests/abc180/tasks/abc180_e) | d[i][j] = abs(a−a') + abs(b−b') + max(0, c'−c) で tsp_cycle(d, dp, 0) |
|  | [ABC190 E - Magical Ornament](https://atcoder.jp/contests/abc190/tasks/abc190_e) | K 個の宝石から BFS して K×K の距離表 → 始点自由なので、全頂点へ 0 で行けるダミー頂点を足して tsp_path。答え +1 |
| [`expected_value.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/expected_value.cpp) | [ABC280 E - Critical Hit](https://atcoder.jp/contests/abc280/tasks/abc280_e) | 状態 = 与えたダメージ i（N 以上は N にまとめてゴール）。i → min(i+2, N) が P/100、i → i+1 が 1 − P/100 で ev_backward<mint> |
|  | [ABC263 E - Sugoroku 3](https://atcoder.jp/contests/abc263/tasks/abc263_e) | 出目 0 = 自己ループ。遷移が合計 O(N^2) 本あるので ev_backward は使わず、同じ式 E[i] = (1 + Σ E[j] / (A_i+1)) / (1 − 1/(A_i+1)) を、Σ E[j] を後ろからの累積和で持って手で書く |
| [`alien_dp.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/dp/alien_dp.cpp) | [ABC218 H - Red and Blue Lamps](https://atcoder.jp/contests/abc218/tasks/abc218_h) | 赤をちょうど R 個（R > N/2 なら色を入れ替える）。赤 1 個ごとにペナルティを引く「個数自由」の DP は O(N) の 2 状態 DP。最大化なので −1 倍して alien_dp |
|  | [ABC305 Ex - Shojin](https://atcoder.jp/contests/abc305/tasks/abc305_h) | （難しい）日数 K ごとの最小コストが K について凸。最小の日数 D を探したあと、ちょうど D 日の最小コストを Alien DP で |

### math/ — 数学

| ファイル | 問題 | 使い方 |
|---|---|---|
| [`integer.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/integer.cpp) | [ABC239 B - Integer Division](https://atcoder.jp/contests/abc239/tasks/abc239_b) | 負の数の切り捨て除算（floor_div） |
|  | [ABC400 C - 2^a b^2](https://atcoder.jp/contests/abc400/tasks/abc400_c) | a = 1, 2 の場合に分けて isqrt で数える |
|  | [ABC340 F - S = 1](https://atcoder.jp/contests/abc340/tasks/abc340_f) | \|X·B − Y·A\| = 2 を拡張ユークリッドで解く（ext_gcd） |
|  | [ABC105 C - Base -2 Number](https://atcoder.jp/contests/abc105/tasks/abc105_c) | to_base_str(N, -2) がそのまま答え（N = 0 は "0"） |
|  | [ABC156 B - Digits](https://atcoder.jp/contests/abc156/tasks/abc156_b) | to_base(N, K).size() |
| [`matrix.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/matrix.cpp) | [ABC293 E - Geometric Progression](https://atcoder.jp/contests/abc293/tasks/abc293_e) | (和, A^i) を 2×2 行列で X 乗。M が素数とは限らないので T に atcoder::modint（set_mod）を使う |
|  | [EDPC R - Walk](https://atcoder.jp/contests/dp/tasks/dp_r) | 隣接行列の K 乗（ABC ではないが典型） |
| [`combination.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/combination.cpp) | [ABC145 D - Knight](https://atcoder.jp/contests/abc145/tasks/abc145_d) | 2 種類の移動の回数を求めて C(n, k) |
|  | [ABC202 D - aab aba baa](https://atcoder.jp/contests/abc202/tasks/abc202_d) | mod なしの二項係数表（pascal）で K 番目を決める |
|  | [ARC117 C - Tricolor Pyramid](https://atcoder.jp/contests/arc117/tasks/arc117_c) | 色を 0, 1, 2 にすると上の色 = −(左 + 右) mod 3。一番上 = (−1)^(N−1) Σ C(N−1, i) c_i mod 3 で、二項係数は Lucas(3) |
| [`prime.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/prime.cpp) | [ABC142 D - Disjoint Set of Common Divisors](https://atcoder.jp/contests/abc142/tasks/abc142_d) | gcd(A, B) の素因数の個数 + 1（factorize） |
|  | [ABC177 E - Coprime](https://atcoder.jp/contests/abc177/tasks/abc177_e) | 篩の最小素因数で全要素を素因数分解（Sieve） |
|  | [AOJ NTL_1_D - オイラーのφ関数](https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=NTL_1_D&lang=ja) | n ≤ 10^9 の φ(n)。`euler_phi` そのもの（ABC ではないが練習用の定番） |
| [`xor_basis.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/xor_basis.cpp) | [ABC141 F - Xor Sum 3](https://atcoder.jp/contests/abc141/tasks/abc141_f) | 全体の xor で 1 の桁は和に必ず入るので消し、残りで max_xor を 2 倍して足す |
| [`divisor_transform.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/divisor_transform.cpp) | [ABC162 E - Sum of gcd of Tuples (Hard)](https://atcoder.jp/contests/abc162/tasks/abc162_e) | f[d] = (K / d)^N（全部 d の倍数）→ multiple_mobius → Σ d × f[d]。mod は 1e9+7 なので modint1000000007 |
|  | [ABC206 E - Divide Both](https://atcoder.jp/contests/abc206/tasks/abc206_e) | f[g] = (L 以上 R 以下の g の倍数の個数)^2 → multiple_mobius で gcd がちょうど g の組。そこから x や y が g そのものの組を引く |
| [`gauss.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/math/gauss.cpp) | [ABC253 Ex - We Love Forest](https://atcoder.jp/contests/abc253/tasks/abc253_h) | 頂点の部分集合 S ごとに、S の中の辺だけでの全域木の個数を count_spanning_trees で求め、部分集合 DP で森の数を数える（N ≤ 14） |

### string/ — 文字列

| ファイル | 問題 | 使い方 |
|---|---|---|
| [`rolling_hash.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/rolling_hash.cpp) | [ABC141 E - Who Says a Pun?](https://atcoder.jp/contests/abc141/tasks/abc141_e) | 長さを二分探索し、重ならない同じ部分文字列があるかをハッシュで判定 |
|  | [ABC284 F - ABCBAC](https://atcoder.jp/contests/abc284/tasks/abc284_f) | T と反転した T のハッシュで、各 i について O(1) で判定 |
| [`kmp.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/kmp.cpp) | [ABC150 F - Xor Shift](https://atcoder.jp/contests/abc150/tasks/abc150_f) | 隣り合う要素の xor の列に変換し、b の列を a の列 2 周分の中から kmp_search で探す |
| [`manacher.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/manacher.cpp) | [ABC398 F - ABCBA](https://atcoder.jp/contests/abc398/tasks/abc398_f) | is_pal(i, n) が true になる最小の i を探し、s[0..i) を反転して後ろに付ける |
| [`trie.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/trie.cpp) | [ABC287 E - Karuta](https://atcoder.jp/contests/abc287/tasks/abc287_e) | 全部 insert してから、各文字列の lcp_with_others |
|  | [ABC353 E - Yet Another Sigma Problem](https://atcoder.jp/contests/abc353/tasks/abc353_e) | 全部 insert して sum_lcp_pairs |
| [`aho_corasick.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/aho_corasick.cpp) | [ABC268 Ex - Taboo](https://atcoder.jp/contests/abc268/tasks/abc268_h) | S を前から読み、cnt[v] > 0 の状態（どれかの T_i で終わった）に来たらその文字を * にして根に戻す貪欲 |
| [`bracket.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/bracket.cpp) | [ABC394 D - Colorful Bracket Sequence](https://atcoder.jp/contests/abc394/tasks/abc394_d) | is_balanced(s, "([<", ")]>") がそのまま答え |
|  | [ABC064 D - Insertion](https://atcoder.jp/contests/abc064/tasks/abc064_d) | bracket_fix(s) = {a, b} で、'(' を a 個 + s + ')' を b 個 |
|  | [ABC312 D - Count Bracket Sequences](https://atcoder.jp/contests/abc312/tasks/abc312_d) | count_bracket_fill<atcoder::modint998244353>(s) |
| [`subsequence.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/subsequence.cpp) | [ARC081 E - Don't Be a Subsequence](https://atcoder.jp/contests/arc081/tasks/arc081_c) | Subsequence<>(a).shortest_absent() がそのまま答え |
|  | [ABC452 D - No-Subsequence Substring](https://atcoder.jp/contests/abc452/tasks/abc452_d) | 各開始位置 l で e = sub.end_of(t, l)。t を含む区間は N − e + 1 個なので、全体 N(N+1)/2 から引く |
|  | [ABC138 E - Strings of Impurity](https://atcoder.jp/contests/abc138/tasks/abc138_e) | s を 2 回つなげたもので nxt を作り、t の文字ごとに nxt(i % \|s\|, c) で次の位置へ進んで、何周したかを数える |
| [`palindrome.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/palindrome.cpp) | [ABC147 B - Palindrome-philia](https://atcoder.jp/contests/abc147/tasks/abc147_b) | pal_min_changes(s) がそのまま答え |
| [`suffix_array.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/string/suffix_array.cpp) | [ALPC I - Number of Substrings](https://atcoder.jp/contests/practice2/tasks/practice2_i) | SuffixArray(s).count_distinct() がそのまま答え（ABC ではないが ACL 公式の練習問題） |
|  | [ABC213 F - Common Prefixes](https://atcoder.jp/contests/abc213/tasks/abc213_f) | 答え(k) = Σ_i lcp_of(k, i)。sa の順に並べると LCP は間の lcp[] の最小なので、前から・後ろからスタックで「最小値の和」を持って O(N) |

### ds/ — データ構造

| ファイル | 問題 | 使い方 |
|---|---|---|
| [`bit2d.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/bit2d.cpp) | [ABC106 D - AtCoder Express 2](https://atcoder.jp/contests/abc106/tasks/abc106_d) | (L, R) に点を足して長方形の和（更新がないので PrefixSum2D で十分だが、BIT2D でも解ける） |
| [`mo.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/mo.cpp) | [ABC242 G - Range Pairing Query](https://atcoder.jp/contests/abc242/tasks/abc242_g) | 色ごとの個数を持ち、ペアの数 Σ⌊個数/2⌋ を add / erase で更新 |
|  | [ABC293 G - Triple Index](https://atcoder.jp/contests/abc293/tasks/abc293_g) | 値ごとの個数 c を持ち、C(c, 3) の和を add / erase で更新 |
| [`interval_set.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/interval_set.cpp) | [ABC256 D - Union of Interval](https://atcoder.jp/contests/abc256/tasks/abc256_d) | 全部 insert して mp をそのまま出力 |
|  | [ABC228 D - Linear Probing](https://atcoder.jp/contests/abc228/tasks/abc228_d) | 埋まった位置 x を [x, x+1) として入れ、空き位置を mex で探す（末尾を越えたら mex(0)） |
| [`erasable_pq.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/erasable_pq.cpp) | [ABC253 C - Max - Min Query](https://atcoder.jp/contests/abc253/tasks/abc253_c) | 最大用・最小用の ErasablePQ に同じ値を入れる。消す個数は値ごとの個数を map で数えて min(c, 個数) |
|  | [ABC306 E - Best Performances](https://atcoder.jp/contests/abc306/tasks/abc306_e) | TopKSum で、値の変更を erase(古い値) と insert(新しい値) にする |
|  | [ABC281 E - Least Elements](https://atcoder.jp/contests/abc281/tasks/abc281_e) | 長さ M の区間をずらしながら -A を TopKSum に入れ、小さい方から K 個の和 = -sum |
| [`li_chao_tree.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/li_chao_tree.cpp) | [EDPC Z - Frog 3](https://atcoder.jp/contests/dp/tasks/dp_z) | 使用例と同じ。直線 (−2 h_i, dp[i] + h_i^2) を入れて x = h_j の最小値（ABC ではないが Li Chao / CHT の定番） |
| [`dynamic_segtree.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/dynamic_segtree.cpp) | [ABC403 G - Odd Position Sum Query](https://atcoder.jp/contests/abc403/tasks/abc403_g) | 前の答えで次の値が決まる（オンライン）ので座標圧縮できない。値 x を添字に、{個数, 奇数番目の和, 偶数番目の和} を持つ動的セグ木（左の個数の偶奇で右の奇偶を入れ替えてつなぐ） |
| [`persistent_segtree.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/ds/persistent_segtree.cpp) | [ABC339 G - Smaller Sum](https://atcoder.jp/contests/abc339/tasks/abc339_g) | 前の答えで次のクエリが決まる（オンライン）。値を座標圧縮し、版 i = A[0..i) の値の位置に A を足したもの。答え = 版 R の [0, X 以下) の和 − 版 L−1 の同じ和 |

### geometry/ — 幾何

| ファイル | 問題 | 使い方 |
|---|---|---|
| [`geometry.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp) | [ABC266 C - Convex Quadrilateral](https://atcoder.jp/contests/abc266/tasks/abc266_c) | 4 つの角すべてで ccw が 1 なら凸 |
|  | [ABC016 D - 一刀両断](https://atcoder.jp/contests/abc016/tasks/abc016_4) | 多角形の各辺と切る線分が交わる数 c を segments_intersect で数え、答え = c / 2 + 1 |
|  | [AOJ CGL_3_A - 面積](https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=CGL_3_A&lang=ja) | area2(poly) / 2 を小数 1 桁で出力（ABC ではないが練習用の定番） |
|  | [AOJ CGL_3_C - 多角形-点の包含](https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=CGL_3_C&lang=ja) | point_in_polygon の返り値 2 / 1 / 0 がそのまま答え（ABC ではないが練習用の定番） |
|  | [ABC225 E - フ](https://atcoder.jp/contests/abc225/tasks/abc225_e) | 各「フ」を、原点から見た向き (x, y−1) から (x−1, y) までの角度の区間にして、終わりの向きの順（arg_less）に貪欲（区間スケジューリング） |

### techniques/ — 解法パターン

| ファイル | 問題 | 使い方 |
|---|---|---|
| [`event_sort.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/event_sort.cpp) | [ABC221 D - Online games](https://atcoder.jp/contests/abc221/tasks/abc221_d) | A 日目から B 日間 = [A, A + B)。length_exactly() の ans[k] がそのまま答え |
| [`offline_query.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/offline_query.cpp) | [ABC040 D - 道路の老朽化対策について](https://atcoder.jp/contests/abc040/tasks/abc040_d) | 「w 年より後」の道路だけ。年を −1 倍して重み −y、クエリ x = −(w+1) で offline_connectivity の成分の大きさ |
| [`interval_scheduling.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/interval_scheduling.cpp) | [ABC103 D - Islands War](https://atcoder.jp/contests/abc103/tasks/abc103_d) | a と b の間の橋 = 閉区間 [a, b−1] に 1 本刺す。min_stabbing_points の個数 |
| [`inclusion_exclusion.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/inclusion_exclusion.cpp) | [ABC131 C - Anti-Division](https://atcoder.jp/contests/abc131/tasks/abc131_c) | (B − A + 1) − (count_divisible_any(B, {C, D}) − count_divisible_any(A − 1, {C, D})) |
| [`plane_sweep_2d.cpp`](https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/techniques/plane_sweep_2d.cpp) | [ABC106 D - AtCoder Express 2](https://atcoder.jp/contests/abc106/tasks/abc106_d) | 列車 (L, R) を点、「p ≤ L かつ R ≤ q」を長方形 [p, N+1) × [1, q+1) に（制約が小さいので 2 次元累積和でも解ける） |
