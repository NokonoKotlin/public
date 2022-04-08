#include <curses.h>
#include<cstdlib>
#include<vector>
#include<unordered_set>
#include<set>
#include<map>
#include<string>
#include<iostream>

#include <utility>
#include<random>
#include <sstream>

#include <time.h>



/*
	ボタン操作:
  	w -> 上ボタン
	s -> 下ボタン
	d -> 右ボタン
	a -> 左ボタン

	j -> モード切り替えボタン
	k -> 決定ボタン

*/




/*

	クラス間の関係性:参照でループが起きないようにする
	A -> B　:= クラスBはクラスAを参照することができるが、クラスAはクラスBを参照できない
	直感的に言い換えると、 A->B := クラスBからクラスAの中身をいじることができる

	(Stage , Enemy , Bomb , Bullet , PlayerState) -> GameWindowProcesser -> MenuProcesser


	Global領域では実行を伴う処理をしない(mainでglobal変数を初期化してから実行を開始する)



	PlayerState:=Plyaerが持つ属性をもつ
	{
		atk,def,hp,luck
		duration ... 持続性


		SlowMagic , Clairvoyance ... Magicのフラグ
		WideBullet , PenetrateBullet　... 弾丸

		BombNumber ...　一度に落ちる爆弾の個数

	}








	Stage ... gameWindowProcesserで使う変数の初期化。MenuProcesserからもアクセスする
	init() ->{
		StageMap,StageWindowSize,など
		IDに対応したStageMapを返す
		StageWindoowSize :=　表示する半径
	}	





	Enemy , Bullet,Bomb ... 属性やステータスを保存





	GameWindowProcesser := {
		ゲームの画面を制御

		init(Stage){
			...変数を初期化
			MenuProcesserからステージを変更するときにも使う
		}

		BulletList , BombList , EnemyList ..現在存在する弾丸などのlist

		display , displayUI ->{
			UIやゲーム画面を表示
		}

		generateBullet ...弾丸を生成
		generateBomb ...爆弾を生成
		generateEnemy ...敵を生成


		Update_Bullet,Update_Bomb,Update_Enemy .. 更新

		processlimit_xxx ... xxxの処理を行う頻度

		playermove ... playerをstage上で動かす

	}







	MenuProcesser ->{
		ここは一番上のクラスで、さまざまなクラスにアクセスして干渉することができる

		Layer .. 現在いるlayer
		SelectCursor .. 選択している項目
		
		purchase ... 買い物{
			処置が複雑...
			ShopItemListは手動で更新していて、
			配列の範囲外かどうかや、商品が存在するかどうかも全てif文で判断する


		}

		display　、displayUI -> {
			UIや買い物画面を表示
			買い物画面はLayerやSelectCursorでSHopItemListのindexを管理し、矢印や選択肢を表示する


		}



	}









*/





template<class T> inline std::string toString(T x) {std::ostringstream sout;sout<<x;return sout.str();}

template<class T> 
T max(T a, T b){
	if(a>b)return a;
	else return b;
}


template<class T> 
T min(T a, T b){
	if(a<b)return a;
	else return b;
}

#define STAGESIZE 150

//ゲーム内に表示される(変数的な)文字列の最大長
#define STRING_SIZE_MAX 15

#define PLAYERPLACE  1



//=========================================================================================
//====================================================================================================================================================================
//====================================================================================================================================================================
//====================================================================================================================================================================
//====================================================================================================================================================================
//====================================================================================================================================================================



//プログラム内のグローバル変数


//Playerの行為に関するもの========================================================================================================

long long PlayersScore = 0;

long long Sum = 0;







//stageのmapの情報==================================================================================================================




//マップの個数
int MapDataList_Size;


//map情報の配列(NxNの正方行列を１次元のstringで表現)
std::vector<std::string> MapDataList;

//Mapのサイズ
std::vector<int> MapSizeList;

//Mapの名前
std::vector<std::string> MapNameList;

//MapID := mapに関する配列の0-index
int MapID_now;//現在のマップのID


std::string MapInfomation0_name = "Ruin";
int MapInfomation0_size = 30;
std::string MapInfomation0 = 
	".............................."
	".....................GG......."
	"..GG...............GGG........"
	".............................."
	"..........GGGG..G............."
	".........GGGGGGGG............."
	"........GGGGGGGG.............."
	".G.......GGGGGG..............."
	"....###......................."
	"..##BBB##....................."
	".#BBBBBBB#............GGG....."
	".#BBBBB##............GG......."
	".##BB##...............G......."
	"...##........GGG.............."
	"............GGGGG............."
	".............GGG.............."
	".............................."
	"...............###............"
	"..............#BBB#..........."
	"......GGG......###............"
	".....GGG.........GG..........."
	"....GGG...........GG....#....."
	"..............###......###...."
	".............#...#......#....."
	"....GG.......#...#............"
	".............#G..#............"
	"......GGG....#...#............"
	"...................###........"
	".....G.............#B#........"
	"....GG.............###........"; 



std::string MapInfomation1_name = "Ancient Artifact";
int MapInfomation1_size = 30;
std::string MapInfomation1 = 
	".............................."
	".......####......####......##."
	"..#####....###...#......##...."
	"..#.....#....##..#.....#....#."
	".#.....###....##..#..##....#.."
	"....###..................#...."
	".........#............#......."
	"......##.....###....####......"
	".....#......#...#.......#....."
	"..#..#......#...#......#......"
	"......#.....#...#............."
	".............###.............."
	"....####............#######..."
	".......##................###.."
	"........##...................."
	"..##.........................."
	"..##...........#######........"
	".................###.........."
	"........#.........#..........."
	".......###..............###..."
	"........#................#...."
	".............###.............."
	"..............#.....##........"
	"....###.............##.....#.."
	"....#.#......................."
	"......#####........#######...."
	"..........#####........####..."
	"..##.........................."
	"...###........###............."
	"..............................";



std::string MapInfomation2_name = "Green Green";
int MapInfomation2_size = 30;
std::string MapInfomation2 = 
	".............................."
	".....................GG......."
	"...................GGGGG......"
	".....................GGGG....."
	"..........GGGG..G.....GGG....."
	".........GGGGGGGG............."
	"........GGGGGGGG.............."
	".........GGGGGG..............."
	"....GGG......................."
	"..GGGGGGG....................."
	".GGGGGGGGG............GGG....."
	".GGGGGGGG............GG......."
	".GGGGGGG..............G......."
	"...GGGG......GGG.............."
	"............GGGGG............."
	".............GGG.............."
	".............................."
	"...............GGG............"
	"..............GGGGG..........."
	"......GGG......GGG............"
	".....GGG.........GG..........."
	"....GGG...........GG....G....."
	"..............GGG......G.G...."
	".............G...G......G....."
	"....GG.......G...G............"
	".............GG..G............"
	"......GGG....G...G............"
	"...................GGGG......."
	".....G.............GGG........"
	"....GG..............GG........"; 




//========================================================初期化実行関数=========================================================
//================================================================================================================================




void InitGlobalValue(){

	//昇順(MapIDがindexなので)
	std::vector<std::string>(0).swap(MapDataList);
	std::vector<int>(0).swap(MapSizeList);
	std::vector<std::string>(0).swap(MapNameList);

	MapDataList.push_back(MapInfomation0);
	MapDataList.push_back(MapInfomation1);
	MapDataList.push_back(MapInfomation2);


	MapSizeList.push_back(MapInfomation0_size);
	MapSizeList.push_back(MapInfomation1_size);
	MapSizeList.push_back(MapInfomation2_size);


	MapNameList.push_back(MapInfomation0_name);
	MapNameList.push_back(MapInfomation1_name);
	MapNameList.push_back(MapInfomation2_name);

	MapDataList_Size = MapDataList.size();

	return;


}


//======================================================Shopの売り物たちを初期化======================================================
//=================================================================================================================================

//項目名 : Item[x] := x番目の項目名
std::vector<std::string> Items;

//項目の中身 : G[x] := x番目の項目の中身(商品)

std::vector<std::vector<std::string> >ShopItemList;

//項目名及び商品名は短めが好ましい

//項目の個数
int ItemSize = 5;


//商品の値段
std::map<std::string , long long> Values;


//商品の説明(改行：\n を入れてもいいが、しすぎないように)
std::map<std::string , std::string> Explanation;





