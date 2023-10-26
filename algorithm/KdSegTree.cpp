
//include
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
typedef long long ll;

















// 各演算のフォーマット(引数の順番など)は変えないで!!!
ll op(ll a , ll b){return a+b;} // 二つの引数のマージを返す
ll e(){return 0ll;}
ll upd(ll& a , ll b){return a += b;}


/*
    多次元セグ木(1次元も含む)
    コンストラクタで、vector<int> size_ を渡すと、
    各次元のサイズが size_[0],size_[1] , ... , size_[d-1] の d 次元グリッドを表現する

    基本的に座標を表すのは d 次元座標で、d 次元座標上の矩形を表す際は、その矩形の対角の座標 A , B を用いて表す。
    ただし、矩形の各次元は半開区間 [l , r) として渡す。

    内部実装は、座標を表す列の先頭を削除する操作を行うため、vectorで受け取ったものをdequeに直している

    テンプレートには 
    データの型        : T
    マージする時の演算 : op(a,b)
    op(a,b) の単位元 : e()
    一点の値を更新する : upd(&x , val) 
    を渡す

    初め、グリッドの全ての値は e() で初期化される

*/
template<class T , T (*op)(T ,T) , T (*e)() , T (*upd)(T& , T) >
class KdSegTree{

    private:

    vector<int> size; //[i]:= i 次元目のサイズ (処理で使うのはsize[0]のみだが、アクセス可能にしておく)
    vector<KdSegTree<T , op , e , upd > > node;
    T data;
    int d; // このセグメント木の次数

    // sz を sz 以上の、2 の冪乗の値にフォーマットする
    int resize(int& sz){
        int p = 1;
        while(p < sz)p*=2;
        return (sz = p);
    }



    // 座標 (A[0],A[1],...,A[d-1]) の値を、val でupdateする
    void update_sub(deque<int> A , T val){
        if(int(A.size()) != d)assert(0);
        if(d <= 0){ // 0 次元の時、
            upd(data,val);
        }else{
            int index = A.front(); // 最初の次元を取り出す
            if(index < 0 || index >= size[0])return;
            index += size[0];

            A.pop_front();// d-1 次元以下を見たいので、d 次を表す先頭を削除する
            node[index].update_sub(A,val);// 下の次元に投げる

            while(index/2 > 0){
                index/=2;
                node[index].set_sub(A, op(node[index*2].get_sub(A) , node[index*2+1].get_sub(A)) );
            }
        }
    }

    // 座標 (A[0],A[1],...,A[d-1]) の値を val に変更する
    void set_sub(deque<int> A , T val){
        if(int(A.size()) != d)assert(0);
        if(d <= 0){ // 0 次元の時、
            data = val;
        }else{
            int index = A.front(); // 最初の次元を取り出す
            if(index < 0 || index >= size[0])return;
            index += size[0];

            A.pop_front();// d-1 次元以下を見たいので、d 次を表す先頭を削除する
            node[index].set_sub(A,val);// 下の次元に投げる

            while(index/2 > 0){
                index/=2;
                node[index].set_sub(A, op(node[index*2].get_sub(A) , node[index*2+1].get_sub(A)) );
            }
        }
    }



    // 座標 (A[0],A[1],...,A[d-1]) と (B[0],B[1],...,B[d-1]) を対角とする矩形内の値の op を取得 (ただし、各次元の区間は半開区間である)
    T query_sub(deque<int> A , deque<int> B  , int SubRoot = 1 , int SubTreeLeft = 0 , int SubTreeRight = -1){
        if(int(A.size()) != d)assert(0);
        if(int(B.size()) != d)assert(0);

        if(d <= 0)return data; // 0 次元はデータ単体しか持たないので

        if(SubRoot == 1)SubTreeRight = size[0];

        int lef = A.front();
        int rig = B.front();
        if(lef > rig)swap(lef,rig);
        T res = e();

        //どれかの一つのステートしか実行されないので、A , B を同期しなくていい
        if(rig <= SubTreeLeft){//区間が被っていない
            return e();
        }else if(lef >=SubTreeRight){//区間が被っていない
            return e();
        }else if( lef <= SubTreeLeft && SubTreeRight <= rig ){
            //SubRootを根とする部分木の範囲が、取得したい範囲に完全に含まれている-> node[SubRoot]の値をreturn
            A.pop_front();B.pop_front(); 
            return node[SubRoot].query_sub(A , B);
        }else{//SubRootの右の根と左の根に降りて、再度調べる
            int border = (SubTreeLeft+SubTreeRight)/2;
            return op( query_sub(A,B,SubRoot*2,SubTreeLeft,border) , query_sub(A,B,SubRoot*2+1,border,SubTreeRight) );
        }
    }

    // 座標 (A[0],A[1],...,A[d-1]) の値を取得
    T get_sub(deque<int> A){
        if(int(A.size()) != d)assert(0);
        if(d <= 0)return data;

        int index = A.front();

        A.pop_front();// 先頭を削除して、d-1次元以降の座標にする
        return node[index+size[0]].get_sub(A); // 次の次元以降に投げる
        
    }


    public:
    
    KdSegTree(){}
    KdSegTree(vector<int> size_v){d = size_v.size() ; init(size_v);}


    void init(vector<int> size_v){   
        d = int(size_v.size());
        if(d<= 0){// 0 次元セグ木 は、単なる T 型(データ単体)
            data = e();
        }else{
            size.resize(d);
            for(int& s : size_v) s = resize(s);
            for(int i = 0 ; i < d ; i++ )size[i] = size_v[i];
            size_v.erase(size_v.begin());
            node.clear();
            node.resize(size[0]*2, KdSegTree< T , op , e , upd >(size_v));
        }
    } 

    // 座標 (A[0],A[1],...,A[d-1]) の値を取得
    T get(vector<int> A){
        deque<int> A_;for(int x : A)A_.push_back(x);
        return get_sub(A_);
    }  

    // 座標 (A[0],A[1],...,A[d-1]) と (B[0],B[1],...,B[d-1]) を対角とする矩形
    T query(vector<int> A , vector<int> B){
        deque<int> A_;for(int x : A)A_.push_back(x);
        deque<int> B_;for(int x : B)B_.push_back(x);
        return query_sub(A_,B_);
    }

    // 座標 (A[0],A[1],...,A[d-1]) の値を val に変更
    void set(vector<int> A , T  val){
        deque<int> A_;for(int x : A)A_.push_back(x);
        return set_sub(A_ , val);
    }  

    // 座標 (A[0],A[1],...,A[d-1]) の値に、upd(val) で更新をかける
    void update(vector<int> A , T  val){
        deque<int> A_;for(int x : A)A_.push_back(x);
        return update_sub(A_ , val);
    }  



    // グリッドの i 番目の次元の幅を返す(d 次元目は、0 次元のグリッドと捉えて 0 を返す)
    int width(int i){
        if(i< 0 || i>d)assert(0);
        if(i == d)return 0;
        else return size[i];
    }

};











