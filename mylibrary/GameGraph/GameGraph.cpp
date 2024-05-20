#include "./../template/template.hpp"





/*  
    - game探索のプロトタイプ(後退解析,Grundy数)
    - add_edgeで有向辺を加える。
    |
    - Gnum や Bstate で状態を取得する際に自動で grundy 数や後退解析の結果を計算してくれる。
        |- 初回のみ O(|E|)
        |- 計算済みかのフラグを持っておいて、重複して計算されない様にする
        |- 新たに辺を追加したら、次の取得次に再計算する (O(|E|))

    - Bstate(x) := 後退解析結果で、頂点 x の状態 
        |- 0(終了状態 : 先手の番でゲームが終了)　, 1(非終了状態 : 後手の番でゲームが終了) , -1(ゲームが無限に続く) 
    - Gnum(x) := 頂点 x の Grundy 数
*/
class Game{
    private:
    // グラフを受け取り、そのトポロジカルソートを記録した配列を返す関数 (DAGでなければ 空列 を返す)
    template<typename Integer>
    vector<Integer> topological_sort(const vector<vector<Integer> > &G) {
        int V = G.size();
        vector<int> indegree(V,0);
        for(int i = 0 ; i < V ; i++)for(Integer x:G[i])indegree[x]++;
        vector<Integer> sorted_vertices;
        queue<Integer> q;
        for (int i = 0; i < V; i++) {
            if (indegree[i] == 0)q.push(i);
        }
        while (!q.empty()) {
            Integer u = q.front();
            q.pop();
            sorted_vertices.push_back(u);
            for(Integer v : G[u]){
                indegree[v] -= 1;
                if (indegree[v] == 0) q.push(v);
            } 
        }
        bool ng = 0;
        for(int i = 0 ; i < V ; i++)if(indegree[i] != 0)ng = 1;
        if(ng)return vector<Integer>(0);
        return sorted_vertices;
    }

    // メンバ
    vector<vector<long long > >G;//状態遷移のグラフ
    vector<vector<long long > >G_inv;//後退解析したいので辺の向きを逆に

    // mex
    long long mex(vector<long long> v){
        if(v.size()==0)return 0;
        vector<int> M(v.size()+1,0);
        for(long long x: v)if(x>=0&&x<=v.size())M[x]=1;

        for(int res = 0 ; res < M.size();res++)if(M[res] == 0) return res;
        return 0;
    }

    
    // 頂点数 (0 , 1 ,...,N-1 と割り振る)
    long long N;
    // grundy数を格納
    vector<long long> Grundy;
    // 後退解析の結果
    vector<long long> state;// -1 (ゲームが無限に続く) , 0(終了状態 : 先手の番でゲームが終了)　, 1(非終了状態 : 後手の番でゲームが終了)

    void init(long long size){
        N=size;
        vector<vector<long long > >(N).swap(G);
        vector<vector<long long > >(N).swap(G_inv);
    }


    // 各計算を重複して行わないためのフラグ
    bool Backward_done = false;
    bool Grundy_done = false;

    // 後退解析の結果を state に記録    
    void Backward(){ 
        // 重複して実行しないように
        if(Backward_done)return;
        Backward_done = true;
        // stateを初期化、辺が出てない頂点が終了状態
        vector<long long>(N,-1).swap(state);
        
        // 出次数
        vector<long long> Deg(N,0);
        for(int u = 0 ; u < N ; u++)Deg[u]=G[u].size();
        
        for(int x = 0 ; x < N ; x++)if(Deg[x]==0)state[x] = 0;
        // 終了状態から bfs
        queue<long long> Q;

        //終了状態を持った状態でスタート
        for(int x = 0 ; x < N ; x++)if(state[x] == 0){Q.push(x);}
        while(!Q.empty()){
            long long v = Q.front();
            Q.pop();
            //逆辺を辿っていける頂点
            for(long long pre : G_inv[v]){
                if(state[pre]!=-1)continue;
                Deg[pre]--; //preにつながる辺を切る
                if(state[v]==0){    
                    state[pre] = 1;
                    Q.push(pre);
                }else if(state[v]==1 && Deg[pre]==0){
                    //vが終了状態でなく、辺(pre,v)以外が全て探索済み(切られている)場合
                    state[pre] = 0;
                    Q.push(pre);
                }
                // それ以外の場合、pre の評価はまだできない   
            }
        }
    }

    // 各状態の Grundy 数を grundy に記録
    void GrundyCalc(){
        // 重複して実行しないように
        if(Grundy_done)return;
        Grundy_done = true;

        vector<long long>(N , -1).swap(Grundy);
        // Gをトポロジカルソートしたやつ
        vector<long long> G_tp = topological_sort<long long>(G);
        assert(G_tp.size() == G.size());// DAG である必要がある
        // MexVector[v]:= 頂点vから行ける頂点たちの Grundy 数を保管
        vector<vector<long long> > MexVector(N);
        //トポロジカル順で後ろの方から前の方へ逆辺を辿り、頂点ごとのGrundy数を伝搬していく.  
        reverse(G_tp.begin(),G_tp.end());
        for(long long now : G_tp){
            //自分から行ける頂点のGrundy数のMexを計算
            Grundy[now] = mex(MexVector[now]);
            // 自分よりも前の頂点に Grundy 数を渡す。
            for(long long pre : G_inv[now])MexVector[pre].push_back(Grundy[now]);
        }
    }
    public:
    Game(){}
    Game(long long size){init(size+10);}

    //u->vの有向辺
    void add_edge(long long u , long long v){
        assert(0<= u && u < N );
        assert(0<= v && v < N );
        G[u].push_back(v);
        G_inv[v].push_back(u);
        // 状態が更新されたので再計算が必要
        Backward_done = false;
        Grundy_done = false;
    }


    // 状態 x の判定 (後退解析を使用)
    // -1 (ゲームが無限に続く) , 0(終了状態 : 先手の番でゲームが終了)　, 1(非終了状態 : 後手の番でゲームが終了)
    long long Bstate(long long x){
        if(!Backward_done)Backward();
        return state[x];
    }

    // 状態 x の grundy 数
    long long Gnum(long long x){
        if(!Grundy_done)GrundyCalc();
        return Grundy[x];
    }
};



