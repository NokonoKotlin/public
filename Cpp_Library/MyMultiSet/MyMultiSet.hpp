#include "./../template/template.hpp"





template<class type_key , class type_value>
class MyMultiSet{
    private:
 
    struct SplayNode{
        SplayNode *parent = nullptr;//親ノード
        SplayNode *left = nullptr;//左の子ノード
        SplayNode *right = nullptr;//右の子ノード
 
        type_key Key;//値(この値をもとに昇順に並んでいる)
        type_value Value;//Keyとは別に頂点に持たせる値(Keyと型が違う可能性もあるので、T2)
 
        type_key Sum_key;//部分木のうち、Keyの値の和
        type_value Min_val,Max_val,Sum_val;//部分木のうち、Valueの値の最大、最小、和
 
        int SubTreeSize = 1;//1は初期化値(自分自身)
 
 
        SplayNode(){}
        SplayNode(type_key key_ , type_value val_){
            Key = key_;
            Value = val_;
            update();
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
            if(this->parent == nullptr)return 0;
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
            this->update();// while が呼ばれなかった場合のため
            return;
        }

 
        /*
            サイズなど、Nodeの持っている情報をupdateする(順番大事)
        */
        void update(){
            this->SubTreeSize = 1;
            this->Sum_key = this->Key;
            // 結合して良い代入演算子を持つクラスである必要がある
            this->Max_val = this->Sum_val = this->Min_val = this->Value;
            
            if(bool(this->left)){
                this->SubTreeSize += this->left->SubTreeSize;
                if(this->left->Min_val < this->Min_val)this->Min_val = this->left->Min_val;
                if(this->left->Max_val > this->Max_val)this->Max_val = this->left->Max_val;
                this->Sum_key += this->left->Sum_key;
                this->Sum_val += this->left->Sum_val;
            }
 
            if(bool(this->right)){
                this->SubTreeSize += this->right->SubTreeSize;
                if(this->right->Min_val < this->Min_val)this->Min_val = this->right->Min_val;
                if(this->right->Max_val > this->Max_val)this->Max_val = this->right->Max_val;
                this->Sum_key += this->right->Sum_key;
                this->Sum_val += this->right->Sum_val;
            }
 
            return;
        }
 
    };
 
    
    /*
        Node a , bに関して a <= b かどうか...
        paired_key = false ならKeyのみの比較。そうでないなら(Key,Value)の比較
        Node は update() されている必要がある
    */
    bool CompareNode(SplayNode *a , SplayNode *b , bool paired_key = false){
        if(!paired_key)return a->Key <= b->Key;
        else{
            if(a->Key < b->Key)return true;
            else if(a->Key == b->Key){
                if(a->Value <= b-> Value)return true;
                else return false;
            }else return false;
        }
    }
    
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
 
    
    
    // rootを根とする部分木において、
    // 左から index の場所を削除して、両端をくっつける
    // second は消したやつ (ここではメモリの解放はしない)
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
    

