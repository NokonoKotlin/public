# 競プロライブラリなど

ここは以下のコードを置いて置く場所です。
- 普段使いのライブラリの一部
- 定期的に整備するまでもないが、必要な箇所がたまにあるライブラリ 
  - 普段使いライブラリの亜種など
- 登場する場面が少ないが、特別な考察や発想を伴う問題のコード

ライブラリ同士の依存関係はなく、全てのライブラリはこのドキュメントの最後に掲載する `競プロテンプレート` に依存しています (できるだけ依存しないように意識していますが)。

`/library` に置いてあるやつは、まだ説明を書いていませんが、随時説明を追加していきます。

## ライセンス的なもの
このディレクトリにあるコードに関して、以下は許します。  
- 他人に共有しない場所で個人使用する。  
- このコードを使い、競技プログラミングの提出コードとして提出する(ただし、その提出を他者に共有するのは禁止)。  

以上 2 つ以外の用途は認めません。

## 索引と簡単な説明

#### ArithmeticProgression
```
区間に等差数列を 加算/代入 するやつ。スプレー木なのでセグ木より遅いが、挿入/削除をサポートする。
```

#### LinkCutTree  
```
辺を自由に切ったり張ったりした上で、Tree上のpathに関する情報を管理できる。  
LCAや根の張り替えにも対応。構築以外の各操作はO(logN)。  
任意のパス上の頂点の持つデータのマージ結果を計算できる。  
辺にデータを持たせたい時は頂点と頂点の間に余分に頂点を作り、余分な頂点にデータを持たせてそれを辺のデータとして扱う。
強化したら部分木クエリにも対応できるらしいので、TODO にしておく。
```


#### ExtendedArray
```
数列タイプの平衡二分木とほぼ同じ操作が可能。  
index の型が自由であり、index 値の範囲が計算量に影響しない。  
```

#### Kd Segment Tree
```
K 次元のグリッド上での{矩形取得,点更新} または {矩形更新、点取得} が可能。
```

#### Kd Tree
```
K 次元の空間内の点集合に対する操作が可能
```


#### mod library
```
定数 p に対して、剰余環 Z/pZ 内での数学的処理を行う。
    modint : Z/pZ の整数型
    FFT    : 高速フーリエ変換 (+畳み込み)
    FPS    : 多項式を表現したクラス
```


#### Rerooting
```
全方位木 DP
    木 T の全ての頂点に対して、その頂点が根である場合の集約を計算する。
```


#### Suffix Tree
```
列に対して接尾辞木を構築する。
Ukkonen's Algorithm を採用(オンライン構築可能)。  
部分文字列系の問題に対する解決策として tier 1 
```


#### 他にもたくさんありますが、長くなるので他は実際に見ていただきたく...
- 暇な時に随時追加していきます。


## 競プロテンプレート

