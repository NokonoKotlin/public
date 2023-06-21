# レドメ
[![Anurag's GitHub stats](https://github-readme-stats.vercel.app/api?username={NokonoKotlin})]
(https://github.com/anuraghazra/github-readme-stats)


Link-Cut Tree  :  グラフに関するデータ構造のライブラリ
	辺を自由に切ったり張ったりした上で、Tree上のpathに関する情報を管理できる。
	LCAや、根の張り替えにも対応
	構築以外の各操作はO(logN)





Searchable Array  :  列に関するデータ構造のライブラリ	
	配列としての基本操作は、vectorなどと同じ追加、削除、アクセスが可能	
	ただしどの操作も O(logN)かかる (対して、std::vectorは基本的にO(1)だが、メモリの操作に時間がかかることがある)
	また基本操作に加え、任意の区間の和、最小値などの値を取得可能
	発展的な操作として、任意の区間に対して右/左シフトが可能 (右シフトの例 : { 1 3 2 } → { 2 1 3 } )
	
	メイン機能として、get_position(x) := 数列内に現れる値 : x の index を任意の個数列挙できる。
	通常のvectorでこれを実装しようとすると、O(N^2) だが、このライブラリではO(NlogN)で解決。



ChatApp  :  以前作った簡易的なアプリ
	初めて作った動くアプリケーション
	コードに関してメモがあるが、あまり覚えてはいない。
	flaskで作った。
	チャットルームとTCP/IPで通信を行い、チャットメッセージをデータベースに入れたり出したりして表示、受信する。
	