    /*
        rootが根であるような部分木で、以下の条件を満たすノード x の個数を返す (比較 <= は ComparNode の意味)
        lower = true : x < node (lower_bound)
        lower = false: x <= node (upper_bound)
        paired_key で ペアの比較か、Key のみの比較かを指定する
        条件を満たすギリギリの境界を探索する (ただし、境界の左右どちらかは不定)
    */
    std::pair<SplayNode*,int> bound_sub(SplayNode* Node , SplayNode *root , bool lower ,  bool paired_key ){
        if(bool(root) == false)return std::make_pair(root,0);
        SplayNode *now = root;// 条件による境界ギリギリのノードを根から探索する
        int res = 0;// 答え
        Node->update();
        while(true){
            bool satisfy = CompareNode(now,Node,paired_key); // upper_bound (now <= Node)
            if(lower)satisfy = !CompareNode(Node,now,paired_key); // lower_bound (now < Node)

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
    SplayNode *insert_sub(SplayNode *NODE , SplayNode *root , bool paired_key){
        NODE->update();// 集約と遅延評価をしておく
        if(bool(root) == false)return NODE;

        // lower_bound で、x < NODE の境界となる x を取得 (ただし境界の右側か左側かは不定)
        root = bound_sub(NODE,root,true,paired_key).first;
        // root が x < NODE の境界の左右どちらかによって、NODE を配置する場所が変わる
        if(!CompareNode(NODE , root , paired_key)){// root < NODE の場合は root の右に
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
    
 
  
    protected:

    //この木のroot,splitした後はmergeして戻す！！rootが迷子にならないように！！
    SplayNode *Root = nullptr;
    // (Key,Value) の両方が有効な値かどうか
    // insert(k) を呼び出すと未定義の Value が含まれることになるので false になる。
    bool _paired = true; // 初期状態は必ず true (コピー時などに影響)

    // ArraySplayTree のように position を取得する場合は Delete はメモリ解放をしないことに注意
    void release(){while(Root != nullptr)this->Delete(0);}
    // 初期状態にする (copy() などで使用)
    void init(){
        Root = nullptr;
        _paired = true; 
    }
    // 先頭要素のアドレスを取得。
    const SplayNode* const begin(){
        if(size() == 0)return nullptr;
        Root = getNode(0,Root);
        return Root;
    }

    public:

    MyMultiSet(){init();}
    ~MyMultiSet(){release();}
    // 複雑な挙動を回避するので、コンストラクタによるコピー/ムーブを一律に禁止する。コピーは copy() 関数に任せる
    MyMultiSet(const MyMultiSet<type_key,type_value> & x) = delete ;
    MyMultiSet& operator = ( const MyMultiSet<type_key,type_value> & x) = delete ;
    MyMultiSet ( MyMultiSet<type_key,type_value>&& x){assert(0);}
    MyMultiSet& operator = ( MyMultiSet<type_key,type_value>&& x){assert(0);}
    // 宣言後に代入する用に、コピーコンストラクタとは別のコピー関数を用意 
    // アクセスは構造を変化させるので、const をつけて右辺値を取得できない。
    // よって、右辺値 copy と左辺値の copy を両方実装する
    void copy(MyMultiSet<type_key,type_value>&& x){
        // 自己代入禁止
        assert(this->begin() != x.begin());
        release();
        init();
        for(int i = 0 ; i < x.size() ; i++ ){
            SplayNode t = x.get(i);
            this->insert_pair(t.Key,t.Value);// T の _paired を無視してとりあえずペアを入れる
        }
        this->_paired = x._paired;// _paired の状態はコピー元を参照。
    }
    void copy(MyMultiSet<type_key,type_value>& x){
        // 自己代入禁止
        if(this->begin() == x.begin())return;// 先頭要素のアドレスだけ見れば、同一オブジェクトかを判定できる
        release();
        init();
        for(int i=0;i<x.size();i++){
            SplayNode t=x.get(i);
            this->insert_pair(t.Key,t.Value);
        }
        this->_paired = x._paired;
    }
    
    int size(){
        if(Root == nullptr)return 0;
        return Root->SubTreeSize;
    }

    // 左から i 番目のNodeのコピーを取得 (0-index)
    // ただし、値の取得が目的なので right などの隣接頂点へのアクセスを封印する
    SplayNode get(int i){
        assert(0 <= i && i < size());
        Root = getNode(i,Root);
        SplayNode res = (*Root);
        res.right = res.left = res.parent = nullptr;// 隣接するノードへのアクセスを封じる
        return res;
    }


    // [l,r) に対応する部分木の根のコピーを返す。ただし、隣接ノードへのアクセスを封じる。
    // 使用用途はモノイド積の取得に限定 (コピーなので、オリジナルの現在の値とは関係なく [l,r) のデータが格納されている)
    SplayNode GetRange(int l , int r){
        assert(0 <= l && l < r && r <= size());
        std::pair<SplayNode*,SplayNode*> tmp = split(r,Root);
        SplayNode* rightRoot = tmp.second;
        tmp = split(l,tmp.first);// 部分木を取り出す。
        SplayNode res = (*tmp.second);// 取り出した部分木の根をコピーする。
        res.right = res.left = res.parent = nullptr;// 隣接するノードへのアクセスを封じる
        Root = merge(merge(tmp.first,tmp.second),rightRoot);
        return res;
    }
 
    // Keyのみを指定して挿入 ( Value は指定しないので未定義 )
    void insert( type_key key_ ){
        // 未定義の Value が追加されたので、今後 (Key,Value) の pair に注目する操作が行えなくなる
        _paired = false;
        Root = insert_sub(new SplayNode(key_,type_value(0)) ,Root , false);
        return;
    }
 
 
    // KeyとValueを指定して挿入
    void insert_pair( type_key key_ , type_value val_){
        assert("insert() を行ってからは Value にアクセスできません。" != "" && _paired);
        Root = insert_sub(new SplayNode(key_,val_) ,Root,true);
        return;
    }
 

    // index番目のNodeを消去 (0-index)
    void Delete(int index){
        if(index<0 || index >= size())assert(0);
        std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(index,Root);
        Root = tmp.first;
        return;
    }


    // Keyとして、key_を持つ頂点を一つpop。ただし、Valueについては指定しない
    void erase(type_key key_){
        int it = find(key_);
        if(it!=-1)Delete(it);
        return;
    }
 
    /*
        (key_,val_)を持つ頂点を一つ削除
    */
    void erase_pair(type_key key_ , type_value val_){
        assert("insert() を行ってからは Value にアクセスできません。" != "" && _paired);
        int it = find_pair(key_ , val_);
        if(it!=-1)Delete(it);
        return;
    }
 

    // Key が x 未満の頂点の個数を返す    
    int lower_bound(type_key x){
        if(size() == 0)return 0;
        SplayNode* nd = new SplayNode(x,type_value(0));
        std::pair<SplayNode*,int> tmp = bound_sub(nd,Root,true,false);
        Root = tmp.first;
        delete nd;
        return tmp.second;
    }
 
     
    // (Key , Value)に関して、a < NODE(x,y)であるような頂点の個数を返す
    int lower_bound_pair(type_key x , type_value y){
        assert("insert() を行ってからは Value にアクセスできません。" != "" && _paired);
        if(size() == 0)return 0;
        SplayNode* nd = new SplayNode(x,y);
        std::pair<SplayNode*,int> tmp = bound_sub(nd,Root,true,true);
        Root = tmp.first;
        delete nd;
        return tmp.second;
    }
 
 
    // Key が x 以下の頂点の個数を返す
    int upper_bound(type_key x){
        if(size() == 0)return 0;
        SplayNode* nd = new SplayNode(x,type_value(0));
        std::pair<SplayNode*,int> tmp = bound_sub(nd,Root,false,false);
        Root = tmp.first;
        delete nd;
        return tmp.second;
    }
 
    // (Key , Value)に関して、a <= NODE(x,y)であるような頂点の個数を返す
    int upper_bound_pair(type_key x , type_value y){
        assert("insert() を行ってからは Value にアクセスできません。" != "" && _paired);
        if(size() == 0)return 0;
        SplayNode* nd = new SplayNode(x,y);
        std::pair<SplayNode*,int> tmp = bound_sub(nd,Root,false,true);
        Root = tmp.first;
        delete nd;
        return tmp.second;
    }
    
    // Key == xであるような頂点のindex (複数あるなら最初のもの) を返す (0-index)。なければ-1を返す
    int find(type_key x){
        if(size() == 0)return -1;
        if(upper_bound(x) - lower_bound(x) <= 0)return -1;
        else return lower_bound(x);
    }

    // (Key,Value) == (x,y)であるような頂点の index (複数あるなら最初のもの) を返す (0-index)。なければ-1を返す
    int find_pair(type_key x , type_value y){
        assert("insert() を行ってからは Value にアクセスできません。" != "" && _paired);
        if(size() == 0)return -1;
        if(upper_bound_pair(x,y) - lower_bound_pair(x,y) <= 0)return -1;
        else return lower_bound_pair(x,y);
    }


    // 一列に表示
    void Debug(){
        std::cerr<<"DEBUG: -- Size = " << size() << std::endl;
        for( int i = 0 ; i < size() ; i++)std::cerr<< get(i).Key << " ";
        if(_paired == false){
            cerr << "key only" << endl;
            return ; 
        }
        std::cerr<< std::endl;
        for( int i = 0 ; i < size() ; i++)std::cerr<< get(i).Value << " ";
        std::cerr<< std::endl;
    }
 
    // 末尾を取得
    SplayNode back(){assert(size()>0);return get(size()-1);}
    // 先頭を取得
    SplayNode front(){assert(size()>0);return get(0);}
    // 末尾を削除
    void pop_back(){assert(size()>0);Delete(size()-1);}
    // 先頭を削除
    void pop_front(){assert(size()>0);Delete(0);}
    // read-only でアクセス
    SplayNode operator [](int index){return get(index);}
                   
};




