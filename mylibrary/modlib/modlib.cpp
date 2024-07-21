#include "./../template/template.hpp"











/*
    modint , FFT , FPS ライブラリ
    - 設計&方針
        |- 参照する演算が複数存在するような状況を回避するため、modint と FPS のキャスト関係は作らない。
*/
namespace modlib{
    
    /*
        MOD つき整数。自動で mod をとってくれる。
        基本的な演算は整数型と同じような設計にしてあり、整数型との演算も行える。
        mod が素数の場合、全ての整数に逆数が存在する。  
            |- 「%」演算は意味をなさない (必ず 0)
        - 設計
            |- 整数型 -> modint のキャストは暗黙
                |- これにより、modint 同士の演算のみ定義しておけば、整数型との演算もカバーできる
            |- modint -> 整数型 のキャストは明示的
                |- 参照する演算が複数存在するような状況を回避するため。
    */
    template<long long MOD>
    struct modint{
        private:
        long long v = 0;
        public:
        modint(){}
        modint(long long v_){
            v = v_%MOD;
            if(v<0)v+=MOD;
        }
        long long val(){return v;}

        modint<MOD> pow(long long b){
            modint<MOD> a = (*this);
            modint<MOD> res = modint<MOD>(1);
            modint<MOD> acum = a;
            while(b > 0){
                if(b&(1ll))res*=acum;
                acum*=acum;
                b>>=1;
            }
            return res;
        }

        modint<MOD> inv() const& {
            assert(this->v%MOD != 0);
            modint<MOD> b_ = (*this);
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

        // - 符号をつけた値
        modint<MOD> operator -()const&{return modint<MOD>(this->v*(-1));}
    
        // modint の演算 (long long なども暗黙キャストされてここが呼ばれる)
        modint<MOD>& operator =(const modint<MOD>& t){this->v = t.v; return (*this); }
        modint<MOD>& operator +=(const modint<MOD>& t){return ((*this) = modint<MOD>(t.v + this->v));}
        modint<MOD>& operator -=(const modint<MOD>& t){return ((*this)+= (-t));}
        modint<MOD>& operator *=(const modint<MOD>& t){return ((*this) =modint<MOD>((this->v)*t.v));}
        modint<MOD>& operator /=(const modint<MOD>& t){return ((*this)*=t.inv());}
        friend modint<MOD> operator +(modint<MOD> a , modint<MOD> b){return a+=b;}
        friend modint<MOD> operator -(modint<MOD> a , modint<MOD> b){return a-=b;}
        friend modint<MOD> operator *(modint<MOD> a , modint<MOD> b){return a*=b;}
        friend modint<MOD> operator /(modint<MOD> a , modint<MOD> b){return a/=b;}
        friend bool operator ==(const modint<MOD>& a , const modint<MOD>& b){return a.v == b.v;}
        friend bool operator !=(const modint<MOD>& a , const modint<MOD>& b){return !(a==b);}        
        /*
            順序も定義するだけしておく (min/max や set/map に入れる用)
        */
        friend partial_ordering operator <=> (modint<MOD> a , modint<MOD> b){
            if(a.v == b.v)return partial_ordering::equivalent;
            else if(a.v < b.v)return partial_ordering::less;
            return partial_ordering::greater;
        }
        friend ostream& operator << (ostream& os, const modint<MOD>& x){
            os << x.v;
            return os;
        }
        friend istream& operator >> (istream& is, modint<MOD>& x){
            is >> x.v;
            x.v%=MOD;
            if(x.v < 0)x.v += MOD;
            return is;
        }
        // 明示的にしか変換してはいけない ( 参照する演算子の重複を避ける )
        explicit operator long long(){return this->v;}
    };




    /*
        nCr % MOD の計算
        計算量はO(r) . TODO : modfrac を作る
    */
    template<long long MOD>
    modint<MOD> modcomb(long long n , long long r){
        modint<MOD> N = 1;
        modint<MOD> K = 1;
        for(int i = 0 ; i < r ; i++){
            N*=modint<MOD>(n-i);
            K*=modint<MOD>(i+1);
        }
        return N/K;
    }

