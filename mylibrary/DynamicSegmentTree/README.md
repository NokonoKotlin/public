# 遅延評価付き動的セグメント木

`ドキュメント最終更新 : 2024/5/7`

範囲に制約のない整数区間に対するクエリを処理する列。

Copyright (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)


## NokonoKotlin のお気持ち
ライセンスを守って使って欲しいよ (ライブラリの冒頭に以下を書くだけ)
- Copyright ©️ (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)



### 概要 & 仕様
- 数列を表すデータ構造
    - 区間更新 (一律に代入)
    - 区間アフィン変換 (区間内の要素全てに A 掛けて B 足す)
    - 区間のモノイド積(集約値 Sum など)を求める
- 定数倍が気になるならいらない機能を削除してね
- テンプレートで インデックスの型 index_int(整数) と 要素の型 T  を渡す
- 初期化値もモノイド積に含まれるので、Sum などがオーバーフローしないように注意
- コンストラクタで [L,R) と初期化値 init_  を渡し、[L,R) に対応する根ノードを作成する。
    - はじめ、ノードは [L,R) に対応するものしかなく、必要に応じて半分に分割することを繰り返す。
    - L,R に範囲の制約はない (オーバーフロー注意)
- #### アルゴリズム
    - はじめ、セグ木のノードは根のみ存在する。
    - アクセスを伴う処理が行われた際に、アクセスするセグメントまでのノードを必要に応じて作る。  
        - 作るノード数はクエリあたり log(R-L) 
        - eval() から内部処理関数 make_child を呼び出す。
    - eval() は遅延評価 + 必要な子ノードを作る役割を持つ。
        - ノードの Value は本来は葉ノードのみが持つものだが、Root から init_value を継承するためにも使う。
        - 継承される Value (init_value) には遅延評価も作用させる。
          - 子ノードを新たに作った時に限り、子ノードに遅延評価を降ろさない
### 設計
- Node クラスに必要なデータを持たせる
- いじるのは基本的に Node の eval , UpdateNode およびコンストラクタでの変数の初期化