#include "./../template/template.hpp"




// Copyright ©️ (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)
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




