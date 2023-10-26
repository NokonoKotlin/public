
//include
//------------------------------------------
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <stack>
#include <bitset>
#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cctype>
#include <string>
#include <cstring>
#include <ctime>
#include<queue>
#include<complex>
#include <cassert>
using namespace std;
typedef long long ll;












/*
    
    SuffixTree
    T 型の列 : S に対して Suffix Tree を構築 
    endChar := 終端文字を表す値。S に含まれない値でないといけない
    === クラスの外から利用する関数では、endChar はないものとして扱う (ex. get_Kth_Substr() など) ===

    頂点 0 が Suffix Tree の根 (root)
    各ノードの Suffix Link は必ず存在し、root で初期化されるものとする(root からも rootに)

    基本はvector の Suffix Tree ですが、外向きの機能は string / vector の両方に対応しておきたい
    (引数を template<class VecOrStr> にするなど)

    ===================ノードの番号は生成順(0から)。変数などは基本的に 1-index=================
    ===================ユーザ側の操作も、基本的に 1-index (suffix の左端や、辞書順 k 番目の部分文字列など)====================


    Suffix Link : ノード x が列 V に対応している時、V の先頭の文字を削除した列に対応するノード y に対して、 
                  x → y にリンクを張る . ただし、root は列外的に root → root にリンクを張る(全てのノードからリンクが出ている)
    

    読み込みと挿入を駆使して Suffix Tree を構築

    build(s_) : s_+endChar をもとに一気にSuffix Treeを構築する(オフライン構築)。
        Suffix Tree の構築上、S の末尾には endChar を勝手に追加してSuffixTreeを構築するが、
        基本的に S の末尾の endChar は無視する(=========空文字みたいな扱い=========)

    link(s_) : オンライン構築中の S に、s_ をくっつけて、それの Suffix Tree をオンラインで構築する。
        末尾に endChar を追加した後は、オンライン構築を行うことはできない( RE になる )
    ===========オンライン構築の Suffix Tree では、tree_dp 系の、全ての接尾辞の終端ノードを前提とする操作を行えない。=========
    
    end_build() : オンライン構築を終了し、オフラインにする
        S の末尾に endChar を追加する
        

    ==================必要に応じて以下のメソッドを呼び出して情報を計算する====================
    ==================標準の機能(LCAなど)では以下を勝手に呼び出してくれる(複数回呼び出すのも回避してくれる)(信用しすぎないで)====================

    init_data()//以下のデータを初期化
        Nodes[i] := 頂点番号 i のノードへのポインタ(終端文字を表すノードも++含む++)
        SuffixNodes[i] := S の i 番目以降の接尾辞に対応するノードへのポインタ( 空列や S 自身も接尾辞として考える 空列 := 終端文字 endChar である)
        tree_depth[i] := root から頂点 i まで、辺が何本か(Node->depthと混同しないよう注意)

    init_tree_dp() //各ノードについて、以下のデータを計算
        int SubTreeKindOfString := 自分以降の部分木のパスが持つ文字列の種類数。ただし、endChar は空文字列みたいな扱いなので、endChar を含む文字列は無視する
        int SubTreeBranchSum := 自分以降の部分木が持つ葉の個数

    init_LCA() //LCA 計算のためのデータを計算

    一度呼び出したら、その後は呼び出す必要がないので、呼び出し済みかどうかを管理するフラグを持つ(複数回呼ばれるのを防ぐ)

    find(t) := t が S の連続部分列かどうかを判定
    count(t) := t が S の連続部分列として何回現れるか計算
    search(t):= t を検索し、移動できた地点までのデータを返す
    LCA(地点のデータ 2 つ) := 地点(辺の途中含む)の LCA のデータを返す(引数がノード同士の場合に限ってノード)
    suffix_array() := S の SuffixArray を返す(1 - index)
    get_Kth_subStr(K) := S の連続部分列のうち、辞書順で K 番目のものをvector<T>で返す(Kは1-index)
    size() := S のサイズを返す(ただし、終端文字は無視する)

    
*/
template<typename T = long long, T endChar = T(-2e18)>
class SuffixTree{
    
    public:// Node と ProccesingData は public であるべき

    struct Node{

        int path_len_sub = 0;//根からこのノードまでの文字数 - 葉ノードの深さは不定なので、外からは get_path_len() でアクセスする
        
