#include "MyTemplate.hpp"







    
/*
    複数の 半開区間 : [ l , r ) を管理するデータ構造(set)
    r の値でソートして、探索するときも r の値の大小で探索
    重複する部分がある区間同士はマージして管理
        : ex.[ 1 , 6 ) と [ 3 , 8 ) が同時に set に入れられた場合は [ 1 , 8 ) として管理
    
    void insert( l , r ) : [ l , r ) を setに追加
    void erase(l , r ) := set に入っている区間たちの中で、区間 [ l , r ) と重複している部分を削除
        : ex. [ 1 , 6 ) , [ 9 , 11 ) が set に入っている状態で erase( 5 , 9 ) をすると、 [ 1 , 5 ) , [ 9 , 11 ) になる
    T cover_length( l , r ) := set に入っている区間たちと、[ l , r ) の重複している長さを返す
    /////////////ただし、区間 [ l , r ) の長さは r-l であるとする/////////////
    
    int find_included( x ) := setの区間のうち、点 x を含む区間のindexを返す(存在しなければ-1を返す)
    int find_cross( l , r ) := setの区間のうち、[ l , r ) と交差する区間のindexを+++一つ+++返す(存在しなければ-1を返す)
    その他、通常のSetライブラリとほぼ同様
    RangeLengthSumQuery( l , r ) := setに入っている区間のうち、l番目からr-1番目までの区間の長さの総和を返す
*/
template<class T >
class RangeSet{
    private:
    
    
    struct SplayNode{
        SplayNode *parent = nullptr;//親ノード
        SplayNode *left = nullptr;//左の子ノード
        SplayNode *right = nullptr;//右の子ノード
 
        T R;//区間の右端
        T L;//区間の左端
        //区間は半開区間で [ L , R )
        T length_sum;//部分木のうち、区間がカバーしている範囲の長さの和
        int SubTreeSize = 1;//1は初期化値(自分自身)
        SplayNode(){}

        SplayNode(T l , T r){
            if(l >= r)swap(l,r);
            R = r;
            L = l;
            length_sum = R-L;
        }
 

        // this - this->parent を回転させる (親は必ず存在するものとし、位置関係も誤りがないものとする)
        void rotate(){
            if(this->parent->parent){//GrandParentの子を張り替え
                if(this->parent == this->parent->parent->left)this->parent->parent->left = this;
                else this->parent->parent->right = this; 
            }
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
            自分から見た親の位置
            1 -> 親の左の場合
            2 -> 親の右の場合
        */
        int state(){
            if(this->parent->left == this)return 1;
            return 2;
        }

        // あるNodeを回転を駆使し一番上まで持っていく
        void splay(){
            while(bool(this->parent)){
                if(this->parent->parent == nullptr){
                    this->rotate();
                    break;
                }
                // 回転の方法
                if( this->parent->state() == this->state() )this->parent->rotate();
                else this->rotate();
                this->rotate();
            }
            this->update();// while が呼ばれなかった場合のため
            return;
        }

 
        // サイズなど、Nodeの持っている情報をupdateする(順番大事)
        void update(){
            this->SubTreeSize = 1;
            this->length_sum = abs(this->R - this->L);
            if(bool(this->left)){
                this->SubTreeSize += this->left->SubTreeSize;
                this->length_sum += this->left->length_sum;
            }
            if(bool(this->right)){
                this->SubTreeSize += this->right->SubTreeSize;
                this->length_sum += this->right->length_sum;
            }
            return;
        }
 
    };
 
 
    // この木のroot,splitした後はmergeして戻す！！rootが迷子にならないように！！
    SplayNode *Root = nullptr;

    // <= であることが必須
    bool CompareNode(SplayNode *a , SplayNode *b ){return a->R <= b->R;}
 

