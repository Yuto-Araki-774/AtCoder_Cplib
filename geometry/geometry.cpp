// =====================================================================
// geometry/geometry.cpp  幾何（点・ベクトル、内積・外積、向き、線分の交差、凸包、多角形の面積・内外判定、偏角ソート）
//   座標は整数（long long）で、誤差なしで判定する。座標の絶対値は 1e9 程度まで（外積が 1e18 程度に収まる）
//   ※ 実数座標で使うなら T = double にして、== 0 の判定を |x| < 1e-9 に変える
// 依存: なし
// ABC / ARC での使用例:
//   ABC266 C - Convex Quadrilateral  https://atcoder.jp/contests/abc266/tasks/abc266_c
//     → 4 つの角すべてで ccw が 1 なら凸
//   ABC016 D - 一刀両断  https://atcoder.jp/contests/abc016/tasks/abc016_4
//     → 多角形の各辺と切る線分が交わる数 c を segments_intersect で数え、答え = c / 2 + 1
//   AOJ CGL_3_A - 面積  https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=CGL_3_A&lang=ja
//     → area2(poly) / 2 を小数 1 桁で出力（ABC ではないが練習用の定番）
//   AOJ CGL_3_C - 多角形-点の包含  https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=CGL_3_C&lang=ja
//     → point_in_polygon の返り値 2 / 1 / 0 がそのまま答え（ABC ではないが練習用の定番）
//   ABC225 E - フ  https://atcoder.jp/contests/abc225/tasks/abc225_e
//     → 各「フ」を、原点から見た向き (x, y−1) から (x−1, y) までの角度の区間にして、終わりの向きの順（arg_less）に貪欲（区間スケジューリング）
//   （凸包は ABC で使う問題を確認できていないので例なし。AOJ CGL_4_A は一直線上の点も出力する問題なのでこの convex_hull とは合わない）
// 貼る場所: コピー範囲を main 関数の外（using namespace std; の後、main より上）に貼る
// =====================================================================
#include <bits/stdc++.h>
using namespace std;

// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ ここからコピー ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// ---------- 本体 ----------
// using P = Pt<long long>;   P a{1, 2}, b{3, 4};
//   a + b, a - b, a * k            ベクトルの和・差・定数倍
//   dot(a, b)                      内積 a.x b.x + a.y b.y（> 0 なら鋭角、= 0 なら直角、< 0 なら鈍角）
//   cross(a, b)                    外積 a.x b.y - a.y b.x（> 0 なら b は a から反時計回り、= 0 なら平行）
//   ccw(a, b, c)                   a → b → c が反時計回りなら 1、時計回りなら -1、一直線なら 0
//   on_segment(p, a, b)            点 p が線分 ab 上（端点を含む）にあるか
//   segments_intersect(a, b, c, d) 線分 ab と線分 cd が共有点を持つか（端点で触れる・重なるも含む）
//   norm2(a)                       長さの 2 乗（距離を比べるときは 2 乗のまま比べれば誤差がない）
// ◆ struct Pt<T> : 点・ベクトル {x, y}
//   a + b, a − b, a * k : ベクトルの和・差・定数倍
//   a == b             : 同じ点か
//   a < b              : x が小さい順、同じなら y が小さい順（ソート用）
//   T : 座標の型（long long 推奨。座標の絶対値 1e9 程度までなら外積が long long に収まる）
template <class T>
struct Pt {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp
    T x, y;
    Pt operator+(const Pt& o) const { return {x + o.x, y + o.y}; }
    Pt operator-(const Pt& o) const { return {x - o.x, y - o.y}; }
    Pt operator*(T k) const { return {x * k, y * k}; }
    bool operator==(const Pt& o) const { return x == o.x && y == o.y; }
    bool operator<(const Pt& o) const { return x != o.x ? x < o.x : y < o.y; }
};
// ◆ dot(a, b) : 内積 a.x b.x + a.y b.y（正なら鋭角、0 なら直角、負なら鈍角）
template <class T> T dot(const Pt<T>& a, const Pt<T>& b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp
    return a.x * b.x + a.y * b.y;
}
// ◆ cross(a, b) : 外積 a.x b.y − a.y b.x（正なら b は a から見て反時計回り、0 なら平行）
template <class T> T cross(const Pt<T>& a, const Pt<T>& b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp
    return a.x * b.y - a.y * b.x;  // ★ a, b が作る平行四辺形の符号付き面積
}
// ◆ norm2(a) : 長さの 2 乗（距離は 2 乗のまま比べると誤差が出ない）
template <class T> T norm2(const Pt<T>& a) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp
    return dot(a, a);
}
// ◆ ccw(a, b, c) : a → b → c の向き。反時計回りなら 1、時計回りなら −1、一直線なら 0
//   a, b, c : 3 点（同じ点を含んでもよい。そのときは 0）
template <class T> long long ccw(const Pt<T>& a, const Pt<T>& b, const Pt<T>& c) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp
    T v = cross(b - a, c - a);  // ★ a から見た b と c の向き
    return (v > 0) - (v < 0);
}
// ◆ on_segment(p, a, b) : 点 p が線分 ab の上（端点を含む）にあるか
//   p : 調べる点、a, b : 線分の端点（a == b なら点 a と同じか）
template <class T> bool on_segment(const Pt<T>& p, const Pt<T>& a, const Pt<T>& b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp
    return cross(a - p, b - p) == 0 && dot(a - p, b - p) <= 0;  // ★ 一直線上で、p から見て a と b が反対側（か p が端点）
}
// ◆ segments_intersect(a, b, c, d) : 線分 ab と線分 cd が共有点を持つか（端点で触れる・重なるも true）
//   a, b : 1 本目の端点、c, d : 2 本目の端点
template <class T> bool segments_intersect(const Pt<T>& a, const Pt<T>& b, const Pt<T>& c, const Pt<T>& d) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp
    int d1 = ccw(a, b, c), d2 = ccw(a, b, d), d3 = ccw(c, d, a), d4 = ccw(c, d, b);
    if (d1 == 0 && d2 == 0)  // ★ 4 点が一直線（線分が点のときも含む）→ どれかの端点がもう一方の線分上にあるか
        return on_segment(c, a, b) || on_segment(d, a, b) || on_segment(a, c, d) || on_segment(b, c, d);
    return d1 * d2 <= 0 && d3 * d4 <= 0;  // ★ c, d が直線 ab の両側（か上）にあり、a, b が直線 cd の両側（か上）にある
}

