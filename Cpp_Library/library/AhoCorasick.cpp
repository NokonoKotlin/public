#include "MyTemplate.hpp"




/*
    T 型の列を管理する Trie 木
    VecOrStr は vector or string の意
    string の中身は T 型にキャストして扱う前提で実装

    動的にしたいが、ひとまずは静的に aho-corasick も実装

    add(s) : 辞書に s を追加
    erase(s) : 辞書から s を削除

    辞書では要素の重複を許さない。
    許す場合は Node クラスの持つ bool endpoint; を、格納されている文字列の個数として書き直す。

    aho_corasick_build() で aho-corasick の準備

    match(s) で、s の全ての部分文字列(同じものも、取り出す位置が異なれば区別する)のうち、
    辞書に格納されているものの個数を計算する。

*/
template<class T>
class Trie{
    private:
    struct Node{
        int id;
        
        long long match_cnt = 0;
        // failure を辿って到達できる頂点の持つ値をマージ (今回はマッチしてるパターンの個数)
        // link ができた時に計算する (BFS なので DP しています)

        int degree = 0;//出次数

        T in;// この頂点に入ってくる辺に書かれた文字

        Node* parent;// 親
        Node* link;// failure の先
        Node* rlink;// このノードにリンクするノード

        unordered_map<T , Node*> child;

        bool endpoint = false; //終端ノードかどうか
        Node(int id , Node* parent , T in):id(id),parent(parent),in(in){}

        unordered_map<T , Node*> nearest_descendant_linker; 
        // [c] := このノードから failure を辿って到達できる頂点 (自身を除外) で、c 方向の辺が存在する直近の頂点から、c 方向に移動した頂点
        // link を構築した時点で、link 先の nearest_descendant_linker と、link 先の child から計算する
    };

    Node* root = nullptr;//根
    int vertex_id_cnt = 0;

    // Node* のファクトリ関数(安全に使うため、root だけにはこの関数を適用しないで)
    Node* fac_node(int id , Node* par , T c){
        Node* res = new Node(id ,par ,c);
        res->link = root;// root にリンク
        return res;
    }

    // now から c 方向に移動できるか
    bool findnext(T c , Node *now){
        if(now->child[c] == nullptr)return false;
        else return true;
    }

    // now から c 方向に移動する( now のポインタ先を変更する(ポインタを参照渡し)。ポインタの中身は変更しない)
    bool move(T c , Node*& now){
        if(!findnext(c,now))return false;
        now = now->child[c];
        return true;
    }

    // s を探索し、対応するノードのポインタを返す
    template<class VecOrStr>
    Node* search(VecOrStr s){
        Node* now = root;
        for(T c : s){
            if(!move(c,now))return nullptr;
        }
        return now;
    }

    // ポインタの中身を編集し、分岐先を追加する
    void addleaf(T c , Node *now){
        if(findnext(c,now))return; //assert にしようかな
        Node* next = fac_node(vertex_id_cnt++ , now , c);// リーフを追加
        
        now->child[c] = next;
        now->degree++;
    }

    public:

    Trie(){
        root = new Node(vertex_id_cnt++,nullptr,T());
        root->link = root;
    }

    // 辞書に存在するか判定 (ノードが存在するかとはまた別なので注意)
    template<class VecOrStr>
    bool find(VecOrStr s){
        Node* now = search(s);
        if(now == nullptr)return false;
        return now->endpoint;// 終端ノードかどうか
    }






    // s を辞書に追加
    template<class VecOrStr>
    void add(VecOrStr s){
        Node* now = search(s);
        
        if(now == nullptr){
            now = root;
            for(int i = 0 ; i < s.size() ; i++){
                if(findnext(s[i] , now))move(s[i],now);
                else {
                    addleaf(s[i],now);
                    move(s[i],now);
                }
            }
        }
        if(!now->endpoint){
            now->match_cnt = 1;
            now->endpoint = true;
        }
        return;
    }


