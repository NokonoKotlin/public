#include "./../template/template.hpp"












/*
    - SplayTree で UnionFind を実装。ノードに対応する SplayNode* を用意する。
    |
    - unite(u,v) するときは  u,v それぞれが属する SplayTree を merge する。
    |
    - x が属する連結成分の代表頂点(UF でいうところの root) は、x が属する SplayTree で最も左の頂点
        |- left[x] := x よりも左にあるノード。ただし、どれかはわからない。
        |- left[x] を経路圧縮することで、目的の leftmost(x) を得られる。
        |- これは splay を減らす工夫で、UnionFind の経路圧縮と同じ。
        |- rollback が必要な場合は経路圧縮せずに x から可能な限り上にのぼり、可能な限り左に降りて logN で計算
            |- rollback があるとまあ少し遅い
    |
    - UF と同じ機能だけを使いたい場合は unite&rollback 以外は splay する必要がなく速い。
    |
    - 注意点
        - UF 木をマージするデータ構造であり、辺をつなげると捉えるとバグる。  
        - unite(u,v) で u , v の属する UF 木が同じ場合はそもそも unite を呼び出さず、rollback にもストックされない。  
    |
    - 実装/設計 について
        |- 頂点が持つ値を使用する場合は、通常の splay 木と同程度と考えれば OK
        |- splay する場面としない場面があるので、編集する際は注意
*/
template<class T , bool _rollback_ = false>
class SplayUnionFind{
    struct SplayNode{
        SplayNode *parent = nullptr;SplayNode *left = nullptr;SplayNode *right = nullptr;
        T Value;// 頂点の値
        T Min,Max,Sum;// モノイド積
        int Nodeid; // 頂点の id
        int SubTreeSize = 1;

        // 遅延評価
        pair<bool,pair<T,T> > lazy_affine ={false , {T(),T()}};
        void set_lazyAffine(T& a, T& b){// アフィン変換
            if(this->lazy_affine.first)this->lazy_affine.second = { a*this->lazy_affine.second.first , a*this->lazy_affine.second.second + b};
            else this->lazy_affine = {true , {a,b}};
        }
        pair<bool,T> lazy_update = {false,T()};
        void set_lazyUpdate(T &x){// 値の更新
            this->lazy_update.first = true;
            this->lazy_update.second=x;
            this->lazy_affine.first = false;
        }
        SplayNode(){}
        SplayNode(T val){Value = val;update();}
        void update(){
            this->eval();
            this->Sum = this->Max = this->Min = this->Value;
            this->SubTreeSize = 1;
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
            if(this->lazy_update.first){
                this->Value = this->Min = this->Max = this->lazy_update.second;
                this->Sum = (this->lazy_update.second)*(this->SubTreeSize);
                if(bool(this->left))this->left->set_lazyUpdate(this->lazy_update.second);
                if(bool(this->right))this->right->set_lazyUpdate(this->lazy_update.second);
                this->lazy_update.first = false; 
            }
            if(this->lazy_affine.first){
                this->Value = this->lazy_affine.second.first * this->Value + this->lazy_affine.second.second;
                this->Min = this->lazy_affine.second.first * this->Min + this->lazy_affine.second.second;
                this->Max = this->lazy_affine.second.first * this->Max + this->lazy_affine.second.second;
                this->Sum = this->lazy_affine.second.first * this->Sum + this->SubTreeSize*this->lazy_affine.second.second;
                if(bool(this->left))this->left->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                if(bool(this->right))this->right->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                this->lazy_affine.first = false;
            }
        }

        // splay木の内部実装なので、基本いじる必要なし
        void rotate(){
            if(this->parent->parent){if(this->parent == this->parent->parent->left)this->parent->parent->left = this;else this->parent->parent->right = this; }
            this->parent->eval();this->eval();
            if(this->parent->left == this){
                this->parent->left = this->right;if(this->right)this->right->parent = this->parent;
                this->right = this->parent;this->parent = this->right->parent;
                this->right->parent = this;this->right->update();
            }else{
                this->parent->right = this->left;if(this->left)this->left->parent = this->parent;
                this->left = this->parent;this->parent = this->left->parent;
                this->left->parent = this;this->left->update();
            }this->update();
        }
        int state(){if(this->parent == nullptr)return 0;if(this->parent->left == this)return 1;return 2;}
        void splay(){
            while(bool(this->parent)){if(this->parent->state() == 0){this->rotate();break;}
                if( this->parent->state() == this->state() )this->parent->rotate();else this->rotate();this->rotate();
            }this->update();
        }
    };

    /*
        内部実装
    */
    // unite を実行したときにマージする右側の SplayTree の leftmost の履歴を新しいものから管理
    // ただし、unite(u,v) を実行するとは、異なる連結成分に属する u,v に対して unite(u,v) することを指す
    stack<int> history;

