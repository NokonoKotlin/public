#include "./../template/template.hpp"










/*
    Copyright ©️ (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)
*/
template<long long base = 41 , long long MOD = 998244353>
class RollingHashSegTree{
    vector<long long> node;
    vector<pair<int,int>> range; //[x] := 頂点(セグメント) x が担当する半開区間 [ l , r ) 
    long long size;// 列(最下段)のサイズ
    vector<long long> b_pow;//[x] := base^x
    static const long long root = 1;
    void init(int size_){
        int p = 1;
        while(p < size_)p*=2;
        size = p;
        node.resize(size*2,0);
        range.resize(size*2,{2e9,-2e9});
        b_pow.resize(size+2,1);
        for(int x = 1 ; x < b_pow.size() ; x++)b_pow[x] = b_pow[x-1]*base%MOD;

        for(int i = 0; i < size ; i++){
            int now = i+size;
            range[now] = {i,i+1};
            while(now > root){
                int l = range[now].first;
                int r = range[now].second;
                now >>= 1;// 親ノード
                range[now].first = min(range[now].first , l);
                range[now].second = max(range[now].second , r);
            }
        }
    }

    // i 番目に x を代入
    void set_sub(int i , long long x){
        int now = size + i;
        long long val = node[now];// 元の値

        while(now > root){// 1 が 根ノードなので
            now >>= 1;
            // 親が [l , r) を担当しているところに
            int l = range[now].first;
            int r = range[now].second;
            int N_ = r-l;// 区間 の長さ
            int p = i-l;
            
            node[now] -= val*b_pow[N_-1-p]%MOD;// 元の値の分を減算
        }

        now = size+i;
        node[now] = x%MOD;

        while(now > root){
            now >>= 1;
            int l = range[now].first;
            int r = range[now].second;
            int N_ = r-l;
            int p = i-l;
            node[now] += x*b_pow[N_-1-p]%MOD;
            node[now] %=MOD;
            if(node[now]<0)node[now]+=MOD;
        }

    }

    pair<long long , int> hash_sub(int lef , int rig , int node_id = root){
        int SubTreeLeft = range[node_id].first;
        int SubTreeRight = range[node_id].second;
        int length = 0;
        long long res = 0;

        if(rig <= SubTreeLeft)return {0ll,0ll};
        else if(lef >=SubTreeRight)return {0ll,0ll};
        else if( lef <= SubTreeLeft && SubTreeRight <= rig )return {node[node_id],SubTreeRight-SubTreeLeft};
        else{
            // 右の根と左の根に降りて、左右の結果をガッチャンコ
            pair<long long , int> right_res =  hash_sub(lef,rig,node_id*2+1);
            res += right_res.first;
            length += right_res.second;

            pair<long long , int> left_res =  hash_sub(lef,rig,node_id*2);
            res += left_res.first*b_pow[length]%MOD;
            res %= MOD;
            if(res < 0)res += MOD;
            length += left_res.second;
            return {res ,length};
        }
    }

    public:

    RollingHashSegTree(){}
    RollingHashSegTree(int sz){
        init(sz);
    }
    // i 番目を x に変更
    void set(int i , long long x){set_sub(i,x);}
    // 半開区間 [l,r) のローリングハッシュを取得
    long long hash(int l , int r){return hash_sub(l,r).first;}
};







/*
    (base1,mod1) , (base2,mod2) の二種類のローリングハッシュを統合したハッシュ値
    列型 splay tree の各区間に対して、上記のハッシュを計算可能 & 挿入/削除/変更 も可能
    遅ければ 1 種類に減らしても運が良ければ通る
    mod1 × mod2 < 1e18 ぐらいが良い。
*/
namespace rh_array{

    const long long mod1 = 998244353;
    const long long mod2 = 998244853;
    const long long base1 = 41;
    const long long base2 = 52;

    vector<long long> power_base1(1,1),power_base2(1,1);
    // base1^x 
    long long power1(int x){
        int i = power_base1.size();
        while(i++ <= x)power_base1.push_back(power_base1.back()*base1%mod1);
        return power_base1[x];
    }    
    // base2^x 
    long long power2(int x){
        int i = power_base2.size();
        while(i++ <= x)power_base2.push_back(power_base2.back()*base2%mod2);
        return power_base2[x];
    }    

    // T からのハッシュ
    template<typename T>
    class RollingHashArray{
        private:

