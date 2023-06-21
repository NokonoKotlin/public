#include<algorithm>
#include<iostream>
#include<map>
#include<vector>

using namespace std;










/*

    splitした後はmergeを忘れずに！！(それかSplitした頂点のrootを覚えておこう)

    頂点にアクセスしたら、その頂点をsplayするのを忘れるな！！
    頂点をいじったらsplayの後、Updateするのを忘れないで(順番も注意)


    rootを頂点とする部分木において{
        split(l,root) := lで分割

        merge(l_root , r_root) := 頂点がl_root ,r_rootであるような部分木を連結

        insert_sub(ind,NODE,root) := ind番目にNODEが挿入される

        Delete_sub(ind ,root) := ind番目を削除(返り値はfirstが新しいroot , secondが削除したNode)

    }
    これらを駆使し、様々な操作を実装可能



    
    この木では、ノードは常に連結であることを前提とした機能を搭載する
    この木では、ノードは常に連結であることを前提とした機能を搭載する
    この木では、ノードは常に連結であることを前提とした機能を搭載する
    この木では、ノードは常に連結であることを前提とした機能を搭載する
    この木では、ノードは常に連結であることを前提とした機能を搭載する


    つまり、削除した扱いのノードは、連結のノードと区別するために bool isThisDeleted をもち、
    split したまま放置はできない。
    と言うのも、後述する pos[x] で、pos[x] に格納されてる Node をsplayしてrootにするのだが、
    別の連結成分に属するNodeをrootにすると、バグってしまうのだ

    sub 関数でpos を操作しないで！！！

    

    get_position(x)で、数列内のxのindexをまとめたvectorを返す。
    要素xを持つNODEを木に挿入したとき、pos[x] に 挿入したノードをプッシュする
    Deleteするときは、isThisDeleted をtrueにする
    get_position(x)でpos[x]の中身を調べて、無効になっている(もしくは同じ要素だが、shiftなどで複数回挿入されている)要素があればpopする



    posに入っているNODEは、SplayNodeのポインタなので、数列が更新されると、ポインタの中身も連動して更新される。
    中身のポインタをsplayし、自分の位置を計算できる。ただし、splayした後はrootにすることを忘れないで





    遅延評価のreverseはつけれるかどうか不明


    他のライブラリと違い、Delete(i)の後、削除したNodeのisThisDeletedをtrueにしている
    ただし、これはshift_subなどの内部の関数でDelete_sub()を使う際は、削除したノードをまた挿入するので、
    Delete_subではこの操作を行わない



*/
template<class T>
class SearchableArray{
    private:


    /*
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
            SplayTreeの、Nodeのデータ構造。
            SplayTreeでは、Glaphの配列上で操作のやり取りをするのではなく、
            SplayNodeというデータ構造に持たせたデータでやり取りをする
        ------------------------------------------------------------------------------------------------------------------------------------------------------------   
        ------------------------------------------------------------------------------------------------------------------------------------------------------------

    */
    
    struct SplayNode{

        SplayNode *parent;//親ノード
        SplayNode *left;//左の子ノード
        SplayNode *right;//右の子ノード

        T Value;//値(普通、頂点には値や重みが割り当てられることが多い)
        T Min,Max,Sum;//部分木のうち、値の最大、最小、和

        bool isThisDeleted = false;//このノードが、木から削除されたか(getpositionで使用する。shift_rightなどで一時的に切り離された場合は、falseのまま)

        int SubTreeSize = 1;//1は初期化値(自分自身)


        SplayNode(){
            parent = nullptr;
            left = nullptr;
            right = nullptr;
        }
        SplayNode(T val){
            parent = nullptr;
            left = nullptr;
            right = nullptr;
            Value = val;
            Min = val;
            Max = val;
            Sum = val;
        }


        bool isExist(SplayNode *Nd){
            if(Nd!=nullptr)return true;
            else return false;
        } 



