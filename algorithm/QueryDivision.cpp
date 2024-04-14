#include "MyTemplate.hpp"







/*
    更新クエリの有効期限が決まっているようなクエリ処理をオフライン(クエリ先読み)で行うコードのテンプレート
    
    以下の実装例は区間更新クエリの例。  
        1 - 区間 [l,r) 内の要素を、x との大きい方で置き換える(更新クエリ)。
        2 - x 番目のクエリを無かったことにする。
        3 - i 番目の要素を取得する。  

    2 のクエリは、x 番目のクエリの有効期限を定めることを意味する。  (便宜上、2 のクエリの対象は更新クエリであることが保証されているのものとする)

    更新クエリを処理するためのデータ構造 S を必要とし、直近の更新クエリを無かったことにする rollback 機能を持つ必要がある。
        双対セグ木ならこれらを行うことができる(更新も rollback も O(logN) )。

    クエリが 1 つ飛んでくるごとに時刻が 1 進むとする。  

    ++クエリの集合++ の列を定義する : Q[x] := 時刻 x の時点で有効なクエリの集合
    クエリの有効期限は区間なので、Q をセグメントツリーにすることで、O(QlogQ) の空間で全てのクエリの有効期限を、時系列順に並べることができる。

    時系列順にクエリを適用すると時間計算量が O(Q^2・[Sのクエリ処理]) となる。 
    そこで、時系列のセグメントツリー の根から、DFS を行う。  
    親から子頂点 x に移動するときに、頂点 x が持つクエリの集合を全て S に適用し、  
    頂点 x から親に戻る時に、頂点 x で処理したクエリを全て rollback する。(これは、頂点 x のクエリ数回 rollback すれば OK)

    すると、末端ノード x に到達した時点で、データ構造 S には時刻 qtime = Rnage[x].first におけるクエリが全て適用されている。
    時刻 qtime に取得クエリが登録されているなら、このタイミングで ans[qtime] に書き込む

    設計 :
        データ構造 S は自由だが、以下を持つ必要がある。  
            - 更新クエリに対応する update_query 構造体
            - 取得クエリに対応する get_query 構造体
            - update_query を適用する apply(query) 関数
            - ge_query を適用する get(query) 関数
            - 直近のクエリ適用を無かったことにする rollback 関数

        データ構造 S に初期値が必要な場合、以下のように全ての時刻にクエリを適用すれば OK
            i 番目を a[i] で初期化する場合 : QS.add_query(0,QS.tlimit(),QS.make_query(i,i+1,a[i]));

        add_update_query(l , r , q) : 有効期限が [l,r) の更新クエリ q を追加する
        add_get_query(t , q) : 時刻 t における取得クエリ q を登録する (ask を参照)
        apply() : クエリを処理して ans に答えを書き込む

        ask[t] := 時刻 t における取得クエリについて {存在するか ,  取得クエリ} のペア
        ans[t] := 時刻 t における取得クエリが存在するなら、その答えを保存しておく

        クラス外では
            make_uddate_query() で更新クエリを生成
            make_add_query() で取得クエリを生成

*/  
class OfflineQueryTemplate{
    private:
    /*
        双対セグ木 : 区間更新/一点取得
        クエリの内容を、問題ごとに変更
    */
    template<class T>
    class DualSegTree{
        public:

        // [l,r) を x で更新
        struct update_query{
            int l , r ;T x;
            update_query(){}
            update_query(int l , int r , T x):l(l),r(r),x(x){}
        };

        // i 番目の要素を取得
        struct get_query{
            int i;
            get_query(){}
            get_query(int i):i(i){}
        };

        // 更新クエリの種類(今回は chmax : (大きい方で置き換える操作))
        T merge(T a , T b){return max(a,b);}

        private:

        // ノードクラス (遅延評価やマージ値などのデータを保持)
        struct Node{T Value;};

        struct query_log{// このログを辿って rollback する
            int r;T v;// Tree[r] を v に戻す。        
            query_log(){}
            query_log(int r , T v):r(r),v(v){}
        };

        // 直近の変更履歴をメモ( 1 クエリでも、複数ノードに影響があるので vector<query_log> を管理)
        vector<vector<query_log>> history;

        int array_size;
        int tree_size;
        const static int SegTreeRoot = 1;// 根ノードは 1 とする
        vector<Node> Tree;
        vector<pair<int,int>> Range;// [x] := 頂点 x が担当する(元の数列上の)区間の範囲(半開区間)