    /*
        log_a{b} mod MOD を計算する。
        - つまり、a^x == b mod MOD なる最小の非負整数 x を計算する。
            |- このような x は、存在するなら MOD 未満である (a の冪乗の周期より)
        - 存在しなければ RE とするので、存在判定は上位スコープで行う。
        離散対数問題と呼ばれます
    */
    template<long long MOD>
    modint<MOD> modlog(long long a , long long b){
        long long t = sqrt(MOD);
        vector<modint<MOD>> B(t);
        B[0] = b;
        unordered_map<long long,priority_queue<long long>> find;
        find[(long long)(B[0])].push(0);
        modint<MOD> a_inv = modint<MOD>(a).inv(); 
        for(int i = 1 ; i < B.size() ; i++ ){
            B[i] = B[i-1]*a_inv;
            find[(long long)(B[i])].push(-i);
        }
        long long rlim = MOD/t;
        modint<MOD> apt = 1;
        modint<MOD> at = modint<MOD>(a).pow(t);
        long long ans = MOD;
        for(int p = 0 ; p <= rlim ; p++ ){
            if(!find[(long long)(apt)].empty()){
                long long q = find[(long long)(apt)].top()*(-1);
                ans = min(p*t+q,ans);
            }
            apt *= at;
        }
        assert(0 <= ans && ans < MOD);
        return ans;
    }


    /* 
        素数 mod で高速なシグマ計算を行う。
        combination のための階乗テーブルを計算するので、指数の上限 K に対して前計算 O(Klog(MOD)) 時間
        - 上限 K が決まっていて、前計算 O(Klog(MOD)) , クエリ O(1) の combination の代わりとしても使える
        - Σ(i≦n) i^k の計算が O(k^2) 時間 (n に依存しない)
    */
    template<long long MOD>
    class FastSigma{
        // メモ化用
        // [k][n] := Σ(i≦n) i^k が計算ずみかどうか
        unordered_map<long long ,unordered_map<long long , bool>> ready;
        // [k][n] := Σ(i≦n) i^k の値
        unordered_map<long long ,unordered_map<long long , modint<MOD>>> memo;
        // 階乗 , 階乗の逆数 , 累乗 , 逆数 のテーブルを前計算
        vector<modint<MOD>> fac , fac_inv , inv;
        public:
        FastSigma(){}
        // 指数の上限を与える (計算のために少し余分に必要)
        FastSigma(long long k_max):fac(k_max+3),fac_inv(k_max+3),inv(k_max+3){
            fac[0] = modint<MOD>(1);
            for(long long k = 1 ; k < fac.size() ; k++)fac[k] = fac[k-1]*k;
            for(long long k = 0 ; k < fac.size() ; k++){
                fac_inv[k] = fac[k].inv();
                if(k >= 1)inv[k] = fac_inv[k] * fac[k-1];
            }
        }
        // nCr の計算 (変数名がこんがらがらないように注意)
        modint<MOD> nCr(const long long n ,const long long r){
            assert(0 <= n && n < fac.size());
            assert(0 <= r && r <= n);
            return (fac[n]*fac_inv[r])*fac_inv[n-r];
        }
        // Σ(i≦n) i^k を計算
        modint<MOD> F(const long long n ,const long long k){
            assert(1<=n);
            assert(0 <= k && k < fac.size());
            if(k == 0)return n;
            if(ready[n][k])return memo[n][k];
            modint<MOD> res = modint<MOD>(n+1).pow(k + 1) - 1;
            for(long long p = 0 ; p <= k + 1 - 2 ; p++)res -= this->nCr(k + 1,p)*this->F(n,p);
            memo[n][k] = res*inv[k+1];
            ready[n][k] = true;
            return memo[n][k];
        }
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