        /*
            ある地点を回転させる
            回転時、自分自身(this)と親(Parent)の位置関係が逆になる
            よって、
            下から　Parent　⇨  this の順番でupdateする

        */
        void rotate(){

            SplayNode *Parent , *GrandParent , *Child;
            Parent = this->parent;
            
            Child = nullptr;

            if(isExist(Parent) == false){
                return;
            }
            GrandParent = Parent->parent;
            



            //元の親に対する位置で場合分け
            if(Parent->left == this){
                Child = this->right;//Childはnullptrかもしれない
                this->right = Parent;
                Parent->left = Child;

            }else if(Parent->right == this){
                Child = this->left;
                this->left = Parent;
                Parent->right = Child;

            }

            if(isExist(GrandParent)){
                //GrandParentのどちらの辺なのかで場合分け
                if(Parent == GrandParent->left){
                    GrandParent->left = this;

                }else if(Parent == GrandParent->right){
                    GrandParent->right = this;
                    
                }
            }

            //GrandParentがnullptrの場合でも代入OK
            this->parent = GrandParent;


            //親と位置を入れ替えるので
            Parent->parent = this;



            //Childは移動するので。
            if(isExist(Child))Child->parent = Parent;


            Parent->update();
            this->update();
            

            

            return;

        }







        /*
            自分の親から見た自分の位置
            0 -> 親がそもそもいない or 例外の場合
            1 -> 親から見て左の子の場合
            2 -> 親から見て右の子の場合 
        */
        int state(){
            if(isExist(this->parent)== false){
                return 0;
            }else{
                if(this->parent->left == this){
                    return 1;
                }else if(this->parent->right == this){
                    return 2;
                }else {
                    return 0;
                }
            }
        }







        /*
            あるNodeを回転を駆使し一番上まで持っていく
            この時、回転する順番に注意
        */
        void splay(){

            if(isExist(this) == false)return;
            
            while(isExist(this->parent)){
                if(isExist(this->parent->parent)==false){
                    //(自分のGrandParentがいない場合、rotateは一回のみ)
                    this->rotate();
                    return;


                //それ以外の場合、(自分、親、親の親)の位置関係が大事
                }else if(this->state() == this->parent->state()){
                    //GrandParentからの道が(右、右)or (左、左)の場合
                    //GrandParentを一つ下に下ろして、自分を上げる
                    this->parent->rotate();
                    this->rotate();

                }else{
                    //それ以外の場合、単純に自分を2回上に上げれば良い
                    this->rotate();
                    this->rotate();
                }
            }



            return;
        }   






        /*
            サイズなど、Nodeの持っている情報をupdateする(順番大事)
        */
        void update(){
            if(isExist(this) == false)return;


            this->SubTreeSize = 1;
            this->Min = this->Value;
            this->Max = this->Value;
            this->Sum = this->Value;


            if(isExist(this->left)){
                this->SubTreeSize += this->left->SubTreeSize;
                if(this->left->Min < this->Min)this->Min = this->left->Min;
                if(this->left->Max > this->Max)this->Max = this->left->Max;
                this->Sum += this->left->Sum;
            }

            if(isExist(this->right)){
                this->SubTreeSize += this->right->SubTreeSize;
                if(this->right->Min < this->Min)this->Min = this->right->Min;
                if(this->right->Max > this->Max)this->Max = this->right->Max;
                this->Sum += this->right->Sum;

            }

            

            

            return;
        }



    };



    /*
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
        ------------------------------------------------------------------------------------------------------------------------------------------------------------
            ここからやっとSplayTreeに載せる関数たちの出番
            rootを根とする部分木において、
              split(ind,root) , merge(l_root, r_root) , delete_sub(ind,root) , insert_sub()　など
            これらは主にclass内で使われる。
            classの外で使う関数として、
                Delete , insert, RangeSumQuery , shiftなどが想定される
        ------------------------------------------------------------------------------------------------------------------------------------------------------------   
        ------------------------------------------------------------------------------------------------------------------------------------------------------------

    */



    
    