    // rootを根とする SplayTree において、求めたいindexまで降りていく(左側の部分木サイズを参照する)
    SplayNode *getNode(int index , SplayNode *root){
        if(root==nullptr)return root;
        SplayNode *now = root;
        while(true){
            int left_size = 0;
            if(now->left != nullptr)left_size = now->left->SubTreeSize;
            if(index < left_size)now = now->left;
            else if(index > left_size){
                now = now->right;
                index -= left_size+1;
            }else break;
        }
        now->splay();
        return now;
    }
 
    /*
        merge と split が反対の操作であることで、計算量を削減できる
        merge(lef,rig) : rig の左の子に lef をつける
        split(i,rt) : i 番目のノードの左の辺を無くし、(lef,rig) のペアを返す
    */
    // それぞれ、leftrootとrightrootが根であるような部分木をくっつけた木の根を返す
    // それぞれは update() されている必要がある
    SplayNode *merge(SplayNode *leftRoot , SplayNode *rightRoot){
        //怖いのでここで update かけときます。大したコストではないです
        if(leftRoot!=nullptr)leftRoot->update();
        if(rightRoot!=nullptr)rightRoot->update();
        if(bool(leftRoot ) == false)return rightRoot;//右のみあり
        if(bool(rightRoot) == false )return leftRoot;//左のみあり
        rightRoot = getNode(0,rightRoot);
        rightRoot->left = leftRoot;
        leftRoot->parent = rightRoot;
        rightRoot->update();
        return rightRoot;
    }
    
 
    // rootを根とする部分木において、
    // 左からleftnum番目の頂点で左右にsplit。
    // splitした後はmergeしないと,splitした木の頂点が行方不明になりそう
    std::pair<SplayNode*,SplayNode*> split(int leftnum, SplayNode *root){
        if(leftnum<=0)return std::make_pair(nullptr , root);
        if(leftnum >= root->SubTreeSize)return std::make_pair(root, nullptr);
        root = getNode(leftnum , root);
        SplayNode *leftRoot = root->left;
        SplayNode *rightRoot = root;
        if(bool(rightRoot))rightRoot->left = nullptr;
        if(bool(leftRoot))leftRoot->parent = nullptr;
        leftRoot->update();
        rightRoot->update();
        return std::make_pair(leftRoot,rightRoot);
    }
 
    /*
        rootを根とする部分木において、
        左からindexの場所を削除して、両端をくっつける
        firstは消したやつ以外をmergeした時のroot
        secondは消したSplayNode
    */
    std::pair<SplayNode*,SplayNode*> Delete_sub(int index , SplayNode *root){
        if(bool(root) == false)return std::make_pair(root,root);
        root = getNode(index,root);
        SplayNode *leftRoot = root->left;
        SplayNode *rightRoot = root->right;
        if(bool(leftRoot))leftRoot->parent = nullptr;
        if(bool(rightRoot))rightRoot->parent = nullptr;
        root->left = nullptr;
        root->right = nullptr;
        root->update();
        return std::make_pair(merge(leftRoot,rightRoot) , root );
    }

 
    // rootを根とする部分木の[l,r)の区間取得クエリ
    // 木の根と、[l,r) に対応する部分木の根のコピーを返す
    std::pair<SplayNode*,SplayNode> RangeNode_sub(int l , int r , SplayNode *root){
        SplayNode *leftRoot , *centerRoot , *rightRoot;
        std::pair<SplayNode*,SplayNode*> tmp = split(r,root);
        rightRoot = tmp.second;
        tmp = split(l,tmp.first);
        SplayNode res = *tmp.second;
        return std::make_pair(merge(merge(tmp.first,tmp.second),rightRoot),res);
    }

   
    /*
        rootが根であるような部分木で、以下の条件を満たすノード x の個数を返す (比較 <= は ComparNode の意味)
        lower = true : x < node (lower_bound)
        lower = false: x <= node (upper_bound)
        条件を満たすギリギリの境界を探索する
    */
    std::pair<SplayNode*,int> bound_sub(SplayNode* Node , SplayNode *root , bool lower){
        if(bool(root) == false)return std::make_pair(root,0);
        SplayNode *now = root;// 条件による境界ギリギリのノードを根から探索する
        int res = 0;// 答え
        while(true){
            bool satisfy = CompareNode(now,Node); // upper_bound (now <= Node)
            if(lower)satisfy = !CompareNode(Node,now); // lower_bound (now < Node)
            if(satisfy){// now が条件を満たす限り、右に降りる
                if(bool(now->right))now = now->right;
                else {
                    res++;// now が条件を満たす側の境界で終了
                    break;
                }
            }else{// now が条件を満たさない限り、左に降りる
                if(bool(now->left))now = now->left;
                else break;// now が条件を満たさない側の境界で終了
            }
        }
        now->splay();
        if(bool(now->left))res += now->left->SubTreeSize;
        return std::make_pair(now ,res);
    }
    

