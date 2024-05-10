# ArraySplayTree
`ドキュメント最終更新 : 2024/5/10`

列型スプレー木の基本となるライブラリ。  
コードのコメントなども、他のスプレー木ライブラリより丁寧めに書く。   

## ライセンス (license)
Copyright (c) 2024 NokonoKotlin
Released under the MIT license
https://opensource.org/licenses/mit-license.php


## NokonoKotlin のお気持ち
- 他人に共有しない場所で個人使用してほしいよ。
- 競プロの提出とかも、ここのコードを使っているなら他人に共有しないでほしいよ。
- ライブラリに組み込むのは自由だけど、自分のライブラリと称するのはやめてほしいよ。



## 仕様 & 使い方
- 数列型の Splay Tree 
- 頂点にアクセスするたびにその頂点を splay する。
- SplayNode というデータ構造に必要なデータを持たせ、Root から探索することでアクセスする。
- SplayNode にモノイド積などの集約データを持たせる(Sum,Min など) 
- 基本的に、機能の変更をしたい場合は SplayNode のメンバや update 方法を編集することになる
- merge/split ベースの実装
    - merge と split が++全く++反対の操作であることで、計算量を削減できる
        - merge(lef,rig) : rig の左の子に lef をくっつける
        - split(i,ROOT) : i 番目のノードの左の辺をカットして (lef,rig) に分割する
- insert_sub(ind,NODE,root) := ind番目にNODEが挿入される
- Delete_sub(ind ,root) := ind番目を削除(返り値はfirstが新しいroot , secondが削除したNode)
-  _sub で実装される関数は、スプレー木を管理するために必要な最低限の機能で、クラス内部での処理のための関数。
    -  _sub 関数は、基本的な操作のみを担当するので、ユーザー向けの機能は、_subでない関数で実装する。
    - (重要)  _sub 関数の中身は、_sub 関数だけで完結させる!!!!!!!
- find(x,c) : a_i = x である様な i を c 個まで検索する。
    - find() を使うには、テンプレートで _find_ = true を指定する (指定せずに使うと RE)
    - 以下のタイミングで、x を持つノードのポインタ pos[x] を編集する。
    - insert するとき / Delete するとき /update_valするとき
    - Delete(i)の際、メモリを解放する代わりに削除した Node の isThisDeleted を true にしている。
    - (重要) 前提として Splay Tree の連結性が必要 ( Splay Tree を分割してから find() を使うのは未定義動作 )
    

- 区間クエリ
    - 区間のモノイド積ができる (総和,Min など)
    - 区間作用について
        - _find_ が true なのに更新をかけると RE になる。
        - 半開区間 [l,r) の要素を reverse することができる (区間反転)
            - _find_ が true の場合の計算量解析不明 , 多分 O(NQ) になるケースもある
        - 区間内の要素に一律に更新をかけることができる
            - 区間内の要素に対して、代入/アフィン変換/chmax など
- 計算量について :
    - 遅い時はいらない集約や遅延評価を消す。
    - eval() しなくていい時は、eval() の中身を全消ししたり、eval() を呼び出さなくするとかなり速くなる

### 設計
- 内部処理 (Node の splay や _sub がついた関数) を信頼する  
- 関数に渡す SplayNode や、関数から返される SplayNode は全て update (& eval) されている必要がある
- 抽象化はせずに、モノイド積や遅延評価作用を例示することで、書き換えを容易にする。  
    - Sum , Min , Max , RangeUpdate , RangeAffine , RangeReverse で特殊化している
    - 不必要なモノイド積や作用の記述を消すと速くなる  
    - 特殊化されたモノイド積や作用は以下に書き込まれているので、書き換えるときは主に以下を書き換える
        - Node のメンバ (lazy_affine や set_LazyAffine)
        - Node の update ,eval の処理
        - RangeHoge_sub 関数で、Node に遅延評価を載せる
- T を自作クラス (modint) などにするときは、pos メンバのキーを long long などにしておく 
        (modint は long long にキャストできる設計なので)
- merge と split が++全く++反対の操作であることで、計算量を削減できる
    - merge(lef,rig) : rig の左の子に lef をくっつける
    - split(i,ROOT) : i 番目のノードの左の辺をカットして (lef,rig) に分割する
- 内部実装の merge , split の使用法は最適化されていることに注意、変に変更したら遅くなる
    - split は右から、merge は左から行うと良いです
- ヒープ上のオブジェクトのポインタを扱う関係で、コピー/ムーブを禁止する。
    - 宣言後に代入するときは、copy(X) で、X の中身を再度生成して自身に格納する。
    - 平衡二分木は、アクセスするだけでも構造を変えるので、const 修飾できない。
        - 右辺値用と左辺値用それぞれの copy を定義する。
- get(i) や getRange(l,r) で取得するノードは、要素取得の用途での使用のみを想定しているので、 隣接するノードへのアクセスを封印したコピーを返すようにしてある。