// ---------- 削除可: 凸包（上の本体が必要） O(N log N) ----------
// 返り値: すべての点を囲む最小の凸多角形の頂点を反時計回りに（一直線上の途中の点・重複は除く）
//         x が最小（同じなら y が最小）の点から始まる。点が 1 個なら 1 個、全部一直線なら両端の 2 個
// 例: auto h = convex_hull(pts);  周の長さ・面積・最も遠い 2 点などは h の上で調べる
// ◆ convex_hull(ps) : 凸包の頂点（反時計回り。x が最小の点から始まる。一直線上の途中の点は含めない）。O(N log N)
//   ps : 点の一覧（重複してもよい）。点が 2 個以下ならそのまま（重複を除いて）返す
template <class T> vector<Pt<T>> convex_hull(vector<Pt<T>> ps) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp
    sort(ps.begin(), ps.end());
    ps.erase(unique(ps.begin(), ps.end()), ps.end());
    int n = ps.size();
    if (n <= 2) return ps;
    vector<Pt<T>> h(2 * n);
    int k = 0;
    for (int i = 0; i < n; i++) {  // ★ 下側: 左から右へ、左に曲がらない（右折・直進）点を捨てる
        while (k >= 2 && cross(h[k - 1] - h[k - 2], ps[i] - h[k - 2]) <= 0) k--;
        h[k++] = ps[i];
    }
    for (int i = n - 2, t = k + 1; i >= 0; i--) {  // ★ 上側: 右から左へ、同じことをする
        while (k >= t && cross(h[k - 1] - h[k - 2], ps[i] - h[k - 2]) <= 0) k--;
        h[k++] = ps[i];
    }
    h.resize(k - 1);  // 最後に始点がもう一度入るので除く
    return h;
}