    //この木のroot,splitした後はmergeして戻す！！rootが迷子にならないように！！
    SplayNode *Root = nullptr;

    //エラー時の返り値
    T initializer;

    //pos[x] := Value = xであるようなSplayNodeのポインタを入れておく。(Value = xであるような要素の探索に使える)
    map<T , vector<SplayNode*> > pos;

    /*
        pos[x] の中身を見る中で、update_valなどの操作により、同じ要素(ポインタ)がpos[x]に複数存在する場合がある。
        get_position を 呼び出した回数が get_pos_called_time 回で、pos[x] の中に位置が p の要素があった時、
        checked[ p ] = get_pos_called_time を書き込み、すでに書き込んであったら重複しているとみなして pos[x]からeraseする。
        これにより、checked を使い回すことができて、mapを使うよりも計算量を落とせる。
    */
    vector<int> checked;
    int get_pos_called_time = 0;//get_position関数を呼び出した回数をメモ



    bool isExist(SplayNode *Nd){
        if(Nd==nullptr){
            return false;
        }else{
            return true;
        }
    }




    /*
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        ここからは、クラス内で動かす関数たちです。俗にいうブラックボックスです。
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    */





    /*
        rootを根とする部分木において、
        indexを左からの0-indexとすると、

        求めたいindexになるまで降りていく

    */
    SplayNode *getNode(int index , SplayNode *root){
        
        if(isExist(root)==false)return root;
        

        SplayNode *now = root;
        

        while(1){
            
            //left_sizeに代入する前にeval。（後でevalするとleft_sizeの値が変わるから）
            int left_size = 0;
            if(isExist(now->left)){
                left_size = now->left->SubTreeSize;
            } 
            
            if(index < left_size){
                now = now->left;
            }else if(index == left_size){
                now->splay();
                break;
            }else if(index > left_size){
                now = now->right;
                index = index - left_size-1;
            }

        }
        

        return now;
    }













    /*
        それぞれ、leftrootとrightrootが根であるような部分木をくっつける

        splitした後、これをしてまたsplitしたものをくっつける
        splitしたまま放置すると分割された木のrootが行方不明になりそう
        leftとrightの順番大事！！
    */
    SplayNode *merge(SplayNode *leftRoot , SplayNode *rightRoot){

       
        if(isExist(leftRoot) == false)return rightRoot;//右のみあり
        if(isExist(rightRoot) == false )return leftRoot;//左のみあり


        leftRoot = getNode(leftRoot->SubTreeSize-1 , leftRoot);
        leftRoot->right = rightRoot;
        rightRoot->parent = leftRoot;
        leftRoot->update();
        
        
        return leftRoot;
    }









    /*
         rootを根とする部分木において、
        左からleftnum番目の頂点で左右にsplit。
        splitした後はmergeしないと,splitした木の頂点が行方不明になりそう
    */
    std::pair<SplayNode*,SplayNode*> split(int leftnum, SplayNode *root){

        if(leftnum<=0)return std::make_pair(nullptr , root);

        if(leftnum >= root->SubTreeSize)return std::make_pair(root, nullptr);



        root = getNode(leftnum , root);
        
        SplayNode *leftRoot , *rightRoot;

        leftRoot = root->left; 
        
        rightRoot = root; 

        if(isExist(rightRoot))rightRoot->left = nullptr;

        if(isExist(leftRoot))leftRoot->parent = nullptr;
        
        rightRoot->update();
        leftRoot->update();
        

        return std::make_pair(leftRoot,rightRoot);

    }





    /*
        rootを根とする部分木において、
        indexの場所を切り離して、NODEを追加して元に戻す(merge)
    */
    SplayNode  *insert_sub(int index , SplayNode *NODE , SplayNode *root){
        if(isExist(root) == false){
            root=NODE;
            Size=1;
            return root;
        }
        if(Size<index || index<0)return root;

        std::pair<SplayNode*,SplayNode*> Trees = split(index,root);

        SplayNode *leftRoot = Trees.first;
        SplayNode *rightRoot = Trees.second;
        Size++;

        return merge(merge(leftRoot , NODE),rightRoot);
    }









