#include "MyTemplate.hpp"



/*
    S の Suffix Tree。
    接尾辞を***辞書順***に Suffix Tree に追加していく。   

    ある接尾辞を追加する時、一つ前に追加した接尾辞との LCP が、Suffix Tree に接尾辞を追加する時の共有パスとなる。  
    
    新たに接尾辞を追加する際に分岐する地点を、ひとつ前に追加した接尾辞に対応する葉ノードから親に遡って探索する。

    追加する時に見る頂点の移動とその順番に注目すると、
    これは完成後の Suffix Tree で辞書順に辺を選ぶ DFS を行う時の移動経路と同じになる。

    よって、接尾辞の追加を行う際の分岐点を愚直に探しても O(|S|) で構築可能。  

    boid build(): 文字列 s_ , その Suffix Array と LCP Array から Suffix Tree を構築
        Suffix Array は以下の要件を満たす必要がある。  
            ・0-index 
        LCP Array は以下の要件を満たす必要がある。  
            ・LCP[0] = 0
            ・LCP[x] := suffix_array[i-1] と suffix_array[i] の LCP
*/
template<typename T = long long>
class StaticSuffixTree{ 
    private:
    struct Node{
        Node(pair<int,int> cover , int id_ = -1 , Node* parent_ = nullptr):cover(cover),id(id_),parent(parent_){}

        int path_length = 0;//根からこのノードまでの文字(葉ノードの深さは不定)
        //このノードに入ってくる辺がどの連続部分列をカバーしてるか(閉区間 [l , r] を表す) (1-index)
        pair<int ,int> cover = make_pair(-1,-2);

        int cover_length(){
            if(this->parent == nullptr)return 0;// root の場合
            return cover.second - cover.first + 1;
        }

        int id = -1;//id(頂点番号 ... 追加された順に割り振る)
    
        unordered_map<T , Node*> child;//[ x ] :=  要素 x で続く辺の先のノード(遅ければ unordered にする)
        Node* parent = nullptr;//親ノード( nullptr <=> このノードがroot )

        int suffix_id = -1; // この Node が 何文字目から始まる接尾辞を表しているか(1-index) (接尾辞に対応していない場合は -1)
        bool is_endpoint(){return (this->suffix_id >= 1);}// 接尾辞に対応するノードかどうか

        int SubTreeLeafCount = 0;          // 自分以降の部分木が持つ葉の個数(専用の初期化関数で初期化)
        long long SubTreeKindOfPrefix = 0; // このノードから移動して得られる文字列の種類数 (ただし、終端文字や空文字列を含まない)
    };

    /*
        Suffix Tree 上の地点を表すデータ(辺の途中の場合もある) : 
        ちょうど 頂点 x の上である場合、RecentNode = x , NextNode = nullptr , place = 0 で統一する
    */
    struct PositionData{
        Node* RecentNode = nullptr;//すでに到達したノードで、直近のもの(これまで到達した中で、今いる地点に一番近いノード)
        Node* NextNode = nullptr;//次に向かうノード(今いる辺の先)
        int place = 0;// 辺の何文字目の箇所か

        // 根から現在地までのパス上の文字数(距離)
        int get_path_length(){return this->RecentNode->get_path_length() + this->place;}

        // 地点が同じ地点かどうか
        bool operator ==(PositionData& b){
            return (this->RecentNode == b.RecentNode && this->NextNode == b.NextNode && this->get_path_length() == b.get_path_length());
        }

        void deb(){
            cerr<< "now on : " << RecentNode->id;
            if(NextNode!=nullptr)cerr <<  " → " << NextNode->id << " : " << place;
            cerr << endl;
        }
        
    };

    static bool cmp_char(pair<char,Node*> a ,pair<char ,Node*> b){
        return a.first<b.first;
    };


    // 根頂点
    Node* root;
    vector<T> S;
    int vertex_id = 0;
    
    void init(){
        vertex_id=0;
        S.clear();
        root = new Node({-1,-2} , vertex_id++ , nullptr);
    }
    
    /*
        文字列 s_ , その Suffix Array と LCP Array から Suffix Tree を構築
        Suffix Array は以下の要件を満たす必要がある。  
            ・0-index 
        LCP Array は以下の要件を満たす必要がある。  
            ・LCP[0] = 0
            ・LCP[x] := suffix_array[i-1] と suffix_array[i] の LCP
    */
    void build_sub(vector<T> s_ , vector<int> suffix_array , vector<int> lcp_array){
        init();
        S = s_;
        Node* now = root;// 最後に見た頂点

        // 空文字列は無視
        for(int i = 0 ; i < suffix_array.size() ; i++){
            if(suffix_array[i] >= S.size())continue;// 空文字列の分は無視
            Node* NewLeaf;
            int depth = lcp_array[i];
            int suffix_id = suffix_array[i] + 1;// 1-index
            T c = S[suffix_id-1];
            // 必要な分だけ戻る
            while(depth < now->path_length)now = now->parent;
            
            // 新たに分岐点を作る必要がある場合
            if(depth != now->path_length){
                T edge_id = S[suffix_id + now->path_length - 1];
                Node* next = now->child[edge_id];// 辺の先の頂点
                int lef = next->cover.first;
                int mid = lef + depth - now->path_length - 1;

                Node* NewBranch = new Node({lef,mid} , vertex_id++ , now);
                NewBranch->path_length = depth;
                NewBranch->child[S[mid]] = next;

                // 分岐点の追加に伴う変数の再計算をする
                next->cover.first = mid+1;
                next->parent = NewBranch;
                now->child[edge_id] = NewBranch;
                now = NewBranch;
            }

            // 辞書順に接尾辞を見ているので、この時点で接尾辞を追加するノードは必ず葉であることが保証される
            NewLeaf = new Node({suffix_id + now->path_length ,S.size()} , vertex_id++ , now);// 区間は 1-index の閉区間
            NewLeaf->suffix_id = suffix_id;
            NewLeaf->path_length = now->path_length + NewLeaf->cover_length();
            now->child[S[NewLeaf->cover.first-1]] = NewLeaf;
            now = NewLeaf;
        }
    }




