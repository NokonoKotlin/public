
#include "./../template/template.hpp"


/*

*/
template<typename T = long long>
class SuffixTree{
    private:

    struct Node{
        public:
        unordered_map<T , Node*> child;//[ x ] :=  要素 x で続く辺の先のノード
        Node* parent = nullptr;//親ノード( nullptr <=> このノードがroot )
        Node* SuffixLink = nullptr;// 「このノードに対応する列」の「先頭を削除した列に対応するノード」へリンクする

        Node(pair<int,int*> cover , int id_ = -1 , Node* parent_ = nullptr , Node* SuffixLink_ = nullptr)
            :cover(cover),id(id_),parent(parent_),SuffixLink(SuffixLink_){}

        int path_len_sub = 0; // このノードに対応する列の長さ (葉ノードについては不定なので、内部ノードにのみ定義)
        pair<int ,int*> cover = make_pair(-1,nullptr);// 自身に入る辺が、S の半開区間 [first , &second) に対応

        // 辺に書かれた文字列の半開区間の左端と右端 (0-index)
        int cleft(){return cover.first;}
        int cright(){return (*cover.second);}
        // 入る辺の長さ
        int cover_length(){
            if(this->parent == nullptr)return 0;// root の場合は 0 とする
            return cright() - cleft();
        }
        //根からノードまでの辺の文字数 path_len_sub を参照して再計算
        int get_path_length(){
            if(this->parent == nullptr)return 0;
            return this->parent->path_len_sub + cover_length();
        }   

        int id = -1;//id(頂点番号 ... 追加された順に割り振る)

        // このノードが、いずれかの接尾辞に対応しているかどうか
        bool isEndPoint(){return (this->suffix_id >= 0);}
        int suffix_id = -1; // isEndPoint()なら、何文字目から始まるものか(0-index)。そうでないなら -1。
        
        // 木 DP (専用の初期化関数で初期化)
        int SubTreeEndPointCount = 0;      // 自分以降の部分木が持つ EndPoint の個数
        long long SubTreeKindOfPrefix = 0; // このノードから移動して得られる文字列の種類数 (空文字列を含まない)
    };



    /*
        頂点 RecentNode から、NextNode の方向に place 進んだ地点を指すデータ。  
        ちょうど 頂点 x の上である場合、RecentNode = x , NextNode = nullptr , place = 0 で統一する
        この地点に対応する列として、refer を持つ。
        refering = false を指定すると、refer を非明示的にして定数サイズオブジェクトとして扱える
    */
    struct PositionData{
        Node* RecentNode = nullptr;//すでに到達した直近のノード
        Node* NextNode = nullptr;//次に向かうノード(今いる辺の先)
        int place = 0;// 辺の先頭から何文字目の箇所か

        // 根から現在地までのパス上の文字数(距離)
        int get_path_length(){return this->RecentNode->get_path_length() + place;}
        bool is_root(){return bool(this->get_path_length() == 0);}

        // この地点と対応する列を明示的に管理するかどうか
        bool refering = true;
        deque<T> refer;// この地点に対応する列 (pop_front するので deque で管理)
        
        // 現在地(refer) から、refer の先頭を削除した地点に移動。(明示的に参照 (refer) を管理する場合)
        bool walkSuffixLink(){
            assert(refering);
            assert(refer.size() == this->get_path_length());
            if(this->is_root())return false; // これ以上 Suffix Link を辿れないなら false
            refer.pop_front();// 先頭の文字を削除する

            if(this->RecentNode == this->RecentNode->SuffixLink)this->place--;// root → root の移動は辺上の位置が変わる
            this->RecentNode = this->RecentNode->SuffixLink;// Suffix Link を辿って移動する

            // 分岐点についたら終了して OK (place で判定する必要性に注意)
            if(this->place == 0){this->NextNode = nullptr;return true;}

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

        // refer が非明示な状況で Suffix Link を辿って移動する場合。
        // PositionData は何らかの列の連続部分列なので、
        // refer の代わりに列 S_ と、S_ のどこから始まる部分文字列かを表す整数を引数に取る
        template<class VecOrStr>
        bool walkSuffixLink(const VecOrStr& S_ , int start_index){
            refering = false;
            refer.clear();
            if(this->is_root())return false; 
            start_index += 1;
            if(this->RecentNode == this->RecentNode->SuffixLink)this->place--;
            this->RecentNode = this->RecentNode->SuffixLink;
            if(this->place == 0){this->NextNode = nullptr;return true;}
            this->NextNode = this->RecentNode->child[S_[start_index + this->RecentNode->get_path_length()]];
            while(this->NextNode != nullptr && this->NextNode->cover_length() <= this->place){
                this->place -= this->NextNode->cover_length(); 
                this->RecentNode = this->NextNode;
                if(this->place == 0)this->NextNode = nullptr;
                else this->NextNode = this->RecentNode->child[S_[start_index + this->RecentNode->get_path_length()]];
            }
            return true;
        }

        // 地点が同じ地点かどうか (refer を参照しないので O(1) )
        friend bool operator ==(PositionData& a,  PositionData& b){return (a.RecentNode == b.RecentNode && a.NextNode == b.NextNode && a.place == b.place);}

        void deb(){
            cerr<< "now on : " << RecentNode->id<< " . " << place << " th place ";
            if(NextNode!=nullptr)cerr <<  " → " << NextNode->id;
            cerr << endl << "length : " << get_path_length() << endl;
        }
        
    };


