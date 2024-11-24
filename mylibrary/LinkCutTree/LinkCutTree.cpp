#include "./../template/template.hpp"



/*   
    このコメントは消さないでください。
    Don't remove this comment!!!

    Copyright ©️ (c) NokonoKotlin (okoteiyu) 2024. 
    Released under the MIT license(https://opensource.org/licenses/mit-license.php) 
*/
template<typename T>
class LinkCutTree{
    private:
    
    struct SplayNode{
        SplayNode *parent = nullptr;
        SplayNode *left = nullptr;
        SplayNode *right = nullptr;
        
        // value of this node
        T Value;

        // Monoid product of subtree
        T Min,Max,Sum;

        // number of this node
        long long id = -1;

        // size of subtree
        long long SubTreeSize = 1;// SplayTree の部分木サイズ

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

        bool reverse_flag_lazy = false;
        void set_lazyReverse(){this->reverse_flag_lazy = !(this->reverse_flag_lazy);}
        
        pair<bool,pair<T,T> > lazy_affine ={false , {T(),T()}};
        void set_lazyAffine(T a, T b){
            if(this->lazy_affine.first)this->lazy_affine.second = { a*this->lazy_affine.second.first , a*this->lazy_affine.second.second + b};
            else this->lazy_affine = {true , {a,b}};
        }

        pair<bool,T> lazy_update = {false,T()};
        void set_lazyUpdate(T x){
            this->lazy_update.first = true;
            this->lazy_update.second=x;
            this->lazy_affine.first = false;
        }

        SplayNode(long long id_ ,T val){
            id = id_;
            Value = val;
            Min = val;
            Max = val;
            Sum = val;
            update();
        }
        
