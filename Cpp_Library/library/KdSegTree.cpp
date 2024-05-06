#include "MyTemplate.hpp"






/*
    多次元セグ木(1次元も含む)
    コンストラクタで vector<int> size_ と T init_ を渡す。
    各次元のサイズが size_[0],size_[1] , ... , size_[d-1] の d 次元グリッド(マス目)を表現し、初期値が init となる

    基本的に座標を表すのは d 次元座標で、d 次元座標上の矩形を表す際は、その矩形の対角の座標 A , B を用いて表す。
    その矩形内のマスを対象に、矩形クエリを行う

    テンプレートには 
    データの型        : T 
    矩形マージ演算 : op(a,b)
    op(a,b) の単位元 : e()
    一点の値 x を更新する : upd(&x , val) を渡す

    デフォルトは 一点加算/矩形和 の場合
*/
template<class T,
T (*op_)(T,T) = [](T a , T b){return a+b;}, 
T (*e_)() = [](){return T(0);},
T (*upd_)(T& , T) = [](T& a , T b){return a+=b;}>
class NdSegTree{

    private:

    vector<int> size; // [i]:= このセグ木以下の次元で i 次元目のサイズ (処理で使うのはsize[0]のみだが、アクセス可能にしておく)
    vector<NdSegTree<T , op_ , e_ , upd_ > > node;
    T data;
    int d; // このセグメント木の次数

    const static int SegTreeRoot = 1;

    // sz を sz 以上の、2 の冪乗の値にフォーマットする
    template<typename Integer>
    Integer resize(Integer& sz){
        Integer p = 1;
        while(p < sz)p*=2;
        return (sz = p);
    }


    template<typename Integer>
    void init(vector<Integer> size_v,T init_){   
        d = int(size_v.size());
        if(d<= 0){// 0 次元セグ木 は、単なる T 型(データ単体)
            data = init_;
        }else{
            size.resize(d);
            for(int& s : size_v) s = resize(s);
            for(int i = 0 ; i < d ; i++ )size[i] = size_v[i];
            size_v.erase(size_v.begin());// 下の次元に
            node.clear();
            node.resize(size[0]*2, NdSegTree< T , op_ , e_ , upd_ >(size_v,init_));
        }
    } 

    // 座標 (A[0],A[1],...,A[d-1]) の値を val に変更する (特に、A[deg] 以降に注目)
    template<typename Integer>
    void set_sub(vector<Integer> &A, T val , int deg = 0){
        if(d <= 0){ // 0 次元の時、
            data = val;
            return;
        }
        int index = A[deg];
        assert(index >= 0 && index < size[0]);
        index += size[0];

        node[index].set_sub(A,val,deg+1);// 下の次元に投げる

        // 上段の更新は、同次元内で完結する
        while(index/2 >= SegTreeRoot ){
            index/=2;
            // N 次のセグ木内で更新をかけるのは N-1 次元要素であることに注意
            node[index].set_sub(A, op_(node[index*2].get_sub(A,deg+1) , node[index*2+1].get_sub(A,deg+1)) , deg + 1);
        }
        
    }

    // 座標 (A[0],A[1],...,A[d-1]) の値を、val でupdateする 
    template<typename Integer>
    void update_sub(vector<Integer> A , T val){
        T x = get_sub(A);
        upd_(x,val);// x に更新をかける
        set_sub(A,x);
    }

