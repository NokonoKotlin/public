# オフラインクエリ処理テンプレート
`ドキュメント最終更新 : 2024/05/20`

クエリの取り消しがある場合に、クエリの有効期限を区間として捉えて、有効期限のセグメントツリーを構築する。   
<a href = "https://nokonokotlin.github.io/Home/Contents/algorithm/Articles/typical/OfflineQuery.html">参考記事(自筆)</a>

Copyright (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)


## NokonoKotlin のお気持ち
ライセンスを守って使って欲しいよ (ライブラリの冒頭に以下を書くだけ)
- Copyright ©️ (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)

## 使い方
- クエリが 1 つ飛んでくるごとに時刻が 1 進む問題が対象。  
- 更新クエリを処理するためのデータ構造を必要とし、直近の更新クエリを無かったことにする rollback 機能を持つ必要がある。
    - これらの操作 + 取得クエリは高速である必要がある
    - 更新クエリの引数は update 構造体 , 取得クエリの引数は get 構造体 に対応
- クエリの寿命を列の区間と捉え、セグメントツリーに有効期限をのせる
### クエリの適用
- クエリの適用は、セグメントツリーの根から DFS を行い、
    - あるセグメントに入った時、そのセグメントの有効期限にかかっている更新クエリをデータ構造に適用
    - あるセグメントから出る時、入る時に適用したクエリ数だけデータ構造を roolback (無かったことにする)
- 末端ノード x に到達した時点で、データ構造には時刻 Range[x].first (ノード x の区間の左) における更新クエリが全て適用されている。
- 時刻 qtime に取得クエリが登録されているなら、このタイミングで ans[qtime] に書き込む

### 設計 
- データ構造 S は自由だが、以下を持つ必要がある。  
    - 更新クエリ update に対応する更新クエリ処理
    - 取得クエリ get に対応する取得クエリ処理
    - 直近の更新クエリ適用を無かったことにする rollback 関数
- データ構造 S に初期値が必要な場合、以下のように全ての時刻にクエリを適用すれば OK
    - i 番目を a[i] で初期化する場合 : QS.add_query(0,QS.tlimit(),QS.make_query(i,i+1,a[i]));
    - add_update_query(l , r , q) : 有効期限が [l,r) の更新クエリ q を追加する
    - add_get_query(t , q) : 時刻 t における取得クエリ q を登録する (ask を参照)
    - apply(引数入れちゃダメ) : クエリを処理して ans に答えを書き込む
    - ask[t] := 時刻 t における取得クエリについて {存在するか ,  取得クエリ} のペア
    - ans[t] := 時刻 t における取得クエリが存在するなら、その答えを保存しておく