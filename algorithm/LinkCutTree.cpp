
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
    
    
    
    (-1を返す可能性がある関数)
    find_root(x) := xの属する連結成分のrootを見つける
    identify_upper(x,y) := x,yが同じ連結成分内の頂点の時、上の方(rootに近い方)を返す
    identify_lower(x,y) := x,yが同じ連結成分内の頂点の時、下の方(rootから遠い方)を返す
    find_parent(x) := xの親を返す
    isExist_Edge (u,v) := 辺(u,v)が存在するか
    
    パスに対して、一律に更新 (add や update) をしたい場合は、reverse と同様に遅延評価したら良い
    辺の表現には頂点拡張をすると良い
    バグっている可能性もあるので、安心しないでね
*/
template<class T>
class LinkCutTree{
    private:
    /*
        -----------------------------------------------------------------
        -----------------------------------------------------------------
            LinkCutTreeの、Nodeのデータ構造。splay木のノードとほぼ同じ
        -----------------------------------------------------------------
        -----------------------------------------------------------------
    */
    
    struct LinkCutNode{
        LinkCutNode *parent = nullptr;//親ノード
        LinkCutNode *left = nullptr;//左の子ノード
        LinkCutNode *right = nullptr;//右の子ノード
        T Value;//値(普通、頂点には値や重みが割り当てられることが多い)
        T Min,Max,Sum;//部分木のうち、値の最大、最小、和
        int id = -1;//頂点番号
        int SubTreeSize = 1;//1は初期化値(自分自身)
        bool reverse_flag_lazy = false;
        void set_lazyReverse(){
            this->reverse_flag_lazy = !(this->reverse_flag_lazy);//反転flagを更新
        }

        LinkCutNode(){}
        LinkCutNode(int id_ ,T val){
            id = id_;
            Value = val;
            Min = val;
            Max = val;
            Sum = val;
        }
        