    // root が根の splaytree に NODE (別の連結成分)を挿入する
    // NODE は update されている必要がある
    SplayNode *insert_sub(SplayNode *NODE , SplayNode *root){
        NODE->update();// 集約と遅延評価をしておく
        if(bool(root) == false)return NODE;
        // lower_bound で、x < NODE の境界となる x を取得 (ただし境界の右側か左側かは不定)
        root = bound_sub(NODE,root,true).first;
        // root が x < NODE の境界の左右どちらかによって、NODE を配置する場所が変わる
        if(!CompareNode(NODE , root )){// root < NODE の場合は root の右に
            if(root->right != nullptr)root->right->parent = NODE;
            NODE->right = root->right;
            root->right = nullptr;
            NODE->left = root;
        }else{// NODE <= root の場合は root の左に
            if(root->left != nullptr)root->left->parent = NODE;
            NODE->left = root->left;
            root->left = nullptr;
            NODE->right = root;
        }
        root->parent = NODE;
        root->update();
        NODE->update();
        return NODE;// NODE が根になっている
    }
    
    /*
        --------------------------------------------------------------
        ここからは、クラスの外で動かす関数たちです。
        --------------------------------------------------------------
    */

    public:
    RangeSet(){}
 
    int size(){
        if(Root==nullptr)return 0;
        return Root->SubTreeSize;
    }
    
    // 木全体のindex番目のNode(値渡し)を取得
    SplayNode get(int index){
        if(index<0 || index>=size())assert(0);
        Root = getNode(index,Root);
        return *(Root);
    }

    // index番目のNodeを消去
    void Delete(int index){
        if(index<0 || index >= size())assert(0);
        std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(index,Root);
        Root = tmp.first;
        return;
    }
    
    // R が x 未満の頂点の個数を返す
    int lower_bound(T x){
        if(size() == 0)return 0;
        SplayNode* nd = new SplayNode(x,x);
        std::pair<SplayNode*,int> tmp = bound_sub(nd,Root,true);
        Root = tmp.first;
        delete nd;
        return tmp.second;
    }
    
    // Rがx以下の頂点の個数を返す
    int upper_bound(T x){
        if(size() == 0)return 0;
        SplayNode* nd = new SplayNode(x,x);
        std::pair<SplayNode*,int> tmp = bound_sub(nd,Root,false);
        Root = tmp.first;
        delete nd;
        return tmp.second;
    }

    // 小さい方からl番目以上、r番目未満の +++頂点が持つ区間+++ たちの長さの和
    T RangeLengthSumQuery(int l , int r){
        l = max(0,l);
        r = min(size(),r);
        if(l>=r)assert(0);
        std::pair<SplayNode*,SplayNode> tmp = RangeNode_sub(l,r,Root);
        Root = tmp.first;
        return tmp.second.length_sum;
    }
    
    // 二つの区間 [ l1 , r1 ) と [ l2 , r2 ) が交差するか
    bool intersect(T l1 , T r1 , T l2 , T r2){return (l1 <= l2 && l2 < r1) || (l2 <= l1 && l1 < r2);}
    
