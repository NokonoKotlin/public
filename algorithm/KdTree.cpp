#include "MyTemplate.hpp"







/*
    k demensional Tree
    k次元座標上に、あらかじめいくつかの点が与えられる。このライブラリではk = 2。

    addPointで点を与える(頂点を追加する)。ただし、追加した後はbuildをしないといけないので、高速ではない。
    buildで木を構築する。


    Rootは全体の根で、大事なので書き換えたりしてわからなくならないように気を付けてください。
    
    find_included_points : 二点:A,Bを対角としてもつ矩形の内部(周上を含む)に存在する点を列挙できる。
    find_nearest_point : 点(x,y)から、与えられた点のうち最も近い点とその距離(の二乗)を計算できる。


    点の座標は、K次元のvectorで与える
    i番目の要素が、i番目の次元の座標に対応

    ただし、点の座標はPointという構造体で定義し、Pointの中では座標はvectorではなく配列として宣言している(実行時間がだいぶ違う)
    Pointには、idという変数もあり、これはその点が何番目に追加されたものなのかを識別するもの

    long long の場合、座標が -1e9 ≦ x ≦ 1e9 の領域を超える場合に要注意(実装を変更する必要がある(判定の部分で二乗が出てきているから))
*/
template<class T , int K = 2>
class kdTree{
    public:
    //KdTreeの次元(変えたい場合はここをいじる)
    //コンストラクタ
    kdTree(){}





    
    /*
        ----------------------------------------------------------------------------------------------------------------------------------------
        ----------------------------------------------------------------------------------------------------------------------------------------
                            クラス内で使う構造体。クラス外からもアクセス可能
        ----------------------------------------------------------------------------------------------------------------------------------------    
        ----------------------------------------------------------------------------------------------------------------------------------------
    */
    /*  
        k次元のvector A_ を受け取る
        Aの中身が、k次元座標の各座標に対応する
    */
    struct Point{

        
        //vector<T> A;
        T A[K];
        
        /*
            identifer : この点がどの点かを判断する
            kDtreeでは点をソートしたりするので、どの点がどこに行ったのかごちゃごちゃになりやすい。
            なので、点にidを与えることで、どこにどの点があるのかを判断する。
        */  
        int id;

        Point(vector<T> &A_ , int id_ = -1){
            id = id_;
            REP(i,A_.size())A[i] = A_[i];
        }

        Point(){}

    };


    struct Node{
        public:
        Node *left , *right ;

        Point val;

        Node(Node *left_ , Node *right_ , Point val_){
            left = left_;
            right = right_;
            val = val_;
        }

        Node(){
            left = nullptr;
            right = nullptr;
        }

    };


    private:
    /*
        ----------------------------------------------------------------------------------------------------------------------------------------
        ----------------------------------------------------------------------------------------------------------------------------------------
                            クラス内で行う処理。目には見えないブラックボックスってやつ
        ----------------------------------------------------------------------------------------------------------------------------------------    
        ----------------------------------------------------------------------------------------------------------------------------------------
    */


    /*
        P_listを元に木を作る。
        [lef,rig) の区間に対応する部分木の根を作る。
    */
    Node *build_sub(int lef = 0, int rig = 1e9 , int depth = 0){
        if(rig >= P_list.size())rig = P_list.size();
        if(lef< 0)lef = 0;

        if(rig - lef <= 0)return nullptr;
        else if(rig-lef == 1)return new Node(nullptr , nullptr , P_list[lef]);
        

        int mid = (lef+rig)/2;

        //depth(階層)によってソートする座標軸を変える。
        //depthをKで割った余りをxとすると、x番目の軸でソートする
        ll dimension_now = depth%K;//ソートする次元を指定

        function<bool(Point& , Point&)> comp_point = [&](Point& a , Point& b){
            return a.A[dimension_now] < b.A[dimension_now];
        };
        
        sort(P_list.begin() + lef , P_list.begin() + rig , comp_point);//ソート
        return new Node(build_sub(lef,mid ,depth+1) , build_sub(mid+1 ,rig , depth+1) , P_list[mid]);
    }


