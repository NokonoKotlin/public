#include "MyTemplate.hpp"





/*
    未 verified なので注意。
    ノードに MyMultiSet を持たせると動的になる
    StaticRangeCountSegTree(vector<T> A, T init_) := 列 A でセグ木を作り、空いている領域を init_ で初期化する
    Static な列 A に対して、以下を行う。

    主に、半開区間 [l,r) の要素 に対して x 以上/以下 の制約をつけたクエリに答える 

    RangeCountQuery(l,r,x) : 半開区間 [l,r) の x の個数
    RangeLowerBound(l,r,x) : 半開区間 [l,r) の x 未満の要素数
    RangeUpperBound(l,r,x) : 半開区間 [l,r) の x 以下の要素数
    RangePrefixSumQuery(l,r,x,lowerbound) : 半開区間 [l,r) の x 以下(lowerboundがtrueなら未満)の要素の総和
    RangeRangeSumQuery(l,r,x,y) := 半開区間 [l,r) の x 以上 y 以下の要素の総和を計算    
    RangeMinQuery(l,r,x) := 半開区間 [l,r) の x 以上の要素の最小値 
    RangeMaxQuery(l,r,x) := 半開区間 [l,r) の x 以下の要素の最大値


    [i] : i 番目の要素を取得(read-only)
    size() : 最下層のサイズ (A + 余った部分の init_)
*/
template<class T>
class StaticMergeSortTree{
    private:
    vector<vector<T>> Node;
    vector<vector<T>> PrefixSum;// 各ノードの数列の累積和
    int array_size;
    int tree_size;
    T T_INF;// T 型の max
    T T_NINF;// T 型の min
    const static int SegTreeRoot = 1;// 根ノードは 1 とする

    void init(vector<T> A , T init_){
        array_size = 1;
        while(array_size<int(A.size()))array_size*=2;
        tree_size = array_size*2;
        T_INF = numeric_limits<T>::max();
        T_NINF = numeric_limits<T>::min();
        Node = vector<vector<T>>(tree_size);
        PrefixSum = vector<vector<T>>(tree_size);
        REP(i,array_size){
            if(i < A.size())Node[i+array_size].push_back(A[i]);
            else Node[i+array_size].push_back(init_);
            PrefixSum[i+array_size] = {0,A[i]};
        }
        for(ll x = array_size-1;x>=SegTreeRoot;x--){
            ll l = x*2;
            ll r = x*2+1;
            for(T &y : Node[l])Node[x].push_back(y);
            for(T &y : Node[r])Node[x].push_back(y);
            sort(ALL(Node[x]));
            PrefixSum[x] = vector<T>(Node[x].size() + 1 , 0);
            for(int i = 0 ; i < Node[x].size();i++)PrefixSum[x][i+1]=PrefixSum[x][i]+Node[x][i];
        }   
    }

    // 半開区間 [l,r) 内の、x 未満の要素数
    int RangeUpperBound_sub(int l , int r , T x , int SubRoot = SegTreeRoot , int SubTreeLeft = 0 , int SubTreeRight = -1){

        if(SubRoot == SegTreeRoot)SubTreeRight = array_size;
        if(r <= SubTreeLeft)return 0;
        else if(l >=SubTreeRight)return 0;
        else if( l <= SubTreeLeft && SubTreeRight <= r)return int(upper_bound(Node[SubRoot].begin() , Node[SubRoot].end() , x) - Node[SubRoot].begin());
        else {
            int border = (SubTreeLeft+SubTreeRight)>>1;
            return RangeUpperBound_sub(l,r,x,SubRoot*2,SubTreeLeft,border) + RangeUpperBound_sub(l,r,x,SubRoot*2+1,border,SubTreeRight);
        }
    }


    // 半開区間 [l,r) 内の、x 未満の要素数
    int RangeLowerBound_sub(int l , int r , T x , int SubRoot = SegTreeRoot , int SubTreeLeft = 0 , int SubTreeRight = -1){
        if(SubRoot == SegTreeRoot)SubTreeRight = array_size;
        if(r <= SubTreeLeft)return 0;
        else if(l >=SubTreeRight)return 0;
        else if( l <= SubTreeLeft && SubTreeRight <= r)return int(lower_bound(Node[SubRoot].begin() , Node[SubRoot].end() , x) - Node[SubRoot].begin());
        else {
            int border = (SubTreeLeft+SubTreeRight)>>1;
            return RangeLowerBound_sub(l,r,x,SubRoot*2,SubTreeLeft,border) + RangeLowerBound_sub(l,r,x,SubRoot*2+1,border,SubTreeRight);
        }
    }


