#include<iostream>
#include<vector>
#include<deque>
const long long INF = 1e18;
using namespace std;
int main(){
    int N ;
    cin >> N ;
    vector<vector<int > > G(N+1);
    vector<int> U(N-1),V(N-1);
    vector<double> cost(N-1);
    for(int i = 0 ; i < N -1 ; i++){
        int u,v;cin >> u >> v;
        double c;cin >> c;
        U[i] = u;
        V[i] = v;
        cost[i]=c;
        G[u].push_back(v);
        G[v].push_back(u);
    }
    vector<int> dist(N+1,N+1), SubTreeSize(N+1,1);
    deque<int> que;
    deque<int> dfs;
    int st = 1;
    que.push_back(st);
    dfs.push_back(st);
    dist[st] = 0;
    while(que.size()!=0){
        int now = que.back();
        que.pop_back();
        
        for( int next : G[now]){
            if(dist[next] >= N){
                dist[next] = dist[now]+1;
                que.push_back(next);
                dfs.push_back(next);
            }
        }
    }
    while(dfs.size()!=0){
        int now = dfs.back();
        dfs.pop_back();
        for(int next : G[now]){
            if(dist[next]< dist[now]){
                SubTreeSize[next] += SubTreeSize[now];
            }
        }
    }
   
    double ans = 0;
    for(int i = 0 ; i < N-1 ; i++){
        int u = U[i];
        int v = V[i];
        if(dist[u] > dist[v])swap(u,v);
        
        ans += (double)(N-SubTreeSize[v])*(double)SubTreeSize[v]*cost[i]*2/((double)N*(double)(N-1));
    }
    cout.precision(18);
    cout << ans << endl;
    
    
    return 0;
}