    // s を辞書から削除
    template<class VecOrStr>
    void erase(VecOrStr s){
        Node* now = search(s);
        if(now == nullptr)return ;
        if(now->degree == 0){
            now->endpoint = false;
            // now (sに対応) から上にのぼり、一本道かつ、endpoint でない限りそのノードは不要なので削除
            while(now->degree == 0 && !now->endpoint){
                Node* tmp = now->parent;
                delete now;// メモリ大事
                now = tmp;
                now->degree--;
            }
        }else{
            now->endpoint = false;
            now->match_cnt = 0;
        }
    }

    // aho-corasick の構築()できれば動的にしたいので、今度考える
    void aho_corasick_build(){
        queue<Node*> q;
        q.push(root);
        while(!q.empty()){
            Node* now = q.front();
            q.pop();
            for(pair<T,Node*> nxpr : now->child){
                if(nxpr.second == nullptr)continue;

                // Now → Next という辺に注目する
                Node* Next = nxpr.second;
                
                // 新しいリンク先は、Now の nearest_descendant_linker から Next->in の方向に移動したもの (Now 自身から出る辺は見ていない)
                Node* newLink = now->nearest_descendant_linker[Next->in];

                if(newLink != nullptr)Next->link = newLink;
                else Next->link = root;

            
                q.push(Next);

                // Next の link がわかったので、Next の nearest_descendant_linker やマージ値を link 先の情報から計算する
                   
                Next->match_cnt += Next->link->match_cnt; // failure で移動できる子孫が持つ値をマージ
                   
                for(pair<T,Node*> pr : Next->link->nearest_descendant_linker){// link 先の nearest_descendant_linker を参照
                    if(pr.second == nullptr)continue;
                    Next->nearest_descendant_linker[pr.first] = pr.second;
                }

                for(pair<T,Node*> pr : Next->link->child){// link 先の child を参照 (こっちを優先して更新(深さが浅いので))
                    if(pr.second == nullptr)continue;
                    Next->nearest_descendant_linker[pr.first] = pr.second; 
                }
            }
        }
    }

    // s の部分文字列で、辞書に格納されているものにマッチするものがいくつあるか
    template<class VecOrStr>
    long long match(VecOrStr s){
        Node* now = root;
        long long res = 0;
        for(T c : s){
            while(now != root && !findnext(c,now))now = now->link;
            if(move(c,now))res += now->match_cnt; 
        }
        return res;
    }

    // 地点 s からリンクが正しくできているかをチェック
    template<class VecOrStr>
    void debug_check_link(VecOrStr s){
        Node* now = search(s);
        if(now == nullptr){
            cerr << "DEBUG : " << s << " was Not found" << endl;
            return;
        }
        Node* link = now->link;
        vector<T> V;
        while(link->parent!=nullptr){
            V.push_back(link->in);
            link = link->parent;
        }
        reverse(V.begin() , V.end());

        cerr << "detected link : " ;
        for(T c : V)cerr << c << " ";
        cerr << endl;
        
        vector<T> V_naive;

        
        for(int L = 1 ; L < s.size() ; L++){// s の接尾辞 [ L , |s| ) を全部見る
            string s_suf = "";
            for(int i = L ; i < s.size() ; i++)s_suf.push_back(s[i]);
            if(search(s_suf)!=nullptr){
                for(T c : s_suf)V_naive.push_back(c);   
                break;
            }
        }

        cerr << "correct link : " ;
        for(T c : V_naive)cerr << c << " ";
        cerr << endl;

        bool ok = true;
        if(V_naive.size() != V.size())ok = false;
        else{
            for(int i =  0 ; i < V.size() ; i++ )if(V_naive[i] != V[i])ok = false;
        }

        if(ok)cout << "result : OK" << endl;
        else cout << "result : invalid link" << endl;
        cout << "DEBUG END" << endl;
    }
    

};