    // 区間 [ l , r ) と交差する区間を +++ 一つ +++ 見つける(indexで返す)
    int find_cross(T l , T r){
        if(l > r)assert(0);
        if(l == r)assert(0); // 未定義動作
        int it_ = upper_bound(l);// 右端が l よりも真に大きい区間
        if(it_ >= 0 && it_ < size()){
            T lef = get(it_).L;
            T rig = get(it_).R;
            if(intersect(l,r,lef,rig))return it_;// 被った区間のindexを返す   
        }
        return -1;// なければ-1を返す
    }
    
 
    // 区間 [ l , r )を追加
    void insert( T lef , T rig){
        if(lef > rig)assert(0);
        if(lef == rig)return;
        SplayNode x;
        while(true){// 被っている区間を全てsetから取り出して、[ l ,r ) と一つにmergeしてからsetに入れ直す
            int it_ = find_cross(lef,rig);
            if(it_ < 0 || it_ >= size())break;
            x = get(it_);
            // 区間をマージ
            lef = min(lef,x.L);
            rig = max(rig,x.R);
            Delete(it_);// 重複する区間を削除
        }
        // 隣接している区間をマージ
        int it_ = upper_bound(lef);
        // ループ内でDeleteするので、大きいindexから見る
        for(int itr = it_ ; itr >= it_ - 1 ; itr-- ){
            if(itr >= 0 && itr < size()){
                x = get(itr);
                // 以下のif条件はどちらも独立
                if(x.R == lef){
                    lef = min(lef,x.L);
                    Delete(itr);
                }else if(x.L == rig){
                    rig = max(rig,x.R);
                    Delete(itr);
                }
            }
        }
        Root = insert_sub(new SplayNode(lef , rig) ,Root);
        return;
    }
 
    
    // 区間 [ l , r ) を削除
    void erase(T lef , T rig){
        if(lef > rig)assert(0);
        if(lef == rig)return;
        SplayNode x;
        while(true){//被っている区間を見つけて、被っている部分を消していく
            int it_ = find_cross(lef,rig);
            if(it_ < 0 || it_ >= size())break;
            x = get(it_);
            Delete(it_);
            if(lef <= x.L && x.R <= rig)continue;//すっぽりと削除区間に含まれる場合
            else if(x.L <= lef && rig <= x.R){//削除区間がすっぽりと含まれている場合(穴が空いたような状態に)
                insert(x.L,lef);
                insert(rig,x.R);
            }else if(lef <= x.L && x.L < rig)insert(rig,x.R);//左側が削除区間に含まれている場合    
            else if(lef < x.R && x.R <= rig)insert(x.L,lef);//右側が削除区間に含まれている場合
        }
        return;
    }
    
    // setに入っている区間で、区間 [ l , r ) と被っている部分の長さの総和
    // ただし、区間 [ l , r ) の長さは r-l であるとする
    T cover_length(T lef , T rig){
        if(lef > rig)assert(0);
        if(lef == rig)return 0;
        int it_lef = upper_bound(lef);
        if(it_lef == size())return 0;
        int it_rig = upper_bound(rig);
        T res = 0;//答え
        SplayNode x = get(it_lef);
        if(x.L <= lef && rig <= x.R)return rig-lef;
        if(x.L<lef){
            it_lef++;
            res += x.R - lef;
        }
        if(it_lef < it_rig)res += RangeLengthSumQuery(it_lef,it_rig);
        if(it_rig < size()){
            x = get(it_rig);
            if(x.L < rig)res += rig-x.L;
        }
        return res;
    }
    //点 x をカバーする区間のindex(存在しなければ-1を返す)
    int find_included(T x){
        int it_ = upper_bound(x);
        if(it_ < 0 || it_ == size())return -1;
        else{
            if(get(it_).L <= x)return it_;
            else return -1;
        }
        return -1;
    }
    void Debug(){
        cerr << "DEBUG" << endl;
        for(int i = 0 ; i < size();i++)cerr << get(i).L << " ";cerr << endl;
        for(int i = 0 ; i < size();i++)cerr << get(i).R << " ";cerr << endl;
    }
    
    // read-only でアクセス
    SplayNode operator [](int index){return get(index);}
         
 
};