    // 区間 [l,r) 内の要素のうち、x (以下/未満)の要素の総和 (lowerbound が true なら x 未満)
    T RangePrefixSumQuery_sub(int l , int r , T x , bool lowerbound , int SubRoot = SegTreeRoot , int SubTreeLeft = 0 , int SubTreeRight = -1){
        if(SubRoot == SegTreeRoot)SubTreeRight = array_size;
        if(r <= SubTreeLeft)return 0;
        else if(l >=SubTreeRight)return 0;
        else if( l <= SubTreeLeft && SubTreeRight <= r){
            // x の制約が実質存在しない場合は全ての要素の総和を返す
            if(!lowerbound && x == T_INF)return PrefixSum[SubRoot][Node[SubRoot].size()];
            int it ;
            if(lowerbound)it = int(lower_bound(Node[SubRoot].begin() , Node[SubRoot].end() , x) - Node[SubRoot].begin());
            else it = int(upper_bound(Node[SubRoot].begin() , Node[SubRoot].end() , x) - Node[SubRoot].begin());
            return PrefixSum[SubRoot][it];
        }else {
            int border = (SubTreeLeft+SubTreeRight)>>1;
            return RangePrefixSumQuery_sub(l,r,x,lowerbound, SubRoot*2,SubTreeLeft,border) + RangePrefixSumQuery_sub(l,r,x,lowerbound,SubRoot*2+1,border,SubTreeRight);
        }
    }


    // 区間 [l,r) 内の要素のうち、x 以上の要素の最小値
    T RangeMinQuery_sub(int l , int r , T x , int SubRoot = SegTreeRoot , int SubTreeLeft = 0 , int SubTreeRight = -1){
        if(SubRoot == SegTreeRoot)SubTreeRight = array_size;
        if(r <= SubTreeLeft)return T_INF;
        else if(l >=SubTreeRight)return T_INF;
        else if( l <= SubTreeLeft && SubTreeRight <= r){
            if(x == T_NINF)return Node[SubRoot][0];// x の制約が実質存在しない場合は先頭の要素を返す

            auto it = lower_bound(Node[SubRoot].begin() , Node[SubRoot].end() , x);
            if(it == Node[SubRoot].end())return T_INF;
            else return *it;
        }else {
            int border = (SubTreeLeft+SubTreeRight)>>1;
            return min(RangeMinQuery_sub(l,r,x, SubRoot*2,SubTreeLeft,border) , RangeMinQuery_sub(l,r,x,SubRoot*2+1,border,SubTreeRight));
        }
    }


    // 区間 [l,r) 内の要素のうち、x 以下の要素の最大値
    T RangeMaxQuery_sub(int l , int r , T x , int SubRoot = SegTreeRoot , int SubTreeLeft = 0 , int SubTreeRight = -1){
        if(SubRoot == SegTreeRoot)SubTreeRight = array_size;
        if(r <= SubTreeLeft)return T_NINF;
        else if(l >=SubTreeRight)return T_NINF;
        else if( l <= SubTreeLeft && SubTreeRight <= r){
            if(x == T_INF)return Node[SubRoot].back();// x の制約が実質存在しない場合は末尾の要素を返す
            auto it = upper_bound(Node[SubRoot].begin() , Node[SubRoot].end() , x);
            if(it == Node[SubRoot].begin())return T_NINF;
            else {
                it--;
                return *it;
            }
        }else {
            int border = (SubTreeLeft+SubTreeRight)>>1;
            return max(RangeMaxQuery_sub(l,r,x, SubRoot*2,SubTreeLeft,border) , RangeMaxQuery_sub(l,r,x,SubRoot*2+1,border,SubTreeRight));
        }
    }

    public:

    // 列のサイズを受けとる
    StaticMergeSortTree(){}

    // 列と、余った部分の初期化値を渡す
    StaticMergeSortTree(vector<T> A , T init_){init(A,init_);}


    // 半開区間 [l,r) 内の、x 未満の要素数
    int RangeLowerBound(int l , int r , T x){
        l = max(l,0);r = min(size(),r);assert(l<r);
        return RangeLowerBound_sub(l,r,x);
    }
    // 半開区間 [l,r) 内の、x 以上の要素数
    int RangeUpperBound(int l , int r , T x){
        l = max(l,0);r = min(size(),r);assert(l<r);
        return RangeUpperBound_sub(l,r,x);
    }
    
    // 半開区間 [l,r) 内の、x の要素数
    int RangeCountQuery(int l , int r , T x , int SubRoot = SegTreeRoot , int SubTreeLeft = 0 , int SubTreeRight = -1){
        l = max(l,0);r = min(size(),r);assert(l<r);
        return RangeUpperBound_sub(l,r,x)-RangeLowerBound_sub(l,r,x);
    }