```


//------------------------------------------
// include
//------------------------------------------
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <stack>
#include <bitset>
#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cctype>
#include <string>
#include <cstring>
#include <ctime>
#include<queue>
#include<complex>
#include <cassert>
using namespace std;


//------------------------------------------
// typename
//------------------------------------------

typedef long long ll;
typedef vector<long long > vll;
typedef pair<long long , long long> pll;
typedef map<ll , ll >mll;
typedef vector<pair<ll , ll> > vpll;

//------------------------------------------
// const
//------------------------------------------

const int DOUBLE_PRECISION = 20;// 出力の有効数字(適用のためには別で宣言が必要)
const double EPS = 1e-12;// double の最小単位
const double PI  = acos(-1.0); // 
// long long の ∞/-∞。差が 63 bit に入る
const long long INF = 4e18;
const long long NINF = -4e18;

//------------------------------------------
// macro
//------------------------------------------

#define ALL(a)  (a).begin(),(a).end()
#define RALL(a) (a).rbegin(), (a).rend()
#define SORT(c) sort((c).begin(),(c).end())
#define FOR(i,a,b) for(long long i=(a);i<(b);++i)
#define REP(i,n)  FOR(i,0,n)
// input/output
#define dump(x)  cerr << #x << " = " << (x) << endl;
#define debug(x) cerr << #x << " = " << (x) << " (L" << __LINE__ << ")" << " " << __FILE__ << endl;
#define ENDL cout << endl;
#define Read(x) cin >> x;
#define CinLL(x) long long x;cin>>x;
#define CinSTR(x) string x;cin>>x;
#define CinVec(a) for(auto& xxxxxxxxxx_uniquename_xxxxxxxxxxx : a)cin >> xxxxxxxxxx_uniquename_xxxxxxxxxxx;
#define CinTable(a) for(auto& asdfghjkl : a)for(auto& zxcvbnm : asdfghjkl)cin >> zxcvbnm;

//------------------------------------------
// template 
//------------------------------------------

// num <-> string
long long toInt(string s) {long long v; istringstream sin(s);sin>>v;return v;}
double toReal(string s){// 計算時間は大富豪だし、色々とこわいので注意
    if(s.find('.') == s.size())return double(toInt(s));string t = s.substr(s.find('.')+1);
    return double(toInt(s.substr(0,s.find('.')))) + double(toInt(t))*pow(0.1,double(t.size()));
}
template<class T> inline string toString(T x) {ostringstream sout;sout.precision(DOUBLE_PRECISION);sout<<x;return sout.str();}
int getdigit(ll n){return toString(n).size();}// 整数 n の桁数
string tobinary(ll x){
    string res = "";
    do{res+='0' + x%2;}while((x>>=1) > 0);
    while(res.size() > 1 && res.back() == '0')res.pop_back();
    reverse(res.begin(),res.end());
    return res;
}


// 2 次元のテーブルを出力 (T << オペレータを持つ型)。セル幅と行数と幅を指定。(セル幅は 3 以上、行幅はセル幅 * 2 + 3 以上)
template<class T> void DebugTable(vector<vector<T>> A , const int cellwidth = 7 , const int linewidth = 89 , const int line_num_lim = 100){
    while(A.size() > line_num_lim)A.pop_back();
    A.insert(A.begin(),vector<T>(0));vector<T>(linewidth+1,0).swap(A[0]);
    for(int i = 0; i < A[0].size() ; i++)A[0][i] = i;// index ををメモするところを追加
    vector<vector<char>> buffer((A.size())*2+1,vector<char>(linewidth,' '));
    for(int i = 0 ; i < buffer.size() ; i++)if(i%2==0)for(int j = 0; j < linewidth ; j++)buffer[i][j] = ('-');// 初期化値
    for(int i = 0 ; i < buffer.size() ; i++)buffer[i][0] = '|';
    for(int i = 0 ; i < buffer.size() ; i++)buffer[i].back() = '|';
    for(int i = 0 ; i < A.size() ; i++){
        for(int j = 0 ; j <= linewidth ; j++){
            ll lef = (cellwidth+1)*(j+1) + 1;
            if(lef >= linewidth)break;
            buffer[i*2+1][lef-1] = '|';// 縦の仕切り棒
        }
    }
    for(int i = 0 ; i < A.size() ; i++){
        for(int j = 0 ; j < A[i].size() ; j++){
            T x = A[i][j];string tmp = toString(x);
            ll lef = (cellwidth+1)*(j+1) + 1;if(lef+tmp.size() >= linewidth)break;
            if(tmp.size() > cellwidth){
                while(tmp.size() > cellwidth)tmp.pop_back();
                tmp.back() = '~';// 切り捨てたことを知らせるため
            }
            for(int k = 0 ; k < tmp.size() ; k++)buffer[i*2+1][lef + k] = tmp[k];
        }
    }
    buffer[1][(cellwidth)/2] = 'i';buffer[1][(cellwidth)/2+1 ] = ',';buffer[1][(cellwidth)/2+2 ] = 'j';
    for(int i = 1 ; i < A.size() ; i++){
        T x = i-1;string tmp = toString(x);
        ll lef = 1;if(lef+tmp.size() >= linewidth)break;
        if(tmp.size() > cellwidth){
            while(tmp.size() > cellwidth)tmp.pop_back();
            tmp.back() = '~';// 切り捨てたことを知らせるため
        }
        for(int k = 0 ; k < tmp.size() ; k++)buffer[i*2+1][lef + k] = tmp[k];
    }
    cerr << " DEBUG TABLE : " << endl;
    cerr << "(セル幅 - " <<  cellwidth << ") , (行幅 - " << linewidth << ") , (行数 - " << line_num_lim << ")" << endl;
    for(auto& r : buffer){for(char& c : r)cerr << char(c);ENDL;}
    return;
}
// DebugTable に依存
template<class T> void DebugArray(vector<T> A){
    vector<vector<T>> Table = {A};
    DebugTable(Table);
}







```