void InitShopItems(){
	//ItemとShopItemListを要素数ItemNumで初期化
	std::vector<std::vector<std::string> >(ItemSize).swap(ShopItemList);
	std::vector<std::string>(ItemSize).swap(Items);
	Items[0] = "Upgrade";
	Items[1] = "Buff Effect";
	Items[2] = "Magic";
	Items[3] = "Weapon";
	Items[4] = "Travel";
	


	//各販売物の値段を初期化

	//ItemCursor = 0 (Upgrade)
	ShopItemList[0] = { "training(Atk +1)" , "training(HP +1)" , "training(Def +1)" , "meditation(Luck +1)"};
	Values[ShopItemList[0][0]] = 2;//atk+1
	Values[ShopItemList[0][1]] = 3;//HP+1
	Values[ShopItemList[0][2]] = 2;//Def+1
	Values[ShopItemList[0][3]] = 6;//Luck+1
	Explanation[ShopItemList[0][0]] = "Add 1 to Your Atk";
	Explanation[ShopItemList[0][1]] = "Add 1 to Your HP";
	Explanation[ShopItemList[0][2]] = "Add 1 to Your Def";
	Explanation[ShopItemList[0][3]] = "Add 1 to Your Luck";

	//ItemCursor = 1(Buff Effect)
	ShopItemList[1] = {"Absolute Guard" , "Big Bullet" , "CounterAttack" };
	Values[ShopItemList[1][0]] = 10;//absolute guard
	Values[ShopItemList[1][1]] = 2;//Big Bullet
	Values[ShopItemList[1][2]] = 10;//CounterAttack
	Explanation[ShopItemList[1][0]] = "decrease damage";
	Explanation[ShopItemList[1][1]] = "increase damage to enemy once";
	Explanation[ShopItemList[1][2]] = "Player's CounterAttack";



	//ItemCursor = 2(Magic)
	ShopItemList[2] = { "Cure" , "Explosion" , "Steal" , "Clairvoyance" , "Slow"};
	Values[ShopItemList[2][0]] = 4;//cure .. Hpを回復
	Values[ShopItemList[2][1]] = 20;//explosion .. 敵全体に{敵たちのHPの最小値}のダメージ
	Values[ShopItemList[2][2]] = 15;//steal .. 敵たちからお金を盗む（Player.Luckが関わってくる）
	Values[ShopItemList[2][3]] = 20;//clairvoyance := 草むらの下の敵を見える
	Values[ShopItemList[2][4]] = 20;//clairvoyance := 草むらの下の敵を見える
	Explanation[ShopItemList[2][0]] = "cure your HP";
	Explanation[ShopItemList[2][1]] = "smallest_enemy's_HP damage to all enemy";
	Explanation[ShopItemList[2][2]] = "steal random coins from all enemies";
	Explanation[ShopItemList[2][3]] = "you can find hidden enemies for a while";
	Explanation[ShopItemList[2][4]] = "enemy moves slower";


	//ItemCursor = 3(Weapon)
	ShopItemList[3] = { "Wide Bullet", "Penetrate Bullet" ,"Bullet UpGrade" , "Bomb+1"};
	Values[ShopItemList[3][0]] = 70;//wide
	Values[ShopItemList[3][1]] = 70;//wide
	Values[ShopItemList[3][2]] = 15;//duration
	Values[ShopItemList[3][3]] = 40;//num+1

	

	Explanation[ShopItemList[3][0]] = "3 Bullets in one shot";
	Explanation[ShopItemList[3][1]] = "Bullet Penetrates Wall";

	//widebulletを有効にし、弾の個数を増やす(自分の向いている方、及び自分の左右に弾丸を生成し、前に飛ばす)
	Explanation[ShopItemList[3][2]] = "increase Bullet's Duration";
	Explanation[ShopItemList[3][3]] = "increase Bomb";





	//ItemCursor = 4(Travel)
	ShopItemList[4] = MapNameList;//Mapの名前に一致してるので(Globa変数のMapNameList)
	Values[ShopItemList[4][0]] = 30;//wide
	Values[ShopItemList[4][1]] = 30;//duration
	Values[ShopItemList[4][2]] = 30;//duration

	Explanation[ShopItemList[4][0]] = "Change the stage to 'Ruin'";
	//widebulletを有効にし、弾の個数を増やす(自分の向いている方、及び自分の左右に弾丸を生成し、前に飛ばす)
	Explanation[ShopItemList[4][1]] = "Change the stage to 'Ancient Artifact'";
	Explanation[ShopItemList[4][2]] = "Change the stage to 'Green Green'";

	return;
}






//====================================================================================================================================================================
//====================================================================================================================================================================
//====================================================================================================================================================================
//====================================================================================================================================================================
//====================================================================================================================================================================
//====================================================================================================================================================================
//=========================================================================================
//=========================================================================================




class PlayerState;
class MenuProcesser;
class GameWindowProcesser;




//クラス内のグローバル変数は大文字から始める




class PlayerState{
	private:


	public:
	//基本ステータス
	long long Atk;//爆弾や弾丸の威力に関係する
	long long Hp_max;
	long long Hp;
	long long Def;
	long long Luck;//お金の入手に関わる
	long long Money;
	int Y , X;

	//弾丸関連=============================================
	int BulletDuration;//撃つ弾丸の持続性(何マス飛ぶか)
	int WideBullet = 0;//trueなら弾丸が三列になって出る
	int PenetrateBullet = 0;//撃つ弾丸が貫通する

	//特殊効果関連==========================================
	int ClairVoyance = 0; // Clairvoyance > 0である間、草むら(G)を表示しない(草むらの下の敵が見える).一歩ごとに-1する

	int SlowMagic = 0;//SlowMagic > 0である間、敵の動きが鈍くなる(processLimit_enemyMoveに作用),一歩ごとに-1



	//爆弾関連==============================================	
	int BombNumber = 1;//一度に落とす爆弾の個数



	//方向
	//0,1,2,3の順に上、右、下、左となる
	int direction;


	//特殊ステータス
	//これらはそれぞれ一度のみ使用できる
	long long CounterAttack = 0;//カウンター(0になるまで有効)
	long long Specialdefense = 0; //追加で特殊防御(0になるまで有効)
	long long SpecialAttack = 0;  //一度だけ攻撃up

	PlayerState(){

		init();
	}

	void init(){
		Atk = 3;
		Hp_max = 12;
		Hp = Hp_max;
		Def = 4;
		Luck = 0;
		Money = 17;
		X = STAGESIZE/2;
		Y = STAGESIZE/2;

		BulletDuration = 7;
		BombNumber = 0;

		ClairVoyance = 0;
	}






};















//ステージ(map)の情報
class Stage{
	public:
	//stageのmap
	std::vector<std::vector<char> > StageMap;


	int Size = STAGESIZE;
	int MapID;


	//画面に表示するステージの幅
	int StageWindowSize = 14;

	//座標は(y,x)で表す

	//stageMapの情報をstringで表している
	//後でvector<vector<char> >に変換
	//これを向きを変えながら連結してstageを作る
	int MapInfoSize;//30*30
	std::string MapInfo; 




	/*
		MapのID(MapID)を渡して、global変数のMapDataList と MapSizeListの中からIDのものを持ってくる
		0-index
	*/
	Stage(int MapID_){
		init(MapID_);
	}







	void init(int MapID_){
		MapID = MapID_;//0-index
		if(MapID >= MapDataList_Size)MapID=0;

		MapInfoSize = MapSizeList[MapID];
		MapInfo = MapDataList[MapID];

		std::vector<std::vector<char> >(STAGESIZE , std::vector<char>(STAGESIZE , 'n')).swap( StageMap);

		

		for( int y = 0 ; y < STAGESIZE ; y++){
			for( int x = 0 ; x < STAGESIZE ; x++ ){
				int X , Y;
				int x_lef = MapInfoSize*(x/MapInfoSize);
				int y_lef = MapInfoSize*(y/MapInfoSize);
				//同じステージを使い回すのも良くないので、区画によって、角度を変える
				//角度のバリエーションは8パターン
				switch((x/MapInfoSize + y/MapInfoSize)%8){
					case 0://左上から
						Y = y - y_lef;
						X = x - x_lef;
						break;
					case 1://右上から
						Y = y - y_lef;
						X = MapInfoSize - (x - x_lef);
						break;
					case 2://左下から
						Y = MapInfoSize -( y - y_lef );
						X = x - x_lef;
						break;
					case 3://右下から
						Y = MapInfoSize - (y - y_lef);
						X = MapInfoSize - (x - x_lef);
						break;
					case 4://
						X = y - y_lef;
						Y = x - x_lef;
						break;
					case 5://
						X = y - y_lef;
						Y = MapInfoSize - (x - x_lef);
						break;
					case 6://
						X = MapInfoSize -( y - y_lef );
						Y = x - x_lef;
						break;
					case 7://
						X = MapInfoSize - (y - y_lef);
						Y = MapInfoSize - (x - x_lef);
						break;

				}
				StageMap[y][x] = MapInfo[Y*MapInfoSize+X];
			}
		}
		//IsActive = true;
	}





};












