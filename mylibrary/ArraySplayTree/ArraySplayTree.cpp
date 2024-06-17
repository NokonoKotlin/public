#include "./../template/template.hpp"













/*
    - 数列型の Splay Tree 
    - 頂点にアクセスしたら、その頂点をsplay (& update) するのを忘れないで！！
    - SplayNodeというデータ構造に必要なデータを持たせ、m_Root から探索することでアクセスする。
    - SplayNode にモノイド積などの集約データを持たせる(Sum,Min など) 
    - 基本的に、機能の変更をしたい場合は SplayNode のメンバや update 方法を編集することになる
    |
    - rootを頂点とする部分木において
        |- split(l,root) := l 番目で分割
        |- merge(l_root , r_root) := 頂点がl_root ,r_rootであるような部分木を連結
        |- insert_sub(ind,NODE,root) := ind番目にNODEが挿入される
        |- Delete_sub(ind ,root) := ind番目を削除(返り値はfirstが新しいroot , secondが削除したNode)
    |
    - これらを駆使し、様々な操作を実装可能
    - : _sub で実装される関数は、スプレー木を管理するために必要な最低限の機能で、クラス内部での処理のための関数。
    - : _sub 関数は、基本的な操作のみを担当するので、ユーザー向けの機能は、_subでない関数で実装する。
    - (重要) : _sub 関数の中身は、_sub 関数だけで完結させる!!!!!!!
    |
    - find(x,c) : a_i = x である様な i を c 個まで検索する。
        |- 以下のタイミングで、x を持つノードのポインタ pos[x] を編集する。
        |- insert するとき / Delete するとき /update_valするとき
        |- Delete(i)の後、削除したNodeのisThisDeletedをtrueにしている
        |- (重要) 前提として Splay Tree の連結性が必要 ( Splay Tree を分割してから find() を使うのは未定義動作 )
        |- find() を使うには、テンプレートで _find_ = true を指定する (指定せずに使うと RE)
    |
    - 区間クエリ
        |- 区間のモノイド積ができる (総和,Min など)
        |- 区間作用について
            |- _find_ が true なのに更新をかけると RE になる。
            |- 半開区間 [l,r) の要素を reverse することができる (区間反転)
                |- _find_ が true の場合の計算量解析不明 , 多分 O(NQ) になるケースもある
            |- 区間内の要素に一律に更新をかけることができる
                |- 区間内の要素に対して、代入/アフィン変換/chmax など
    |
    - 計算量について :
        |- 遅い時はいらない集約や遅延評価を消す。
        |- eval() しなくていい時は、eval() の中身を全消ししたり、eval() を呼び出さなくするとかなり速くなる
    |
    - ライブラリの設計 : 
        |- 内部処理 (Node の splay や _sub がついた関数) を信頼する  
        |- 関数に渡す SplayNode や、関数から返される SplayNode は全て update (& eval) されている必要がある
        |- 抽象化はせずに、モノイド積や遅延評価作用を例示することで、書き換えを容易にする。  
            |- Sum , Min , Max , RangeUpdate , RangeAffine , RangeReverse で特殊化している
            |- 不必要なモノイド積や作用の記述を消すと速くなる  
        |- 特殊化されたモノイド積や作用は以下に書き込まれているので、書き換えるときは主に以下を書き換える
            |- Node のメンバ (lazy_affine や set_LazyAffine)
            |- Node の update ,eval の処理
            |- RangeHoge_sub 関数で、Node から 取り出す/遅延評価を載せる
        |- T を自作クラス (modint) などにするときは、pos メンバのキーを long long などにしておく 
            (modint は long long にキャストできる設計なので)
        |- merge と split が++全く++反対の操作であることで、計算量を削減できる
            |- merge(lef,rig) : rig の左の子に lef をくっつける
            |- split(i,ROOT) : i 番目のノードの左の辺をカットして (lef,rig) に分割する
        |- 内部実装の merge , split の使用法は最適化されていることに注意、変に変更したら遅くなる
            |- split は右から、merge は左から行うと良いです
        |
        |- ヒープ上のオブジェクトのポインタを扱う関係で、コピー/ムーブを禁止する。
            |- 宣言後に代入するときは、copy(X) で、X の中身を再度生成して自身に格納する。
            |- 平衡二分木は、アクセスするだけでも構造を変えるので、const 修飾できない。
                |- 右辺値用と左辺値用それぞれの copy を定義する。
        |- get(i) や getRange(l,r) で取得するノードは、要素取得の用途での使用のみを想定しているので、
            / 隣接するノードへのアクセスを封印したコピーを返すようにしてある。
*/
template<class T , bool _find_=false>
class ArraySplayTree{
    private:
    
