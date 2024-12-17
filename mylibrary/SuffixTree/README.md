# Suffix Tree
`ドキュメント最終更新 : 2024/5/7`

連続部分列に対する解法 Tier 1 データ構造。


Copyright (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)


## NokonoKotlin のお気持ち
ライセンスを守って使って欲しいよ (ライブラリの冒頭に以下を書くだけ)
- Copyright ©️ (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)



### 概要 & 前提

列 S の Suffix Tree を動的に構築する。
- Suffix Tree は S の連続部分列を辺ラベルに持つ木である。
    - 各辺は S のどの区間かを表す半開区間を持つ
    - root から列 R に沿って移動した結果到達した地点を「R に対応する地点」と呼ぶことにする
    - 根から地点 P までの辺上に存在する要素を連結してできる列を「P に対応する列」と呼ぶことにする
- S の末尾に要素を加えながら、Suffix Tree にも反映させることができる (オンライン構築) 。
#### 使用上の注意
- Suffix Tree を動的に構築中の状態を <b>オンライン</b> と呼び、動的構築が完了した状態を <b>オフライン</b> と呼ぶ。  
- オンライン構築が完了した時点で determine() を呼び出して Suffix Tree が確定したことを明示する必要がある。
    - オンライン状態でできることは多くない
- determine() を呼び出すと Suffix Tree の状態が確定し、S の全ての接尾辞それぞれについて、対応するノードが存在するようになる。
- #### Suffix Tree の機能
    - LCA や 木 DP などの操作を特殊化して内蔵してある。
        - LCA や 木 DP を行うためには、関連するデータを計算しておく必要がある (LCA の親ノードのテーブルなど) 。
    - これらに必要なデータの計算を行うのは、<b>ユーザがその機能を必要としたタイミング</b>で行うのが望ましい。
        - 本来であれば LCA や 木DP の一部は determine() を呼ぶ前でも意味を持つが、<b>計算されるデータの一貫性を重視して、これらの計算は必ず determine() 後に行われるものとする。</b> 
    - determine() 後であれば、ユーザがその機能を必要とした際に<b>最初の 1 度だけ</b>各種の `init_xxx` 関数が呼び出されて初期化される。
        - データの計算が行われるのは最初に呼ばれた時のみ。このことは<b>データ計算を determine() 後に限定したことと相性が良い。</b>
        - 各種データについて、計算が行われたかどうかのフラグを管理して、何度も重複してデータ計算が行われないようにする。

### 仕様 & 設計
- T 型の列 : S に対して、頂点 0 を根とする Suffix Tree を構築。
- 変数などは基本的に 0-index。
- 列は string / vector の両方に対応したいので template&lt;class VecOrStr> とする
- 頂点は Node クラスのポインタで扱うが、ポインタで扱うので中身を変更しないように注意
    - ノードの番号は生成順 (root の id が 0)。
- 空列も接尾辞として含み、辞書順で最小とする(suffix_id は |S| とする)
    - 空列である接尾辞に対応するノードが root であるとする。
- #### Node クラス (頂点 & 辺のデータを保持する)
    - **id** 
        -  頂点の生成順に割り振られる
    - **parent** 
        -  自身の親 Node のポインタ
    - **child**
        - child[x] := 先頭が x の辺の先の Node のポインタ (辺は先頭の要素がユニーク)
    - <b>cover = {int,int*}</b>
        - 自身に入ってくる辺に対応する列が S のどの連続部分列かを表す 
        - S の長さが不定なので、S の終端を表すときは S_end_Symbol という変数を参照するポインタで表す
        - S の半開区間 [cleft() , cright() ) を担当する    
    - get_path_length() 
        - このノードに対応する列の長さ
    - isEndPoint() 
        - 根からこのノードまでの経路上の要素の列が S の接尾辞かどうか
        - suffix_id : 接尾辞に対応する場合、何番目から始まるものか(0-index)(なければ-1)
    - SuffixLink 
        - 「このノードに対応する列の先頭を削除した列」に対応するノードのポインタ
        - 実装の都合上、root からは root につながる。
    - 木 DP の変数なども持つ
        - SubTreeEndPointCount := 自分以降の部分木に含まれる EndPoint の個数
        - SubTreeKindOfPrefix  := 自分から根方向に移動して得られる列の種類数 (空列を含まない)
