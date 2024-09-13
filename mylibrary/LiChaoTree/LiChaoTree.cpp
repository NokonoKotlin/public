#include "./../template/template.hpp"









/*
    Copyright ©️ (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)
*/
template<class T , class Integer_>
class LiChaoTree{

    private:
    // true なら小さい方を採用、false なら大きい方を採用
    bool smaller;
    // 二つの y 座標 Y1,Y2 を比較するとき、Y1 を優先するかどうか
    bool comp(T Y1 , T Y2){
        if(smaller)return Y1 < Y2;
        return Y1 > Y2;
    }
    
    /*動的なノードクラス*/
    struct Node{
        pair<Integer_,Integer_> range;// 担当する区間 (半開区間)
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;

        // このノードが持つ線分の 傾きと切片
        T A,B;// (A,B) : y = Ax + B を意味する
        Node(){}
        Node(Node* p, Integer_ l , Integer_ r , T a , T b){
            A = a;B = b;
            this->parent = p;
            this->range = {l,r};
        }
        void make_child(){
            if(left == nullptr && right==nullptr){
                left = new Node(this,range.first,(range.first+range.second)/2,A,B);
                right = new Node(this,(range.first+range.second)/2,range.second,A,B);
            }
            assert(left != nullptr && right!=nullptr);// 片方だけ存在しないことはあり得ない。
        }
        // このノードがカバーする区間の長さ
        constexpr Integer_ range_length(){return (range.second - range.first);}
        // このノードが i をカバーするか
        constexpr bool cover(Integer_ i){return bool(range.first <= i && i < range.second);}
        // 現在このノードが葉ノードか
        constexpr bool isleaf(){return bool(this->left == nullptr && this->right == nullptr);}
    };

    // 現在構築済みのセグ木で、x 座標が i を含む葉ノードを取得する
    Node* access(Integer_ i , Node* subr){ 
        assert(subr->cover(i));
        if(subr->isleaf())return subr;
        if(subr->left->cover(i))return access(i,subr->left); // i を含む方に降りる
        return access(i,subr->right);
    }

    // subr が持つ線分と ax + b を比較する。
    void EvalNode(T a ,T b ,Node* subr){
        Integer_ l = subr->range.first;
        Integer_ r = subr->range.second;
        Integer_ m = (l+r)/2;

        // subr が持つ線分の、x = l,r における y 座標
        T ly1 = subr->A*T(l) + subr->B;
        T ry1 = subr->A*T(r) + subr->B;
        // y = ax + b の、x = l,r における y 座標
        T ly2 = a*T(l)+b;
        T ry2 = a*T(r)+b;
        
        if(subr->range_length() == Integer_(1)){
            if(comp(ly2,ly1)){subr->A = a;subr->B = b;}// セグメントが最小単位なら x = l だけ参照
        }else if(subr->A == a){
            if(comp(b,subr->B)){subr->A = a;subr->B = b;}// 傾きが同じ場合、切片を見る
        }else if(ly1 == ly2){
            if(comp(a,subr->A)){subr->A = a;subr->B = b;}// 区間端で交わる場合、傾きの大小を見る
        }else if(ry1 == ry2){
            if(comp(subr->A,a)){subr->A = a;subr->B = b;}// 区間端で交わる場合、傾きの大小を見る
        }else if(comp(ly1,ly2) != comp(ry1,ry2)){
            // 両端で大小関係が異なるなら、区間内に交点が含まれる。
            // 交点が存在する場所を見て、左右のどちらか片方に降りる
            subr->make_child();
            Node* bound = subr->right;

            T my1 = subr->A*T(m) + subr->B;// x = m の subr の線分の y 座標
            T my2 = a*T(m) + b; // x = m の y = ax + b の y 座標
            // 中間で交わる場合
            if(my1 == my2){if(comp(subr->A,a))bound = subr->left;// 傾きを見る
            }else{
                if(comp(ly1,ly2) != comp(my1,my2))bound = subr->left;// 左側の領域に交点があるなら左に降りる
                if(comp(my2,my1)){
                    // 降りない方の領域の線分に y = ax + b を採用する場合
                    // bound には subr の線分を降ろし、subr は y = ax + b で置き換える(かしこい)
                    swap(a,subr->A);
                    swap(b,subr->B);
                }
            }
            EvalNode(a,b,bound);
        }else{
            // 区間の完全に外側で交わるなら、x = l の部分だけ参照すれば OK
            if(comp(ly2,ly1)){subr->A = a;subr->B = b;}
        }
        return;
    }
    