/*
	敵のオブジェクト
	座標は（行、列）= (Y,X)とし、
	(Y,X)は相対的位置 := 左上を(StageMapPositionY,StageMapPositionX)とする位置付け
	敵はStageMapのタイルに沿って動く(U,D,L,R)	
*/
struct Enemy{
	long long HP;
	//座標
	int Y;
	int X;
	long long Atk;
	long long Movement;
	Enemy(){}
	Enemy(int Y_ , int X_ , long long HP_ = 3,long long Atk_ = 3){
		Y=Y_;
		X=X_;
		HP=HP_;
		Atk = Atk_;
	}

	char rank(){
		switch(HP/20){
			case 0:return 'a';
			case 1:return 'b';
			case 2:return 'c';
			case 3:return 'd';
			case 4:return 'e';
			case 5:return 'f';
			case 6:return 'g';
			case 7:return 'h';
			case 8:return 'i';
			case 9:return 'j';
			case 10:return 'k';
			case 11:return 'l';
			case 12:return 'n';
			case 13:return 'm';
			case 14:return 'o';
			case 15:return 'p';
			case 16:return 'q';
			case 17:return 'r';
			case 18:return 's';
			case 19:return 't';
			case 20:return 'u';
			case 21:return 'v';
			case 22:return 'w';
			case 23:return 'x';
			case 24:return 'y';
			case 25:return 'z';
			default:return '?';
		}
		return '?';
	}

	//落とし物(お金の値段)(運に対する金額がもらえる)
	int lost_money(int luck){
		return (int)(( (double)luck/4.0 + 1.0)*(double)(max(1.0 , (HP+Atk-25.0)/10 ) ) );
	}
	
};














/*
	Playerのうった弾丸のデータ構造
	Enemyと同じく、Y,XはMapに対する相対的位置
	BulletもStageMapのタイルに沿って動くが、Enemyとは逆(書かれているものと逆に動く)
	弾丸は貫通しない
	同じ位置に複数個の弾丸が重なっていても良い
*/
struct Bullet{
	int Y;//発射位置
	int X;
	long long Power;
	char figure = '*';
	int direction = 0;
	int duration;//弾丸をどれぐらいの距離持続して飛ばせるか
	int Penetrate = 0;//貫通性(1なら貫通)
	Bullet(){
		duration = 7;
		Power= 0;
		Penetrate = 0;

	}
	Bullet(int Y_ , int X_ , int Power_ , int direction_ , int duration_ = 7){
		Y=Y_;
		X=X_;
		Power = Power_;
		direction = direction_;
		duration = duration_;
		Penetrate = 0;
	}

	
};








/*

	爆弾(Y , X , 範囲 , 威力)
	座標、範囲、威力、持続性


*/
struct Bomb{
	int Y;
	int X;
	int Width = 1;
	long long Power;
	int duration;//持続性

	Bomb(){
		Y = STAGESIZE/2;
		X = STAGESIZE/2;
		Width = 1;
		Power = 0;
		duration = 3;

	}

	Bomb(int Y_ , int X_ ,  long long Power_ =  1,int duration_ = 3){
		Y = Y_;
		X = X_;
		Width = 1;
		Power = Power_;
		duration =duration_;;

	}
};




















/*

	ステージ上の処理は全てここに任せる
	敵の生成、移動、ダメージ管理、消滅
	プレイヤーの動き、攻撃、攻撃によるオブジェクト(Bullet)の処理など

*/
class GameWindowProcesser{
	private:
	std::vector<std::vector<char> > StageMap;




	int Size = STAGESIZE;


	//画面に表示するステージの幅
	int StageWindowSize ;;




	//ウィンドウ内で、stageのを表示させる絶対位置

	int StageMapPositionY = 10;
	int StageMapPositionX = 45;






	//移動の方向(４方向)
	std::vector<std::pair<int ,int > > nextDirection = 
	{
		std::make_pair(0,-1)
		,std::make_pair(0,1)
		,std::make_pair(1,0)
		,std::make_pair(-1,0)					
	};







	public:

	//map内に存在できる数の上限
	int EnemyNumLimit = 100;
	int BulletNumLimit = 100;
	int BombNumLimit = 50;





	//処理の間隔'(対象ごとに異なる)
	long long processLimit_enemyMove = 6500;
	long long processLimit_enemyGenerate = 4000;
	long long processLimit_bullet = 2000;
	long long processLimit_bombGenerate = 24000;
	long long processLimit_bombUpdate = 8000;


	//現在のStageのID
	int MapID;


	//座標は(y,x)で表す

	//stageMapの情報をstringで表している
	//後でvector<vector<char> >に変換
	//これを向きを変えながら連結してstageを作る
	int MapInfoSize;//30*30
	std::string MapInfo;


	//今いる敵の一覧
	std::vector<Enemy> EnemyList;

	//今ある弾丸の一覧
	std::vector<Bullet> BulletList;

	//今ある爆弾の一覧
	std::vector<Bomb> BombList;

	//activeかどうか
	bool IsActive = true;


	//ステージの中に収まるかどうか(STAGESIZExSTAGESIZE) STAGESIZEはMapInfoSizeではない
	bool Isin(int Y , int X){
		if(StageMap.size() <= Y)return false;

		return Y>= 0 && Y< StageMap.size() && X >= 0 && X<StageMap[0].size();

	}







	//MapInfoは要素数が900(30x30)だが、それを使い回すことで(150x150)のマップにしている
	//同じ向きのまま使い回すと単調になってしまうので、区画によって向きを変える
	GameWindowProcesser(PlayerState &P , Stage &S){
		init_value(P,S);
	}






	//Sから変数を取ってきて、Playerの位置を初期化
	void init_value(PlayerState &P, Stage &S){

		///==============Stageの変数の初期化====================
		MapInfoSize = S.MapInfoSize;//stage情報のサイズ
		MapInfo = S.MapInfo;//mapの情報の1次元配列
		StageMap = S.StageMap;//Stageのmap
		Size = S.Size;//配列のサイズ
		StageWindowSize = S.StageWindowSize;//windowサイズ
		MapID = S.MapID;//ID



		///===============Playerの位置などを初期化=================
		//スポーン可能な地点まで探索.見つかったらそこを初期値にする
		int Y = STAGESIZE/2;
		int X = STAGESIZE/2;
		int flag = 0;//終了フラグ
		for(int i = Y ; i < Y+30 ; i++){
			for(int j = X ; j < X+30 ; j++){
				//スポーン可能地点な場合
				if(StageMap[i][j] != '#' && StageMap[i][j] !='B'){
					Y=i;
					X=j;
					flag = 1;
				}
				if(flag)break;
			}
			if(flag)break;
		}

		P.Y = Y;
		P.X = X;


		///===============GameWindowの色々の初期化=================

		
		std::vector<Enemy>(0).swap(EnemyList);
		std::vector<Bullet>(0).swap(BulletList);
		std::vector<Bomb>(0).swap(BombList);


	}









	void debug(PlayerState &P){
		char p[30];
		sprintf(p,"%d  , %d",P.Y , P.X);
		mvaddstr(40,50,p);	
		for(int i = 0 ; i< BombList.size();i++){
			char p[30];
			sprintf(p,"%d , %d",BombList[i].Y , abs(BombList[i].Y - P.Y));
			mvaddstr(41+i,50,p);
		}

	}