        //根からノードまでの辺の文字数 (葉ノードの場合、辺の右端が不定なので再計算して返す)
        int get_path_len(){
            if(this->suffix_id >= 1)return this->parent->get_path_len() + cover_len();
            return this->path_len_sub;
        }


        //このノードに ---入ってくる辺--- がどの連続部分列をカバーしてるか(閉区間 [l , r] を表す)
        // ただし、endSymbol はオンライン構築で変化するので、r は 整数へのポインタとする(leaf.cover.second は endSymbol を参照する)
        pair<int ,int*> cover = make_pair(-1,nullptr);

        // 辺に書かれた文字列の区間の左端と右端(1-index)
        int cleft(){return cover.first;}
        int cright(){return *cover.second;}
        int cover_len(){
            if(parent == nullptr)return 0;// root の場合
            return cright() - cleft() + 1;
        }

        int id = -1;//id(頂点番号 ... 追加された順に割り振る)

    
        unordered_map<T , Node*> child;//[ x ] :=  要素 x で続く辺の先のノード
        Node* parent = nullptr;//親ノード( nullptr <=> このノードがroot )

        Node* SuffixLink = nullptr;// 全てのノードの Suffix Link は root で初期化する
        
        Node(pair<int,int*> cover , int id_ = -1):cover(cover),id(id_){}

        // Leaf Node の場合、その葉が 何文字目から始まる suffix を表しているかを表す(1-index) (leaf node でない場合は -1 )
        int suffix_id = -1; // 構築中に代入するので、いつでも利用して良い

        bool isLeaf(){
            return this->suffix_id >= 1;
        }

        //木DP用のデータ(専用の初期化関数ないで初期化)
        int SubTreeBranchSum = 0;//自分以降の部分木が持つ葉の個数
        long long SubTreeKindOfString = 0;//自分以降の部分木のパスが持つ文字列の種類数

    };



    /*
        Suffix Tree 上の地点を表すデータ(辺の途中の場合もある) : 
        探索時や構築時に 列を読み込んで移動した地点を表すとき、見るべきノードや辺のデータなどを管理する
        構築時**など**に使うデータは、読み込む列への参照(refer)を持つ

        ProcessingData の中身が矛盾しないことを保証するプログラムにしなければならない(適宜 format() で整形)
        ちょうど 頂点 x の上である場合、RecentNode = x , nextNode = nullptr で統一する
    */
    struct ProcessingData{
        Node* RecentNode = nullptr;//すでに到達したノードで、直近のもの(これまで到達した中で、今いる地点に一番近いノード)
        Node* nextNode = nullptr;//次に向かうノード
        int path_length = 0;// 現在地点の深さ(パスに書かれた文字数)
        // refer の何文字目から読み込んだか(1-index)
        int start_index = 1; // Suffix Link を辿るタイミングで +1 される(次の接尾辞に移るから)

        // 今いる辺の先頭の文字 = nextNode->cleft() から計算できる

        // 地点が同じ地点かどうか(地点データが format() 済みである必要がある!!!!)
        bool operator ==(ProcessingData& b){
            return (this->RecentNode == b.RecentNode && this->nextNode == b.nextNode && this->path_length == b.path_length);
        }


        /*
            この地点データ が Suffix Link を移動して、次の接尾辞のデータに移る 
            root からは例外的に root にリンクされている : (recentNode はそのままで、先頭の文字を削除した方向に進む)
            成功したかどうかを bool で返す (失敗 = data が root を指す)
            失敗した場合は start_index++ などが起こらないので、注意してください
            refer を参照して移動する (refer = processingData が読み込んでいる文字列)
        */
        template<class VecOrStr>
        bool walkSuffixLink(VecOrStr& refer){
            if(this->path_length == 0)return false; // now が root の場合、移動しようがないので ng
            this->path_length--;// 先頭の文字が削除されるので
            this->start_index++;// 次の接尾辞のデータになる
            this->RecentNode = this->RecentNode->SuffixLink;

            /*
                サフィックスリンクを辿って移動した時、
                今見ている地点 が Recentnode と nextNode の間にくるように整形
                refer を参照して、正しい地点まで移動する
            */

            int lef = this->start_index + this->RecentNode->get_path_len(); // 次に向かうべき方向に対応する refer のindex (1-index)

            if(this->path_length == this->RecentNode->get_path_len())this->nextNode = nullptr;
            else this->nextNode = this->RecentNode->child[refer[lef-1]];
            
            // 地点が Recentnode と nextNode の間にくるように整形
            while(this->nextNode != nullptr && this->nextNode->get_path_len() < this->path_length){
                lef += this->nextNode->cover_len(); 
                this->RecentNode = this->nextNode;
                this->nextNode = this->RecentNode->child[refer[lef-1]];
            }
            // 分岐点にいる場合、RecentNode が分岐点を表す様にフォーマットする 
            if(this->nextNode != nullptr && this->path_length == this->nextNode->get_path_len())this->RecentNode = this->nextNode;
            // ちょうど RecentNode にいる場合、次に向かう地点はない
            if(this->path_length == this->RecentNode->get_path_len())this->nextNode = nullptr;

            return true;
        }


    };



