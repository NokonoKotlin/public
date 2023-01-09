
//include
//------------------------------------------
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
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
//conversion
//------------------------------------------
inline long long toint(string s) {long long v; istringstream sin(s);sin>>v;return v;}
template<class T> inline string toString(T x) {ostringstream sout;sout<<x;return sout.str();}
//math
//-------------------------------------------
template<class T> inline T sqr(T x) {return x*x;}
//typedef
//------------------------------------------
typedef long long ll;
typedef long long LL;
typedef vector<int > vi;
typedef vector<long long > VLL;
typedef vector<long long > vll;
typedef vector<string > ves;
typedef vector<char > vech;

typedef pair<long long , long long> pll;
typedef pair<long long , long long> PLL;
typedef map<ll , ll >mll;
typedef map<int , int >mii;
typedef map<char , int >mci;
typedef map<char , ll >mcl;
typedef vector<pair<ll , ll> > vpll;

//container util
//------------------------------------------
#define ALL(a)  (a).begin(),(a).end()
#define RALL(a) (a).rbegin(), (a).rend()
#define VECMAX(x) *max_element(ALL(x))
#define VECMIN(x) *min_element(ALL(x))
#define PB push_back
#define MP make_pair
#define SZ(a) int((a).size())
#define EACH(i,c) for(typeof((c).begin()) i=(c).begin(); i!=(c).end(); ++i)
#define EXIST(s,e) ((s).find(e)!=(s).end())
#define SORT(c) sort((c).begin(),(c).end())
//repetition
//------------------------------------------
#define FOR(i,a,b) for(long long i=(a);i<(b);++i)
#define REP(i,n)  FOR(i,0,n)
//#define MULTIPLE(i,n,k) for(int i = (k) ; i<(n) ; i+=k+1)//倍数ループ
//constant
//------------------------------------------
const double EPS = 1e-10;
const double PI  = acos(-1.0);
//clear memory
#define CLR(a) memset((a), 0 ,sizeof(a))
//debug
#define dump(x)  cerr << #x << " = " << (x) << endl;
#define debug(x) cerr << #x << " = " << (x) << " (L" << __LINE__ << ")" << " " << __FILE__ << endl;
#define SIZEOF(x) sizeof(x)/sizeof(x[0])


const long long INF = 4e18;
const long long NINF = 1 - INF;

#define ENDL cout << endl;
#define CIN(a) REP(i,a.size())cin >> a[i];

//二次元座標の点を表す構造体。xとyをメンバに持つ
struct POINT{
    double x;
    double y;

};






//最小公倍数、最大公約数

ll gcd(ll x, ll y) { return y ? gcd(y, x % y) : x; }
ll lcm(ll a, ll b) { return (a / gcd(a, b)) * b; }

/*
    nCr , 値が大きいとオーバーフローする
    n<=kなら1 (0C1や1C5= 1)
*/
ll nCr(ll n,  ll r){
  if ( r * 2 > n ) r = n - r;
  ll dividend = 1;
  ll divisor  = 1;
  for ( unsigned int i = 1; i <= r; ++i ) {
    dividend *= (n-i+1);
    divisor  *= i;
  }
  return dividend / divisor;
}





