	//UIを描画する関数で、eraseもrefreshのしない
	//displayがこれよりも強い関数で、画面表示において、eraseやrefreshはdisplayに任せる
	void displayUI(PlayerState &P){
		//各種情報の描画

		//playerの情報

		//情報を表示する位置
		std::pair<int ,int> infomation_place = std::make_pair(20,85);

		char pl_HP[40];sprintf(pl_HP,"HP : %lld / %lld" ,P.Hp , P.Hp_max);//hp
		char pl_Atk[40];sprintf(pl_Atk,"Atk : %lld" ,P.Atk);//atk
		char pl_Def[40];sprintf(pl_Def,"Def : %lld" ,P.Def);//def
		char pl_Money[40];sprintf(pl_Money,"Money : %lld" ,P.Money);//playerの所持金
		char pl_sp_atk[40];sprintf(pl_sp_atk,"Special attack : %lld" ,P.SpecialAttack);
		char pl_sp_def[40];sprintf(pl_sp_def,"Special defense : %lld" ,P.Specialdefense);
		char pl_counterattack[40];sprintf(pl_counterattack , "Counter Attack : %lld" ,P.CounterAttack);

		mvaddstr(infomation_place.first+2 , infomation_place.second,pl_HP);
		mvaddstr(infomation_place.first+4 , infomation_place.second,pl_Atk);
		mvaddstr(infomation_place.first+6 , infomation_place.second,pl_Def);
		mvaddstr(infomation_place.first+8 , infomation_place.second,pl_Money);
		mvaddstr(infomation_place.first+10 , infomation_place.second,pl_sp_atk);
		mvaddstr(infomation_place.first+12 , infomation_place.second,pl_sp_def);
		mvaddstr(infomation_place.first+14 , infomation_place.second,pl_counterattack);



		//現在のマップの名前
		char map_name[70];
		std::string map_name_ = MapNameList[MapID_now];
		for(int index = 0 ; index < map_name_.size();index++)map_name[index] = map_name_[index];

		mvaddstr(infomation_place.first , infomation_place.second,"Map Name : ");
		mvaddstr(infomation_place.first , infomation_place.second + 11 , map_name);





		//scoreの情報
		char Sc_info[70];sprintf(Sc_info , "PlayersScore : %lld" , PlayersScore);
		mvaddstr(infomation_place.first + 16 , infomation_place.second , Sc_info);

		std::pair<int,int> GameWindow = std::make_pair(6,5);
		
		//境界
		mvaddstr(GameWindow.first , GameWindow.second 
		 , "----------------------------------------------------------------------------------------------------------------");
		mvaddstr(GameWindow.first+36 , GameWindow.second 
		 , "----------------------------------------------------------------------------------------------------------------");


		//ステージwindowの枠線
		for(int y = StageMapPositionY-1 ; y < StageMapPositionY + StageWindowSize*2+1 ; y++){
			mvaddch(y,StageMapPositionX-1,'|');
		}
		for(int y = StageMapPositionY-1 ; y < StageMapPositionY + StageWindowSize*2+1 ; y++){
			mvaddch(y,StageMapPositionX+StageWindowSize*2+1,'|');
		}
		for(int x = StageMapPositionX ; x < StageMapPositionX + StageWindowSize*2 +1; x++){
			mvaddch(StageMapPositionY-1,x,'-');
		}
		for(int x = StageMapPositionX ; x < StageMapPositionX + StageWindowSize*2+1 ; x++){
			mvaddch(StageMapPositionY+StageWindowSize*2+1,x,'-');
		}

	}














	/*
		画面表示の注意!!!

		敵は草むら('G')に隠れることができるので最初に描画
		その後ステージのオブジェクトや地面の色を描画
		Playerの透視効果(Clairvoyance)がONなら草むらは表示しない
		最後に弾丸とplayerを描画

		表示する順番:敵->草むら,object -> player ,bullet -> bomb
	*/
	void display(PlayerState &P, int mode = 0){


		
		//初期設定---------------------------------------------------------------------------
		//初期設定---------------------------------------------------------------------------

		//アクティブでない状態なら何もしない
		if(IsActive == false){
			return;
		}

		

		//画面更新の必要があるなら更新
		if(mode == 1){
			erase();
		}

		init_pair(1, COLOR_MAGENTA, COLOR_BLACK); //文字が赤， 背景が黒
		init_pair(2, COLOR_BLUE, COLOR_BLACK); //文字が青， 背景が黒
		init_pair(3, COLOR_BLUE, COLOR_BLUE); //文字が緑， 背景が黒
		init_pair(4, COLOR_YELLOW, COLOR_BLACK); //文字が黄， 背景が黒
		init_pair(5, COLOR_GREEN,COLOR_GREEN);//緑
		init_pair(6, COLOR_WHITE , COLOR_RED);//青
		init_pair(7, COLOR_WHITE , COLOR_MAGENTA);//文字が白,背景が赤

		
		//表示するmapの範囲 -> (Y_lef , X _lef)と (Y_rig , X_rig)からなる長方形
		int X_lef = max(P.X - StageWindowSize , 0);
		int Y_lef = max(P.Y - StageWindowSize , 0);

		int X_rig = min(P.X + StageWindowSize , STAGESIZE-1);
		int Y_rig = min(P.Y + StageWindowSize , STAGESIZE-1);



		


		//敵は草むらに隠れるので先に表示---------------------------------------------------------------------
		//敵は草むらに隠れるので先に表示---------------------------------------------------------------------

		//敵を描画
		for(int index = (int)EnemyList.size()-1;index>=0;index--){
			//自分の位置から見える敵のみ表示(mapの表示サイズ内の敵のみ表示)
			if(abs(EnemyList[index].Y - P.Y) <= StageWindowSize && abs(EnemyList[index].X - P.X) <= StageWindowSize){
				attrset(COLOR_PAIR(6));
				mvaddch(StageMapPositionY + EnemyList[index].Y - Y_lef , 
					StageMapPositionX + EnemyList[index].X - X_lef , EnemyList[index].rank());
				attrset(COLOR_PAIR(0));
			}
		}




		//stagemapの描画-------------------------------------------------------------------------
		//stagemapの描画-------------------------------------------------------------------------



		for(int y = Y_lef ; y<= Y_rig ; y++){
			for(int x = X_lef ; x<= X_rig ; x++){
				switch(StageMap[y][x]){
					case 'G' : 
						if(P.ClairVoyance <= 0){
							attrset(COLOR_PAIR(5));
							mvaddch(StageMapPositionY + y - Y_lef , StageMapPositionX + x - X_lef , ' ');
						}
						break;
					case 'B' : 
						attrset(COLOR_PAIR(3));
						mvaddch(StageMapPositionY + y - Y_lef , StageMapPositionX + x - X_lef , ' ');
						break;
					case '#' : 
						mvaddch(StageMapPositionY + y - Y_lef , StageMapPositionX + x - X_lef , 'X');
						break;
					case '.' : 
						//何もないので何も表示しない
						break;
					default : break;
				};
				attrset(COLOR_PAIR(0));

			}
		}



		//弾丸を描画
		for(int index = (int)BulletList.size()-1;index>=0;index--){
			
			attrset(COLOR_PAIR(0));
			mvaddch(StageMapPositionY + BulletList[index].Y - Y_lef, 
				StageMapPositionX + BulletList[index].X - X_lef, BulletList[index].figure);
			attrset(COLOR_PAIR(0));
		}

		//自分を表示
		attrset(COLOR_PAIR(4));
		mvaddch(StageMapPositionY + P.Y - Y_lef , StageMapPositionX + P.X - X_lef , '@');
		attrset(COLOR_PAIR(0));



		
		//爆弾を表示
		attrset(COLOR_PAIR(1));

		for(int index = 0; index < BombList.size() ; index++){
			Bomb Bm = BombList[index];
			for(int B_Y = max(0,Bm.Y-Bm.Width) ; B_Y <= min(STAGESIZE - 1 , Bm.Y + Bm.Width) ; B_Y++){
				for(int B_X = max(0,Bm.X-Bm.Width) ; B_X <= min(STAGESIZE - 1 , Bm.X + Bm.Width) ; B_X++){
					//爆発の中心地からのマンハッタン距離がwidth以内なら爆風を表示
					if(abs(B_Y-Bm.Y)+abs(B_X - Bm.X) <= Bm.Width  && Isin(B_Y,B_X)){
						//画面の表示範囲内なら表示
						if(abs(P.Y-Bm.Y) <= StageWindowSize && abs(P.X -Bm.X) <= StageWindowSize){
							mvaddch(B_Y + StageMapPositionY - Y_lef , B_X + StageMapPositionX - X_lef , '*');
						}
					}
				}
			}
			
		}
		attrset(COLOR_PAIR(0));


		displayUI(P);	
		//debug(P);
		refresh();


	}


	












