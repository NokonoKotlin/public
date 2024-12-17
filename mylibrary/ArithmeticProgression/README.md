## 列の区間に等差数列を加算したりするやつ。

`ドキュメント最終更新 : 2024/5/7`  
`非ライブラリ`  

スプレー木ライブラリの亜種。遅延評価 (作用) に等差数列 加算/更新 を載せる代わりに、要素のモノイド積を取れなくなっている。  

等差数列を $ax+b$ と書いたとき、初項 $b$ . 公差 $a$ の等差数列とする。よって、$x$ は $0$ 始まりとする。

- 等差数列加算 : 列の半開区間 $[l,r)$ の左から $x$ 番目の要素に $ax + b$ を加算する
- 等差数列更新 : 列の半開区間 $[l,r)$ の左から $x$ 番目の要素を $ax + b$ に変更する

splay 木なのでセグ木より定数倍が遅いが、挿入/削除 をサポートする。  
セグ木なら双対セグ木で済むこともあるが、双対セグ木では 加算/更新 の両方をサポートできない。


<a href = "https://atcoder.jp/contests/abc268/submissions/53006388" target = "_brank">
verify 問題リンク (ABC 268-E)
</a>

Copyright (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)


## NokonoKotlin のお気持ち
ライセンスを守って使って欲しいよ (ライブラリの冒頭に以下を書くだけ)
- Copyright ©️ (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)


