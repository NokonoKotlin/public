#include "MyTemplate.hpp"





/*
    - LiChaoTree : 動的双対セグメント木に線分を載せたもの。
    - 実装のベースは動的セグメント木と同じだが、異なる点もあるので要注意
    |
    - Real (実数) の座標にアクセスすることから、異色な実装になっている。
        |- 大小などの比較には AisSmallerThanB , same 関数を使う。
        |- セグメントの分割の下限サイズ : eps を決める。
            |- eps 未満のセグメントはこれ以上分割しないという意味で、eps 未満のセグメントは存在しうる
            |- min_unit(r) := r が最小単位未満かどうか
            |- eps を小さくしすぎると MLE するので、調整が大事。
    |
    - コンストラクタには以下を渡す。
        |- L,R := x 座標の定義域が半開区間 [L,R) を指定
        |- init_ := [L,R) の y 座標の初期値が init_ である。(直線 : Y=init_ が存在する)
        |- mode を指定して、最小か最大のどちらを取得するかを指定する。
            |- true  : 最小
            |- false : 最大
    |
    - add_segment(l,r,a,b) : 半開区間　[l,r) に線分 y = ax+b を追加
    - add_line(a,b) : 定義域全体に直線 y = ax+b を追加

    - (非推奨) write_segment(a,b,c,d) : 点 (a,b) と 点 (c,d) をつなぐ。
                              / ただし、x 座標が小さい方の点は線分に含まれ、大きい方は含まれない。
                             / なんかバグってる?? もしかしたら気づいていない制約があるかも。[L,R) がでかいとバグるかも

    - f(X) := 全ての線分&直線のうち、x 座標が X において最小or最大 (mode で指定) の y 座標を取得。
    |
    |
    - TLE,RE,WA,MLE が出たら、以下をゴネゴネすると良い。
        |- 定義域 [L,R) : これはケースごとにランダムに投げることができる
        |- 最小単位 eps
        |- double 比較関数の eps (上記のものとは別物)
*/
template<typename Real>
class LiChaoTree{

    private:
    // r が分割するセグメントサイズの下限より小さいかを返す。
    static bool min_unit(Real r){
        // セグメントの最小単位 ε の意味。
        // ε (eps) 未満のセグメントをこれ以上分割しないという意味。ε 未満のセグメントは存在しうる
        const Real eps = 5e-9;// MLE とかしたら小さくすると良い
        return bool(abs(r)<eps);
    }
    /*double の比較*/
    // 比較用の EPS を宣言
    #define CompareEPS 1e-16
    // A == B
    static bool same(Real A , Real B){return abs(A-B) < CompareEPS;}
    // A < B
    static bool AisSmallerThanB(Real A , Real B){return (A-B)< -CompareEPS;}// ここも微少量も必要に応じて編集
    // A <= B なら !AisSmallerThanB(B,A) とする。
    
    /* 
        二つの線分 y = ax+b ,  y = cx+d と定数 X に対して、
        Y1 = aX + b , Y2 = cX+d であるような状況を考える。
        Y1 と Y2 を比較して、Y1 を採用するなら true, Y2 を採用するなら false を返す。
        採用するとは、mode の比較結果である方を採用する.
        要するに、最小or最大 の y 座標を計算する際に使用する
    */
    bool mode;// true なら小さい方を優先,false なら大きい方
    bool comp(Real Y1 , Real Y2){
        if(mode)return AisSmallerThanB(Y1 , Y2);
        else return AisSmallerThanB(Y2 , Y1);
    }
    
    /*
        動的なノードクラス
    */
    struct Node{
        pair<Real,Real> range;// 担当する区間 (半開区間)
        Real partition;// この区間を分割する点 (make_child の時に決まる)
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;

        // このノードが持つ線分 ( segment という名称の方が適切だが、セグメント木と被るので line とする)
        pair<Real,Real> line; // (a,b) : y = ax + b を意味する
        Node(){}

        // 子供全ての要素が x であると仮定してノードを作る
        // 特に init_ については Root から伝播される init_value を引き継ぐ
        Node(Node* p, Real l , Real r , Real a , Real b){
            line.first = a;
            line.second = b;
            this->parent = p;
            this->range = {l,r};
        }

        // 動的セグ木ライブラリと違い、分割点 par を指定できる。
        // 特に必要性がなければ (range.first+range.second)/2 とする
        // par が両端のいずれかなら分割しないように注意 (これは上位で処理するべき)
        bool make_child(Real par){
            if(this->left == nullptr && this->right==nullptr){
                this->left = new Node(this,this->range.first,par,this->line.first , this->line.second);
                this->right = new Node(this,par,this->range.second,this->line.first , this->line.second);
                this->partition = par;
                return true;
            }
            assert(this->left != nullptr && this->right!=nullptr);// 片方だけ存在しないことはあり得ない。
            return false;
        }

        // このノードがカバーする区間の長さ
        constexpr Real range_length(){return (range.second - range.first);}
        // このノードが i をカバーするか
        constexpr bool cover(Real i){return bool(!AisSmallerThanB(i,range.first) && AisSmallerThanB(i,range.second) );}
        // 現在このノードが葉ノードか
        constexpr bool isleaf(){return bool(this->left == nullptr && this->right == nullptr);}
    };

    
    // x 座標が i を含む葉ノードを取得する
    Node* access(Real i , Node* subr){ 
        assert(subr->cover(i));
        if(subr->isleaf())return subr;
        // 左右のノードが存在するので、i を含む方に降りる
        if(subr->left->cover(i))return access(i,subr->left);
        return access(i,subr->right);
    }