        struct SplayNode{
            SplayNode *parent = nullptr;
            SplayNode *left = nullptr;
            SplayNode *right = nullptr;
            T Value;
            long long Hash1,Hash2;
            int SubTreeSize = 1;
            SplayNode(){}
            SplayNode(T x ){Value = x;update();}

            void rotate(){
                if(this->parent->parent){//GrandParentの子を張り替え
                    if(this->parent == this->parent->parent->left)this->parent->parent->left = this;
                    else this->parent->parent->right = this; 
                }    
                if(this->parent->left == this){
                    this->parent->left = this->right;
                    if(this->right)this->right->parent = this->parent;
                    this->right = this->parent;
                    this->parent = this->right->parent;
                    this->right->parent = this;
                    this->right->update();
                }else{
                    this->parent->right = this->left;
                    if(this->left)this->left->parent = this->parent;
                    this->left = this->parent;
                    this->parent = this->left->parent;
                    this->left->parent = this;
                    this->left->update();
                }
                this->update();
                return;
            }
            int state(){
                if(this->parent == nullptr)return 0;
                if(this->parent->left == this)return 1;
                else if(this->parent->right == this)return 2;
                return 0;
            }
            void splay(){
                while(bool(this->parent)){
                    if(this->parent->state() == 0){
                        this->rotate();
                        break;
                    }
                    if( this->parent->state() == this->state() )this->parent->rotate();
                    else this->rotate();
                    this->rotate();
                }
                this->update();
            }

            void update(){
                this->Hash1 = 0;
                this->Hash2 = 0;
                this->SubTreeSize = 1;
                int r_size = 0;
                if(bool(this->right)){
                    this->SubTreeSize += this->right->SubTreeSize;
                    this->Hash1 += this->right->Hash1*power1(r_size)%mod1;
                    this->Hash2 += this->right->Hash2*power2(r_size)%mod2;
                    r_size += this->right->SubTreeSize;
                } 
                this->Hash1 += (this->Value)*power1(r_size)%mod1;
                this->Hash2 += (this->Value)*power2(r_size)%mod2;
                r_size++;
                if(bool(this->left)){
                    this->SubTreeSize += this->left->SubTreeSize;
                    this->Hash1 += this->left->Hash1*power1(r_size)%mod1;
                    this->Hash2 += this->left->Hash2*power2(r_size)%mod2;
                }
                this->Hash1 %= mod1;
                this->Hash2 %= mod2;
            }
        };

        SplayNode *getNode(int index , SplayNode *root){
            if(root==nullptr)return root;
            SplayNode *now = root;
            while(true){
                int left_size = 0;
                if(now->left != nullptr)left_size = now->left->SubTreeSize;
                if(index < left_size)now = now->left;
                else if(index > left_size){
                    now = now->right;
                    index -= left_size+1;
                }else break;
            }
            now->splay();
            return now;
        }
        
        SplayNode *merge(SplayNode *leftRoot , SplayNode *rightRoot){
            if(leftRoot!=nullptr)leftRoot->update();
            if(rightRoot!=nullptr)rightRoot->update();
            if(bool(leftRoot ) == false)return rightRoot;//右のみあり
            if(bool(rightRoot) == false )return leftRoot;//左のみあり
            rightRoot = getNode(0,rightRoot);
            rightRoot->left = leftRoot;
            leftRoot->parent = rightRoot;
            rightRoot->update();
            return rightRoot;
        }
        
        std::pair<SplayNode*,SplayNode*> split(int leftnum, SplayNode *root){
            if(leftnum<=0)return std::make_pair(nullptr , root);
            if(leftnum >= root->SubTreeSize)return std::make_pair(root, nullptr);
            root = getNode(leftnum , root);
            SplayNode *leftRoot = root->left;
            SplayNode *rightRoot = root;
            if(bool(rightRoot))rightRoot->left = nullptr;
            if(bool(leftRoot))leftRoot->parent = nullptr;
            leftRoot->update();
            rightRoot->update();
            return std::make_pair(leftRoot,rightRoot);
        }
        
        SplayNode *insert_sub(int index , SplayNode *NODE , SplayNode *root){
            NODE->update();
            if(bool(root) == false)return NODE;
            std::pair<SplayNode*,SplayNode*> Trees = split(index,root);
            return merge(merge(Trees.first,NODE),Trees.second);
        }

