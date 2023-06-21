# NokonoKotlin Library

<p align="left">
	 <img alt="Top Langs" height="160px" src="https://github-readme-stats.vercel.app/api/top-langs/?username=NokonoKotlin&layout=compact&count_private=true&show_icons=true&theme=tokyonight" />
	<img alt="github stats" height="160px" src="https://github-readme-stats.vercel.app/api?username=NokonoKotlin&theme=tokyonight&show_icons=ture" />
</p>

<p align="left">
	<img alt="github stats" height="100px" src="https://github-profile-trophy.vercel.app/?username=NokonoKotlin&theme=tokyonight&column=7" />
</p>




<h3 align="left">My WebSite:</h3>
<p align="left">
	<a href="https://nokonokotlin.github.io/Home/index.html" target="blank"><img align="center"
	alt="nokonokotlin" height="30" width="30" src = "https://raw.githubusercontent.com/NokonoKotlin/public/master/static/images/NokonoKotlin.jpeg"></a>  
	<a href="https://nokonokotlin.github.io/Home/database/algorithm_database/index.html" target="blank"><img align="center"
	alt="nokonokotlin" height="30" width="40" src = "https://raw.githubusercontent.com/NokonoKotlin/public/master/static/images/algo.jpeg"></a>
</p>





<h3 align="left">Connect with me:</h3>
<p align="left">
	<a href="https://twitter.com/nokonokotlin" target="blank"><img align="center" src="https://raw.githubusercontent.com/rahuldkjain/github-profile-readme-generator/master/src/images/icons/Social/twitter.svg" alt="nokonokotlin" height="30" width="40" /></a>
	<a href="https://codeforces.com/profile/nokonokotlin" target="blank"><img align="center" src="https://raw.githubusercontent.com/rahuldkjain/github-profile-readme-generator/master/src/images/icons/Social/codeforces.svg" alt="nokonokotlin" height="30" width="40" /></a>
</p>




<p align="left"> <img src="https://komarev.com/ghpvc/?username=nokonokotlin&label=Profile%20views&color=0e75b6&style=flat" alt="nokonokotlin" /> </p>

<p align="left"> <a href="https://twitter.com/nokonokotlin" target="blank"><img src="https://img.shields.io/twitter/follow/nokonokotlin?logo=twitter&style=for-the-badge" alt="nokonokotlin" /></a> </p>







<h3 align="left">Languages and Tools:</h3>
<p align="left"> 
	<a href="https://www.w3schools.com/cpp/" target="_blank" rel="noreferrer"> <img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/cplusplus/cplusplus-original.svg" alt="cplusplus" width="40" height="40"/> </a> <a href="https://flask.palletsprojects.com/" target="_blank" rel="noreferrer"> <img src="https://www.vectorlogo.zone/logos/pocoo_flask/pocoo_flask-icon.svg" alt="flask" width="40" height="40"/> </a> 
	<a href="https://www.java.com" target="_blank" rel="noreferrer"> <img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/java/java-original.svg" alt="java" width="40" height="40"/> </a> <a href="https://www.mysql.com/" target="_blank" rel="noreferrer"> <img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/mysql/mysql-original-wordmark.svg" alt="mysql" width="40" height="40"/> </a> 
	<a href="https://www.python.org" target="_blank" rel="noreferrer"> <img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/python/python-original.svg" alt="python" width="40" height="40"/> </a> 
</p>


## algorithm/LinkCutTree  
	辺を自由に切ったり張ったりした上で、Tree上のpathに関する情報を管理できる。  
 	LCAや、根の張り替えにも対応。構築以外の各操作はO(logN)  
	任意のパス上の頂点の持つデータのマージ結果を計算できる。  
	辺にデータを持たせたい時は頂点と頂点の間に余分に頂点を作り、余分な頂点にデータを持たせてそれを辺のデータとして扱う。

  
    
## algorithm/Searchable Array  
	配列としての基本操作は、vectorなどと同じ追加、削除、アクセスが可能	 
	ただしどの操作も O(logN)かかる (対して、std::vectorは基本的にO(1)だが、メモリの操作に時間がかかることがある)  
	また基本操作に加え、任意の区間の和、最小値などの値を取得可能   
	発展的な操作として、任意の区間に対して右/左シフトが可能 (右シフトの例 : { 1 3 2 } → { 2 1 3 } )   
	
	メイン機能として、get_position(x) := 数列内に現れる値 : x の index を任意の個数列挙できる。   
	通常のvectorでこれを実装しようとすると、O(N) だが、このライブラリではO(logN*(取得する個数))で解決。   



## ChatApp 
	初めて作った動くアプリケーション  
	コードに関してメモがあるが、あまり覚えてはいない。  
	flaskで作った。  
	チャットメッセージをデータベースに入れたり出したりして表示、受信する。   
	一番大事な特徴は、位置情報を使っており、自分の近くにいる人とのみ会話可能(厳密には少し違うけど)   

	