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
- #### access と evert 
    - `access(x)` := 頂点 $x$ の属する木の根から $x$ までを heavy-edge で繋いでパスにする。
        - 頂点 $x$ を SplayTree の根にし、パスのデータを頂点 $x$ からアクセス可能にする。
        - 頂点 $x$ を、根から頂点 $x$ までのデータ取得のインターフェースにする役割。
    - `evert(x)` := 頂点 $x$ の属する木の根を $x$ に変更する。
        - ついでに `access(x)` が実行される。
        - 連結成分の根が何かを保証する
- #### 基本機能
    - `link(x,y)` := 頂点 $x,y$ に辺を追加 (元々 $x,y$ は非連結である必要がある)
        - 連結後の連結成分の根は $x$ である。
    - `cut(x,y)` := 頂点 $x,y$ を繋ぐ辺を削除 (元々 $x,y$ に辺がある必要がある)
        - 削除後、元の連結成分の根が属さない方の連結成分の根が何になるかを保証しない。
        - 適宜 `evert` などをしてください。
    - `[x]` := 「根から $x$ までのパスを表す SplayTree の根」(のコピー) を取得する
        - 隣接する SplayNode へのポインタを封印してある。
        - 自動で `access(x)` される。
    - `update_val(i,x)` := 頂点 $i$ の重み (Value) を $x$ にする。
    - `root(x)` := 頂点 $x$ の属する木の根
    - `depth(x)` := 頂点 $x$ の深さ
    - `same(x,y)` := 頂点 $x,y$ が同じ木に属しているか
    - `parent(x)` := (頂点 $x$ の親が存在するか, 親の番号) のペア
    - `LCA(x,y)` := 頂点 $x,y$ の LCA ( $x,y$ は同じ木に属する必要がある )
- #### パスに関する機能
    - `path_xth_element(v,x)` := 頂点 $v$ の属する木の「根から頂点 $v$ までのパス」のうち 深さが $x$ の頂点番号
    - `PathUpdate(i,x)` := 頂点 $i$ が属する木の根から頂点 $i$ までのパス上の頂点重み
を (Value) 一律 $x$ に更新する
    - `PathAffine(i,A,B)` := 頂点 i が属する木の根から頂点 $i$ までのパス上の頂点重み
 (Value) を一律 $A\times\mathrm{Value} + B$ に更新



- 辺の表現には頂点拡張をすると良い