/*

    splitした後はmergeを忘れずに！！(それかSplitした頂点のrootを覚えておこう)

    頂点にアクセスしたら、その頂点をsplayするのを忘れるな！！
    頂点をいじったらsplayの後、Updateするのを忘れないで(順番も注意)


    rootを頂点とする部分木において{
        split(l,root) := lで分割

        merge(l_root , r_root) := 頂点がl_root ,r_rootであるような部分木を連結

        insert_sub(ind,NODE,root) := ind番目にNODEが挿入される

        Delete_sub(ind ,root) := ind番目を削除(返り値はfirstが新しいroot , secondが削除したNode)

    }
    これらを駆使し、様々な操作を実装可能

    
    get_position(x)で、数列内のxのindexをまとめたvectorを返す。
    要素xを持つNODEを木に挿入したとき、pos[x] に 挿入したノードをプッシュするが、
    Deleteするときは、特に何もしない
    ただし、get_position(x)でpos[x]の中身を調べて、無効になっている要素があればpopする


    posに入っているNODEは、SplayNodeのポインタなので、数列が更新されると、ポインタの中身も連動して更新される。よって、NODEの位置がを調べたい場合、
    NODEの上の辺を辿っていき、自分の左側の頂点にきたときに、その頂点の部分木のサイズから、自分の位置を計算していける(これをindex()関数とする)

    遅延評価のreverseもつけれそうだが、つけた場合、NODEのポインタを直接持っているときに,
    NODE->index()でNODEの位置を受け取ったときに間違った位置が帰ってくる(遅延評価で位置関係の逆転を降ろしてきていないから)
    これを解決するために、NODEのポインタを直接持っていて、その位置(index())を計算したい場合は、
    そのNODEのparentを辿りながら、左右どっちに進んだかをメモしておいて、一番上からメモした道を辿りながら遅延評価を降ろしてこないといけない
    そのとき、反転の遅延評価を降ろしてくると、辿る道も反転するので注意


    他のライブラリと違い、Delete(i)の後、temp.secondをnullptrにしている 
    ただし、これはshift_subなどの内部の関数でDelete()ではなく、Delete_sub()を使うように統一しているからできることである。


*/
template<class T>
class SearchableArray{
    public:


    /*
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
            SplayTreeの、Nodeのデータ構造。
            SplayTreeでは、Glaphの配列上で操作のやり取りをするのではなく、
            SplayNodeというデータ構造に持たせたデータでやり取りをする
        ------------------------------------------------------------------------------------------------------------------------------------------------------------   
        ------------------------------------------------------------------------------------------------------------------------------------------------------------

    */
    
    struct SplayNode{

        SplayNode *parent;//親ノード
        SplayNode *left;//左の子ノード
        SplayNode *right;//右の子ノード

        T Value;//値(普通、頂点には値や重みが割り当てられることが多い)
        T Min,Max,Sum;//部分木のうち、値の最大、最小、和

        bool isThisDeleted = false;//このノードが、木から削除されたか(getpositionで使用する。shift_rightなどで一時的に切り離された場合は、falseのまま)

        int SubTreeSize = 1;//1は初期化値(自分自身)


        SplayNode(){
            parent = nullptr;
            left = nullptr;
            right = nullptr;
        }
        SplayNode(T val){
            parent = nullptr;
            left = nullptr;
            right = nullptr;
            Value = val;
            Min = val;
            Max = val;
            Sum = val;
        }


        bool isExist(SplayNode *Nd){
            if(Nd!=nullptr)return true;
            else return false;
        } 



        /*
            ある地点を回転させる
            回転時、自分自身(this)と親(Parent)の位置関係が逆になる
            よって、
            下から　Parent　⇨  this の順番でupdateする

        */
        void rotate(){

            SplayNode *Parent , *GrandParent , *Child;
            Parent = this->parent;
            
            Child = nullptr;

            if(isExist(Parent) == false){
                return;
            }
            GrandParent = Parent->parent;
            



            //元の親に対する位置で場合分け
            if(Parent->left == this){
                Child = this->right;//Childはnullptrかもしれない
                this->right = Parent;
                Parent->left = Child;

            }else if(Parent->right == this){
                Child = this->left;
                this->left = Parent;
                Parent->right = Child;

            }

            if(isExist(GrandParent)){
                //GrandParentのどちらの辺なのかで場合分け
                if(Parent == GrandParent->left){
                    GrandParent->left = this;

                }else if(Parent == GrandParent->right){
                    GrandParent->right = this;
                    
                }
            }

            //GrandParentがnullptrの場合でも代入OK
            this->parent = GrandParent;


            //親と位置を入れ替えるので
            Parent->parent = this;



            //Childは移動するので。
            if(isExist(Child))Child->parent = Parent;


            Parent->update();
            this->update();
            

            

            return;

        }







        /*
            自分の親から見た自分の位置
            0 -> 親がそもそもいない or 例外の場合
            1 -> 親から見て左の子の場合
            2 -> 親から見て右の子の場合 
        */
        int state(){
            if(isExist(this->parent)== false){
                return 0;
            }else{
                if(this->parent->left == this){
                    return 1;
                }else if(this->parent->right == this){
                    return 2;
                }else {
                    return 0;
                }
            }
        }







