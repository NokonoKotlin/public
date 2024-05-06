





/*
    区間に等差数列を加算 (Add) したり代入 (update) したりするやつ。
    等差数列 ax + b とは、初項 b , 公差 a の等差数列とする。よって、x の範囲は x ≧ 0
    update は未 verify だけど多分大丈夫でしょう。
*/
template<class T>
class ArithmeticProgression{
    private:
    struct SplayNode{
        SplayNode *parent = nullptr;//親ノード
        SplayNode *left = nullptr;//左の子ノード
        SplayNode *right = nullptr;//右の子ノード
        T Value;
        int SubTreeSize = 1;//部分木のサイズ. 1 は自分自身の分

        pair<bool,pair<T,T> > lazy_add ={false , {T(),T()}};
        void set_lazyAdd(const T& a,const T& b){
            if(this->lazy_add.first){
                this->lazy_add.second = { a  + this->lazy_add.second.first , this->lazy_add.second.second + b};
            }else this->lazy_add = {true , {a,b}};
        }

        // 遅延中の区間 等差数列 代入クエリ (遅延しているか , 作用)
        pair<bool,pair<T,T>> lazy_update = {false,{T(),T()}};
        void set_lazyUpdate(const T& a,const T& b){
            this->lazy_update.first = true;
            this->lazy_update.second= { a, b };
            this->lazy_add.first = false;//これ以前の加算クエリは無効化
        }
        
        SplayNode(){}
        SplayNode(T val){
            Value = val;
            update();
        }


        void rotate(){
            if(this->parent->parent){
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

        int state(){
            if(this->parent == nullptr)return 0;
            if(this->parent->left == this)return 1;
            return 2;
        }

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

        void update(){
            this->eval();
            this->SubTreeSize = 1;
            if(bool(this->left)){
                this->left->eval();
                this->SubTreeSize += this->left->SubTreeSize;
            }
            if(bool(this->right)){
                this->right->eval();
                this->SubTreeSize += this->right->SubTreeSize;
            } 
            return;
        }

        void eval(){
            // update が先
            if(this->lazy_update.first){
                ll leftsize = 0;
                if(this->left != nullptr)leftsize += this->left->SubTreeSize;
                T a = this->lazy_update.second.first;
                T b = this->lazy_update.second.second;
                this->Value = a*leftsize + b;
                if(bool(this->left))this->left->set_lazyUpdate(a,b);
                if(bool(this->right))this->right->set_lazyUpdate(a,(leftsize+1)*a+b);
                this->lazy_update.first = false;
            }

            if(this->lazy_add.first){
                ll leftsize = 0;
                if(this->left != nullptr)leftsize += this->left->SubTreeSize;
                T a = this->lazy_add.second.first;
                T b = this->lazy_add.second.second;
                this->Value += a*leftsize + b;
                if(bool(this->left))this->left->set_lazyAdd(a,b);
                if(bool(this->right))this->right->set_lazyAdd(a,(leftsize+1)*a+b);
                this->lazy_add.first = false;
            }
        }
    };


    protected:
    SplayNode *Root = nullptr;

   
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

    SplayNode *merge(SplayNode *leftRoot , SplayNode *rightRoot){
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


    SplayNode *insert_sub(int index , SplayNode *NODE , SplayNode *root){
        NODE->update();
        if(bool(root) == false)return NODE;
        std::pair<SplayNode*,SplayNode*> Trees = split(index,root);
        return merge(merge(Trees.first,NODE),Trees.second);
    }

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

    SplayNode* RangeArithmeticAdd_sub(int l , int r , T A , T B , SplayNode* root){
        std::pair<SplayNode*,SplayNode*> tmp = split(r,root);
        SplayNode* rightRoot = tmp.second;
        tmp = split(l,tmp.first);
        tmp.second->set_lazyAdd(A,B);
        return merge(merge(tmp.first, tmp.second),rightRoot);
    }

    SplayNode* RangeArithmeticUpdate_sub(int l , int r , T A , T B, SplayNode* root){
        std::pair<SplayNode*,SplayNode*> tmp = split(r,root);
        SplayNode* rightRoot = tmp.second;
        tmp = split(l,tmp.first);
        tmp.second->set_lazyUpdate(A,B);
        return merge(merge(tmp.first, tmp.second),rightRoot);
    }


    public:
    // コピー/ムーブ を禁止 
    void init(){Root = nullptr;}
    void release(){while(size() > 0)Delete(0);}
    ArithmeticProgression(){init();}
    ~ArithmeticProgression(){release();}

    ArithmeticProgression(const ArithmeticProgression<T>& A) = delete ;
    ArithmeticProgression& operator = ( const ArithmeticProgression<T>& A) = delete ;
    ArithmeticProgression ( ArithmeticProgression<T>&& A){assert(0);}
    ArithmeticProgression& operator = ( ArithmeticProgression<T>&& A){assert(0);}

    int size(){
        if(Root == nullptr)return 0;
        return Root->SubTreeSize;
    }
    private: 
    void insert(int index , SplayNode *NODE){
        if(index<0 || index> size())assert(0);
        Root = insert_sub(index , NODE , Root);
    }
    public:

    // 左から i 番目のNodeのコピーを取得 (0-index)
    // ただし、値の取得が目的なので right などの隣接頂点へのアクセスを封印する
    SplayNode get(int i){
        assert(0 <= i && i < size());
        Root = getNode(i,Root);
        SplayNode res = (*Root);
        res.right = res.left = res.parent = nullptr;// 隣接するノードへのアクセスを封じる
        return res;
    }

    // index 番目を消去
    void Delete(int index){
        assert(0 <= index && index < size());
        std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(index,Root);
        Root = tmp.first;
        delete tmp.second;
    }
    // x 番目を y にする
    void update_val(int x , T y){
        assert(0 <= x && x < size());
        Root = getNode(x,Root);
        if(bool(Root)==false)return;
        Root->Value = y;
        Root->update();
    }

    // 半開区間 [l,r) の左から x 番目 (0-index) に、Ax + B を加算する
    void RangeArithmeticAdd(int l , int r , T A , T B){
        assert(0 <= l && l < r && r <= size()) ;
        Root = RangeArithmeticAdd_sub(l,r,A,B,Root);
        return;
    }

    // 半開区間 [l,r) の左から x 番目 (0-index) を、Ax + B にする
    void RangeArithmeticUpdate(int l , int r , T A , T B){
        assert(0 <= l && l < r && r <= size()) ;
        Root = RangeArithmeticUpdate_sub(l,r,A,B,Root);
        return;
    }


    
    // 一列に表示
    void Debug(){std::cerr<<"DEBUG:" << std::endl;for( int i = 0 ; i < size() ; i++)std::cerr<< get(i).Value << " ";std::cerr<< std::endl;}
    // 末尾に追加
    void push_back(T val){insert(size() , new SplayNode(val));return;}
    //先頭に追加
    void push_front(T val){insert(0 , new SplayNode(val));return;}
    // index 番目に挿入
    void push(int index , T val){insert(index , new SplayNode(val));return;}
    // 末尾を取得
    SplayNode back(){assert(size()>0);return get(size()-1);}
    // 先頭を取得
    SplayNode front(){assert(size()>0);return get(0);}
    // 末尾を削除
    void pop_back(){assert(size()>0);Delete(size()-1);}
    // 先頭を削除
    void pop_front(){assert(size()>0);Delete(0);}
    // read-only でアクセス
    T operator [](int index){return get(index).Value;}
       
};