    // 外部向け機能
    public:// findValidPath と walkdown も public で使えるべき


    StaticSuffixTree(){}
    ~StaticSuffixTree(){}


    // 地点 : dataから要素 x に向かって進むことができるかを判定
    bool findValidPath(T x , PositionData& data){
        if(data.NextNode == nullptr )return (data.RecentNode->child[x] != nullptr);// ノードの上にいる場合は、x 方向に辺があるかを確かめる
        else return (S[data.NextNode->cover.first + data.place - 1] == x);// 辺の上にいるときは、次の文字を確かめる
    }


    // 地点 : data から x の方向に 1 進み、data を更新する
    bool walkdown( T x , PositionData& data ){
        if(findValidPath(x,data) == false)return false;//進めない
        Node* branch = data.NextNode;
        //分岐点にいる場合
        if(data.NextNode == nullptr)data.NextNode = data.RecentNode->child[x];
        data.place++;//進む
        // 分岐点に到達した場合、RecentNode が分岐点を表す様にフォーマットする        
        if(data.NextNode != nullptr && data.place == data.NextNode->cover_length()){
            data.RecentNode = data.NextNode;
            data.NextNode = nullptr;
            data.place = 0;
        }
        return true;
    }

    
    /*
        文字列 s_ , その Suffix Array と LCP Array から Suffix Tree を構築
        Suffix Array は以下の要件を満たす必要がある。  
            ・0-index 
        LCP Array は以下の要件を満たす必要がある。  
            ・LCP[0] = 0
            ・LCP[x] := suffix_array[i-1] と suffix_array[i] の LCP
    */
    void build(string& s , vector<int>& suffix_array , vector<int>& lcp_array){
        vector<T> s_;
        for(char c : s)s_.push_back(T(c));
        build(s_,suffix_array,lcp_array);
    }
    void build(vector<T>& s , vector<int>& suffix_array , vector<int>& lcp_array){build_sub(s,suffix_array,lcp_array);}

    // dfs 順の Node* を返す (辞書順に dfs するかを、引数で指定)
    vector<Node*> dfs(bool lexicographical = false){
        vector<Node*> res;
        if(root == nullptr)return res;
        stack<Node*> s;
        s.push(root);
        while(!s.empty()){
            Node* now = s.top();s.pop();
            res.push_back(now);
            vector<pair<char , Node*> > nxt;
            for(pair<char , Node*> nx : now->child )if(nx.second != nullptr)nxt.push_back(nx);
            if(lexicographical)sort(nxt.rbegin() , nxt.rend() , cmp_char);//辺の最初の文字でソート
            for(pair<char , Node*> nx : nxt )s.push(nx.second);
        }
        return res;
    }


    void debug_dfs(){
        vector<Node*> d = dfs();
        vector<pair<int,int> > links;
        for(Node* now : d){
            if(now != root)cerr << now->parent->id << " → " << now->id << endl;
            if(now != root)cerr << "cover : " << now->cover.first << " " << now->cover.second << endl;
        }
    }


};















