#ifndef MODLIB
#define MODLIB 
#include "MyTemplate.hpp"

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
        modint<MOD>& operator =(long long t){return (*this) = modint<MOD>(t); }

        modint<MOD>& operator +=(long long t){return (*this) = this->v + t%MOD;}

        modint<MOD>& operator -=(long long t){return (*this) = this->v - t%MOD;}
        

        modint<MOD>& operator *=(long long t){return (*this) = this->v*(t%MOD);}

        modint<MOD>& operator /=(long long t){return (*this) = moddiv(*this,modint<MOD>(t));}


        modint<MOD> pow(long long x){return modpow((*this) , x);}

        modint<MOD> inv(){return modinv((*this));}

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

        // 比較 を modint 用に特殊化しておく
        // 内容は特に意味のない値(未定義)
        friend bool operator <(modint<MOD> a , modint<MOD> b){return a.v < b.v;}
        friend bool operator <=(modint<MOD> a , modint<MOD> b){return a.v <= b.v;}
        friend bool operator >(modint<MOD> a , modint<MOD> b){return a.v > b.v;}
        friend bool operator >=(modint<MOD> a , modint<MOD> b){return a.v >= b.v;}

        
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

    

    /*
        NTT(数論変換)
        Z/modZ での変換
        mod の値を変えると、色々変える必要がある
        ll MOD = 998244353;// Z/modZ で計算

        MOD = 998244353 = 119×2^23 + 1 の場合、
        ll A = 119;//係数(MODの値による)
        ll power = 23; //2^power までの長さに対応できる。MODの値による
        ll base = 3;
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




        // 数列 a を Z/modZで フーリエ変換(数論変換する), inv = 1なら逆変換
        // いつかわかりやすい実装に書き直す
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
                for(int bit = 0 ; bit < bitsize ;bit++)if(i&(1<<bit)) j |= (1<<(bitsize-1-bit)); 
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
                    mint et =  g[x][j];
                    if(inv){
                        if(j==0)et = g[x][0];//GroupeSize乗 は 0 乗と同じ
                        else et = g[x][GroupSize-j];
                    }
                    
                    for(int k = 0 ; k < sz ; k+= GroupSize){
                        mint p , q;
                        p = a[j+k];
                        q = a[j+k+GroupSize_Half]*et;
                                        
                        a[j+k] = (p + q);//前半の計算
                        a[j+k+GroupSize_Half] = (p - q);//後半の計算
                    }
                }   
            }
            if(inv)for(int i = 0 ;i < sz ; i++)a[i]/=sz;
            return a;
        }
    

        // a,bを畳み込み　　-->  c[t] = Σ a[i]*b[t-i] .. (0<=i<=t) となるような数列：c を計算する
        vector<mint> convolution(vector<mint> a ,vector<mint> b){
            int sz = int(a.size()) + int(b.size()) - 1;//畳み込みの結果のサイズ
            int t = 1;//計算に使うサイズに整形
            while(t < sz)t*=2;
            a.resize(t,mint(0));
            b.resize(t,mint(0));
            
            vector<mint> A,B;
            vector<mint> C(t);
            
            A = FFT(a,false);
            B = FFT(b,false);
            for(int i = 0 ; i < t ; i++)C[i] = A[i]*B[i];
            
            return FFT(C , true);
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



    /*
        形式的冪級数ライブラリ : 
        x^i の係数を A[i] と置くので、Aのサイズは常に 1 以上
        この式は A.size()-1 次式である

        しかし、係数を変化させたりすると次数も変化する可能性があるので(最高次係数を 0 にした場合、次数が減る)
        == 演算などで、正確な次数が必要になったタイミングで format 関数を呼び出して A を整形する

        format() := A.back()が 0 であるかぎり、pop_back する。ただし、A[0] = 0の場合はpopしない
        ~~~~~~~~format()は自分自身への自分自身への参照を返すことに注意!!!!~~~~~~~~~~~~~~~~~
        ~~~~~~~~コピー演算は format しない (参照を受け取る実装の場合) !!  ~~~~~~~~~~~~~~~~~

        バグをなくすため、基本的にオブジェクトが呼び出される全てのタイミングで format() を呼び出すようにする
        ただし、[] 演算子で領域外にアクセスした場合は 係数 0 の領域を確保し、そのタイミングに限って format() されていない状態を許容する
        [] 演算子は、要素の参照を返す(代入などができる)

        !!!!!!!!!!!!!!!基本的に、format() 後の A に準拠するようにしてください!!!!!!!!!!!!!!!!!
        [] などでアクセスした後はなるべく自力でformat()してください!!!

        次数の上限 deg_max を 設定する!!!
        次数が deg_max を超える部分は切り捨てる。
        余分に設定しておく方が良いが、遅い場合は set_max_deg で必要な分だけ確保する。

        deg_max を小さくした場合、それ以前に計算した deg_max より大きい項は 0 になるので注意
        新たに FPS を生成するときも、set_deg を忘れないで
    */
    template<long long mod,long long A_fft ,long long base_fft ,long long power_fft , FastFourierTransform<mod, A_fft , base_fft , power_fft> &calculator>
    class FormalPowerSeries {
        private:
        using mint = modint<mod>;
        // 過疎ってる FPS : (次数,係数) のペアの vector (早め)
        typedef vector<pair<long long , mint> > SparseFPS;

        
        //バグ取りのため、for文などは A.size()に従う
        //係数 -- a[i] := x^i の項の係数 (サイズは常に1以上)
        vector<mint> A;

        // 次数の上限 (FFT の上限と関連)
        int deg_max = 2000000;




        /*
            内部処理用の関数
        */
        /*
            A の係数を書き換えると、字数も変化する可能性がある(最高次係数を 0 にした場合など)
            == 演算などで、正確な次数が必要になったタイミングで format 関数を呼び出して N と A を整形する
            format() := A.back()が 0 であるかぎり、pop_back する。ただし、A[0] = 0の場合はpopしない  
        */
        FormalPowerSeries& format(){//自分自身を返すようにする
            while(this->A.size() > 1 && this->A.back()==0)this->A.pop_back();
            while(this->A.size() > deg_max+1)this->A.pop_back();// 大事
            return *this;
        }


        // 先頭 s 項のコピーを取り出す (つまり、*this mod x^s を返す)
        FormalPowerSeries prefix(int s){
            FormalPowerSeries res((*this));    
            res.set_max_deg(deg_max);
            s = max(s,1);
            res.A.resize(s,mint(0));
            return res;
        }

        /*
            mod x^k でのthisの逆元を求める。
            つまり、this*g = 1 mod x^k であるような g を求める
        */
        FormalPowerSeries inv(int k){
            if((*this)[0] == 0)assert(0);
            FormalPowerSeries res;
            res.set_max_deg(deg_max);
            res[0] = (*this)[0].inv();
            for(int i = 1; i < k ; i *=2 ) {  
                FormalPowerSeries tmp = *this;
                res = res*2 - ((tmp.prefix(i*2)*res).prefix(i*2)*res).prefix(i*2);
            }
            return res.prefix(k);
        }

        public:
        
        /*
            ----------------------------------------------------------------------------------------------------------------
            コンストラクタなど
            ----------------------------------------------------------------------------------------------------------------
        */


        FormalPowerSeries():A(1,mint(0)){}    
        FormalPowerSeries(vector<long long> A_){
            this->A.resize(A_.size());
            for(int i = 0 ; i < A.size() ; i++)A[i] = A_[i];
            this->format();
        }    
        FormalPowerSeries(vector<mint> A_){
            this->A = A_;
            this->format();
        }    

        void set_max_deg(int mx){
            deg_max = mx;
            this->format();
        }

        // F に代入 (ベーシックな演算なので、他の関数を使いたくない)
        // A 以外の変数(deg_maxなど)は変えない
        // format しない!!!!
        FormalPowerSeries& operator = (FormalPowerSeries F){
            if(F.A.size() == 0){
                this->A.resize(1);
                (*this)[0] = mint(0);
                return *this;
            }
            const int deg = min(int(F.A.size()) - 1 , deg_max);
            this->A.resize(deg+1,mint(0));
            for(int i = 0 ; i <= deg ; i++ )this->A[i] = F[i];
            
            return *this;
        }    

        /*
            ----------------------------------------------------------------------------------------------------------------
            各種オペレーター
            ----------------------------------------------------------------------------------------------------------------
        */


        //出力
        friend std::ostream& operator <<(std::ostream& stream,FormalPowerSeries F){
            stream << F.degree() <<"次式 : " ;
            for(int i = 0 ; i < F.size() ; i++){
                stream << F[i] << "x^"<<i ;
                if(i < F.size()-1)stream << " + ";
            }
            return stream;
        }


        
        /*
            i 項目の係数のアドレスにアクセス(代入可能)
            領域外なら 領域を拡張して、0 を返す。領域外は　0*x^i と考えられるから
            A のサイズは、format済みのAのサイズ以上である(当然ですが)
        */
        mint& operator [](int i){
            assert(i <= deg_max);
            if(i >= this->A.size())this->A.resize(i+1,mint(0));
            return this->A[i];
        }



        //Fに代入(vector a) x^i の係数 = a[i]
        FormalPowerSeries& operator = (vector<mint> a){
            *this = FormalPowerSeries(a);
            return *this;
        } 

        //Fに代入(vector a) x^i の係数 = a[i]
        FormalPowerSeries& operator = (vector<long long> a){
            *this = FormalPowerSeries(a);
            return *this;
        }       


        //二つの多項式が等しいかどうか
        bool operator == (FormalPowerSeries F){
            //最高次係数が0の場合もあるので、formatしてから判定しないといけない(degree()関数がformat()してくれる)
            if(this->degree() != F.degree())return false;
            bool res = true;
            for(int i = 0 ; i < (*this).size();i++)if((*this)[i] != F[i]){ res = false; break; }

            return res;
        }

        //Fの係数を自身の係数に加算する
        FormalPowerSeries& operator +=(FormalPowerSeries F){
            int deg = min(F.degree() , deg_max);
            for(int i = 0 ; i <= deg ; i++ )(*this)[i] = ((*this)[i] + F[i]);
            return this->format();
        }     

        //自身に整数 p を足す
        FormalPowerSeries& operator +=(mint p){
            (*this)[0] = ((*this)[0] + p);
            return this->format();
        }

        // 引き算
        FormalPowerSeries& operator -=(FormalPowerSeries F){
            int deg = min(F.degree() , deg_max);
            for(int i = 0 ; i <= deg ; i++ )(*this)[i] = ((*this)[i] - F[i]);
            
            return this->format();
        }     

        //定数項に整数 p を足す
        FormalPowerSeries& operator -=(mint p){
            (*this)+=(p*mint(-1));
            return this->format();
        }


        //自身に多項式 F をかける
        FormalPowerSeries& operator *=(FormalPowerSeries F){
            int deg_this = (*this).degree();
            int deg_F = F.degree();
            vector<mint> C = calculator.convolution(this->format().A , F.format().A);
            std::swap(this->A , C);
            return this->format();
        }


        // 自身に x^p をかける (シフト)
        FormalPowerSeries& operator <<= (int p){
            vector<mint> nA( min<int>(p + this->A.size() , deg_max+1) , 0);
            for(int i = p ; i < nA.size() ; i++)nA[i] = this->A[i-p];
            this->A = nA;
            return this->format();
        }

        // 自身を x^p でわり、あまりは切り捨て
        FormalPowerSeries& operator >>= (int p){
            vector<mint> nA( min<int> (this->A.size() - p , deg_max+1) , 0);
            for(int i = 0 ; i < this->A.size() ; i++){
                if(i - p < 0)continue;
                nA[i-p] = this->A[i];
            }
            this->A = nA;
            return this->format();
        }


        //自身に整数 p をかける
        FormalPowerSeries& operator *=(mint p){
            for(int i = 0 ; i < (*this).size() ; i++ )(*this)[i] = (*this)[i]*p;
            
            return this->format();
        }


        // 過疎っている多項式をかける vpll (first : 次数 , second 係数)
        FormalPowerSeries &operator *=(SparseFPS F) {
            FormalPowerSeries res(vector<mint>(1,mint(0)));
            for(pair<long long ,mint>& y: F){
                FormalPowerSeries tmp(*this);
                tmp<<=y.first;
                res += tmp*y.second;
            }
            *this = res;
            return this->format();
        }


        //自身を多項式 F で割る (1/Fをかける)
        FormalPowerSeries& operator /=(FormalPowerSeries F){
            assert(F.size() != 0);
            if(F.size() > (*this).size()){
                while((*this).size()>1)this->A.pop_back();
                (*this)[0]=mint(0);
                return *this;
            }

            const int k = this->size() - F.size() + 1;
            int r = 0;// 定数項が 0 である式で割るのを避ける
            while(F[r] == 0)r++;;
            F >>= r;
            F = F.inv(k+r);
            *this *= F;
            (*this)>>=r;
            return *this = this->prefix(k);
        }


        //自身を整数 p で割る (1/pをかける)
        FormalPowerSeries& operator /=(mint p){
            (*this)*=p.inv();
            return this->format();
        }

        
        // 過疎っている多項式で割る vpll (first : 次数 , second 係数)
        FormalPowerSeries &operator/=(SparseFPS F) {
            while(this->A.size() > deg_max + 1)this->A.pop_back();
            int n = (*this).size();
            pair<long long,mint> p = F.front();
            mint ic = p.second.inv();
            F.erase(F.begin());
            for(int i = 0 ; i < n ; i++ ) {
                for (pair<long long , mint>& y : F) {
                    int j = y.first;
                    mint b = y.second;
                    if (j > i) break;
                    (*this)[i] = ((*this)[i] - (*this)[i-j] * b);
                }
                (*this)[i] *= ic;
            }
            return this->format();
        }
        
        // F^-1 の次数制限ギリギリまで取得
        FormalPowerSeries f_inv(){
            return inv(deg_max+1);
        }

    
        FormalPowerSeries& operator +=(long long p){return (*this) += mint(p);}
        FormalPowerSeries& operator -=(long long p){return (*this) -= mint(p);}
        FormalPowerSeries& operator *=(long long p){return (*this) *= mint(p);}
        FormalPowerSeries& operator /=(long long p){return (*this) /= mint(p);}

        friend FormalPowerSeries operator +(FormalPowerSeries F , FormalPowerSeries G){return F+=G;}
        friend FormalPowerSeries operator -(FormalPowerSeries F , FormalPowerSeries G){return F-=G;}
        friend FormalPowerSeries operator *(FormalPowerSeries F , FormalPowerSeries G){return F*=G;}
        friend FormalPowerSeries operator /(FormalPowerSeries F , FormalPowerSeries G){return F/=G;}

        friend FormalPowerSeries operator +(FormalPowerSeries F , long long p){return F+=p;}
        friend FormalPowerSeries operator -(FormalPowerSeries F , long long p){return F-=p;}
        friend FormalPowerSeries operator *(FormalPowerSeries F , long long p){return F*=p;}
        friend FormalPowerSeries operator /(FormalPowerSeries F , long long p){return F/=p;}

        friend FormalPowerSeries operator +(FormalPowerSeries F ,SparseFPS G){for(pair<long long,mint> &a : G)F[a.first]+=a.second;return F;}
        friend FormalPowerSeries operator +(SparseFPS G , FormalPowerSeries F){return F+G;}
        friend FormalPowerSeries operator *(FormalPowerSeries F ,SparseFPS G){return F*=G;}
        friend FormalPowerSeries operator *(SparseFPS G , FormalPowerSeries F){return F*G;}
        friend FormalPowerSeries operator /(FormalPowerSeries F ,SparseFPS G){return F/=G;}

        friend FormalPowerSeries operator +(long long p , FormalPowerSeries F){return F+p;}
        friend FormalPowerSeries operator -(long long p , FormalPowerSeries F){return (F-p)*mint(-1);}
        friend FormalPowerSeries operator *(long long p , FormalPowerSeries F){return F*p;}


        //この式が何次式かを返す(formatされた N を返す)
        int degree(){return int(this->format().A.size())-1;}

        //Aが確保している領域(format済み)
        int size(){return int(this->format().A.size());}

        // 次数の上限
        int degree_limit(){return deg_max;}

    };

}