        void init(int N , T init_){
            array_size = 1;
            while(array_size<N)array_size*=2;
            tree_size = array_size*2;

            Tree = vector<Node>(tree_size,Node(init_));
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


        // クエリ qr を適用する
        // 変更履歴を rb にメモし、再帰呼び出しの呼び出し元である場合(root == 1)に、history に rb を追加する
        void RangeUpdate(update_query qr, vector<query_log> &rb , bool root = 1,  int SubRoot = SegTreeRoot){
            int left = Range[SubRoot].first;
            int right = Range[SubRoot].second;
            if(qr.r <= left)return;
            else if(qr.l >= right)return;
            else if( qr.l <= left && right <= qr.r){
                rb.push_back(query_log(SubRoot,Tree[SubRoot].Value));
                Tree[SubRoot].Value = merge(Tree[SubRoot].Value,qr.x);// 更新を反映
            }else {
                RangeUpdate(qr,rb,0,SubRoot*2);
                RangeUpdate(qr,rb,0,SubRoot*2+1);
            }
            if(root)history.push_back(rb);
        }

        public:
        DualSegTree(){}
        // 列サイズと初期化値を渡す
        DualSegTree(int N , T init_ = T()){init(N,init_);}


        // クエリ qr を適用する
        void apply(update_query qr){
            vector<query_log> R;
            RangeUpdate(qr,R,1);
        }


        // 直近の更新クエリを無かったことにする
        void rollback(){
            if(history.size() == 0)return;
            for(query_log x : history.back())Tree[x.r].Value = x.v;
            history.pop_back();
        }

        // 取得クエリ qr を適用
        T get(get_query qr){
            int index = qr.i + array_size;
            T v = Tree[index].Value;
            while(index >= SegTreeRoot*2){
                index/=2;
                v = merge(v,Tree[index].Value);
            }
            return v;
        }

    };



    public:
    // 更新クエリ
    using update_query = DualSegTree<ll>::update_query;
    update_query make_update_query(ll l , ll r , ll x){return update_query(l,r,x);}

    // 取得クエリ
    using get_query = DualSegTree<ll>::get_query;
    get_query make_get_query(ll i){return get_query(i);}

    private:

    // [x] := 頂点 x がカバーする期限にかかるクエリの集合
    vector<vector<update_query>> QNode;
    int array_size;
    int tree_size;
    const static int SegTreeRoot = 1;
    vector<pair<int,int>> Range;


    void init(int N){
        array_size = 1;
        while(array_size<N)array_size*=2;
        tree_size = array_size*2;
        QNode = vector<vector<update_query>>(tree_size);
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
    void add_query_sub(ll l , ll r , update_query x , int SubRoot = SegTreeRoot){
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


    public:
    DualSegTree<ll> S;// 更新を行うためのデータ構造
    vector<ll> ans;// [t] := 時刻 t における取得クエリの答えを格納
    vector<pair<bool,get_query>> ask;// [t] := 時刻 t における取得クエリの内容(存在しなければ frist が false)

    // クエリ数(時系列のスケール)と、列のサイズ N を受け取る
    OfflineQueryTemplate(int Q,int N){
        init(Q);
        ans.resize(array_size,-1);
        ask.resize(array_size,{false,get_query()});
        S = DualSegTree<ll>(N,0);
    }
    
    

    // 時刻の上限(tlimit()は含まない)
    int tlimit(){
        return array_size;
    }

    // 有効期限が [l,r) の更新クエリ x を追加
    void register_update_query(ll l , ll r , update_query x){
        assert(l<r);
        assert(0<=r && l < tlimit());
        assert(0 < r && r <= tlimit());
        add_query_sub(l,r,x);
    }

    // 時刻 t の時点での取得クエリを登録
    void register_get_query(ll t , get_query x){
        assert(0<=t && t < tlimit());
        ask[t] = {true,x};
    }

    // クエリを適用していくテンプレート(SegTreeRoot から DFS)
    void apply(int now = SegTreeRoot){
        // ノードに入る時に、このノードがカバーする期間で有効なクエリを全て適用する
        for(update_query qr : QNode[now])S.apply(qr);

        if(Range[now].second - Range[now].first == 1){
            // 葉ノードの場合    
            ll qtime = Range[now].first;
            // 時刻が qtime の時点での更新クエリが全て適用されている状態
            //取得クエリが登録されているなら、取得結果を ans に書き込む
            
            if(ask[qtime].first){
                ans[qtime] = S.get(ask[qtime].second);
            }
        }else{
            apply(now*2);
            apply(now*2+1);
        }
        // ノードから出る時に、このノードがカバーする期間で有効なクエリを全て無かったことにする
        for(update_query qr : QNode[now])S.rollback();
    }

};



void ABC342G(){
    int n,q;cin >> n;
    OfflineQueryTemplate Q(200001,n+1);
    for(int i = 0 ; i < n ; i++){
        ll A;cin >> A;
        // 常時適用される更新 ( [ 0 , tlimit() ) が有効期限)
        Q.register_update_query(0,Q.tlimit(),Q.make_update_query(i,i+1,A));
    }
    cin >> q;
    // i 番目のクエリ(更新クエリ)の有効期限を last[i] の形で持つ。これは、i 番目のクエリの有効期限が [ i , last[i] ) であることを示す。  
    vector<int> last(q,-1);
    vector<OfflineQueryTemplate::update_query> query(q);
    for(int i = 0 ; i < q ; i++){
        int t;cin >> t;
        if(t == 1){
            cin >> query[i].l >> query[i].r >> query[i].x;
            query[i].l--;
            last[i] = q;
        }else if(t==2){
            ll x;cin >> x;
            last[x-1] = i;
        }else{
            ll x;cin >> x;
            Q.register_get_query(i,Q.make_get_query(x-1));
        }
    }
    
    for(int i = 0 ; i < q ;i++)if(i <= last[i])Q.register_update_query(i,last[i],query[i]);
    Q.apply();
    for(int i = 0 ; i < q ; i++){
        if(Q.ask[i].first)cout << Q.ans[i] << endl;
    }
}




