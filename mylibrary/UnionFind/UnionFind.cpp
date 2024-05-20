#include "./../template/template.hpp"





/* 
    - ただの UnionFind (rollback 付き)
    - 注意点
        - UF 木をマージするデータ構造であり、辺をつなげると捉えるとバグる。  
        - unite(u,v) で u , v の属する UF 木が同じ場合はそもそも unite を呼び出さず、rollback にもストックされない。  
*/
template<bool _rollback_ = false>
class UnionFind{
    private:
    // メンバ
    int N;// 要素数 (0 ~ N-1)
    vector<int> par;// [x] := UF 木における頂点 x の親
    stack<pair<int,int>> history;// {x,y} := x の親を y にしたという履歴を新しい順に保存
    /* 以降の配列は UF 木の根のみがアクセス可能*/
    vector<int> size;// [x] := x を根とする UF 木の要素数 (マージするときに参照する)
    public:
    UnionFind() {}
    UnionFind(ll N_){init(N_);}
    // 値が init_ で初期化された N 個のノードを用意
    void init(int N_){
        N = N_;
        size = vector<int>(N,1);
        par = vector<int>(N);
        for(int i = 0 ; i < N ; i++)par[i] = i;
    }
    // 頂点 x が属する UF 木の根。連結成分の代表と読み替えて OK。経路圧縮アリ
    int root(int x){if(par[x] ==x)return x;if(_rollback_ == false)par[x] = root(par[x]);return root(par[x]);}
    // x , y が同じ UF 木に属するか。同じ連結成分と読み替えて OK
    bool same(int x, int y){return root(y) == root(x);}
    // u , v の属する UF 木をマージ。u,v を同じ連結成分として扱うと読み替えて OK 
    bool unite(int x, int y){
        if(same(x,y))return false;
        int rx = root(x);int ry = root(y);
        if(size[rx] < size[ry])swap(ry,rx);
        par[ry] = rx;
        size[rx] += size[ry];
        if(_rollback_)history.push({ry,rx});
        return true;
    }
    // 最後に行った unite を取り消す。 _rollback_ = true を指定してある必要あり
    // 同じ連結成分に属する u,v の unite(u,v) は履歴に計上されていないので、スルーしてさらに前の unite を rollback する。
    void rollback(){ 
        assert(_rollback_);
        if(history.empty())return;
        pair<int,int> last = history.top();history.pop();
        size[last.second] -= size[last.first];
        par[last.first] = last.first;
    }
    // x と同じ UF 木に属する要素を O(N) で計算。おそい。
    vector<int> member(int x){
        vector<int> res;for(int y = 0 ; y < N ; y++)if(root(x) == root(y))res.push_back(y);
        return res;
    }
    // x と same なノードの個数 (連結成分サイズ)
    int Size(int x){return size[root(x)];}
};