        /* 係数のテーブルを初期化 */        
        void init(){
            // 1 の 2^i乗根 gi を計算し、g[i][x] = gi^x も前計算
            g = vector<vector<mint>>(power_fft);
            mint g_power = base_fft;
            g_power = g_power.pow(A_fft*2);
            for(int i = int(g.size())-1;i>=0;i--){
                g[i].resize((1<<i)+1);// 0 乗根 から 2^i 乗根 (inclusive) までのテーブルを作る
                mint now = 1;
                for(int x = 0 ; x < int(g[i].size()) ; x++){
                    g[i][x] = now;
                    now*=g_power;
                }
                g_power *= g_power;
            }
        }

        // 数列 a を Z/modZで フーリエ変換(数論変換する), inv = true なら逆変換
        // 再帰で実装しているので、逆変換後にサイズで割る操作は外で行う!!!!! (めっちゃ注意)
        vector<mint> fft_sub(vector<mint> a , bool inv){
            if(a.size() == 1)return a;
            int N = 1; 
            int bitsize = 0;
            // 2 の冪乗の形になるように整形
            while(N < a.size()){
                N<<=1;
                bitsize++;
            }
            a.resize(N,0);
            vector<mint> tmp1,tmp2;
            for(int i = 0 ; i*2 < N ; i++){
                tmp1.push_back(a[2*i]);
                tmp2.push_back(a[2*i+1]);
            }
            tmp1 = fft_sub(tmp1,inv);
            tmp2 = fft_sub(tmp2,inv);
            
            for(int i = 0 ; i < N ; i++){
                mint w = g[bitsize][i];
                if(inv)w = g[bitsize][N-i];
                if(i < tmp1.size())a[i] = tmp1[i] + tmp2[i]*w;
                else a[i] = tmp1[i-tmp1.size()] + tmp2[i-tmp1.size()]*w;
            }
            return a;
        }
        public:
        FastFourierTransform(){init();}

        // 通常の fft 
        vector<mint> FFT(vector<mint>a){return fft_sub(a,false);}
        // fft の逆変換
        vector<mint> IFFT(vector<mint>a){
            a = fft_sub(a,true);
            // 実装の設計上、サイズで割る操作は fft_sub の外で行う
            for(int i = 0 ; i < a.size() ; i++)a[i] /= mint(a.size());
            return a;
        }

        // a,bを畳み込み　　-->  c[t] = Σ a[i]*b[t-i] .. (0<=i<=t) となるような数列：c を計算する
        // long long とかは mint に暗黙にキャストされるので, long long でも OK
        vector<mint> convolution(vector<mint> a ,vector<mint> b){
            int sz = int(a.size()) + int(b.size()) - 1;//畳み込みの結果のサイズ
            int t = 1;//計算に使うサイズに整形
            while(t < sz)t*=2;
            a.resize(t,mint(0));
            b.resize(t,mint(0));
            vector<mint> A,B;
            vector<mint> C(t);
            
            A = FFT(a);
            B = FFT(b);
            for(int i = 0 ; i < t ; i++)C[i] = A[i]*B[i];
            return IFFT(C);// 逆変換
        }
    };