    // data に対して、向かっている辺の何番目の要素を指しているのかを返す( format() 前は辺の長さに収まるとは限らない)
    int pos_on_edge(ProcessingData& data){
        return data.path_length - data.RecentNode->get_path_len();
    }





    //文字比較用
    static bool cmp_char(pair<char,Node*> a ,pair<char ,Node*> b){
        if(a.first == endChar)return true;
        else if(b.first == endChar)return false;
        else return a.first<b.first;
    };



    private:
    /*
        ==================================
        内部実装用の関数
        ==================================
    */



    /*
        メンバのメモリ解放
        nd を根として構成する suffix tree のメモリを解放
    */
    void release(Node* nd){
        if(nd == nullptr)return;
        //nd から幅優先探索する (通ったnodeはdelete)
        queue<Node*> q;
        q.push(nd);
        while(!q.empty()){
            Node* now = q.front();
            q.pop();
            for(pair<T , Node*> p : now->child){
                q.push(p.second);
            }
            delete now->cover.second;
            delete now;
        }
        nd = nullptr;
    }

    


    // ノードができた順に id を 0 から割り振る
    int vertex_id = 0;// Suffix Tree の頂点数でもある

    //構築元の列(Tree構築時以外で編集不可)
    vector<T> S;

    //辺に書かれた区間の右端が、終端を表すためのシンボル
    int endSymbol = 0;// 葉ノードに入る辺の右端 (int*) に参照される
    

    /*
        グローバルな変数たち
    */


    // public ではあるものの、編集は絶対ダメ！！！！

    //Node* のコピーを取ると child などの重たいメンバもコピーしてしまうので、Suffix Tree が構築できてからは頂点は頂点番号で扱いたい
    vector<Node*> Nodes;//     [x] := 頂点番号 x のノードへのポインタ
    vector<Node*> SuffixNodes;// [x] := S の x 文字目から始まる Suffix に対応する 葉ノードへのポインタ(1-index)
    vector<int> tree_depth;// [x] := 頂点 x までの深さ(rootから辺が何本か)。Node->get_path_len() と混同しないように!!
    vector<vector<int> > parent; // LCA でのダブリング . [i][u] := 頂点 u の 2^i 個上の頂点
    
    Node* root = nullptr;//Suffix Tree の根を表すデータ(Node* のリンクで木を表現)

    Node* lastCreatedLeafNode = nullptr;// 最後に追加された葉ノード(葉のSuffix Link を構築する用)


    //構築時に参照するデータ(Tree構築時以外で編集不可)
    ProcessingData construction_data;// オンライン構築では、前のオンライン構築データを利用してオンライン構築する

    //必要に応じて計算するかどうかを決めるので、計算済みかどうかを管理して複数回計算してしまうのを回避
    bool initialized_basic_data = false;//基本的なデータを初期化ずみか

    //Suffix Tree について、部分木の情報を 木DP で初期化したかどうか
    bool initialized_tree_dp = false;

    // LCA の索引(ダブリング配列)について、初期化したかどうか
    bool initialized_LCA = false;



    

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
        release(root);//メモリ解放
        vertex_id=0;
        root = new Node({0,new int(0)}, vertex_id++);
        root->path_len_sub = 0;
        root->SuffixLink = root; // root からは root にリンクが出ている(ここだけ例外)
        root->parent = nullptr;

