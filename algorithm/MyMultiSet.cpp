#include "MyTemplate.hpp"





/*
 
    ノードが T 型のKey と T2 型のValueを持つ Splay Tree で、(Key , Value) が +++ 辞書順 +++ に並んでいる

    C++ の set とは異なり、Key の重複を許す。(Value も当然重複 OK)
    また、get(i) で Splay Tree (Set) の i 番目の要素へのアクセスができる (0-index)
    (Key,Value) に関して以下の操作が可能
        - insert_pair(k,v) で (k,v) を持つノードを追加し、erase_pair(k,v) で (k,v) を持つノードを(存在すれば)削除する。

        - upper_bound_pair(k,v) : (Key,Value) が辞書順で (k,v) 以下の要素数を返す
        - lower_bound_pair(k,v) : (Key,Value) が辞書順で (k,v) 未満の要素数を返す
        - find_pair(k,v) : (Key,Value) が (k,v) である要素の index を返す。存在しなければ -1 を返す。(0-index)、

        - RangeValueSumQuery(l,r) : 要素のうち、辞書順で l 番目から r-1 番目までの要素の Value の総和を返す ([l,r) は 0-index の半開区間 )
        - RangeValueMinQuery(l,r) : 要素のうち、辞書順で l 番目から r-1 番目までの要素の Value の Min を返す ([l,r) は 0-index の半開区間 )
        - RangeValueMaxQuery(l,r) : 要素のうち、辞書順で l 番目から r-1 番目までの要素の Value の Max を返す ([l,r) は 0-index の半開区間 )
        

    ノードの Key だけに注目して、通常の set のように振る舞わせることもできる。  
        - insert(k) : Key = k である要素を追加する。ただし、Value を指定しないので Value は未定義とする。
        - erase(k) : Key = k である要素を一つ削除する。ただし、Value について特に指定しないことに注意。

        - upper_bound(k) : Key が k 以下の要素数を返す
        - lower_bound(k) : Key が k 未満の要素数を返す
        - find(k) : Key が k である要素の index を返す。存在しなければ -1 を返す。(0-index)、

        - RangeKeySumQuery(l,r) : 要素のうち、辞書順で l 番目から r-1 番目までの要素の Key の総和を返す ([l,r) は 0-index の半開区間 )


    ペアを持つノードとKeyしか持たないノードが混在するといけないので、insert(k) について以下の仕様とする。
        insert() を呼び出した時点で、Value が未定義の要素が存在することになるので、
            | insert() を呼び出した後は upper_bound_pair , find_pair , RangeValueMaxQuery などの
            | Value に関する関数を呼び出すとランタイムエラーになるようにしました。
        (ただし、Key だけに関係する関数は変わらず使用できる。)
        

    (Key,value) 辞書順や Key の昇順で並んでいると書いたが、CompareNode() を変更することで要素の並び順を変更できる。
    
    Splay Tree ライブラリの基本仕様 : 
        関数に渡す SplayNode や、関数から返される SplayNode は全て update (& eval) されている必要がある
 
*/
template<class T , class T2>
class MyMultiSet{
    private:
 
    struct SplayNode{
        SplayNode *parent = nullptr;//親ノード
        SplayNode *left = nullptr;//左の子ノード
        SplayNode *right = nullptr;//右の子ノード
 
        T Key;//値(この値をもとに昇順に並んでいる)
        T2 Value;//Keyとは別に頂点に持たせる値(Keyと型が違う可能性もあるので、T2)
 
        T Sum_key;//部分木のうち、Keyの値の和
        T2 Min_val,Max_val,Sum_val;//部分木のうち、Valueの値の最大、最小、和
 
