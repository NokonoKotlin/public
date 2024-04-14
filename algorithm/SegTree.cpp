#include "MyTemplate.hpp"







/*
    遅延評価付き動的セグメント木
    - 数列を表すデータ構造
    |
    - できること
        |- 区間更新 (一律に代入)
        |- 区間アフィン変換 (区間内の要素全てに A 掛けて B 足す)
        |- 区間のモノイド積(集約値 Sum など)を求める
    |   
    - 定数倍が気になるならいらない機能を削除してね
    - テンプレートで要素の型 T , インデックスの型 index_int(整数) を渡す
    - 初期化値もモノイド積に含まれるので、Sum などがオーバーフローしないように注意
    |
    - コンストラクタで [L,R) と初期化値 init_  を渡し、[L,R) に対応する根ノードを作成する。
    - はじめ、ノードは [L,R) に対応するものしかなく、必要に応じて半分に分割することを繰り返す。
    - L,R に範囲の制約はない (オーバーフロー注意)
    |
    - アルゴリズム
        |- 
        |- はじめ、セグ木のノードは根のみ存在する。
        |- アクセスを伴う処理が行われた際に、アクセスするセグメントまでのノードを必要に応じて作る。  
            |- 作るノード数はクエリあたり log(R-L) 
        |- eval,NodeUpdate を行う必要があるので、ノードにアクセスするたびに make_child を呼び出す。
        |- make_child で作ったノード以下の要素は init_ であるものとし、モノイド積も init_ をもとに計算する
            |- 計算に NodeUpdate を使うと再帰してしまうので、コンストラクタで手計算する
    |
    - 設計思想
        |- Node クラスに必要なデータを持たせる
        |- いじるのは基本的に Node の eval , UpdateNode およびコンストラクタでの変数の初期化
*/
template<class T , typename index_int = long long>
class SegmentTree{
    private:
    // ノードクラス (遅延評価やマージ値などのデータを保持)
    struct Node{
        /*range の長さが 1 以下 <=> 子の存在を定義しない*/
        pair<index_int,index_int> range;// 担当する区間 (半開区間)
        index_int partition;
        T init_;// ノードを初期化する値
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;
        
        // 値と集約値
        T Value , Sum ,Max, Min;

        Node(){}

        // 子供全ての要素が x であると仮定してノードを作る
        // 特に init_ については Root から伝播される init_value を引き継ぐ
        Node(Node* p, index_int l , index_int r , T x){
            init_ = x;
            this->parent = p;
            this->range = {l,r};
            this->partition = (l+r)/2;
            // ここで初期化する (NodeUpdate を呼び出すと再帰してしまうのでダメ)
            Min = x;
            Max = x;
            Sum = range_length()*x;
            if(range_length() <= 1)Value = x;
        }

        // 遅延評価などをおろしておくために左右の子を作っておく必要がある
        void make_child(){
            if(this->range_length()<=1)return;
            if(this->left == nullptr)this->left = new Node(this,this->range.first,this->partition,this->init_);
            if(this->right == nullptr)this->right = new Node(this,this->partition,this->range.second,this->init_);
        }

        // 遅延中の区間アフィン変換クエリ(遅延しているか , {A,B})
        pair<bool,pair<T,T> > lazy_affine ={false , {T(),T()}};
        void set_lazyAffine(T& a, T& b){
            if(this->lazy_affine.first)this->lazy_affine.second = { a*this->lazy_affine.second.first , a*this->lazy_affine.second.second + b};
            else this->lazy_affine = {true , {a,b}};
        }

        // 遅延中の区間代入クエリ(遅延しているか , 値)
        pair<bool,T> lazy_update = {false,T()};
        void set_lazyUpdate(T &x){
            this->lazy_update.first = true;
            this->lazy_update.second=x;
            this->lazy_affine.first = false;//これ以前の affine クエリは無効化
        }

        // このノードがカバーする区間の長さ
        constexpr index_int range_length(){return (range.second - range.first);}
        // このノードが i をカバーするか
        constexpr bool cover(index_int i){return bool(range.first <= i && i < range.second);}


        void NodeUpdate(){
            make_child();
            this->eval();
            if(range_length() <= 1){
                Sum = Min = Max = Value;
                return;
            }
            this->left->eval();
            this->right->eval();
            this->Sum = this->left->Sum + this->right->Sum;
            this->Min = min(this->left->Min, this->right->Min);
            this->Max = max(this->left->Max, this->right->Max);
        }

        // 遅延を評価し、左右に降ろす
        void eval(){
            make_child();
            // 代入(update) の評価が先
            if(this->lazy_update.first){
                this->Value = this->lazy_update.second;
                this->Min = this->lazy_update.second;
                this->Max = this->lazy_update.second;
                this->Sum = this->lazy_update.second*(range.second-range.first);
                if(this->left)this->left->set_lazyUpdate(this->lazy_update.second);
                if(this->right)this->right->set_lazyUpdate(this->lazy_update.second);
                this->lazy_update.first = false;//伝播したらフラグを折る
            }

            // 区間アフィン変換
            if(this->lazy_affine.first){
                this->Value = this->lazy_affine.second.first * this->Value + this->lazy_affine.second.second;
                this->Min = this->lazy_affine.second.first * this->Min + this->lazy_affine.second.second;
                this->Max = this->lazy_affine.second.first * this->Max + this->lazy_affine.second.second;
                this->Sum = this->lazy_affine.second.first * this->Sum + this->lazy_affine.second.second * this->range_length();
                if(this->left)this->left->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                if(this->right)this->right->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                this->lazy_affine.first = false;//伝播したらフラグを折る
            }
        }
    };

    
    // 数列の i 番目の要素に対応するノードまで subr からアクセスする (道中で遅延評価を降ろす)
    Node* access(index_int i , Node* subr){ 
        subr->make_child();
        subr->eval();
        assert(subr->cover(i));
        if(subr->range_length()<=1)return subr;
        if(i < subr->partition)return access(i,subr->left);
        else if(i >= subr->partition)return access(i,subr->right);
        return nullptr;
    }