        construction_data.RecentNode = root;
        construction_data.path_length = 0;
        construction_data.nextNode = nullptr;
        construction_data.start_index = 1;// S におけるProcessingDataの開始地点
    }




    // 基本的なデータの初期化(ノードへのアクセス用の配列など)
    void init_data(){
        if(root == nullptr)return;
        if(initialized_basic_data)return ;
        initialized_basic_data = true;

        Nodes.resize(vertex_id , nullptr);
        tree_depth.resize(vertex_id , 0);
        SuffixNodes.resize(S.size()+1 , nullptr);
        stack<Node*> s;
        s.push(root);
        tree_depth[root->id] = 0;
        while(!s.empty()){
            Node* now = s.top();s.pop();

            Nodes[now->id] = now;//id と Node* を対応付け
            if(now->isLeaf())SuffixNodes[now->suffix_id] = now; // x 文字目から始まる Suffix に対応するノードを管理
            
            for(pair<T , Node*> pr : now->child)if(pr.second != nullptr){
                tree_depth[pr.second->id] = tree_depth[now->id] + 1;
                s.push(pr.second);
            }
        }
    }




    /*
        あるノード以降の部分木に関するデータを計算
        SubTreeKindOfString := 部分木のパスが表す文字列の種類数
        SubTreeBranchSum    := 部分木の葉の個数
        initialized_tree_dp が false なら、未計算
    */
    void init_tree_dp(){
        if(root == nullptr)return;
        if(initialized_tree_dp)return;//計算済みなので
        
        initialized_tree_dp = true;

        stack<Node*> s;
        stack<Node*> s_rev;
        s.push(root);
        
        while(!s.empty()){//根からdfsして全ての頂点を探索
            Node* now = s.top(); s.pop();
            if(now == nullptr)continue;
            s_rev.push(now);
            if(now->cright() == endSymbol)now->SubTreeBranchSum = 1;//自身が葉ノードなら自身をカウント
            else now->SubTreeBranchSum = 0;
            now->SubTreeKindOfString = 0;
            for(pair<T , Node*> pr : now->child)if(pr.second != nullptr)s.push(pr.second);
        }

        
        while(!s_rev.empty()){//dfsの逆順に探索して木DP
            Node* now = s_rev.top(); s_rev.pop();
            if(now == nullptr)continue;
            if(now->parent == nullptr)continue;

            //親ノードに自身以降の部分木をマージ
            now->parent->SubTreeBranchSum += now->SubTreeBranchSum;//部分木の葉の個数
            now->parent->SubTreeKindOfString += now->SubTreeKindOfString;//部分木のパスが表す文字列の種類数

            now->parent->SubTreeKindOfString += now->cover_len();
            if(now->isLeaf() && S.back() == endChar)now->parent->SubTreeKindOfString--; // 終端ノードの分は無視する
        }
    }


    // LCA 計算用の、parent 配列の初期化
    void init_LCA(){
        if(root == nullptr)return;
        if(initialized_LCA)return;//計算済みなので
        
        initialized_LCA = true;
        const int K = log2(vertex_id)+2;
        parent.resize(K,vector<int>(vertex_id,-1));

        stack<Node*> st;
        st.push(root);
        while(!st.empty()){
            Node* now = st.top();
            st.pop();
            
            for(pair<T,Node*> nx : now->child){
                if(nx.second == nullptr)continue;
                parent[0][nx.second->id] = now->id;
                st.push(nx.second);
            }
        }       
        
        for(int x = 1 ; x < K ; x++){
            for(int u = 0 ; u < parent[x].size(); u++){
                int mid = parent[x-1][u];
                if(mid >= 0)parent[x][u] = parent[x-1][mid];
                else parent[x][u] = -1;// 存在しない
            }
        }
    }



    
    // V_ をもとにオフライン構築(勝手に終端文字が追加される)
    void build_sub(vector<T> V_){
        reset();// データ初期化

        for(T x : V_){
            bool continue_read = read(x,construction_data);//読み込みパート
            if(!continue_read)insert(construction_data);//挿入パート
        }
        //終端文字を挿入
        read(endChar,construction_data);
        insert(construction_data);
    }

    // V_ をオンラインで S の末尾に連結する (終端文字が追加されない ... 代わりに機能が制限される)
    void link_sub(vector<T> V_){
        if(root == nullptr)reset();//初期化
        
        if(S.size() != 0)assert(S.back() != endChar); // endChar を追加した後はオンライン構築できない

        for(T x : V_){
            bool continue_read = read(x,construction_data);//読み込みパート
            if(!continue_read)insert(construction_data);//挿入パート
        }

        // 終端文字を追加せずに終了(これにより、次のオンライン構築が呼び出された時に construction_data を引き続き使える)
    }

    // オンライン構築を終了する
    void end_build_sub(){
        if(root == nullptr)return;
        else if(S.size() != 0 && S.back() == endChar)return;

        //終端文字を追加する
        read(endChar,construction_data);
        insert(construction_data);
    }


    // 次の文字 c を読み込む
    bool read(T c , ProcessingData& data){
        S.push_back(c); // 読み込んだ時点で、最新の文字を反映
        endSymbol++;// 読み込んだ分だけ終端を伸ばす
        return walkdown(c , data); // c の方向に移動できたら true を返す
    }


    // Processing なデータが data である場合の挿入パート
    void insert(ProcessingData& data){
        T x = S.back();
        Node* lastCreatedBranch = nullptr;
        while(true){// Processing なやつ(の接尾辞)を全部見る
            // 移動可能なら、挿入パートは終了(共有パスを伸ばせるから)
            bool end_insert = findValidPath(x,data);
            if(end_insert){ 
                // 途中で終了する時、現在地が必ず分岐点上であることが保証される
                if(lastCreatedBranch!=nullptr)lastCreatedBranch->SuffixLink = data.RecentNode; 
                walkdown(x,data);// 挿入前に読み込まれた文字の方向に進む
                return; // Processed になったら終了
            }

            Node* branch = add(data);
            if(lastCreatedBranch!=nullptr)lastCreatedBranch->SuffixLink = branch;
            lastCreatedBranch = branch;

            // suffix link を辿れた(次に挿入する processing な接尾辞が存在する) かどうか
            bool next_insertion = data.walkSuffixLink(S);// data は S を参照して Suffix Link を辿る

            if(next_insertion == false){
                // 全ての processing な接尾辞を読み込んだので、読み込み地点だけ次の読み込み位置にずらして挿入終了
                data.start_index++;
                return;
            }
        }
    }





    /*
        data が表す地点に新たに Processing な文字列を挿入する
        挿入する文字列が分岐する分岐点を返す
    */
    Node* add(ProcessingData& data){
        T x = S[data.start_index + data.path_length - 1];
        Node* res = nullptr; 
        Node* NewLeaf = nullptr; 
        Node* NewBranch = nullptr;

        if(data.nextNode == nullptr){//今いる地点が分岐点(branch)           // leaf に入る辺の右端は endChar を参照する
            NewLeaf = new Node( { data.start_index + data.path_length , &endSymbol} ,vertex_id++);
            NewLeaf->suffix_id = data.start_index;// suffix_id =  data の読み込み開始地点(1-index)

            data.RecentNode->child[x] = NewLeaf;
            NewLeaf->parent = data.RecentNode;
            
            res = data.RecentNode;// 分岐点
        }else{ //辺の途中で分岐したい場合、新たにinternal Nodeを作成
            NewBranch = new Node({data.nextNode->cleft() , new int(data.nextNode->cleft() +pos_on_edge(data) - 1) },vertex_id++);
            NewLeaf = new Node({data.start_index + data.path_length , &endSymbol},vertex_id++);
            NewLeaf->suffix_id = data.start_index;
            
            T edge_id_bef = S[data.nextNode->cleft()-1]; // 元々いた辺の先頭の文字 

            //元々向かってた先のノードのカバー範囲を調整
            data.nextNode->cover.first += pos_on_edge(data);
            //辺を分割して、ブランチを作成
            NewBranch->child[ S[data.nextNode->cleft() - 1 ] ] = data.nextNode;
            NewBranch->child[x] = NewLeaf;
            data.RecentNode->child[edge_id_bef] = NewBranch;

            NewBranch->parent = data.RecentNode;
            NewBranch->path_len_sub = data.RecentNode->get_path_len() + NewBranch->cover_len();
                   
            NewLeaf->parent = NewBranch;

            data.nextNode->parent = NewBranch;

            NewBranch->SuffixLink = root;

            res = NewBranch;// 分岐点
        }

        NewLeaf->SuffixLink = root;
        // ひとつ前の leaf から suffix link をつなげる
        if(lastCreatedLeafNode != nullptr)lastCreatedLeafNode->SuffixLink = NewLeaf;
        lastCreatedLeafNode = NewLeaf;// 新しく leaf を作ったので

        return res;
    }





    public:// findValidPath と walkdown と walkSuffixLink も public で使えるべき





    /*
        SuffixTree はグローバルな列 S を読み込み済みであるとする
         processing なデータを パラメータ : data で受け取った時、
        次に要素 x に向かって進むパスが存在するかどうかを判定する
    */
    bool findValidPath(T x , ProcessingData& data){
        if(data.nextNode == nullptr ){//分岐点にいる場合
            if(data.RecentNode->child[x] != nullptr)return true;//x に対応する辺が存在する
            else return false;
            //分岐点にいないとき(そのような場合、必ず nextNode が存在する)
        }else {//次の点も辺の内側にいる時
            if(S[data.nextNode->cleft() + pos_on_edge(data) - 1] == x)return true;//次の点が一致
            else return false;
        }

        return false;
    }




    /*
        SuffixTree はグローバルな列 S を読み込み済みであるとする

        今いる地点のデータを、パラメータ : data として受け取る。
        data をもとに、x の方向に 1 進む。
        data を更新する
    */
    bool walkdown( T x , ProcessingData& data ){
        if(findValidPath(x,data) == false)return false;//進めない
        Node* branch = data.nextNode;
        
        //分岐点にいる場合
        if(data.nextNode == nullptr)data.nextNode = data.RecentNode->child[x];

        data.path_length++;//進む
        
        // 分岐点に到達した場合、RecentNode が分岐点を表す様にフォーマットする        
        if(data.nextNode != nullptr && data.path_length == data.nextNode->get_path_len()){
            data.RecentNode = data.nextNode;
            data.nextNode = nullptr;
        }
        return true;
    }


    /*
        ===========================
        ===========================
        外部用の機能
        ===========================
        ===========================
    */

    // 列から文字列に
    string parse_Str(vector<T> &V_){
        string res = "";
        for(T x : V_)res += x;
        return res;
    }

    // 文字列から列に
    vector<T> parse_Vector(string &S_){
        vector<T> res;
        for(T x : S_)res.push_back(x);
        return res;
    }




    // 文字列 S_ をもとにオフライン構築(勝手に終端文字が追加される)
    void build(string S_){
        vector<T> vS_;
        for(char c : S_)vS_.push_back(T(c));//無理やりキャスト
        build_sub(vS_);
    }

    // 列 V_ をもとにオフライン構築(勝手に終端文字が追加される)
    void build(vector<T> S_){
        build_sub(S_);
    }




    // 文字列 S_ をオンラインで追加する(終端文字が追加されない ... 代わりに、find() しか使えない)
    void link(string S_){
        vector<T> vS_;
        for(char c : S_)vS_.push_back(T(c));//無理やりキャスト
        link_sub(vS_);
    }


    // V_ をオンラインで追加する(終端文字が追加されない ... 代わりに、find() しか使えない)
    void link(vector<T> V_){
        link_sub(V_);
    }

    // 文字 c をオンライン構築で追加する
    void push_back(T c){
        string b = "";
        b += c;
        link(b);
    }

    // オンライン構築を終了する
    void end_build(){
        end_build_sub();
    }


    // 列 X が、このクラスが保持する列 S に含まれるかどうか
    template<class VecOrStr>
    bool find(const VecOrStr& X){
        bool res = true;
        ProcessingData p_data;
        p_data.RecentNode = root;
        p_data.path_length = 0;
        p_data.nextNode = nullptr;
        p_data.start_index = 1;
        for(T x : X){
            if(findValidPath(x,p_data)){
                walkdown(x,p_data);
            }else{
                return false;
            }
        }
        return true;
    }


    // 列 X が、このクラスが保持する列 S にいくつ含まれるか
    template<class VecOrStr>
    int count(const VecOrStr& X){
        init_tree_dp();
        int res = 0;
        ProcessingData p_data;
        p_data.RecentNode = root;
        p_data.path_length = 0;
        p_data.nextNode = nullptr;
        p_data.start_index = 1;
        for(T x : X){
            if(findValidPath(x,p_data))walkdown(x,p_data);
            else return 0;
        }

        //今いる地点が RecentNodeちょうどの時や、nextNode ちょうどの時の注意が重要
        if(p_data.nextNode == nullptr)res = p_data.RecentNode->SubTreeBranchSum;
        else res = p_data.nextNode->SubTreeBranchSum;

        return res;
    }

    // 列 X に対して、Suffix Tree 上を移動できるだけ移動し、最終的な地点のデータを返す
    template<class VecOrStr>
    ProcessingData search(const VecOrStr& X){
        ProcessingData p_data;
        p_data.RecentNode = root;
        p_data.path_length = 0;
        p_data.nextNode = nullptr;
        p_data.start_index = 1;
        for(T x : X){
            if(findValidPath(x,p_data))walkdown(x,p_data);
            else break;
        }
        return p_data;
    }



    /*
        このクラスが保持する列 : S のsuffix array を返す
        1 - index で返す。
        終端文字の分(空列に対応)も含む
    */
    vector<int> suffix_array(){
        stack<Node*> s;
        s.push(root);
        vector<int> res;

        while(!s.empty()){
            Node* now = s.top();
            s.pop();
            if(now->isLeaf())res.push_back(now->suffix_id);
            vector<pair<char , Node*> > nxt;
            for(pair<char , Node*> nx : now->child )if(nx.second != nullptr)nxt.push_back(nx);
            
            sort(nxt.rbegin() , nxt.rend() , cmp_char);//辺の最初の文字でソート

            for(pair<char , Node*> nx : nxt )s.push(nx.second);
        }
        return res;
    }




    /*
        列 S(終端文字を除く) の 辞書順で K 番目の連続部分列
        ただし、返り値は vector<T> なのに注意
        K は 1-index
    */
    vector<T> get_Kth_subStr(long long K){
        Node* now = root;
        long long acum = 0;
        vector<T> res(0);
        init_tree_dp();

        if(root == nullptr)return res;
        if(K > root->SubTreeKindOfString)return res;//そもそも連続部分列の種類数が K に満たない場合

        while(K > acum){
            vector<pair<T,Node*>>cand;
            for(pair<T , Node*> pr : now->child)if(pr.second!=nullptr)cand.push_back(pr);
            sort(cand.begin(),cand.end(),cmp_char);//辺の最初の文字の昇順でソート

            for(pair<T ,Node*> pr : cand){
                Node* nx = pr.second;
                int nx_edge_num = nx->cright() - nx->cleft() + 1;//次の頂点への辺上のprefixの種類数
                if(nx->cright() == endSymbol && S.back() == endChar)nx_edge_num--;//(終端文字を無視するのも忘れずに)
                
                //この辺の先に答えがある場合
                if(nx->SubTreeKindOfString + nx_edge_num + acum >= K){
                    if(acum + nx_edge_num >= K){//答えとなる文字列が、次の頂点への辺の上にあるとき
                        for(int i = nx->cleft() - 1 ; i < nx->cleft() - 1 + K - acum ; i++ )res.push_back(S[i]);
                        return res;
                    }else{
                        for(int i = nx->cleft() - 1 ; i < nx->cright() ; i++ )res.push_back(S[i]);
                        now = nx;//辺の先に移動
                        acum += nx_edge_num;
                        break;//さっきいたノードからは抜ける
                    }
                }else{
                    //この辺の先に答えはないので、次の辺を見る
                    acum += nx->SubTreeKindOfString + nx_edge_num;
                }
                
            }
        }
        assert(0);
        return res;
    }


    // LCA 周りは少し怖い

    // 頂点 u と v の LCA (u,vは頂点番号)
    int LCA(int u , int v){
        
        init_LCA();
        init_data();// tree_depth が必要なので
        if(tree_depth[u] > tree_depth[v])swap(u,v);// u の方が v よりも浅い方で統一する
        // u と v の深さが一致するまで v を上に上げる
        for(int k = int(parent.size())-1 ; k >= 0 ; k--){
            int vx = parent[k][v];
            if(vx < 0)continue;
            if(tree_depth[vx]>=tree_depth[u])v = vx;
        }

        // 根から同一パス上にある場合
        if(u == v)return u;

        for(int k = int(parent.size())-1 ; k >= 0 ; k--){
            int ux = parent[k][u];
            int vx = parent[k][v];
            if(ux != vx){
                u = ux;
                v = vx;
            }
        }

        return parent[0][u];
    }


    // 頂点 u と v の LCA (u,vは頂点のポインタ)、返り値もポインタ
    Node* LCA(Node* u , Node* v){
        return Nodes[LCA(u->id , v->id)];
    }

    // 地点データ sd と、頂点 v の LCA 。LCA が 辺の途中になる可能性もあるので ProcessingData で返す 
    ProcessingData LCA(ProcessingData sd , Node* v){
        if(sd.nextNode!=nullptr){
            if(LCA(sd.nextNode , v) == sd.nextNode){
                // 地点 sd が根から v までのパス上にある場合
                return sd;
            }
        }
        ProcessingData res;
        res.RecentNode = LCA(sd.RecentNode , v);
        res.nextNode = nullptr;
        res.start_index = 1;
        res.path_length = res.RecentNode->get_path_len();
        return res;
    }

    // 地点データ sd と、頂点 v の LCA 。LCA が 辺の途中になる可能性もあるので ProcessingData で返す 
    ProcessingData LCA(Node* v , ProcessingData sd){    
        return LCA(sd,v);
    }


    // 地点データ sd と、地点データ sp の LCA 。LCA が 辺の途中になる可能性もあるので ProcessingData で返す 
    ProcessingData LCA(ProcessingData sd , ProcessingData sp){
        ProcessingData res;
        if(sd.nextNode != nullptr && sp.nextNode != nullptr){
            if(sd.nextNode == sp.nextNode && sd.RecentNode == sp.RecentNode){// 同じ辺の上の地点の場合
                if(sd.path_length < sp.path_length)return sd;// 浅い方が答え
                else return sp;
            }else {
                // 同じ辺にいない場合、深い方は recentNode まで下げて良い
                if(sd.path_length > sp.path_length)swap(sd,sp);// sp が深い方で標準化
                return LCA(sd,sp.RecentNode);
            }
        }else if(sd.nextNode == nullptr && sp.nextNode != nullptr)return LCA(sp , sd.RecentNode);
        else if(sp.nextNode == nullptr && sd.nextNode != nullptr)return LCA(sd , sp.RecentNode);
        else {
            res.RecentNode = LCA(sd.RecentNode , sp.RecentNode);
            res.nextNode = nullptr;
            res.path_length = res.RecentNode->get_path_len();
            res.start_index = 1;
            return res;
        }
    }

    // root を表す ProcessingData(地点データ) を返す
    ProcessingData from_root(){
        ProcessingData res;
        res.RecentNode = root;
        res.nextNode = nullptr;
        res.path_length = 0;
        res.start_index = 1;// 文字列読み込み開始地点 (Suffix Link を辿るタイミングで +1 する必要がある)
        return res;
    }

    // 頂点番号が v であるノードを返す
    Node* get_node(int v){
        init_data();
        return Nodes[v];
    }

    // i 番目の suffix の終端に対応する葉ノードを返す(1-index)
    Node* get_suffix_node(int i){
        init_data();
        return SuffixNodes[i];
    }

    // 根ノードを返す
    Node* get_root(){
        return root;
    }

    // 頂点 v の深さ(根から辺を何本経由するか)
    int get_depth_as_tree(int v){
        init_data();
        return tree_depth[v];
    }

    // S[i] (0-index) を返す (read-only)
    const T operator [](int i) const {
        return S[i];
    }



    //終端文字は無視した、S.size()
    int size(){
        int res = S.size();
        if(res >= 1 && S.back() == endChar)res--;
        return res;
    }


    int tree_size(){
        return vertex_id;
    }
    
    // dfs 順の Node* を返す
    vector<Node*> dfs(){
        vector<Node*> res;
        if(root == nullptr)return res;
        stack<Node*> q;
        q.push(root);

        while(!q.empty()){
            Node* now = q.top();
            q.pop();
            if(now == nullptr)continue;        
            res.push_back(now);
        for(pair<T , Node*> p : now->child){
                if(p.second == nullptr)continue;
                q.push(p.second);
            }
        }
        return res;
    }

    void debug_dfs(){
        vector<Node*> d = dfs();
        vector<pair<int,int> > links;
        for(Node* now : d){
            if(now == nullptr)continue;
            if(now != root)cerr << now->parent->id << " → " << now->id << endl;
            if(now->SuffixLink != root && now->SuffixLink != nullptr)links.emplace_back(now->id , now->SuffixLink->id);
            cerr << "cover : " << now->cleft() << " " << now->cright() << endl;
        }
        
        for(pair<int,int>pr : links){
            cerr << "suffix link : " << pr.first << " - " << pr.second << endl;
        }
    }


};





