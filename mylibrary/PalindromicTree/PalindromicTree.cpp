#include "./../template/template.hpp"







/*
    - 回分木
        |- 列 S の回文をノードとして持つ
    - 空の回文もノードとして持つが、(外部機能のために) 取得するときは空の列は回文として扱わない
    - 構造 : 各ノードは以下の 2 種類の辺をもつ。
        |- connect[x] := 自分自身の両端に x をつけた回文 (S に存在するなら) のノードへの有向辺 
        |- SuffixLink := 自分自身の Suffix のうち、最長の回文への有向辺
    |
    - SuffixLink を無向辺として見ると、このグラフは木となる。
        |- DP なども、このように考えた木上で行う
    |
    - 特殊なノードとして、以下がある。
        |- m_SeedNode : 長さが -1 の空回文を表す不自然なノード (頂点番号は -1 でアクセス不可)
            |- 外で使用する機能では基本的に無視する。
        |- m_EmptyNode : 長さが 0 の空回文を表す自然なノード (頂点番号は 0 )
    |
    - aggregate : ノードの情報を Suffix Link に沿って集約する。
        |- ただし空回文は集約に含んでいない。
        |- そうすることで、全体の集約データは m_EmptyNode からアクセスできる。
*/
template<typename T>
struct PalindromicTree{
    private:
    struct Node{
        int id;
        // ノードが表す回文の区間の参照 (半開区間)
        pair<int,int> cover;
        // [c] := 自身の両端に c をつけてできる回文ノードのポインタ
        unordered_map<T,Node*> connect;
        // 親ノード
        Node* parent = nullptr;
        // Suffix Link (存在するノードのうち、自身の接尾辞であり最長のノードのポインタ)
        Node* SuffixLink = nullptr;

        Node(int l , int r ,int id_, Node* p):cover({l,r}), id(id_) , parent(p){}
        
        // 半開区間の左端と右端 (0-index)
        int cleft() const& {return cover.first;}
        int cright() const& {return cover.second;}
        // 回文の長さ
        int length() const& {return cright() - cleft();}

        // このノードが表す回文の長さが非負であることを nature と呼ぶことにする。
        bool is_nature() const& {return bool(length()>0);}

        void deb() const& {
            cerr << " !!! DEBUG !!! " << endl;
            cerr << "id : " << id << endl;
            cerr << "cover : " << cleft() << " " << cright() << " : " <<length() <<  endl;
            if(this->SuffixLink != nullptr)cerr << "link : " << SuffixLink->id << endl;
        }

        // DP メンバ変数
        long long Frequency = 0; // この回文が連続部分列として何回登場するか
    };

    // 空の回文 : [x,x-1) に対応するノード (回文木の根みたいな立ち位置ですが、混乱するので根という表現は避けておく)
    Node* m_SeedNode;// 頂点番号は -1。アクセス不可
    // 
    Node* m_EmptyNode;// 空の回文 : [x,x) に対応するノード。頂点番号は 0。
    Node* m_SuffixPalindrome;// 最長接尾辞回文に対応するノード
    // 
    int m_vertex_id = -1;// 頂点 id に割り振っていく整数。木のサイズでもある。
    vector<const Node*> m_Nodes;// index でアクセスできるようにする。
    vector<const Node*> m_PrefixSuffixPalindrome;// [i] :=「S の先頭 i 文字」の接尾辞回文ノード
    

    // DP を計算した後は push などをしてはいけない
    bool is_static = false;// determine() を呼び出して、DP を計算したかどうか