        /*
            あるNodeを回転を駆使し一番上まで持っていく
            この時、回転する順番に注意
        */
        void splay(){

            if(isExist(this) == false)return;
            
            while(isExist(this->parent)){
                if(isExist(this->parent->parent)==false){
                    //(自分のGrandParentがいない場合、rotateは一回のみ)
                    this->rotate();
                    return;


                //それ以外の場合、(自分、親、親の親)の位置関係が大事
                }else if(this->state() == this->parent->state()){
                    //GrandParentからの道が(右、右)or (左、左)の場合
                    //GrandParentを一つ下に下ろして、自分を上げる
                    this->parent->rotate();
                    this->rotate();

                }else{
                    //それ以外の場合、単純に自分を2回上に上げれば良い
                    this->rotate();
                    this->rotate();
                }
            }



            return;
        }   






        /*
            サイズなど、Nodeの持っている情報をupdateする(順番大事)
        */
        void update(){
            if(isExist(this) == false)return;


            this->SubTreeSize = 1;
            this->Min = this->Value;
            this->Max = this->Value;
            this->Sum = this->Value;


            if(isExist(this->left)){
                this->SubTreeSize += this->left->SubTreeSize;
                if(this->left->Min < this->Min)this->Min = this->left->Min;
                if(this->left->Max > this->Max)this->Max = this->left->Max;
                this->Sum += this->left->Sum;
            }

            if(isExist(this->right)){
                this->SubTreeSize += this->right->SubTreeSize;
                if(this->right->Min < this->Min)this->Min = this->right->Min;
                if(this->right->Max > this->Max)this->Max = this->right->Max;
                this->Sum += this->right->Sum;

            }

            

            

            return;
        }



        /*  
            自分自身が木の中で右から何番目かを、上の辺を辿りながら、左の部分木の要素数を足し合わせて計算する
            親が自分の左にあるとき、その親の部分木の左側の要素数を足し、右側なら何もしない
        */
        int index(){
            int res_index = 0;
            SplayNode *now = this;
            if(isExist(now->left))res_index += now->left->SubTreeSize;
            ///親に遷移し続ける(一番上に行くまでのぼる)
            while(isExist(now->parent)){
                
                //親が自分より左のノードなら、indexにその親の左の部分木のサイズを足す
                //右の親の場合、その親のindexは自分よりも大きいから何も足さない
                if(now->state() == 2){
                    //親の部分木のサイズから、自分の部分木のサイズを引くと、ちょうど親の部分木の左側になる
                    res_index += now->parent->SubTreeSize - now->SubTreeSize;
                }
                now = now->parent;
            }
            return res_index;

        }

    };



    /*
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
            ここからやっとSplayTreeに載せる関数たちの出番
            rootを根とする部分木において、
              split(ind,root) , merge(l_root, r_root) , delete_sub(ind,root) , insert_sub()　など
            これらは主にclass内で使われる。
            classの外で使う関数として、
                Delete , insert, RangeSumQuery , shiftなどが想定される
        ------------------------------------------------------------------------------------------------------------------------------------------------------------   
        ------------------------------------------------------------------------------------------------------------------------------------------------------------

    */



    
    int Size = 0;

    //この木のroot,splitした後はmergeして戻す！！rootが迷子にならないように！！
    SplayNode *Root = nullptr;

    //エラー時の返り値
    T initializer;

    //pos[x] := Value = xであるようなSplayNodeのポインタを入れておく。(Value = xであるような要素の探索に使える)
    map<T , vector<SplayNode*> > pos;

    SearchableArray(){

    }

    SearchableArray(int size_){
  
    }

    bool isExist(SplayNode *Nd){
        if(Nd==nullptr){
            return false;
        }else{
            return true;
        }
    }




    /*
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        ここからは、クラス内で動かす関数たちです。俗にいうブラックボックスです。
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    */


    private:







