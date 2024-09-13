#include "./../template/template.hpp"





/*
    Copyright ©️ (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)
*/
template<class T,
T (*op_)(T,T) ,
T (*e_)() ,
T (*upd_)(T& , T)>
class NdSegTree{

    private:
    // [i]:= このセグ木以下の次元で i 次元目のサイズ (この次元の処理で使うのはsize[0]のみ)
    vector<int> size; 
    vector<NdSegTree<T , op_ , e_ , upd_ > > node;
    T data;// 0 次元セグ木に限り、data を持つ。
    int d; // このセグメント木の次元

    // セグ木の root 番号
    const static int SegTreeRoot = 1;

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
    void init(vector<Integer> size_v,T init_){   
        d = int(size_v.size());

        if(d<= 0)data = init_;// 0 次元セグ木 は、単なる T 型要素
        else{
            size.resize(d);
            for(int& s : size_v) s = resize(s);
            for(int i = 0 ; i < d ; i++ )size[i] = size_v[i];
            size_v.erase(size_v.begin());
            node.clear();
            // 下の次元のセグ木を要素にもつセグ木を構築する
            node.resize(size[0]*2, NdSegTree< T , op_ , e_ , upd_ >(size_v,init_));
        }
    } 

    // 座標 (A[0],A[1],...,A[d-1]) の値を val に変更する (特に、A[deg] 以降に注目)
    template<typename Integer>
    void set_sub(const vector<Integer> &A, T val , int deg = 0){
        if(d <= 0){ // 0 次元の時、
            data = val;
            return;
        }
        int index = A[deg];
        index += size[0];
        // 下の次元に投げる
        node[index].set_sub(A,val,deg+1);
        // 上段の更新は、同次元内で完結する
        while((index>>1) >= SegTreeRoot ){
            index>>=1;
            // N 次のセグ木内で更新をかけるのは N-1 次元要素であることに注意
            node[index].set_sub(A, op_(node[index*2].get_sub(A,deg+1) , node[index*2+1].get_sub(A,deg+1)) , deg + 1);
        }
    }

    // 座標 (A[0],A[1],...,A[d-1]) の値を、val でupdateする 
    template<typename Integer>
    void update_sub(const vector<Integer> A , T val){
        T x = get_sub(A);
        upd_(x,val);// x に更新をかける
        set_sub(A,x);
    }

    // 座標 (A[0],A[1],...,A[d-1]) と (B[0],B[1],...,B[d-1]) を対角とする矩形内の値の op を取得 (ただし、各次元の区間は半開区間である) , A[deg] , B[deg] 以降に注目
    template<typename Integer>
    T query_sub(const vector<Integer>& A ,const vector<Integer>& B , int deg = 0 , int SubRoot = SegTreeRoot , int SubTreeLeft = 0 , int SubTreeRight = -1 ){
        if(d <= 0)return data; // 0 次元はデータ単体しか持たないので

        if(SubRoot == SegTreeRoot)SubTreeRight = size[0];

        int lef = A[deg];
        int rig = B[deg];
        if(lef > rig)swap(lef,rig);
        if(lef == rig)assert(0);
        T res = e_();

        // どれかの一つのステートしか実行されないので、A , B を同期しなくていい
        // 区間が被っていない
        if(rig <= SubTreeLeft)return e_();
        // 区間が被っていない
        else if(lef >=SubTreeRight)return e_();
        // SubRootを根とする部分木の範囲が、取得したい範囲に完全に含まれている-> node[SubRoot]の値をreturn
        else if( lef <= SubTreeLeft && SubTreeRight <= rig )return node[SubRoot].query_sub(A , B , deg+1);
        // SubRootの右の根と左の根に降りて、再度調べる
        else{
            int border = (SubTreeLeft+SubTreeRight)/2;
            return op_( query_sub(A,B,deg,SubRoot*2,SubTreeLeft,border) , query_sub(A,B,deg,SubRoot*2+1,border,SubTreeRight) );
        }
    }

    // 座標 (A[0],A[1],...,A[d-1]) の値を取得  (特に、A[deg] 以降に注目)
    template<typename Integer>
    T get_sub(const vector<Integer>& A , int deg = 0){
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
        assert(A.size() == d);
        for(int i = 0 ; i < d ; i++)assert(0 <= A[i] && A[i] < size[i]);
        return get_sub(A);
    }  

    // 座標 (A[0],A[1],...,A[d-1]) と (B[0],B[1],...,B[d-1]) を対角とする矩形に対して、マージ結果を計算
    template<typename Integer>
    T rect_query(vector<Integer> A , vector<Integer> B){
        assert(A.size() == d && B.size() == d);
        for(int i = 0 ; i < d ; i++)assert(0 <= A[i] && A[i] < size[i]);
        for(int i = 0 ; i < d ; i++)assert(0 <= B[i] && B[i] < size[i]);
        return query_sub(A,B);
    }

