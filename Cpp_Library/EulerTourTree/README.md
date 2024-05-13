# オイラーツアー木
`ドキュメント最終更新 : 2024/5/13`

根付き木を明示的に管理するのではなく、木のオイラーツアーを SplayTree (列型) で管理する。  
ただの Euler Tour と混同しないように注意。


## ライセンス (license)
Copyright (c) 2024 NokonoKotlin
Released under the MIT license
https://opensource.org/licenses/mit-license.php


## NokonoKotlin のお気持ち
- 競プロの提出とかも、ここのコードを使っているなら著作者 (NokonoKotlin) を明記してほしいよ。
- ライブラリに組み込むのは自由だけど、自分のライブラリと称するのはやめてほしいよ。
 
### TODO 

詳しい説明は記事を書いてそこから引用します。


### 概要

SplayTree が管理するオイラーツアーは、オイラーツアーが通った辺を並べるタイプ。  
通った辺を表すインスタンスのことを `EdgeFactor` と呼ぶことにする。  

通常と異なる点として、頂点を表すインスタンス `NodeFactor` もオイラーツアー列に混ざっている。  
頂点 x の `NodeFactor` は、x の部分木のオイラーツアーの区間に登場するが、そのうちのどこに登場するかは不定。  

`NodeFactor`,`EdgeFactor` はどちらも SpalyNode で表現し、オイラーツアーに重複して現れることはない
- u から v に移動したことを表す EdgeFactor は SplayNode(u , v)
- 頂点 u を表す NodeFactor は SplayNode(u , u) 

## 機能 & 仕様
- NodeFactor は頂点を表すので、頂点が持つ値は NodeFactor にのみ持たせる。
- EdgeFactor は Value の値が未定義なので、SplayTree の区間のモノイド積や遅延評価などは、EdgeFactor を無視して計算する。(isNodeFactor というフラグがある)
- #### 基本機能
    - evert(x) := x を連結成分の root にする
    - link(x,y) := x,y に辺を追加 (元々 x,y は非連結)
    - cut(x,y) := x,y から辺を削除 (元々 x,y に辺がある)
    - root(x) := x の属する木の根
    - same(x,y) := x,y が同じ木に属しているか
    - operator [u] := 頂点 u の NodeFactor の SplayNode を返す
        - SplayNode のコピーは、隣接する SplayNode へのポインタを封印して返す
- #### 部分木について
    - 頂点 u の部分木に関するクエリを処理できるが、頂点 u の親頂点 p がわかっていないといけない。
        - 頂点 u の部分木に対応するオイラーツアーの区間を見つけるため (他にうまくやる方法はない)
        - u の親は ETT から計算することはできないので、LinkCutTree と併用する。
            - u が根の場合は p は考えなくて OK (テキトーで OK)
    - SubTree(u,p) := 「u の部分木に対応するオイラーツアー列の区間」に対応する SplayNode のコピーを取得
        - SplayNode のコピーは、隣接する SplayNode へのポインタを封印して返す
    - SubTreeUpdate(u,p,x) := u の部分木 (u 含む) の頂点の持つ値を一律に x に変更する
    - SubTreeAffine(u,p,a,b) := u の部分木 (u 含む) の頂点の持つ値に一律に a かけて b 足す
    - SubTreeAdd(u,p,x) := u の部分木 (u 含む) の頂点の持つ値に一律に x 足す
- tour := オイラーツアー到達順に頂点を並べた vector
- tour_compress := オイラーツアーで、部分木に入る時/出る時 のみ記録
