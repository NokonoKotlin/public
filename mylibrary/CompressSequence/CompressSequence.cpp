#include "./../template/template.hpp"






/*
    ランレングス圧縮
    vector<long long> か string を (連続数 , 要素) の vector に変換
    string は文字を long long　にキャストする

    ex. {1,1,1,1,4,3,3,3,5,5,2,2} -> {(4,1),(1,4),(3,3),(2,5),(2,2)}
*/
template<class VecOrStr>
vector<pair<long long ,long long>> sequence(VecOrStr x){
    vector<pair<long long ,long long>> s;
    bool flag = 0;//今連続しているかどうか
    long long now;
    long long seq = 1;
    long long next;
    for(int i = 0 ; i < x.size() ; i++){
        if(i+1 >= x.size())s.push_back(make_pair(seq , (long long)x[i]));// 最後の要素
        else{
            now = (long long)x[i];
            next = (long long)x[i+1];
            if(flag){
                if(now == next)seq++;
                else{
                    s.push_back(make_pair(seq , now));
                    seq = 1;
                    flag = 0;
                }
            }else{
                if(now == next){
                    seq++;
                    flag = 1;
                }else{
                    s.push_back(make_pair(seq , now));
                    seq = 1;  
                }
            }
        }
    }
    return s;
}





