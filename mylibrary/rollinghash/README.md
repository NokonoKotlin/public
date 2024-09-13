# ローリングハッシュ
`ドキュメント最終更新 : 2024/5/11`

文字列の任意の部分文字列のハッシュを生成する。  

## ライセンス (license)
Copyright (c) 2024 NokonoKotlin
Released under the MIT license
https://opensource.org/licenses/mit-license.php


## NokonoKotlin のお気持ち
ライセンスを守って使って欲しいよ (ライブラリの冒頭に以下を書くだけ)
- Copyright ©️ (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)


## ローリングハッシュ概要

整数の組 $(b,m)$ に対して文字列 $S$ のローリングハッシュ $\mathrm{H}(S)$ を $\sum_{1 \leq i \leq |S|}{S_i \cdot b^{|S|-i} \mod m}$ とする。  

文字列 $S,T$ について以下が成り立つ。  
- $S$ と $T$ が一致する $\rightarrow$ $\mathrm{H}(S) = \mathrm{H}(T)$

逆は必ずしも成り立たつとは限らない。(ハッシュの衝突)
$\mathrm{H}(S) = \mathrm{H}(T)$ にもかかわらず $S \neq T$ である確率が微小ならば、文字列の一致判定を整数の一致判定に帰着させることができる。  

単体のハッシュではどうしても危ないので、2 つのハッシュの組み合わせで判定する。
整数の組 $(b1,m1)$ に対するハッシュを $H_1$、$(b2,m2)$ に対するハッシュを $H_2$ とする。  

$m1\cdot m2 \leq 2^{62} - 1$  であれば、${H_1}\cdot m2 + H_2$ は long long に収まる強力なハッシュとなる。  

文字列に限らず列でも同じですが、要素の値は $1$ 以上 $m$ 未満である必要があります。  

#### static ver.
`rollinghash.cpp`

文字列の prefix に対してハッシュを計算しておくことで、区間ハッシュを計算できる。
具体的には、$S$ の先頭 $i$ 文字のハッシュを $p_i$ としたとき、文字列の 0-index の半開区間 $[l,r)$ のハッシュは以下で計算できる。
- $(p_r-p_l\cdot b^{r-l}) \mod m$  


#### 列 ver.
`rollinghash_array.cpp`

セグ木 ver. & スプレー木 ver.

- セグ木に乗せることで、値の更新をしながらハッシュの計算ができます。 
    - 昔書いたコードなので、いつか書き直します。
- スプレー木に乗せると挿入/削除もサポートします。  