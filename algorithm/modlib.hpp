
//include
//------------------------------------------
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <stack>
#include <bitset>
#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cctype>
#include <string>
#include <cstring>
#include <ctime>
#include<queue>
#include<complex>
#include <cassert>
using namespace std;
typedef long long ll;








namespace modlib{
    

    template<long long MOD>
    struct modint{
        long long v = 0;

        modint(){}
        modint(int v_){
            v = v_%MOD;
            if(v<0)v+=MOD;
        }
        modint(long long v_){
            v = v_%MOD;
            if(v<0)v+=MOD;
        }

        modint<MOD> modpow(modint<MOD> a , long long b){
            modint<MOD> res = modint<MOD>(1);
            modint<MOD> acum = a;
            while(b > 0){
                if(b&(1ll))res*=acum;
                acum*=acum;
                b>>=1;
            }
            return res;
        }

        modint<MOD> modinv(modint<MOD> b_){
            assert(b_!=0);
            long long b = b_.v;
            long long x = MOD;
            modint<MOD> u = modint<MOD>(1);
            modint<MOD> v = modint<MOD>(0);
            while (x!=0) {
                long long t = b / x;
                b -= t * x; 
                swap(b, x);
                u -= t * v; 
                swap(u, v);
            }
            assert(u*b_ == 1);
            return u;
        }

        modint<MOD> moddiv(modint<MOD> a , modint<MOD> b){
            return a*modinv(b);
        }
            
        // 代入はデフォルトコピーに任せる(遅くなったりしたら自分で書く)
        modint<MOD>& operator =(long long t){
            (*this) = modint<MOD>(t);
            return *this;
        }

        modint<MOD>& operator +=(long long t){
            *this = this->v + t%MOD;
            return (*this);
        }

        modint<MOD>& operator -=(long long t){
            *this = this->v - t%MOD;
            return (*this);
        }

        modint<MOD>& operator *=(long long t){
            *this = this->v*(t%MOD);
            return (*this);
        }

        modint<MOD>& operator /=(long long t){
            *this = moddiv(*this,modint<MOD>(t));
            return (*this);
        }


        modint<MOD> pow(long long x){
            return modpow((*this) , x);
        }

        modint<MOD> inv(){
            return modinv((*this));
        }


        // テンプレートを許さない
        modint<MOD>& operator +=(modint<MOD> t){return (*this)+=t.v;}
        modint<MOD>& operator -=(modint<MOD> t){return (*this)-=t.v;}
        modint<MOD>& operator *=(modint<MOD> t){return (*this)*=t.v;}
        modint<MOD>& operator /=(modint<MOD> t){return (*this)/=t.v;}

        friend modint<MOD> operator +(modint<MOD> a , modint<MOD> b){return a+=b;}
        friend modint<MOD> operator -(modint<MOD> a , modint<MOD> b){return a-=b;}
        friend modint<MOD> operator *(modint<MOD> a , modint<MOD> b){return a*=b;}
        friend modint<MOD> operator /(modint<MOD> a , modint<MOD> b){return a/=b;}

        friend modint<MOD> operator +(modint<MOD> a , long long b){return a+=b;}
        friend modint<MOD> operator -(modint<MOD> a , long long b){return a-=b;}
        friend modint<MOD> operator *(modint<MOD> a , long long b){return a*=b;}
        friend modint<MOD> operator /(modint<MOD> a , long long b){return a/=b;}

        friend modint<MOD> operator +(modint<MOD> a , int b){return a+(long long)b;}
        friend modint<MOD> operator -(modint<MOD> a , int b){return a-(long long)b;}
        friend modint<MOD> operator *(modint<MOD> a , int b){return a*(long long)b;}
        friend modint<MOD> operator /(modint<MOD> a , int b){return a/(long long)b;}

        friend modint<MOD> operator +(long long b , modint<MOD> a){return modint<MOD>(b) + a;}
        friend modint<MOD> operator -(long long b , modint<MOD> a){return modint<MOD>(b) - a;}
        friend modint<MOD> operator *(long long b , modint<MOD> a){return modint<MOD>(b) * a;}
        friend modint<MOD> operator /(long long b , modint<MOD> a){return modint<MOD>(b) / a;}

        friend modint<MOD> operator +(int b , modint<MOD> a){return modint<MOD>(b) + a;}
        friend modint<MOD> operator -(int b , modint<MOD> a){return modint<MOD>(b) - a;}
        friend modint<MOD> operator *(int b , modint<MOD> a){return modint<MOD>(b) * a;}
        friend modint<MOD> operator /(int b , modint<MOD> a){return modint<MOD>(b) / a;}





        friend bool operator ==(modint<MOD> a , modint<MOD> b){return a.v == b.v;}
        friend bool operator ==(modint<MOD> a , long long b){return a.v == b;}
        friend bool operator ==(modint<MOD> a , int b){return a.v == (long long)b;}
        friend bool operator ==(long long b , modint<MOD> a){return a == b;}
        friend bool operator ==(int b , modint<MOD> a){return a == (long long)b;}


        friend bool operator !=(modint<MOD> a , modint<MOD> b){return !(a==b);}
        friend bool operator !=(modint<MOD> a , long long b){return !(a == b);}
        friend bool operator !=(modint<MOD> a , int b){return !(a == b);}
        friend bool operator !=(long long b , modint<MOD> a){return !(a == b);}
        friend bool operator !=(int b , modint<MOD> a){return !(a == b);}


        
        friend ostream& operator << (ostream& os, const modint& x){
            os << x.v;
            return os;
        }