        // this - this->parent を回転させる (親との Heavy Edge は必ず存在するものとし、位置関係も誤りがないものとする)
        void rotate(){
            //GrandParentの子を張り替え (light edge かどうかもチェック)
            if(this->parent->parent){
                if(this->parent == this->parent->parent->left)this->parent->parent->left = this;
                else if(this->parent == this->parent->parent->right)this->parent->parent->right = this; 
            }
            
            this->parent->eval();
            this->eval();
            
            //元の親に対する位置で場合分け
            if(this->parent->left == this){
                this->parent->left = this->right;
                if(this->right)this->right->parent = this->parent;
                this->right = this->parent;
                this->parent = this->right->parent;
                this->right->parent = this;
                this->right->update();
            }else{
                this->parent->right = this->left;
                if(this->left)this->left->parent = this->parent;
                this->left = this->parent;
                this->parent = this->left->parent;
                this->left->parent = this;
                this->left->update();
            }

            this->update();
            return;
        }
        /*
            自分の親から見た自分の位置 (親は必ず存在し、位置関係に誤りがないものとする)
            0 -> 親がない or 親方向が light edge (LinkCutTree は特別)
            1 -> 親から見て左の子の場合
            2 -> 親から見て右の子の場合
        */
        int state(){
            if(bool(this->parent)== false )return 0; // 親がない
            else{
                this->parent->eval();
                if(this->parent->left == this)return 1;
                else if(this->parent->right == this)return 2;
                else return 0; // light edge 
            }
        }
        /*
            あるNodeを回転を駆使し一番上まで持っていく
            この時、回転する順番に注意
        */
        void splay(){ 
            this->eval();
            while(this->state()!=0){ // light edge まで上がる
                if(this->parent->state()==0){
                    //(自分のGrandParentがいない場合、rotateは一回のみ)
                    this->rotate();
                    return ;
                //それ以外の場合、(自分、親、親の親)の位置関係が大事
                }else if(this->parent->state() == this->state()){
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
            this->eval();
            this->SubTreeSize = 1;
            this->Min = this->Value;
            this->Max = this->Value;
            this->Sum = this->Value;
            if(bool(this->left)){
                this->left->eval();
                this->SubTreeSize += this->left->SubTreeSize;
                if(this->left->Min < this->Min)this->Min = this->left->Min;
                if(this->left->Max > this->Max)this->Max = this->left->Max;
                this->Sum += this->left->Sum;
            }
            if(bool(this->right)){
                this->right->eval();
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
                if(bool(this->left))this->left->set_lazyReverse();
                if(bool(this->right))this->right->set_lazyReverse();
                this->reverse_flag_lazy = false;
            }
        }
    };

    private:
                   
    int V = 0;// サイズ
    //Nodes[i] := 頂点 i のポインタ
    vector<LinkCutNode*> Nodes;
    
    // root を根とするような 連結成分(パス)のうち、左から i 番目のノードを取得
    // root とは、木の根ではなく splay tree の根のこと。つまり access(root)しておく必要がある
    LinkCutNode *getNode(int index , LinkCutNode *root){
        if(bool(root)==false)return root;
        LinkCutNode *now = root;
        while(true){
            now->eval(); // ノードを見る前にeval
            int left_size = 0;
            if(bool(now->left))left_size = now->left->SubTreeSize;
            if(index < left_size)now = now->left;
            else if(index == left_size){
                now->splay();
                break;
            }else{
                now = now->right;
                index = index - left_size-1;
            }
        }
        return now;
    }
    /*
        Nodeまでheavy edgeで繋げて、heavy edgeで繋げたpathをsplay木として管理する
        splay木で左の頂点ほど、根に近い
        最後にlight edgeを辿った先の頂点のidを返す(LCAを求めるときに使う)
    */
    int access_sub(LinkCutNode* Node){
        if(bool(Node) == false)return -1;//存在しない場合
        int res = Node->id;
        while(1){
            Node->splay();
            Node->right = nullptr;
            Node->update();
            if(bool(Node->parent) == false)break;
            res = Node->parent->id;
            
            Node->parent->splay();
            Node->parent->right = Node;
            Node->parent->update();
        }
        return res;
    }
    /*
        c -> p に辺をリンク
        副作用 : p にアクセスする
    */
    void link_sub(LinkCutNode* c , LinkCutNode* p ){
        if(bool(c) && bool(p)){
            evert_sub(c);
            access_sub(p);
            c->parent = p;
            p->update(); // light edge の情報を持たせる場合は update が必要
        }
    }
    /*
        Nodeを根にもつ部分木をCutする
        (Nodeより上に頂点がなければCutできない)
    */
    void cut_sub(LinkCutNode* Node){
        if(bool(Node) == false)return;
        access_sub(Node);
        if(bool(Node->left) == false)return;
        Node->left->parent = nullptr;
        Node->left = nullptr;
        Node->update();
    }
    /*
        Nodeを , Nodeが属するHeavy Edgeの連結成分のrootに
        つまり、Nodeが属するheavy edgeの向きを上下反転する
    */
    void evert_sub(LinkCutNode *Node){
        if(bool(Node) == false)return;
        access_sub(Node);
        Node->set_lazyReverse();
        Node->update();
        access_sub(Node);
    }
    public:
    
    
    // 頂点数と、各頂点が持つ値の初期値
    LinkCutTree(int size_ , T init_ = 0){
        V = size_ + 10;
        Nodes = vector<LinkCutNode*>(V);
        REP(i,V)Nodes[i] = new LinkCutNode(i,init_);
    }
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
        副作用 : access(i)
    */
    void update_val(int i , T x){
        access(i);
        Nodes[i]->Value = x;
        Nodes[i]->update();
    }
    /*
        頂点 x が属する部分木(連結成分)のrootを求める(エラー時は-1を返す)
        副作用 : x が属する部分木の root に access する
    */
    int find_root(int x){
        if(x< 0 || x >=V)assert(0);
        access(x);
        LinkCutNode* now = Nodes[x];
        now->eval();
        while(bool(now->left)){
            now = now->left;
            now->eval();
        }
        now->splay();
        now->update();
        access(now->id);
        return now->id;
    }
    /*
        同じ連結成分に属する 頂点 x と、頂点 y のどちらが深さが浅い頂点か(同じ深さの場合は-1を返す)
        副作用 : 浅い方の頂点にアクセスする
    */
    int identify_upper(int x , int y){
        if(min(x,y)< 0 || max(x ,y)>=V)assert(0);
        if(find_root(x) != find_root(y))assert(0);
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
        同じ連結成分に属する 頂点 x と、頂点 y のどちらが深さが深い頂点か(同じ深さの場合は-1を返す)
        副作用 : 下の方の頂点にアクセスする
    */
    int identify_lower(int x , int y){
        if(min(x,y)< 0 || max(x ,y)>=V)assert(0);
        if(find_root(x) != find_root(y))assert(0);
        
        int upper_v = identify_upper(x,y);
        if(upper_v!=-1){
            access((x^y)^upper_v);
            return (x^y)^upper_v;//上じゃない方
        }else{
            return -1;
        }
    }
    /*
        頂点 x の親を見つける(存在しないなら-1を返す)
        副作用 : 親が存在するなら、その親にaccessする
    */
    int find_parent(int x){
        if(x < 0 || x >= Nodes.size())assert(0);
        if(bool(Nodes[x])==false)assert(0);
        access(x);
        LinkCutNode* now = Nodes[x];
        now->eval();
        if(bool(now->left) == false)return -1;
        now = now->left;
        now->eval();
        
        while(bool(now->right) == true){
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
        副作用 : どこかしらに access した
    */
    bool isExist_Edge(int u , int v){
        if(find_root(u) != find_root(v))return false;
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
        c - p をつなげる
        c の親をpにする
        副作用 : 親の方にaccessする
    */
    void link(int c , int p){
        if(find_root(c) == find_root(p))return;
        link_sub(Nodes[c] , Nodes[p]);
    }
    /*
        頂点番号がidの頂点をねとする部分木を、木から切り離す
        つまり、辺:(id , idの親)をなくす
        副作用 : access の状態が不定になる
    */
    void cut(int id){
        if(id < 0 || id>=V)return;
        cut_sub(Nodes[id]);
    }
                   
    /*
        辺 u-v をカット
        副作用 : access の状態が不定になる
    */
    bool cut(int u , int v){
        if(min(u,v) < 0 || max(u,v) >= V )return false;
        if(!isExist_Edge(u,v))return false;
        cut(identify_lower(u,v));
        return true;
    }
    /*
        xとyのLCAを求める
        無効なクエリなら、-1
        副作用 : access(lca) する
    */
    int LCA(int x , int y){
        if(min(x,y) < 0 || max(x,y) >= V)assert(0);
        if(find_root(x) != find_root(y))return -1;
        else {
            access(x);//最後にlightedgeを辿って到達した、先を返すので、ちょうど分岐点を返すようになっている
            int lca = access(y);
            access(lca);
            return lca;
        }
    }
    /*
        v の属する連結成分の根から頂点 v までのパスのうち、根から x 番目(0-index)の頂点の番号
        未定義なら -1 を返す。
        副作用 : access(v) する
    */
    int path_xth_element(int v , int x){
        if(x < 0)assert(0);
        access(v);// 副作用
        if(Nodes[v]->SubTreeSize <= x)return -1;
        LinkCutNode* nd = getNode(x , Nodes[v]);
        nd->update();
        access(v); // 副作用を統一
        return nd->id;
    }
    // read-only で ノード のコピーを取得 ( 副作用で ノードに access (重要) )
    LinkCutNode operator [](int nodeID){
        assert(nodeID>=0 && nodeID < int(Nodes.size()) );
        access(nodeID);
        return *(Nodes[nodeID]);
    }
    // 頂点数
    int size(){
        return V;
    }
};







