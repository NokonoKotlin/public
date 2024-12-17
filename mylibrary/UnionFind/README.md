# UnionFind (rollback 付き)
`ドキュメント最終更新 : 2024/05/21`

ただの UnionFind。rollback もついてる。  

### 注意点
UF 木をマージするデータ構造であり、辺をつなげると捉えるとバグる。  
unite(u,v) で u , v の属する UF 木が同じ場合はそもそも unite を呼び出さず、rollback にもストックされない。  

Copyright (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)


## NokonoKotlin のお気持ち
ライセンスを守って使って欲しいよ (ライブラリの冒頭に以下を書くだけ)
- Copyright ©️ (c) 2024 NokonoKotlin(okoteiyu) Released under the MIT license(https://opensource.org/licenses/mit-license.php)