	//ランダムな確率で敵を生成(生成したらtrue , else -> false)して敵のリストに入れる
	//敵の移動(状態のUpdate)と共に呼び出すので、敵が重なったりはしない(してもいいけど)
	bool generateEnemy( PlayerState &P , long long HP_ = 6 , long long Atk_ = 3 ){
		if(EnemyList.size()>EnemyNumLimit)return false;//すでに敵が大量にいるときは生成しない

		int generateScope = 30;
		//playerを中心に
		int Y = P.Y + rand()%(generateScope*2 + 1) - generateScope;
		int X = P.X + rand()%(generateScope*2 + 1) - generateScope;

		//ステージの範囲内かつ,playerの目の届かない場所でスポーン
		//スポーン地点はランダムなので、ある意味確率でスポーンしている
		//objectがない地点でしかスポーンできない(#やBがある場所はだめ)
		if(Isin(Y,X) && abs(Y-P.Y) > StageWindowSize && abs(X-P.X) > StageWindowSize
			&& StageMap[Y][X] != 'B' && StageMap[Y][X] != '#'){
							
			EnemyList.push_back(Enemy(Y,X,HP_,Atk_));
			
			return true;
		}else{
			
			return false;
		}
	}






	//弾丸を生成(弾丸は同じ場所に複数個存在しても良い)して、StageMapに存在する弾丸のリストに入れる
	bool generateBullet(PlayerState &P , int direction){
		

		if(BulletList.size()>=BulletNumLimit)return false;//すでに大量のときは生成しない


		direction %=4;
		//0,1,2,3 -> U,R,D,L

		//元の攻撃力+一度きりの攻撃上昇
		long long attack = P.Atk + P.SpecialAttack;
		P.SpecialAttack = 0;//一度きりの攻撃上昇なので使ったら0にする

		int Y = P.Y;
		int X = P.X;
		std::vector<std::pair<int ,int > > newBullet;

		switch(direction){
			case 0://上
				newBullet.push_back(std::make_pair(Y-1,X));
				if(P.WideBullet!=0){//widebulletが有効なら、自分の左右に弾を生成し、前に飛ばす
					newBullet.push_back(std::make_pair(Y,X-1));
					newBullet.push_back(std::make_pair(Y,X+1));	
				}
				break;
			case 1://右
				newBullet.push_back(std::make_pair(Y,X+1));
				if(P.WideBullet!=0){//widebulletが有効なら、自分の左右に弾を生成し、前に飛ばす
					newBullet.push_back(std::make_pair(Y-1,X));
					newBullet.push_back(std::make_pair(Y+1,X));	
				}
				break;
			case 2://した
				newBullet.push_back(std::make_pair(Y+1,X));
				if(P.WideBullet!=0){//widebulletが有効なら、自分の左右に弾を生成し、前に飛ばす
					newBullet.push_back(std::make_pair(Y,X-1));
					newBullet.push_back(std::make_pair(Y,X+1));	
				}
				break;
			case 3://左
				newBullet.push_back(std::make_pair(Y,X-1));
				if(P.WideBullet!=0){//widebulletが有効なら、自分の左右に弾を生成し、前に飛ばす
					newBullet.push_back(std::make_pair(Y-1,X));
					newBullet.push_back(std::make_pair(Y+1,X));	
				}
				break;
		}

		bool done = 0;
		for(std::pair<int ,int > BLT : newBullet){
			Y = BLT.first;
			X = BLT.second;
			if(Isin(Y,X)){
				//障害物がない,または貫通(P.Penetrate)がONならOK
				if((StageMap[Y][X] != '#' && StageMap[Y][X ]!='B')||P.PenetrateBullet != 0){
					BulletList.push_back(Bullet(Y , X ,attack , direction , P.BulletDuration));
					done = 1;
				}
			}
		}
		
		return done;
	}












	/*
		爆弾を一つ生成
		BombStateに書き込む
	*/
	void generateBomb(PlayerState &P){
		int cnt = P.BombNumber;
		while(cnt --> 0){
			if(BombList.size()>BombNumLimit)break;
			int generateScope = StageWindowSize;//見える範囲に爆弾を落とす
			//playerを中心に
			int Y = P.Y + rand()%(generateScope*2 + 1) - generateScope;
			int X = P.X + rand()%(generateScope*2 + 1) - generateScope;

			BombList.push_back(Bomb(Y,X,P.Atk/3,2));
		}
		return;
	}






	//敵の情報を更新
	//敵に関して：StageMapの床のタイル(U,D,R,L)に沿って移動
	//   HPが０などの特別な状態になったら消滅
	void Enemy_Update(PlayerState &P){
		//状態更新フラグが立ってないなら何もしない
		if(IsActive == false)return;

		//敵の移動それによる処理
		//敵をリストから削除したあとは必ずcontinue!!
		for(int index = (int)(EnemyList.size())-1 ; index>=0;index--){
			if(EnemyList[index].HP <=0 ){//0以下なら消すけど、多分ここは実行されない
				P.Money += EnemyList[index].lost_money(P.Luck);//落とし物
				EnemyList.erase(EnemyList.begin() + index);
				PlayersScore++;//グローバル変数のスコア
			}else{

				//移動前の処理==================================================================================================
				//移動前の処理==================================================================================================


				//現在地にplayerがいる->ダメージを与えて消える
				if(EnemyList[index].Y == P.Y && EnemyList[index].X == P.X){
					//一度だけplayerがカウンター
					long long NextVal_counterattack = max(0ll , P.CounterAttack - EnemyList[index].HP);

					EnemyList[index].HP-=P.CounterAttack;

					P.CounterAttack = NextVal_counterattack;//使った分だけマイナスする

					if(EnemyList[index].HP <= 0){
						P.Money += EnemyList[index].lost_money(P.Luck);//落とし物
						//敵を消滅させる
						EnemyList.erase(EnemyList.begin()+index);
						
						PlayersScore++;//グローバル変数のスコア
						continue;
					}

					//HPから(敵の攻撃力-自分の防御力)を引く(一度だけの防御も発動)

					long long NextVal_specialdefense = max(0ll , P.CounterAttack - EnemyList[index].HP);//使った分だけマイナスする
					P.Hp -= max(EnemyList[index].Atk - P.Def-P.Specialdefense ,0ll);
					P.Specialdefense=NextVal_specialdefense;

					if(P.Hp<=0){
						//画面を消してGameOverと表示
						IsActive = false;//ゲームを中断して、色々処理
						erase();
						mvaddstr(30,30,"Game Over !!");
						refresh();

					}else{
						//敵を消滅させる
						EnemyList.erase(EnemyList.begin()+index);
						continue;
					}
					
				}




				//四方向移動先=================================================================================================================================
				//四方向移動先=================================================================================================================================


				//次の移動さきの候補地点(マンハッタン距離が短くなるような候補)
				std::vector<std::pair<int ,int > > nextPositionCandidate;


				//playerまでのマンハッタン距離が近くなるように移動
				//ただし、確率で不利な移動も行う
				
				for(std::pair<int ,int> next : nextDirection){
					int nextY = EnemyList[index].Y + next.first; 
					int nextX = EnemyList[index].X + next.second;
					//移動可能な場合のみ
					if(Isin(nextY,nextX) && StageMap[nextY][nextX] !='B' && StageMap[nextY][nextX] !='#'){
						///マンハッタン距離が近くなった場合
						if( abs(nextY-P.Y) + abs(nextX -P.X) < abs(EnemyList[index].Y - P.Y) + abs(EnemyList[index].X - P.X) ){
							nextPositionCandidate.push_back(std::make_pair(nextY,nextX));
						}else{
							if(rand()%10 == 0){
								//不利な移動でも確率で移動する（そっちの方がゲームとして難しいから）
								nextPositionCandidate.push_back(std::make_pair(nextY,nextX));
							}
						}
					} 
				}

				
				//移動後の処理===============================================================================================================
				//移動後の処理===============================================================================================================

				//次の地点は候補地点からランダムに選ぶ
				int y , x;
				if(nextPositionCandidate.size()==0){
					y = EnemyList[index].Y;
					x = EnemyList[index].X;



				}else{

					int random = rand()%nextPositionCandidate.size();
					y = nextPositionCandidate[random].first;
					x = nextPositionCandidate[random].second;
				}

				//更新
				EnemyList[index].Y = y;
				EnemyList[index].X = x;

				
				//playerに到達してしまった場合
				if(y==P.Y && x == P.X ){
					

					//一度だけplayerがカウンター
					EnemyList[index].HP-=P.CounterAttack;
					P.CounterAttack = 0;
					if(EnemyList[index].HP <= 0){
						P.Money += EnemyList[index].lost_money(P.Luck);//落とし物
						//敵を消滅させる
						EnemyList.erase(EnemyList.begin()+index);
						 
						PlayersScore++;//グローバル変数のスコア
						continue;
					}

					//HPから(敵の攻撃力-自分の防御力)を引く(一度だけの防御も発動)
					P.Hp -= max(EnemyList[index].Atk - P.Def-P.Specialdefense ,0ll);
					P.Specialdefense=0;//一度だけなので使ったら0にする

					if(P.Hp<=0){
						//画面を消してGameOverと表示
						IsActive = false;//ゲームを中断して、色々処理
						erase();
						mvaddstr(30,30,"Game Over !!");
						refresh();

					}else{
						//敵を消滅させる
						EnemyList.erase(EnemyList.begin()+index);
						continue;
					}
					
				}

			}
		}//敵の処理

		return;
	}



