    /*
        rootを根とする部分木において、
        indexを左からの0-indexとすると、

        求めたいindexになるまで降りていく

    */
    SplayNode *getNode(int index , SplayNode *root){
        
        if(isExist(root)==false)return root;
        

        SplayNode *now = root;
        

        while(1){
            
            //left_sizeに代入する前にeval。（後でevalするとleft_sizeの値が変わるから）
            int left_size = 0;
            if(isExist(now->left)){
                left_size = now->left->SubTreeSize;
            } 
            
            if(index < left_size){
                now = now->left;
            }else if(index == left_size){
                now->splay();
                break;
            }else if(index > left_size){
                now = now->right;
                index = index - left_size-1;
            }

        }
        

        return now;
    }













    /*
        それぞれ、leftrootとrightrootが根であるような部分木をくっつける

        splitした後、これをしてまたsplitしたものをくっつける
        splitしたまま放置すると分割された木のrootが行方不明になりそう
        leftとrightの順番大事！！
    */
    SplayNode *merge(SplayNode *leftRoot , SplayNode *rightRoot){

       
        if(isExist(leftRoot) == false)return rightRoot;//右のみあり
        if(isExist(rightRoot) == false )return leftRoot;//左のみあり


        leftRoot = getNode(leftRoot->SubTreeSize-1 , leftRoot);
        leftRoot->right = rightRoot;
        rightRoot->parent = leftRoot;
        leftRoot->update();
        
        
        return leftRoot;
    }









    /*
         rootを根とする部分木において、
        左からleftnum番目の頂点で左右にsplit。
        splitした後はmergeしないと,splitした木の頂点が行方不明になりそう
    */
    std::pair<SplayNode*,SplayNode*> split(int leftnum, SplayNode *root){

        if(leftnum<=0)return std::make_pair(nullptr , root);

        if(leftnum >= root->SubTreeSize)return std::make_pair(root, nullptr);



        root = getNode(leftnum , root);
        
        SplayNode *leftRoot , *rightRoot;

        leftRoot = root->left; 
        
        rightRoot = root; 

        if(isExist(rightRoot))rightRoot->left = nullptr;

        if(isExist(leftRoot))leftRoot->parent = nullptr;
        
        rightRoot->update();
        leftRoot->update();
        

        return std::make_pair(leftRoot,rightRoot);

    }





    /*
        rootを根とする部分木において、
        indexの場所を切り離して、NODEを追加して元に戻す(merge)
    */
    SplayNode  *insert_sub(int index , SplayNode *NODE , SplayNode *root){
        if(isExist(root) == false){
            pos[NODE->Value].push_back(NODE);
            root=NODE;
            Size=1;
            return root;
        }
        if(Size<index || index<0)return root;

        pos[NODE->Value].push_back(NODE);
        std::pair<SplayNode*,SplayNode*> Trees = split(index,root);

        SplayNode *leftRoot = Trees.first;
        SplayNode *rightRoot = Trees.second;
        Size++;

        return merge(merge(leftRoot , NODE),rightRoot);
    }









    /*  
        rootを根とする部分木において、
        左からindexの場所を削除して、両端をくっつける
        firstは消したやつ以外をmergeした時のroot
        secondは消したSplayNode
    */
    std::pair<SplayNode*,SplayNode*> Delete_sub(int index , SplayNode *root){
        
        if(isExist(root) == false){
            return std::make_pair(root,root);
        }

        if(Size == 1){
            Size = 0;
            return std::make_pair(nullptr,root);
        }

        root = getNode(index,root);
        

        SplayNode *leftRoot = root->left;
        SplayNode *rightRoot = root->right;

        if(isExist(leftRoot))leftRoot->parent = nullptr;
        if(isExist(rightRoot))rightRoot->parent = nullptr;
        root->left = nullptr;
        root->right = nullptr;

        Size--;
        
        root->update();
        
        return std::make_pair(merge(leftRoot,rightRoot) , root );
    }








    /*
        rootを根とする部分きにおいて、
        0-indexの区間[l,r)を右シフト(右端は左端へ)する
    */
    SplayNode *shift_right_sub(int l , int r , SplayNode *root ){
        r--;//半開区間なので

        std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(r,root);
        root = tmp.first;
        SplayNode *N = tmp.second;

        return insert_sub(l,N,root);

    }