// ---------- 削除可: 多角形の面積・点の内外判定（上の本体が必要） ----------
// poly は頂点を順番に並べたもの（時計回りでも反時計回りでもよい。自己交差しないこと）
// area2(poly)               面積の 2 倍（整数のまま）。反時計回りなら正、時計回りなら負
// point_in_polygon(p, poly) 内部なら 2、辺の上なら 1、外部なら 0
// ◆ area2(poly) : 多角形の面積の 2 倍（整数のまま）。反時計回りなら正、時計回りなら負
//   poly : 頂点を順番に並べたもの（自分と交わらない多角形）
template <class T> T area2(const vector<Pt<T>>& poly) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp
    T s = 0;
    for (size_t i = 0; i < poly.size(); i++) s += cross(poly[i], poly[(i + 1) % poly.size()]);  // ★ 原点と各辺が作る三角形の符号付き面積の和
    return s;
}
// ◆ point_in_polygon(p, poly) : 点 p が多角形の内部なら 2、辺の上なら 1、外部なら 0
//   p : 調べる点、poly : 頂点を順番に並べたもの（時計回り・反時計回りどちらでもよい）
template <class T> long long point_in_polygon(const Pt<T>& p, const vector<Pt<T>>& poly) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp
    bool in = false;
    for (size_t i = 0; i < poly.size(); i++) {
        Pt<T> a = poly[i] - p, b = poly[(i + 1) % poly.size()] - p;
        if (cross(a, b) == 0 && dot(a, b) <= 0) return 1;  // 辺の上
        if (a.y > b.y) swap(a, b);
        if (a.y <= 0 && 0 < b.y && cross(a, b) > 0) in = !in;  // ★ p から右へ伸ばした半直線が辺と交わるたびに内外が入れ替わる
    }
    return in ? 2 : 0;
}

// ---------- 削除可: 偏角ソート ----------
// sort(ps.begin(), ps.end(), arg_less<long long>);
//   原点から見た向き（偏角）の順に並べる。x 軸の正の向き（角度 0）から反時計回りに、角度 [0, 360°)
//   同じ向き（長さだけ違う）は「同じ」扱い（順番は決まらない。決めたいなら stable_sort か、比較に norm2 を足す）
//   原点 (0, 0) は一番前
//   外積だけで比べるので誤差なし（atan2 は使わない）。座標は 1e9 程度まで
// （内部用）原点は −1、角度 [0°, 180°) は 0、[180°, 360°) は 1
template <class T> int arg_half(const Pt<T>& p) {  // 原点は -1、[0, 180°) は 0、[180°, 360°) は 1
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp
    if (p.x == 0 && p.y == 0) return -1;
    return (p.y < 0 || (p.y == 0 && p.x < 0)) ? 1 : 0;
}
// ◆ arg_less(a, b) : 偏角ソート用の比較。原点から見た向きが a の方が前か（x 軸の正の向きから反時計回り）
//   a, b : ベクトル。原点 (0, 0) は一番前、同じ向きは同順位
template <class T> bool arg_less(const Pt<T>& a, const Pt<T>& b) {
    // Library: https://github.com/Yuto-Araki-774/AtCoder_Cplib/blob/main/geometry/geometry.cpp
    int ha = arg_half(a), hb = arg_half(b);
    if (ha != hb) return ha < hb;  // ★ まず上半分（0 以上 180° 未満）か下半分かで分ける
    if (ha == -1) return false;
    return cross(a, b) > 0;  // ★ 同じ半分の中なら角度の差は 180° 未満 → 外積の符号で前後が決まる
}

// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ ここまでコピー ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

