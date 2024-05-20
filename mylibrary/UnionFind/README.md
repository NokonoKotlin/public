# UnionFind (rollback 付き)
`ドキュメント最終更新 : 2024/05/21`

ただの UnionFind。rollback もついてる。  

### 注意点
UF 木をマージするデータ構造であり、辺をつなげると捉えるとバグる。  
unite(u,v) で u , v の属する UF 木が同じ場合はそもそも unite を呼び出さず、rollback にもストックされない。  

## ライセンス (license)
Copyright (c) 2024 NokonoKotlin
Released under the MIT license
https://opensource.org/licenses/mit-license.php


## NokonoKotlin のお気持ち
- 競プロの提出とかも、ここのコードを使っているなら著作者 (NokonoKotlin) を明記してほしいよ。
- ライブラリに組み込むのは自由だけど、自分のライブラリと称するのはやめてほしいよ。