/*
    - T 型の列 : S に対して、頂点 0 を根とする Suffix Tree を構築 !!!!!!!!!!!!!!!(変数などは基本的に 1-index です)!!!!!!!!!!!!!!!
    - vector の Suffix Tree ですが string / vector の両方に対応したいので template<class VecOrStr> する
    - オンラインで S を編集 (push_back のみ) しながら Suffix Tree にも変更を反映する
    - 頂点は Node クラスのポインタで扱うが、!!!!!!!!!!!!!!!!!!内部処理以外では絶対にポインタの中身を変更してはいけない!!!!!!!!!!!!!!!!!!
    - ノードの番号は生成順(root の id が 0)。
    - 空列も接尾辞として含み、辞書順で最小とする(suffix_id は |S|+1 とする)
        |- 空列の接尾辞に対応するノードが root であるとする。
    |
    |
    - Node クラスが、Suffix Tree を構成する 
        |- id : 頂点の生成順に割り振られる
        |- parent : 自身の親 Node のポインタ
        |- child[x] := 先頭が x の辺の先の Node のポインタ (辺は先頭の文字でユニーク)
        |- cover : 自身に入ってくる辺に対応する列が、S のどの連続部分列かを表す {int,int*} 
            |- S の長さが不定なので、S の終端を表すときは S_length_Symbol という変数を参照するポインタで表す
        |- get_path_length() : 根から現在地までのパス上の要素数 (辺の本数ではないことに注意)
        |- isEndPoint() : このノードが、S のいずれかの接尾辞に対応しているか
            |- 根からこのノードまでの経路上の要素の列が、S の接尾辞かどうか
            |- suffix_id : 接尾辞に対応する場合、何番目から始まるものか(1-index)(なければ-1)
        |- SuffixLink : 「根からの経路上の要素の列の先頭を削除してできた列」に対応するノードのポインタ
        |- 木 DP の変数なども持つ
    |
    |
    - root から列 R に沿って移動した結果到達した地点を「R に対応する地点」と呼ぶことにする
    - 地点 P に対応する文字列を「P に対応する列」と呼ぶことにする
    |
    - PositionData という構造体は Suffix Tree 上の地点を表す 
        |- RecentNode から NextNode の方向に place だけ進んだ地点を指す。
        |- ちょうど頂点の上の場合は、NextNode を nullptr , place を 0 とする。
        |- get_path_length() : 今いる地点に対応する列の長さ (地点の深さ) を返す
        |- 今いる地点に対応する列を refer とする(ただし deque で実装してある)
        |- walkSuffixLink : 現在地から、refer の先頭を削除した列に対応する地点に移動する(refer も変更される) 
    |
    - from_root() で root を表す地点のデータを受け取る(autoを使ってね)
    |               
    |
    - (割と重要) : Suffix Tree 上の各地点は S の連続部分列と 1 対 1 対応している (連続部分列を重複なく列挙しているのと同様)
    - (割と重要) : S の [L,R] に対応する辺上の地点に対応する連続部分列は、S の L - RecentNode.get_path_length() 文字目から始まる(ように実装しています)
    |
    |
    - Suffix Tree 構築メソッド
        |- push(s_) : オンライン構築中の S に、s_ (列 or T 型単体) をくっつけて、Suffix Tree にも反映させる。
            |- オンライン構築中の Suffix Tree では、tree_dp 系の、全ての接尾辞を格納している前提の操作は行えない。
        |- determine() : オンライン構築を終了し、オフラインにする
        |- build(s) := 初期状態から push(s) + determine()
        |- determine() : しないと、木 DP などができないので制限される機能がある
    |
    |
    - find(t) := t が S の連続部分列かどうかを判定
    - count(t) := t が S の連続部分列として何回現れるか計算
    - search(t):= t を検索し、移動できた地点までのデータを返す
    - size() := S のサイズを返す
    - LCA(地点のデータ 2 つ) := 地点(辺の途中含む)の LCA のデータを返す。
    |
    - その他機能(詳細はコメントを参照)
    - suffix_array() , get_Kth_subStr(K) , matching_statistics(t)
    |
*/
template<typename T = long long>
class SuffixTree{
    private:

    struct Node{
        Node(pair<int,int*> cover , int id_ = -1 , Node* parent_ = nullptr , Node* SuffixLink_ = nullptr)
            :cover(cover),id(id_),parent(parent_),SuffixLink(SuffixLink_){}

        int path_len_sub = 0;//根からこのノードまでの文字(葉ノードの深さは不定)
        int get_path_length(){//根からノードまでの辺の文字数 path_len_sub を参照して再計算
            if(this->parent == nullptr)return 0;
            return this->parent->path_len_sub + cover_length();
        }

        //このノードに入ってくる辺がどの連続部分列をカバーしてるか(閉区間 [l , r] を表す) (1-index)
        pair<int ,int*> cover = make_pair(-1,nullptr);// ただし、r は整数へのポインタとする(葉ノードの右端は S_length_Symbol を参照する)

        // 辺に書かれた文字列の区間の左端と右端(1-index)
        int cleft(){return cover.first;}
        int cright(){return *cover.second;}
        int cover_length(){
            if(this->parent == nullptr)return 0;// root の場合
            return cright() - cleft() + 1;
        }

        int id = -1;//id(頂点番号 ... 追加された順に割り振る)
    
        unordered_map<T , Node*> child;//[ x ] :=  要素 x で続く辺の先のノード
        Node* parent = nullptr;//親ノード( nullptr <=> このノードがroot )
        
        Node* SuffixLink = nullptr;// 「このノードに対応する文字列」の「先頭を削除した文字列に対応するノード」へリンクする

        // このノードが、いずれかの接尾辞に対応しているかどうか
        bool isEndPoint(){return (this->suffix_id >= 1);}
        int suffix_id = -1; // isEndPoint()なら、何文字目から始まるものか(1-index)。そうでないなら -1。
        
        // 木 DP (専用の初期化関数で初期化)
        int SubTreeEndPointCount = 0;      // 自分以降の部分木が持つ EndPoint の個数
        long long SubTreeKindOfPrefix = 0; // このノードから移動して得られる文字列の種類数 (空文字列を含まない)
    };



