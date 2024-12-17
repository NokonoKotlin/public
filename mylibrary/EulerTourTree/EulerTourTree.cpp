
/*
    このコメントは消さないでください。
    Don't Remove this Comment !!

    Copyright ©️ (c) NokonoKotlin (okoteiyu) 2024.
    Released under the MIT license(https://opensource.org/licenses/mit-license.php)
*/
template<typename T>
class EulerTourTree{
    private:
    
    struct SplayNode{
        SplayNode *parent = nullptr;
        SplayNode *left = nullptr;
        SplayNode *right = nullptr;
        T Value;
        T Min,Max,Sum;
        long long  SubTreeSize = 1;

        private:
        bool copied_instance = false;
        public:
        SplayNode copy(){
            assert(copied_instance == false);
            SplayNode res = *this;
            res.left = nullptr;
            res.right = nullptr;
            res.parent = nullptr;
            res.copied_instance = true;
            return res;
        }

        long long from , to;
        bool isNodeFactor = false;
        long long SubTreeNodeFactorCount = 0;

        pair<bool,pair<T,T> > lazy_affine ={false , {T(),T()}};
        void set_lazyAffine(T& a, T& b){
            if(this->lazy_affine.first)this->lazy_affine.second = { a*this->lazy_affine.second.first , a*this->lazy_affine.second.second + b};
            else this->lazy_affine = {true , {a,b}};
        }


        pair<bool,T> lazy_update = {false,T()};
        void set_lazyUpdate(T &x){
            this->lazy_update.first = true;
            this->lazy_update.second=x;
            this->lazy_affine.first = false;
        }
        
        SplayNode(){}
        SplayNode(long long u_, long long v_)
            :from(u_),to(v_),isNodeFactor(false){update();}
        SplayNode(long long u_, long long v_, T val)
            :from(u_),to(v_),isNodeFactor(true),Value(val){update();}

        void rotate(){
            if(this->parent->parent){
                if(this->parent == this->parent->parent->left)this->parent->parent->left = this;
                else this->parent->parent->right = this; 
            }
            this->parent->eval();
            this->eval();
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
            this->parent->eval();
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
            return;
        }

        void update(){
            assert(copied_instance == false);
            
            this->eval();
            this->SubTreeSize = 1;
            this->SubTreeNodeFactorCount = 0;
            if(this->isNodeFactor){
                this->Sum = this->Value;
                this->Min = this->Value;
                this->Max = this->Value;
                this->SubTreeNodeFactorCount++;
            }else{
                this->Sum = 0;
                this->Min = numeric_limits<T>::max();
                this->Max = numeric_limits<T>::min();
            }
            if(this->left != nullptr){
                this->left->eval();
                this->SubTreeSize += this->left->SubTreeSize;
                this->SubTreeNodeFactorCount += this->left->SubTreeNodeFactorCount;
            }
            if(this->right != nullptr){
                this->right->eval();
                this->SubTreeSize += this->right->SubTreeSize;
                this->SubTreeNodeFactorCount += this->right->SubTreeNodeFactorCount;
            }
            if(this->SubTreeNodeFactorCount == 0)return;
            
            if(this->left != nullptr){
                this->left->eval();
                this->Min = min<T>(this->left->Min,this->Min);
                this->Max = max<T>(this->left->Max,this->Max);
                this->Sum += this->left->Sum;
            }
            if(this->right != nullptr){
                this->right->eval();
                this->Min = min<T>(this->right->Min,this->Min);
                this->Max = max<T>(this->right->Max,this->Max);
                this->Sum += this->right->Sum;
            } 
            return;
        }