    struct SplayNode{
        SplayNode *parent = nullptr;//親ノード
        SplayNode *left = nullptr;//左の子ノード
        SplayNode *right = nullptr;//右の子ノード
        T Value;//値(普通、頂点には値や重みが割り当てられることが多い)
        T Min,Max,Sum;//部分木のうち、値の最大、最小、和
        bool isThisDeleted = false;//このノードが、木から削除されたか( find() で使用する。)
        int SubTreeSize = 1;//部分木のサイズ. 1 は自分自身の分

        /*
            遅延評価が存在するかをフラグで持っておき、この頂点にアクセスされた時にtrueなら評価を行い、左右の子に伝播させる
            reverse : 左右の子を反転させる(左右の子に伝播させることで、結果的に対応する区間全てが反転する)
            affine : 部分木が対応する区間のValueすべてに A 掛けた後 B 足す
            update : 部分木が対応する区間のValueをすべて x にする
        */

        // 反転 flag と、反転 flag のセット
        bool reverse_flag_lazy = false;
        void set_lazyReverse(){this->reverse_flag_lazy = !(this->reverse_flag_lazy);}

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
        SplayNode(T val){
            Value = val;
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
                // 親の state をすでに計算しているので、親の親 → 親 → 自分に reverse の評価を降ろせている
                if( this->parent->state() == this->state() )this->parent->rotate();
                else this->rotate();
                this->rotate();
            }
            this->update();// while が呼ばれなかった場合のため
            return;
        }

        // Nodeの持っている情報をupdateする
        void update(){
            this->eval();
            // (注意) : 結合する代入演算子を持っているクラスである必要がある
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
        /*
            この頂点にアクセスされるたびに、各flagがtrueなら遅延させていた処理を実行し、左右の子にflagを伝播する
                - reverse : 自分の左右の子を入れ替えて、左右の子に遅延伝播(最終的に区間全体が反転する)
                - affine : 自分自身以下の部分木のValueをアフィン変換する (A 掛けて B 足す) 
                - update : 自分自身以下の部分木のValueを一律更新(再代入)する
                - Min,Max,Sum なども、遅延している値から計算する
            更新クエリは update が優先されるので、update 以前に溜まった affine の遅延評価は消えている(set_lazyupdateを参照)
            よって、update の遅延から評価する
        */
        void eval(){
            if(this->reverse_flag_lazy){
                swap(this->left , this->right);
                //下に伝播
                if(bool(this->left))this->left->set_lazyReverse();
                if(bool(this->right))this->right->set_lazyReverse();
                this->reverse_flag_lazy = false;
            }

            //updateクエリの処理が先
            if(this->lazy_update.first){
                //eval の実装を工夫することで色んな区間更新ができます(以下の if は value と lazy_update のうち大きい方で置き換える更新クエリ)
                //if(this->Value < lazy_update.second)this->Value = this->lazy_update.second;
                //ただし、この場合 lazy_affine の処理や、Sum,Min,Max などが未定義(自分で定義してね)(使えなくなる場合もある)
                this->Value = this->Min = this->Max = this->lazy_update.second;
                this->Sum = (this->lazy_update.second)*(this->SubTreeSize);
                if(bool(this->left))this->left->set_lazyUpdate(this->lazy_update.second);
                if(bool(this->right))this->right->set_lazyUpdate(this->lazy_update.second);
                //伝播したらフラグを折る
                this->lazy_update.first = false; 
            }
            if(this->lazy_affine.first){
                this->Value = this->lazy_affine.second.first * this->Value + this->lazy_affine.second.second;
                this->Min = this->lazy_affine.second.first * this->Min + this->lazy_affine.second.second;
                this->Max = this->lazy_affine.second.first * this->Max + this->lazy_affine.second.second;
                this->Sum = this->lazy_affine.second.first * this->Sum + this->SubTreeSize*this->lazy_affine.second.second;
                if(bool(this->left))this->left->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                if(bool(this->right))this->right->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                //伝播したらフラグを折る
                this->lazy_affine.first = false;
            }
        }
    };