    /*
        頂点 RecentNode から、NextNode の方向に place 進んだ地点を指すデータ。  
        ちょうど 頂点 x の上である場合、RecentNode = x , NextNode = nullptr , place = 0 で統一する
        インスタンスのコピーに O(N) かかるので、必要に応じて refer をコピーしないようにしたりする
        そのため、get_path_length() などは refer に依存しないほうがいい
    */
    struct PositionData{
        Node* RecentNode = nullptr;//すでに到達した直近のノード
        Node* NextNode = nullptr;//次に向かうノード(今いる辺の先)
        int place = 0;// 辺の先頭から何文字目の箇所か

        // 根から現在地までのパス上の文字数(距離)
        int get_path_length(){return this->RecentNode->get_path_length() + place;}

        // この地点に対応する文字列 (pop_front するので deque で管理)
        deque<T> refer;

        // 現在地(refer に対応する地点) から、refer の先頭を削除したものに対応する地点に移動
        bool walkSuffixLink(){
            if(refer.size() == 0)return false; // これ以上 Suffix Link を辿れないなら return false
            // 先頭の文字を削除する
            refer.pop_front();

            // root → root の移動は辺上の位置も変わる
            if(this->RecentNode == this->RecentNode->SuffixLink)this->place--;
            // Suffix Link を辿って移動する
            this->RecentNode = this->RecentNode->SuffixLink;

            // 遷移先も頂点の上の場合は何もせず終了
            if(place == 0){
                this->NextNode = nullptr;
                return true;
            }

            // Suffix Link を辿ったあとの NextNode を、RecentNode の深さと refer から復元
            this->NextNode = this->RecentNode->child[refer[this->RecentNode->get_path_length()]];
            
            // place が Recentnode と NextNode の間にくるように整形
            while(this->NextNode != nullptr && this->NextNode->cover_length() <= this->place){
                this->place -= this->NextNode->cover_length(); 
                this->RecentNode = this->NextNode;
                if(this->place == 0)this->NextNode = nullptr;
                else this->NextNode = this->RecentNode->child[refer[this->RecentNode->get_path_length()]];
            }

            return true;
        }

        // 地点が同じ地点かどうか (refer を参照しないので O(1) )
        friend bool operator ==(PositionData& a,  PositionData b){
            return (a.RecentNode == b.RecentNode && a.NextNode == b.NextNode && a.place == b.place);
        }

        void deb(){
            cerr<< "now on : " << RecentNode->id<< " - " << place << " th place ";
            if(NextNode!=nullptr)cerr <<  " → " << NextNode->id;
            cerr << endl;
            cerr << "length : " << get_path_length() << endl;
        }
        
    };


    // 要素比較用 (suffix array や、辞書順で k 番目の連続部分列の計算に使用)
    static bool cmp_(pair<char,Node*> a ,pair<char ,Node*> b){
        return a.first<b.first;
    };

    // メンバたち
    // これらは内部関数でのみ編集される
    int vertex_id = 0;// ノードができた順に id を 0 から割り振る(現時点の Suffix Tree の頂点数でもある)
    vector<T> S;// Suffix Tree 構築元の列
    int S_length_Symbol = 0;// S の終端を表すためのシンボル (葉ノードに入る辺の右端 (int*) に参照される)
    Node* root = nullptr;// 根を表すデータ(Node* のリンクで木を表現)
    Node* lastCreatedLeafNode = nullptr;// 最後に追加された葉ノード(葉のSuffix Link を構築する用)
    //
    // ノードを追加する際に、共有する部分の(端点の)地点データ
    PositionData C_data_;
    //
    bool _determined = false;// 構築が完全終了したか (それぞれの接尾辞に対応するノードが存在するか)
    bool initialized_basic_data = false;//基本的なデータを初期化ずみか
    bool initialized_tree_dp = false;//Suffix Tree の部分木の情報を 木DP で初期化したかどうか
    bool initialized_LCA = false;// LCA の索引について、初期化したかどうか
    //
    vector<Node*> Nodes;//     [x] := 頂点番号 x のノードへのポインタ
    vector<Node*> SuffixNodes;// [x] := x 文字目から始まる Suffix に対応するノードへのポインタ(1-index)
    vector<int> tree_depth;// [x] := 頂点 x まで辺が何本か
    vector<vector<int> > parent; // [i][u] := 頂点 u の 2^i 個上の頂点
    //



    // 基本的なデータの初期化(ノードへのアクセス用の配列など)
    void init_data(){
        if(root == nullptr)return;
        if(initialized_basic_data)return ;
        if(!_determined)assert("接尾辞に対応するノードが存在する必要があります"=="");
        initialized_basic_data = true;
        Nodes.resize(tree_size() , nullptr);
        tree_depth.resize(tree_size() , 0);
        SuffixNodes.resize(S.size()+2 , nullptr);// 空文字列 (|S|+1 からの接尾辞) も含む
        vector<Node*> s = dfs();
        tree_depth[root->id] = 0;
        for(Node* now : s){
            Nodes[now->id] = now;//id と Node* を対応付け
            if(now->isEndPoint())SuffixNodes[now->suffix_id] = now; // x 文字目から始まる Suffix に対応するノードを管理
            if(now->parent != nullptr)tree_depth[now->id] = tree_depth[now->parent->id] + 1;
        }
    }