    // 座標 (A[0],A[1],...,A[d-1]) と (B[0],B[1],...,B[d-1]) を対角とする矩形内の値の op を取得 (ただし、各次元の区間は半開区間である) , A[deg] , B[deg] 以降に注目
    template<typename Integer>
    T query_sub(vector<Integer>& A , vector<Integer>& B , int deg = 0 , int SubRoot = SegTreeRoot , int SubTreeLeft = 0 , int SubTreeRight = -1 ){
        if(d <= 0)return data; // 0 次元はデータ単体しか持たないので

        if(SubRoot == SegTreeRoot)SubTreeRight = size[0];

        int lef = A[deg];
        int rig = B[deg];
        if(lef > rig)swap(lef,rig);
        if(lef == rig)assert(0);
        T res = e_();

        //どれかの一つのステートしか実行されないので、A , B を同期しなくていい
        if(rig <= SubTreeLeft){//区間が被っていない
            return e_();
        }else if(lef >=SubTreeRight){//区間が被っていない
            return e_();
        }else if( lef <= SubTreeLeft && SubTreeRight <= rig ){
            //SubRootを根とする部分木の範囲が、取得したい範囲に完全に含まれている-> node[SubRoot]の値をreturn
            return node[SubRoot].query_sub(A , B , deg+1);
        }else{//SubRootの右の根と左の根に降りて、再度調べる
            int border = (SubTreeLeft+SubTreeRight)/2;
            return op_( query_sub(A,B,deg,SubRoot*2,SubTreeLeft,border) , query_sub(A,B,deg,SubRoot*2+1,border,SubTreeRight) );
        }
    }

    // 座標 (A[0],A[1],...,A[d-1]) の値を取得  (特に、A[deg] 以降に注目)
    template<typename Integer>
    T get_sub(vector<Integer>& A , int deg = 0){
        if(d <= 0)return data;
        int index = A[deg];
        return node[index+size[0]].get_sub(A,deg+1); // 次の次元以降に投げる
    }


    public:
    
    NdSegTree(){}

    template<typename Integer>
    NdSegTree(vector<Integer> size_v , T init_){d = size_v.size() ; init(size_v,init_);}
    ~NdSegTree(){}


    // 座標 (A[0],A[1],...,A[d-1]) の値を取得
    template<typename Integer>
    T get(vector<Integer> A){
        return get_sub(A);
    }  

    // 座標 (A[0],A[1],...,A[d-1]) と (B[0],B[1],...,B[d-1]) を対角とする矩形に対して、マージ結果を計算
    template<typename Integer>
    T rect_query(vector<Integer> A , vector<Integer> B){
        return query_sub(A,B);
    }

    // 座標 (A[0],A[1],...,A[d-1]) の値を val に変更
    template<typename Integer>
    void set(vector<Integer> A , T  val){
        return set_sub(A , val);
    }  

    // 座標 (A[0],A[1],...,A[d-1]) の値に、upd(val) で更新をかける
    template<typename Integer>
    void update(vector<Integer> A , T  val){
        return update_sub(A, val);
    }  


    // グリッドの i 番目の次元の幅を返す(d 次元目は、0 次元のグリッドと捉えて 0 を返す)
    int width(int i){
        if(i< 0 || i>d)assert(0);
        if(i == d)return 0;
        else return size[i];
    }

};


// N 次元セグ木の使用例 (2 次元グリッドの一点加算、矩形和)
namespace PointAddRectSum{
    // 書き換えて使用するときも、各演算のフォーマット(引数の順番など)は変えないで!!!

    using TypE = long long;
    TypE OP(TypE a , TypE b){return a+b;} // 二つの引数のマージを返す
    TypE E(){return TypE(0);}
    TypE UPD(TypE& a , TypE b){return a += b;}// 第一引数を update する

    // 3 次元とかにしたい場合は引数に z 軸の座標を加えるだけ
    // 処理は全部 NdSegTree に丸投げする
    class _2dSegTree{
        private:
        int H , W;
        NdSegTree<TypE , OP , E, UPD> T;
        public:
        // 縦幅,横幅,初期化値
        _2dSegTree(int H_,int W_,TypE init_):H(H_),W(W_){
            vector<int> S = {H_,W_};
            T = NdSegTree<TypE , OP , E, UPD>(S,init_);
        }
        // 上から y 番目、左から x 番目の要素を返す (0-index)
        TypE get(int y , int x){
            vector<int> s = {y,x};return T.get(s);
        }
        // 上から y 番目、左から x 番目に V を代入する (0-index)
        void set(int y , int x , TypE V){
            vector<int> s = {y,x};T.set(s,V);
        }
        // 上から y 番目、左から x 番目にの要素に対して V で update する (UPD() を行う)
        void update(int y , int x , TypE V){
            vector<int> s = {y,x};T.update(s,V);
        }
        // 点(y1,x1) と 点(y2,x2) を対角とする矩形内のマージ値を計算 (全ての要素に OP() をしたもの)
        TypE rect_query(int y1, int x1, int y2, int x2){
            vector<int> s1 = {y1,x1};vector<int> s2 = {y2,x2};
            return T.rect_query(s1,s2);
        }
        int height(){return H;}
        int width(){return W;}
    };

}












