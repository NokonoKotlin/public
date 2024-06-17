#include "./../template/template.hpp"








/*
    - 辺を切ったり貼ったりできる森、accessがとっても大事！！忘れないでね！！
    - 木を明示的に管理するのではなく、木上ののパスを SplayTree (列型) として管理する。
    |
    - evert(x) := xを連結成分のrootに
    - link(x,y) := x,y に辺を追加 (元々 x,y は非連結)
    - cut(x,y) := x,y から辺を削除 (元々 x,y に辺がある)
    |
    - access(x) := x の属する木の根から x までを HeavyEdge で繋ぎ、パスにする。
        |- パスは SplayTree である。
        |- ついでに m_Nodes[x] を SplayTree の根にする (splay)
    - access(x)で繋がった SplayTreeは、左の方が根に近く、右に行くほど下の頂点になる (path である)
    - 根から x までの path の情報 (Value の Sum など) は、access(x) してから m_Nodes[x]->Sum などで取得できる
    - operator [x] := 頂点 x に対応する SplayNode のコピー (隣接 SplayNode へのポインタを封印してある)
        |- [x] にアクセスすると自動で access(x) される。
        |- つまり、[x] は 「根から x までのパスを表す SplayTree の根」(のコピー) を取得する

    - root(x) := x の属する木の根
    - depth(x) := x の深さ
    - same(x,y) := x,y が同じ木に属しているか
    - parent(x) := ( x の親が存在するか, 親の番号) のペア
    - LCA(x,y) := x,y の lca (x,y は同じ木に属する)
    - path_xth_element(v,x) := v の属する木の「根から頂点 v までのパス」のうち 深さが x の頂点番号
    |
    - PathUpdate(i,x) := 頂点 i が属する木の根から頂点 i までのパス上の頂点の値を一律 x に更新する
    - PathAffine(i,A,B) := 頂点 i が属する木の根から頂点 i までのパス上の頂点の値 (Value) を一律 A*Value + B に更新
    - PathUpdate(i,x) := 頂点 i が属する木の根から頂点 i までのパス上の頂点の値に一律 x 加算する
    |
    - 辺の表現には頂点拡張をすると良い

    頂点の持つ値 (Value) は T 型。    
    遅ければいらない遅延評価やモノイド積を消す。
*/
template<typename T>
class LinkCutTree{
    private:
    
    struct SplayNode{
        SplayNode *parent = nullptr;//親ノード
        SplayNode *left = nullptr;//左の子ノード
        SplayNode *right = nullptr;//右の子ノード
        T Value;// 元の木の頂点についている値
        T Min,Max,Sum;// SplayTree の部分木のうち、Value の最大、最小、和
        long long id = -1;//頂点番号
        long long SubTreeSize = 1;// SplayTree の部分木サイズ

        bool reverse_flag_lazy = false;
        void set_lazyReverse(){this->reverse_flag_lazy = !(this->reverse_flag_lazy);}
        
        // 遅延中の区間アフィン変換クエリ(遅延しているか , {A,B})
        pair<bool,pair<T,T> > lazy_affine ={false , {T(),T()}};
        void set_lazyAffine(T a, T b){
            if(this->lazy_affine.first)this->lazy_affine.second = { a*this->lazy_affine.second.first , a*this->lazy_affine.second.second + b};
            else this->lazy_affine = {true , {a,b}};
        }

        // 遅延中の区間代入クエリ(遅延しているか , 値)
        pair<bool,T> lazy_update = {false,T()};
        void set_lazyUpdate(T x){
            this->lazy_update.first = true;
            this->lazy_update.second=x;
            this->lazy_affine.first = false;//これ以前の affine クエリは無効化
        }

