# N 次元双対セグメント木
`ドキュメント最終更新 : 2024/05/20`

N 次元領域で、一点取得/矩形更新 を行うデータ構造。  
セグメントツリーの要素を、一次元下のセグメントツリーと捉えることで再帰的に領域を選択する。  

<a href = "https://nokonokotlin.github.io/Home/Contents/algorithm/Articles/DataStructure/NdSegTree.html">説明はこちらの記事(自筆)</a>

Copyright (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)


## NokonoKotlin のお気持ち
ライセンスを守って使って欲しいよ (ライブラリの冒頭に以下を書くだけ)
- Copyright ©️ (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)




### 概要
- 多次元双対セグ木
- N 次元のグリッドの矩形領域に対して更新クエリを行う。
- 更新クエリをおこなった区間に対応するセグメントに、更新クエリによる影響を評価値として貯めておく。  
- 評価値とは別に init\_val を単体のデータで保持しておくことで、あたかもグリッドが init\_val で初期化してあったかのように振る舞うようにする
- 初め、全てののマス目にかかる評価値は e() : 単位元 で初期化される(何も評価がかかっていないことを表す)
    - ある点の値を取得する際は、グリッドの初期値 init\_val と、グリッドにかかる評価値の総マージ結果をマージしたものを返す
- コンストラクタで vector&lt;int> size\_v と T init\_val を渡す。
- 各次元のサイズが size\_v[0],size\_v[1] , ... , size\_v[d-1] の d 次元グリッド(マス目)を表現し、グリッドの値は init\_val で初期化されている。  
- 基本的に座標を表すのは d 次元座標で、d 次元座標上の矩形を表す際は、その矩形の対角の座標 A , B を用いて表す。
- その矩形内のマスを対象に矩形クエリを行う    
- 多次元セグ木のノードは一次元下のセグ木なので、セグ木 = Node として捉えて OK 
- 抽象化 
    - データの型 : T
    - データ x に val を反映させる : upd_(&x , val) 
    - ある点 (マス目) にかかるすべての評価をマージする演算 : op(a,b) 
    - 評価マージの単位元 : e()

### ラッパークラス
二次元グリッドの特定の操作に特化したラッパークラスを用意しています。  
