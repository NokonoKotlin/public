
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

    木構造
    辺を切ったり貼ったりできる

    辺をHeavyEdge と LightEdgeに分けて管理、

    add_edgeでとりあえず無向辺を作り、
    build(r)で指定したrootに向かって向きをつける

    accessがとっても大事！！忘れないでね！！
    頂点の所得は Nodes から直接とってもいいです(accessは忘れないでね)。

    evert(x) := xを連結成分のrootに
    link(x,y) := xを連結成分(path)のrootにしたのちに、yの子供として連結する(すでに連結ならpass)
    cut(x) := xをrootとする部分木を親から切り離す
    
    access(x) := xまでのpathを構築して、SplayTreeとして連結させ、連結成分にする
    accessすると、accessした頂点までをHeavyEdgeで繋ぐ
    accessした頂点以降(子供)へのHeavyEdgeをLightEdgeにする
    根から出ているHeavyEdgeによるpathは分岐しないので(各頂点について、そこから出るHeavyEdgeは高々1つ)、
    根からHeavyEdgeを辿っていける頂点は、根から近い順に横に並べることができる(分岐がないので)
    根からHeavyEdgeを辿っていける頂点を、根から近い順に並べたものを、splayTreeで管理する
    よって、access(x)で繋がったpathを表すsplaytreeは、左の方が根に近く、右に行くほど下の頂点になる
    
    
    
    
    

    (エラー時は-1を返す関数)
    find_root(x) := xの属する連結成分のrootを見つける
    identify_upper(x,y) := x,yが同じ連結成分内の頂点の時、上の方(rootに近い方)を返す
    identify_lower(x,y) := x,yが同じ連結成分内の頂点の時、下の方(rootから遠い方)を返す
    find_parent(x) := xの親を返す


    isExist_Edge (u,v) := 辺(u,v)が存在するか
    


    バグっている可能性もあるので、安心しないでね

*/
template<class T>
class LinkCutTree{
    public:


    /*
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
            LinkCutTreeの、Nodeのデータ構造。splay木のノードとほぼ同じ
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
        ------------------------------------------------------------------------------------------------------------------------------------------------------------

    */
    
    struct LinkCutNode{

        LinkCutNode *parent;//親ノード
        LinkCutNode *left;//左の子ノード
        LinkCutNode *right;//右の子ノード

        T Value;//値(普通、頂点には値や重みが割り当てられることが多い)
        T Min,Max,Sum;//部分木のうち、値の最大、最小、和

        int id = -1;//頂点番号

        int SubTreeSize = 1;//1は初期化値(自分自身)

        bool reverse_flag_lazy = false;

        LinkCutNode(){
            parent = nullptr;
            left = nullptr;
            right = nullptr;
            
        }


        LinkCutNode(int id_ ,T val){
            parent = nullptr;
            left = nullptr;
            right = nullptr;
            id = id_;
            Value = val;
            Min = val;
            Max = val;
            Sum = val;
        }


        bool isExist(LinkCutNode *Nd){
            if(Nd!=nullptr)return true;
            else return false;
        }
        

        /*
            自分がSplayTreeの根かどうか
        */
        bool isRoot(){
            //親がいない || 親の子が自分ではない(親から自分にheavy edge が出てない)
            return (isExist(this->parent) == false || (this->parent->left != this && this->parent->right!=this));
        }


