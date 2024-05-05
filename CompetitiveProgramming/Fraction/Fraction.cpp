

/*
    幾何-数学ライブラリ
*/
namespace MyMath{
    // math
    template<class T> T pow2(T x) {return x*x;}// x の 2 乗
    template<class T> double dist2(pair<T,T> a , pair<T,T> b) {return sqrt<double>(pow2<double>(a.first-b.first) + pow2<double>(a.second-b.second));}// 2 点間の距離
    double toRadian(double D){return D*PI/180.0;}// 度数法 (°) から弧度法 (ラジアン)
    double toDegree(double R){return R*180.0/PI;}// 弧度法 (ラジアン) から度数法 (°) 
    //最小公倍数、最大公約数
    template<class Integer>Integer gcd(Integer x, Integer y) { return (y!=0) ? gcd(y, x % y) : x; }
    template<class Integer>Integer lcm(Integer a, Integer b) { return (a / gcd(a, b)) * b; }
    // 組み合わせ
    template<class Integer>Integer nPr(ll n , ll r){Integer res = 1;assert(n>=r);while(r-->0)res *= Integer(n--);return res;}
    template<class Integer>Integer nCr(ll n,  ll r){// 必ず k <= n 
        if ( r * 2 > n ) r = n - r;
        return nPr<Integer>(n,r) / nPr<Integer>(r,r);
    }

    /*
        - 整数型 (__int128_t) の A ,B に対して A/B を表現 (普通にオーバーフローしやすいから注意。)
        | 
        - 値を T 型で取り出したい場合は、val<T>() を呼び出す (T が整数なら切り捨てられた値になることもある)
        - 生の値を取り出したい場合は、raw_pair() ( long long のペア ) を呼び出す 
        |
        - 設計 : 
            |- 整数 -> Fraction のキャストは OK は暗黙
            |- Fraction -> なにか のキャストは明示的にする必要がある
                |- 別のクラスのメンバ型として使う時、そのクラスのキャストはメンバ型のキャストを明示的にする必要がある
            |
            |- 内部の値は __int128_t 型なので 分母/分子 が 64 bit 整数の範囲を超えなければ処理できる
            |- A/B の値が、{0 , 微少量 , 有理数 , 無限大/無限小 , 0/0 (曖昧) } のどれかで処理を分ける
                |- A/B が有理数の場合、A/B は常に既約であるように設計 
                    |- 分母 B は常に正整数 0 より大きい)  とする
                |- A/B が 0 or 微少量の際、(A,B) = (0,1) で統一し、eps_sign で分別する
                |- A/B が無限大/無限小の場合、(A,B) = (符号 , 0) で統一する
                |- A/B が曖昧の場合、(A,B) = (0,0) とする                
            |
            |- 各演算の両辺は必ず整数 or Fraction。
        |
        - 仕様 : 
            |- 演算の規則は一般の数学の規則に準拠
            |
            |- 0 で割った値は amb() = 0/0 になる。
            |- ただし、コンストラクタで明示的に分母が 0 の値を与えた場合、それは無限大/無限小 になる
            |- EX. 
                |- Fraction(4)/0 → amb (0/0)
                |- Fraction(9,0) = ∞ (1/0)
                |- Fraction(-2,0) = -∞ (-1/0)
            |
            |- -0 , 0 , +0 を区別する (特に、-0,+0 を微少量 -ε , ε と呼ぶ)
                |- 0 徐算は amb になるが、-0 や +0 で割ったら無限大/無限小になる
                |- 0 は何をかけても、0 以外の何で割っても 0 のまま
                |- (+0) + (-0) は amb になる
    */
    class Fraction{
        protected:
        using Integer = __int128_t;// 内部でもつ整数
        Integer A,B;
        // (ε なら 1 , -ε なら -1, 0 なら 0)
        Integer eps_sign = 0;// デフォルト (A == 0 の時のみアクセス可能)
        // A ,B の形を制約通りに整える
        void format(){
            if(A == 0 && B == 0){}// 0/0 は未定義
            else if(A == 0)B = 1; // A が 0 の時、B は 1 (微少量かどうかは eps_sign を見る)
            else if(B == 0){// ∞ , -∞ の時、A は 1 or -1 とする
                Integer sign_ = 1;
                if(this->A < 0)sign_ = -1;
                this-> A = sign_;
            }else{ // 有理数として評価できる場合
                // A/B は規約分数。B は常に正整数 (非零) とする
                long long g = gcd<long long>(A,B);
                A/=g;B/=g;
                if(B < 0){A*=-1;B*=-1;} 
            }
        }
        public:
        // 特殊シンボル
        inline static const Fraction inf(){return Fraction(1,0);}// 無限大
        inline static const Fraction ninf(){return Fraction(-1,0);}// 無現小
        inline static const Fraction amb(){return Fraction(0,0);}// 0/0
        inline static const Fraction zero(){return Fraction(0,1);}// 0
        // コンストラクタ
        Fraction(){}
        // A,B に入る値は 64 bit 整数じゃないと演算内でオーバーフローする
        // Integer to Fraction ()
        Fraction(long long a):A(a),B((long long)(1)),eps_sign(0){this->format();}// 通常の整数型からのキャスト用
        // Fraction
        Fraction(long long a , long long b):A(a),B(b),eps_sign(0){this->format();}
        Fraction(const Fraction& x):A(x.A),B(x.B),eps_sign(x.eps_sign){this->format();}
        Fraction& operator =(const Fraction& x){
            this->eps_sign = x.eps_sign;
            this->A = x.A;
            this->B = x.B;
            this->format();
            return (*this);
        } 
        