    /*
        - 形式的冪級数ライブラリ : 自身のメンバを変化させるので、const 参照はできない。
        - x^i の係数を A[i] と置く。この式は A.size()-1 次式である。
            |- 係数は mod 付き整数
            |- 整数を定数項のみの多項式にキャストして扱うことで、実装の共通化をしている
                |- mod 付き整数 (剰余環) なので、0 以外の整数 (定数項のみの多項式) 
                   / に対して必ず逆数が存在するような場合がある (mod998244353の場合など)
                   / この時、定数項のみの多項式同士で「%」演算をしても、欲しい結果は得られない !!!!!! 
                   / 「%」演算はあくまで多項式の徐算である。係数は剰余環であることを忘れないでね
        - 多項式同士の演算ができる。
        |
        - 係数を変化させたりすると次数も変化する可能性がある (最高次係数を 0 にした場合、次数が減る)。
            |- 適切なタイミングで format 関数を呼び出して A を整形する
                |- format() := A.back()が 0 であるかぎり、pop_back する。ただし、A[0] = 0の場合はpopしない
                |- FPS の実装を含むメンバ関数の冒頭と、値を返す前に必ず呼ぶ。
            |- バグをなくすため、基本的にオブジェクトが呼び出される全てのタイミングで format() を呼び出すようにする
                |- 基本的に、format() 後の A に準拠するようにしてください
            |- [i] 演算子は x^i の次数のコピーを返す
            |- set(i,d) := x^i の係数を d にする 
                |- 0 を set すると format が発生するかもしれないのでかなり注意
            |- cin_and_set(i) := 入力から d を受け取り、x^i の係数を d にする 
        |
        - 次数の上限 deg_max を 設定する!!!
            |- 次数が deg_max を超える部分は切り捨てる。
            |- 余分に設定しておく方が良いが、遅い場合は set_max_deg で必要な分だけ確保する。
            |- deg_max を小さくした場合、それ以前に計算した deg_max より大きい項は 0 になるので注意
            |- 新たに FPS を生成するときも、set_deg を忘れないで
        |
        - 注意
            |- 整数も定数項のみの FPS (多項式) として扱う。
            |- オペレータは FPS と整数の掛け算,割り算に FFT,ダブリング を使うため、整数との演算では for 文を回すプログラムを書く方が良い
    */
    template<long long mod,long long A_fft ,long long base_fft ,long long power_fft , FastFourierTransform<mod, A_fft , base_fft , power_fft> &calculator>
    class FormalPowerSeries {
        private:
        using mint = modint<mod>;
        // 過疎ってる FPS : (次数,係数) のペアの vector (早め)
        typedef vector<pair<long long , mint> > SparseFPS;
                
        // for文などは A.size()に従う
        // 係数 -- a[i] := x^i の項の係数 (サイズは常に1以上)
        vector<mint> A;

        // 次数の上限。動的 (FFT の上限も考慮)
        // 必要な分だけ適切に確保する必要がある (set_max_deg())
        int deg_max = 2000000;

        /* ===== 内部処理用の関数===== */
        /* ===== 内部処理用の関数===== */
        /* ===== 内部処理用の関数===== */


        /*
            A の係数を書き換えると、次数も変化する可能性がある(最高次係数を 0 にした場合など)ので、正確な次数が必要な場合のために A を整形する
            format() := A.back()が 0 であるか、次数上限より大きいかぎり、pop_back する。ただし、A[0] = 0 は pop しない  
        */
        void format(){//自分自身を返すようにする
            while(this->A.size() > 1 && this->A.back()==0)this->A.pop_back();
            while(this->A.size() > deg_max+1)this->A.pop_back();// 大事
            return;
        }

        // 先頭 s 項のコピーを取り出す (*this mod x^s を返す)
        FormalPowerSeries prefix(int s){
            FormalPowerSeries res((*this));    
            res.set_max_deg(deg_max);
            s = max(s,1);
            res.A.resize(s,mint(0));
            return res;
        }
        // 先頭 s 項を左右反転した多項式のコピーを取り出す
        FormalPowerSeries rev(int s){
            FormalPowerSeries res((*this));
            res.set_max_deg(deg_max);
            res.A.resize(s,mint(0));
            reverse(res.A.begin(),res.A.end());
            return res;
        }

        // mod x^k でのthisの逆元を求める。(this*g = 1 mod x^k である g を求める)
        FormalPowerSeries inv(int k){
            if((*this)[0] == 0)assert(0);
            FormalPowerSeries res((*this)[0].inv());
            res.set_max_deg(deg_max);
            for(int i = 1; i < k ; i *=2 )res = res*2 - ((this->prefix(i*2)*res).prefix(i*2)*res).prefix(i*2);
            return res.prefix(k);
        }

        public:
        
        /*
            ----------------------------------------------------------------------------------------------------------------
            コンストラクタなど
            ----------------------------------------------------------------------------------------------------------------
        */
        FormalPowerSeries():A(1,mint(0)){}    
        FormalPowerSeries(vector<mint> A_){
            this->A = A_;
            this->format();
        }    