        friend istream& operator >> (istream& is, modint& x){
            is >> x.v;
            x.v%=MOD;
            if(x.v < 0)x.v += MOD;
            return is;
        }

        operator long long(){return this->v;}

        
    };





    // 剰余環においては意味のない演算だが、Segment Tree 実装例などで定義してしまっているので、一応定義しておく。
    template<long long MOD>modint<MOD> max(modint<MOD> a , modint<MOD> b){return a;}
    template<long long MOD>modint<MOD> min(modint<MOD> a , modint<MOD> b){return a;}
    






    /*
        NTT(数論変換)
        Z/modZ での変換

        mod の値を変えると、色々変える必要がある

        ll MOD = 998244353;// Z/modZ で計算

        MOD = 998244353 = 119×2^23 + 1 の場合、
        ll A = 119;//係数(MODの値による)

        ll power = 23; //2^power までの長さに対応できる。MODの値による

        ll base = 3;//原始根のもと

        です
    */
    template<long long mod_fft = 998244353 , long long A_fft = 119 , long long base_fft = 3 , long long power_fft = 23>
    class FastFourierTransform{
        private:
        
        using mint = modint<mod_fft>;
        

        vector<vector<mint>> g;// g[i][x] := 1 の 2^i 乗根 の x 乗


        public:

        FastFourierTransform(){
            init();
        }

        void init(){
            //1 の 2^i乗根 gi を計算し、gi^xも前計算
            g = vector<vector<mint>>(power_fft);
            mint g_power = base_fft;
            g_power = g_power.pow(A_fft*2);
            for(int i = int(g.size())-1;i>=0;i--){
                g[i].resize((1<<i));
                mint now = 1;
                for(int x = 0 ; x < int(g[i].size()) ; x++){
                    g[i][x] = now;
                    now*=g_power;
                }
                g_power = g_power.pow(2);
            }
        }




        //数列 a を Z/modZで フーリエ変換(数論変換する), inv = 1なら逆変換
        vector<mint> FFT(vector<mint> a , bool inv = 0){
            int sz = a.size();

            //2の冪乗の形になるように整形
            int t = 1;
            while(t < sz)t*=2;
            
            a.resize(t);
            sz = a.size();
            int bitsize = int(log2(sz)+0.1); 

            for(int i = 0; i < sz ; i++){
                int j = 0;
                for(int bit = 0 ; bit < bitsize ;bit++){   
                    if(i&(1<<bit)) j |= (1<<(bitsize-1-bit)); 
                }
                if(i<j)swap(a[i] , a[j]);
            }


            /*
                分割統治の計算 (小さい方から)
            */
            for(int x = 1 ; x <= bitsize; x++){
                /*
                    演算グループのサイズはGroupSizeだが、実際の計算ではその半分のGroupSize_Halfしか使わない
                    なぜなら、後半半分の計算は前半半分の計算のついでにできるから
                */
                int GroupSize = (1<<x);
                int GroupSize_Half = GroupSize/2;

                for(int j = 0 ; j < GroupSize_Half ; j++ ){ 
                    //上からj番目の要素から出る辺の係数
                    mint et;
                    if(inv){
                        if(j==0)et = g[x][0];//GroupeSize乗 は 0 乗と同じ
                        else et = g[x][GroupSize-j];
                    }else et = g[x][j];

                    for(int k = 0 ; k < sz ; k+= GroupSize){
                        mint p , q;

                        p = a[j+k];
                        q = a[j+k+GroupSize_Half]*et;
                        
                        //前半の計算
                        a[j+k] = (p + q);
        
                        //後半の計算
                        a[j+k+GroupSize_Half] = (p - q);

                    }
                }   
            }
            if(inv){
                for(int i = 0 ;i < sz ; i++)a[i]/=sz;
            }
            return a;
        }



        /*
            a,bを畳み込み
            　　-->  c[t] = Σ a[i]*b[t-i] .. (0<=i<=t) となるような数列：c を計算する
        */
        vector<mint> convolution(vector<mint> a ,vector<mint> b){
            //畳み込みの結果のサイズ
            int sz = int(a.size()) + int(b.size()) - 1;
            
            //計算に使うサイズに整形
            int t = 1;
            while(t < sz)t*=2;
            a.resize(t,mint(0));
            b.resize(t,mint(0));
            
            vector<mint> A,B;
            vector<mint> C(t);
            
            A = FFT(a,false);
            B = FFT(b,false);
            for(int i = 0 ; i < t ; i++){
                C[i] = A[i]*B[i];
            }
            C = FFT(C , true);

            return C;
        }

        vector<ll> convolution(vector<ll> a ,  vector<ll> b){
            vector<mint> a_(a.size(),mint(0));
            vector<mint> b_(b.size(),mint(0));

            for(int i = 0 ; i < int(a.size()); i++)a_[i] = a[i];
            for(int i = 0 ; i < int(b.size()); i++)b_[i] = b[i];

            vector<mint> c_ = convolution(a_,b_);
            vector<ll> res(c_.size());  
            for(int i = 0 ; i < int(c_.size()) ; i++ )res[i] = c_[i].v;
            return res;
        }
    };



}


using mint998 = modlib::modint<998244353>;
using mint1e9_7 = modlib::modint<1000000007>;
modlib::FastFourierTransform<998244353,119,3,23> fft998244353;