    // あるノード以降の部分木に関するデータを計算
    void init_tree_dp(){
        if(root == nullptr)return;
        if(initialized_tree_dp)return;//すでに計算済み
        if(!_determined)assert("接尾辞に対応するノードが存在する必要があります"=="");
        initialized_tree_dp = true;
        vector<Node*> rdfs = dfs();
        reverse(rdfs.begin() , rdfs.end());
        for(Node* now:rdfs){// 初期化
            now->SubTreeKindOfPrefix = 0;
            now->SubTreeEndPointCount = 0;
        }

        for(Node* now:rdfs){
            if(now->isEndPoint())now->SubTreeEndPointCount+=1;
            if(now->parent == nullptr)continue;

            // 部分木のノードのうち、接尾辞と対応しているものの個数
            now->parent->SubTreeEndPointCount += now->SubTreeEndPointCount; 
            // 部分木のパスが表す文字列の種類数 + 辺の分
            now->parent->SubTreeKindOfPrefix += now->SubTreeKindOfPrefix + now->cover_length();
        }
        
    }


    // LCA 計算用の、parent 配列の初期化
    void init_LCA(){
        if(root == nullptr)return;
        if(initialized_LCA)return;//計算済みなので
        if(!_determined)assert("接尾辞に対応するノードが存在する必要があります"=="");
        initialized_LCA = true;
        const int K = log2(tree_size())+2;
        parent.resize(K,vector<int>(tree_size(),-1));
        vector<Node*> s = dfs();
        
        for(Node* now : s)if(now->parent != nullptr)parent[0][now->id] = now->parent->id;

        for(int x = 1 ; x < K ; x++){
            for(int u = 0 ; u < parent[x].size(); u++){
                int mid = parent[x-1][u];
                if(mid >= 0)parent[x][u] = parent[x-1][mid];
                else parent[x][u] = -1;// 存在しない
            }
        }
    }


    // メンバの初期化
    void reset(){
        S.clear();
        Nodes.clear();
        SuffixNodes.clear();
        parent.clear();
        initialized_basic_data = false;
        initialized_tree_dp = false;
        initialized_LCA = false;

        // 構築用データ初期化
        vertex_id=0;
        root = new Node({0,new int(0)}, vertex_id++,nullptr,nullptr);
        root->SuffixLink = root; // root からは root にリンクが出ている(ここだけ例外)

        C_data_ = from_root();
    }


    

    // x をオンラインで S の末尾に連結する 
    void push_sub(T x){
        if(root == nullptr || S.size() == 0)reset();//初期化
        assert(!_determined && "構築を完了した Suffix Tree を更新してはいけない" != "");
        
        S.push_back(x); 
        S_length_Symbol++;// x を追加したので、S の長さが 1 増える

        // この変更において、葉を追加するために最後に使用した内部ノード
        Node* lastUsedBranch = nullptr;

        do{
            // 現時点の Suffix Tree で C_data_ → x に移動できるなら、
            // 現時点で Suffix Tree に x の影響がないので C_data_ から x の方向に移動して終了
            if(findValidPath(C_data_,x)){
                if(lastUsedBranch!=nullptr)lastUsedBranch->SuffixLink = C_data_.RecentNode; 
                walkdown(C_data_,x);
                break;
            }

            // Branch を分岐点として、S の接尾辞 {C_data_ + x} を Suffix Tree に挿入する
            Node* Branch = C_data_.RecentNode;

            if(C_data_.NextNode != nullptr){// 辺の途中に分岐点を新たに作成する場合
                int middle = C_data_.NextNode->cleft() + C_data_.place;
                Branch = new Node({C_data_.NextNode->cleft() , new int(middle-1) } ,vertex_id++ , C_data_.RecentNode , root);
                // 辺の上に新たな Branch ができたことによる辺の張り替えなど
                C_data_.RecentNode->child[ S[C_data_.NextNode->cleft()-1]] = Branch;
                Branch->parent = C_data_.RecentNode;
                Branch->path_len_sub = C_data_.get_path_length();
                C_data_.NextNode->cover.first = middle;
                Branch->child[ S[C_data_.NextNode->cleft() - 1 ] ] = C_data_.NextNode;
                C_data_.NextNode->parent = Branch;
            }
            
            Node* NewLeaf = new Node( { S_length_Symbol , &S_length_Symbol} ,vertex_id++ , Branch , root);
            NewLeaf->suffix_id = S_length_Symbol - C_data_.get_path_length();
            Branch->child[x] = NewLeaf;

            if(lastUsedBranch!=nullptr)lastUsedBranch->SuffixLink = Branch;// 使った分岐点と leaf に Suffix Link を張る
            if(lastCreatedLeafNode != nullptr)lastCreatedLeafNode->SuffixLink = NewLeaf;
            
            lastUsedBranch = Branch;
            lastCreatedLeafNode = NewLeaf;

            // 接尾辞 {C_data+x} の挿入の次は、{C_data+x} の先頭の文字を消した接尾辞を挿入するために、
            // Suffix Tree 上の地点 C_data_ から Suffix Link を使用して C_data_ の先頭を削除した地点に移動する
        }while(C_data_.walkSuffixLink());
    }