    /*  
        rootを根とする部分木において、
        左からindexの場所を削除して、両端をくっつける
        firstは消したやつ以外をmergeした時のroot
        secondは消したSplayNode
    */
    std::pair<SplayNode*,SplayNode*> Delete_sub(int index , SplayNode *root){
        
        if(isExist(root) == false){
            return std::make_pair(root,root);
        }

        if(Size == 1){
            Size = 0;
            return std::make_pair(nullptr,root);
        }

        root = getNode(index,root);
        

        SplayNode *leftRoot = root->left;
        SplayNode *rightRoot = root->right;

        if(isExist(leftRoot))leftRoot->parent = nullptr;
        if(isExist(rightRoot))rightRoot->parent = nullptr;
        root->left = nullptr;
        root->right = nullptr;

        Size--;
        
        root->update();
        
        return std::make_pair(merge(leftRoot,rightRoot) , root );
    }








    /*
        rootを根とする部分きにおいて、
        0-indexの区間[l,r)を右シフト(右端は左端へ)する
    */
    SplayNode *shift_right_sub(int l , int r , SplayNode *root ){
        r--;//半開区間なので

        std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(r,root);
        root = tmp.first;
        SplayNode *N = tmp.second;

        return insert_sub(l,N,root);

    }



    /*
        rootを根とする部分きにおいて、
        0-indexの区間[l,r)を左シフト(左端は右端へ)する
    */
    SplayNode *shift_left_sub(int l , int r , SplayNode *root ){
        r--;//半開区間なので

        std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(l,root);
        root = tmp.first;
        SplayNode *N = tmp.second;

        return insert_sub(r,N,root);

    }









    /*
        rootを根とする部分木の[l,r)の区間取得クエリ
        modeが 0->Min, 1->Max ,2->Sum 
    */
    std::pair<SplayNode*,T> RangeQuery_sub(int l , int r , int mode , SplayNode *root){

        SplayNode *leftRoot , *centerRoot , *rightRoot; 
        r--;//半開区間なので

        std::pair<SplayNode*,SplayNode*>  tmp = split(r+1,root);
        rightRoot = tmp.second;
        
        tmp = split(l,tmp.first);
        leftRoot = tmp.first;
        centerRoot = tmp.second;
        
        T ans;
        //modeによって取得する値が違う
        if(mode == 0){
            ans  = centerRoot->Min;
        }else if(mode == 1){
            ans  = centerRoot->Max;
        }else if(mode == 2){
            ans = centerRoot->Sum;
        }else{
            ans = centerRoot->Sum;
        }

        return std::make_pair(merge(merge(leftRoot,centerRoot),rightRoot),ans);
    }






    /*
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        ここからは、クラスの外で動かす関数たちです。
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    */



    public:

    int Size = 0;

    SearchableArray(){}



    /*
        木全体のindex番目のNode(値渡し)を取得
    */
    SplayNode get(int index){
        if(index<0 || index>=Size){
            std::cerr<< "index exceeds Size" << std::endl;
            return SplayNode(initializer);
        }

        Root = getNode(index,Root);
        Root->update();

        return *(Root);

    }




    /*
        index番目にSplayNode*を挿入
        checked 配列のサイズも更新 
    */
    void insert(int index , SplayNode *NODE){
        if(index<0 || index> Size)return;
        
        //位置検索mapに追加
        pos[NODE->Value].push_back(NODE);

        Root = insert_sub(index , NODE , Root);
        Root->update();

        if(checked.size() <= Size){
            int resize = checked.size()+1;//２倍ぐらい取るか ~
            checked.resize(resize , -1);//要素が存在しうる位置の候補が増えたので、配列を拡張
        }
        

        return;

    }