        SplayNode(long long id_ ,T val){
            id = id_;
            Value = val;
            Min = val;
            Max = val;
            Sum = val;
            update();
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
            自分から見た親の位置
            0 -> 親が存在しない or light edge の場合
            1 -> 親の左の場合
            2 -> 親の右の場合
        */
        int state(){
            if(this->parent == nullptr)return 0;
            this->parent->eval();// reverse を反映
            if(this->parent->left == this)return 1;
            else if(this->parent->right == this)return 2;
            return 0;// light edge の場合
        }

        // あるNodeを回転を駆使し一番上まで持っていく
        void splay(){
            while(this->state()!=0){
                if(this->parent->state() == 0){
                    this->rotate();
                    break;
                }
                if( this->parent->state() == this->state() )this->parent->rotate();
                else this->rotate();
                this->rotate();
            }
            this->update();
            return;
        }

        // サイズなど、Nodeの持っている情報をupdateする(順番大事)
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
                if(bool(this->left))this->left->set_lazyReverse();//下に伝播
                if(bool(this->right))this->right->set_lazyReverse();//下に伝播
                this->reverse_flag_lazy = false;
            }
            //updateクエリの処理が先
            if(this->lazy_update.first){
                this->Value = this->Min = this->Max = this->lazy_update.second;
                this->Sum = (this->lazy_update.second)*(this->SubTreeSize);
                if(bool(this->left))this->left->set_lazyUpdate(this->lazy_update.second);
                if(bool(this->right))this->right->set_lazyUpdate(this->lazy_update.second);
                this->lazy_update.first = false; //伝播したらフラグを折る
            }
            if(this->lazy_affine.first){
                this->Value = this->lazy_affine.second.first * this->Value + this->lazy_affine.second.second;
                this->Min = this->lazy_affine.second.first * this->Min + this->lazy_affine.second.second;
                this->Max = this->lazy_affine.second.first * this->Max + this->lazy_affine.second.second;
                this->Sum = this->lazy_affine.second.first * this->Sum + this->SubTreeSize*this->lazy_affine.second.second;
                if(bool(this->left))this->left->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                if(bool(this->right))this->right->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                this->lazy_affine.first = false;//伝播したらフラグを折る
            }
        }

