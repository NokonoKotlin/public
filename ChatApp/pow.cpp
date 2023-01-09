
//include
//------------------------------------------
#include<iostream>
#include<vector>
#include<queue>
#include<map>
#include<algorithm>
using namespace std;

const long long INF = 1e18;





long long modpow(long long a, long long b , long long m){

    //bを二進数表示して、1になる位をbit配列に保存
    vector<int> bit;
    for(int i = 0 ; i <= 62;i++){
        if(b&(1ll<<i))bit.push_back(i);
    }

    //a^(2^i)の前計算
    vector<long long> power(63 , 0);
    power[0] = (a%m+m)%m;
    for(int x = 1 ; x <= 62 ; x++){
        power[x] = power[x-1]*power[x-1];
        power[x] %=m;
    }

    //対応するものをかける
    long long res = 1;
    for(int b : bit){
        res *= power[b];
        res%=m;
    }
    
    return res;
}


int main(){

    long long MOD = 1000000007; 
    long long a = 1561318923819;
    long long b = 1232123123212314;
    cout << modpow(a,b,MOD) << endl;

    return 0;
}   