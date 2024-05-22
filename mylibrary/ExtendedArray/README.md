# ExtendedArray
`ドキュメント最終更新 : 2024/05/23`

定数倍さえ気にしなければかなり便利。動的セグ木 + 挿入&削除 ができる。   

## ライセンス (license)
Copyright (c) 2024 NokonoKotlin
Released under the MIT license
https://opensource.org/licenses/mit-license.php


## NokonoKotlin のお気持ち
- 競プロの提出とかも、ここのコードを使っているなら著作者 (NokonoKotlin) を明記してほしいよ。
- ライブラリに組み込むのは自由だけど、自分のライブラリと称するのはやめてほしいよ。


### 概要
- T 型の半開区間 : [ L , R ) を持つノードを持つ平衡二分木
    
- ベースは set 型の splay 木で、_sub がついた関数はスプレー木の機能のための内部関数である。  
- upper_bound や modify などの _sub がついていない内部関数もある。
                   
### 設計
- コンストラクタで leftmost と rightmost を与え、アクセスできるのは [leftmost , rightmost) の中のみとする。  
- set で管理される区間で [leftmost , rightmost) をカバーしていることを保証する(ように実装した)。  
    - つまり [leftmost , rightmost) 内の任意の座標は set 内のいずれかの区間に含まれることが保証される!!!!!!
- set で管理される+++区間は重複がなく+++、区間の右端の値の大きさ順に管理されている。  
    - (区間を左から並べると、[a,b) , [b,c) , [c,d) ... みたいになっている。)
    - upperbound(x) :(右端が x 以下の区間の個数) は、x を含む区間の index になる(半開区間なので、左端も x 以下であることが保証される)。
        - ただし、leftmost や rightpost 周りは例外があるので、余分に領域を取ることをお勧めする。
            - (例えば、upper_bound(rightmost) は存在しない区間の index を返すし、lower_bound(leftmost) の index の区間 は leftmost を含まない )
    - 区間に F 型の値(Value)を持たせることで、[leftmost , rightmost) の任意の index にアクセスできる配列のように振る舞う
        - ただし、区間の長さが処理に必要なので、leftmost,rightmost を 最大値ギリギリにするとオーバーフローする。([-1e18 , 1e18) あたりがいい感じかも)
### 最重要実装方針
 座標の半開区間区間 [lef,rig) にクエリが飛んできた時、<b>[lef,rig) が、set で管理される区間の集合でちょうどカバーできるように SplayTree のノードが持つ区間の値を変更する</b>
例 :
```
        l.......r
<...><....><.>......><..>

            ↓↓↓

        l.......r
<...><.><....><.><..><..>
```

### 機能
- 更新
    - RangeUpdate(l,r,v) で、[l,r) の領域の値を一律 v に更新する
    - RangeRffine(l,r,a,b) で、[l,r) の領域の値にアフィン変換を適用できる
    - RangeQuery(l,r) で、[l,r) の領域の値のマージを取得できる
    - ただし、+++++座標は連続+++++なので、厳密に一点を v に更新するといった操作ができない
        - 配列のように i 番目の要素を v に変更する場合、RangeUpdate(i,i+1,v) と読み替える必要がある。
- Sum は 区間長*Value とかになるのでオーバーフローに注意  
- 区間 [L,R) の x の circular shift を以下のように定義する(x>0)。  
    - [L,R) 内の全ての要素の位置を右に x ずらす。ただし、R より右に移動したものは L に戻ってそこから更にずれる。  
    - ex : {1,4,5,2} を 2 シフト →→→→ {5,2,1,4}
    - 左に x ずらす場合は、代わりに (r-l)-x だけ右にずらせば良い  
- circular_shift を駆使すると、数直線に対して区間の 挿入/削除 が行える。
    - LErase(l,r,init_) := [l,r) の要素を削除して残りの要素を ++左詰めする++。
        - 余分にできた領域は init_ で初期化する。  
        - RangeUpdate(l,r,init_) → circular_shift(l,rightmost,-(r-l))
    - RPush(l,r,x) := [l,r] に値が x の領域を確保し、元々あった領域を ++右に押しやる++。
        - rightmost からはみ出た領域は破棄する。  
        - circular_shift(l,rightmost,(r-l)) → RangeUpdate(l,r,x) 
    - RErase(),LPush() も同様の考え方
- 内部実装では SplayTree の [l,r) の部分木の遅延評価やマージを行うので、座標と混同しないように注意。
        - 遅い時はいらない集約や遅延評価を消す。
        - eval() しなくていい時は、eval() の中身を全消ししたり、eval() を呼び出さなくするとかなり速くなる