    // {接尾辞 → ノード} の対応を全て決定する。  
    // これによって、それぞれの接尾辞に対応するノードが Suffix Tree に存在するようになる
    void determine_sub(){
        if(_determined)return ;// すでに完了している場合
        _determined = true;

        // 接尾辞に対応するノードから、追加された順に Suffix Link をつなげる
        Node* lastUsedBranch = lastCreatedLeafNode;

        do{
            Node* Branch = C_data_.RecentNode;
            if(C_data_.NextNode != nullptr){// 辺の途中に分岐点を新たに作成する場合
                int middle = C_data_.NextNode->cleft() + C_data_.place;
                Branch = new Node({C_data_.NextNode->cleft() , new int(middle-1) } ,vertex_id++ , C_data_.RecentNode , root);
                // 辺の上に新たな Branch ができたことによる辺の張り替えなど
                C_data_.RecentNode->child[ S[C_data_.NextNode->cleft()-1]] = Branch;
                Branch->parent = C_data_.RecentNode;
                Branch->path_len_sub = C_data_.get_path_length();
                C_data_.NextNode->cover.first = middle;
                Branch->child[ S[C_data_.NextNode->cleft() - 1 ] ] = C_data_.NextNode;
                C_data_.NextNode->parent = Branch;
            }
            Branch->suffix_id = S_length_Symbol + 1 - C_data_.get_path_length();
            if(lastUsedBranch!=nullptr)lastUsedBranch->SuffixLink = Branch;// Suffix Link を張る
            lastUsedBranch = Branch;
        }while(C_data_.walkSuffixLink());
    }


    // 外部向け機能
    public:// findValidPath と walkdown も public で使えるべき


    SuffixTree(){}
    ~SuffixTree(){}


    // 地点:data から要素 x に向かって進むことができるかを判定
    bool findValidPath(PositionData& data , T x){
        if(data.NextNode == nullptr )return (data.RecentNode->child[x] != nullptr);// ノードの上にいる場合は、x 方向に辺があるかを確かめる
        else return (S[data.NextNode->cleft() + data.place - 1] == x);// 辺の上にいるときは、次の文字を確かめる
    }


    // 地点:data から x の方向に 1 進み、data を更新する
    bool walkdown(PositionData& data , T x){
        if(findValidPath(data,x) == false)return false;//進めない
        Node* branch = data.NextNode;
        //分岐点にいる場合
        if(data.NextNode == nullptr)data.NextNode = data.RecentNode->child[x];
        data.place++;//進む
        // 分岐点に到達した場合、RecentNode が分岐点を表す様にフォーマットする        
        if(data.NextNode != nullptr && data.place == data.NextNode->cover_length()){
            data.RecentNode = data.NextNode;
            data.NextNode = nullptr;
            data.place = 0;
        }
        // 地点に対応する列を更新
        data.refer.push_back(x);
        return true;
    }




    // V_ をオンラインで追加する
    void push(vector<T> V_){
        for(T x : V_)push_sub(x);
    }

    // string も vector にキャスト
    void push(string V_){
        for(T x : V_)push_sub(x);
    }

    // 文字 c をオンライン構築で追加する
    void push(T c){push_sub(c);}


    // オンライン構築を終了する
    void determine(){determine_sub();}

    // 列 V_ をもとにオフライン構築
    template<class VecOrStr>
    void build(VecOrStr S_){
        reset();
        push(S_);
        determine();
    }


    // 列 X が、このクラスが保持する列 S に含まれるかどうか
    template<class VecOrStr>
    bool find(const VecOrStr& X){
        bool res = true;
        PositionData p_data = from_root();
        for(T x : X)if(!walkdown(p_data,x))return false;
        return true;
    }


    // 列 X が、このクラスが保持する列 S にいくつ含まれるか
    template<class VecOrStr>
    int count(const VecOrStr& X){
        init_tree_dp();
        PositionData p_data = from_root();
        for(T x : X)if(!walkdown(p_data,x))return 0;
        if(p_data.NextNode == nullptr)return p_data.RecentNode->SubTreeEndPointCount;
        return p_data.NextNode->SubTreeEndPointCount;
    }

    // 列 X に対して、Suffix Tree 上を移動できるだけ移動し、最終的な地点のデータを返す
    template<class VecOrStr>
    PositionData search(const VecOrStr& X){
        PositionData p_data = from_root();
        for(T x : X)if(!walkdown(p_data,x))break;
        return p_data;
    }




    /*
        内蔵している典型問題たち (Suffix Tree のメンバを使用するものたち)
    */


    /*
        このクラスが保持する列 : S のsuffix array を返す(1 - index)
        空列は辞書順最小の接尾辞とし、その suffix_id を |S|+1 とする
    */
    vector<int> suffix_array(){
        vector<Node*> s = dfs(true);
        vector<int> res;
        for(Node* now : s)if(now->isEndPoint())res.push_back(now->suffix_id);
        return res;
    }