    // [l,r) に対応するセグメントを、bucket に入れていく
    void RangeSegments(index_int l ,index_int r , Node* subr , vector<Node*>& bucket){
        subr->make_child();
        subr->eval();
        if(r <= subr->range.first)return;
        else if(l >= subr->range.second)return;
        else if( l <= subr->range.first && subr->range.second <= r)bucket.push_back(subr);
        else {
            RangeSegments(l,r,subr->left,bucket);
            RangeSegments(l,r,subr->right,bucket);
        }
    }


    // 半開区間 [l,r) 内の要素全てに x を代入
    void RangeUpdate_sub(index_int l , index_int r , T x , Node* subr){
        subr->make_child();
        subr->eval();
        if(r <= subr->range.first)return ;
        else if(l >= subr->range.second)return ;
        else if( l <= subr->range.first && subr->range.second <= r)return subr->set_lazyUpdate(x);
        else {
            RangeUpdate_sub(l,r,x,subr->left);
            RangeUpdate_sub(l,r,x,subr->right);
            subr->NodeUpdate();
        }
    }

    // 半開区間 [l,r) 内の要素全てに A 掛けて B 足す
    void RangeAffine_sub(index_int l , index_int r , T A , T B , Node* subr){
        subr->make_child();
        subr->eval();
        if(r <= subr->range.first)return ;
        else if(l >= subr->range.second)return ;
        else if( l <= subr->range.first && subr->range.second <= r)return subr->set_lazyAffine(A,B);
        else {
            RangeAffine_sub(l,r,A,B,subr->left);
            RangeAffine_sub(l,r,A,B,subr->right);
            subr->NodeUpdate();
        }
    }

    Node* Root = nullptr;
    index_int _Llim,_Rlim;

    public:
    SegmentTree(){}
    // index の範囲を受け取る
    SegmentTree(index_int L_ , index_int R_ , T init_value):_Llim(L_),_Rlim(R_),Root(new Node(nullptr,L_,R_,init_value)){}
    // 有効な index の範囲が [ Llimit() , Rlimit() ) に対応
    index_int Llimit(){return _Llim;}
    index_int Rlimit(){return _Rlim;}

    // 数列の i 番目を x に変更する
    void update_val(index_int i , T x){
        assert(i >= Llimit() && i < Rlimit());
        Node* instance = access(i,Root);
        instance->Value = x;
        while(instance != nullptr){
            instance->NodeUpdate();
            instance = instance->parent;
        }
    }

    // 数列の i 番目の値を取得    
    T get(index_int i){
        assert(i >= Llimit() && i < Rlimit());
        return access(i,Root)->Value;
    }

    // 区間 [l,r) の min
    T RangeMinQuery(index_int l , index_int r){
        l = max(l,Llimit());r = min(r,Rlimit());
        assert(l<=r);
        vector<Node*> bucket;
        RangeSegments(l,r,Root,bucket);
        assert(bucket.size() > 0);
        T res = bucket[0]->Min;
        for(int i = 1 ; i < bucket.size() ; i++)res = min(res,bucket[i]->Min);
        return res;
    }
    // 区間 [l,r) の max
    T RangeMaxQuery(index_int l , index_int r){
        l = max(l,Llimit());r = min(r,Rlimit());
        assert(l<=r);
        vector<Node*> bucket;
        RangeSegments(l,r,Root,bucket);
        assert(bucket.size() > 0);
        T res = bucket[0]->Max;
        for(int i = 1 ; i < bucket.size() ; i++)res = max(res,bucket[i]->Max);
        return res;
    }
    // 区間 [l,r) の sum
    T RangeSumQuery(index_int l , index_int r){
        l = max(l,Llimit());r = min(r,Rlimit());
        assert(l<=r);
        vector<Node*> bucket;
        RangeSegments(l,r,Root,bucket);
        assert(bucket.size() > 0);
        T res = bucket[0]->Sum;
        for(int i = 1 ; i < bucket.size() ; i++)res += bucket[i]->Sum;
        return res;
    }

    // 半開区間 [l,r) 内の要素全てに x を代入
    void RangeUpdate(index_int l , index_int r, T x){
        l = max(l,Llimit());r = min(r,Rlimit());
        assert(l<=r);
        RangeUpdate_sub(l,r,x,Root);
    }

    // 半開区間 [l,r) 内の要素全てに A 掛けて B 足す
    void RangeAffine(index_int l , index_int r, T A , T B){
        l = max(l,Llimit());r = min(r,Rlimit());
        assert(l<=r);
        RangeAffine_sub(l,r,A,B,Root);
    }
                   
    // 半開区間 [l,r) 内の要素全てに x 足す
    void RangeAdd(index_int l , index_int r, T x){
        l = max(l,Llimit());r = min(r,Rlimit());
        assert(l<=r);
        RangeAffine_sub(l,r,T(1) , x,Root);
    }
    
    // 数列の i 番目の値を取得    
    T operator [](index_int i){return get(i);}

    // デバッグ
    void Debug(){   
        cerr << "debug : ";
        for(index_int i = Llimit() ; i < Rlimit() ; i++)cerr << get(i) << " ";
        cerr << endl;
    }

};