    // 半開区間 [l,r) 内の x 以上 y 以下の要素の総和
    T RangeRangeSumQuery(int l,int r , T x , T y){
        l = max(l,0);r = min(size(),r);assert(l<r);
        return RangePrefixSumQuery_sub(l,r,y,false) - RangePrefixSumQuery_sub(l,r,x,true);
    }

    // 半開区間 [l,r) 内の要素のうち、x 以下( lowerbound = true なら x 未満)のものの総和
    T RangePrefixSumQuery(int l , int r , T x , bool lowerbound = false){
        l = max(l,0);r = min(size(),r);assert(l<r);
        return RangePrefixSumQuery_sub(l,r,x,lowerbound);
    }

    // 半開区間 [l,r) 内の総和
    T RangeSumQuery(int l , int r){
        l = max(l,0);r = min(size(),r);assert(l<r);
        return RangePrefixSumQuery_sub(l,r,T_INF,false);
    }



    // 半開区間 [l,r) 内の最小値
    T RangeMinQuery(int l , int r){
        l = max(l,0);r = min(size(),r);assert(l<r);
        return RangeMinQuery_sub(l,r,T_NINF);
    }

    // 半開区間 [l,r) 内の x 以上の要素のうちの最小値
    T RangeMinQuery(int l , int r , T x){
        l = max(l,0);r = min(size(),r);assert(l<r);
        return RangeMinQuery_sub(l,r,x);
    }

    // 半開区間 [l,r) 内の最大値
    T RangeMaxQuery(int l , int r){
        l = max(l,0);r = min(size(),r);assert(l<r);
        return RangeMaxQuery_sub(l,r,T_INF);
    }


    // 半開区間 [l,r) 内の x 以下の要素のうちの最大値
    T RangeMaxQuery(int l , int r , T x){
        l = max(l,0);r = min(size(),r);assert(l<r);
        return RangeMaxQuery_sub(l,r,x);
    }

    // 列のサイズ
    int size(){
        return array_size;
    }

    // read-only で要素を取得。
    T operator [](int index){
        return Node[size()+index][0];
    }


};






void test_StaticMergeSetTree(){
    using testtype = long long;
    ll n = 100000;
    ll Q = 500000;
    ll mx = 10000;
    ll mn = -10000;
    ll bdr = Q/50;
    vector<testtype> A(n);
    for(testtype& x : A)x = rand()%(mx-mn) + mn; 
    StaticMergeSortTree<testtype> S(A,INF);
    while(Q-->0){
        ll t = rand()%9+1;
        /*
            0 : update
            1 : RangeSum
            2 : RangeRangeSum
            3 : RangeCount
            4 : RangeMin
            5 : RangeMin(x)
            6 : RangeMax
            7 : RangeMax(x)
            8 : check[i]
            9 : check all
        */

        ll i = rand()%n;
        ll x = rand()%(mx-mn) + mn; ll y = rand()%(mx-mn) + mn; 
        ll l , r;l = rand()%n;r = rand()%n;
        ll TMP = 0;

        if(x>y)swap(x,y);
        if(l>r)swap(l,r);
        if(l==r){
            l = 0;
            r = n;
        }
        if(t==1){
            FOR(i,l,r)TMP += A[i];
            assert(TMP == S.RangeSumQuery(l,r));
        }else if(t == 2){
            FOR(i,l,r)if(A[i] >= x && A[i] <= y)TMP += A[i];
            assert(TMP == S.RangeRangeSumQuery(l,r,x,y));
        }else if(t == 3){
            FOR(i,l,r)if(A[i] == x)TMP++;
            assert(TMP == S.RangeCountQuery(l,r,x));
        }else if(t==4){
            TMP = numeric_limits<testtype>::max();
            FOR(i,l,r)TMP = min(TMP,A[i]);
            assert(TMP == S.RangeMinQuery(l,r));
        }else if(t==5){
            TMP = numeric_limits<testtype>::max();
            FOR(i,l,r)if(A[i]>=x)TMP = min(TMP,A[i]);
            assert(TMP == S.RangeMinQuery(l,r,x));
        }else if(t==6){
            TMP = numeric_limits<testtype>::min();
            FOR(i,l,r)TMP = max(TMP,A[i]);
            assert(TMP == S.RangeMaxQuery(l,r));
        }else if(t==7){
            TMP = numeric_limits<testtype>::min();
            FOR(i,l,r)if(A[i]<=x)TMP = max(TMP,A[i]);
            assert(TMP == S.RangeMaxQuery(l,r,x));
        }else if(t == 8)assert(A[i] == S[i]);
        else REP(iii,n)assert(A[iii] == S[iii]);
        
        if(Q%bdr == 0)debug(Q/bdr)
    }
    debug("end")
}