    /*
        末尾に追加

    */
    void push_back(T val){
        insert(Size , new SplayNode(val)); 

        return;
    }

    /*
        先頭に追加
    */
    void push_front(T val){
        insert(0 , new SplayNode(val));
        return;
    }


    /*
        index番目に追加
    */
    void push(int index , T val){
        insert(index , new SplayNode(val));
        return;
    }









    /*
        木全体の[l,r)のRMQ
    */
    T RangeMinQuery(int l , int r){
        l = max(0,l);
        r = min(Size,r);
        if(l>=r){
            cerr << "right must be larger than left " << endl;
            return initializer;
        }

        std::pair<SplayNode*,T> tmp = RangeQuery_sub(l,r,0,Root);
        Root = tmp.first;
        
        Root->update();

        return tmp.second;
    }


    /*
        木全体の[l,r)のRMQ
    */
    T RangeMaxQuery(int l , int r){
        l = max(0,l);
        r = min(Size,r);
        if(l>=r){
            cerr << "right must be larger than left " << endl;
            return initializer;
        }

        std::pair<SplayNode*,T> tmp = RangeQuery_sub(l,r,1,Root);
        Root = tmp.first;
        Root->update();

        return tmp.second;
    }



    /*
        木全体の[l,r)のRSQ
    */
    T RangeSumQuery(int l , int r){
        l = max(0,l);
        r = min(Size,r);
        if(l>=r){
            cerr << "right must be larger than left " << endl;
            return initializer;
        }

        std::pair<SplayNode*,T> tmp = RangeQuery_sub(l,r,2,Root);

        Root = tmp.first;
        Root->update();

        return tmp.second;
    }






    /*
        木全体で、[l,r)を右シフト(右端は左端へ)
    */
    void shift_right(int l , int r){
        l = max(0,l);
        r = min(Size,r);
        if(l+1>=r)return;

        Root = shift_right_sub(l,r,Root);
        Root->update();

        return;
    }





    /*
        木全体で、[l,r)を左シフト(左端は右端へ)
    */
    void shift_left(int l , int r){
        l = max(0,l);
        r = min(Size,r);
        if(l+1>=r)return;

        Root = shift_left_sub(l,r,Root);
        Root->update();

        return;
    }






    /*
        木全体のx番目のvalueをyにする
    */
    void update_val(int x , T y){
    
        Root = getNode(x,Root);

        if(isExist(Root)==false)return;
        Root->Value = y;
        Root->update();

        //位置検索mapに追加
        pos[y].push_back(Root);

        return;
    }




    /*
        index番目のNodeを消去
    */
    void Delete(int index){
        if(index<0 || index >= Size)return ;
        std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(index,Root);
        Root = tmp.first;
        if(tmp.second != nullptr)tmp.second->isThisDeleted = true;
        if(isExist(Root) == false )return;
        Root->update();

        return;
    }