	//弾丸の処理
	void Bullet_Update(PlayerState &P){
		//弾丸の処理(移動前の位置がEnemyとかぶっていたら処理)
		//また、移動後の位置にもEnemyがいたら処理
		//弾丸は貫通力がないため、一度敵に当たると消滅する

		for(int index = (int)BulletList.size()-1 ; index>=0;index--){
			bool isHit = false;
			for(int index_e = (int)EnemyList.size()-1 ; index_e>=0;index_e--){
				//弾丸と敵が衝突した場合
				if(BulletList[index].Y == EnemyList[index_e].Y 
				   && BulletList[index].X == EnemyList[index_e].X){

					EnemyList[index_e].HP-=BulletList[index].Power;
					isHit = true;

					//HPが0以下になったら消去+playerにお金が入る
					if(EnemyList[index_e].HP<=0){
						P.Money += EnemyList[index_e].lost_money(P.Luck);//落とし物

						EnemyList.erase(EnemyList.begin()+index_e);
						
						PlayersScore++;//グローバル変数のスコア
					}

					//貫通力はないので当たったらbreak;
					break;
				}
			}

			//衝突したら弾丸は消える
			if(isHit){
				BulletList.erase(BulletList.begin()+index);
				continue;
			}


			//弾丸の移動
			switch(BulletList[index].direction){
				case 0://上
					BulletList[index].Y--;
					break;
				case 1://右
					BulletList[index].X++;
					break;
				case 2://した
					BulletList[index].Y++;
					break;
				case 3://左
					BulletList[index].X--;
					break;
			}
			
			
			//移動後にステージ外に出たら消す
			//または、貫通(Penetrate)がOFFかつobjectにぶつかるなら消す
			char object = StageMap[BulletList[index].Y][BulletList[index].X];
			if( ((object == '#' || object == 'B') && P.PenetrateBullet == 0 )
			   || !Isin(BulletList[index].Y , BulletList[index].X)){


				BulletList.erase(BulletList.begin()+index);
				continue;
			}


			//移動後も衝突確認をする	
			for(int index_e = (int)EnemyList.size()-1 ; index_e>=0;index_e--){
				//弾丸と敵が衝突した場合
				if(BulletList[index].Y == EnemyList[index_e].Y 
				   && BulletList[index].X == EnemyList[index_e].X){
					EnemyList[index_e].HP-=BulletList[index].Power;
					isHit = true;

					//HPが0以下になったら消去+playerにお金が入る
					if(EnemyList[index_e].HP<=0){
						
						P.Money += EnemyList[index_e].lost_money(P.Luck);//落とし物
					
						EnemyList.erase(EnemyList.begin()+index_e);//倒したので消す

						PlayersScore++;//グローバル変数のスコア

					}

					//貫通力はない(壁とは別)ので当たったらbreak;
					break;
				}
			}



			BulletList[index].duration--;

			//衝突する、もしくは弾丸の持続がなくなると弾丸は消える
			if(isHit || BulletList[index].duration <=0){
				BulletList.erase(BulletList.begin()+index);
				continue;
			}


		}


		return;
	}












	/*
		
		敵にダメージ、その後爆弾の更新...durationを減らす

	*/
	void Bomb_Update(PlayerState &P){



		for(int index = (int)BombList.size()-1 ; index >= 0;index--){




			//移動後も衝突確認をする	
			for(int index_e = (int)EnemyList.size()-1 ; index_e>=0;index_e--){
				//爆弾の範囲内に敵がいた場合
				if(abs(EnemyList[index_e].Y-BombList[index].Y) + abs(EnemyList[index_e].X - BombList[index].X) <= BombList[index].Width){

					EnemyList[index_e].HP-=BombList[index].Power;
					//HPが0以下になったら消去+playerにお金が入る
					if(EnemyList[index_e].HP<=0){
						
						P.Money += EnemyList[index_e].lost_money(P.Luck);//落とし物
					
						EnemyList.erase(EnemyList.begin()+index_e);//倒したので消す

						PlayersScore++;//グローバル変数のスコア

					}

				}

			}





			BombList[index].duration--;
			//持続性がなくなったら消滅
			if(BombList[index].duration<=0){
				BombList.erase(BombList.begin()+index);
			}
		}
	}















	//いろんな情報をupdate
	void Update(PlayerState &P){
		Enemy_Update(P);
		Bullet_Update(P);
		return;
	}















	//playerの移動
	void playerMove(PlayerState &P , int direction){
		direction %=4;
		if(direction<0)return;

		int Y = P.Y;
		int X = P.X;

		switch(direction){
			case 0:
				Y--;
				break;
			case 1:
				X++;
				break;
			case 2:
				Y++;
				break;
			case 3:
				X--;
				break;
		}

		if(Isin(Y,X) && StageMap[Y][X]!='#' && StageMap[Y][X]!='B'){
			P.Y = Y;
			P.X = X;

			//透視効果(Clairvoyance)は一歩ごとに-1される(0になると効果が消える)
			P.ClairVoyance = max(P.ClairVoyance-1 , 0);
			P.SlowMagic = max(P.SlowMagic-1 , 0);
		}
		return;

	} 


















	//-------------------------------------特殊効果-------------------------------------
	//-------------------------------------特殊効果-------------------------------------
	//-------------------------------------特殊効果-------------------------------------



	//
	//爆発の魔法
	//敵のHPの最小値だけ、敵全体にダメージ
	void Explosion(){
		long long mn_hp = 1e18;
		for(int index = (int)EnemyList.size()-1 ; index>=0 ; index--){
			mn_hp = min(EnemyList[index].HP , mn_hp);
		}

		for(int index = (int)EnemyList.size()-1 ; index>=0 ; index--){
			EnemyList[index].HP -= mn_hp;
			if(EnemyList[index].HP<=0){
				//ここはゲームバランス調節のため、お金はドロップしない
				EnemyList.erase(EnemyList.begin()+index);
				PlayersScore++;//グローバル変数のスコア
			}

		}
	}


	//盗みの魔法
	//敵全体からお金を奪う(random要素あり)
	//返り値は合計
	long long Steal(long long Luck){
		long long moneySum = 0;
		long long counter =(int)EnemyList.size();
		while(counter>0){
			long long r =  rand()%7;
			if(r>=5)moneySum++;
			counter--;
		}

		return moneySum*4/7;
	}




};




























/*
	Menuのクラス
	基本的にはplayerの能力をいじったり、GameWindowProcesserの情報をいじったりする
	UI構築もここでしている
	ここが一番偉いクラス : 全てのクラスにアクセスできる
*/
class MenuProcesser{
	//class内のグローバルな変数は大文字から始めてます


	private:

	//項目選択カーソルの位置			項目内要素(商品)選択のカーソルの位置
	int ItemCursor = 0;				int SelectCursor = 0;
	

	//各Pointの座標たち(の代表点)
	//Itemsを表示する場所のy座標      	  Itemsの中身(商品)を表示するy座標         商品説明のy座標
	int ItemPointY = 10;				int SelectPointY = 10;             int ExplanePointY = 10;

	//Itemsのx座標                     Itemsの中身(商品)のx座標　　　　　　　　　　商品説明のx座標
	int ItemPointX = 10;				int SelectPointX = 32;			   int ExplanePointX = 55;
	


	

	//カーソルの高さ(Itemの項目の場所にあるのか、Itemの中身を探しているのか)
	//0ならItemの高さにある。1ならItemの中身の高さ。
	int Layer = 0;


	public:
	
	//色々な変数の初期化(手作業Ver.)
	MenuProcesser(){
		
	}








