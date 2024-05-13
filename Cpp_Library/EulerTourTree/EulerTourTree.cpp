#include "./../template/template.hpp"










/*

*/
template<typename node_type , typename T>
class EulerTourTree{
    private:
    
    struct SplayNode{

        /* SplayNode としてのメンバ */
        SplayNode *parent = nullptr;//親ノード
        SplayNode *left = nullptr;//左の子ノード
        SplayNode *right = nullptr;//右の子ノード
        T Value;//値(普通、頂点には値や重みが割り当てられることが多い)
        T Min,Max,Sum;//部分木のうち、値の最大、最小、和
        long long  SubTreeSize = 1;

        /* Euler Tour の要素 */
        node_type from , to;// オイラーツアーの移動方向の両端点
        bool isNodeFactor = false;// NodeFactor かどうか (NodeFactor のみ Value を持つ)
        long long SubTreeNodeFactorCount = 0;// SplayTree としての部分木に NodeFactor がいくつ含まれているか


        // 遅延中の区間アフィン変換クエリ(遅延しているか , {A,B})
        pair<bool,pair<T,T> > lazy_affine ={false , {T(),T()}};
        void set_lazyAffine(T& a, T& b){
            if(this->lazy_affine.first)this->lazy_affine.second = { a*this->lazy_affine.second.first , a*this->lazy_affine.second.second + b};
            else this->lazy_affine = {true , {a,b}};
        }

        // 遅延中の区間代入クエリ(遅延しているか , 値)
        pair<bool,T> lazy_update = {false,T()};
        void set_lazyUpdate(T &x){
            this->lazy_update.first = true;
            this->lazy_update.second=x;
            this->lazy_affine.first = false;//これ以前の affine クエリは無効化
        }
        
        SplayNode(){}
        // EdgeFactor としての SplayNode
        SplayNode(node_type u_, node_type v_)
            :from(u_),to(v_),isNodeFactor(false){update();}
        // NodeFactor としての SplayNode
        SplayNode(node_type u_, node_type v_, T val)
            :from(u_),to(v_),isNodeFactor(true),Value(val){update();}