    // rootを根とする SplayTree において、求めたいindexまで降りていく(左側の部分木サイズを参照する)
    SplayNode *get_sub(int index , SplayNode *root){
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
        rightRoot = get_sub(0,rightRoot);
        rightRoot->left = leftRoot;
        leftRoot->parent = rightRoot;
        rightRoot->update();
        return rightRoot;
    }
    
    // root を根とする Splay木 の i 番目のノードの左の辺を無くし、2 つの root (lef,rig) を返す
    std::pair<SplayNode*,SplayNode*> split(int leftnum, SplayNode *root){
        if(leftnum<=0)return std::make_pair(nullptr , root);
        if(leftnum >= root->SubTreeSize)return std::make_pair(root, nullptr);
        root = get_sub(leftnum , root);
        SplayNode *leftRoot = root->left;
        SplayNode *rightRoot = root;
        if(bool(rightRoot))rightRoot->left = nullptr;
        if(bool(leftRoot))leftRoot->parent = nullptr;
        leftRoot->update();
        rightRoot->update();
        return std::make_pair(leftRoot,rightRoot);
    }
    
    // rootを根とする SplayTree において、indexの場所を切り離して、NODEを追加して元に戻す(merge)
    // NODE は update されている必要がある
    SplayNode *insert_sub(int index , SplayNode *NODE , SplayNode *root){
        NODE->update();
        if(bool(root) == false)return NODE;
        std::pair<SplayNode*,SplayNode*> Trees = split(index,root);
        return merge(merge(Trees.first,NODE),Trees.second);
    }
                   
    // rootを根とする部分木において、
    // 左から index の場所を削除して、両端をくっつける
    // second は消したやつ (ここではメモリの解放はしない)
    std::pair<SplayNode*,SplayNode*> Delete_sub(int index , SplayNode *root){
        if(bool(root) == false)return std::make_pair(root,root);
        root = get_sub(index,root);
        SplayNode *leftRoot = root->left;
        SplayNode *rightRoot = root->right;
        if(bool(leftRoot))leftRoot->parent = nullptr;
        if(bool(rightRoot))rightRoot->parent = nullptr;
        root->left = nullptr;
        root->right = nullptr;
        root->update();
        return std::make_pair(merge(leftRoot,rightRoot) , root );
    }


    // 半開区間[l , r)の要素のValue全てに A 掛けて、その後 B 足す(区間アフィン変換)
    SplayNode* RangeAffine_sub(int l , int r , T A , T B , SplayNode* root){
        std::pair<SplayNode*,SplayNode*> tmp = split(r,root);
        SplayNode* rightRoot = tmp.second;
        tmp = split(l,tmp.first);
        tmp.second->set_lazyAffine(A,B);
        return merge(merge(tmp.first, tmp.second),rightRoot);
    }

    // 半開区間[l , r)の要素のValue全てをxにする(区間更新クエリ)
    SplayNode* RangeUpdate_sub(int l , int r , T x , SplayNode* root){
        std::pair<SplayNode*,SplayNode*> tmp = split(r,root);
        SplayNode* rightRoot = tmp.second;
        tmp = split(l,tmp.first);
        tmp.second->set_lazyUpdate(x);
        return merge(merge(tmp.first, tmp.second),rightRoot);
    }

    // 半開区間[l , r)を反転させる
    SplayNode* reverse_sub(int l , int r , SplayNode* root){
        std::pair<SplayNode*,SplayNode*> tmp = split(r,root);
        SplayNode* rightRoot = tmp.second;
        tmp = split(l,tmp.first);
        tmp.second->set_lazyReverse();
        return merge(merge(tmp.first, tmp.second),rightRoot);
    }



    protected:

    // 常に m_Root が根であるようにする (merge した後は m_Root に代入するなど)
    SplayNode *m_Root = nullptr;
    map<T , vector<SplayNode*> > pos; 
    vector<int> checked;
    int find_called_time = 0;//find 関数を呼び出した回数をメモ
    /*
        - pos[x] := 値が x であるようなSplayNodeのポインタ達。insert , update_val が呼ばれた時に変更する
            |- Delete などで削除があって pos から即座に削除はせず、find() で中身を見るついでに削除する
            |- pos[x]に 同じノード(ポインタ)が複数存在する場合や、x 以外の値を持つノードのポインタがある場合がある。
        - find(x) で pos[x] の中身を見るついでに、複数存在するものや x 以外のものを削除する
            |- 同じノードが複数存在するかは、checked に find_called_time をカウンターとして書き込むことで判定する。
            |- find() を呼び出すたびに find_called_time を変更することで、checked を使い回すことができる。
        - 同じノードかどうかは splay() した後の左側の部分木サイズでしか判定しないので、Splay Tree を 2 つ以上に分割した状態で呼び出されると未定義動作
    */
    
