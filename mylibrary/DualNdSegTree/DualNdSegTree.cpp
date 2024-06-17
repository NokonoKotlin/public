#include "./../template/template.hpp"










/*
    - 多次元双対セグ木
    - N 次元のグリッドの矩形領域に対して更新クエリを行う。
    - 更新クエリをおこなった区間に対応するセグメントに、更新クエリによる影響を評価値として貯めておく。  
    |
    |
    - 評価値とは別に init_val を単体のデータで保持しておくことで、あたかもグリッドが init_val で初期化してあったかのように振る舞うようにする
    |
    - 初め、全てののマス目にかかる評価値は e() : 単位元 で初期化される(何も評価がかかっていないことを表す)
        |- ある点の値を取得する際は、グリッドの初期値 init_val と、グリッドにかかる評価値の総マージ結果をマージしたものを返す
    |
    - コンストラクタで vector<int> size_ と T init_val を渡す。
    - 各次元のサイズが size_[0],size_[1] , ... , size_[d-1] の d 次元グリッド(マス目)を表現し、グリッドの値は init_val で初期化されている。  
    |
    |
    - 基本的に座標を表すのは d 次元座標で、d 次元座標上の矩形を表す際は、その矩形の対角の座標 A , B を用いて表す。
    - その矩形内のマスを対象に矩形クエリを行う    
    |
    - 多次元セグ木のノードは一次元下のセグ木なので、セグ木 = Node として捉えて OK 
    |
    - 抽象化 
        |- データの型        : T
        |- データ x に val を反映させる : upd_(&x , val) 
        |- ある点 (マス目) にかかるすべての評価をマージする演算 : op(a,b) 
        |- 評価マージの単位元 : e()
*/
template<class T,
T (*op_)(T,T),
T (*e_)(),
T (*upd_)(T& , T)>
class DualNdSegTree{
    private:
    // [i]:= このセグ木以下の次元で i 次元目のサイズ (処理で使うのはsize[0]のみだが、アクセス可能にしておく)
    vector<int> size; 
    vector<DualNdSegTree<T , op_ , e_ , upd_ > > node;

    // グリッドの初期値(全ての次元で共通、コンストラクタで与えられてからは一歳変化しない)
    T init_val;
    // このセグ木 (Node) にかかっている評価値 
    T data;
    

    int d; // このセグメント木の次数
    const static int SegTreeRoot = 1;// セグ木の root 

    // sz を sz 以上の、2 の冪乗の値にフォーマットする
    template<typename Integer>
    Integer resize(Integer& sz){
        Integer p = 1;
        while(p < sz)p*=2;
        return (sz = p);
    }
    // 初期化用の一時オブジェクト size_v をコピーして渡す。
    // size メンバや node メンバを初期化する
    template<typename Integer>
    void init(vector<Integer> size_v){   
        d = int(size_v.size());
        if(d<= 0)data = e_();// 0 次元セグ木 は、単なる T 型要素
        else{
            size.resize(d);
            for(int& s : size_v) s = resize(s);
            for(int i = 0 ; i < d ; i++ )size[i] = size_v[i];
            size_v.erase(size_v.begin());// 下の次元に
            node.clear();
            node.resize(size[0]*2, DualNdSegTree< T , op_ , e_ , upd_ >(size_v,init_val));
        }
    } 

    // 座標 (A[0],A[1],...,A[d-1]) の評価値を取得する (特に、A[deg] 以降に注目)
    template<typename Integer>
    T get_sub(const vector<Integer> &A, int deg = 0){
        // 0 次元の時
        if(d <= 0)return data;

        int index = A[deg];
        index += size[0];
        // 下の次元に投げる
        T res = node[index].get_sub(A,deg+1);

        // 評価をマージする
        while((index>>1) >= SegTreeRoot){
            index>>=1;
            res = op_(res , node[index].get_sub(A,deg+1));
        }
        return res;
    }



    // 座標 (A[0],A[1],...,A[d-1]) と (B[0],B[1],...,B[d-1]) を対角とする矩形内に upd_ を作用させる
    template<typename Integer>
    void update_sub(const vector<Integer>& A ,const vector<Integer>& B, T val , int deg = 0 , int SubRoot = SegTreeRoot , int SubTreeLeft = 0 , int SubTreeRight = -1 ){
        if(d <= 0){// 該当区間内の最低次元要素
            upd_(data,val);// 更新クエリをかける
            return;
        }
        if(SubRoot == SegTreeRoot)SubTreeRight = size[0];

        int lef = A[deg];
        int rig = B[deg];
        if(lef > rig)swap(lef,rig);
        if(lef == rig)assert(0);
        // 区間が被っていない
        if(rig <= SubTreeLeft)return ;
        // 区間が被っていない
        else if(lef >=SubTreeRight)return ;
        // SubRootを根とする部分木の範囲が、取得したい範囲に完全に含まれている
        else if( lef <= SubTreeLeft && SubTreeRight <= rig )node[SubRoot].update_sub(A , B , val , deg+1);// このノードが担当している区間に評価をかける
        // SubRootの右の根と左の根に降りて、再度更新
        else{
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
        assert(A.size() == d);
        for(int i = 0 ; i < d ; i++)assert(0 <= A[i] && A[i] < size[i]);
        return op_(get_sub(A),init_val); // 評価値の総マージ と グリッドの初期値 のマージを計算
    }  

    // 座標 (A[0],A[1],...,A[d-1]) と (B[0],B[1],...,B[d-1]) を対角とする矩形に対して、upd_ を作用させる
    template<typename Integer>
    void rect_update(vector<Integer> A , vector<Integer> B , T val){
        assert(A.size() == d && B.size() == d);
        for(int i = 0 ; i < d ; i++)assert(0 <= A[i] && A[i] < size[i]);
        for(int i = 0 ; i < d ; i++)assert(0 <= B[i] && B[i] < size[i]);
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
// 評価値はタイムスタンプを保持する必要があるので、DualNdSegTree にはペアを渡す。
namespace RectUpdatePointGet{
    using raw_type = long long;// value の型
    using TypE = pair<raw_type , long long>;// {value , time} 
    // 外で変更したらだめ
    long long timestamp = 0;// 更新クエリの順番のタイムスタンプ(プログラムを通して使い回してOK)

    // time stamp を割り振る
    TypE convert_rawtype(raw_type v){return {v,timestamp++};}

    // 区間の評価 a に対して評価を加算する
    TypE UPD(TypE& a , TypE b){
        if(a.second < b.second)a = convert_rawtype(b.first);
        return a;
    }

    // 貯めておいた更新データのマージを行う (新しい評価を優先する)
    TypE OP(TypE a , TypE b){
        if(a.second > b.second)return a;
        return b;
    } 

    // 評価及びマージの単位元 (タイムは最小値)
    TypE E(){return {0, numeric_limits<long long>::min()};}

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
            vector<int> s1 = {y1,x1};
            vector<int> s2 = {y2,x2};
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
        const int H = 30;
        const int W = 30;
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
            if(qcnt%bdr == 0)cerr << "OK : " <<  qcnt/bdr << endl;
        }
        S.print();
    }
}