    // 要素比較用 (suffix array や、辞書順で k 番目の連続部分列の計算に使用)
    static bool cmp_(pair<char,Node*> a ,pair<char ,Node*> b){return a.first<b.first;};

    // メンバたち
    // これらは内部関数でのみ編集される
    int vertex_id = 0;// ノードができた順に id を 0 から割り振る(現時点の Suffix Tree の頂点数でもある)
    vector<T> S;// Suffix Tree 構築元の列
    int S_end_Symbol = 0;// S の終端を表すためのシンボル (葉ノードに入る辺の右端 (int*) に参照される)
    Node* root = nullptr;// 根を表すデータ(Node* のリンクで木を表現)
    Node* lastCreatedLeafNode = nullptr;// 最後に追加された葉ノード

    // ノードを追加する際に、共有する部分の(端点の)地点データ
    PositionData C_data_;
    // 重複して初期化を行わないためのフラグなど
    bool _determined = false;// 構築が完全終了したか (それぞれの接尾辞に対応するノードが存在するか)
    bool initialized_basic_data = false;//基本的なデータを初期化ずみか
    bool initialized_tree_dp = false;//Suffix Tree の部分木の情報を 木DP で初期化したかどうか
    bool initialized_LCA = false;// LCA の索引について、初期化したかどうか
    // 木構造のデータにアクセスするための配列
    vector<Node*> Nodes;//     [x] := 頂点番号 x のノードへのポインタ
    vector<Node*> SuffixNodes;// [x] := x 文字目から始まる Suffix に対応するノードへのポインタ(0-index)
    vector<int> tree_depth;// [x] := 頂点 x まで辺が何本か
    vector<vector<int> > parent; // [i][u] := 頂点 u の 2^i 個上の頂点



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
        parent.resize(K,vector<int>(tree_size(),-1));// デフォルトは -1 にしておく
        vector<Node*> s = dfs();
        
