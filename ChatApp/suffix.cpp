//include
//------------------------------------------
#include<iostream>
#include<vector>
#include<queue>
#include<map>
#include<stack>
#include<string>
using namespace std;
const long long INF = 1e18;


int H , W;


vector<pair<int ,int > > dire = {make_pair(0,1),make_pair(1,0),make_pair(-1,0),make_pair(0,-1)};


bool isin(int y , int x){
    return (y >= 0 && y < H && x >= 0 && x < W);
}

int main(){

    int n;
    int player1 = -1;
    int player2 = -1;
    int inf = 1e9;
    cin >> n >> player2 >> player1;

    vector<int> dist_pl1(n+1,inf);
    vector<int> dist_pl2(n+1,inf);
    vector<vector<int>> G(n+1);
    for(int i = 0 ; i < n-1 ; i++){
        int u , v ;
        cin >> u >> v;
        G[u].push_back(v);
        G[v].push_back(u);
    }

    stack<int> s1;
    s1.push(player1);
    dist_pl1[player1] = 0;

    while(!s1.empty()){
        int now = s1.top();
        s1.pop();
        for(int nx : G[now]){
            if(dist_pl1[nx]==inf){
                dist_pl1[nx] = dist_pl1[now]+1;
                s1.push(nx);
            }
        }
    }
    


    stack<int> s2;
    s2.push(player2);
    dist_pl2[player2] = 0;

    while(!s2.empty()){
        int now = s2.top();
        s2.pop();
        for(int nx : G[now]){
            if(dist_pl2[nx] == inf ){
                dist_pl2[nx] = dist_pl2[now]+1;
                s2.push(nx);
            }
        }
    }

    vector<bool> is_reachable(n+10 , 0);//２から見て到達可能か

    stack<int> st;
    st.push(player2);

    

    while(!st.empty()){
        int now = st.top();
        st.pop();
        if(dist_pl2[now] >= dist_pl1[now]){
            continue;
        }
        is_reachable[now] = 1;
        for(int nx : G[now]){
            if(dist_pl2[nx] == dist_pl2[now] + 1 ){
                st.push(nx);
            }
        }


    }

    int ans = 0;

    for(int x = 1; x < n+1 ; x++){
        if(is_reachable[x] && ans < dist_pl2[x] +abs(dist_pl1[x] - dist_pl2[x]) - 1){
          
            ans = dist_pl2[x] +abs(dist_pl1[x] - dist_pl2[x]) - 1;
        }
    }

    cout << ans << endl;

    

}