    vector<T> S;
    void init(){
        S.clear();
        m_vertex_id = -1;
        // ある回文に対応する区間は複数存在しうるが、そのうち 1 つで特殊化して OK (構築では長さしか興味ないので)
        m_SeedNode = new Node(0,-1,m_vertex_id++,nullptr);// 不自然な空回文 (長さが -1)
        m_EmptyNode = new Node(0,0,m_vertex_id++,nullptr); // 自然な空回文 (空列)
        m_EmptyNode->SuffixLink = m_SeedNode;

        m_SuffixPalindrome = m_EmptyNode;// 自然な空回文から始める (SuffixLink を辿れば不自然な空回文にも行ける)

        m_Nodes.push_back(m_EmptyNode);
        m_PrefixSuffixPalindrome.push_back(m_EmptyNode);// 空文字列の接尾辞回文
    }

    
    // 今見ている SuffixPalindrome に対して、両端に c をつなげることを試みる
    //     - 成功したら S の末尾に c を追加し、SuffixPalindrome に c を追加した回文の Node* を返す。
    //     - 失敗したら何もせずに nullptr を返す。
    //     副作用 : 成功した場合、S や新たにできるノード周りの変数を書き換える
    //     注意 : Node の集約値やカウントは行わない。
    Node* try_connect(T c){
        const int psize = m_SuffixPalindrome->length();
        // 範囲外になる場合
        if(psize + 2 > int(S.size()) + 1 )return nullptr;
        // 回文じゃなくなるかチェック ( 空回文 [0, -1) からは問答無用で connect できる)
        if( m_SuffixPalindrome != m_SeedNode)if(S[int(S.size())-psize-1] != c)return nullptr;

        S.push_back(c);// c + m_SuffixPalindrome + c が 接尾辞回文になったら S に反映

        // すでに存在するなら、それを返す
        if(m_SuffixPalindrome->connect[c] != nullptr)return m_SuffixPalindrome->connect[c];

        // 新たな回文ができる場合、新たに Node を生成する
        Node* res = new Node(int(S.size())-psize-2 , S.size() , m_vertex_id++ , m_SuffixPalindrome);
        m_Nodes.push_back(res);
        res->parent = m_SuffixPalindrome;
        res->SuffixLink = m_EmptyNode;// 空回文 2 にデフォルトでリンク。

        // SuffixLink は、now の SuffixLink を辿って構築
        Node* linked = m_SuffixPalindrome->SuffixLink;
        while(linked != nullptr){
            // 「リンク先が回文 → すでにノードが存在する」が成り立つので、対偶を取って判定
            if(linked->connect[c] != nullptr){
                if(S[int(S.size()) - linked->length()-2] == c){
                    res->SuffixLink = linked->connect[c];
                    break;
                }
            }
            linked = linked->SuffixLink;
        }
        return (m_SuffixPalindrome->connect[c] = res);
    }

    // S の末尾に c を追加する。移動先の接尾辞回文ノードを返す。
    Node* push_back_sub(T c){
        // 左端が c である接尾辞回文を Suffix Link を辿って探す。
        Node* next_sp = try_connect(c);
        while(next_sp == nullptr){
            m_SuffixPalindrome = m_SuffixPalindrome->SuffixLink;
            next_sp = try_connect(c);// Suffix Link 先が nullptr になるまでには終了する
        }
        m_SuffixPalindrome = next_sp;
        m_SuffixPalindrome->Frequency++;
        // このタイミングで集約やカウントの更新をする
        return m_SuffixPalindrome;
    }

    void release(){
        vector<const Node*> nds = crawl();
        for(const Node* x : nds)delete x;
    }

    public:
    PalindromicTree(){init();}
    ~PalindromicTree(){release();}
    // コピー,ムーブ の禁止
    PalindromicTree(const PalindromicTree &x) = delete ;
    PalindromicTree& operator = (const PalindromicTree &x) = delete ;
    PalindromicTree ( PalindromicTree&& x){assert(0);}
    PalindromicTree& operator = ( PalindromicTree&& x){assert(0);}

    // 列の末尾に c を加える
    void push_back(T c){
        assert(!is_static);
        m_PrefixSuffixPalindrome.push_back(push_back_sub(c));
    }

    // Suffix Link の向きに沿って値を集約する。ただし空回文は集約に含んでいない
    // m_SeedNode ノードに集約しても良いが、全体の集約は m_EmptyNode からアクセスする。
    void aggregate(){
        is_static = true;
        
        for(int i = m_vertex_id-1 ; i>=0 ; i--){// crawl の逆順でも OK
            if(m_Nodes[i]->SuffixLink == nullptr)continue;
            m_Nodes[i]->SuffixLink->Frequency += m_Nodes[i]->Frequency;
        }
    }


    // 文字列のサイズ
    long long str_size(){return S.size();}
    long long tree_size(){return m_vertex_id;}

    const Node* get_node(int i){assert(0<=i && i < tree_size());return m_Nodes[i];}
    const Node* PSP(int i){assert(0<=i && i <= str_size());return m_PrefixSuffixPalindrome[i];}


    // m_SeedNode からクロールして、全てのノードを 1 回ずつ vector に入れて返す。
    // 回文のサイズでソートしたものを返す。ただし m_SeedNode ノード (長さ -1) は無視する 
    vector<const Node*> crawl(){
        vector<const Node*> res;
        for(int i = 0 ; i < m_Nodes.size() ; i++)res.push_back(m_Nodes[i]);
        sort(res.begin(),res.end(),[](const Node* a , const Node* b){return a->length() < b->length();});
        return res;
    }
};