        /*
            メンバアクセス
        */
        // 0/0 かどうか
        bool ambiguous() const {return bool(this->A == 0 && this->B == 0);}
        // ∞ または -∞ かどうか
        bool infinite() const {return bool(this->B==0);}
        // 0 または微少量かどうか (A が 0 である前提)
        // (ε なら 1 , -ε なら -1, 0 なら 0)
        long long eps()const {
            assert(this->A == 0);
            return this->eps_sign;
        }
        // 符号 (負なら -1 , 正なら 1 ,0 なら 0)
        // ambiguous の場合は エラー (注意)
        Integer sign() const {
            assert(!this->ambiguous());
            if(this->A == 0)return this->eps();// 0 or ±ε の場合
            if(this->A < 0)return -1;
            return 1;
        }
        //
        // T 型にして返す関数
        template<typename T>constexpr T val(){return T(A)/T(B);}
        // {A,B} を long long のペアで返す
        pair<long long ,long long> raw_pair(){return {(long long)(A),(long long)(B)};}
        //
        //
        // キャスト (明示的) (別のクラスに入れる場合は、そのクラスのキャスト処理内で明示的にする)
        // Point<Fraction> → Point<double> とかにキャストしたい場合、Point<T> → Point<C> の中でちゃんと T → C のキャストを明示する
        // これにより、演算子の重複を阻止する (int や long long の演算が呼ばれないようにする)
        template<typename C>explicit operator C(){return C(A)/C(B);}
        //
        // - の値
        Fraction operator -(){
            Fraction res(*this);
            if(this->ambiguous())return res;// 曖昧なやつはそのまま
            if(this->A == 0)res.eps_sign *= -1;// 0 or 微少量は eps_sign を参照
            else res.A *= -1;// それ以外は分子の符号を反転
            return res;
        }
        // 逆数を返す
        Fraction inv(){
            if(this->ambiguous())return amb();// 曖昧なやつはそのまま
            if(this->A == 0){
                if(this->eps() == 0)return amb();// 0 で割るのは曖昧
                return Fraction(this->eps(),0);// 微少量は、同じ符号の無限大に
            }
            if(this->B == 0){// 無限大は同じ符号の微少量に
                Fraction res(0,1);
                res.eps_sign = this->sign();
                return res;
            }
            return Fraction(this->B,this->A);// 逆数
        }
        //
        // 代入演算たち
        Fraction& operator +=(Fraction x){
            if(this->ambiguous() || x.ambiguous()) [[unlikely]] (*this) = amb();// 0/0 は変わらない
            else if(this->infinite() && x.infinite()) [[unlikely]] {
                // 両方とも無限大の場合 (sign は -1 or 1)
                if(this->sign() != x.sign())(*this) = amb();// ∞ - ∞ は曖昧
                // 符号が同じ場合
                else if(this->sign() == -1)(*this) = ninf();
                else (*this) = inf(); 
            }else if(this->infinite()) [[unlikely]] {
                // 代入される側のみ無限大の場合、何も代入しなくて OK
            }else if(x.infinite()) [[unlikely]] (*this) = x;// 代入する側が無限大ならそのまま代入
            else if(this->A == 0 && x.A == 0) [[unlikely]] {
                // 両方とも 0 or ±ε の場合 (曖昧になる可能性がある)
                if(this->eps() == x.eps()){}// 同じ符号 (または両方 0) ならそのまま
                else if(this->eps() == 0)(*this) = x;// 片方 0 なら、0 じゃない方に合わせる
                else if(x.eps() == 0){}// 片方 0 なら、0 じゃない方に合わせる
                else (*this) = amb();// 反対の符号なら曖昧になる
            }else [[likely]]{
                // (0 or 微少量は 0/1 の形なので、片方が 0 でも 微少量でもない時はそのまま計算可能)
                (*this) = Fraction(this->A*x.B + this->B*x.A , this->B*x.B);
            }
            return (*this);
        }
        Fraction& operator -=(Fraction x){return ((*this) += (-x));}
        Fraction& operator *=(Fraction x){
            if(this->ambiguous() || x.ambiguous()) [[unlikely]] (*this) = amb();
            // どちらかが無限の場合
            else if(this->infinite() || x.infinite()) [[unlikely]] {
                // どちらも 0 でも 微少量でもない場合
                if(this->A != 0 && x.A != 0){
                    if(this->sign() == x.sign())(*this) = inf();// 符号に依存する
                    else (*this) = ninf();
                }else{
                    // どちらかが 0 or 微少量の場合
                    // 0 にかけたらなんでも 0 になる
                    if(this->sign() == 0 || x.sign() == 0)(*this) = 0;
                    // ±ε × 無限 は ambiguous
                    else (*this) = amb(); 
                }
            }
            // 以降,両方とも無限ではない
            // 少なくとも一方が 0 or ±ε の時
            else if(this->A == 0 || x.A == 0) [[unlikely]] {
                // 0 にかけたらなんでも 0 になる
                if(this->sign() == 0 || x.sign() == 0)(*this) = 0;
                else {
                    Fraction new_eps = 0;// 微少量になる場合
                    // 片方は ε でも 0 でもないかもしれないので sign を参照
                    new_eps.eps_sign = this->sign()*x.sign();
                    (*this) = new_eps;
                }
            }else [[likely]] {
                (*this) = Fraction( this->A*x.A, this->B*x.B );
            }
            return (*this);
        }
        Fraction& operator /=(Fraction x){return ((*this) *= x.inv());}
        // 徐算 (有理数で割っても OK な特殊なやつ)
        // (this) を整数 m を使って (this) = m*x + r と表せる r を返す
        // 返す r の規則は int の % と同じ挙動
        Fraction& operator %=(Fraction x){
            // 収束した値同士でないといけない
            assert(!this->ambiguous() && !this->infinite() && !x.ambiguous() && !x.infinite());
            assert(x.A != 0);
            Fraction tmp = (*this);
            Integer m = (tmp/=x).val<Integer>();// 整数に丸める
            (*this) -= m*x;
            return (*this);
        }