/*
    多次元双対セグ木
    N 次元のグリッドの矩形領域に対して更新クエリを行う。
    更新クエリをおこなった区間に対応するセグメントに、更新クエリによる影響を評価値として貯めておく。  
    

    本来双対セグ木が表すグリッドは、矩形更新クエリによってマス目にかかる評価値にのみ注目するものだが、
    しかし、評価値とは別に init_val を単体のデータで保持しておくことで、あたかもグリッドが init_val で初期化してあったかのように振る舞うようにする

    初め、全てののマス目にかかる評価値は e() : 単位元 で初期化される(何も評価がかかっていないことを表す)
        - ある点の値を取得する際は、グリッドの初期値 init_val と、グリッドにかかる評価値の総マージ結果をマージしたものを返す


    コンストラクタで vector<int> size_ と T init_val を渡す。
    各次元のサイズが size_[0],size_[1] , ... , size_[d-1] の d 次元グリッド(マス目)を表現し、グリッドの値は init_val で初期化されている。  


    基本的に座標を表すのは d 次元座標で、d 次元座標上の矩形を表す際は、その矩形の対角の座標 A , B を用いて表す。
    その矩形内のマスを対象に、矩形クエリを行う    


    テンプレートには 
    データの型        : T
    元々の評価値 x に val を反映させる : upd_(&x , val) 
    ある点(マス目)にかかるすべての評価をマージする演算 : op(a,b) 
    評価マージの単位元 : e()


    デフォルトは矩形加算/一点取得
*/
template<class T,
T (*op_)(T,T) = [](T a , T b){return a+b;}, 
T (*e_)() = [](){return T(0);},
T (*upd_)(T& , T) = [](T& a , T b){return a+=b;}>
class DualNdSegTree{
    private:

    vector<int> size; // [i]:= このセグ木以下の次元で i 次元目のサイズ (処理で使うのはsize[0]のみだが、アクセス可能にしておく)
    vector<DualNdSegTree<T , op_ , e_ , upd_ > > node;

    T init_val;// グリッドの初期値(全ての次元で共通、コンストラクタで与えられてからは一歳変化しない)

    T data;// このノードにかかっている評価値 
    

    int d; // このセグメント木の次数
    const static int SegTreeRoot = 1;

    // sz を sz 以上の、2 の冪乗の値にフォーマットする
    template<typename Integer>
    Integer resize(Integer& sz){
        Integer p = 1;
        while(p < sz)p*=2;
        return (sz = p);
    }

    template<typename Integer>
    void init(vector<Integer> size_v){   
        d = int(size_v.size());
        if(d<= 0){// 0 次元セグ木 は、単なる T 型(データ単体)
            data = e_();
        }else{
            size.resize(d);
            for(int& s : size_v) s = resize(s);
            for(int i = 0 ; i < d ; i++ )size[i] = size_v[i];
            size_v.erase(size_v.begin());// 下の次元に
            node.clear();
            node.resize(size[0]*2, DualNdSegTree< T , op_ , e_ , upd_ >(size_v,init_val));
        }
    } 


    // 座標 (A[0],A[1],...,A[d-1]) の値を取得する (特に、A[deg] 以降に注目)
    template<typename Integer>
    T get_sub(vector<Integer> &A, int deg = 0){
        if(d <= 0){ // 0 次元の時、
            return data;
        }
        int index = A[deg];
        assert(index >= 0 && index < size[0]);
        index += size[0];

        T res = node[index].get_sub(A,deg+1);// 下の次元に投げる

        // 上段の評価をマージする
        while(index/2 >= SegTreeRoot){
            index/=2;
            res = op_(res , node[index].get_sub(A,deg+1));
        }
        return res;
    }