        // {Integer or modint} から FPS (定数項のみ) へのキャスト
        FormalPowerSeries(mint p):A(1,p){} 
        FormalPowerSeries(long long p):A(1,mint(p)){} 

        // vector から代入 (vector a) x^i の係数 = a[i]
        FormalPowerSeries& operator = (vector<mint> a){return (*this = FormalPowerSeries(a));} 

        // F をコピー (ベーシックな演算なので、他の関数を使いたくない)
        FormalPowerSeries& operator = (FormalPowerSeries F){
            this->A = F.A;
            this->deg_max = F.deg_max;
            return *this;
        }    

        // 次数の最大値を設定する。
        void set_max_deg(int mx){
            deg_max = mx;
            this->format();
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


        
        // x^i の係数を read-only で取得
        mint operator [](int i){
            // 多項式の次数より大きい範囲は 0 で埋まっている
            if(i >= this->A.size())return mint(0);
            return this->A[i];
        }
        
        // x^i の係数を d にする (mint は int などから暗黙キャスト可能)
        void set(int i , mint d){
            // 元と同じなら無視する。特に、0 に 0 を set し、resize & format で O(N) 時間かかるのを阻止
            if(d == (*this)[i])return;
            
            if(i >= this->A.size())this->A.resize(i+1,mint(0));
            this->A[i] = d;
            this->format();// 最高次が 0 になった場合
        }


        //Fの係数を自身の係数に加算する
        FormalPowerSeries& operator +=(FormalPowerSeries F){
            this->format();
            F.format();
            int deg = min(F.degree() , deg_max);
            for(int i = 0 ; i <= deg ; i++ )this->set(i,(*this)[i] + F[i]);
            this->format();
            return *this;
        }     

        // 引き算
        FormalPowerSeries& operator -=(FormalPowerSeries F){
            this->format();
            F.format();
            int deg = min(F.degree() , deg_max);
            for(int i = 0 ; i <= deg ; i++ )this->set(i,(*this)[i] - F[i]);
            this->format();
            return *this;
        }     


        //自身に多項式 F をかける
        FormalPowerSeries& operator *=(FormalPowerSeries F){
            this->format();
            F.format();
            int deg_this = (*this).degree();
            int deg_F = F.degree();
            vector<mint> C = calculator.convolution(this->A , F.A);
            std::swap(this->A , C);
            this->format();
            return *this;
        }

        // 自身に x^p をかける (シフト)
        FormalPowerSeries& operator <<= (int p){    
            this->format();
            vector<mint> nA( min<int>(p + this->A.size() , deg_max+1) , 0);
            for(int i = p ; i < nA.size() ; i++)nA[i] = this->A[i-p];
            this->A = nA;
            this->format();
            return *this;
        }

        // 自身を x^p でわり、あまりは切り捨て
        FormalPowerSeries& operator >>= (int p){
            this->format();
            vector<mint> nA( min<int> (this->A.size() - p , deg_max+1) , 0);
            for(int i = 0 ; i < this->A.size() ; i++){
                if(i - p < 0)continue;
                nA[i-p] = this->A[i];
            }
            this->A = nA;
            this->format();
            return *this;
        }


        // 過疎っている多項式をかける vpll (first : 次数 , second 係数)
        FormalPowerSeries &operator *=(SparseFPS F) {
            this->format();
            FormalPowerSeries res(vector<mint>(1,mint(0)));
            for(pair<long long ,mint>& y: F){
                FormalPowerSeries tmp(*this);
                tmp<<=y.first;
                res += tmp*y.second;
            }
            *this = res;
            this->format();
            return *this;
        }


        // this を「this = F*G + r」 である G に変更する
        // 注意 : this/F にするわけではない。
        FormalPowerSeries& operator /=(FormalPowerSeries F){
            this->format();
            F.format();
            assert(F.size() != 0);
            if(F.size() > (*this).size()){
                while((*this).size()>1)this->A.pop_back();
                this->set(0,mint(0));
                return *this;
            }
            const int k = this->size() - F.size() + 1;
            *this = (this->rev(this->size())*F.rev(F.size()).inv(k)).rev(k);
            return *this;
        }

        // this を「this = F*G + r」 である r に変更する
        FormalPowerSeries& operator %=(FormalPowerSeries F){
            this->format();
            F.format();
            assert(F.size() != 0);
            if(F.size() > (*this).size())return *this;
            (*this)-=((*this)/F)*F;
            this->format();
            return *this;
        }

        
        // 過疎っている多項式で割る vpll (first : 次数 , second 係数)
        FormalPowerSeries &operator/=(SparseFPS F) {
            this->format();
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
                    this->set(i,(*this)[i] - (*this)[i-j] * b);
                }
                this->set(i,(*this)[i]*ic);
            }
            this->format();
            return *this;
        }

        
        // F^-1 の次数制限ギリギリまでを取得
        FormalPowerSeries inv(){this->format();return inv(deg_max+1);}