	void init(){

		return;
	}















	//購入 : ここがややこしい
	//playerやstageをいじるので大変,そして重要
	//どの商品を買って、何を発動させるかはindexで記述する
	void purchase(PlayerState &P , GameWindowProcesser &W){

		//購入できるものがない or 「購入」を押せるタイミングではない
		if(Layer <= 0 || Layer >= 2){
			return;
		}
		//購入商品がない
		if(SelectCursor >= ShopItemList[ItemCursor].size()){
			return;
		}

		std::string goods = ShopItemList[ItemCursor][SelectCursor];

		//お金が足りないと買えない
		if(P.Money < Values[goods]){
			//debug();
			return;
		}
		if(ItemCursor >= ShopItemList.size())return;
		if(SelectCursor >= ShopItemList[ItemCursor].size())return;

		//買い物できたかどうか
		bool purchased = 1;

		//ここ以降は商品の使用
		if(ItemCursor == 0){
			//基本ステータス
			if(SelectCursor == 0)P.Atk++;
			else if(SelectCursor == 1)P.Hp_max++;
			else if(SelectCursor == 2)P.Def++;
			else if(SelectCursor == 3)P.Luck++;
		}else if(ItemCursor == 1){
			//特殊ステータス
			if(SelectCursor == 0)P.Specialdefense+=P.Hp_max/2;
			else if(SelectCursor == 1)P.SpecialAttack+=P.Hp_max;
			else if(SelectCursor == 2)P.CounterAttack+=P.Hp_max/2;

		}else if(ItemCursor == 2){
			//魔法関連(ここは記述が多い)
			if(SelectCursor == 0){
				if(P.Hp == P.Hp_max){
					purchased=0;
				}else{
					P.Hp = min(P.Hp + P.Hp_max/3,P.Hp_max);
				}
				
			}else if(SelectCursor == 1){
				// explosion : 敵のHPの最小値を求めて、敵全体にその分ダメージ
				// HPが０になった敵は消滅
				W.Explosion();
			}else if(SelectCursor == 2){
				//敵全体からお金を盗む
				P.Money += W.Steal(P.Luck);
			}else if(SelectCursor == 3){
				P.ClairVoyance += 64;
			}else if(SelectCursor == 4){
				P.SlowMagic += 64;
			}


		}else if(ItemCursor == 3){
			
			if(SelectCursor == 0){
				if(P.WideBullet != 0){
					//もう購入できないので、flagを折る
					purchased=0;
				}else{
					P.WideBullet=1;
				}
			}else if(SelectCursor == 1){
				if(P.PenetrateBullet != 0){
					//もう購入できないので、flagを折る
					purchased = 0;
				}else{
					P.PenetrateBullet = 1;
				}

			}else if(SelectCursor == 2){
				P.BulletDuration++;
			}else if(SelectCursor==3){
				//これ以上増やしても無駄な場合
				if(P.BombNumber>=W.BombNumLimit){
					purchased=0;
				}else{
					P.BombNumber++;
				}
				
			}else{
				
			}


		}else if(ItemCursor == 4){//ステージを変えるところ。
			//SelectCursor = MapIDで、対応している
			if(SelectCursor == W.MapID){
				//現在のstageが選択したstageなので、購入することはできない(購入する必要はない)
				purchased = 0;
			}else{	
				//selectcursorがmapID
				MapID_now = SelectCursor;
				Stage Stg(MapID_now);

				W.init_value(P , Stg);
			}
			
		}else{
			
		}
		if(purchased)P.Money-=Values[goods];

		return;

	}
















	


	//UIを描画する関数で、eraseもrefreshのしない
	//displayがこれよりも強い関数で、画面表示において、eraseやrefreshはdisplayに任せる
	void displayUI(PlayerState &P){
		//UIの装飾


		
		//現在のマップの名前

		std::pair<int , int> mapNameDisplay = std::make_pair(5,36);

		char map_name[50];

		std::string map_name_ = MapNameList[MapID_now];


		for(int index = 0 ; index < map_name_.size();index++)map_name[index] = map_name_[index];
		mvaddstr(mapNameDisplay.first , mapNameDisplay.second - 11 , "Map Name : ");
		mvaddstr(mapNameDisplay.first , mapNameDisplay.second , map_name);

		//名前の位置
		std::pair<int , int > shopLogo =std::make_pair(5,10);

		attrset(COLOR_PAIR(3));
		mvaddstr(shopLogo.first , shopLogo.second,"Menu");
		attrset(COLOR_PAIR(0));

		//playerの残金の位置
		std::pair<int , int> moneyDisplay = std::make_pair(5,60);
		char playerMoney[50];
		sprintf(playerMoney ,"Remaining Money : %lld" , P.Money);
		mvaddstr(moneyDisplay.first , moneyDisplay.second ,playerMoney);


		//ショップウィンドウの位置
		std::pair<int ,int > shopWindow = std::make_pair(6,5);
		mvaddstr(shopWindow.first , shopWindow.second  
		 , "----------------------------------------------------------------------------------------------------------------"
		);
		
		mvaddstr(shopWindow.first+20 , shopWindow.second  
		 , "----------------------------------------------------------------------------------------------------------------"
		);


		//プレイヤーのステータスを表示
		std::pair<int,int> p_State = std::make_pair(31 , 10);

		char playerInfo[200];
		char playerSpInfo[150];
		//現段階、表示してる各数値は12桁なので調整が必要かも
		sprintf(playerInfo , "HP  : %-12lld / %-12lld      Atk  : %-12lld      Def  : %-12lld      Luck  : %-12lld" , P.Hp ,P.Hp_max , P.Atk , P.Def , P.Luck);
		sprintf(playerSpInfo , "Special Attack : %-12lld     Special defense : %-12lld     CounterAttack : %-12lld" , P.SpecialAttack , P.Specialdefense, P.CounterAttack);
		mvaddstr(p_State.first+2 , p_State.second , playerInfo);
		mvaddstr(p_State.first+6 , p_State.second , playerSpInfo);


		//bulletのタイプ
		if(P.WideBullet == 0 && P.PenetrateBullet == 0){
			mvaddstr(p_State.first + 4 , p_State.second  , "Skills : Normal");
		}else if(P.WideBullet != 0 && P .PenetrateBullet==0){
			mvaddstr(p_State.first + 4 , p_State.second  , "Skills : Wide");
		}else if(P.WideBullet == 0 && P.PenetrateBullet!=0){
			mvaddstr(p_State.first + 4 , p_State.second  , "Skills : Penetrate");
		}else{
			mvaddstr(p_State.first + 4 , p_State.second  , "Skills : Wide , Penetrate");
		}



		//debug(map_name_.size());


	}


















	//画面に描画(modeが0 -> 画面の更新が必要ない　, 1 ->　画面を全消しして更新する必要あり)
	void display(PlayerState &P ,int mode = 0){

		///画面の更新が必要ならまず全部消す
		if(mode == 1){
			erase();
		}
		
		init_pair(1, COLOR_RED, COLOR_BLACK); //文字が赤， 背景が黒
		init_pair(2, COLOR_BLUE, COLOR_BLACK); //文字が青， 背景が黒
		init_pair(3, COLOR_GREEN, COLOR_BLACK); //文字が緑， 背景が黒
		init_pair(4, COLOR_YELLOW, COLOR_BLACK); //文字が黄， 背景が黒


		//項目一覧を表示
		mvaddstr(ItemPointY-2,ItemPointX-2,"Items");
		for (int itemNumber = 0; itemNumber < ItemSize ; itemNumber++){
			//mvaddch(行、列,文字)
			//std::stringは表示できないそうなので、一文字ずつcharを表示していきます。
			std::string item = Items[itemNumber];
			
			//選択中のものなら色をつける
			if(ItemCursor == itemNumber)attrset(COLOR_PAIR(4));

			for(int index = 0;index < item.size() ; index++){//画面に表示
				mvaddch(ItemPointY + itemNumber*2 , ItemPointX+index , item[index] );
				
			}
			//カーソルがある位置にカーソル(<)を表示
			if(ItemCursor == itemNumber){
				//カーソルが項目選択の位置でないなら色を戻す
				if(Layer!=0)attrset(COLOR_PAIR(0));
				mvaddch(ItemPointX + itemNumber*2 , ItemPointY + item.size()+1,'<');
			}
			//色を戻す
			if(ItemCursor == itemNumber)attrset(COLOR_PAIR(0));

		}

		//項目内要素(商品)の表示と説明
		mvaddstr(SelectPointY-2 , SelectPointX-2,"Goods");
		for(int selectionNumber = 0 ; selectionNumber < ShopItemList[ItemCursor].size();selectionNumber++){
			//Items[ItemCursor]の商品を表示
			std::string selection = ShopItemList[ItemCursor][selectionNumber];

			//商品選択中なら選択中のものに色をつける
			if(SelectCursor == selectionNumber && Layer == 1)attrset(COLOR_PAIR(4));

			for(int index = 0;index < selection.size() ; index++){
				mvaddch(SelectPointY + selectionNumber*2 , SelectPointX +index , selection[index] );
			}

			//商品選択中ならカーソルがある位置にカーソル(<)を表示
			if(SelectCursor == selectionNumber && Layer == 1){
				mvaddch(SelectPointY + selectionNumber*2 , SelectPointX+ selection.size()+1,'<');
			}
			//色を戻す
			attrset(COLOR_PAIR(0));

			
		}

		//商品選択中なら説明(+値段)を表示
		if(Layer >= 1){
			mvaddstr(ExplanePointY-2,ExplanePointX-2,"Explanation");
			std::string explaneString = Explanation[ShopItemList[ItemCursor][SelectCursor]];
			explaneString += "   " + toString(Values[ShopItemList[ItemCursor][SelectCursor]]) + "  Yen";
		
			for( int index = 0 ; index < explaneString.size() ; index++ ){
				mvaddch(ExplanePointY , ExplanePointX+index ,explaneString[index] );
			}
			
			
		}


		
		displayUI(P);

		//リフレッシュ
		refresh();

	}














