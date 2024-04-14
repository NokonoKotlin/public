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