    // 座標 (A[0],A[1],...,A[d-1]) と (B[0],B[1],...,B[d-1]) を対角とする矩形内に upd_ を作用させる
    template<typename Integer>
    void update_sub(vector<Integer>& A , vector<Integer>& B, T val , int deg = 0 , int SubRoot = SegTreeRoot , int SubTreeLeft = 0 , int SubTreeRight = -1 ){
        if(d <= 0){// 該当区間内の最低次元要素
            data = upd_(data,val);// 更新クエリをかける
            return;
        }

        if(SubRoot == SegTreeRoot)SubTreeRight = size[0];

        int lef = A[deg];
        int rig = B[deg];
        if(lef > rig)swap(lef,rig);
        if(lef == rig)assert(0);

        if(rig <= SubTreeLeft){//区間が被っていない
            return ;
        }else if(lef >=SubTreeRight){//区間が被っていない
            return ;
        }else if( lef <= SubTreeLeft && SubTreeRight <= rig ){
            //SubRootを根とする部分木の範囲が、取得したい範囲に完全に含まれている
            node[SubRoot].update_sub(A , B , val , deg+1);// このノードが担当している区間に評価をかける
        }else{//SubRootの右の根と左の根に降りて、再度更新
            int border = (SubTreeLeft+SubTreeRight)/2;
            update_sub(A,B,val,deg,SubRoot*2,SubTreeLeft,border) ;
            update_sub(A,B,val,deg,SubRoot*2+1,border,SubTreeRight);
        }
        return;
    }


    public:
    
    DualNdSegTree(){}
    template<typename Integer>
    DualNdSegTree(vector<Integer> size_v , T init_val_) : init_val(init_val_){d = size_v.size() ; init(size_v);}


    // 座標 (A[0],A[1],...,A[d-1]) の値を取得
    template<typename Integer>
    T get(vector<Integer> A){
        return op_(get_sub(A),init_val); // 評価値の総マージ と グリッドの初期値 のマージを計算
    }  

    // 座標 (A[0],A[1],...,A[d-1]) と (B[0],B[1],...,B[d-1]) を対角とする矩形に対して、upd_ を作用させる
    template<typename Integer>
    void rect_update(vector<Integer> A , vector<Integer> B , T val){
        update_sub(A,B,val);
    }


    // グリッドの i 番目の次元の幅を返す(d 次元目は、0 次元のグリッドと捉えて 0 を返す)
    int width(int i){
        if(i< 0 || i>d)assert(0);
        if(i == d)return 0;
        else return size[i];
    }

};







// N 次元セグ木の使用例 (2 次元グリッドの一点取得、矩形加算)
namespace RectAddPointGet{
    // 書き換えて使用するときも、各演算のフォーマット(引数の順番など)は変えないで!!!
    using TypE = long long;
    TypE UPD(TypE& a , TypE b){return a += b;}// 更新クエリ : 元々の評価値 a に、評価値 b を反映させる
    TypE OP(TypE a , TypE b){return a+b;} // ある点(マス目)にかかる評価値を全てマージする演算
    TypE E(){return TypE(0);} // 評価値のマージの単位元
    

    // 3 次元とかにしたい場合は引数に z 軸の座標を加えるだけ
    // 処理は全部 NdSegTree に丸投げする
    class _2dSegTree{
        private:
        int H , W;
        DualNdSegTree<TypE , OP , E, UPD> T;
        public:
        // 縦幅,横幅,初期化値
        _2dSegTree(int H_,int W_,TypE init_):H(H_),W(W_){
            vector<int> S = {H_,W_};
            T = DualNdSegTree<TypE , OP , E, UPD>(S,init_);
        }
        // 上から y 番目、左から x 番目の要素を返す (0-index)
        TypE get(int y , int x){
            vector<int> s = {y,x};return T.get(s);
        }

        // 点(y1,x1) と 点(y2,x2) を対角とする矩形内の全てのマス目に更新クエリを行う (評価値を加える)
        void rect_update(int y1, int x1, int y2, int x2 ,TypE V){
            vector<int> s1 = {y1,x1};vector<int> s2 = {y2,x2};
            T.rect_update(s1,s2,V);
        }
        
