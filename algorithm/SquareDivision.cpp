#include "MyTemplate.hpp"




template<class T>
class SqDivision{
    private:
    vector<T> A;// 元の列 
    vector<vector<T>>  block;// 区分けした列たち

    int N;// 列サイズ (サイズを平方数に整形しておく)
    int partition;// 1 区画のサイズ
    int block_num;// 区画の個数
    
    // 集約値たち
    vector<T> sum;// [x] := x 番目の区画内の要素の総和
    vector<T> min_;// [x] := x 番目の区画内の要素の min

    
    // 初期化
    void build(int N_ , T init_){
        vector<T>(N_,init_).swap(A);
        partition = sqrt(N_)+1;
        block_num = (N_+partition-1)/partition;
        N = partition*block_num;
        vector<vector<T>>(block_num,vector<T>(partition)).swap(block);
        vector<T>(block_num,T(0)).swap(sum);
        vector<T>(block_num,T(INF)).swap(min_);
        for(int i = 0 ; i < N ; i++){
            block[i/partition][i%partition] = A[i];
            sum[i/partition] += A[i];
            min_[i/partition] = min(min_[i/partition],A[i]);
        }
    
    }

    // A の半開区間 [l,r) に対して
    // first  = 完全に含まれる区画の index たち 
    // second = 残りの部分それぞれが属する { 区画の index , その区画内での index }
    pair<vector<int> , vector<pair<int,int>>> div_sub(int l , int r){
        vector<int> F;
        vector<pair<int,int>> S;

        int lb = l/partition;
        int rb = r/partition;
        if(l%partition != 0){
            for(int i = 0 ; i < block[lb].size() ; i++){
                if(l <= partition*lb + i && partition*lb + i < r)S.push_back({lb,i});
            }
            lb++;
        }
        if(r%partition != 0){
            for(int i = 0 ; i < block[rb].size() ; i++){
                if(l <= partition*rb + i && partition*rb + i < r)S.push_back({rb,i});
            }
        }
        for(int i = lb ; i < rb ; i++)F.push_back(i);
        return {F,S};
    }

    public:
    // N_ 以上の平方数に整形される。init_ は A の初期化値
    SqDivision(int N_ , T init_){build(N_,init_);}

    void update_val(int i , T x){
        const int b = i/partition;
        sum[b] += x - A[i]; 
        A[i] = x;
        block[b][i%partition] = x;
        min_[b] = block[b][0];
        for(int j = 1; j < block[b].size() ; j++)min_[b] = min(min_[b] , block[b][j]);
    }   

    // 区間和
    T rsum(int l , int r){
        auto &&[f,s] = div_sub(l,r);
        T res = 0;
        for(int i : f)res += sum[i];
        for(auto pr : s)res += block[pr.first][pr.second];
        return res;    
    }

    // 区間 min
    T rmin(int l , int r){
        auto &&[f,s] = div_sub(l,r);
        T res = T(INF);
        for(int i : f)res = min(res,min_[i]);
        for(auto pr : s)res = min(block[pr.first][pr.second],res);
        return res;
    }

    // read-only access
    T operator [](int i){
        return A[i];
    }
};