    /*
        列 S の 辞書順で K 番目の連続部分列
        ただし、返り値は vector<T> なのに注意
        K は 1-index
    */
    vector<T> get_Kth_subSeq(long long K){
        Node* now = root;
        long long acum = 0;
        vector<T> res(0);
        init_tree_dp();

        if(root == nullptr)return res;
        if(K > root->SubTreeKindOfPrefix)return res;//そもそも連続部分列の種類数が K に満たない場合

        while(K > acum){
            vector<pair<T,Node*>>cand;
            for(pair<T , Node*> pr : now->child)if(pr.second!=nullptr)cand.push_back(pr);
            sort(cand.begin(),cand.end(),cmp_);//辺の最初の文字の昇順でソート

            for(pair<T ,Node*> pr : cand){
                
                Node* nx = pr.second;
                int nx_edge_num = nx->cover_length();//次の頂点への辺上のprefixの種類数
                if(nx_edge_num == 0)continue;
                //この辺の先に答えがある場合
                if(nx->SubTreeKindOfPrefix + nx_edge_num + acum >= K){
                    if(acum + nx_edge_num >= K){//答えとなる文字列が、次の頂点への辺の上にあるとき
                        for(int i = nx->cleft() - 1 ; i < nx->cleft() - 1 + K - acum ; i++ )res.push_back(S[i]);
                        return res;
                    }else{
                        for(int i = nx->cleft() - 1 ; i < nx->cright() ; i++ )res.push_back(S[i]);
                        now = nx;//辺の先に移動
                        acum += nx_edge_num;
                        break;//さっきいたノードからは抜ける
                    }
                }else acum += nx->SubTreeKindOfPrefix + nx_edge_num;//この辺の先に答えはないので、次の辺を見る
            }
        }
        assert(0);
        return res;
    }



    // LCA 周りは少し怖い

    // 頂点番号で普通にLCA
    int LCA(int u , int v){
        init_LCA();
        init_data();
        if(tree_depth[u] > tree_depth[v])swap(u,v);
        for(int k = int(parent.size())-1 ; k >= 0 ; k--){
            if(parent[k][v] >= 0 && tree_depth[parent[k][v]]>=tree_depth[u])v = parent[k][v];
        }
        if(u == v)return u;
        for(int k = int(parent.size())-1 ; k >= 0 ; k--){
            if(parent[k][u] != parent[k][v]){
                u = parent[k][u];
                v = parent[k][v];
            }
        }
        return parent[0][u];
    }


    // 頂点 u と v の LCA (u,vは頂点のポインタ)、返り値もポインタ
    Node* LCA(Node* u , Node* v){return Nodes[LCA(u->id , v->id)];}

    // 地点データ sd と、頂点 v の LCA 。LCA が 辺の途中になる可能性もあるので PositionData で返す 
    PositionData LCA(PositionData sd , Node* v){
        // 地点 sd が根から v までのパス上にある場合
        if(sd.NextNode!=nullptr && LCA(sd.NextNode , v) == sd.NextNode)return sd;    
        
        PositionData res = from_root();
        res.RecentNode = LCA(sd.RecentNode , v);
        return res;
    }

    // 地点データ sd と、頂点 v の LCA 。LCA が 辺の途中になる可能性もあるので PositionData で返す 
    PositionData LCA(Node* v , PositionData sd){return LCA(sd,v);}


    // 地点データ sd と、地点データ sp の LCA 。LCA が 辺の途中になる可能性もあるので PositionData で返す 
    PositionData LCA(PositionData sd , PositionData sp){
        PositionData res = from_root();
        if(sd.NextNode != nullptr && sp.NextNode != nullptr){
            if(sd.NextNode == sp.NextNode && sd.RecentNode == sp.RecentNode){// 同じ辺の上の地点の場合
                if(sd.get_path_length() < sp.get_path_length())return sd;// 浅い方が答え
                else return sp;
            }else {
                // 同じ辺にいない場合、深い方は recentNode まで下げて良い
                if(sd.get_path_length() > sp.get_path_length())swap(sd,sp);// sp が深い方で標準化
                return LCA(sd,sp.RecentNode);
            }
        }else if(sd.NextNode == nullptr && sp.NextNode != nullptr)return LCA(sp , sd.RecentNode);
        else if(sp.NextNode == nullptr && sd.NextNode != nullptr)return LCA(sd , sp.RecentNode);
        else {
            res.RecentNode = LCA(sd.RecentNode , sp.RecentNode);
            return res;
        }
    }

    // root を表す PositionData(地点データ) を返す
    PositionData from_root(){
        PositionData res;
        res.RecentNode = root;
        res.NextNode = nullptr;
        res.place = 0;
        res.refer = {};
        return res;
    }

    // 頂点番号が v であるノードを返す
    Node* get_node(int v){
        init_data();
        return Nodes[v];
    }

    // i 番目の suffix の終端に対応するノードを返す(1-index)
    Node* get_suffix_node(int i){
        init_data();
        return SuffixNodes[i];
    }

    // 根ノードを返す
    Node* get_root(){return root;}

    // 頂点 v の深さ(根から辺を何本経由するか)
    int get_depth_as_tree(int v){
        init_data();
        return tree_depth[v];
    }

    // S[i] (0-index) を返す (read-only)
    const T operator [](int i) const {return S[i];}

    // S.size()
    int size(){
        int res = S.size();
        return res;
    }