        void eval(){
            assert(copied_instance == false);
            
            if(this->SubTreeNodeFactorCount == 0){
                this->lazy_update.first = false;
                this->lazy_affine.first = false;
                return;
            }

            if(this->lazy_update.first){
                this->Value = this->lazy_update.second;
                this->Min   = this->lazy_update.second;
                this->Max   = this->lazy_update.second;
                this->Sum   = (this->lazy_update.second)*(this->SubTreeNodeFactorCount);
                if(bool(this->left))this->left->set_lazyUpdate(this->lazy_update.second);
                if(bool(this->right))this->right->set_lazyUpdate(this->lazy_update.second);
                this->lazy_update.first = false;
            }
            if(this->lazy_affine.first){
                this->Value = this->lazy_affine.second.first * this->Value + this->lazy_affine.second.second;
                this->Min = this->lazy_affine.second.first * this->Min + this->lazy_affine.second.second;
                this->Max = this->lazy_affine.second.first * this->Max + this->lazy_affine.second.second;
                this->Sum = this->lazy_affine.second.first * this->Sum + this->SubTreeNodeFactorCount*this->lazy_affine.second.second;
                if(bool(this->left))this->left->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                if(bool(this->right))this->right->set_lazyAffine(this->lazy_affine.second.first,this->lazy_affine.second.second);
                this->lazy_affine.first = false;
            }
        }

        SplayNode* _before(){
            SplayNode* res = this;
            res->splay();
            res->eval();
            if(res->left == nullptr)return nullptr;
            res = res->left;
            res->eval();
            while(res->right != nullptr){
                res = res->right;
                res->eval();
            }
            res->splay();
            return res;
        }
    };


