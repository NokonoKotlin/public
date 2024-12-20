# Li-Chao Tree
`ドキュメント最終更新 : 2024/5/11`

線分の集まりからなる 点の集合 S において、与えられた x 座標に対して S に属する点の y 座標の最小/最大 を取得できる。
線分の集まりはオンラインで更新することができる。  

double はだめ (原因究明中)



Copyright (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)


## NokonoKotlin のお気持ち
ライセンスを守って使って欲しいよ (ライブラリの冒頭に以下を書くだけ)
- Copyright ©️ (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)




### 概要&仕様

- 動的双対セグメント木に線分を載せたもの。
    - データ構造の概要は NokonoKotlin City (私のポートフォリオサイト) を参照。 
    - <a href = "https://qiita.com/NokonoKotlin/items/433f2a65a0d31fac21b6">こちらの Qiita 記事</a> にも NokonoKotlin City からの移植記事があります。
- 線分の集合を管理し、線分上の点に対して与えられた x 座標における Y 座標の min/max を返す
- 線分の集合を、動的＆双対 セグメントツリーに対応させる
    - クエリで要求する座標は全て整数。
    - 取得する Y 座標の型をテンプレートで指定するが、整数型で済む場合もある(後述) 
    - 各ノードは線分 ax+b と、担当する区間 [L,R) を持つ。(双対セグ木の要領で持たせる)
    - セグメントツリーの末端は幅 1 の区間 [x,x+1) に対応させる。
        - [i,i+1) は、x 座標が i にのみ対応している。
        - 整数以外の x 座標は要求されないのでこれで十分。
    - 逆に、点 (a,b) の一点だけのために [a,a+1) のセグメントに直線 y = 0x + b を持たせて良い
        - 幅が 2 以上のセグメントにはちゃんと要求通りの直線を持たせる必要がある (x = a+1 とかにもアクセスされるので)
- コンストラクタには以下を渡す。
    - L , R 
        - x 座標の定義域を閉区間 [L,R] で指定
        - ここもオーバーフロー & TLE の要因になるので必要最低限の定義域にするように注意
    - init_ := [L,R] の y 座標の初期値が init_ である。(直線 : y=init_ が存在する)
    - smaller を指定して、最小か最大のどちらを取得するかを指定する。
        - true  : 最小
        - false : 最大
- 以下の 3 種類の関数を使う際は y 座標の値にも整数型を使える (オーバーフローに注意)。
    - f(X) 
        -  x = X における min/max を取得
    - <b>(推奨)</b> add_segment(l,r,a,b)
      -  閉区間　[l,r] に線分 y = ax+b を追加 (閉区間であることに注意)
    - add_line(a,b) 
        -  定義域全体に直線 y = ax+b を追加
        - |x 座標の上限|×a ぐらいの値になるのでオーバーフローに注意 (できれば add_segment を使う)
- 以下の関数を使う場合は y には有理数を使わないといけない 
    - 誤差の関係で __float128 がオススメ、ただし遅い。
    - (非推奨) write_segment(a,b,c,d,e) 
        -  点 (a,b) と 点 (c,d) をつなぐ。bool exclusive = e
        - exclusive が真の場合、両端の端点を含まない (デフォルトは含まれる)
        - 整数座標しか要求しないので 90° の線分も追加できる。[a,a+1) に対応するセグメントに端点の y 座標を登録する
            - exclusive かつ 90° の場合、長さが正の線分ならば極限を考えて端点の y 座標を採用する       
- TLE,RE,WA,MLE が出たら、以下をゴネゴネすると良い。
    - double の精度を上げる工夫(eps を使った誤差付き有理数の比較など)
    - 他も募集中