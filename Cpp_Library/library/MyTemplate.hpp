
#ifndef MyTemplate
#define MyTemplate 

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
// math
template<class T> T pow2(T x) {return x*x;}// x の 2 乗
template<class T> double dist2(pair<T,T> a , pair<T,T> b) {return sqrt(pow2(a.first-b.first) + pow2(a.second-b.second));}// 2 点間の距離
double toRadian(double D){return D*PI/180.0;}// 度数法 (°) から弧度法 (ラジアン)
double toDegree(double R){return R*180.0/PI;}// 弧度法 (ラジアン) から度数法 (°) 
//最小公倍数、最大公約数
ll gcd(ll x, ll y) { return y ? gcd(y, x % y) : x; }
ll lcm(ll a, ll b) { return (a / gcd(a, b)) * b; }
// 組み合わせ
ll nPr(ll n , ll r){ll res = 1;assert(n>=r);while(r-->0)res *= (n--);return res;}
ll nCr(ll n,  ll r){// n<=kなら 1 (0C1や1C5= 1)
  if ( r * 2 > n ) r = n - r;
  return nPr(n,r) / nPr(r,r);
}

// 2 次元のテーブルを出力 (T << オペレータを持つ型)
template<class T> void DebugTable(vector<vector<T>> A){
    const char space_ = ' ';
    // サイズ指定。(符号付き int を格納する場合 (11,121) とするとよい)
    const int cellwidth = 7;// 1 マスのサイズを設定 (3 以上)
    const int linewidth = 89;// 行の幅の上限 (cellwidth*2 + 3 以上)
    A.insert(A.begin(),vector<T>(0));vector<T>(linewidth+1,0).swap(A[0]);for(int i = 0; i < A[0].size() ; i++)A[0][i] = i;// index ををメモするところを追加
    vector<vector<char>> buffer((A.size())*2+1,vector<char>(linewidth,space_));
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
    cerr << " DEBUG TABLE :" << endl;
    for(auto& r : buffer){for(char& c : r)cerr << char(c);ENDL;}
    return;
}
// DebugTable に依存
template<class T> void DebugArray(vector<T> A){
    vector<vector<T>> Table = {A};
    DebugTable(Table);
}



#endif