    // [l,r) に対応するセグメントを bucket に入れていく
    void RangeSegments(Integer_ l ,Integer_ r , Node* subr , vector<Node*>& bucket){
        if(r <= subr->range.first)return;
        else if(subr->range.second<=l)return;
        else if(l <= subr->range.first && subr->range.second <= r)bucket.push_back(subr);
        else {
            subr->make_child();
            RangeSegments(l,r,subr->left,bucket);
            RangeSegments(l,r,subr->right,bucket);
        }
    }

    // [l,r] に ax+b を追加 rt から探索
    void add_segment_sub(Integer_ l , Integer_ r , T a , T b , Node* rt){
        vector<Node*> segments;
        RangeSegments(l,r+1,rt,segments);// セグ木のノードは半開区間なので r に 1 加算する
        for(Node* nd : segments)EvalNode(a,b,nd);
    }
    Node* Root = nullptr;
    Integer_ _Llim,_Rlim;// 定義域の両端
    public:
    LiChaoTree(){}
    // 定義域と、全ての x 座標での y 座標の初期値、
    // y 座標の最小or最大のどっちを取得するかを smaller で指定する (true→最小 , false→最大)  
    // セグ木の区間は半開区間なので、[L,R_+1) とする
    LiChaoTree(Integer_ L_ , Integer_ R_ , T init_ , bool smaller = true):_Llim(L_),_Rlim(R_),Root(new Node(nullptr,L_,R_+1,0,init_)),smaller(smaller){}
    // アクセスできる x 座標の範囲 (閉区間) : [Llimit() , Rlimit()]
    Integer_ Llimit(){return _Llim;}
    Integer_ Rlimit(){return _Rlim;}

    // 全ての線分&直線のうち、x 座標が X において最小or最大 (smaller で指定) の y 座標を取得。
    T f(Integer_ X){
        Node* now = access(X,Root);
        T res = now->A*T(X) + now->B;
        while(now->parent!=nullptr){
            now = now->parent;
            if(comp(now->A*T(X) + now->B,res))res = now->A*T(X) + now->B;
        }
        return res;
    }
    
    // 定義域が [l,r) である線分 y = ax+b を追加する
    void add_segment(Integer_ l , Integer_ r , T a , T b ){add_segment_sub(l,r,a,b,Root);}

    // 全域に y = ax+b を追加
    void add_line(Integer_ a , Integer_ b){add_segment(Llimit(),Rlimit(),a,b);}

    // 点(a,b) から 点(c,d) に線分をひく。(y 座標は T 型で OK)
    // exclusive が true なら端点は含めない
    void write_segment(Integer_ a , T b , Integer_ c , T d , bool exclusive = false){
        if(a == c){
            if(exclusive && b == d)assert( 0 && "線分の領域が null です。");
            // 90° の直線なら、端点の y 座標を見る (exclusive の場合も、極限を考えて端点を採用することにする)
            if(comp(b,d))add_segment(a,a,0,b);
            else add_segment(a,a,0,d);
        }else{
            if(c < a){swap(a,c);swap(b,d);} 
            T p = T(d-b)/T(c-a);
            T q = T(b-p*a);
            // 端点を含まない場合、線分を追加する両端を縮小
            if(exclusive){a++;c--;}
            if(a>c)return; 
            add_segment(a,c,p,q);        
        }
    }

};


void ABC296G(){
    // テンプレート型宣言は明示的に!!! (日明示だとコンストラクタを優先してしまう)
    LiChaoTree<__float128,ll> U(-1e9,1e9,NINF,false);//凸包の上側
    LiChaoTree<__float128,ll> L(-1e9,1e9,INF,true);//凸包の下側
    ll n;cin >> n;vll x(n),y(n);
    ll lmost = INF;ll rmost = NINF; // 凸包の最左右の x 座標
    REP(i,n)cin >> x[i] >> y[i];
    x.push_back(x[0]);
    y.push_back(y[0]);
    REP(i,n){
        lmost = min(lmost,x[i]);
        rmost = max(rmost,x[i]);
        U.write_segment(x[i],y[i],x[i+1],y[i+1]);
        L.write_segment(x[i],y[i],x[i+1],y[i+1]);
    }
    ll q;cin >> q;
    REP(i,q){
        ll X ;ll Y;cin >> X >> Y;
        auto l = L.f(X);
        auto u = U.f(X);
        if(X == lmost || X == rmost){
            if(l - Y < EPS && Y - u < EPS)cout << "ON" << endl;
            else cout << "OUT" << endl;
        }else{
            if(l - Y < -EPS && Y - u < -EPS )cout << "IN" << endl;
            else if(Y - l < -EPS || u - Y < -EPS)cout << "OUT" << endl;
            else cout << "ON" << endl;
        }
    }
}





