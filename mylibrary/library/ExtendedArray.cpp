#include "MyTemplate.hpp"





/*
    T 型の半開区間 : [ L , R ) を set で管理する
    
    ベースは set 型の splay 木で、_sub がついた関数はスプレー木の機能のための内部関数である。  
    upper_bound や modify などの _sub がついていない内部関数もある。
                   
    重要 : 設計思想
        template で leftmost と rightmost を与え、アクセスできるのは [leftmost , rightmost) の中のみとする。  

        set で管理される区間で [leftmost , rightmost) をカバーしていることを保証する(ように実装した)。  
        つまり、[leftmost , rightmost) 内の任意の座標は set 内のいずれかの区間に含まれることが保証される!!!!!!

        また、set で管理される+++区間は重複がなく+++、区間の右端の値の大きさ順に管理されている。  
            (区間を左から並べると、[a,b) , [b,c) , [c,d) ... みたいになっている。)
        upperbound(x) :(右端が x 以下の区間の個数) は、x を含む区間の index になる(半開区間なので、左端も x 以下であることが保証される)。
        ただし、leftmost や rightpost 周りは例外があるので、余分に領域を取ることをお勧めする。
            (例えば、upper_bound(rightmost) は存在しない区間の index を返すし、lower_bound(leftmost) の index の区間 は leftmost を含まない )
        
        区間に T2 型の値(Value)を持たせることで、[leftmost , rightmost) の任意の index にアクセスできる配列のように振る舞う

    ただし、区間の長さが処理に必要なので、leftmost,rightmost を 最大値ギリギリにするとオーバーフローする。
    [-1e18 , 1e18) あたりがいい感じかも

    set(l,r,v) で、[l,r) の領域の値を一律 v に更新する
    RangeRffine(l,r,a,b) で、[l,r) の領域の値にアフィン変換を適用できる
    RangeQuery(l,r) で、[l,r) の領域の値のマージを取得できる

    ただし、Sumは 区間長*Value とかになるのでオーバーフローに注意
    

    ただし、+++++座標は連続+++++なので、厳密に一点を v に更新するといった操作ができない
        配列のように i 番目の要素を v に変更する場合、set(i,i+1,v) と読み替える必要がある。

    区間 [L,R) の x の circular shift を以下のように定義する(x>0)。  
        [L,R) 内の全ての要素の位置を右に x ずらす。ただし、R より右に移動したものは L に戻ってそこから更にずれる。  
        ex : {1,4,5,2} を 2 シフト →→→→ {5,2,1,4}
        左に x ずらす場合は、代わりに (r-l)-x だけ右にずらせば良い
    
    circular_shift を駆使すると、数直線に対して区間の 挿入/削除 が行える。
        LErase(l,r,init_) := [l,r) の要素を削除して残りの要素を ++左詰めする++。
                              余分にできた領域は init_ で初期化する。  
                            → RangeUpdate(l,r,init_) → circular_shift(l,rightmost,-(r-l))

        RPush(l,r,x) := [l,r] に値が x の領域を確保し、元々あった領域を ++右に押しやる++。
                              rightmost からはみ出た領域は破棄する。  
                            → circular_shift(l,rightmost,(r-l)) → RangeUpdate(l,r,x) 

        RErase(),LPush() も同様の考え方


    内部実装では SplayTree の [l,r) の部分木の遅延評価やマージを行うので、座標と混同しないように注意。
    |
    - 計算量について :
        |- 遅い時はいらない集約や遅延評価を消す。
        |- eval() しなくていい時は、eval() の中身を全消ししたり、eval() を呼び出さなくするとかなり速くなる
    |
    重要実装方針 : modify という内部関数を使う。
        座標の半開区間区間 [lef,rig) にクエリが飛んできた時、 
        [lef,rig) が、set で管理される区間の集合でちょうどカバーできるように
        SplayTree のノードが持つ区間の値を変更する
        例 :
                l.......r
        <...><....><.>......><..>

                   ↓↓↓

                l.......r
        <...><.><....><.><..><..>

        
    
*/
template<class T, class T2 >
class ExtendedArray{
    private:
    
    
    struct SplayNode{
 