        // 二項演算。整数 → Fraction のキャストは暗黙なので、これらの演算は、片方が整数でも OK。
        friend Fraction operator +(Fraction a , Fraction b){return a+=b;}
        friend Fraction operator -(Fraction a , Fraction b){return a-=b;}
        friend Fraction operator *(Fraction a , Fraction b){return a*=b;}
        friend Fraction operator /(Fraction a , Fraction b){return a/=b;}
        // 徐算 (有理数で割っても OK な特殊なやつ) (収束している値同士である必要がある)
        friend Fraction operator %(Fraction a , Fraction b){return a%=b;}
        // 
        // 比較演算たち 
        /*  
            - <=> が返す less / greater / equivalent の結果から、< , <= , > , >= 演算の結果を作るすごいやつ
                |- 例えば、0 <=> ∞ は less (<) を返す。すると、0 < ∞ が true になる < を作ってくれる。
            |
            - unordered について
                |- a,b の順序比較ができない時、unordered を返す。(0/0 との比較など)
                    |- unordered な順序があるバブルソート
                        |- 全ての順序が true になるまで操作を続けるので、永遠に終わらない
                |
                |- ∞ と ∞ の比較なども本来は unordered だが、プログラム内部での違いがないので equivalent として OK
                    |- バブルソート時にに ±0 や ±∞ は許容されるようになった。
                    |- 実際は異なることに注意!!!! (∞/∞ や 0/0 は 1 にはならない)
                |
                |- amb() の比較は unordered にせざるを得ない。
                    |- 例えばソートする列に amb() が混ざっていたら、amb() 以外も正しくソートされない
                    |- 可能な限り amb() の比較は避けよう
        */
        friend partial_ordering operator <=>(const Fraction& x , const Fraction& y){
            // どちらかが 0/0 なら比較は不可能。プログラム内部での値も異なるので、unordered 
            if(x.ambiguous() || y.ambiguous()) [[unlikely]] return partial_ordering::unordered;
            // 符号が異なる場合、符号で判定して OK . (符号は -1,0,1 のいずれか)
            else if(x.sign() != y.sign()) [[unlikely]] return (x.sign() <=> y.sign());
            // ここからは、符号は同じとして OK 
            // いずれかが 0 または 微少量の場合
            else if(x.A == 0 || y.A == 0) [[unlikely]] {
                // 0 同士
                if(x.sign() == 0 && y.sign() == 0)return partial_ordering::equivalent;
                // 両方とも微少量の場合 (本来比較は無理だが、プログラム内部では同じなので、equivalent として OK)
                if(x.A == 0 && y.A == 0)return partial_ordering::equivalent; 
                // x のみ微少量なら、y の符号で決める
                else if(x.A == 0){
                    if(y.sign() == -1)return partial_ordering::greater;
                    return partial_ordering::less;
                }
                // y のみ微少量の場合、x の符号で決める
                else{
                    if(x.sign() == -1)return partial_ordering::less;
                    return partial_ordering::greater;
                }
            }
            // 以降、どちらも 0 でも微少量でもないとして OK ( & 符号も同じ )
            // 符号が同じ無限大の比較は不可能。しかし、プログラム内部では同じなので equivalent として OK
            else if(x.infinite() && y.infinite()) [[unlikely]] return partial_ordering::equivalent;
            else if(x.infinite()) [[unlikely]] {// x のみ無限大 or 無限小
                if(x.sign() == -1)return partial_ordering::less;
                return partial_ordering::greater;
            }else if(y.infinite()) [[unlikely]] {// y のみ無限大 or 無限小
                if(y.sign() == -1)return partial_ordering::greater;
                return partial_ordering::less;
            }
            else [[likely]] return ((x.A)*(y.B) <=> (y.A)*(x.B));
        }
        /*
            0 周りの挙動が特殊なので注意が必要
        */
        friend bool operator ==(const Fraction& x,const Fraction& y){
            // 収束していない値同士を比較できない
            if(x.ambiguous() || x.infinite() || y.ambiguous() || y.infinite())return false;
            // (0 or 微少量) 同士を比較する
            if(x.A == 0 && y.A == 0){
                if(x.eps() == 0 && y.eps() == 0)return true;// 0 同士のみ true
                return false;// 符号が同じ ε 同士は曖昧な関係 & ε != 0 & ε != -ε
            }
            // 0 も 微少量も 0/1 の形なので、片方が有理数なら単純比較して OK
            return bool(x.A == y.A && x.B == y.B);
        }
        friend bool operator !=(const Fraction& x,const Fraction& y){return !(x==y);}

        // 
        // 
        // 有理数形式で出力
        // 分母分子ともに 64 bit を超えない前提
        friend ostream& operator << (ostream& os, const Fraction& x){os << (long long)(x.A) << "/" << (long long)(x.B);return os;}  
        //
        // 64bit 整数型なら入力ストリームから受け取れる
        friend istream& operator >> (istream& is, Fraction& x){
            long long tmp;// 64bit 整数の受け皿
            is >> tmp;
            x = Fraction(tmp);
            return is;
        }
    };

}