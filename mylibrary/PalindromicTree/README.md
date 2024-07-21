# 回文木
`ドキュメント最終更新 : 2024/7/22`

文字列 $S$ の部分文字列の回文を管理するデータ構造。一体何のために生まれたのかはわからない。  

## ライセンス (license)
Copyright (c) 2024 NokonoKotlin
Released under the MIT license
https://opensource.org/licenses/mit-license.php


## NokonoKotlin のお気持ち
- 競プロの提出とかも、ここのコードを使っているなら著作者 (NokonoKotlin) を明記してほしいよ。
- ライブラリに組み込むのは自由だけど、自分のライブラリと称するのはやめてほしいよ。


### 概要
文字列 $S$ の部分文字列の回文を木のノードと対応させて管理すると、ノードの個数は $O(|S|)$ 個になる。  
それぞれのノードは、以下の $2$ 種類の辺を持つ。
- $\mathrm{connect[c]} := $ 自身が表す回文の両端に文字 $c$ を連結した回文のノード
- $\mathrm{SuffixLink} := $ 自身が表す回文の接尾辞であり、$S$ の部分文字列かつ回文であるものうち、最長のものに対応するノード

$S$ の回文木とは、上記のグラフの関係で $\mathrm{SuffixLink}$ のみに注目したもの。これは木です。

### 機能
- `push(c)` : $S$ の末尾に新たに文字 $c$ を `push` する。この時、回文木にも反映される。
- `aggregate()` : 回文木の構築を終了し、回文木における集約を行う。
    - 各ノードに対して、対応する回文が $S$ に登場する頻度など
    - `aggregate()` を実行してからは `push` を行うことはできない。
- `crawl()` : 全てのノードを $1$ 度ずつ vector に入れて返す。ただし、回文の長さでソートされて返される。


### 内部実装/仕様
初期状態として、以下の $2$ 種類のノードを持つ。
- $\mathrm{seed}$ : 区間 $[x,x-1)$ の長さ $-1$ の空文字回文に対応するノード。ただし、長さが負の文字列は実在しないので、外部機能で明示的に扱うことはない (`crawl` では無視する)。
- $\mathrm{empty}$ : 区間 $[x,x)$ の長さ $-1$ の空文字回文に対応するノード。

$\mathrm{seed}$ の両端に文字 $c$ をつなげることで長さ $1$ の回文 $c$ が得られ、$\mathrm{empty}$ の両端に文字 $c$ をつなげることで長さ $2$ の回文 $cc$ が得られる。


### アルゴリズム
<a href = "https://nokonokotlin.github.io/Home/Contents/algorithm/Articles/string/PalindromicTree.html" target = "_blank_">解説はこちらのページから</a>