    // 連続部分列の種類数
    long long KindOfSubSequence(){
        init_tree_dp();
        return root->SubTreeKindOfPrefix;
    }

    int tree_size(){return vertex_id;}

    // 木の辺を頂点番号のペアとして、辺集合(vector)を返す
    vector<pair<int,int>> edges(){
        vector<Node*> dfs_ = dfs(false);
        vector<pair<int,int>> res;
        for(Node* x :dfs_){
            if(x->parent==nullptr)continue;
            res.push_back({x->parent->id , x->id});
        }
        return res;
    }


    // dfs 順の Node* を返す (辞書順に dfs するかを、引数で指定)
    vector<Node*> dfs(bool lexicographical = false){
        vector<Node*> res;
        if(root == nullptr)return res;
        stack<Node*> s;
        s.push(root);
        while(!s.empty()){
            Node* now = s.top();s.pop();
            res.push_back(now);
            vector<pair<char , Node*> > nxt;
            for(pair<char , Node*> nx : now->child )if(nx.second != nullptr)nxt.push_back(nx);
            if(lexicographical)sort(nxt.rbegin() , nxt.rend() , cmp_);//辺の最初の文字でソート
            for(pair<char , Node*> nx : nxt )s.push(nx.second);
        }
        return res;
    }

    void debug_dfs(){
        vector<Node*> d = dfs();
        vector<pair<int,int> > links;
        for(Node* now : d){
            if(now != root)cerr << now->parent->id << " → " << now->id << endl;
            if(now->SuffixLink != root && now->SuffixLink != nullptr)links.emplace_back(now->id , now->SuffixLink->id);
            if(now != root)cerr << "cover : " << now->cleft() << " " << now->cright() << endl;
        }
        for(pair<int,int>pr : links)cerr << "suffix link : " << pr.first << " - " << pr.second << endl;
    }


};








void sa(){
    string s;cin >> s;SuffixTree T;T.build(s);
    vector<int> sa = T.suffix_array();
    sa.erase(sa.begin());
    for(int x : sa)cout << x-1 << " ";
    cout << endl;
}




// https://onlinejudge.u-aizu.ac.jp/problems/ALDS1_14_A
void lca_test(){
    string s ,t;cin >> s >> t;
    SuffixTree S;
    S.push(s);
    S.determine();
    if(!S.find(t))return;
    auto t_data = S.search(t);
    REP(i,s.size()){
        auto leaf = S.get_suffix_node(i+1);
        auto lca = S.LCA(leaf , t_data);
        if(lca == t_data)cout << i << endl;
    }
}



// s , t の共通の部分文字列の個数(unique : 区間が異なるものを重複して数えるかどうか)
void common_substr(bool unique = true){
    string s , t;
    cin >> s >> t;
    SuffixTree T;

    // s + ? + t + ! を入れる
    T.push(s);
    T.push(INF);
    T.push(t);
    T.determine();

    // 数え方(uniqueによる) 
    function<int(int,int)> op = [&](int a , int b){
        if(unique)return a|b;
        else return a+b;
    };

    vector<int> dp_s(T.tree_size() , 0);// [x] := 頂点 x の部分木に s の leaf が幾つ存在するか(or 単に存在するかどうか)
    vector<int> dp_t(T.tree_size() , 0);// [x] := 頂点 x の部分木に t の leaf が幾つ存在するか(or 単に存在するかどうか)
    auto dfs = T.dfs(); reverse(dfs.begin() , dfs.end());

    for(auto now : dfs){
        if(now->isEndPoint()){
            if(now->suffix_id == s.size() + 1)continue;
            if(now->suffix_id == s.size() + t.size() + 2)continue;

            if(now->suffix_id <= s.size())dp_s[now->id] = 1;
            else dp_t[now->id] = 1;
        }
        if(now->parent != nullptr)dp_s[now->parent->id] = op(dp_s[now->parent->id], dp_s[now->id]);
        if(now->parent != nullptr)dp_t[now->parent->id] = op(dp_t[now->parent->id], dp_t[now->id]);
    }
    ll ans = 0;
    REP(x,T.tree_size()){
        auto now = T.get_node(x);
        // 部分木の葉のうち、(s , t) の組み合わせを考える
        if(!now->isEndPoint())ans += now->cover_length()*dp_s[now->id]*dp_t[now->id];
    }
    cout << ans << endl;
}



/* 
    列 t を受け取り、S (Suffix Tree の構成元の文字列) に対して、
    ms[i] := t の i 文字目から始まる部分文字列と、S の任意の箇所の部分文字列の最長接頭辞の長さ
    を計算し、返す(1-index)
    構築時みたいに、行き詰まったら Suffix Link で先頭の文字を削除した地点に移動する
*/
vector<int> matching_statistics(string s , string t){
    SuffixTree<char> T;
    vector<int> ms(t.size() + 1 , 0);// 1-index とする

    T.push(s); 

    auto now = T.from_root();
    int r = 0;
    for(int i = 1 ; i <= t.size() ; i++){
        r = max(i-1,r);
        while(r < t.size() && T.walkdown(now,t[r]))r++;
        
        ms[i] = now.get_path_length();
        now.walkSuffixLink();// t の次の接尾辞に移行
    }
    return ms;
}