        void rotate(){
            if(this->parent->parent){
                if(this->parent == this->parent->parent->left)this->parent->parent->left = this;
                else if(this->parent == this->parent->parent->right)this->parent->parent->right = this; 
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
        /*
            direction of parent
            0 -> no parent or connected by light-edge
            else 1 or 2 (detail of direction is not so important)
        */
        int state(){
            if(this->parent == nullptr)return 0;
            this->parent->eval();
            if(this->parent->left == this)return 1;
            else if(this->parent->right == this)return 2;
            return 0;
        }

        void splay(){
            while(this->state()!=0){
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

        // サイズなど、Nodeの持っている情報をupdateする(順番大事)
        void update(){
            assert(copied_instance == false);
            
            this->eval();
            this->SubTreeSize = 1;
            this->Min = this->Value;
            this->Max = this->Value;
            this->Sum = this->Value;
            if(bool(this->left)){
                this->left->eval();
                this->SubTreeSize += this->left->SubTreeSize;
                if(this->left->Min < this->Min)this->Min = this->left->Min;
                if(this->left->Max > this->Max)this->Max = this->left->Max;
                this->Sum += this->left->Sum;
            }
            if(bool(this->right)){
                this->right->eval();
                this->SubTreeSize += this->right->SubTreeSize;
                if(this->right->Min < this->Min)this->Min = this->right->Min;
                if(this->right->Max > this->Max)this->Max = this->right->Max;
                this->Sum += this->right->Sum;
            }
            return;
        }
        void eval(){
            assert(copied_instance == false);
            
            if(this->reverse_flag_lazy){
                swap(this->left , this->right);
                if(bool(this->left))this->left->set_lazyReverse();
                if(bool(this->right))this->right->set_lazyReverse();
                this->reverse_flag_lazy = false;
            }

            if(this->lazy_update.first){
                this->Value = this->Min = this->Max = this->lazy_update.second;
                this->Sum = (this->lazy_update.second)*(this->SubTreeSize);
                if(bool(this->left))this->left->set_lazyUpdate(this->lazy_update.second);
                if(bool(this->right))this->right->set_lazyUpdate(this->lazy_update.second);
                this->lazy_update.first = false; 
            }
            if(this->lazy_affine.first){
                this->Value = this->lazy_affine.second.first * this->Value + this->lazy_affine.second.second;
                this->Min = this->lazy_affine.second.first * this->Min + this->lazy_affine.second.second;
                this->Max = this->lazy_affine.second.first * this->Max + this->lazy_affine.second.second;
                this->Sum = this->lazy_affine.second.first * this->Sum + this->SubTreeSize*this->lazy_affine.second.second;
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

    private:

    int m_N;
    // m_Nodes[i] := pointer of vertex i
    vector<SplayNode*> m_Nodes;
    
    vector<unordered_map<long long,bool>> m_ExistEdge;

    T init_v;
    
    SplayNode* get_sub(long long  index , SplayNode* root){
        if(root==nullptr)return root;
        SplayNode* now = root;
        while(true){
            now->eval();
            long long  left_size = 0;
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


    void init(){
        m_Nodes.clear();
        m_ExistEdge.clear();
        for(int i = 0 ; i < m_N ; i++)m_Nodes.push_back(new SplayNode(i,init_v));

        m_ExistEdge = vector<unordered_map<long long,bool>>(m_N);
    }
    void release(){
        for(SplayNode* p : m_Nodes){
            if(p != nullptr)delete p;
        }
    }

    public:
    
    LinkCutTree():init_v(T(0)){init();}
    LinkCutTree(int n, T init_v_ = T(0)):init_v(init_v_),m_N(n){init();}
    // Don' copy this object
    LinkCutTree(const LinkCutTree<T> &x) = delete ;
    LinkCutTree<T>& operator = (const LinkCutTree<T> &x) = delete ;
    // Don' move this object
    LinkCutTree (LinkCutTree<T>&& x){assert(0);}
    LinkCutTree<T>& operator = ( LinkCutTree<T>&& x){assert(0);}
    
    // construct a path from root to [id]
    long long access(const long long id){
        assert(0 <= id && id < m_N);
        if(bool(m_Nodes[id]) == false)return -1;
        long long res = id;
        while(1){
            m_Nodes[id]->splay();
            m_Nodes[id]->right = nullptr;
            m_Nodes[id]->update();
            if(bool(m_Nodes[id]->parent) == false)break;
            res = m_Nodes[id]->parent->id;
            m_Nodes[id]->parent->splay();
            m_Nodes[id]->parent->right = m_Nodes[id];
            m_Nodes[id]->parent->update();
        }
        return res;
    }

    long long path_xth_element(long long v , int x){
        assert(0 <= v && v < m_N);
        assert(0 <= x && x <= depth(v));
        access(v);
        SplayNode* nd = get_sub(x , m_Nodes[v]);
        access(v);
        return nd->id;
    }

    long long root(long long x){
        assert(0 <= x && x < m_N);
        return path_xth_element(x,0);
    }
    
    bool same(long long u , long long v){ 
        assert(0 <= u && u < m_N);
        assert(0 <= v && v < m_N);
        return bool(root(u) == root(v));
    }
    
    long long depth(long long x){
        assert(0 <= x && x < m_N);
        access(x);
        return m_Nodes[x]->SubTreeSize - 1;
    }

    pair<bool,long long> parent(long long x){
        assert(0 <= x && x < m_N);
        pair<bool,long long> res;
        access(x);
        SplayNode* p = m_Nodes[x]->_before();
        if(p == nullptr)res.first = false;
        else {
            res.first = true;
            res.second = p->id;
        }
        return res;
    }

    void evert(long long x){
        assert(0 <= x && x < m_N);
        access(x);
        m_Nodes[x]->set_lazyReverse();
        m_Nodes[x]->update();
        access(x);
    }
    
    void link(long long u , long long v){
        assert(0 <= u && u < m_N);
        assert(0 <= v && v < m_N);
        assert(!same(u,v));
        evert(v);
        access(v);
        access(u);
        m_ExistEdge[u][v] = true;
        m_ExistEdge[v][u] = true;
        m_Nodes[v]->parent = m_Nodes[u];
        m_Nodes[u]->update();
    }

    bool cut(long long u , long long v){
        assert(0 <= u && u < m_N);
        assert(0 <= v && v < m_N);
        assert(m_ExistEdge[u][v] && m_ExistEdge[v][u]);
        m_ExistEdge[u][v] = false;
        m_ExistEdge[v][u] = false;
        if(depth(u) > depth(v))swap(u,v);
        access(v);

        m_Nodes[v]->left->parent = nullptr;
        m_Nodes[v]->left = nullptr;
        m_Nodes[v]->update();
        return true;
    }

    void update_val(long long i , T x){
        assert(0 <= i && i < m_N);
        access(i);
        m_Nodes[i]->Value = x;
        m_Nodes[i]->update();
    }

    void PathUpdate(long long i , T x){
        assert(0 <= i && i < m_N);
        access(i);
        m_Nodes[i]->set_lazyUpdate(x);
        m_Nodes[i]->update();
        access(i);
    }

    void PathAffine(long long i , T A , T B){
        assert(0 <= i && i < m_N);
        access(i);
        m_Nodes[i]->set_lazyAffine(A,B);
        m_Nodes[i]->update();
        access(i);
    }

    void PathAdd(long long i , T x){PathAffine(i,T(1),x);}

    long long LCA(long long x , long long y){
        assert(0 <= x && x < m_N);
        assert(0 <= y && y < m_N);
        assert(same(x,y));
        if(root(x) != root(y))return -1;
        else {
            access(x);
            long long lca = access(y);
            access(lca);
            return lca;
        }
    }
    // get the copy object of node [nodeID]
    SplayNode operator [](long long nodeID){
        access(nodeID);
        return m_Nodes[nodeID]->copy();
    }
};