    SplayNode *get_sub(int index , SplayNode *root){
        if(root==nullptr)return root;
        SplayNode *now = root;
        while(true){
            now->eval();
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
        if(bool(leftRoot ) == false)return rightRoot;
        if(bool(rightRoot) == false )return leftRoot;
        rightRoot = get_sub(0,rightRoot);
        rightRoot->left = leftRoot;
        leftRoot->parent = rightRoot;
        rightRoot->update();
        return rightRoot;
    }
    
    std::pair<SplayNode*,SplayNode*> split(int leftnum, SplayNode *root){
        if(leftnum<=0)return std::make_pair(nullptr , root);
        if(leftnum >= root->SubTreeSize)return std::make_pair(root, nullptr);
        root = get_sub(leftnum , root);
        SplayNode *leftRoot = root->left;
        SplayNode *rightRoot = root;
        if(bool(rightRoot))rightRoot->left = nullptr;
        if(bool(leftRoot))leftRoot->parent = nullptr;
        leftRoot->update();
        rightRoot->update();
        return std::make_pair(leftRoot,rightRoot);
    }
    
    
    int m_N;
    vector<SplayNode*> m_NodeFactor;
    vector<unordered_map<long long,SplayNode*>> m_EdgeFactor;
    T init_v;

    bool register_edge(long long u , long long v){
        assert(0 <= u && u < m_N);
        assert(0 <= v && v < m_N);
        if(m_EdgeFactor[u][v] != nullptr)return false;
        m_EdgeFactor[u][v] = new SplayNode(u,v);
        return true;
    }

    bool delete_edge(long long u , long long v){
        assert(0 <= u && u < m_N);
        assert(0 <= v && v < m_N);
        if(m_EdgeFactor[u][v] == nullptr)return false;
        delete m_EdgeFactor[u][v];
        m_EdgeFactor[u][v] = nullptr;
        return true;
    }


    long long find_NodeFactor(long long u){
        assert(0 <= u && u < m_N);
        m_NodeFactor[u]->splay();
        if(m_NodeFactor[u]->left != nullptr)return m_NodeFactor[u]->left->SubTreeSize;
        return 0;
    }
    
    long long find_EdgeFactor(long long u , long long v){
        assert(0 <= u && u < m_N);
        assert(0 <= v && v < m_N);
        if(m_EdgeFactor[u][v] == nullptr)return -1;
        m_EdgeFactor[u][v]->splay();
        if(m_EdgeFactor[u][v]->left != nullptr)return m_EdgeFactor[u][v]->left->SubTreeSize;
        return 0;
    }
    
    pair<SplayNode*,pair<SplayNode*,SplayNode*>> SplitSubTree(long long u , long long p){
        assert(0 <= u && u < m_N);
        assert(0 <= p && p < m_N);
        long long rt = root(u);
        if(u == rt){
            m_NodeFactor[u]->splay();
            return {m_NodeFactor[u],{nullptr,nullptr}};
        }
        assert(m_EdgeFactor[p][u] != nullptr);
        long long l = find_EdgeFactor(p,u);
        long long r = find_EdgeFactor(u,p);
        assert(l < r);
        m_NodeFactor[u]->splay();
        pair<SplayNode*,SplayNode*> tmp = split(r+1,m_NodeFactor[u]);
        SplayNode* rightnode = tmp.second;
        tmp = split(l,tmp.first);
        return {tmp.second,{tmp.first,rightnode}};
    }

    void init(){
        m_NodeFactor.clear();
        m_EdgeFactor.clear();
        for(int i = 0 ; i < m_N ; i++)m_NodeFactor.push_back(new SplayNode(i,i,init_v));
        m_EdgeFactor.resize(m_N);
    }
    void release(){
        for(SplayNode* p : m_NodeFactor)if(p != nullptr)delete p;
        for(unordered_map<long long,SplayNode*> table : m_EdgeFactor){
            for(pair<long long,SplayNode*>e : table)if(e.second != nullptr)delete e.second;
        }
    }
    public:

    EulerTourTree():init_v(T()){}
    EulerTourTree(int n ,T init_v_ = T(0)):init_v(init_v_),m_N(n){
        init();
    }
    ~EulerTourTree(){release();}
    EulerTourTree(const EulerTourTree<T> &x) = delete ;
    EulerTourTree<T> & operator = (const EulerTourTree<T> &x) = delete ;
    EulerTourTree ( EulerTourTree<T> && x){assert(0);}
    EulerTourTree<T> & operator = ( EulerTourTree<T> && x){assert(0);}


    void update_val(long long i , T x){
        assert(0 <= i && i < m_N);
        m_NodeFactor[i]->splay();
        m_NodeFactor[i]->Value = x;
        m_NodeFactor[i]->update();
    }


    long long EulerTourSize(long long r){
        assert(0 <= r && r < m_N);
        evert(r);
        m_NodeFactor[r]->splay();
        return m_NodeFactor[r]->SubTreeSize;
    }
    
    long long ComponentSize(long long u){
        assert(0 <= u && u < m_N);
        return (EulerTourSize(u) + 2)/3;
    }

    long long root(long long u){
        assert(0 <= u && u < m_N);
        SplayNode* tmp = m_NodeFactor[u];
        tmp->splay();
        tmp->eval();
        while(tmp->left != nullptr){
            tmp = tmp->left;
            tmp->eval();
        }
        tmp->splay();
        return tmp->from;
    }

    bool same(long long u ,long long v){return bool(root(u) == root(v));}

    void evert(long long u){
        assert(0 <= u && u < m_N);
        if(root(u) == u)return;
        long long c = find_NodeFactor(u);
        m_NodeFactor[u]->splay();
        pair<SplayNode*,SplayNode*> tmp = split(c,m_NodeFactor[u]);
        merge(tmp.second,tmp.first);
        m_NodeFactor[u]->splay();
    }

    bool exist_edge(long long u, long long v){
        assert(0 <= u && u < m_N);
        assert(0 <= v && v < m_N);
        return bool(m_EdgeFactor[u][v] != nullptr && m_EdgeFactor[v][u] != nullptr);
    }

    void link(long long u , long long v){
        assert(!same(u,v));
        assert(0 <= u && u < m_N);
        assert(0 <= v && v < m_N);
        register_edge(u,v);
        register_edge(v,u);
        evert(u);
        evert(v);
        m_NodeFactor[u]->splay();
        m_NodeFactor[v]->splay();
        merge(merge(merge(m_NodeFactor[u],m_EdgeFactor[u][v]),m_NodeFactor[v]),m_EdgeFactor[v][u]);
        m_NodeFactor[u]->splay();
    }

    void cut(long long u , long long v){
        assert(0 <= u && u < m_N);
        assert(0 <= v && v < m_N);
        assert(exist_edge(u,v));
        evert(u);
        long long l = find_EdgeFactor(u,v);
        long long r = find_EdgeFactor(v,u);
        m_NodeFactor[u]->splay();
        std::pair<SplayNode*,SplayNode*> tmp = split(r+1,m_NodeFactor[u]);
        tmp.first = split(r,tmp.first).first;
        tmp.first = split(l+1,tmp.first).first;
        tmp.first = split(l,tmp.first).first;
        merge(tmp.first, tmp.second);
        m_NodeFactor[u]->splay();
        m_NodeFactor[v]->splay();
        delete_edge(u,v);
        delete_edge(v,u);
    }

    void SubTreeUpdate(long long u , T x , long long p){
        assert(0 <= u && u < m_N);
        pair<SplayNode*,pair<SplayNode*,SplayNode*>> tmp = SplitSubTree(u,p);
        tmp.first->set_lazyUpdate(x);
        tmp.first->update();
        merge(merge(tmp.second.first, tmp.first),tmp.second.second);
    }

    void SubTreeAffine(long long u , T A , T B , long long p){
        assert(0 <= u && u < m_N);
        pair<SplayNode*,pair<SplayNode*,SplayNode*>> tmp = SplitSubTree(u,p);
        tmp.first->set_lazyAffine(A,B);
        tmp.first->update();
        merge(merge(tmp.second.first, tmp.first),tmp.second.second);
    }

    void SubTreeAdd(long long u , T x , long long p ){SubTreeAffine(u,T(1),x,p);}

    SplayNode SubTree(long long u , long long p){ 
        SplayNode res;
        assert(0 <= u && u < m_N);
        pair<SplayNode*,pair<SplayNode*,SplayNode*>> tmp = SplitSubTree(u,p);
        res = (*tmp.first);
        merge(merge(tmp.second.first, tmp.first),tmp.second.second);
        res.parent = res.left = res.right = nullptr;
        return res;
    }

    SplayNode tour(int i , long long r){
        assert(0 <= r && r < m_N);
        assert(0 <= i && i < this->EulerTourSize(r));
        evert(r);
        SplayNode* rt = m_NodeFactor[r];
        rt->splay();
        rt = get_sub(i,rt);
        SplayNode res = *rt;
        res.parent = res.left = res.right = nullptr;
        return res;
    }

    vector<long long> euler_tour(long long r){
        assert(0 <= r && r < m_N);
        evert(r);
        vector<long long> res(0);
        SplayNode* rt = m_NodeFactor[r];
        rt->splay();
        long long sz = rt->SubTreeSize;
        rt = get_sub(0,rt);
        res.push_back(rt->to);
        for( int i = 0 ; i < sz ; i++){
            rt = get_sub(i,rt);
            if(rt->from != rt->to)res.push_back(rt->to);
        }
        return res;
    }

    vector<long long> euler_tour_compress(long long r){
        assert(0 <= r && r < m_N);
        evert(r);
        vector<long long> res,tmp;
        tmp = this->euler_tour(r);
        unordered_map<long long,priority_queue<long long>> first;
        unordered_map<long long,priority_queue<long long>> last;
        for(int i =0 ; i < tmp.size() ; i++){
            first[tmp[i]].push(-i);
            last[tmp[i]].push(i);
        }
        for(int i =0 ; i < tmp.size() ; i++){
            if(i == abs(first[tmp[i]].top()))res.push_back(tmp[i]);
            if(i == abs(last[tmp[i]].top()))res.push_back(tmp[i]);
        }
        return res;
    }

    SplayNode operator [](long long nodeID){
        m_NodeFactor[nodeID]->splay();
        SplayNode res = *(m_NodeFactor[nodeID]);
        res.parent = res.left = res.right = nullptr;
        return res;
    }
};
