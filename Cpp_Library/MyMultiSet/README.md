# MyMultiSet (平衡二分木)

`ドキュメント最終更新 : 2024/5/7`

C++ 使いの競プロ er なら誰しもが思う「`std::set` にもっと機能を付けてくれ〜」を解決したデータ構造です。
(`std::set` との互換性は全くありません)


##### TODO_FLAG
自作 splay 木の使用方法についての注意事項をまとめて、そこにリンクする。

## ライセンス (license)
Copyright (c) 2024 NokonoKotlin
Released under the MIT license
https://opensource.org/licenses/mit-license.php


## NokonoKotlin からのお願い
- 他人に共有しない場所で個人使用してほしいよ。
- 競プロの提出とかも、ここのコードを使っているなら他人に共有しないでほしいよ。
- ライブラリに組み込むのは自由だけど、自分のライブラリと称するのはやめてほしいよ。




### 概要 & 仕様
- ノードが type_key 型のKey と type_value 型のValueを持つ Splay Tree で、(Key , Value) が +++ 辞書順 +++ に並んでいる
- C++ の set とは異なり、Key の重複を許す。(Value も当然重複 OK)
- get(i) や [i] で i 番目のノードのコピーを取得 (隣接頂点へのアクセス (ポインタ) が封印されたものを返す) (0-index)
- GetRange(l,r) は半開区間 [l,r) をカバーする部分木の根のコピーを返す。get 同様に、隣接頂点のポインタは封印されている。
    - GetRange(l,r).Sum_val のようにして [l,r) の持つ要素のモノイド積を取得する
- #### (Key,Value) に関して以下の操作が可能
    - insert_pair(k,v)  
        -  (k,v) を持つノードを追加
    - erase_pair(k,v) 
        -  (k,v) を持つノードを(存在すれば)削除する。
    - upper_bound_pair(k,v) 
        - (Key,Value) が辞書順で (k,v) 以下の要素数を返す
    - lower_bound_pair(k,v)  
        - (Key,Value) が辞書順で (k,v) 未満の要素数を返す
    - find_pair(k,v) 
        - (Key,Value) が (k,v) である要素の index を返す。存在しなければ -1 を返す。(0-index)、    
- #### ノードの Key だけに注目して、通常の set のように振る舞わせることもできる。  
    - insert(k) 
        -  Key = k である要素を追加する。ただし、Value を指定しないので Value は未定義とする。
    - erase(k) 
        -  Key = k である要素を一つ削除する。ただし、Value について特に指定しないことに注意。
    - upper_bound(k) 
        - Key が k 以下の要素数を返す
    - lower_bound(k)
        - Key が k 未満の要素数を返す
    - find(k)
        - Key が k である要素の index を返す。存在しなければ -1 を返す。(0-index)、
- #### ペアを持つノードとKeyしか持たないノードが混在するといけない
    - insert() を呼び出した時点で、Value が未定義の要素が存在することになる
        - insert() を呼び出した後は upper_bound_pair , find_pair , RangeValueMaxQuery などの
        - Value に関する関数を呼び出すとランタイムエラーになるようにしました。
    - (ただし、Key だけに関係する関数は変わらず使用できる。)
- (Key,value) 辞書順や Key の昇順で並んでいると書いたが、CompareNode() を変更することで要素の並び順を変更できる。

### 設計
- 関数に渡す SplayNode や、関数から返される SplayNode は全て update (& eval) されている必要がある
- ヒープ上のオブジェクトのポインタを扱う関係で、コピー/ムーブを禁止する。
    - 宣言後に代入するときは、copy(T) で、T の中身を再度生成して自身に格納する。
    - 平衡二分木は、アクセスするだけでも構造を変えるので、const 修飾できない。
        - 右辺値用と左辺値用それぞれの copy() 関数を定義する。
    - get(i) や getRange(l,r) で取得するノードは、要素取得の用途での使用のみを想定しているので、隣接するノードへのアクセスを封印したコピーを返すようにしてある。