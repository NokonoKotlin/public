# タイトル
`ドキュメント最終更新 : 2024/5/13`

根付き木を明示的に管理するのではなく、木上のパスを SplayTree (列型) で管理する。  

## ライセンス (license)
Copyright (c) 2024 NokonoKotlin
Released under the MIT license
https://opensource.org/licenses/mit-license.php


## NokonoKotlin のお気持ち
ライセンスを守って使って欲しいよ (ライブラリの冒頭に以下を書くだけ)
- Copyright ©️ (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)



### TODO 

オイラーツアーと一緒に記事を書く


### 機能 & 仕様
- #### accessがとっても大事
    - access(x) := x の属する木の根から x までを HeavyEdge で繋ぎ、パスにする。
        - パスは SplayTree である。
        - ついでに Nodes[x] を SplayTree の根にする (splay)
    - access(x)で繋がった SplayTreeは、左の方が根に近く、右に行くほど下の頂点になる (path なので)
    - 根から x までの path の情報 (Value の Sum など) は、access(x) してから Nodes[x]->Sum などで取得できる
- #### 基本機能
    - evert(x) := x を連結成分の root にする (x までのパスの SplayTree を反転して管理する)
    - link(x,y) := x,y に辺を追加 (元々 x,y は非連結)
    - cut(x,y) := x,y から辺を削除 (元々 x,y に辺がある)
    - operator [x] 
        - 「根から x までのパスを表す SplayTree の根」(のコピー) を取得する
            - (隣接 SplayNode へのポインタを封印してある)
        - 自動で access(x) される。
    - root(x) := x の属する木の根
    - depth(x) := x の深さ
    - same(x,y) := x,y が同じ木に属しているか
    - parent(x) := ( x の親が存在するか, 親の番号) のペア
    - LCA(x,y) := x,y の lca (x,y は同じ木に属する)
- #### パスに関する機能
    - - path_xth_element(v,x) := v の属する木の「根から頂点 v までのパス」のうち 深さが x の頂点番号
    - PathUpdate(i,x) := 頂点 i が属する木の根から頂点 i までのパス上の頂点の値を一律 x に更新する
    - PathAffine(i,A,B) := 頂点 i が属する木の根から頂点 i までのパス上の頂点の値 (Value) を一律 A*Value + B に更新
    - PathUpdate(i,x) := 頂点 i が属する木の根から頂点 i までのパス上の頂点の値に一律 x 加算する

- 辺の表現には頂点拡張をすると良い