using mint998 = modlib::modint<998244353>;
using mint1e9_7 = modlib::modint<1000000007>;

// 過疎ってる FPS : (次数,係数) のペアの vector (早め)
template<long long mod>
using SparseFPS = vector<pair<long long , modlib::modint<mod>>>;

using SparseFPS998 = vector<pair<long long , modlib::modint<998244353>>>;
using SparseFPS1e9_7 = vector<pair<long long , modlib::modint<1000000007>>>;

modlib::FastFourierTransform<998244353,119,3,23> fft998244353;
modlib::FastFourierTransform<1000000007,1,1,1> fft_nansence;
// 1000000007 は ntt friendly じゃないのですが、クラス設計の都合上テンプレートに渡さないといけない

using FPS998 = modlib::FormalPowerSeries<998244353,119,3,23,fft998244353>;
using FPS1e9_7 = modlib::FormalPowerSeries<1000000007,1,1,1,fft_nansence>;



void TDPC_A_sparse(){
    int n;cin >> n;
    vll p(n);CinVec(p);
    FPS998 F;
    F[0] = 1;
    F.set_max_deg(10010);
    for(ll t :p){
        SparseFPS998 G = {{0,1} , {t , 1}};
        F *= G;
    }
    ll ans = 0;
    for(int x = 0 ; x < 10010 ; x++)if(F[x] != 0)ans++;
    
    cout << ans << endl;
}


void polydiv(){
    int n , m;cin >> n >> m;
    FPS998 F, G;
    for(int i = 0 ; i <= n ; i++)cin >> F[n-i];
    for(int i = 0 ; i <= n+m ; i++)cin >> G[n+m-i];

    FPS998 ans = (G/F);
    
    REP(i,m+1)if(ans[m-i].v < 450000000)cout << ans[m-i] << " ";else cout << ans[m-i].v-998244353 << " ";
    cout << endl;      
}


// ABC084 - D
void prefix_sum(){
    int Q;cin >> Q;
    FPS998 F,G;
    const int Mx = 200021;
    auto isprime = [](const int p){
        if(p == 1)return false;
        for(int x = 2 ; x*x <= p ; x++)if(p%x == 0)return false;
        return true;
    };
    for(int x = 1 ; x <= Mx ; x++){
        if(x%2 == 1 && isprime(x) && isprime((x+1)/2))F[x]=1;
    }
    
    G[0] = 1;
    G[1] = -1;
    F /= G;
    while(Q-->0){
        int l,r;cin >> l >> r;
        cout << F[r]-F[l-1] << endl;
    }

}




#endif