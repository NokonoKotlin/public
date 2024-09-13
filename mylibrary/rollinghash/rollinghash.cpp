#include "./../template/template.hpp"



//Copyright ©️ (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)
// 32 bit ローリングハッシュ (Rolling Hash Int)
template<long long MOD>
class _rhi{
    private:
    long long base;
    vector<long long> power,prefix_hash;
    public:
    _rhi(){}
    _rhi(string s , long long base_):base(base_){
        power = vector<long long>(s.size()+1);
        prefix_hash = vector<long long>(s.size()+1);
        power[0] = 1;
        for(int i = 1 ; i <= s.size() ; i++)power[i] = power[i-1]*base%MOD;
        prefix_hash[0] = 0;
        for(int i = 0 ; i < s.size() ; i++)prefix_hash[i+1] = (prefix_hash[i]*base + (s[i] - 'a' + 1) )%MOD ;
    }
    //半開区間 [ l , r ) のハッシュ
    long long hash(int l , int r){return ((prefix_hash[r] - prefix_hash[l]*power[r-l] )%MOD + MOD)%MOD;}
};  

//Copyright ©️ (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)
// (b1,m1) , (b2,m2) の 2 つのハッシュをマージするローリングハッシュ
// m1*m2 < (1ll<<62) 
template<long long m1 , long long m2>
class RollingHashLL{
    long long b1,b2;
    _rhi<m1> R1;
    _rhi<m2> R2;
    public:
    RollingHashLL(){}
    RollingHashLL(string s , long long b1_ , long long b2_):b1(b1_),b2(b2_){
        R1 = _rhi<m1>(s,b1);
        R2 = _rhi<m2>(s,b2);
    }
    // 半開区間 [ l , r ) のハッシュ。
    // { (b1,m1) のハッシュ , (b2,m2) のハッシュ } の組を整数値に変換
    long long hash(int l , int r){return R1.hash(l,r)*m2 + R2.hash(l,r);}
};




