# N 次元セグメント木
`ドキュメント最終更新 : 2024/05/20`

N 次元矩形の領域の取得クエリ/一点更新 を行うデータ構造。  
セグメントツリーの要素を、一次元下のセグメントツリーと捉えることで再帰的に領域を選択する。  

<a href = "https://nokonokotlin.github.io/Home/Contents/algorithm/Articles/DataStructure/NdSegTree.html">説明はこちらの記事(自筆)</a>

## ライセンス (license)
Copyright (c) 2024 NokonoKotlin
Released under the MIT license
https://opensource.org/licenses/mit-license.php


## NokonoKotlin のお気持ち
- 競プロの提出とかも、ここのコードを使っているなら著作者 (NokonoKotlin) を明記してほしいよ。
- ライブラリに組み込むのは自由だけど、自分のライブラリと称するのはやめてほしいよ。




### 概要
- 多次元セグ木(1次元も含む)
- コンストラクタで vector&lt;int> size\_v と T init\_ を渡す。
- 各次元のサイズが size\_v[0],size\_v[1] , ... , size\_v[d-1] の d 次元グリッド(マス目)を表現し、初期値が init\_ となる
- 基本的に座標を表すのは d 次元座標。
- d 次元座標上の矩形を表す際は、その矩形の対角の座標 A , B を用いて表す。
- 矩形内のマスを対象に、矩形クエリを行う。
- 0 次元セグ木 = T 型要素 として扱う。
- 多次元セグ木のノードは一次元下のセグ木なので、セグ木 = Node として捉えて OK 
- 抽象化
    - データの型 : T 
    - データ a , b のマージ演算 : op(a,b)
    - op の単位元 : e
    - 1 点の値 x に対して、val で更新をかける : upd(&x , val) 

### ラッパークラス
二次元グリッドの特定の操作に特化したラッパークラスを用意しています。  