        // 前の隣接ノード
        // 副作用 : this と前のノードを splay する
        SplayNode* _before(){
            SplayNode* res = this;
            res->splay();
            res->eval();
            if(res->left == nullptr)return nullptr;
            res = res->left;
            res->eval();
            while(res->right != nullptr){
                res = res->right;
                res->eval();
            }
            res->splay();
            return res;
        }
    };

    private:
                   
    //m_Nodes[i] := 頂点 i のポインタ
    unordered_map<long long,SplayNode*> m_Nodes;
    // [u][v] := 辺 u-v が存在するか
    unordered_map<long long , unordered_map<long long,bool>> ExistEdge;

    // 頂点の持つ値の初期値
    T init_v;
    // 頂点集合を明示的に持たないので、使う前に 1 度宣言する
    bool register_node(long long u){
        if(m_Nodes[u] != nullptr)return false;
        m_Nodes[u] = new SplayNode(u,init_v);
        return true;
    }
    
    // rootを根とする SplayTree において、求めたいindexまで降りていく(左側の部分木サイズを参照する)
    SplayNode* get_sub(long long  index , SplayNode* root){
        if(root==nullptr)return root;
        SplayNode* now = root;
        while(true){
            now->eval();// ノードを見る前にeval
            long long  left_size = 0;
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


    void init(){m_Nodes.clear();}
    void release(){
        for(pair<long long,SplayNode*>p : m_Nodes){
            if(p.second != nullptr)delete p.second;
        }
    }

    public:// クラス外で使う用
    
    LinkCutTree():init_v(T()){init();}
    LinkCutTree(T init_v_):init_v(init_v_){init();}// 初期化値指定つき
    // 複雑な挙動を回避するので、コンストラクタによるコピー/ムーブを一律に禁止する。
    LinkCutTree(const LinkCutTree<T> &x) = delete ;
    LinkCutTree<T>& operator = (const LinkCutTree<T> &x) = delete ;
    LinkCutTree (LinkCutTree<T>&& x){assert(0);}
    LinkCutTree<T>& operator = ( LinkCutTree<T>&& x){assert(0);}

    // 頂点 : idにアクセス ( root から id まで Heavy edge でつなげる) 
    // 最後にlight edgeを辿った先の頂点番号を返す(LCA用)
    long long access(const long long id){
        register_node(id);
        if(bool(m_Nodes[id]) == false)return -1;//存在しない場合
        long long res = id;
        while(1){
            m_Nodes[id]->splay();// splay は eval を兼ねる
            m_Nodes[id]->right = nullptr;
            m_Nodes[id]->update();
            if(bool(m_Nodes[id]->parent) == false)break;
            res = m_Nodes[id]->parent->id;
            m_Nodes[id]->parent->splay();
            m_Nodes[id]->parent->right = m_Nodes[id];
            m_Nodes[id]->parent->update();
        }
        return res;
    }
    // v の属する木の「根から頂点 v までのパス」のうち 深さが x の頂点番号
    // 副作用 : access(v) する
    long long path_xth_element(long long v , int x){
        register_node(v);
        assert(0 <= x && x <= depth(v));
        access(v);
        SplayNode* nd = get_sub(x , m_Nodes[v]);
        access(v); // 副作用を統一
        return nd->id;
    }

    // 頂点 x が属する部分木(連結成分)のrootを求める(エラー時は-1を返す)
    // 副作用 : x が属する部分木の root に access する
    long long root(long long x){
        register_node(x);
        return path_xth_element(x,0);
    }
    // 頂点 u,v が同じ木に属するか
    // 副作用 : u,v を未定義の順で access する
    bool same(long long u , long long v){ 
        register_node(u);
        register_node(v);
        return bool(root(u) == root(v));
    }
    
    // 頂点 x が属する木における頂点 x の深さ。
    // 副作用 : x にアクセス
    long long depth(long long x){
        register_node(x);
        access(x);
        return m_Nodes[x]->SubTreeSize - 1;
    }
    // ( 親が存在するか , 親の番号 ) のペアを返す
    // 副作用 : 親が存在するなら、その親にaccessする
    pair<bool,long long> parent(long long x){
        register_node(x);
        pair<bool,long long> res;
        access(x);
        SplayNode* p = m_Nodes[x]->_before();
        if(p == nullptr)res.first = false;
        else {
            res.first = true;
            res.second = p->id;
        }
        return res;
    }

    // 頂点 x を、属する木の root にする。
    // つまり、頂点 x が属する SplayTree の向きを反転する
    // 副作用 : x に access
    void evert(long long x){
        register_node(x);
        access(x);
        m_Nodes[x]->set_lazyReverse();// 反転
        m_Nodes[x]->update();
        access(x);
    }

    // u , v をつなげる。(u,v は非連結である必要がある)
    // 副作用 : u に access する
    void link(long long u , long long v){
        register_node(u);
        register_node(v);
        assert(!same(u,v));
        evert(v);
        access(v);
        access(u);
        ExistEdge[u][v] = true;
        ExistEdge[v][u] = true;
        m_Nodes[v]->parent = m_Nodes[u];// v → u に light edge を追加
        m_Nodes[u]->update();
    }

    // 辺 u-v をカット
    // 副作用 : access の状態が不定になる
    bool cut(long long u , long long v){
        register_node(u);
        register_node(v);
        assert(ExistEdge[u][v] && ExistEdge[v][u]);
        ExistEdge[u][v] = false;
        ExistEdge[v][u] = false;
        if(depth(u) > depth(v))swap(u,v);// v が深い方とする
        access(v);
        // v が SplayTree の根なので、左の部分木を切れば v より上の頂点と切り離せる
        m_Nodes[v]->left->parent = nullptr;
        m_Nodes[v]->left = nullptr;
        m_Nodes[v]->update();
        return true;
    }

    // 頂点 i のもつ値(value)を x に変更する
    // 副作用 : access(i)
    void update_val(long long i , T x){
        register_node(i);
        access(i);
        m_Nodes[i]->Value = x;
        m_Nodes[i]->update();
    }

    // 頂点 i が属する木の根から頂点 i までのパス上の頂点の値を一律 x に更新する
    // 副作用 : 頂点 i に access
    void PathUpdate(long long i , T x){
        register_node(i);
        access(i);
        m_Nodes[i]->set_lazyUpdate(x);
        m_Nodes[i]->update();
        access(i);
    }

    // 頂点 i が属する木の根から頂点 i までのパス上の頂点の値 (Value) を一律 A*Value + B に更新する
    // 副作用 : 頂点 i に access
    void PathAffine(long long i , T A , T B){
        register_node(i);
        access(i);
        m_Nodes[i]->set_lazyAffine(A,B);
        m_Nodes[i]->update();
        access(i);
    }

    // 頂点 i が属する木の根から頂点 i までのパス上の頂点の値に一律 x 加算する
    // 副作用 : 頂点 i に access
    void PathAdd(long long i , T x){PathAffine(i,T(1),x);}

    
    // 同じ木に属する頂点 x と y のLCAを求める。
    // 副作用 : access(lca) する
    long long LCA(long long x , long long y){
        register_node(x);
        register_node(y);
        assert(same(x,y));
        if(root(x) != root(y))return -1;
        else {
            access(x);//最後にlightedgeを辿って到達した、先を返すので、ちょうど分岐点を返すようになっている
            long long lca = access(y);
            access(lca);
            return lca;
        }
    }
    // read-only で ノード のコピーを取得 ( 副作用で ノードに access (重要) )
    SplayNode operator [](long long nodeID){
        access(nodeID);
        SplayNode res = *(m_Nodes[nodeID]);
        res.parent = res.left = res.right = nullptr;// read-only なので隣接頂点へのアクセスを封印
        return res;
    }
};