        for(Node* now : s)if(now->parent != nullptr)parent[0][now->id] = now->parent->id;
        for(int x = 1 ; x < K ; x++){
            for(int u = 0 ; u < parent[x].size(); u++){
                if(parent[x-1][u] >= 0)parent[x][u] = parent[x-1][parent[x-1][u]];
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

        C_data_ = from_root(true);// 地点データは refer を明示的に持つ
    }

    // x をオンラインで S の末尾に連結する 
    void push_sub(T x){
        if(root == nullptr || S.size() == 0)reset();//初期化
        assert(!_determined && "構築を完了した Suffix Tree を更新してはいけない" != "");
        
        S.push_back(x); 
        S_end_Symbol++;// x を追加したので、S の長さが 1 増える
        Node* lastUsedBranch = nullptr;// 最後に追加された分岐点
        do{
            if(findValidPath(C_data_,x)){
                // 現時点の Suffix Tree で C_data_ → x に移動できるなら、x の影響がないので C_data_ から x の方向に移動して終了
                if(lastUsedBranch!=nullptr)lastUsedBranch->SuffixLink = C_data_.RecentNode; 
                walkdown(C_data_,x);
                break;
            }

            // Branch を分岐点として S の接尾辞 {C_data_ + x} を Suffix Tree に挿入する
            Node* Branch = C_data_.RecentNode;
            if(C_data_.NextNode != nullptr){// 辺の途中に分岐点を新たに作成する場合
                int middle = C_data_.NextNode->cleft() + C_data_.place;
                Branch = new Node({C_data_.NextNode->cleft() , new int(middle) } ,vertex_id++ , C_data_.RecentNode , root);
                // 辺の上に新たな Branch ができたことによる辺の張り替えなど
                C_data_.RecentNode->child[ S[C_data_.NextNode->cleft()]] = Branch;
                Branch->parent = C_data_.RecentNode;
                Branch->path_len_sub = C_data_.get_path_length();
                C_data_.NextNode->cover.first = middle;
                Branch->child[ S[C_data_.NextNode->cleft()] ] = C_data_.NextNode;
                C_data_.NextNode->parent = Branch;
            }
            
            // はみ出た部分 (1 文字分) を Leaf Node として追加する
            Node* NewLeaf = new Node( { S_end_Symbol-1 , &S_end_Symbol} ,vertex_id++ , Branch , root);
            NewLeaf->suffix_id = NewLeaf->cright() - C_data_.get_path_length() - 1;
            Branch->child[x] = NewLeaf;

            if(lastUsedBranch!=nullptr)lastUsedBranch->SuffixLink = Branch;// 使った分岐点と leaf に Suffix Link を張る
            if(lastCreatedLeafNode != nullptr)lastCreatedLeafNode->SuffixLink = NewLeaf;
            
            lastUsedBranch = Branch;
            lastCreatedLeafNode = NewLeaf;

            // 接尾辞 {C_data+x} の挿入の次は、{C_data+x} の先頭の文字を消した接尾辞を挿入するために、
            //  C_data_ から Suffix Link を使用して C_data_ の先頭を削除した地点に移動する
        }while(C_data_.walkSuffixLink());
    }

    // {接尾辞 → ノード} の対応を全て決定する。  
    // これによって、それぞれの接尾辞に対応するノードが Suffix Tree に存在するようになる
    void determine_sub(){
        if(_determined)return ;// すでに完了している場合
        _determined = true;
        do{
            Node* Branch = C_data_.RecentNode;
            if(C_data_.NextNode != nullptr){// 辺の途中に分岐点を新たに作成する場合
                int middle = C_data_.NextNode->cleft() + C_data_.place;
                Branch = new Node({C_data_.NextNode->cleft() , new int(middle) } ,vertex_id++ , C_data_.RecentNode , root);
                C_data_.RecentNode->child[ S[C_data_.NextNode->cleft()]] = Branch;
                Branch->parent = C_data_.RecentNode;
                Branch->path_len_sub = C_data_.get_path_length();
                C_data_.NextNode->cover.first = middle;
                Branch->child[ S[C_data_.NextNode->cleft()] ] = C_data_.NextNode;
                C_data_.NextNode->parent = Branch;
            }
            Branch->suffix_id = S_end_Symbol - C_data_.get_path_length();
            if(lastCreatedLeafNode!=nullptr)lastCreatedLeafNode->SuffixLink = Branch;// Suffix Link を張る
            lastCreatedLeafNode = Branch;
        }while(C_data_.walkSuffixLink());
    }


    // 外部向け機能
    public:// findValidPath と walkdown も public で使えるべき

    SuffixTree(){}
    ~SuffixTree(){}


    // 地点:data から要素 x に向かって進むことができるかを判定
    bool findValidPath(PositionData& data , T x){
        if(data.NextNode == nullptr )return (data.RecentNode->child[x] != nullptr);// ノードの上にいる場合は、x 方向に辺があるかを確かめる
        else return (S[data.NextNode->cleft() + data.place] == x);// 辺の上にいるときは、次の文字を確かめる
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
        // 地点に対応する列が明示的なら、それを更新
        if(data.refering)data.refer.push_back(x);
        return true;
    }


    // V_ をオンラインで追加する
    void push(vector<T> V_){for(T x : V_)push_sub(x);}

    // string も vector にキャスト
    void push(string V_){for(T x : V_)push_sub(x);}

    // 文字 c をオンライン構築で追加する
    void push(T c){push_sub(c);}

    // オンライン構築を終了する
    void determine(){determine_sub();}

    // 列 V_ をもとにオフライン構築
    template<class VecOrStr>
    void build(VecOrStr S_){reset();push(S_);determine();}


    // 列 X が、このクラスが保持する列 S に含まれるかどうか
    template<class VecOrStr>
    bool find(const VecOrStr& X){
        bool res = true;
        PositionData p_data = from_root(true);
        for(T x : X)if(!walkdown(p_data,x))return false;
        return true;
    }


    // 列 X が、このクラスが保持する列 S にいくつ含まれるか
    template<class VecOrStr>
    int count(const VecOrStr& X){
        init_tree_dp();
        PositionData p_data = from_root(true);
        for(T x : X)if(!walkdown(p_data,x))return 0;
        if(p_data.NextNode == nullptr)return p_data.RecentNode->SubTreeEndPointCount;
        return p_data.NextNode->SubTreeEndPointCount;
    }

    // 列 X に対して、Suffix Tree 上を移動できるだけ移動し、最終的な地点のデータを返す
    template<class VecOrStr>
    PositionData search(const VecOrStr& X){
        PositionData p_data = from_root(true);
        for(T x : X)if(!walkdown(p_data,x))break;
        return p_data;
    }


    // Suffix Tree の元の文字列 S の [l,r) の部分文字列に対応する地点データを返す。
    // 部分文字列に対応する定数サイズオブジェクトを O(logN) 時間で取得できておいしい。
    // 返り値の地点データは、自身が表す文字列に関して非明示的 (refering が false)
    PositionData search(int l , int r ){
        // S の l 文字目から始まる接尾辞方向に (r-l) 文字進んだ地点が答え
        init_LCA();// parent の初期化
        PositionData p_data = from_root(false);// (root,nullptr,0,{}) のデータ
        Node* border = get_suffix_node(l);
        
        // ギリギリ (r-l) 文字以上まで親方向に戻る (ダブリング)
        for(int k = int(parent.size())-1; k >= 0 ; k--){
            int y = parent[k][border->id];
            if(y < 0)continue;
            if(get_node(y)->get_path_length() >= r-l)border = get_node(y);
        }
        // さらに 1 つ戻ることで、S[l,r) に到達できる直近のノードになる
        if(border->get_path_length() > (r-l)){
            border = border->parent;
            p_data.NextNode = border->child[S[l + border->get_path_length()]];
            p_data.place = (r-l) - border->get_path_length();
        }
        p_data.RecentNode = border;
        return p_data;
    }


    // 頂点 x の地点に対応する文字列を含む接尾辞の開始地点を返す (0-index)
    // 複数存在する場合はいずれか 1 つを返す
    int decode_suffix(Node* x){
        if(x == root)return S_end_Symbol;
        return x->cleft() - x->parent->get_path_length();
    }   
    // 地点 p_data に対応する文字列を含む接尾辞の開始地点を返す (0-index)
    int decode_suffix(PositionData& p_data){
        if(p_data.NextNode!=nullptr)return decode_suffix(p_data.NextNode);
        return decode_suffix(p_data.RecentNode);
    }

    /*
        内蔵している典型問題たち (Suffix Tree のメンバを使用するものたち)
    */
    /*
        このクラスが保持する列 : S のsuffix array を返す(0-index)
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
        K = 0 なら空の列を返す
    */
    vector<T> get_Kth_subSeq(long long K){
        Node* now = root;
        long long acum = 0;
        vector<T> res(0);
        init_tree_dp();

        if(K == 0)return res;
        assert(root != nullptr);
        assert(K <= root->SubTreeKindOfPrefix);

        while(K > acum){
            vector<pair<T,Node*>>cand;
            for(pair<T , Node*> pr : now->child)if(pr.second!=nullptr)cand.push_back(pr);
            sort(cand.begin(),cand.end(),cmp_);//辺の最初の文字の昇順でソート

            for(pair<T ,Node*> pr : cand){
                Node* nx = pr.second;
                int nx_edge_num = nx->cover_length();//次の頂点への辺上のprefixの種類数
                //辺の先に答えがある場合
                if(nx->SubTreeKindOfPrefix + nx_edge_num + acum >= K){
                    if(acum + nx_edge_num >= K){//答えとなる文字列が、次の頂点への辺の上にあるとき
                        for(int i = nx->cleft() ; i < nx->cleft() + K - acum ; i++ )res.push_back(S[i]);
                        return res;
                    }else{
                        for(int i = nx->cleft() ; i < nx->cright() ; i++ )res.push_back(S[i]);
                        now = nx;//辺の先に移動
                        acum += nx_edge_num;
                        break;//さっきいたノードからは抜ける
                    }
                }else{
                    acum += nx->SubTreeKindOfPrefix + nx_edge_num;//この辺の先に答えはないので、次の辺を見る
                } 
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
    // refering が false なら、対応する列が非明示的な地点データを作って返す
    PositionData from_root(bool refering = true){
        PositionData res;
        res.RecentNode = root;
        res.NextNode = nullptr;
        res.place = 0;
        res.refering = refering;
        res.refer = {};
        return res;
    }

    // 頂点番号が v であるノードを返す
    Node* get_node(int v){
        init_data();
        return Nodes[v];
    }

    // i 番目の suffix の終端に対応するノードを返す(0-index)
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
            if(now->SuffixLink != nullptr)links.emplace_back(now->id , now->SuffixLink->id);
            if(now != root)cerr << "cover : [" << now->cleft() << " " << now->cright() << ")" << endl;
        }
        for(pair<int,int>pr : links)cerr << "suffix link : " << pr.first << " - " << pr.second << endl;
    }


};

