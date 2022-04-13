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
    cin >> W >> H;
    vector<vector<char>> S(H,vector<char>(W));
    


    for(int i = 0 ; i < H ; i++){
        for(int j = 0 ; j < W ; j++){

            cin >> S[i][j];

        }

        
    }
    int player1 = -1;
    int player2 = -1;
    vector<vector<int> > G(H*W+10);
    for(int i = 0 ; i < H ; i++){
        for(int j = 0 ; j < W ; j++){
            if(S[i][j] == '1')player1 = i*W+j;
            if(S[i][j] == '2')player2 = i*W+j;

            if(S[i][j] != '#'){
                for(pair<int,int> next : dire){
                    int I = i + next.first;
                    int J = j + next.second;
                    if(isin(I,J) && S[I][J] != '#'){
                        G[i*W+j].push_back(I*W+J);
                    }
                }
            }

        }
    }   

    int inf = 1e8;
    vector<int> dist_pl1(H*W+10,1e8);//1から見た時の距離
    vector<int> dist_pl2(H*W+10,1e8);//2から見た時の距離   

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

    vector<bool> is_reachable(H*W+10 , 0);//２から見て到達可能か

    stack<int> st;
    st.push(player2);

    is_reachable[player2] = 1;

    while(!st.empty()){
        int now = st.top();
        st.pop();
        if(dist_pl2[now] + 1 >= dist_pl1[now]){
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

    for(int x = 0 ; x < H*W ; x++){
        if(is_reachable[x] && ans < dist_pl1[x]){
            ans = dist_pl1[x];
        }
    }

    cout << ans << endl;

    

}


