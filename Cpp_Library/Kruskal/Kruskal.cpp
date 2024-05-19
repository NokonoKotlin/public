#include "./../template/template.hpp"





/* 
    Kruskal :クラスカル法で minimum spanning forest を求める構造体
    最小全域森の重みの総和: sum
    計算量: O(|E|log|V|)
    add_edge( u , v , cost ) で辺を追加
*/
template<class T>
struct Kruskal {
    private:
    class UnionFind{
        private:
        vector<int> par;
        public:
        vector<int> rank;
        
        UnionFind() {}
        UnionFind( int N):rank(N),par(N){
            for(int i = 0 ; i < N ; i++)par[i] = i;
            for(int i = 0 ; i < N ; i++)rank[i] = 0;
        }

        int root(int x){
            if(par[x] ==x)return x;
            return par[x] = root(par[x]);
        }

        void unite(int x, int y){
            int rx = root(x);
            int ry = root(y);
            if(rx == ry)return;
            if(rank[rx] < rank[ry])par[rx] = ry;
            else{
                par[ry] = rx;
                if(rank[rx] == rank[ry])rank[rx]++;
            }   
        }

        bool same(int x, int y){return bool(root(x) == root(y));}
    };


    // 辺の定義
    struct Edge {
        int u;
        int v;
        T cost;
        Edge(int u , int v , T cost):u(u),v(v),cost(cost){}
        // 辺の重みで順序を定義
        friend partial_ordering operator <=>(const Edge& a , const Edge& b){return a.cost <=> b.cost;}
    };


    T sum;// 最小全域森の重みの総和
    vector<Edge> edges;//辺集合
    vector<Edge> used_edges;//最小全域森に使用される辺集合
    int V;//頂点数

    public:
    Kruskal(int V_):V(V_) { }
    // 無向辺を追加
    void add_edge(int u , int v , T cost){edges.emplace_back(u,v,cost);}
    // 最小全域森を構築 (返り値は最小コスト)
    T msf(){
        sort(edges.begin(), edges.end()); // 辺の重みでソート
        UnionFind uft(V);
        sum = 0;
        for (Edge e : edges) {
            if (!uft.same(e.u, e.v)) { // 閉路にならなければ加える
                uft.unite(e.u, e.v);
                sum += e.cost;
                used_edges.push_back(e);
            }
        }
        return sum;
    }
    // msf の辺集合
    vector<Edge> used_Edges(){return used_edges;}
};