// ===================== 使用例・動作確認（コピー不要） =====================
int main() {
    using P = Pt<long long>;
    P o{0, 0}, a{2, 0}, b{0, 2}, c{2, 2};
    assert(cross(a, b) == 4 && dot(a, b) == 0 && ccw(o, a, c) == 1 && ccw(o, b, c) == -1 && ccw(o, c, P{5, 5}) == 0);
    assert(segments_intersect(o, c, a, b));                // 対角線どうし（(1, 1) で交わる）
    assert(!segments_intersect(o, a, b, c));               // 平行な辺
    assert(segments_intersect(o, a, a, c));                // 端点 (2, 0) で触れる
    assert(segments_intersect(o, P{4, 0}, a, P{6, 0}));    // 一直線上で重なる
    assert(!segments_intersect(o, P{1, 0}, a, P{6, 0}));   // 一直線上で離れている
    assert(on_segment(P{1, 1}, o, c) && !on_segment(P{3, 3}, o, c));

    // ランダムテスト: 交差判定を「交点の位置を分数で求める」別の方法と比較
    mt19937 rr(129);
    auto ref = [](P p, P p2, P q, P q2) {  // p + t r と q + u s（0 <= t, u <= 1）が共有点を持つか
        P r = p2 - p, s = q2 - q, qp = q - p;
        auto on = [](P x, P st, P v) {  // 点 x が st + t v（0 <= t <= 1）上
            P w = x - st;
            if (v.x == 0 && v.y == 0) return w.x == 0 && w.y == 0;
            return cross(w, v) == 0 && dot(w, v) >= 0 && dot(w, v) <= dot(v, v);
        };
        if (r.x == 0 && r.y == 0) return on(p, q, s);
        if (s.x == 0 && s.y == 0) return on(q, p, r);
        long long rxs = cross(r, s);
        if (rxs != 0) {  // t = (qp × s) / (r × s), u = (qp × r) / (r × s) が両方 [0, 1] に入るか
            long long tn = cross(qp, s), un = cross(qp, r);
            if (rxs < 0) rxs = -rxs, tn = -tn, un = -un;
            return 0 <= tn && tn <= rxs && 0 <= un && un <= rxs;
        }
        if (cross(qp, r) != 0) return false;  // 平行で別の直線
        long long rr2 = dot(r, r), t0 = dot(qp, r), t1 = t0 + dot(s, r);  // 直線 r 上の位置（rr2 倍）で区間の重なりを見る
        return max(0LL, min(t0, t1)) <= min(rr2, max(t0, t1));
    };
    for (int it = 0; it < 200000; it++) {
        P p[4];
        for (auto& x : p) x = {(long long)(rr() % 5), (long long)(rr() % 5)};
        assert(segments_intersect(p[0], p[1], p[2], p[3]) == ref(p[0], p[1], p[2], p[3]));
        long long m = 1000000000;  // 大きな座標でも外積があふれないか
        P q[4];
        for (auto& x : q) x = {(long long)(rr() % (2 * m + 1)) - m, (long long)(rr() % (2 * m + 1)) - m};
        assert(segments_intersect(q[0], q[1], q[2], q[3]) == ref(q[0], q[1], q[2], q[3]));
        assert(on_segment(p[0], p[1], p[2]) == ref(p[0], p[0], p[1], p[2]));
    }

    // 凸包・面積・内外判定の例
    // 点 (0,0) (4,0) (4,4) (0,4) (2,2) (2,0): 凸包は正方形の 4 頂点（(2,2) は内部、(2,0) は辺の途中なので除く）
    vector<P> pts = {{0, 0}, {4, 0}, {4, 4}, {0, 4}, {2, 2}, {2, 0}};
    auto h = convex_hull(pts);
    assert((h == vector<P>{{0, 0}, {4, 0}, {4, 4}, {0, 4}}));
    assert(area2(h) == 32);  // 面積 16 の 2 倍
    assert(point_in_polygon(P{2, 2}, h) == 2 && point_in_polygon(P{4, 1}, h) == 1 && point_in_polygon(P{5, 1}, h) == 0);
    assert((convex_hull(vector<P>{{1, 1}, {3, 3}, {2, 2}}) == vector<P>{{1, 1}, {3, 3}}));  // 一直線 → 両端

    // ランダムテスト: 凸包が「(1) 頂点が入力の点 (2) どの角も左に曲がる (3) 全点がどの辺の左側か辺の上」を満たすか
    for (int it = 0; it < 5000; it++) {
        int n = rr() % 12 + 1;
        vector<P> ps(n);
        for (auto& x : ps) x = {(long long)(rr() % 7), (long long)(rr() % 7)};
        auto hh = convex_hull(ps);
        set<P> uniq(ps.begin(), ps.end());
        for (auto& v : hh) assert(uniq.count(v));
        int k = hh.size();
        if (uniq.size() == 1) { assert(k == 1); continue; }
        set<P> u2(ps.begin(), ps.end());
        bool colinear = true;
        P a0 = *u2.begin(), a1 = *next(u2.begin());
        for (auto& q : u2) if (cross(a1 - a0, q - a0) != 0) colinear = false;
        if (colinear) { assert(k == 2 && hh[0] == *u2.begin() && hh[1] == *u2.rbegin()); continue; }
        assert(k >= 3);
        for (int i = 0; i < k; i++) {
            assert(ccw(hh[i], hh[(i + 1) % k], hh[(i + 2) % k]) == 1);  // 真に左に曲がる（途中の点なし）
            for (auto& q : ps) assert(cross(hh[(i + 1) % k] - hh[i], q - hh[i]) >= 0);
        }
        assert(area2(hh) > 0);  // 反時計回り
    }
    // ランダムテスト: 内外判定を「角度の和（巻き数）」という別の方法と比較、面積はピックの定理で確認
    for (int it = 0; it < 3000; it++) {
        // 中心 c のまわりに角度順に並べた多角形（どの隣り合う 2 頂点も c から見て 180 度未満）は自己交差しない
        P c{5, 5};
        int n = rr() % 8 + 3;
        vector<P> ps;
        for (int i = 0; i < n; i++) {
            P q{(long long)(rr() % 11), (long long)(rr() % 11)};
            if (!(q == c)) ps.push_back(q);
        }
        sort(ps.begin(), ps.end(), [&](P a, P b) { return atan2(a.y - c.y, a.x - c.x) < atan2(b.y - c.y, b.x - c.x); });
        bool ok = ps.size() >= 3;
        for (size_t i = 0; i < ps.size() && ok; i++) if (cross(ps[i] - c, ps[(i + 1) % ps.size()] - c) <= 0) ok = false;
        if (!ok) continue;
        long long B = 0, I = 0;
        for (size_t i = 0; i < ps.size(); i++) { P d = ps[(i + 1) % ps.size()] - ps[i]; B += gcd(llabs(d.x), llabs(d.y)); }
        for (long long x = -1; x <= 11; x++)
            for (long long y = -1; y <= 11; y++) {
                P q{x, y};
                int r = point_in_polygon(q, ps);
                bool on = false;
                for (size_t i = 0; i < ps.size(); i++) on |= on_segment(q, ps[i], ps[(i + 1) % ps.size()]);
                double w = 0;  // 巻き数: 各辺を見込む角の合計（内部なら 2π、外部なら 0）
                if (!on) for (size_t i = 0; i < ps.size(); i++) {
                    P a = ps[i] - q, b = ps[(i + 1) % ps.size()] - q;
                    w += atan2((double)cross(a, b), (double)dot(a, b));
                }
                int ref = on ? 1 : (fabs(w) > 3.0 ? 2 : 0);
                assert(r == ref);
                I += r == 2;
            }
        assert(area2(ps) == 2 * I + B - 2);  // ピックの定理: 面積 = I + B/2 - 1
        vector<P> rev(ps.rbegin(), ps.rend());
        assert(area2(rev) == -area2(ps));
    }
    // 偏角ソート: 8 方向の点をばらばらに並べて、0°, 45°, ..., 315° の順に戻るか
    {
        using P = Pt<long long>;
        vector<P> dir = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
        vector<P> v = {{0, -3}, {-2, 2}, {5, 0}, {-1, -1}, {0, 1}, {3, -3}, {-4, 0}, {2, 2}};
        sort(v.begin(), v.end(), arg_less<long long>);
        for (int i = 0; i < 8; i++) assert(cross(v[i], dir[i]) == 0 && dot(v[i], dir[i]) > 0);
        assert(arg_less(P{0, 0}, P{1, 0}) && !arg_less(P{1, 0}, P{0, 0}));
        assert(!arg_less(P{1, 2}, P{2, 4}) && !arg_less(P{2, 4}, P{1, 2}));  // 同じ向き
    }
    // ランダムテスト: atan2 で求めた角度（[0, 2π) に直す）と比べる。同じ向きは外積・内積で判定
    {
        mt19937 rg(154);
        using P = Pt<long long>;
        auto ang = [](const P& p) { long double a = atan2l((long double)p.y, (long double)p.x); return a < 0 ? a + 2 * acosl(-1.0L) : a; };
        for (int it = 0; it < 200000; it++) {
            P a{(long long)(rg() % 21) - 10, (long long)(rg() % 21) - 10}, b{(long long)(rg() % 21) - 10, (long long)(rg() % 21) - 10};
            bool za = a.x == 0 && a.y == 0, zb = b.x == 0 && b.y == 0;
            bool want;
            if (za || zb) want = za && !zb;
            else if (cross(a, b) == 0 && dot(a, b) > 0) want = false;  // 同じ向き
            else want = ang(a) < ang(b);
            assert(arg_less(a, b) == want);
        }
        // 大きい座標でも、ソート結果が隣どうし正しい順か（外積が long long に収まる 1e9 まで）
        vector<P> v;
        for (int i = 0; i < 2000; i++) v.push_back({(long long)(rg() % 2000000001) - 1000000000, (long long)(rg() % 2000000001) - 1000000000});
        sort(v.begin(), v.end(), arg_less<long long>);
        for (int i = 0; i + 1 < (int)v.size(); i++) assert(!arg_less(v[i + 1], v[i]) && ang(v[i]) <= ang(v[i + 1]));
    }
    puts("OK");
}