    /*
        rootを根とする部分きにおいて、
        0-indexの区間[l,r)を左シフト(左端は右端へ)する
    */
    SplayNode *shift_left_sub(int l , int r , SplayNode *root ){
        r--;//半開区間なので

        std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(l,root);
        root = tmp.first;
        SplayNode *N = tmp.second;

        return insert_sub(r,N,root);

    }









    /*
        rootを根とする部分木の[l,r)の区間取得クエリ
        modeが 0->Min, 1->Max ,2->Sum 
    */
    std::pair<SplayNode*,T> RangeQuery_sub(int l , int r , int mode , SplayNode *root){

        SplayNode *leftRoot , *centerRoot , *rightRoot; 
        r--;//半開区間なので

        std::pair<SplayNode*,SplayNode*>  tmp = split(r+1,root);
        rightRoot = tmp.second;
        
        tmp = split(l,tmp.first);
        leftRoot = tmp.first;
        centerRoot = tmp.second;
        
        T ans;
        //modeによって取得する値が違う
        if(mode == 0){
            ans  = centerRoot->Min;
        }else if(mode == 1){
            ans  = centerRoot->Max;
        }else if(mode == 2){
            ans = centerRoot->Sum;
        }else{
            ans = centerRoot->Sum;
        }

        return std::make_pair(merge(merge(leftRoot,centerRoot),rightRoot),ans);
    }






    /*
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        ここからは、クラスの外で動かす関数たちです。
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    */



     public:



    /*
        木全体のindex番目のNode(値渡し)を取得
    */
    SplayNode get(int index){
        if(index<0 || index>=Size){
            std::cerr<< "index exceeds Size" << std::endl;
            return SplayNode(initializer);
        }

        Root = getNode(index,Root);
        Root->update();

        return *(Root);

    }




    /*
        index番目にSplayNode*を挿入
    */
    void insert(int index , SplayNode *NODE){
        if(index<0 || index> Size)return;

        Root = insert_sub(index , NODE , Root);
        Root->update();
        

        return;

    }





    /*
        末尾に追加

    */
    void push_back(T val){
        insert(Size , new SplayNode(val)); 

        return;
    }

    /*
        先頭に追加
    */
    void push_front(T val){
        insert(0 , new SplayNode(val));
        return;
    }


    /*
        index番目に追加
    */
    void push(int index , T val){
        insert(index , new SplayNode(val));
        return;
    }









    /*
        木全体の[l,r)のRMQ
    */
    T RangeMinQuery(int l , int r){
        l = max(0,l);
        r = min(Size,r);
        if(l>=r){
            cerr << "right must be larger than left " << endl;
            return initializer;
        }

        std::pair<SplayNode*,T> tmp = RangeQuery_sub(l,r,0,Root);
        Root = tmp.first;
        
        Root->update();

        return tmp.second;
    }


    /*
        木全体の[l,r)のRMQ
    */
    T RangeMaxQuery(int l , int r){
        l = max(0,l);
        r = min(Size,r);
        if(l>=r){
            cerr << "right must be larger than left " << endl;
            return initializer;
        }

        std::pair<SplayNode*,T> tmp = RangeQuery_sub(l,r,1,Root);
        Root = tmp.first;
        Root->update();

        return tmp.second;
    }



    /*
        木全体の[l,r)のRSQ
    */
    T RangeSumQuery(int l , int r){
        l = max(0,l);
        r = min(Size,r);
        if(l>=r){
            cerr << "right must be larger than left " << endl;
            return initializer;
        }

        std::pair<SplayNode*,T> tmp = RangeQuery_sub(l,r,2,Root);

        Root = tmp.first;
        Root->update();

        return tmp.second;
    }






    /*
        木全体で、[l,r)を右シフト(右端は左端へ)
    */
    void shift_right(int l , int r){
        l = max(0,l);
        r = min(Size,r);
        if(l+1>=r)return;

        Root = shift_right_sub(l,r,Root);
        Root->update();

        return;
    }





    /*
        木全体で、[l,r)を左シフト(左端は右端へ)
    */
    void shift_left(int l , int r){
        l = max(0,l);
        r = min(Size,r);
        if(l+1>=r)return;

        Root = shift_left_sub(l,r,Root);
        Root->update();

        return;
    }






