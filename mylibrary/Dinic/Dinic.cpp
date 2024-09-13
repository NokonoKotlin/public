#include "./../template/template.hpp"










/*
    Copyright ©️ (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)
*/
template<typename T = long long>
class mxfl{
    private : 

    struct edge{
        // u → v の、容量 cap の有向辺
        int u,v;
        T cap;// 容量
        
        edge* r_edge;// 逆辺へのポインタ
        edge(int u_ , int v_ , T cap_):u(u_),v(v_),cap(cap_){}
    };

    // 頂点数
    int V;

    // 特殊シンボル
    const int UNDEFINED_NODE = -1;
    const int UNREACHED = -1; // bfs で到達不可能を表すシンボル
    const T ZERO_EPS = 0;// 容量が 0 であることを示す (double の場合に微小量として活躍)
    
    // グラフ 
    vector<unordered_map<int,edge*>> E;// 辺集合 ( iter(後述) で参照できない )
    vector<vector<pair<int,const edge*>>> G; // 隣接リスト ( iter(後述) で参照できる )
    // [i][j] := 辺 i → j に流れた流量 (逆辺に負の流量を流すので、負になることもある)
    vector<unordered_map<int,T>> F;

    // dfs を行うときに参照する。
    // dfs を行うたびに始点から順に上書きされるので、使いまわして OK
    vector<int> iter; // [x] := 頂点 x にいるとき、次にみる辺の G[x] における index
    vector<int> backward;// [x] := x から移動できる辺がなくなったとき、戻るべき頂点
    
    // 何度も bfs するので 配列を使い回す。
    vector<int> dist;// 最後に bfs をはじめた頂点からの最短経路長

    public:
    mxfl(int V_)
    :V(V_),E(V_),G(V_),F(V_)
    ,dist(V_,UNREACHED),iter(V_,0),backward(V_,UNDEFINED_NODE){}
    ~mxfl(){
        for(int i = 0 ; i < V ; i++){
            for(pair<int,edge*> e: E[i]){
                // edge を 解放すると r_edge も解放される
                if(i < e.first)delete e.second;
            }
        }
    }

    // u → v の容量を c 増やす
    void add_edge(int u , int v , T c){
        assert(u != v);
        // 多重辺をマージ
        if(E[u][v] != nullptr && E[v][u] != nullptr){
            E[u][v]->cap += c;
            return;
        }
        assert(E[u][v] == nullptr && E[v][u] == nullptr);
        E[u][v] = new edge(u,v,c);
        E[v][u] = new edge(v,u,0);
        E[u][v]->r_edge = E[v][u];
        E[v][u]->r_edge = E[u][v];

        G[u].push_back({v,E[u][v]});
        G[v].push_back({u,E[v][u]});
    }

    // s から 各ノード への最短経路長を計算し、ついでに iter を初期化する
    void bfs(int s){
        vector<int>(V,UNREACHED).swap(dist);
        vector<int>(V,0).swap(iter);
        queue<int> q;q.push(s);
        dist[s] = 0;
        while(!q.empty()){
            int now = q.front();
            q.pop();
            for(pair<int,const edge*> e : G[now]){
                if(e.second == nullptr)continue;
                if(dist[e.first] != UNREACHED)continue;
                if(e.second->cap <= ZERO_EPS )continue;
                dist[e.first] = dist[now]+1;
                q.push(e.first);
            }
        }
    }

    // s → t 最短経路上の edge クラスのポインタを通った順に返す
    // 単体使用の前にも bfs(s) してある必要がある。
    // t に到達できなかった場合は空集合を返す
    vector<edge*> path(int s , int t){
        assert(s!=t);
        vector<edge*> res(0);
        int now = s;
        
        // s からの最短路に含まれる辺のみに注目したグラフは DAG であることを利用している
        while(now != UNDEFINED_NODE){
            if(now == t)break;

            // 移動先を見つけるまで iter[now] をインクリメント
            for(;iter[now] < G[now].size(); iter[now]++ ){
                pair<int,const edge*> e = G[now][iter[now]];
                if(e.second == nullptr)continue;
                if(e.second->cap <= ZERO_EPS)continue;
                if(dist[e.first] <= dist[now])continue;// 最短路のみ注目
                break;// 見つけたら終了
            }

            // 見る辺が見つからなかった場合一つ前に戻る。
            if(G[now].size() <= iter[now]){
                now = backward[now];
                if(res.size() != 0)res.pop_back();// 直前に使った辺を不採用
                // iter[now] を使うと t に到達できないことがわかったので、今後 iter[now] は不採用
                if(now != UNDEFINED_NODE)iter[now]++;// 戻った先の見るべき辺を 1 つずらす
            }else{
                pair<int,const edge*> e = G[now][iter[now]];
                backward[e.first] = now;// 戻り先を新たに設定
                res.push_back(const_cast<edge*>(e.second));// 使った辺を追加
                now = e.first;// 移動する
            }
        }
        return res;
    }



    // 実際に s - t パスに f 流す
    // 流せないなら流さずに false を返す
    bool Flow(int s , int t , T f){
        bfs(s);
        if(dist[t] == UNREACHED)false;
        vector<edge*> p = path(s,t);
        if(p.size()==0)return false;
        T bottle_neck = p.back()->cap;
        for(const edge* e : p)bottle_neck = min(bottle_neck,e->cap);
        if(bottle_neck < f)return false;
        for(edge* e : p){
            F[e->u][e->v] += f;
            e->cap -= f;
            e->r_edge->cap += f;
            F[e->v][e->u] -= f;// 逆辺には負の流量が流れたことにする
        }
        return true;
    }

    // s → t に流せるだけ流す。流した流量を返す
    T MaxFlow(int s , int t){
        T res = 0;
        vector<edge*> p;
        while(true){
            bfs(s);
            if(dist[t] == UNREACHED)break;
            while((p = path(s,t)).size() != 0){
                T bottle_neck = p.back()->cap;
                for(const edge* e : p)bottle_neck = min(bottle_neck,e->cap);
                res += bottle_neck;
                for(edge* e : p){
                    F[e->u][e->v] += bottle_neck;
                    e->cap -= bottle_neck;
                    e->r_edge->cap += bottle_neck;
                    F[e->v][e->u] -= bottle_neck;// 逆辺には負の流量が流れたことにする
                }
            }
        }
        return res;
    }

    vector<edge> edges(){
        vector<edge> res;
        for(int x = 0 ; x < V ; x++)for(pair<int,T> e : F[x])res.emplace_back(x,e.first,e.second);
        return res;
    }
    
};