	//debug用(display内でのみ使用)
	void debug(int d){
		//DEBUG

		char c[100];
		sprintf(c,"%d : " ,d );
		mvaddstr(10,65,c);


	}













	//選択カーソルを上に動かす
	// -> bool 移動できたかどうか
	bool move_cursor_up(){
		if(Layer == 0){
			if(ItemCursor == 0){
				return false;
			}else{
				ItemCursor--;
				return true;
			}
		}else if(Layer == 1 ){ 
			if(SelectCursor == 0){
				return false;
			}else{
				SelectCursor--;
				return true;
			}
		}else{
			return false;
		}
	}















	//下に動かす
	bool move_cursor_down(){
		if(Layer == 0){
			if(ItemCursor >= Items.size()-1){
				return false;
			}else{
				ItemCursor++;
				return true;
			}
		}else if(Layer == 1 ){ 
			if(SelectCursor >= ShopItemList[ItemCursor].size()-1){
				return false;
			}else{
				SelectCursor++;
				return true;
			}
		}else{
			return false;
		}
	}













	//右に動かす(Layer++) この時、SelectCursorは0にセットする(オーバーフロー防止)
	//ItemCursorはそのまま(UIの利便性)
	//また、ShopItemList[ItemCursor]のサイズが0 (= 何も商品がない)の時は動かさずにfalse
	//Layerが1(商品選択中)の場合も動かせない
	bool move_cursor_rigth(){
		if(Layer == 0){
			if(ShopItemList[ItemCursor].size()==0){
				return false;
			}else{
				Layer++;
				SelectCursor=0;
				return true;
			}
		}else{
			return false;
		}
	}














	//左に動かす(この時も、move_cursor_rightと同じくSAelectCursor = 0)
	//この場合はItemCursorは0にしない(UIの利便性を考えて)	
	bool move_cursor_left(){
		if(Layer == 0)return false;
		else{
			Layer--;
			SelectCursor = 0;
			return true;
		}
	}













};







//ここから実行される
int main(int argc, char **argv){


	InitGlobalValue();
	InitShopItems();

	
	//画面の初期化
	initscr();
	//カラー属性を扱うための初期化
	start_color();

	//キーを押しても画面に表示しない
	noecho();
	//キー入力を即座に受け付ける
	cbreak();

	curs_set(0);
	//キー入力受付方法指定
	keypad(stdscr, TRUE);

	//キー入力非ブロッキングモード
	timeout(0);

	//global変数,,現在のマップID
	MapID_now = 0;
	Stage St(MapID_now);

	PlayerState Player;

	MenuProcesser Menu;

	


	GameWindowProcesser Processer(Player , St);




	//stage、ショップのどちらを描画するか
	//0->stage , 1 -> ショップ
	int GameMode = 0;


	long long turnCount = 0;




	
	while (1)
	{
		//キー入力受付
		int ch;
		ch = getch();

		//Qの入力で終了
		if (ch == 'Q')
		{
			break;
		}

		int UpdateFlag = 0;

		/*
			キー:
			 w : 上1
			 s : 下1
			 d : 右1
			 a : 左1
			 k : 決定
			 j ; 切り替え
			 
			 shift+方向キーでその方向に弾丸を飛ばす(Mode0の場合)
			

		*/

		if(ch == 'w'){
			if(GameMode == 0){
				Processer.playerMove(Player,0);
			}else{
				Menu.move_cursor_up();
				UpdateFlag = 1;
			}
			
		}else if(ch == 's'){
			if(GameMode == 0){
				Processer.playerMove(Player,2);
			}else{
				Menu.move_cursor_down();
				UpdateFlag = 1;
			}
		}else if(ch == 'a'){
			if(GameMode == 0){
				Processer.playerMove(Player,3);
			}else{
				Menu.move_cursor_left();
				UpdateFlag = 1;
			}
		}else if(ch== 'd'){
			if(GameMode == 0){
				Processer.playerMove(Player,1);
			}else{
				Menu.move_cursor_rigth();
				UpdateFlag = 1;
			}
		}else if(ch == 'j'){

			//Stage上での動きを切り替えないと、勝手にゲームが進んでしまう
			if(GameMode == 0){
				Processer.IsActive = false;
			}else{
				Processer.IsActive = true;
			}

			GameMode++;
			GameMode%=2;
			UpdateFlag = 1;

		}else if(ch =='k'){
			if(GameMode==0){
				int pass = 0;
			}else{
				Menu.purchase(Player,Processer);
			}
		}else if(ch == KEY_UP){//上
			if(GameMode==0){
				Processer.generateBullet(Player , 0);
			}else{

			}
		}else if(ch == KEY_RIGHT){//右
			if(GameMode==0){
				Processer.generateBullet(Player , 1);
			}else{

			}
		}else if(ch == KEY_DOWN){//下
			if(GameMode==0){
				Processer.generateBullet(Player , 2);
			}else{

			}
		}else if(ch == KEY_LEFT){//左
			if(GameMode==0){
				Processer.generateBullet(Player , 3);
			}else{

			}
		}



		turnCount++;

		//processLimitに引っかかった場合、特別な処理(状態の更新など)

		//敵の生成
		if(turnCount%Processer.processLimit_enemyGenerate==0){
			UpdateFlag = 1;
			
			if(GameMode == 0){
				//ここでplayerの能力に見合った敵を生成しないとゲームとしてやばい : enemy(HP,atk)
				Processer.generateEnemy(Player , Player.Hp/4 + Player.Atk*2/3 + 3 , Player.Hp_max*8/17+Player.Atk/7 + Player.Def*7/12);
			}
			
		}

		//敵の移動
		
		if(turnCount%( Processer.processLimit_enemyMove*(1+(Player.SlowMagic!=0) ) )==0){
			UpdateFlag = 1;
			if(GameMode == 0){
				Processer.Enemy_Update(Player);
			}
			
		}

		//弾丸の移動
		if(turnCount%Processer.processLimit_bullet==0){
			UpdateFlag = 1;
			if(GameMode == 0){
				Processer.Bullet_Update(Player);
			}
			
		}


		//爆弾の生成
		if(turnCount%Processer.processLimit_bombGenerate==0){
			UpdateFlag = 1;
			if(GameMode == 0){
				Processer.generateBomb(Player);
			}
		}


		//爆弾の更新
		if(turnCount%Processer.processLimit_bombUpdate==0){
			UpdateFlag = 1;
			if(GameMode == 0){
				Processer.Bomb_Update(Player);
			}
		}



		//オーバーフロー対策
		if(turnCount>1000000000)turnCount=0;



		//画面に表示（画面更新）
		if(GameMode==0){
			Processer.display(Player,1);
		}else{
			UpdateFlag = 1;
			Menu.display(Player,UpdateFlag);
		}
		
		
	}
	


	//画面をリセット
	endwin();

	return 0;
}