    /*
        木全体のx番目のvalueをyにする
    */
    void update_val(int x , T y){
    
        Root = getNode(x,Root);

        if(isExist(Root)==false)return;
        Root->Value = y;
        Root->update();
        
        pos[y].push_back(Root);//pos[y]にpushするのを忘れずに

        return;
    }


    /*
        index番目のNodeを消去
    */
    void Delete(int index){
        if(index<0 || index >= Size)return ;
        std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(index,Root);
        Root = tmp.first;
        if(tmp.second != nullptr)tmp.second->isThisDeleted = true;
        if(isExist(Root) == false )return;
        Root->update();

        return;
    }








    //数列内の、xの位置を全てvector<int>に入れて返す
    vector<int> get_position(T x){
        if(Size == 0)return vector<int>(0);

        vector<SplayNode*> pos_x = pos[x];
        vector<int> res;

        int size = pos_x.size();

        
        //shift_rightなどで、辺や頂点を切ったり張ったりすると,
        //pos[x]の中で同じ頂点が重複して存在することになるので、そこも弾くためにチェック
        map<int,bool> checked;

        for(int i = size -1  ; i >= 0 ; i--){
            SplayNode *Nd = pos_x[i];
            
 
            
            if(Nd == nullptr)pos_x.erase(pos_x.begin()+i);
            else if(Nd->isThisDeleted)pos_x.erase(pos_x.begin()+i);//削除済みを弾く
            else if(Nd->Value != x)pos_x.erase(pos_x.begin()+i);//値の変更ずみを弾く
            else{

                //NODEのindex関数で、上の辺を辿って、自分がどの位置にあるのかを取得(splayして自分を上に持って行ってもいいけど、Deleteで木から削除された頂点を弾く場合の処理が面倒)
                int index = Nd->index();
            
                /* 
                    また、木からDeleteされた頂点を弾くと書いたが、shiftなどの操作によって、一度は木から削除されたが、後に木に戻ってきた頂点は弾かないようにしている
                */
                T val = get(index).Value; 

                //index番目が、本当にxと同じかどうかをチェック(deleteによって無効になったノードもあるので)
                if(val == x){
                    //すでにみていた場合、このNODEはshiftなどの操作により、pos[x]に複数存在するということなので、pop
                    if(checked[index])pos_x.erase(pos_x.begin()+i);
                    else res.push_back(index);    

                    checked[index] = true;
                }else pos_x.erase(pos_x.begin()+i);
                
                
            }
        }
        pos[x] = pos_x;//削除とかをしたので、更新しておく
        
        sort(res.begin() , res.end());
        return res;
    }






    /*
        一列に表示
    */
    void Debug(){
        std::cerr<<"DEBUG:" << std::endl;
        for( int i = 0 ; i < Size ; i++){
            std::cerr<< get(i).Value << " ";
        }
        std::cerr<< std::endl;
    }




};


















int main(){

    SearchableArray<long long > S;
    vector<long long > A = { 1 , 5 , 2 , 1 , 4 , 3 , 2 };
    for(long long c : A)S.push_back(c);
    S.Debug();
    vector<int> pos;
    
    pos = S.get_position(1);
    for(int i : pos)cout << i << " ";cout << endl;
    cout << "------------------------------" << endl;
    S.push(5,1);
    S.push(2,1);
    S.Debug();
    pos = S.get_position(1);
    for(int i : pos)cout << i << " ";cout << endl;
    cout << "------------------------------" << endl;
    S.Delete(0);
    S.Delete(4);
    S.Debug();
    pos = S.get_position(1);
    for(int i : pos)cout << i << " ";cout << endl;
    cout << "------------------------------" << endl;
    S.shift_right(3,S.Size);
    S.Debug();
    pos = S.get_position(1);
    for(int i : pos)cout << i << " ";cout << endl;
    cout << "------------------------------" << endl;

    S.Debug();
    cout << S.RangeMaxQuery(0,S.Size) << endl;
    cout << S.RangeSumQuery(2,6) << endl;
    return 0;
}