    void release(){
        // pos は放置して OK (ポインタしか格納されていないので多重解放でもない)
        while(m_Root != nullptr){
            std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(0,m_Root);
            m_Root = tmp.first;
            delete tmp.second;
        }
    }
    // 初期状態にする (copy() などで使用)
    void init(){
        m_Root = nullptr;
        find_called_time = 0;
        checked.clear();
        pos.clear();
    }
    // 先頭要素のアドレスを取得。
    const SplayNode* const begin(){
        if(size() == 0)return nullptr;
        m_Root = get_sub(0,m_Root);
        return m_Root;
    }
    public:

    ArraySplayTree(){init();}
    ~ArraySplayTree(){release();}
    // 複雑な挙動を回避するので、コンストラクタによるコピー/ムーブを一律に禁止する。コピーは copy() 関数に任せる
    ArraySplayTree(const ArraySplayTree<T,_find_> &x) = delete ;
    ArraySplayTree& operator = (const ArraySplayTree<T,_find_> &x) = delete ;
    ArraySplayTree ( ArraySplayTree<T,_find_>&& x){assert(0);}
    ArraySplayTree& operator = ( ArraySplayTree<T,_find_>&& x){assert(0);}
    // 宣言後に代入する用に、コピーコンストラクタとは別のコピー関数を用意 
    // コピー元 へのアクセスは コピー元 の構造を変化させるので、const をつけて右辺値を取得できない。
    // よって、右辺値 copy と左辺値の copy を両方実装する
    void copy(ArraySplayTree<T,_find_>&& x){
        // 自己代入禁止
        if(this->begin() == x.begin())return;// 先頭要素のアドレスだけ見れば、同一オブジェクトかを判定できる
        
        release();
        init();
        for(int i = 0 ; i < x.size() ; i++ )this->push_back(x[i]);
        // pos や checked は push_back で構築される。
        this->find_called_time = x.find_called_time;// ここは別に必要ないけど、不変性を尊重
    }
    void copy(ArraySplayTree<T,_find_>& x){
        // 自己代入禁止
        if(this->begin() == x.begin())return;// 先頭要素のアドレスだけ見れば、同一オブジェクトかを判定できる
        
        release();
        init();
        for(int i = 0 ; i < x.size() ; i++ )this->push_back(x[i]);
        // pos や checked は push_back で構築される。
        this->find_called_time = x.find_called_time;// ここは別に必要ないけど、不変性を尊重
    }
    int size(){
        if(m_Root == nullptr)return 0;
        return m_Root->SubTreeSize;
    }


    // 左から i 番目のNodeのコピーを取得 (0-index)
    // ただし、値の取得が目的なので right などの隣接頂点へのアクセスを封印する
    SplayNode get(int i){
        assert(0 <= i && i < size());
        m_Root = get_sub(i,m_Root);
        SplayNode res = (*m_Root);
        res.right = res.left = res.parent = nullptr;// 隣接するノードへのアクセスを封じる
        return res;
    }


    // [l,r) に対応する部分木の根のコピーを返す。ただし、隣接ノードへのアクセスを封じる。
    // 使用用途はモノイド積の取得に限定 (コピーなので、オリジナルの現在の値とは関係なく [l,r) のデータが格納されている)
    SplayNode GetRange(int l , int r){
        assert(0 <= l && l < r && r <= size());
        std::pair<SplayNode*,SplayNode*> tmp = split(r,m_Root);
        SplayNode* rightRoot = tmp.second;
        tmp = split(l,tmp.first);// 部分木を取り出す。
        SplayNode res = (*tmp.second);// 取り出した部分木の根をコピーする。
        res.right = res.left = res.parent = nullptr;// 隣接するノードへのアクセスを封じる
        m_Root = merge(merge(tmp.first,tmp.second),rightRoot);
        return res;
    }