    // 座標 (A[0],A[1],...,A[d-1]) の値を val に変更
    template<typename Integer>
    void set(vector<Integer> A , T  val){
        assert(A.size() == d);
        for(int i = 0 ; i < d ; i++)assert(0 <= A[i] && A[i] < size[i]);
        return set_sub(A , val);
    }  

    // 座標 (A[0],A[1],...,A[d-1]) の値に upd(val) で更新をかける
    template<typename Integer>
    void update(vector<Integer> A , T val){
        assert(A.size() == d);
        for(int i = 0 ; i < d ; i++)assert(0 <= A[i] && A[i] < size[i]);
        return update_sub(A, val);
    }  


    // グリッドの i 番目の次元の幅を返す (d 次元目は、0 次元のグリッドと捉えて 0 を返す)
    int width(int i){
        if(i< 0 || i>d)assert(0);
        if(i == d)return 0;
        else return size[i];
    }
};


// N 次元セグ木の使用例 (2 次元グリッドの一点加算、矩形和)
namespace PointAddRectSum{
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
        TypE get(int y , int x){vector<int> s = {y,x};return T.get(s);}
        // 上から y 番目、左から x 番目に V を代入する (0-index)
        void set(int y , int x , TypE V){vector<int> s = {y,x};T.set(s,V);}
        // 上から y 番目、左から x 番目にの要素に対して V で update する (UPD() を行う)
        void update(int y , int x , TypE V){vector<int> s = {y,x};T.update(s,V);}
        // 点(y1,x1) と 点(y2,x2) を対角とする矩形内のマージ値を計算 (全ての要素に OP() をしたもの)
        TypE rect_query(int y1, int x1, int y2, int x2){
            vector<int> s1 = {y1,x1};
            vector<int> s2 = {y2,x2};
            return T.rect_query(s1,s2);
        }
        int height(){return H;}
        int width(){return W;}
    };
    // グリッドに奥行きを追加
    // H,W の関係を維持したかったので、奥行きを第 0 次元にしたが、使う上ではネーミングは関係ない
    class _3dSegTree{
        private:
        int H , W , D;
        NdSegTree<TypE , OP , E, UPD> T;
        public:
        // 奥行き,縦幅,横幅,初期化値
        _3dSegTree(int D_,int H_,int W_,TypE init_):D(D_),H(H_),W(W_){
            vector<int> S = {D,H,W};
            T = NdSegTree<TypE , OP , E, UPD>(S,init_);
        }
        TypE get(int z , int y , int x){vector<int> s = {z,y,x};return T.get(s);}
        void set(int z , int y , int x , TypE V){vector<int> s = {z,y,x};T.set(s,V);}
        void update(int z ,int y , int x , TypE V){vector<int> s = {z,y,x};T.update(s,V);}
        TypE rect_query(int z1 , int y1, int x1 , int z2 , int y2 , int x2){
            vector<int> s1 = {z1,y1,x1};
            vector<int> s2 = {z2,y2,x2};
            return T.rect_query(s1,s2);
        }
        int depth(){return D;}
        int height(){return H;}
        int width(){return W;}
        
    };




    void test3(){
        const int sz = 100;
        vector<vector<vll>> A(sz,vector<vll>(sz,vll(sz,0)));
        PointAddRectSum::_3dSegTree S(sz,sz,sz,0);
        const int vmx = 1000;
        REP(i,A.size()){
            REP(j,A[i].size()){
                REP(k,A[i][j].size()){
                    int r = rand()%(vmx*2) - vmx;
                    S.set(i,j,k,r);
                    A[i][j][k] = r;
                }
            }
        }
        ll cnt = 3000;
        while(cnt-->0){
            int t = rand()%3;
            int x1 = rand()%(sz);int x2 = rand()%(sz);if(x1>x2)swap(x1,x2);if(x2 == x1)x2++;
            int y1 = rand()%(sz);int y2 = rand()%(sz);if(y1>y2)swap(y1,y2);if(y2 == y1)y2++;
            int z1 = rand()%(sz);int z2 = rand()%(sz);if(z1>z2)swap(z1,z2);if(z2 == z1)z2++;
            if(t == 0){
                long long sum = 0;
                for(int i = z1 ; i < z2 ; i++){
                    for(int j = y1 ; j < y2 ; j++){
                        for(int k = x1 ; k < x2 ; k++){
                            sum += A[i][j][k];
                        }
                    }
                }
                assert(sum == S.rect_query(z1,y1,x1,z2,y2,x2));
            }else if(t == 1){
                int r = rand()%(vmx*2) - vmx;
                A[z1][y1][x1] = r;
                S.set(z1,y1,x1,r);
            }else{
                int r = rand()%(vmx*2) - vmx;
                A[z1][y1][x1] += r;
                S.update(z1,y1,x1,r);
            }
            if(cnt%100 == 0)debug(cnt)
        }
    }
}