        friend FormalPowerSeries operator +(FormalPowerSeries F , FormalPowerSeries G){return F+=G;}
        friend FormalPowerSeries operator -(FormalPowerSeries F , FormalPowerSeries G){return F-=G;}
        friend FormalPowerSeries operator *(FormalPowerSeries F , FormalPowerSeries G){return F*=G;}
        friend FormalPowerSeries operator /(FormalPowerSeries F , FormalPowerSeries G){return F/=G;}
        friend FormalPowerSeries operator %(FormalPowerSeries F , FormalPowerSeries G){return F%=G;}

        friend FormalPowerSeries operator +(FormalPowerSeries F ,SparseFPS G){F.format(); for(pair<long long,mint> &a : G)F.set(a.first,F[a.first]+a.second);F.format();return F;}
        friend FormalPowerSeries operator +(SparseFPS G , FormalPowerSeries F){return F+G;}
        friend FormalPowerSeries operator *(FormalPowerSeries F ,SparseFPS G){return F*=G;}
        friend FormalPowerSeries operator *(SparseFPS G , FormalPowerSeries F){return F*G;}
        friend FormalPowerSeries operator /(FormalPowerSeries F ,SparseFPS G){return F/=G;}
        friend bool operator ==(FormalPowerSeries F , FormalPowerSeries G){
            F.format();
            G.format();
            if(G.degree() != F.degree())return false;
            for(int i = 0 ; i < F.size();i++)if(G[i] != F[i])return false;
            return true;
        }
        friend bool operator !=(FormalPowerSeries F , FormalPowerSeries G){return !(F==G);}

        // 順序も定義だけしておく (次数の大きさ順)
        friend partial_ordering operator <=>(FormalPowerSeries F , FormalPowerSeries G){return F.degree()<=>G.degree();}

        

        //この式が何次式かを返す(formatされた N を返す)
        int degree(){this->format();return int(this->A.size())-1;}

        //Aが確保している領域(format済み)
        int size(){this->format();return int(this->A.size());}

        // 次数の上限
        int degree_limit(){return deg_max;}

        // 入力から x^i の係数を受け取る
        void cin_and_set(int i){
            long long d;cin>>d;
            this->set(i,d);
        }
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
    F.set(0,1);
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
    for(int i = 0 ; i <= n ; i++)F.cin_and_set(n-i);
    for(int i = 0 ; i <= n+m ; i++)G.cin_and_set(n+m-i);

    FPS998 ans = (G/F);
    
    REP(i,m+1)if(ans[m-i].val() < 450000000)cout << ans[m-i] << " ";else cout << ans[m-i].val()-998244353 << " ";
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
        if(x%2 == 1 && isprime(x) && isprime((x+1)/2))F.set(x,1);
    }
    G.set(0,1);
    G.set(1,-1);
    F /= G;
    while(Q-->0){
        int l,r;cin >> l >> r;
        cout << F[r]-F[l-1] << endl;
    }

}