    // index 番目に x を挿入
    void insert(int index , T x){
        assert(0 <= index && index <= size());
        SplayNode* NODE = new SplayNode(x);
        m_Root = insert_sub(index , NODE , m_Root);
        if(_find_ == false)return;
        pos[NODE->Value].push_back(NODE);//位置検索mapに追加
        if(checked.size() <= size())checked.resize(int(checked.size())+1 , -1);// アロケータを信じる
        return;
    }
    
    // index番目のNodeを消去
    void Delete(int index){
        if(index<0 || index >= size())assert(0);
        std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(index,m_Root);
        m_Root = tmp.first;
        if(_find_)tmp.second->isThisDeleted = true;
        else delete tmp.second;
        return;
    }
    // x 番目のvalueをyにする
    void update_val(int x , T y){
        if(x<0 || x >= size())assert(0);
        m_Root = get_sub(x,m_Root);
        if(bool(m_Root)==false)return;
        m_Root->Value = y;
        m_Root->update();// ノードを編集したら update!!
        if(_find_ == false)return;
        pos[y].push_back(m_Root);
        return;
    }

    // [l,r)を反転
    void reverse(int l , int r){
        if(_find_)assert("find() を想定する用途で区間更新を呼ぶことはできません" == "");
        l = max(0,l);
        r = min(size(),r);
        if(l>r)assert(0);
        if(r-l <= 1)return;
        m_Root = reverse_sub(l,r,m_Root);
        return;
    }

    // 木全体の[l,r)に区間アフィン変換 (区間内の要素全てに、A 掛けて B 足す)
    void RangeAffine(int l , int r , T A , T B){
        if(_find_)assert("find() を想定する用途で区間更新を呼ぶことはできません" == "");
        l = max(0,l);
        r = min(size(),r);
        if(l>=r)assert(0);
        m_Root = RangeAffine_sub(l,r,A,B,m_Root);
        return;
    }

    // 木全体の[l,r)に区間加算(区間内の要素全てに+x) ((1,x) のアフィン変換と同値)
    void RangeAdd(int l , int r , T x){
        if(_find_)assert("find() を想定する用途で区間更新を呼ぶことはできません" == "");
        RangeAffine(l,r,T(1),x);// 整数型の 1 を使うので注意
        return;
    }
    // 木全体の[l,r)に区間更新(区間内の要素全てをxにする)
    void RangeUpdate(int l , int r , T x){
        if(_find_)assert("find() を想定する用途で区間更新を呼ぶことはできません" == "");
        l = max(0,l);
        r = min(size(),r);
        if(l>=r)assert(0);
        m_Root = RangeUpdate_sub(l,r,x,m_Root);
        return;
    }


    // 数列内の x の位置を vector<int> に入れて返す。
    // countを指定すると、指定した個数以下の個数(選び方は未定義)を返す
    vector<int> find(T x , int count = 100000000){
        assert(_find_ == true);
        if(size() == 0)return vector<int>(0);
        find_called_time++;//呼び出し回数をインクリメント
        vector<int> res(0);
        
        for(int i = int(pos[x].size()) -1 ; i >= 0 ; i--){// erase があるので、後ろから見ていく
            if(res.size()>=count)break;
            SplayNode *Nd = pos[x][i];
        
            if(Nd == nullptr)pos[x].erase(pos[x].begin()+i);
            else if(Nd->isThisDeleted)pos[x].erase(pos[x].begin()+i);//削除済みを弾く
            else if(Nd->Value != x)pos[x].erase(pos[x].begin()+i);//値の変更ずみを弾く
            else{ // この時点で、 Nd は実在するノードであることが確定している
                Nd->splay();
                m_Root = Nd;
                int index = 0;
                if(bool(Nd->left))index+=Nd->left->SubTreeSize;
                if(checked[index] == find_called_time)pos[x].erase(pos[x].begin()+i);//同じ NODE へのポインタが複数存在する場合はpop
                else res.push_back(index);//見つけた場合、答えに追加して、checkedでメモ    
                checked[index] = find_called_time;// この index のノードは既に見たことを意味する
            }
        }
        return res;
    }

    
    // 一列に表示
    void Debug(){std::cerr<<"DEBUG:" << std::endl;for( int i = 0 ; i < size() ; i++)std::cerr<< get(i).Value << " ";std::cerr<< std::endl;}
    // 末尾に追加
    void push_back(T val){insert(this->size() , val);return;}
    //先頭に追加
    void push_front(T val){insert(0 , val);return;}
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