        /*
            自分自身が木の中で右から何番目かを、上の辺を辿りながら、左の部分木の要素数を足し合わせて計算する
            親が自分の左にあるとき、その親の部分木の左側の要素数を足し、右側なら何もしない
        */
        int index(){
            int res_index = 0;
            LinkCutNode *now = this;
            if(isExist(now->left))res_index += now->left->SubTreeSize;
            ///親に遷移し続ける(一番上に行くまでのぼる)
            while(this->state()!=0){
                
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



        /*
            ある地点を回転させる
            遅延評価の順番には注意！
            回転時、自分自身(this)と親(Parent)の位置関係が逆になる
            よって、
            下から　Parent ⇨  this の順番でupdateする

            lazy作用については結構ややこしい図を書いて理解しないといけないが、とりあえずは
            回転を行う前にthisから遅延評価を降ろして、
            回転後にParent(回転後は子)のlazy作用値をthisに移動(swapで解決)
            その後Parentの左右の子の遅延評価を降ろし、Parent->thisの順にupdate()
        */
        void rotate(){

            LinkCutNode *Parent , *GrandParent , *Child;
            Parent = this->parent;
            
            Child = nullptr;

            if(isExist(Parent) == false){
                return;
            }
            GrandParent = Parent->parent;

                        
            Parent->eval();//回転を行う前にevaluate
            this->eval();//回転を行う前にevaluate


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

            if(isExist(GrandParent) ){
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


            /*
                Nodeの持つ情報をupdateしないといけないが、順番に注意！！
                ChildはChild以下の部分木が変化してないので何もしなくてOK
                GrandParentは、部分木の中の順番が変わったけど、集合としては変わってないので何もしなくてOK
                Parentと自分自身(this)は順番が変わって、部分木の内容も変わったので、
                下から　Parent ⇨  this の順番でupdateする
            */

            Parent->update();
            this->update();

            

            return;

        }







        /*
            自分の親から見た自分の位置
            0 -> 親がそもそもいない or 例外の場合
            1 -> 親から見て左の子の場合
            2 -> 親から見て右の子の場合
            3 -> 親と自分に辺が通っていない時(自分からの有向辺のみがある場合)
        */
        int state(){
            if(isExist(this->parent)== false ){
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
            this->eval();
            
            while(this->state()!=0){//親がいない or 親から自分に辺が繋がっていない
                
                if(this->parent->state()==0){
                    //(自分のGrandParentがいない場合、rotateは一回のみ)
                    this->rotate();
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
            evaluate = trueなら遅延評価を下に伝播
            
        */
        void update(bool evaluate = true){
            if(isExist(this) == false)return;

            /*  
                */
            if(evaluate)this->eval();
            if(isExist(this->left) && evaluate)this->left->eval();
            if(isExist(this->right) && evaluate)this->right->eval();


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



        void eval(){
            if(this->reverse_flag_lazy){
                swap(this->left , this->right);
                //下に伝播
                if(isExist(this->left))this->left->reverse_flag_lazy = !(this->left->reverse_flag_lazy);
                if(isExist(this->right))this->right->reverse_flag_lazy = !(this->right->reverse_flag_lazy);
                this->reverse_flag_lazy = false;
            }
        }

        


    };






    
    int Size = 0;

    //エラー時の返り値
    T initializer;

    //Nodes[i] := 頂点 i のポインタ
    vector<LinkCutNode*> Nodes;

    //辺が全て有向辺出会った場合のグラフ(木)
    vector<vector<int> > Graph;



    LinkCutTree(){

    }

    LinkCutTree(int size_){
        init(size_);
    }

    void init(ll size_){
        Size = size_ + 10;
        Nodes = vector<LinkCutNode*>(Size);
        REP(i,Size)Nodes[i] = new LinkCutNode(i,initializer);
        Graph = vector<vector<int> > (Size);
    }

    bool isExist(LinkCutNode *Nd){
        if(Nd==nullptr){
            return false;
        }else{
            return true;
        }
    }





    /*
        buildする前は、両方向に無向辺を貼る
    */
    void add_edge(int u , int v){
        if(max(u,v) >= Size)return;
        if(min(u,v) < 0)return;
        Graph[u].push_back(v);
        Graph[v].push_back(u);
    }


    /*
        Graphに貼られた有向辺をもとに、LinkCutTreeを構築する
        子から親に有効辺を張るようにする
    */
    void build(int r){
        //dfsする
        
        int s = r;
        stack<int> st;
        st.push(s);
        vll memo(Size,0);
        memo[s] = 1;
        while(!st.empty()){
            int now = st.top();
            st.pop();
            for(int v: Graph[now]){
                if(memo[v] == 0){
                    memo[v]=1;
                    
                    Nodes[v]->parent = Nodes[now];//子から親に有向辺をはる
                    st.push(v);
                }
            }
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
        Nodeまでheavy edgeで繋げて、heavy edgeで繋げたpathをsplay木として管理する
        splay木で左の頂点ほど、根に近い

        最後にlight edgeを辿った先の頂点のidを返す(LCAを求めるときに使う)
    */
    int access_sub(LinkCutNode* Node){
        if(isExist(Node) == false)return -1;//存在しない場合
        int res = Node->id;
        while(1){
            Node->splay();
            Node->right = nullptr;
            Node->update();
            if(isExist(Node->parent) == false)break;
            res = Node->parent->id;
            
            Node->parent->splay();
            Node->parent->right = Node;
            Node->parent->update();
        }
        return res;
    }







    /*
        c -> p に有向辺をリンク
    */
    void link_sub(LinkCutNode* c , LinkCutNode* p ){
        if(isExist(c) && isExist(p)){
            evert_sub(c);
            c->parent = p;
        }
    }



    /*
        Nodeを根にもつ部分木をCutする
        (Nodeより上に頂点がなければCutできない)
    */
    void cut_sub(LinkCutNode* Node){
        if(isExist(Node) == false)return;
        access_sub(Node);
        if(isExist(Node->left) == false)return;
        Node->left->parent = nullptr;
        Node->left = nullptr;
        Node->update();
    }


    /*
        Nodeを , Nodeが属するHeavy Edgeの連結成分のrootに
        つまり、Nodeが属するheavy edgeの向きを上下反転する
    */
    void evert_sub(LinkCutNode *Node){
        if(isExist(Node) == false)return;
        access_sub(Node);
        Node->reverse_flag_lazy = !(Node->reverse_flag_lazy);//反転flagを更新
        Node->update();
    }


    public:





    /*
    
        クラス外で使うよう

    */

    /*
        頂点:idにアクセス
        最後にlight edgeを辿った先の頂点番号を返す(LCA用)
    */
    int access(int id){
        return access_sub(Nodes[id]);
    }



    /*
        頂点 i のもつ値(value)を x に変更する
    */
    void update_val(int i , T x){
        access(i);
        Nodes[i]->Value = x;
        Nodes[i]->update();
    }




    /*
        頂点 x が属する部分木(連結成分)のrootを求める(エラー時は-1を返す)
    */
    int find_root(int x){
        if(x< 0 || x >=Size)return -1;
        access(x);
        LinkCutNode* now = Nodes[x];
        now->eval();
        while(isExist(now->left)){
            now = now->left;
            now->eval();
        }
        now->splay();
        now->update();
        access(now->id);
        return now->id;
    }



    /*
        頂点 x と、頂点 y のどちらが上の頂点か(エラー時は-1を返す)
    */
    int identify_upper(int x , int y){
        if(min(x,y)< 0 || max(x ,y)>=Size)return -1;
        if(find_root(x) != find_root(y))return -1;
        //SubTreeSize := 部分木の大きさ = pathの長さが小さい方が、根に近い
        access(x);ll dx = Nodes[x]->SubTreeSize;
        access(y);ll dy = Nodes[y]->SubTreeSize;
        if(dx < dy){
            access(x);
            return x;
        }else if(dy<dx){
            access(y);
            return y;
        }else return -1;
    }


    /*
        頂点 x と、頂点 y のどちらが下の頂点か(エラー時は-1を返す)
    */
    int identify_lower(int x , int y){
        if(min(x,y)< 0 || max(x ,y)>=Size)return -1;
        if(find_root(x) != find_root(y))return -1;
        
        int upper_v = identify_upper(x,y);
        if(upper_v!=-1){
            access((x^y)^upper_v);
            return (x^y)^upper_v;//上じゃない方
        }else{
            return -1;
        }
    }




    /*
        頂点 x の親を見つける(エラー時は-1を返す)
    */
    int find_parent(int x){
        if(x < 0 || x >= Nodes.size())return -1;
        if(isExist(Nodes[x])==false)return -1;
        access(x);
        LinkCutNode* now = Nodes[x];
        now->eval();
        if(isExist(now->left) == false)return -1;
        now = now->left;
        now->eval();
        
        while(isExist(now->right) == true){
            now = now->right;
            now->eval();
        }
        
        now->splay();
        now->update();
        access(now->id);
        return now->id;
        
    }



    /*
        辺(u-v)が存在するか
    */
    bool isExist_Edge(int u , int v){
        int upv = identify_upper(u,v);
        int lwv = identify_lower(u,v);
        int p = find_parent(lwv);
        if(upv == p && upv != -1 && lwv != -1 && p!= -1)return true;
        else return false;
    }



    /*
        頂点xを , 頂点xが属するHeavy Edgeの連結成分のrootに
        つまり、頂点xが属するheavy edgeの向きを上下反転する
    */
    void evert(int x){
        evert_sub(Nodes[x]);
    }   



    /*
        cの親をpにする
    */
    void link(int c , int p){
        if(find_root(c) == find_root(p))return;
        link_sub(Nodes[c] , Nodes[p]);

    }



    /*
        頂点番号がidの頂点をねとする部分木を、木から切り離す
        つまり、辺:(id , idの親)をなくす
    */
    void cut(int id){
        if(id < 0 || id>=Size)return;
        
        cut_sub(Nodes[id]);
    }



    /*
        xとyのLCAを求める
        無効なクエリなら、-1する
    */
    int LCA(int x , int y){
        if(min(x,y) < 0 || max(x,y) >= Size)return -1;

        if(find_root(x) != find_root(y))return -1;
        else {
            access(x);//最後にlightedgeを辿って到達した、先を返すので、ちょうど分岐点を返すようになっている
            int lca = access(y);
            access(lca);
            return lca;
        }

    }



};




























int main(){
    LinkCutTree<long long > T(10);

    T.add_edge(1,3);
    T.add_edge(1,4);
    T.add_edge(1,6);
    T.add_edge(4,5);
    T.add_edge(3,7);
    T.add_edge(7,8);
    T.add_edge(7,9);
    T.add_edge(6,10);

    T.build(1);

    T.update_val(1,9);
    T.update_val(1,2);
    T.update_val(3,5);
    T.update_val(4,3);
    T.update_val(5,2);
    T.update_val(6,8);
    T.update_val(7,1);
    T.update_val(8,2);
    T.update_val(9,1);
    T.update_val(10,5);

    cout << "------------------------------" << endl;

    T.access(5);
    //パス:1-5 上の頂点の頂点の重みのマージを計算
    cout << T.Nodes[5]->Sum << endl;
    cout << T.Nodes[5]->Min << endl;

    cout << "------------------------------" << endl;
    //1が根である木でLCAを計算
    cout << T.LCA(8,9) << endl;

    //頂点を切って、張る
    T.cut(8);
    T.link(8,3);
    cout << T.LCA(8,9) << endl;

    cout << "------------------------------" << endl;
    //根を変更する
    T.evert(5);
    cout << T.LCA(6,3) << endl;

    cout << "------------------------------" << endl;
    //パス :  7 - 10 の長さを求める2つの方法
    //1 : 根を7に張り替えて、10までパスを繋げる
    T.evert(7);
    T.access(10);
    cout << T.Nodes[10]->SubTreeSize - 1 << endl;

    //2 : LCA を求めて、余計な部分を減算する
    ll d7 , d10 , d_lca;
    T.evert(1);
    
    T.access(7);d7 = T.Nodes[7]->SubTreeSize-1;
    T.access(10);d10 = T.Nodes[10]->SubTreeSize-1;
    T.access(T.LCA(7,10));d_lca = T.Nodes[T.LCA(7,10)]->SubTreeSize-1;
    cout << d7 + d10 - 2*d_lca << endl;

    
    return 0;
}










