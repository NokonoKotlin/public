#include "./../template/template.hpp"








/*
    Copyright ©️ (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)
*/  
class OfflineQueryTemplate{
    public:
    // 更新クエリのメンバ
    struct update{
        int l , r , x;// クエリのメンバ
    };

    // 取得クエリの取得クエリのメンバ
    struct get{
        int i;
    };

    // 取得する計算結果の型
    using result = long long;

    private:
    // [x] := 頂点 x がカバーする期限にかかる更新クエリの集合
    vector<vector<update>> QNode;
    int array_size;
    int tree_size;
    const static int SegTreeRoot = 1;
    vector<pair<int,int>> Range;


    void init(int N){
        array_size = 1;
        while(array_size<N)array_size*=2;
        tree_size = array_size*2;
        QNode = vector<vector<update>>(tree_size);
        Range = vector<pair<int,int> >(tree_size);
        for(int i = 0 ; i < array_size ; i++){
            int index = i + array_size;
            Range[index] = {i,i+1};
            index>>=1;
            while(index>=SegTreeRoot){
                Range[index] =  {Range[index*2].first , Range[index*2+1].second};
                index>>=1;
            }
        }
    }


    // 有効期限が [l,r) のクエリを時系列セグ木に追加
    void add_query_sub(int l , int r , update x , int SubRoot = SegTreeRoot){
        // 部分木の区間
        int left = Range[SubRoot].first;
        int right = Range[SubRoot].second;

        if(r <= left)return;
        else if(l >= right)return;
        else if( l <= left && right <= r){
            QNode[SubRoot].push_back(x);
        }else {
            add_query_sub(l,r,x,SubRoot*2);
            add_query_sub(l,r,x,SubRoot*2+1);    
        }
    }

    vector<result> ans;// [t] := 時刻 t における取得クエリの答えを格納
    vector<pair<bool,get>> ask;// [t] := 時刻 t における取得クエリの内容(存在しなければ frist が false)

    public:

    // クエリ数(時系列のスケール)と、列のサイズ N を受け取る
    OfflineQueryTemplate(int Q,int N){
        init(Q);
        ans.resize(array_size,result(-1));
        ask.resize(array_size,{false,get()});
    }
    
    // 時刻の上限(tlimit()は含まない)
    int tlimit(){return array_size;}

    // 有効期限が [l,r) の更新クエリ x を追加
    void register_update_query(int l , int r , update x){
        assert(l<r);
        assert(0<=r && l < tlimit());
        assert(0 < r && r <= tlimit());
        add_query_sub(l,r,x);
    }

    // 時刻 t の時点での取得クエリを登録
    // 適用する時刻はユニーク
    void register_get_query(int t , get x){
        assert(0<=t && t < tlimit());
        ask[t] = {true,x};
    }

    // クエリを適用していくテンプレート(SegTreeRoot から DFS)
    void apply(int now = SegTreeRoot){
        // ノードに入る時に、このノードがカバーする期間で有効なクエリを全て適用する

        for(update qr : QNode[now]){
            /*
                =========== 編集領域 ===========
                
                データ構造に更新クエリ qr を適用する
                 
                =========== 編集領域 ===========
            */
        }

        // 葉ノードの場合、取得クエリがある可能性がある
        if(Range[now].second - Range[now].first == 1){
            int qtime = Range[now].first;
            
            // 取得クエリが存在する場合
            if(ask[qtime].first){
                get qr = ask[qtime].second;
                /*
                    =========== 編集領域 ===========
                    
                    データ構造に取得クエリ qr を適用し、結果を ans[qtime] に格納する
                    
                    =========== 編集領域 ===========
                */
            }
        }else{
            // 内部ノードの場合、時系列セグ木の左右に降りる
            apply(now*2);
            apply(now*2+1);
        }
        // ノードから出る時に、このノードがカバーする期間で有効なクエリを全て無かったことにする
        // qr の中身は関係なく、QNode[now] のサイズ回取り消す
        for(update qr : QNode[now]){
            /*
                =========== 編集領域 ===========
                
                データ構造の最後の操作を取り消す(rollback)
                
                =========== 編集領域 ===========
            */
        }
    }

    // 時刻 t の取得クエリの結果
    // 時刻 t に取得クエリが存在しない場合、first が false (second は未定義)
    pair<bool,result> res(int t){
        if(!ask[t].first)return {false,result(-1)};
        return {true,ans[t]};
    }
    
};