    // subr が持つ線分と ax + b を比較する。
    void UpdateNode(Real a ,Real b ,Node* subr){
        // subr の持つ線分を、subr->line と ax+b のどちらにするかを計算する
        // 並行な場合
        assert(subr!=nullptr);
        if(same(subr->line.first , a)){
            if(!comp(b,subr->line.second)){subr->line.first = a;subr->line.second = b;}
            return;
        }
        // 交点 (cross point) がある場合
        double cpx = (subr->line.second - b)/(a - subr->line.first);
        
        // subr が担当する区間内で交わる場合 (このとき、区間の範囲が最小単位より大きいことが保証される)
        if(subr->cover(cpx)){
            // 左端で交わる場合 (左端で交わる = 左端との距離が最小単位以下)
            if(min_unit(subr->range.first - cpx)){
                // 傾きの大小を見るとよい
                if(comp(a,subr->line.first)){subr->line.first = a;subr->line.second = b;}
            }else{
                if(subr->isleaf())subr->make_child((subr->range.first + subr->range.second)/2);
                UpdateNode(a,b,subr->left);
                UpdateNode(a,b,subr->right);
            }
        }else{
            // 領域の左右どちらに好転があるかで場合分け
            if(AisSmallerThanB(cpx,subr->range.first)){// 傾きに注目
                if(comp(a,subr->line.first)){subr->line.first = a;subr->line.second = b;}
            }else{
                if(comp(subr->line.first,a)){subr->line.first = a;subr->line.second = b;}
            }
        }
        return;
    }


    // [l,r) に対応するセグメントを、bucket に入れていく
    void RangeSegments(Real l ,Real r , Node* subr , vector<Node*>& bucket){
        if(min_unit(subr->range_length()))return;// 最小単位セグメントはこれ以上分割しない
        if(!AisSmallerThanB(subr->range.first,r))return;
        else if(!AisSmallerThanB(l , subr->range.second))return;
        else if(!AisSmallerThanB(subr->range.first,l) && !AisSmallerThanB(r,subr->range.second))bucket.push_back(subr);
        else {
            subr->make_child((subr->range.first + subr->range.second)/2);
            RangeSegments(l,r,subr->left,bucket);
            RangeSegments(l,r,subr->right,bucket);
        }
    }

    // [l,r) に ax+b を追加, rt から探索
    void add_segment_sub(Real l , Real r , Real a , Real b , Node* rt){
        // あらかじめ l と r で分割しておく (バランスが崩れるが、こうすることで l, r ギリギリの場所へのアクセス時の誤差をなくす)
        // この場合に限り、最小単位より小さいセグメントをさらに分割して良い
        // TLE したりするなら無くして試してみる
        if(!same(rt->range.first , l)){Node* lnode = access(l,rt);lnode->make_child(l);}
        if(!same(rt->range.second , r)){Node* rnode = access(r,rt);rnode->make_child(r);}
        vector<Node*> segments;
        RangeSegments(l,r,rt,segments);
        for(Node* nd : segments)UpdateNode(a,b,nd);
    }


    Node* Root = nullptr;
    Real _Llim,_Rlim;

    public:
    LiChaoTree(){}
    // index の範囲と、全ての x 座標での y 座標の初期値、
    // y 座標の最小or最大のどっちを取得するかを mode で指定する (true→最小 , false→最大)
    LiChaoTree(Real L_ , Real R_ , Real init_ , bool mode_ = true):_Llim(L_),_Rlim(R_),Root(new Node(nullptr,L_,R_,0,init_)),mode(mode_){}
    // 有効な x 座標の範囲 : [ Llimit() , Rlimit() ) 
    Real Llimit(){return _Llim;}
    Real Rlimit(){return _Rlim;}

    
    // 定義域が [l,r) である線分 y = ax+b を追加する
    void add_segment(Real l , Real r , Real a , Real b ){add_segment_sub(l,r,a,b,Root);}

    // 点 (a,b) から 点 (c,d) に線分をひく。
    // ただし、x 座標が小さい方の端点は含まれ、x 座標が大きい方の端点は含まれない。
    void write_segment(Real a , Real b , Real c , Real d){
        assert(!min_unit(a-c));// 90° は無理 (x 座標の差が最小単位より小さいなら、セグメントを作れないので NG)
        if(AisSmallerThanB(c,a)){swap(a,c);swap(b,d);}
        if(b==d)add_segment(a,c,0,b);
        else{
            Real p = (d-b)/(c-a);
            Real q = b-p*a;
            add_segment(a,c,p,q);
        }
    }

    // 全域に y = ax+b を追加
    void add_line(Real a , Real b){add_segment(Llimit(),Rlimit(),a,b);}
    
    // 全ての線分&直線のうち、x 座標が X において最小or最大 (mode で指定) の y 座標を取得。
    Real f(Real X){
        Node* now = access(X,Root);
        Real res = now->line.first*X + now->line.second;
        while(now->parent!=nullptr){
            now = now->parent;
            Real tmp = now->line.first*X + now->line.second;
            if(comp(tmp,res))res = tmp;
        }
        return res;
    }
};



void EDPC_Frog3(){
    double c;
    ll n;cin >> n >> c;
    vector<double> a(n);CinVec(a);  

    LiChaoTree<double> C(-1,INF,INF);
    C.add_line(-2*a[0],a[0]*a[0]);

    vector<double> dp(n+1,INF);
    dp[0] = 0;
    
    FOR(i,1,n){
        dp[i] = C.f(a[i])+a[i]*a[i]+c;
        C.write_segment(C.Llimit(),-2*C.Llimit()*a[i]+dp[i]+a[i]*a[i],C.Rlimit(),-2*a[i]*C.Rlimit()+dp[i]+a[i]*a[i]);
    }
    cout << ll(dp[n-1]+0.5) << endl;
}