    // [x] := 頂点 x 対応するノードクラスのポインタを保管
    // UF Tree の代わりに SplayTree で管理
    vector<SplayNode*> SplayUFNode;
    // UF における parent みたいな役割
    vector<int> left; // [x] := x が属する SplayTree で、より左の頂点 (非決定) の番号

    // x が属する連結成分の代表
    // UF の経路圧縮のみの計算量と同じ。
    // x の親の方へ可能な限り移動 → 可能な限り左に降りるでも実現可能 : O(logN)
    int leftmost(int x){
        if(_rollback_){
            // 木を移動する場合はちゃんと splay した方がいい。
            SplayNode* now = SplayUFNode[x];
            now->splay();
            while(now->left != nullptr)now = now->left;
            now->splay();
            return now->Nodeid;
        }
        // rollback しない時は経路圧縮で求める
        if(left[x] == x)return x;// left[x] == x は連結成分の最も左のみであることが実装で保証される
        return left[x] = leftmost(left[x]);// 経路圧縮
    }

    public:
    SplayUnionFind(int N , T init_){
        vector<SplayNode*>(N).swap(SplayUFNode);
        vector<int>(N).swap(left);
        for(int i = 0 ; i < N ; i++){
            SplayUFNode[i] = new SplayNode(init_);
            left[i] = i;
            SplayUFNode[i]->Nodeid = i;
        }
    }

    // leftmost と同じだけどネーミング大事だし
    // 頂点 x が属する連結成分を代表する頂点
    int root(int x){return leftmost(x);}
    // u,v が同じ連結成分か
    bool same(int u , int v){return bool(root(u) == root(v));}
    // u,v を unite。すでに同じ連結成分なら何もせずに false 返す。
    bool unite(int u , int v){
        if(same(u,v))return false;
        u = leftmost(u);
        v = leftmost(v);
        SplayUFNode[u]->splay();
        SplayNode* rig = SplayUFNode[v];
        rig->splay();
        if(_rollback_)history.push(rig->Nodeid);
        SplayUFNode[u]->parent = rig;
        rig->left = SplayUFNode[u];
        rig->update();
        left[rig->Nodeid] = leftmost(u);
        left[v] = leftmost(u);
        
        return true;
    }
    // 直近の unite の実行を取り消す。
    bool rollback(){
        assert(_rollback_);
        if(history.empty())return false;
        int last = history.top();history.pop();
        SplayUFNode[last]->splay();
        SplayUFNode[last]->left->parent = nullptr;
        SplayUFNode[last]->left = nullptr;
        SplayUFNode[last]->update();
        return true;
    }

    // x が属する連結成分内の頂点を列挙 : splay tree を dfs
    vector<int> member(int x){
        vector<int> res;
        SplayUFNode[x]->splay();
        stack<SplayNode*> st;
        st.push(SplayUFNode[x]);
        while(!st.empty()){
            SplayNode* now = st.top();st.pop();
            now->eval();
            res.push_back(now->Nodeid);
            if(now->left != nullptr)st.push(now->left);
            if(now->right != nullptr)st.push(now->right);
        }
        return res;
    }
    // x が属する連結成分のサイズ
    int size(int x){SplayUFNode[x]->splay();return SplayUFNode[x]->SubTreeSize;}

    /*
        頂点が持つ値関連
    */
    // 頂点 x が持つ値
    T value(int x){SplayUFNode[x]->splay();return SplayUFNode[x]->Value;}
    // 頂点 x の値を y にする
    void set(int x , T y){SplayUFNode[x]->splay();SplayUFNode[x]->Value = y;SplayUFNode[x]->update();}
    // x が属する連結成分の要素の総和
    T sum(int x){SplayUFNode[x]->splay();return SplayUFNode[x]->Sum;}
    // x が属する連結成分の要素の min
    T min(int x){SplayUFNode[x]->splay();return SplayUFNode[x]->Min;}
    // x が属する連結成分の要素の max
    T max(int x){SplayUFNode[x]->splay();return SplayUFNode[x]->Max;}
    // x が属する連結成分のノードの値を全て x に変更
    void AllUpdate(int x , T y){SplayUFNode[x]->splay();SplayUFNode[x]->set_lazyUpdate(y);}
    // x が属する連結成分のノードの値(t とする)を全て、a*t + b にする
    void AllAffine(int x , T a , T b){SplayUFNode[x]->splay();SplayUFNode[x]->set_lazyAffine(a,b);}
    // x が属する連結成分のノードの値全てに y 加算する
    void AllAdd(int x , T y){AllAffine(x,1,y);}
};








