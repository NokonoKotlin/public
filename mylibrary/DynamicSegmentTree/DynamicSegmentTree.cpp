#include "./../template/template.hpp"








/*
    Copyright ©️ (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)
*/
template<typename index_int , class T>
class SegmentTree{
    private:
    // ノードクラス (遅延評価やマージ値などのデータを保持)
    struct Node{
        private:
        // アクセスや遅延評価を降ろす際に子ノードを作成する内部用関数
        void make_child(){
            // 片方のみ存在するのはあり得ない。
            if(this->left == nullptr && this->right == nullptr && this->range_length() > 1){
                // make_child が呼ばれるとき、このノードの遅延評価は Value に反映ずみなので、遅延評価は解消させて Value だけ継承する。
                this->left = new Node(this,this->range.first,((this->range.first+this->range.second)>>1),this->Value);
                this->right = new Node(this,((this->range.first+this->range.second)>>1),this->range.second,this->Value);
            }
        }
        public:
        /*range の長さが 1 以下 <=> 子が存在しない*/
        pair<index_int,index_int> range;// 担当する区間 (半開区間)
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;
        
        // 値と集約値
        T Value ;// 本来は葉ノードのみが持つ値だが、root から init_value を継承する役割も持つ
        T Sum ,Max, Min;

        Node(){}

        // 子供全ての要素が x であると仮定してノードを作る
        // Value は root から値 (init_value) を受け継ぎ、遅延評価なども反映しつつ子の方に継承していく。
        // ここで初期化する (NodeUpdate を呼び出すと再帰してしまうのでダメ)
        Node(Node* p, index_int l , index_int r , T x){
            this->parent = p;
            this->range = {l,r};
            Value = x;
            Min = x;
            Max = x;
            Sum = range_length()*x;
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
        constexpr index_int range_length()const{return (range.second - range.first);}
        // このノードが i をカバーするか
        constexpr bool cover(index_int i)const{return bool(range.first <= i && i < range.second);}


        // Value が変化したり、遅延評価がかかった場合に呼び出す。
        void NodeUpdate(){ 
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
        // eval は左右の子ノードの作成も担当する
        void eval(){
            // 代入(update) の評価が先
            if(this->lazy_update.first){
                this->Value = this->lazy_update.second;
                this->Min = this->lazy_update.second;
                this->Max = this->lazy_update.second;
                this->Sum = this->lazy_update.second*(range.second-range.first);
                if(this->left != nullptr){
                    // 片方のみ存在するのはあり得ない
                    this->left->set_lazyUpdate(this->lazy_update.second);
                    this->right->set_lazyUpdate(this->lazy_update.second);
                }
                this->lazy_update.first = false;//伝播したらフラグを折る
            }

            // 区間アフィン変換
            if(this->lazy_affine.first){
                this->Value = this->lazy_affine.second.first * this->Value + this->lazy_affine.second.second;
                this->Min = this->lazy_affine.second.first * this->Min + this->lazy_affine.second.second;
                this->Max = this->lazy_affine.second.first * this->Max + this->lazy_affine.second.second;
                this->Sum = this->lazy_affine.second.first * this->Sum + this->lazy_affine.second.second * this->range_length();
                if(this->left != nullptr){
                    // 片方のみ存在するのはあり得ない
                    this->left->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                    this->right->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                }
                this->lazy_affine.first = false;//伝播したらフラグを折る
            }
            // eval は子ノードの作成も担当
            make_child();
        }
    };

    
    // 数列の i 番目の要素に対応するノードまで subr からアクセスする (道中で遅延評価を降ろす)
    Node* access(index_int i , Node* subr){ 
        subr->eval();
        assert(subr->cover(i));
        if(subr->range_length()<=1)return subr;
        if(subr->left->cover(i))return access(i,subr->left);
        return access(i,subr->right);
    }

    // [l,r) に対応するセグメントを、bucket に入れていく
    void RangeSegments(index_int l ,index_int r , Node* subr , vector<Node*>& bucket){
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

    Node* m_Root = nullptr;
    index_int _Llim,_Rlim;

    // デストラクタ - メモリ解放
    void release(){
        // dfs で解放
        stack<Node*> s;
        s.push(m_Root);
        while(!s.empty()){
            Node* now = s.top();
            s.pop();
            if(now->left != nullptr)s.push(now->left);
            if(now->right != nullptr)s.push(now->right);
            delete now;
        }
    }
    public:
    SegmentTree(){}
    // index の範囲を受け取る
    SegmentTree(index_int L_ , index_int R_ , T init_value):_Llim(L_),_Rlim(R_),m_Root(new Node(nullptr,L_,R_,init_value)){}
    ~SegmentTree(){release();}
    // 複雑な挙動を回避するので、コンストラクタによるコピー/ムーブを一律に禁止する。
    SegmentTree(const SegmentTree<index_int,T> &x) = delete ;
    SegmentTree& operator = (const SegmentTree<index_int,T> &x) = delete ;
    SegmentTree ( SegmentTree<index_int,T>&& x){assert(0);}
    SegmentTree& operator = ( SegmentTree<index_int,T>&& x){assert(0);}

    // 有効な index の範囲が [ Llimit() , Rlimit() ) に対応
    index_int Llimit(){return _Llim;}
    index_int Rlimit(){return _Rlim;}

    // 数列の i 番目を x に変更する
    void update_val(index_int i , T x){
        assert(i >= Llimit() && i < Rlimit());
        Node* instance = access(i,m_Root);
        instance->Value = x;
        while(instance != nullptr){
            instance->NodeUpdate();
            instance = instance->parent;
        }
    }

    // 数列の i 番目の値を取得    
    T get(index_int i){
        assert(i >= Llimit() && i < Rlimit());
        return access(i,m_Root)->Value;
    }

    // 区間 [l,r) の min
    T RangeMinQuery(index_int l , index_int r){
        l = max(l,Llimit());r = min(r,Rlimit());
        assert(l<=r);
        vector<Node*> bucket;
        RangeSegments(l,r,m_Root,bucket);
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
        RangeSegments(l,r,m_Root,bucket);
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
        RangeSegments(l,r,m_Root,bucket);
        assert(bucket.size() > 0);
        T res = bucket[0]->Sum;
        for(int i = 1 ; i < bucket.size() ; i++)res += bucket[i]->Sum;
        return res;
    }

    // 半開区間 [l,r) 内の要素全てに x を代入
    void RangeUpdate(index_int l , index_int r, T x){
        l = max(l,Llimit());r = min(r,Rlimit());
        assert(l<=r);
        RangeUpdate_sub(l,r,x,m_Root);
    }

    // 半開区間 [l,r) 内の要素全てに A 掛けて B 足す
    void RangeAffine(index_int l , index_int r, T A , T B){
        l = max(l,Llimit());r = min(r,Rlimit());
        assert(l<=r);
        RangeAffine_sub(l,r,A,B,m_Root);
    }
                   
    // 半開区間 [l,r) 内の要素全てに x 足す
    void RangeAdd(index_int l , index_int r, T x){
        l = max(l,Llimit());r = min(r,Rlimit());
        assert(l<=r);
        RangeAffine_sub(l,r,T(1) , x,m_Root);
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