        SplayNode *parent = nullptr;//親ノード
        SplayNode *left = nullptr;//左の子ノード
        SplayNode *right = nullptr;//右の子ノード
 
        T R;//区間の右端
        T L;//区間の左端

        // 区間の長さを R-L とする
        T length(){return R-L;}
        
        //区間は半開区間で [ L , R )
        T length_sum;//部分木のうち、区間がカバーしている範囲の長さの和
        T2 Value;// 区間の持つ値
        T2 Sum; // 部分木の要素について、(R-L)*Value の総和
        T2 Min; // 部分木の要素について、Value のmin
        T2 Max; // 部分木の要素について、Value のmax
        int SubTreeSize = 1;//1は初期化値(自分自身)



        // 遅延中の区間アフィン変換クエリ(遅延しているか , {A,B})
        pair<bool,pair<T2,T2> > lazy_affine ={false , {T2(),T2()}};
        void set_lazyAffine(T2& a, T2& b){
            if(this->lazy_affine.first)this->lazy_affine.second = { a*this->lazy_affine.second.first , a*this->lazy_affine.second.second + b};
            else this->lazy_affine = {true , {a,b}};
        }


        // 区間 [L,R) に遅延中のシフトクエリ (区間 [L,R) を右に lazy_shift.second ずらす)
        // 区間のキー [L,R) が数値型の場合のみ有効
        pair<bool,T> lazy_shift = {false,T()};
        void set_lazyShift(T x){
            if(this->lazy_shift.first)lazy_shift.second += x;
            else lazy_shift.second = x;
            this->lazy_shift.first = true;
        }
            

        SplayNode(){}
        
        SplayNode(T l , T r , T2 Value_){
            assert(l < r);
            R = r;
            L = l;
            Value = Value_;
            update();
        }
 
 
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
            return 2;
        }

        // あるNodeを回転を駆使し一番上まで持っていく
        void splay(){
            while(bool(this->parent)){
                if(this->parent->state() == 0){
                    this->rotate();
                    break;
                }
                // 親の state をすでに計算しているので、親の親 → 親 → 自分に reverse の評価を降ろせている
                if( this->parent->state() == this->state() )this->parent->rotate();
                else this->rotate();
                this->rotate();
            }
            this->update();// while が呼ばれなかった場合のため
            return;
        }

        /*
            サイズなど、Nodeの持っている情報をupdateする(順番大事)
        */
        void update(){
            this->eval();
            this->SubTreeSize = 1;
            this->length_sum = this->length();
            this->Min = this->Value;
            this->Max = this->Value;
            this->Sum = this->Value*this->length();
 
            if(bool(this->left)){
                this->left->eval();
                this->SubTreeSize += this->left->SubTreeSize;
                this->length_sum += this->left->length_sum;
                if(this->left->Min < this->Min)this->Min = this->left->Min;
                if(this->left->Max > this->Max)this->Max = this->left->Max;
                this->Sum += this->left->Sum;
            }
 
            if(bool(this->right)){
                this->right->eval();
                this->SubTreeSize += this->right->SubTreeSize;
                this->length_sum += this->right->length_sum;
                if(this->right->Min < this->Min)this->Min = this->right->Min;
                if(this->right->Max > this->Max)this->Max = this->right->Max;
                this->Sum += this->right->Sum;
            }
            return;
        }