        std::pair<SplayNode*,SplayNode*> Delete_sub(int index , SplayNode *root){
            if(bool(root) == false)return std::make_pair(root,root);
            root = getNode(index,root);
            SplayNode *leftRoot = root->left;
            SplayNode *rightRoot = root->right;
            if(bool(leftRoot))leftRoot->parent = nullptr;
            if(bool(rightRoot))rightRoot->parent = nullptr;
            root->left = nullptr;
            root->right = nullptr;
            root->update();
            return std::make_pair(merge(leftRoot,rightRoot) , root );
        }


        protected:

        // 常に Root が根であるようにする (merge した後は Root に代入するなど)
        SplayNode *Root = nullptr;
        
        void release(){
            while(Root != nullptr){
                std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(0,Root);
                Root = tmp.first;
                delete tmp.second;
            }
        }
        // 初期状態にする (copy() などで使用)
        void init(){Root = nullptr;}
        // 先頭要素のアドレスを取得。
        const SplayNode* const begin(){
            if(size() == 0)return nullptr;
            Root = getNode(0,Root);
            return Root;
        }
        public:

        RollingHashArray(){init();}
        ~RollingHashArray(){release();}
        // 複雑な挙動を回避するので、コンストラクタによるコピー/ムーブを一律に禁止する。コピーは copy() 関数に任せる
        RollingHashArray(const RollingHashArray<T>  &x) = delete ;
        RollingHashArray& operator = (const RollingHashArray<T>  &x) = delete ;
        RollingHashArray ( RollingHashArray<T> && x){assert(0);}
        RollingHashArray& operator = ( RollingHashArray<T> && x){assert(0);}
        void copy(RollingHashArray<T> && x){
            // 自己代入禁止
            if(this->begin() == x.begin())return;// 先頭要素のアドレスだけ見れば、同一オブジェクトかを判定できる
            release();
            init();
            for(int i = 0 ; i < x.size() ; i++ )this->push_back(x[i]);
        }
        void copy(RollingHashArray<T> & x){
            // 自己代入禁止
            if(this->begin() == x.begin())return;// 先頭要素のアドレスだけ見れば、同一オブジェクトかを判定できる
            release();
            init();
            for(int i = 0 ; i < x.size() ; i++ )this->push_back(x[i]);
        }
        int size(){
            if(Root == nullptr)return 0;
            return Root->SubTreeSize;
        }

        SplayNode get(int i){
            assert(0 <= i && i < size());
            Root = getNode(i,Root);
            SplayNode res = (*Root);
            res.right = res.left = res.parent = nullptr;// 隣接するノードへのアクセスを封じる
            return res;
        }


        // [l,r) に対応する列のハッシュを計算
        long long hash(int l , int r){
            assert(0 <= l && l < r && r <= size());
            std::pair<SplayNode*,SplayNode*> tmp = split(r,Root);
            SplayNode* rightRoot = tmp.second;
            tmp = split(l,tmp.first);
            SplayNode res = (*tmp.second);// 取り出した部分木の根をコピーする。
            Root = merge(merge(tmp.first,tmp.second),rightRoot);
            return (long long)(res.Hash1*mod2 + res.Hash2);
        }


        // index 番目に x を挿入
        void insert(int index , T x){
            assert(0 <= index && index <= size());
            SplayNode* NODE = new SplayNode(x);
            Root = insert_sub(index , NODE , Root);
            return;
        }
        
        // index番目のNodeを消去
        void Delete(int index){
            if(index<0 || index >= size())assert(0);
            std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(index,Root);
            Root = tmp.first;
            delete tmp.second;
            return;
        }
        // x 番目のvalueをyにする
        void update_val(int x , T y){
            if(x<0 || x >= size())assert(0);
            Root = getNode(x,Root);
            if(bool(Root)==false)return;
            Root->Value = y;
            Root->update();
            return;
        }

        // 一列に表示
        void Debug(){std::cerr<<"DEBUG:" << std::endl;for( int i = 0 ; i < size() ; i++)std::cerr<< get(i).Value << " ";std::cerr<< std::endl;}
        // 末尾に追加
        void push_back(T val){insert(this->size() , val);return;}
        //先頭に追加
        void push_front(T val){insert(0 , val);return;}
        // 末尾を取得
        SplayNode back(){assert(size()>0);return get(size()-1);}
        // 先頭を取得
        SplayNode front(){assert(size()>0);return get(0);}
        // 末尾を削除
        void pop_back(){assert(size()>0);Delete(size()-1);}
        // 先頭を削除
        void pop_front(){assert(size()>0);Delete(0);}
        // read-only でアクセス
        T operator [](int index){return get(index).Value;}
    };

}