        // this - this->parent を回転させる (親は必ず存在するものとし、位置関係も誤りがないものとする)
        void rotate(){
            if(this->parent->parent){//GrandParentの子を張り替え
                if(this->parent == this->parent->parent->left)this->parent->parent->left = this;
                else this->parent->parent->right = this; 
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
            0 -> 親が存在しない
            1 -> 親の左の場合
            2 -> 親の右の場合
        */
        int state(){
            if(this->parent == nullptr)return 0;
            this->parent->eval();// reverse を反映
            if(this->parent->left == this)return 1;
            else if(this->parent->right == this)return 2;
            return 0;
        }
        // あるNodeを回転を駆使し一番上まで持っていく
        void splay(){
            while(bool(this->parent)){
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

        // Nodeの持っている情報をupdateする
        void update(){
            this->eval();
            this->SubTreeSize = 1;
            this->SubTreeNodeFactorCount = 0;
            if(this->isNodeFactor){
                this->Sum = this->Value;
                this->Min = this->Value;
                this->Max = this->Value;
                this->SubTreeNodeFactorCount++;
            }else{
                // NodeFactor でないなら単位元を入れておく
                this->Sum = 0;
                // 自作クラスなら自分で用意する
                this->Min = numeric_limits<T>::max();
                this->Max = numeric_limits<T>::min();
            }
            if(bool(this->left)){
                this->left->eval();
                this->SubTreeSize += this->left->SubTreeSize;
                this->SubTreeNodeFactorCount += this->left->SubTreeNodeFactorCount;
                if(this->left->Min < this->Min)this->Min = this->left->Min;
                if(this->left->Max > this->Max)this->Max = this->left->Max;
                this->Sum += this->left->Sum;
            }
            if(bool(this->right)){
                this->right->eval();
                this->SubTreeSize += this->right->SubTreeSize;
                this->SubTreeNodeFactorCount += this->right->SubTreeNodeFactorCount;
                if(this->right->Min < this->Min)this->Min = this->right->Min;
                if(this->right->Max > this->Max)this->Max = this->right->Max;
                this->Sum += this->right->Sum;
            } 
            return;
        }
        /*この頂点にアクセスされるたびに、各flagがtrueなら遅延させていた処理を実行し、左右の子にflagを伝播する*/
        void eval(){
            //updateクエリの処理が先
            if(this->lazy_update.first){
                this->Value = this->lazy_update.second;// NodeFactor じゃなくても、どうせアクセスしないので代入して OK 
                this->Min   = this->lazy_update.second;
                this->Max   = this->lazy_update.second;
                this->Sum   = (this->lazy_update.second)*(this->SubTreeNodeFactorCount);// 部分木の NodeFactor の Value の総和
                if(bool(this->left))this->left->set_lazyUpdate(this->lazy_update.second);
                if(bool(this->right))this->right->set_lazyUpdate(this->lazy_update.second);
                this->lazy_update.first = false; //伝播したらフラグを折る
            }
            if(this->lazy_affine.first){
                // NodeFactor じゃなくても、どうせ Value にアクセスしないので代入して OK 
                this->Value = this->lazy_affine.second.first * this->Value + this->lazy_affine.second.second;
                this->Min = this->lazy_affine.second.first * this->Min + this->lazy_affine.second.second;
                this->Max = this->lazy_affine.second.first * this->Max + this->lazy_affine.second.second;
                this->Sum = this->lazy_affine.second.first * this->Sum + this->SubTreeNodeFactorCount*this->lazy_affine.second.second;
                if(bool(this->left))this->left->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                if(bool(this->right))this->right->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                this->lazy_affine.first = false;
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

    /*
        SplayTree としての内部処理
        SplayTree としての内部処理
        SplayTree としての内部処理
    */

    // rootを根とする SplayTree において、求めたいindexまで降りていく(左側の部分木サイズを参照する)
    SplayNode *getNode(int index , SplayNode *root){
        if(root==nullptr)return root;
        SplayNode *now = root;
        while(true){
            now->eval();// ノードを見る前にeval
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
    
    // root を根とする Splay木 の i 番目のノードの左の辺を無くし、2 つの root (lef,rig) を返す
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
    


    // root が根の SplayTree の半開区間[l , r)の要素のValue全てに A 掛けて、その後 B 足す(区間アフィン変換)
    SplayNode* RangeAffine_sub(long long l , long long r , T A , T B , SplayNode* root){
        std::pair<SplayNode*,SplayNode*> tmp = split(r,root);
        SplayNode* rightRoot = tmp.second;
        tmp = split(l,tmp.first);
        tmp.second->set_lazyAffine(A,B);
        return merge(merge(tmp.first, tmp.second),rightRoot);
    }

    // root が根の SplayTree の半開区間[l , r)の要素のValue全てをxにする(区間更新クエリ)
    SplayNode* RangeUpdate_sub(long long l , long long r , T x , SplayNode* root){
        std::pair<SplayNode*,SplayNode*> tmp = split(r,root);
        SplayNode* rightRoot = tmp.second;
        tmp = split(l,tmp.first);
        tmp.second->set_lazyUpdate(x);
        return merge(merge(tmp.first, tmp.second),rightRoot);
    }

    /*
        オイラーツアー木としての内部処理
        オイラーツアー木としての内部処理
        オイラーツアー木としての内部処理
    */

    
    // [x] := 頂点 x を表す特殊 SplayNode (x→x で、EulerTour に一度のみ登場する )
    unordered_map<node_type,SplayNode*> NodeFactor;
    // [u][v] := u → v の移動を表す SplayNode 
    unordered_map<node_type,unordered_map<node_type,SplayNode*>> EdgeFactor;
    // 頂点が持つ値の初期値 (NodeFactor のみ値を持つ)
    T init_v;


    // 頂点集合を明示的に宣言しない関係上、必要なタイミングで一度 NodeFactor を確保する必要がある。
    bool register_node(node_type u , T val = T()){
        if(NodeFactor[u] != nullptr)return false;
        NodeFactor[u] = new SplayNode(u,u,init_v);
        return true;
    }
    // 必要なタイミングで一度 EdgeFactor を確保する必要がある。
    // 片方向の有向辺なことに注意
    bool register_edge(node_type u , node_type v){
        if(EdgeFactor[u][v] != nullptr)return false;
        EdgeFactor[u][v] = new SplayNode(u,v);
        return true;
    }
    // (u→v) の EdgeFactor 削除/解放 する
    bool delete_edge(node_type u , node_type v){
        if(EdgeFactor[u][v] == nullptr)return false;// すでに解放済み
        delete EdgeFactor[u][v];
        EdgeFactor[u][v] = nullptr;
        return true;
    }

    // u が属する木のオイラーツアーでの、u の NodeFactor の出現位置を答える (0-index)
    // 副作用 : u の NodeFactor が splay される
    long long find_NodeFactor(node_type u){
        register_node(u);
        NodeFactor[u]->splay();
        if(NodeFactor[u]->left != nullptr)return NodeFactor[u]->left->SubTreeSize;
        return 0;
    }

    // u,v が属する木のオイラーツアーでの、u→v と移動する EdgeFactor の出現位置を答える (0-index)
    // なければ -1
    // 副作用 : u の NodeFactor が splay される
    long long find_EdgeFactor(node_type u , node_type v){
        register_node(u);
        register_node(v);
        if(EdgeFactor[u][v] == nullptr)return -1;
        EdgeFactor[u][v]->splay();
        if(EdgeFactor[u][v]->left != nullptr)return EdgeFactor[u][v]->left->SubTreeSize;
        return 0;
    }


    
    void release(){
        for(pair<node_type,SplayNode*>p : NodeFactor)if(p.second != nullptr)delete p.second;
        for(pair<node_type,unordered_map<node_type,SplayNode*>> table : EdgeFactor){
            for(pair<node_type,SplayNode*>e : table.second)delete e.second;
        }
    }
    public:

    EulerTourTree():init_v(T()){}
    // 頂点の値の初期値を指定
    EulerTourTree(T init_v_):init_v(init_v_){}
    // デストラクタ
    ~EulerTourTree(){release();}
    // 複雑な挙動を回避するので、コンストラクタによるコピー/ムーブを一律に禁止する。コピーは copy() 関数に任せる
    EulerTourTree(const EulerTourTree<node_type,T> &x) = delete ;
    EulerTourTree<node_type,T> & operator = (const EulerTourTree<node_type,T> &x) = delete ;
    EulerTourTree ( EulerTourTree<node_type,T> && x){assert(0);}
    EulerTourTree<node_type,T> & operator = ( EulerTourTree<node_type,T> && x){assert(0);}
    

    // u が属する木のオイラーツアーのサイズ
    // 属する木のサイズ個 NodeFactor と、(サイズ-1)×2 個の EdgeFactor からなる
    // 副作用 : u の NodeFactor が splay される
    long long EulerTourSize(node_type u){
        register_node(u);
        NodeFactor[u]->splay();
        return NodeFactor[u]->SubTreeSize;
    }
    // u が属する木のサイズ
    // 副作用 : u の NodeFactor が splay される
    long long ComponentSize(node_type u){
        register_node(u);
        return (EulerTourSize(u) + 2)/3;// 復元する
    }

    // 頂点 u が属する木の根の番号
    // 副作用 : u の NodeFactor が splay される
    node_type root(node_type u){
        register_node(u);
        SplayNode* tmp = NodeFactor[u];
        tmp->splay();
        tmp->eval();
        while(tmp->left != nullptr){
            tmp = tmp->left;
            tmp->eval();
        }
        tmp->splay();
        return tmp->from;
    }

    // u,v が同じ連結成分に属するか
    // 副作用 : u,v の NodeFactor が未定義の順に splay される
    bool same(node_type u ,node_type v){return bool(root(u) == root(v));}

    // 頂点 i のもつ値(value)を x に変更する
    // 副作用 : i の NodeFactor を splay
    void update_val(node_type i , T x){
        register_node(i);
        NodeFactor[i]->splay();
        NodeFactor[i]->Value = x;
        NodeFactor[i]->update();
    }

    // 頂点 u を、属する木の根とする。
    // evert(u) すると、u の NodeFactor (u→u) が オイラーツアーの先頭になる
    // 副作用 : u の NodeFactor が splay される
    void evert(node_type u){
        register_node(u);
        if(ComponentSize(u) == 1)return;
        long long c = find_NodeFactor(u);
        NodeFactor[u]->splay();
        pair<SplayNode*,SplayNode*> tmp = split(c,NodeFactor[u]);
        merge(tmp.second,tmp.first);
        NodeFactor[u]->splay();
    }

    // u-v をリンクする (u の子に v をつける)
    // 副作用 : u の NodeFactor が splay される
    void link(node_type u , node_type v){
        assert(!same(u,v));
        register_node(u);
        register_node(v);
        register_edge(u,v);
        register_edge(v,u);
        evert(u);
        evert(v);
        NodeFactor[u]->splay();
        NodeFactor[v]->splay();
        merge(merge(merge(NodeFactor[u],EdgeFactor[u][v]),NodeFactor[v]),EdgeFactor[v][u]);
        NodeFactor[u]->splay();
    }

    // u-v をカットする。(辺が存在している前提)
    // 副作用 : u,v の NodeFactor を splay する
    void cut(node_type u , node_type v){
        register_node(u);
        register_node(v);
        assert(EdgeFactor[u][v] != nullptr && EdgeFactor[v][u] != nullptr);
        evert(u);
        // v の部分木に対応する区間を切り取る
        long long l = find_EdgeFactor(u,v);
        long long r = find_EdgeFactor(v,u);
        NodeFactor[u]->splay();
        std::pair<SplayNode*,SplayNode*> tmp = split(r+1,NodeFactor[u]);
        tmp.first = split(r,tmp.first).first;// (v→u) の EdgeFactor を切り取る
        tmp.first = split(l+1,tmp.first).first;
        tmp.first = split(l,tmp.first).first;// (u→v) の EdgeFactor を切り取る
        merge(tmp.first, tmp.second);
        NodeFactor[u]->splay();
        NodeFactor[v]->splay();
        // 切った辺は解放
        delete_edge(u,v);
        delete_edge(v,u);
    }


    // 頂点 u の部分木の頂点全ての Value を x にする
    // ただし、u の親 p を引数で与えないといけない (lct と併用すると ok)
    void SubTreeUpdate(node_type u , node_type p , T x ){
        // 辺が存在する
        assert(EdgeFactor[p][u] != nullptr);
        long long l = find_EdgeFactor(p,u);
        long long r = find_EdgeFactor(u,p);
        // (p→u) が (u→p) より左なら p の情報は正しい
        assert(l < r);
        NodeFactor[u]->splay();
        RangeUpdate_sub(l+1,r,x,NodeFactor[u]);
    }

    // 頂点 u の部分木の頂点全ての Value を A*Value + B にする
    // ただし、u の親 p を引数で与えないといけない (lct と併用すると ok)
    void SubTreeAffine(node_type u , node_type p , T A , T B){
        // 辺が存在する
        assert(EdgeFactor[p][u] != nullptr);
        long long l = find_EdgeFactor(p,u);
        long long r = find_EdgeFactor(u,p);
        // (p→u) が (u→p) より左なら p の情報は正しい
        assert(l < r);
        NodeFactor[u]->splay();
        RangeAffine_sub(l+1,r,A,B,NodeFactor[u]);
    }
    // 頂点 u の部分木の頂点全ての Value を Value + x にする
    // ただし、u の親 p を引数で与えないといけない (lct と併用すると ok)
    void SubTreeAdd(node_type u , node_type p , T x){SubTreeAffine(u,p,T(1),x);}

    // 頂点 u の部分木に対応するオイラーツアーの区間を表す SplayNode のコピーを取得
    // ただし、u の親 p を引数で与えないといけない (lct と併用すると ok)
    // read-only なので SplayNode の隣接ノードへのポインタを封印したものを返す
    SplayNode SubTree(node_type u , node_type p){ 
        // 辺が存在する
        assert(EdgeFactor[p][u] != nullptr);
        long long l = find_EdgeFactor(p,u);
        long long r = find_EdgeFactor(u,p);
        // (p→u) が (u→p) より左なら p の情報は正しい
        assert(l < r);
        NodeFactor[u]->splay();
        std::pair<SplayNode*,SplayNode*> tmp = split(r,NodeFactor[u]);
        SplayNode* rightRoot = tmp.second;
        tmp = split(l+1,tmp.first);
        SplayNode res = (*tmp.second);// 真ん中をコピー
        merge(merge(tmp.first, tmp.second),rightRoot);
        res.parent = res.left = res.right = nullptr;
        return res;// コピーを返す
    }


    // u が属する木のオイラーツアーを出力。
    // u の NodeFactor を splay するのでデバッグの際は注意
    void Debug(node_type u){
        long long sz = EulerTourSize(u);
        SplayNode* rt = NodeFactor[u];
        rt->splay();
        cerr << "Debug : " << u << endl;
        for( int i = 0 ; i < sz ; i++){rt = getNode(i,rt);cerr << "( " << rt->from << " , " << rt->to << " ) , "; } 
        NodeFactor[u]->splay();
        cerr << endl;
    }

    // オイラーツアー (到達を全て記録)
    vector<node_type> tour(node_type u){
        vector<node_type> res(0);
        SplayNode* rt = NodeFactor[u];
        rt->splay();
        long long sz = rt->SubTreeSize;
        rt = getNode(0,rt);
        res.push_back(rt->to);
        for( int i = 0 ; i < sz ; i++){
            rt = getNode(i,rt);
            if(rt->from != rt->to)res.push_back(rt->to);
        }
        return res;
    }
    // compress ver. のオイラーツアー。
    // 部分木に入る時と出る時のみをメモする
    vector<node_type> tour_compress(node_type u){
        vector<node_type> res,tmp;
        tmp = this->tour(u);
        // [x] := tour で x が最初に登場する地点 
        unordered_map<node_type,priority_queue<long long>> first;
        // [x] := tour で x が最後に登場する地点 
        unordered_map<node_type,priority_queue<long long>> last;
        for(int i =0 ; i < tmp.size() ; i++){
            first[tmp[i]].push(-i);
            last[tmp[i]].push(i);
        }
        for(int i =0 ; i < tmp.size() ; i++){
            if(i == abs(first[tmp[i]].top()))res.push_back(tmp[i]);
            if(i == abs(last[tmp[i]].top()))res.push_back(tmp[i]);
        }
        return res;
    }
    // read-only で nodeID の NodeFactor のコピーを取得 
    // 副作用 : nodeID の NodeFactor を splay 
    SplayNode operator [](node_type nodeID){
        NodeFactor[nodeID]->splay();
        SplayNode res = *(NodeFactor[nodeID]);
        res.parent = res.left = res.right = nullptr;// read-only なので隣接頂点へのアクセスを封印
        return res;
    }
};
