        void eval(){
            if(this->lazy_affine.first){
                this->Value = this->lazy_affine.second.first * this->Value + this->lazy_affine.second.second;
                this->Min = this->lazy_affine.second.first * this->Min + this->lazy_affine.second.second;
                this->Max = this->lazy_affine.second.first * this->Max + this->lazy_affine.second.second;
                this->Sum = this->lazy_affine.second.first * this->Sum + (this->length_sum)*this->lazy_affine.second.second;
                if(bool(this->left))this->left->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                if(bool(this->right))this->right->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                //伝播したらフラグを折る
                this->lazy_affine.first = false;// second は放置しても OK (遅延評価をセットする際に上書きされるので)
            }

            // 区間シフト遅延評価(Sum などに影響はない)
            if(this->lazy_shift.first){
                this->L += this->lazy_shift.second; 
                this->R += this->lazy_shift.second; 
                if(this->left)this->left->set_lazyShift(this->lazy_shift.second);
                if(this->right)this->right->set_lazyShift(this->lazy_shift.second);
                this->lazy_shift.first = false;
            }
        }
 
    };
 

    //この木のroot,splitした後はmergeして戻す！！rootが迷子にならないように！！
    SplayNode *Root = nullptr;
        
    // Node の比較を定義 (遅延評価が載っていない前提が必要)
    // Node a , bに関して a <= b かどうかの演算 (<= であることが必須)
    bool CompareNode(SplayNode *a , SplayNode *b){return a->R <= b->R;}
 
    // rootを根とする部分木において、求めたいindexまで降りていく(左側の部分木サイズを参照する)
    SplayNode *getNode(int index , SplayNode *root){
        if(bool(root)==false)return root;
        
        SplayNode *now = root;
        while(true){
            now->eval();// ノードを見る前にeval
            int left_size = 0;
            if(bool(now->left))left_size = now->left->SubTreeSize;
            if(index < left_size)now = now->left;
            else if(index == left_size)break;
            else{
                now = now->right;
                index = index - left_size-1;
            }
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
    std::pair<SplayNode*,SplayNode*> DeleteNode_sub(int index , SplayNode *root){
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


  
    // SplayTree の左から l 以上 r 未満の ++ノード++の要素マージの取得クエリ(区間に対応する部分木の根ノードのコピーを返す)
    std::pair<SplayNode*,SplayNode> RangeNodeQuery_sub(int l , int r , SplayNode *root){
        SplayNode *leftRoot , *centerRoot , *rightRoot; 
        std::pair<SplayNode*,SplayNode*> tmp = split(r,root);
        rightRoot = tmp.second;
        tmp = split(l,tmp.first);
        SplayNode SubTreeRoot = *tmp.second;
        return std::make_pair(merge(merge(tmp.first,tmp.second),rightRoot),SubTreeRoot);
    }

    // SplayTree の左から l 以上 r 未満の ++ノード++ が持つ区間のValue全てに A 掛けて B 足す
    SplayNode* RangeNodeAffine_sub(int l , int r , T2 A , T2 B , SplayNode* root){
        SplayNode *leftRoot , *centerRoot , *rightRoot;
        std::pair<SplayNode*,SplayNode*> tmp = split(r,root);
        rightRoot = tmp.second;
        tmp = split(l,tmp.first);
        tmp.second->set_lazyAffine(A,B);
        return merge(merge(tmp.first,tmp.second),rightRoot);
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
            now->eval();// 遅延評価
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
        now->eval();
        now->splay();
        now->update();
        if(bool(now->left))res += now->left->SubTreeSize;
        return std::make_pair(now ,res);
    }
 
    // root が根の splaytree に NODE (別の連結成分)を挿入する
    // NODE は update されている必要がある
    SplayNode *insertNode_sub(SplayNode *NODE , SplayNode *root){
        NODE->update();// 集約と遅延評価をしておく
        if(bool(root) == false)return NODE;

        // lower_bound で、x < NODE の境界となる x を取得 (ただし境界の右側か左側かは不定)
        root = bound_sub(NODE,root,true).first;
        // root が x < NODE の境界の左右どちらかによって、NODE を配置する場所が変わる
        if(!CompareNode(NODE , root)){// root < NODE の場合は root の右に
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


    // _sub ではないけど、クラス内部でのみ使う処理
    // _sub ではないけど、クラス内部でのみ使う処理
    // _sub ではないけど、クラス内部でのみ使う処理
    // _sub ではないけど、クラス内部でのみ使う処理

 
    // Value に x を持つ区間 [lef,rig) を set に挿入する
    // 現在の Splay 木に [lef,rig) と重なる区間(ノード)が存在しない前提が必要
    void insertNode(T lef , T rig , T2 x){
        assert(leftmost <= lef && rig <= rightmost);
        if(lef == rig)return;
        assert(lef < rig);
        Root = insertNode_sub(new SplayNode(lef,rig,x) ,Root);
        Root->update();
        return;
    }
 

    /*
        index番目のNodeを消去
    */
    void DeleteNode(int index){
        if(index<0 || index >= SplayTreeSize())assert(0);
        std::pair<SplayNode*,SplayNode*> tmp = DeleteNode_sub(index,Root);
        Root = tmp.first;
        if(bool(Root) == false )return;
        Root->update();
        return;
    }



    /*
        座標 i に切れ込みを入れ、切れ込みを入れた地点の両側の区間の、Splay Tree における index を返す
                i
        <...><.......>......><..>
                   ↓↓↓
        <...><..><...>......><..>
               f    s

        first  : 右端が i である区間(ノード)の index
        second : 左端が i である区間(ノード)の index
        また、すでに i に切れ込みが入っている場合も同様に両側の区間の index を返す
        ただし、leftmost や rightmost に切れ込みを入れた場合、SplayTree の index 範囲外を返すことに注意
    */
    pair<int,int> modify(T i){
        if(i == leftmost)return {-1,0};
        if(i == rightmost)return {SplayTreeSize()-1,SplayTreeSize()};
        assert(leftmost < i && i < rightmost);
        
        int it_ = lower_bound(i);
        
        Root = getNode(it_,Root);// 右端が ++++r 以上++++ の区間
        Root->update();

        if(i == Root->L)return {it_-1,it_};// i が 左端と一致する場合 (i が leftmost の時のみ)
        // 通常、it_-1 番目の区間の右端 : lR == Root->L < i であることが保証される。
        if( i < Root->R ){
            T nR = Root->R;
            T2 V = Root->Value;
            Root->R = i;// アクセスした区間の右端を i に変更する。  
            Root->update();
            insertNode(i, nR , V);// その右に [i,nR) を入れる。  
        }
        return {it_,it_+1};
    }



    // このクラスで扱う区間の範囲は leftmost 以上、rightmost 未満とする。
    const T leftmost ; 
    const T rightmost;

    /*
        ここからは、クラスの外で動かす関数たちです。
        ここからは、クラスの外で動かす関数たちです。
        ここからは、クラスの外で動かす関数たちです。
        ここからは、クラスの外で動かす関数たちです。
    */

    public:
    
    // 座標がとりうる値の範囲 (半開区間) を指定し、全ての要素を init_ で初期化する。
    ExtendedArray(T leftmost_ , T rightmost_ ,T2 init_):leftmost(leftmost_),rightmost(rightmost_){
        insertNode(leftmost,rightmost,init_);
    }


    // 座標の上限と下限
    T Llimit(){return leftmost;}
    T Rlimit(){return rightmost;}

    // ノードの個数
    int SplayTreeSize(){if(Root==nullptr)return 0;return Root->SubTreeSize;}
                   
    // Rがx未満のSplayNodeの個数を返す    
    int lower_bound(T x){
        if(SplayTreeSize() == 0)return 0;
        SplayNode* nd = new SplayNode(leftmost,x,T2());
        std::pair<SplayNode*,int> tmp = bound_sub(nd,Root,true);
        Root = tmp.first;
        Root->update();
        delete nd;
        return tmp.second;
    }
 
 
    /*
        Rがx以下のSplayNodeの個数を返す
        重要 : 座標 x が含まれる区間のノードを探すときは、upper_bound(x) すると良い。
    */
    int upper_bound(T x){
        if(SplayTreeSize() == 0)return 0;
        SplayNode* nd = new SplayNode(leftmost,x,T2());
        std::pair<SplayNode*,int> tmp = bound_sub(nd,Root,false);
        Root = tmp.first;
        Root->update();
        delete nd;
        return tmp.second;
    }
                   
    // 木のindex番目のSplayNodeのコピーを取得
    SplayNode get(int index){   
        assert(0<=index && index < SplayTreeSize());
        Root = getNode(index,Root);
        Root->update();
        return *(Root);
    }

    /*  
        |x| <= (rig-lef) であることが必要(座標が数値型の時のみ有効)
        [l,r) 内の要素を右に x だけ circular shift する。 
        circular shift : 区間からはみ出した分は、反対側に移動してシフトする。
        ex : {1,4,5,2} を -2 シフト →→→→ {5,2,1,4}
        割と重い操作です。  
    */
    void circular_shift(T lef , T rig , T x){
        assert(leftmost <= lef && rig <= rightmost);
        if(abs(x) == rig - lef)return;
        if(x == T(0))return;

        // 左に |x| シフトするのは、右に rig-lef-|x| シフトするのと同じ
        if(x < 0)x = rig-lef+x;
        assert(0 < x && x < rig - lef);

        
        // 1-: lef , mid , rig で modify する。
        // 2-: [-∞ , lef) ,[lef , mid) , [mid , rig) ,[rig,∞) の 4 区間に分割する
        // 3-: [lef , mid) に +x シフト、[mid , rig) に +(x-rig+lef) シフトして、マージする
        
        // lef < mid < rig が保証されている。
        T mid = rig - x;

        int it_l = modify(lef).second;// 左端が lef である区間の index
        int it_m = modify(mid).second;// 左端が mid である区間の index
        int it_r = modify(rig).first; // 右端が rig である区間の index
        assert(0 <= it_l && it_l < it_m && it_m <= it_r && it_r < SplayTreeSize());
        // 4 つの部分に分割して、p2,p3 を適切にシフトする
        pair<SplayNode*,SplayNode*> tmp = split(it_r+1 , Root);
        SplayNode* p4 = tmp.second;
        tmp = split(it_m,tmp.first);
        SplayNode* p3 = tmp.second;
        tmp = split(it_l,tmp.first);
        SplayNode* p2 = tmp.second;
        SplayNode* p1 = tmp.first;

        p2->set_lazyShift(x);
        p3->set_lazyShift(T(x-rig+lef));// lef 側に移動させる
        
        p2->update();
        p3->update();
        Root = merge(merge(merge(p1,p3),p2),p4);
    }
 
    /*
        [l,r) に値が x の領域を確保し、元々存在した領域を右に押し退け、rightmost を超えた領域は破棄する。
        ex:     ↓[1,2]
            [0,0,0,2,2,3] → [0,0,1,2,0,2]
    */
    void RPush(T lef , T rig , T2 x){
        assert(leftmost <= lef && rig <= rightmost);
        if(lef == rig)return;
        assert(lef < rig);
        circular_shift(lef,rightmost,(rig - lef));
        RangeUpdate(lef,rig,x);
    }

    // [l,r) に値が x の領域を確保し、元々存在した領域を左に押し退け、lefttmost を超えた領域は破棄する。
    void LPush(T lef , T rig , T2 x){
        assert(leftmost <= lef && rig <= rightmost);
        if(lef == rig)return;
        assert(lef < rig);
        circular_shift(leftmost,rig,-(rig - lef));
        RangeUpdate(lef,rig,x);
    }

    // [l,r) の要素を削除して残りの要素を ++左詰めする++。余分にできた区間を init_ で初期化する。
    void LErase(T lef , T rig , T2 init_){
        assert(leftmost <= lef && rig <= rightmost);
        if(lef == rig)return;
        assert(lef < rig);
        RangeUpdate(lef,rig,init_);
        circular_shift(lef,rightmost,-(rig-lef));
    }

    // [l,r) の要素を削除して残りの要素を ++右詰めする++。余分にできた区間を init_ で初期化する。
    void RErase(T lef , T rig , T2 init_){
        assert(leftmost <= lef && rig <= rightmost);
        if(lef == rig)return;
        assert(lef < rig);
        RangeUpdate(lef,rig,init_);
        circular_shift(leftmost,rig,(rig-lef));
    }

    // 半開区間 [ l , r ) 内の値を x に変更する
    void RangeUpdate( T lef , T rig , T2 x){
        assert(leftmost <= lef && rig <= rightmost);
        if(lef == rig)return;
        assert(lef < rig);

        int itl = modify(lef).second;// 左端が lef の区間の index (modify してるので、必ず存在する)
        int itr = modify(rig).first;// 右端が rig の区間の index (modify してるので、必ず存在する)
        for(int it = itr-1;it>=itl;it--)DeleteNode(it);// index が減っていくので、右から削除する
        Root = getNode(itl,Root);// 最終的に、右端が rig の区間のノードの index は itl になる
        Root->L = lef;
        Root->Value = x;
        Root->update();
        return;
    }


    // 半開区間[lef , rig) 内の要素全てに A かけて B 足す
    void RangeAffine(T lef , T rig , T2 A , T2 B){
        assert(leftmost <= lef && rig <= rightmost);
        if(lef == rig)return;
        assert(lef < rig);

        int itl = modify(lef).second;// 左端が lef の区間の index (modify してるので、必ず存在する)
        int itr = modify(rig).first;// 右端が rig の区間の index (modify してるので、必ず存在する)
        Root = RangeNodeAffine_sub(itl,itr+1,A,B,Root);// Node に対する区間クエリの半開区間であることに注意
        Root->update();
    }

    // 半開区間[lef , rig) 内の要素全てに x 足す
    void RangeAdd(T lef , T rig , T2 x){
        assert(leftmost <= lef && rig <= rightmost);
        RangeAffine(lef,rig,T2(1),x);
    }

    // 半開区間 [l ,r) 内の要素の総和
    T2 RangeSumQuery(T lef , T rig){
        assert(leftmost <= lef && rig <= rightmost);
        assert(lef < rig);

        int itl = modify(lef).second;// 左端が lef の区間の index (modify してるので、必ず存在する)
        int itr = modify(rig).first;// 右端が rig の区間の index (modify してるので、必ず存在する)
        pair<SplayNode*,SplayNode> tmp = RangeNodeQuery_sub(itl,itr+1,Root);// Node に対する区間クエリの半開区間であることに注意
        Root = tmp.first;
        Root->update();
        return tmp.second.Sum;
    }

    // 半開区間 [l ,r) 内の要素のmin
    T2 RangeMinQuery(T lef , T rig){
        assert(leftmost <= lef && rig <= rightmost);
        assert(lef < rig);

        int itl = modify(lef).second;// 左端が lef の区間の index (modify してるので、必ず存在する)
        int itr = modify(rig).first;// 右端が rig の区間の index (modify してるので、必ず存在する)
        pair<SplayNode*,SplayNode> tmp = RangeNodeQuery_sub(itl,itr+1,Root);// Node に対する区間クエリの半開区間であることに注意
        Root = tmp.first;
        Root->update();
        return tmp.second.Min;
    }

    // 半開区間 [l ,r) 内の要素のmin
    T2 RangeMaxQuery(T lef , T rig){
        assert(leftmost <= lef && rig <= rightmost);
        assert(lef < rig);

        int itl = modify(lef).second;// 左端が lef の区間の index (modify してるので、必ず存在する)
        int itr = modify(rig).first;// 右端が rig の区間の index (modify してるので、必ず存在する)
        pair<SplayNode*,SplayNode> tmp = RangeNodeQuery_sub(itl,itr+1,Root);// Node に対する区間クエリの半開区間であることに注意
        Root = tmp.first;
        Root->update();
        return tmp.second.Max;
    }

    // 座標 x の値のコピーを渡す
    T2 operator [](T x){
        if(x == leftmost)return get(0).Value;
        assert(leftmost < x && x < rightmost);
        int it_ = upper_bound(x);
        return get(it_).Value;
    }


    // 列として見た時のサイズ
    int size(){
        assert(rightmost - leftmost == Root->length_sum);
        return rightmost - leftmost;
    }

    void Debug(){
        cerr << "DEBUG" << endl;
        cerr << "L : " ;for(int i = 0 ; i < SplayTreeSize();i++)cerr << get(i).L << " ";cerr << endl;
        cerr << "R : " ;for(int i = 0 ; i < SplayTreeSize();i++)cerr << get(i).R << " ";cerr << endl;
        cerr << "V : " ;for(int i = 0 ; i < SplayTreeSize();i++)cerr << get(i).Value << " ";cerr << endl;
    }
};
 
 



int test_ExtendedArray(){
    ll n = 100000;
    vll A(n,0) ,A_cop(n,0);
    ExtendedArray<ll,ll> S(0,n,0);
    ll cnt = 0;
    bool ok = true;
    /*
        t = 
        0 : update_val(i,x);
        1 : RangeAdd(l,r,x);
        2 : RangeUpdate(l,r,x);
        3 : circular_shift(l,r,1)
        4 : circular_shift(l,r,-1)
        5 : push(p_i,x) : RPush,LPush 両方テスト
        6 : judge ? A[i] = S.get(i).Value
        7 : judge  each Max Min Sum , range(l,r)
        8 : judge all element
        9 : delete(i) : 
        10 : rangeDelete(l,r) : LErase,RErase 両方テスト
    */ 
    while(n > 0){
        cnt++;
        ll t = rand()%11;
        ll l , r , ind , x , p_i;
        ind = rand()%n;
        p_i = rand()%(n);
        ll l_ = rand()%n;
        ll r_ = rand()%n;
        l = min(l_,r_);
        r = max(r_,l_)+1;
        x = rand()%1000;
        bool judge = true;

        if(t == 0){ 
            A[ind] = x;
            S.RangeUpdate(ind,ind+1,x);
        }else if(t == 1){
            S.RangeAdd(l,r,x);
            FOR(i,l,r)A[i] += x;
        }else if(t == 2){
            S.RangeUpdate(l,r,x);
            FOR(i,l,r)A[i] = x;
        }else if(t == 3){
            S.circular_shift(l,r,1);
            A_cop = A;
            FOR(i,l,r){
                ll L = l + (i-l+1+r-l)%(r-l);
                A[L] = A_cop[i];
            }
        }else if(t == 4){
            S.circular_shift(l,r,-1);
            A_cop = A;
            FOR(i,l,r){
                ll L = l + (i-l-1+r-l)%(r-l);
                A[L] = A_cop[i];
            }
        }else if(t == 5){
            // 2 通りでテスト   
            if(rand()%2)S.RPush(p_i,p_i+1,x);
            else {
                S.circular_shift(S.Llimit(),S.Rlimit(),1);
                S.LPush(p_i,p_i+1,x);
            }
            A.insert(A.begin() + p_i , x);
            A.pop_back();
        }else if(t == 6){
            if(A[ind] != S[ind]){
                judge = false;
                cerr << "NG : type = 6 " << endl;
                S.Debug();
                cerr << "A : " << endl;
                REP(i,n)cerr << A[i] << " ";
                
                ENDL;
            }
        }else if(t == 7){
            ll mn = INF;
            ll mx = NINF;
            ll sum = 0;
            FOR(i,l,r){
                sum += A[i];
                mn = min(mn,A[i]);
                mx = max(mx,A[i]);
            }
            if(mn != S.RangeMinQuery(l,r) || mx != S.RangeMaxQuery(l,r) || sum != S.RangeSumQuery(l,r)){
                judge = false;
                cerr << "NG : type = 7 " << endl;
                cerr << "min : " << " " << mn << " " << S.RangeMinQuery(l,r) << endl;
                cerr << "max : " << " " << mx << " " << S.RangeMaxQuery(l,r) << endl;
                cerr << "sum : " << " " << sum << " " << S.RangeSumQuery(l,r) << endl;
            }
        }else if(t == 8){
            REP(i,n){
                if(A[i] != S[i]){
                    judge = false;
                }
            }            
            if(!judge){
                judge = false;
                cerr << "NG : type = 8 " << endl;
                //cerr << "A : " << endl;REP(i,n)cerr << A[i] << " ";ENDL;
                //cerr << "S : " << endl;REP(i,n)cerr << S[i] << " ";ENDL;
            }

        }else if(t == 9){
            A_cop = A;
            vll(0).swap(A);
            
            REP(i,n)if(i != ind)A.push_back(A_cop[i]);
            S.LErase(ind,ind+1,0);
        }else if(t ==10){
            if(r-l >= n /40 )continue;
            A_cop = A;
            vll(0).swap(A);
            REP(i,A_cop.size())if(l > i || i >= r)A.push_back(A_cop[i]);

            // 2 通りでテスト
            if(rand()%2){
                S.RErase(l,r,0);
                S.circular_shift(S.Llimit() , S.Rlimit(),-(r-l));
            }else{
                S.LErase(l,r,0);
            }

            
        }
        n = A.size();
        if(cnt % 5000 == 0)cerr << "OK !  cnt = " << cnt << "  Size  = " << n << endl;
        
        if(!judge)break;
    }   
    return 0;
}



