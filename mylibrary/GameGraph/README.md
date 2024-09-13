# グラフ上のゲーム
`ドキュメント最終更新 : 2024/05/19`

グラフ上のゲームの勝敗判定など。未完成 & 詳細なコメントなどをつけていない

## ライセンス (license)
Copyright (c) 2024 NokonoKotlin
Released under the MIT license
https://opensource.org/licenses/mit-license.php


## NokonoKotlin のお気持ち
ライセンスを守って使って欲しいよ (ライブラリの冒頭に以下を書くだけ)
- Copyright ©️ (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)


## コメント

- game探索のプロトタイプ(後退解析,Grundy数)
- add_edgeで有向辺を加える。
- Gnum や Bstate で状態を取得する際に自動で grundy 数や後退解析の結果を計算してくれる。
    - 初回のみ O(|E|)
    - 計算済みかのフラグを持っておいて、重複して計算されない様にする
    - 新たに辺を追加したら、次の取得次に再計算する (O(|E|))
- Bstate(x) := 後退解析結果で、頂点 x の状態 
    - 0(終了状態 : 先手の番でゲームが終了)　, 1(非終了状態 : 後手の番でゲームが終了) , -1(ゲームが無限に続く) 
- Gnum(x) := 頂点 x の Grundy 数