    //数列内のxの位置を全てvector<int>に入れて返す , countを指定すると、指定した個数以下の個数(選び方がランダム)を返す
    vector<int> get_position(T x , int count = 1000000000){
        if(Size == 0)return vector<int>(0);
        get_pos_called_time++;//呼び出し回数をインクリメント

        vector<SplayNode*> *pos_x = &pos[x];//log を落とすためにポインタで O(1) でコピー
        vector<int> res(0);

        int size = (*pos_x).size();
        
        

        for(int i = size -1  ; i >= 0 ; i--){
            if(res.size()>=count)break;
            SplayNode *Nd = (*pos_x)[i];
            
 
            
            if(Nd == nullptr)(*pos_x).erase((*pos_x).begin()+i);
            else if(Nd->isThisDeleted)(*pos_x).erase((*pos_x).begin()+i);//削除済みを弾く
            else if(Nd->Value != x)(*pos_x).erase((*pos_x).begin()+i);//値の変更ずみを弾く
            else{

                //NODEのindex関数で、上の辺を辿って、自分がどの位置にあるのかを取得(splayして自分を上に持って行ってもいいけど、Deleteで木から削除された頂点を弾く場合の処理が面倒)
                
                Nd->splay();
                Nd->update();
                int index = 0;
                T val = Nd->Value;
                if(isExist(Nd->left))index+=Nd->left->SubTreeSize;


                //index番目が、本当にxと同じかどうかをチェック(deleteによって無効になったノードもあるので)
                if(val == x){
                    //このNODEはupdate_valなどの操作によりpos[x]に複数存在する場合はpop
                    if(index < 0 || index > checked.size()){
                        (*pos_x).erase((*pos_x).begin()+i);
                    }else if(checked[index] == get_pos_called_time){// (*pos) に 同じノードが複数ある。
                        (*pos_x).erase((*pos_x).begin()+i);
                    }else{
                        //見つけた場合、答えに追加して、checkedでメモ
                        res.push_back(index);
                        checked[index] = get_pos_called_time;
                    }

                }else (*pos_x).erase((*pos_x).begin()+i);
                
                Root = Nd;//ルートの更新を忘れずに
                
            }
        }

        
        sort(res.begin() , res.end());
        return res;
    }






    /*
        一列に表示
    */
    void Debug(){
        std::cerr<<"DEBUG:" << std::endl;
        for( int i = 0 ; i < Size ; i++){
            std::cerr<< get(i).Value << " ";
        }
        std::cerr<< std::endl;
    }




};
















int main(){
    
    SearchableArray<int> S;
    S.push_back(rand()%100);
    S.push_back(rand()%100);
    S.push_back(rand()%100);
    S.push_back(rand()%100);
    S.push_back(rand()%100);
    S.push_back(rand()%100);
    S.push_back(rand()%100);
    S.push_back(rand()%100);
    S.push_back(rand()%100);
    S.push_back(rand()%100);
    int Key = S.get(0).Value;
    vector<int> pos = S.get_position(Key);
    cerr << "Key is " << Key << "." << endl;
    cerr << "----------------------------------------------------"<<endl;
    S.Debug();
    cerr << "Key("<<Key << ") is located at index of ";for(int p: pos)cerr << p << " , ";cerr << "now" << endl;

    cerr << "----------------------------------------------------"<<endl;
    S.shift_right(3,7);
    cerr << "right shift [3 , 7) - "; 
    S.Debug();
    pos = S.get_position(Key);
    cerr << "Key("<<Key << ") is located at index of ";for(int p: pos)cerr << p << " , ";cerr << "now" << endl;

    cerr << "----------------------------------------------------"<<endl;
    S.Delete(5);
    cerr << "deleted 5 th (0-index)"; 
    S.Debug();
    pos = S.get_position(Key);
    cerr << "Key("<<Key << ") is located at index of ";for(int p: pos)cerr << p << " , ";cerr << "now" << endl;

    cerr << "----------------------------------------------------"<<endl;
    S.update_val(S.Size-1 , Key);
    cerr << "update back - "; 
    S.Debug();
    pos = S.get_position(Key);
    cerr << "Key("<<Key << ") is located at index of ";for(int p: pos)cerr << p << " , ";cerr << "now" << endl;

    cerr << "----------------------------------------------------"<<endl;
    S.shift_left(1,6);
    cerr << "shift left [1 , 6) - "; 
    S.Debug();
    pos = S.get_position(Key);
    cerr << "Key("<<Key << ") is located at index of ";for(int p: pos)cerr << p << " , ";cerr << "now" << endl;

    cerr << "----------------------------------------------------"<<endl;
    S.push(2,Key);
    cerr << "insert - "; 
    S.Debug();
    pos = S.get_position(Key);
    cerr << "Key("<<Key << ") is located at index of ";for(int p: pos)cerr << p << " , ";cerr << "now" << endl;
    



    return 0;
}