        int height(){return H;}
        int width(){return W;}
        void print(){
            for(int i = 0 ; i < H ; i++){
                for(int j = 0 ; j < W; j++)cerr << get(i,j) << " ";
                cerr << endl;
            }
        }
    };

}




// 矩形一律更新 (タイムスタンプを保持することで、最新の評価値を優先する)
// 評価値はタイムスタンプを保持する必要があるので、DualNdSegTree にはペアを渡して、ラッパークラス (_2dseg) で raw_type でやりとりさせる
namespace RectUpdatePointGet{
    using raw_type = long long;// value の型
    using TypE = pair<raw_type , long long>;// {value , time} 
    long long timestamp = 0;// 更新クエリの順番のタイムスタンプ(プログラムを通して使い回してOK)

    TypE convert_rawtype(raw_type v){
        return {v,timestamp++};// time stamp に割り振りをしてくれる。  
    }

    // 区間の評価 a に対して評価を加算する
    TypE UPD(TypE& a , TypE b){
        return a = convert_rawtype(b.first);
    }

    // 貯めておいた更新データのマージを行う (新しい評価を優先する)
    TypE OP(TypE a , TypE b){
        if(a.second > b.second)return a;
        else return b;
    } 

    TypE E(){return {0, numeric_limits<long long>::min()};}// 評価及びマージの単位元(タイムは 最小値)



    // 3 次元とかにしたい場合は引数に z 軸の座標を加えるだけ
    // 処理は全部 NdSegTree に丸投げする
    class _2dSegTree{
        private:
        int H , W;
        DualNdSegTree<TypE , OP , E, UPD> T;
        public:
        // 縦幅,横幅,初期化値
        _2dSegTree(int H_,int W_,raw_type init_):H(H_),W(W_){
            vector<int> S = {H_,W_};
            // {init_ , timestamp} がグリッドの初期値 (timestamp はこの時点以降単調増加)
            T = DualNdSegTree<TypE , OP , E, UPD>(S,convert_rawtype(init_));
        }
        // 上から y 番目、左から x 番目の要素を返す (0-index)
        raw_type get(int y , int x){
            vector<int> s = {y,x};
            return T.get(s).first;
        }

        // 点(y1,x1) と 点(y2,x2) を対角とする矩形内の全てのマス目に更新クエリを行う (評価値を加える)
        void rect_update(int y1, int x1, int y2, int x2 ,raw_type V){
            vector<int> s1 = {y1,x1};vector<int> s2 = {y2,x2};
            T.rect_update(s1,s2,convert_rawtype(V));
        }
        
        int height(){return H;}
        int width(){return W;}
        void print(){
            for(int i = 0 ; i < H ; i++){
                for(int j = 0 ; j < W; j++)cerr << get(i,j) << " ";
                cerr << endl;
            }
        }
    };


    void test(){
        int H = 30;
        int W = 30;
        raw_type init = rand()%50 - 25;
        vector<vector<raw_type> > A(H,vector<raw_type>(W,init));
        _2dSegTree S(H,W,init);

        int qcnt = 40000;
        int bdr = qcnt/50;
        while(qcnt-->0){
            int t = rand()%2;
            int y1,x1,y2,x2;
            y1 = rand()%H;
            y2 = rand()%H;
            x1 = rand()%W;
            x2 = rand()%W;
            raw_type V = rand()%50 - 25;
            while(y1 == y2){y1 = rand()%H;y2 = rand()%H;}
            while(x1 == x2){x1 = rand()%W;x2 = rand()%W;}

            if(t == 0){
                for(int i = min(y1,y2) ; i < max(y2,y1) ; i++)for(int j = min(x1,x2) ; j < max(x2,x1) ; j++)A[i][j] = V;
                S.rect_update(y1,x1,y2,x2,V);
            }else{
                for(int i = 0 ; i < H ; i++)for(int j = 0 ; j < W ; j++)assert(A[i][j] == S.get(i,j));
            }

            if(qcnt%bdr == 0){
                cerr << "OK : " <<  qcnt/bdr << endl;
            }
        }
        S.print();
        

    }

}