- #### PositionData クラス (Suffix Tree 上の地点データを保持する)
    - RecentNode から NextNode の方向に place だけ進んだ地点を指す。
        - ちょうど頂点の上の場合は、NextNode を nullptr , place を 0 とする。
    - get_path_length() 
        - 今いる地点に対応する列の長さを返す
    - deque&lt;T> refer
        -  今いる地点に対応する列。
        - refer を明示的に持たないことで、地点データを定数サイズオブジェクトとして扱える。
            - そのような場合は **refering を false** としておく。
    - walkSuffixLink() : 自分自身を、現在地に対応する列の先頭削除した地点データに変更する (refer も変更される) 
        - refer を明示的に持たない場合
            - PositionData は何らかの列の連続部分列なので、refer の代わりにその列の参照と、どこから始まる連続部分列かを表す整数を引数に取る
            - 参照する列は必ずしも Suffix Tree の元の列である必要はない
- #### 初期状態オブジェクトを返すユーザ インターフェース
    - SuffixTree::get_root() : 根ノードのポインタを返す。
    - SuffixTree::from_root(bool refering = true) : 根に対応する地点を返す。(地点データの refering を指定する)

### 実践
- #### 割と重要な性質
    - Suffix Tree 上の各地点は S の連続部分列と 1 対 1 対応している (連続部分列を重複なく列挙しているのと同様)
    - search(l,r)
        - S の区間 [l,r) に対応する地点データを返す。ただし refer は非明示的 ( refering = false )。
    - decode_suffix(PositionData x) 
        - 地点 x に対応する列を連続部分列として含む S の接尾辞の開始 index のうちの 1 つを返す。
        - 返り値を l とすると、地点 x は S の [ l  , l + |x| ) に対応する (|x| は地点 x に対応する列の長さ)
- #### Suffix Tree の構築
    - オンラインで S を編集 (push_back のみ) しながら Suffix Tree にも変更を反映する
    - <b>オンライン編集の必要がなくなれば、Suffix Tree を確定させる (determine())</b>
    - Suffix Tree 構築メソッド
        - push(s_) 
            -  オンライン構築中の S の末尾に s_ (列 or T 型単体) をくっつけて、Suffix Tree にも反映させる。
               - オンライン構築中の Suffix Tree では、tree_dp 系の、全ての接尾辞を格納している前提の操作は行えない。
        - determine() 
            -  オンライン構築を終了し、オフラインにする
            - この操作によって、全ての接尾辞が Suffix Tree のノードのいずれかと対応するようになる。
            - determine() しないと 木 DP などができないので制限される機能がある
        - build(s) 
            - 初期状態から push(s) + determine()
            - <b>これ以前の Suffix Tree のデータは全て破棄される</b>

- #### 探索メソッド
    - find(t) 
        -  t が S の連続部分列かどうかを判定 
        - オンラインでも OK
    - count(t) 
        -  t が S の連続部分列として何回現れるか計算
        -  オフライン限定
    - search(t)
        -  t を検索し、移動できた地点までのデータを返す
        -  オンラインでも OK
    - search(l,r) 
        -  S の区間 [l,r) に対応する地点データを返す。ただし、refer は非明示的
        -  オンラインでも OK
- #### データ取得メソッド
    - get_node(i) 
        - 頂点番号が i のノードのポインタを返す
    - get_suffix_node(i) 
        - S[i] から始まる接尾辞に対応するノードのポインタを返す
        - オフライン限定
    - get_depth_as_tree(i) 
        - root から頂点 i までの辺の本数を返す
        - オフライン限定
    - edges() 
        -  Suffix Tree の辺集合を返す
    - dfs(f) 
        - root から dfs するときの到達順に頂点のポインタを並べて返す
        - f が true なら辞書順最小優先で dfs
    - LCA(地点のデータ 2 つ) 
        -  地点(辺の途中含む)の LCA のデータを返す。
- #### その他機能
    - クラス外で Suffix Tree 上地点の移動をする際は、コメントに従って以下を使う    
        - findValidPath(p,c) 
            - 地点 p から c の方向に移動できるか
        - walkdown(p,c) 
            - 地点 p から c の方向に移動し、p を書き換える
    - size() 
        -  S のサイズを返す
    - [i] 
        -  S[i] を read-only でアクセス
    - suffix_array()
    - get_Kth_subStr(K)