        int SubTreeSize = 1;//1は初期化値(自分自身)
 
 
        SplayNode(){}
        SplayNode(T key_ , T2 val_){
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
 
 
    //この木のroot,splitした後はmergeして戻す！！rootが迷子にならないように！！
    SplayNode *Root = nullptr;
    // (Key,Value) の両方が有効な値かどうか
    // insert(k) を呼び出すと未定義の Value が含まれることになるので false になる。
    bool _paired = true; 

 
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
    // second は消したやつ
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
    SplayNode *insert_sub(SplayNode *NODE , SplayNode *root , bool paired_key = false){
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
    
 
    public:
 
 
    MyMultiSet(){}
    int size(){
        if(Root == nullptr)return 0;
        return Root->SubTreeSize;
    }
 
    // 木全体のindex番目のNode(値渡し)を取得 (0-index)
    SplayNode get(int index){
        if(index<0 || index>=size())assert(0);
        Root = getNode(index,Root);
        return *(Root);
    }
 
 
 
    // Keyのみを指定して挿入 ( Value は指定しないので未定義 )
    void insert( T key_ ){
        // 未定義の Value が追加されたので、今後 (Key,Value) の pair に注目する操作が行えなくなる
        _paired = false;
        Root = insert_sub(new SplayNode(key_,T2(0)) ,Root , false);
        return;
    }
 
 
 
    // KeyとValueを指定して挿入
    void insert_pair( T key_ , T2 val_){
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
    void erase(T key_){
        int it = find(key_);
        if(it!=-1)Delete(it);
        return;
    }
 
    /*
        (key_,val_)を持つ頂点を一つ削除
    */
    void erase_pair(T key_ , T2 val_){
        assert("insert() を行ってからは Value にアクセスできません。" != "" && _paired);
        int it = find_pair(key_ , val_);
        if(it!=-1)Delete(it);
        return;
    }
 

    // Key が x 未満の頂点の個数を返す    
    int lower_bound(T x){
        if(size() == 0)return 0;
        SplayNode* nd = new SplayNode(x,T2(0));
        std::pair<SplayNode*,int> tmp = bound_sub(nd,Root,true,0);
        Root = tmp.first;
        delete nd;
        return tmp.second;
    }
 
     
    // (Key , Value)に関して、a < NODE(x,y)であるような頂点の個数を返す
    int lower_bound_pair(T x , T2 y){
        assert("insert() を行ってからは Value にアクセスできません。" != "" && _paired);
        if(size() == 0)return 0;
        SplayNode* nd = new SplayNode(x,y);
        std::pair<SplayNode*,int> tmp = bound_sub(nd,Root,true,1);
        Root = tmp.first;
        delete nd;
        return tmp.second;
    }
 
 
    // Key が x 以下の頂点の個数を返す
    int upper_bound(T x){
        if(size() == 0)return 0;
        SplayNode* nd = new SplayNode(x,T2(0));
        std::pair<SplayNode*,int> tmp = bound_sub(nd,Root,false,0);
        Root = tmp.first;
        delete nd;
        return tmp.second;
    }
 
    // (Key , Value)に関して、a <= NODE(x,y)であるような頂点の個数を返す
    int upper_bound_pair(T x , T2 y){
        assert("insert() を行ってからは Value にアクセスできません。" != "" && _paired);
        if(size() == 0)return 0;
        SplayNode* nd = new SplayNode(x,y);
        std::pair<SplayNode*,int> tmp = bound_sub(nd,Root,false,1);
        Root = tmp.first;
        delete nd;
        return tmp.second;
    }

    // Key == xであるような頂点のindex (複数あるなら最初のもの) を返す (0-index)。なければ-1を返す
    int find(T x){
        if(size() == 0)return -1;
        if(upper_bound(x) - lower_bound(x) <= 0)return -1;
        else return lower_bound(x);
    }

    // (Key,Value) == (x,y)であるような頂点の index (複数あるなら最初のもの) を返す (0-index)。なければ-1を返す
    int find_pair(T x , T2 y){
        assert("insert() を行ってからは Value にアクセスできません。" != "" && _paired);
        if(size() == 0)return -1;
        if(upper_bound_pair(x,y) - lower_bound_pair(x,y) <= 0)return -1;
        else return lower_bound_pair(x,y);
    }
    

    /*
        小さい方からl番目以上、r番目未満の頂点のKeyの和
    */
    T RangeKeySumQuery(int l , int r){
        l = max(0,l);
        r = min(size(),r);
        if(l>=r)assert(0);
        std::pair<SplayNode*,SplayNode> tmp = RangeNode_sub(l,r,Root);
        Root = tmp.first;
        return tmp.second.Sum_key;
    }
 
    T2 RangeValueSumQuery(int l , int r){
        assert("insert() を行ってからは Value にアクセスできません。" != "" && _paired);
        l = max(0,l);
        r = min(size(),r);
        if(l>=r)assert(0);
        std::pair<SplayNode*,SplayNode> tmp = RangeNode_sub(l,r,Root);
        Root = tmp.first;
        return tmp.second.Sum_val;
    }

    T2 RangeValueMinQuery(int l , int r){
        assert("insert() を行ってからは Value にアクセスできません。" != "" && _paired);
        l = max(0,l);
        r = min(size(),r);
        if(l>=r)assert(0);
        std::pair<SplayNode*,SplayNode> tmp = RangeNode_sub(l,r,Root);
        Root = tmp.first;
        return tmp.second.Min_val;
    }
    T2 RangeValueMaxQuery(int l , int r){
        assert("insert() を行ってからは Value にアクセスできません。" != "" && _paired);
        l = max(0,l);
        r = min(size(),r);
        if(l>=r)assert(0);
        std::pair<SplayNode*,SplayNode> tmp = RangeNode_sub(l,r,Root);
        Root = tmp.first;
        return tmp.second.Max_val;
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