    /*
        点Xから、一番近い点とその距離(の二乗)を求める
        pair<点 , 距離>で返す
        vector<T> X(K);
    */
    pair<Point,T> find_nearest_point_sub(vector<T> &X,  int depth, Node *nd , pair<Point,T> r = make_pair(Point() , -1)  ) {
        if(nd == nullptr) return r;

        ll dimension_now = depth%K;//見る次元

        Point p = nd->val;//点
        
        T d = 0;//距離
        REP(i,K)d += (X[i]-p.A[i])*(X[i]-p.A[i]);
        if(r.second == -1 || d < r.second) r = make_pair(p, d);

        /*      
            近い領域を探索
        */

        T border = p.A[dimension_now];//今見ている頂点が領域を分割する境界線
        T x_ = X[dimension_now];//点 X の座標のうち、木の深さに対応した座標

        //2分割した左側に答えがある可能性があるかどうか 
        //点 X が境界線の左側にある、または、点 X から境界線の向こう側までの距離が、現在わかっている最近傍の点までの距離より小さい
        bool is_included_in_left_field = ((x_ - border)*(x_-border) < r.second || x_-border <= 0);
        
        //2分割した右側に答えがある可能性があるかどうか 
        bool is_included_in_right_field = ((x_ - border)*(x_-border) < r.second || border-x_ <= 0);

        if(nd->left != nullptr && is_included_in_left_field) r = find_nearest_point_sub(X,  depth+1 , nd->left , r);
        if(nd->right != nullptr && is_included_in_right_field) r = find_nearest_point_sub(X,  depth+1 , nd->right , r);
        return r;
    }



    /*
        K次元の点 A , B を対角とする矩形領域内の点のidを,found_Points_IDに入れていく
        周上も含む
    */
    void find_included_points_sub(Node *nd, vector<T> &A , vector<T> &B, int depth) {
        if(nd == nullptr)return;

        ll dimension_now = depth%K;
        REP(i,K){
            if(A[i] > B[i])swap(A[i],B[i]);
        }
        Point p = nd->val;
        

        //area[i]:=i番目の座標(次元)で、A,Bの矩形領域に入っている区間
        T area[K][2];

        REP(i,K){
            area[i][0] = min(A[i],B[i]);
            area[i][1] = max(A[i],B[i]);
        }
        bool fl = 1;
        //pが矩形領域内かどうか
        REP(i,K)if(!(area[i][0] <= p.A[i] && p.A[i] <= area[i][1]))fl = 0;
        
        if(fl!=0)found_Points.push_back(p);
                
        if(nd->left != nullptr && area[dimension_now][0] <= p.A[dimension_now]) {
            find_included_points_sub(nd->left,A,B, depth+1);
        }

        if(nd->right != nullptr && p.A[dimension_now] <= area[dimension_now][1]) {
            find_included_points_sub(nd->right, A,B, depth+1);
        }
    
        

    }



    public:
    /*
        ----------------------------------------------------------------------------------------------------------------------------------------
        ----------------------------------------------------------------------------------------------------------------------------------------
                            クラス外で行う処理。
        ----------------------------------------------------------------------------------------------------------------------------------------    
        ----------------------------------------------------------------------------------------------------------------------------------------
    */

    //全体の根,記憶しておくこと。ここから木上の探索を開始する
    Node *Root = nullptr;
    /*
        addPointでPointを追加していく。
        ただし、追加した後はbuildで木を構築しないといけないので、追加はO(n)
    */
    vector<Point> P_list;
    /*
        find_included_pointsで取得した点を入れる配列
    */
    vector<Point> found_Points;

    /*
        Pointを追加。ただし、追加した後に木を構築しないといけないので、O(n)
        X[i] := 点のi番目の次元の座標
    */
    void addPoint(vector<T> X){
        if(X.size()!=K){
            cerr << "dimension must be K" << endl;
            return;//K次元の座標のみ受け取る
        }
        int id_ = P_list.size();
        P_list.push_back(Point(X,id_));
    }

    /*
        build_subを使い、Rootを求める
    */
    void build(){
        Root = nullptr;//前の情報が残っていたらなんか嫌だから
        Root = build_sub();
    }

    /*
        点Xから、一番近い点とその距離(の二乗)を求める
        pair<点 , 距離>で返す
        vector<T> X(K);
    */
    pair<Point,T> find_nearest_point(vector<T> X){
        if(X.size()!=K){
            return make_pair(Point(),-1);
        }
        return find_nearest_point_sub(X,0,Root);
    }


    /*
        K次元の点A,Bを対角に持つ矩形領域に含まれる点のidを,found_Points_IDに入れていく
        
    */
    void find_included_points(vector<T> A , vector<T> B){
        vector<Point>(0).swap(found_Points);
        if(A.size()!=K || B.size()!=K){
            return;
        }        
        find_included_points_sub(Root , A,B , 0);
    }

};




