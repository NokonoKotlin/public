#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import tkinter as tkn


import pickle

def showMessage(text):
    tkn.messagebox.showinfo('メッセージ',text)









# Mはメインobject root1はtkinter parent Bはメニューに戻るボタン


def adv(root1):
    
    global x , y , dire , lab , b1 ,b2 ,b3,b4 ,lab2 ,deletebutton,Map

    

    i1 = open("./kabe/item.txt" , "r")
    fn = open("./kabe/nil.txt" , "r")
    f02 = open("./kabe/0_2.txt","r")
    f12 = open("./kabe/1_2.txt","r")
    f22 = open("./kabe/2_2.txt","r")
    f30 = open("./kabe/3_0.txt","r")
    f32 = open("./kabe/3_2.txt","r")
    f33 = open("./kabe/3_3.txt","r")
    f40 = open("./kabe/4_0.txt","r")
    f41 = open("./kabe/4_1.txt","r")
    f42 = open("./kabe/4_2.txt","r")
    f50 = open("./kabe/5_0.txt","r")
    f51 = open("./kabe/5_1.txt","r")
    f52 = open("./kabe/5_2.txt","r")
    f53 = open("./kabe/5_3.txt","r")
    f60 = open("./kabe/6_0.txt","r")
    f62 = open("./kabe/6_2.txt","r")
    f63 = open("./kabe/6_3.txt","r")
    f70 = open("./kabe/7_0.txt","r")
    f71 = open("./kabe/7_1.txt","r")
    f72 = open("./kabe/7_2.txt","r")
    f80 = open("./kabe/8_0.txt","r")
    f81 = open("./kabe/8_1.txt","r")
    f82 = open("./kabe/8_2.txt","r")
    f83 = open("./kabe/8_3.txt","r")
    f90 = open("./kabe/9_0.txt","r")
    f92 = open("./kabe/9_2.txt","r")
    f93 = open("./kabe/9_3.txt","r")
    f100 = open("./kabe/10_0.txt","r")
    f101 = open("./kabe/10_1.txt","r")
    f102 = open("./kabe/10_2.txt","r")
    f110 = open("./kabe/11_0.txt","r")
    f111 = open("./kabe/11_1.txt","r")
    f112 = open("./kabe/11_2.txt","r")
    f113 = open("./kabe/11_3.txt","r")
    f120 = open("./kabe/12_0.txt","r")
    f123 = open("./kabe/12_3.txt","r")
    f130 = open("./kabe/13_0.txt","r")
    f131 = open("./kabe/13_1.txt","r")
    f140 = open("./kabe/14_0.txt","r")
    f141 = open("./kabe/14_1.txt","r")
    f143 = open("./kabe/14_3.txt","r")
    glll = open("./kabe/goal.txt","r")
    d = {2:f02.read(),12:f12.read(),22:f22.read(),30:f30.read(),32:f32.read(),33:f33.read(),40:f40.read(),41:f41.read(),42:f42.read(),50:f50.read(),51:f51.read(),52:f52.read(),53:f53.read(),60:f60.read(),62:f62.read(),63:f63.read(),70:f70.read(),71:f71.read(),72:f72.read(),80:f80.read(),81:f81.read(),82:f82.read(),83:f83.read(),90:f90.read(),92:f92.read(),93:f93.read(),100:f100.read(),101:f101.read(),102:f102.read(),110:f110.read(),111:f111.read(),112:f112.read(),113:f113.read(),120:f120.read(),123:f123.read(),130:f130.read(),131:f131.read(),140:f140.read(),141:f141.read(),143:f143.read()}
    nil = fn.read()
    goal = glll.read()

    do1 = open("./kabe/door1.txt" , "r")
    door1 = do1.read()
    do2 = open("./kabe/door2.txt" , "r")
    door2 = do2.read()
    
    #リストaの中の道のイラストを合成する関数
    def f(a):           #描画に関する約束事
                        #引数はリストで与えること
                        #リスト内の要素を足し合わせた描画を返すこと
                        #スペースとスペース以外ならスペース以外が優先される。
                        #足し合わせが被った場合、リストの順が早かったものが優先される。
                        #テキストの縦横は同じ大きさでなければならない

        
        
        
        
        
        
        
        
        
        p = ""
        for i in range(len(a) - 1):
        
            for t in range(len(a[i])):
                if a[i][t] == "　" and a[i+1][t] != "　":
                    p +=a[i+1][t]
                elif a[i+1][t] == "　" and a[i][t] != "　":
                    p +=a[i][t]
                elif a[i+1][t] == a[i][t]:
                    p += a[i][t]
                elif a[i][t] != "　" and a[i+1][t] != "　":
                    p+=a[i][t]
                else:
                    p+="　"  
            a[i+1] = p
            p = ""
                
        return a[len(a)-1]
    
    
    
    
    def g(a,b):   #a から bと被った部分を消す関数
        p = ""
        for t in range(len(a)):
            
            if a[t] == b[t] and( b[t] != "　" and b[t] != "\n"):
                p+="　"
            else:
                p+=a[t]
        return p
    
    
    
    
    
             #通路の型と方向を定義 0:上、１：左、２：下、３：右
           #スタート地点と方向を設定
     
    x = 1        #マップのデータをロードする
    y = 1
    dire = 0
 
                                    #0:壁、 1:道、 2:ゴール、 4以上:扉、 3:アイテム
                                        
    variable = open("./var/variable_Stage" , "rb")

    st0 = pickle.load(variable)          #st0は 0 , 1 , 2 .... でかかれた二次元配列

    variable.close()
    
    
    st1 = []
    for t in range(len(st0[0])):
        st1.append([])
        
    for t in range(len(st0[0])):
        for u in range(len(st0)):
            st1[t].append(st0[len(st0)-u-1][t])               #st1:  st0を右から見たもの
    
    
    
    st3 = []                                                    #st3: st0を左から見たもの
    
    for t in range(len(st0[0])):
        st3.append([])
        
    for t in range(len(st0[0])):
        for u in range(len(st0)):
            st3[t].append(st0[u][len(st0[0])-t-1])
            
            
    
    st2 = []                                                    #st2: st0を上から見たもの
    
    for t in range(len(st0)):
        st2.append([])
        
    for t in range(len(st0)):
        for u in range(len(st0[0])):
            st2[t].append(st0[len(st0)-t-1][len(st0[0])-u-1])
    
    
    
        
    def dstage(d):        #方角にマッチしたステージを返す.    
        if d == 0:
            s = st0
        elif d == 1:
            s = st1
        elif d == 2:
            s = st2
        elif d == 3:
            s = st3
        return s
    
    
    def dcplus1(b,a,d):   #方角を１増やし,そのときのx,yを返す。   この時ステージも変わる
        global x , y , dire
        x = len(dstage(d))-b-1
        y = a
        if d == 3:
            dire = 0
        else:
            dire +=1
    
    
    def dcminus1(b,a,d):            #方角を１減らし,そのときのx,yを返す。　この時、ステージも変わる。
        global x , y , dire
        x = b
        y = len(dstage(d)[0]) - a - 1   
        if d == 0:
            dire = 3
        else:
            dire -= 1
    
    
    def root(b,a,di):         #a座標　b座標　向き　を入れると　目の前の道の図を返す
                        # 道:1 以外は基本的に壁のイラストを追加する
                        # アイテム：４　の場所にも壁のイラストを追加し、その１つ前の位置にアイテムのイラストを追加する
        
        s = dstage(di)        #方角に合わせた向きのステージを返す
            
            
        p = nil     #何もない空間をセット
        
        def check(e):                       #自分のいくつ向こうに壁、ゴールがあるかを調べる。
            
            if s[b-1][a] != 1 :          #1(道でない)ならば壁である。
                cp = 1
            else:
                if s[b-2][a] != 1 :
                    cp = 2
                else:  
                    if s[b-3][a] != 1:
                        cp = 3
                    else:
                        if s[b-4][a] != 1:
                            cp = 4
                        elif s[b-4][a] == 1:
                            cp = 5
            return cp
        
        cp = check(0)
        
        if cp >= 1:                         #手前から順番に壁の存在を調べる。
            if s[b][a-1] != 1:
                p = f([p,d[123]])
            elif s[b][a-1] == 1:
                if s[b-1][a-1] != 1: 
                    p = f([p,d[120]])                   #左前の壁を作る
                else:
                    p = p
            
            
            if s[b][a+1] != 1:
                p = f([p , d[131]])
            elif s[b][a+1] == 1:
                if s[b-1][a+1] != 1:
                    p = f([p , d[130]])                   #右前の壁を作る
                else:
                    p = p
                    
            if cp == 1:                             #フタをする1
                if s[b-1][a] == 2:
                    p = f([p , goal])
                elif s[b-1][a] == 3:
                    p = f([p , door1])                     #目の前がドアだったらドア開けてして3を1にする、同じ方向だったら二度とドアは表示されない
                    s[b-1][a] = 1                       #違う方向から見るとドアは開いてない
                elif s[b-1][a] >= 4:      #4以上は全てドア
                    p = f([p , door1])                     #ドア
                p = f([p , d[112]])
                
            
            else:
                if s[b-1][a-1] != 1:
                    p = f([p,d[93]])
                elif s[b-1][a-1] == 1:
                    if s[b][a-1] == 1:              #空間が２つ以上ある場合、１つ前の壁を消去
                        p = g(p,d[120])
                    p = f([p,d[90]])                   #左前の壁を作る
            
                if s[b-1][a+1] != 1:
                    p = f([p , d[101]])
                elif s[b-1][a+1] == 1:
                    if s[b][a+1] == 1:              #空間が２つ以上ある場合、１つ前の壁を消去
                        p = g(p,d[130])
                    p = f([p , d[100]])                   #右前の壁を作る
                    
                
                
                if cp == 2:                            #フタをする2
                    if s[b-2][a] == 2:    #ゴールの場合 
                        p = f([d[52],p])
                    elif s[b-2][a] == 3:
                        p = f([p, door2])
                    elif s[b-2][a] >= 4:            #ドア
                        p = f([p, door2])                    
                    if s[b-2][a-1] == 1 and s[b-1][a-1] == 1:
                        p = g(p,d[62])
                        if s[b][a-1] != 1:
                            p = f([p,d[123]])                # 誤って消した壁の部分を元に戻す
                    if s[b-2][a+1] == 1 and s[b-1][a+1] == 1:
                        p = g(p , d[72])
                        if s[b][a+1] != 1:
                            p = f([p , d[131]])               # 誤って消した壁の部分を元に戻す
                    p = f([p,d[82]])
                else:
                    if s[b-2][a-1] != 1:
                        p = f([p,d[63]])
                    elif s[b-2][a-1] == 1:
                        if s[b-1][a-1] == 1:              #空間が２つ以上ある場合、１つ前の壁を消去
                            p = g(p,d[90])
                            if s[b][a-1] != 1:
                                p = f([p , d[123]])              # 誤って消した壁の部分を元に戻す
                        p = f([p,d[60]])                   #左前の壁を作る
            
                    if s[b-2][a+1] != 1:
                        p = f([p , d[71]])
                    elif s[b-2][a+1] == 1:
                        if s[b-1][a+1] == 1:              #空間が２つ以上ある場合、１つ前の壁を消去
                            p = g(p,d[100])
                            if s[b][a+1] != 1:
                                p = f([p , d[131]])              # 誤って消した壁の部分を元に戻す
                        p = f([p , d[70]])                   #右前の壁を作る
            
                    if cp == 3:                            #フタをする3
                        if s[b-3][a] == 2:    #ゴールの場合 
                            p = f([d[22],p])
                        if s[b-3][a-1] == 1 and s[b-2][a-1] == 1:
                            p = g(p,d[32])
                            if s[b-1][a-1] != 1:
                                p = f([p , d[93]])               # 誤って消した壁の部分を元に戻す
                        if s[b-3][a+1] == 1 and s[b-2][a+1] == 1:
                            p = g(p , d[42])
                            if s[b-1][a+1] != 1:
                                p = f([p , d[101]])               # 誤って消した壁の部分を元に戻す
                        p = f([p,d[52]])
    
                    else:
                        if s[b-3][a-1] != 1:
                            p = f([p,d[33]])
                        elif s[b-3][a-1] == 1:
                            if s[b-2][a-1] == 1:              #空間が２つ以上ある場合、１つ前の壁を消去
                                p = g(p,d[60])
                                if s[b-1][a-1] != 1:
                                    p = f([p , d[93]])                # 誤って消した壁の部分を元に戻す
                            p = f([p,d[30]])                   #左前の壁を作る
            
                        if s[b-3][a+1] != 1:
                            p = f([p , d[41]])
                        elif s[b-3][a+1] == 1:
                            if s[b-2][a+1] == 1:              #空間が２つ以上ある場合、１つ前の壁を消去
                                p = g(p,d[70])
                                if s[b-1][a+1] != 1:
                                    p = f([p , d[101]])              # 誤って消した壁の部分を元に戻す
                            p = f([p , d[40]])                   #右前の壁を作る
            
                        if cp == 4:                  #フタをする4
                            if s[b-4][a-1] == 1 and s[b-3][a-1] == 1:
                                p = g(p,d[2])
                                if s[b-2][a-1] != 1:               # 誤って消した壁の部分を元に戻す
                                    p = f([p , d[63]])
                            if s[b-4][a+1] == 1 and s[b-3][a+1] == 1:
                                p = g(p , d[12])
                                if s[b-2][a+1] != 1:
                                    p = f([p , d[71]])              # 誤って消した壁の部分を元に戻す
                            p = f([p,d[22]])
                        else:                     #フタしをない
                            if s[b-4][a-1] == 1 and s[b-3][a-1] ==1 :
                                p = g(p,d[2])
                                if s[b-2][a-1] != 1:
                                    p = f([p , d[63]]) # 誤って消した壁の部分を元に戻す
                                if s[b-1][a-1] != 1:
                                    p = f([p , d[93]]) # 誤って消した壁の部分を元に戻す
                            if s[b-4][a+1] == 1 and s[b-3][a+1] ==1 :
                                p = g(p,d[12])
                                if s[b-2][a+1] != 1:
                                    p = f([p , d[71]]) # 誤って消した壁の部分を元に戻す
                                if s[b-1][a+1] != 1:
                                    p = f([p , d[101]]) # 誤って消した壁の部分を元に戻す
                            
                             
    
    
        return p
    
    # 現在地のマップを返す関数
    def graf(b,a,di):         #st0からウィンドウに表示するための文字列を作成　　
        st = dstage(di)
        g = ""
        for t in range(5):
            for u in range(5):
                if u == 2 and t == 2:    #自分の止まってる場所
                    g += "＠"                #自身は＠とする
                    continue   #下の場合分けを回避
                if b-2+t >= len(st) or a-2+u >= len(st[0]):      #stを方角に合わせた際、リストのインデックスがリスト長をオーバーするから場合分け
                    g+="口"
                else:                   
                    if st[b-2+t][a-2+u] == 1:                 #数字割り当てで場合分け 道のとき
                        if b-2+t < 0 or a-2+u < 0:         #リストのスコープから出た場合は’口’を表示
                            g+= "口"
                        else:
                            g += "　"
                            
                    elif st[b-2+t][a-2+u] != 1:        #数字割り当てで場合分け 道以外のとき
                        if b-2+t < 0 or a-2+u < 0:
                            g+= "口"
                        else:
                            g += "＊"                    #壁は’＊’を表示

                            
            
            g += "\n"
        return g


    
    

    
    
    
    var1 = tkn.StringVar()
    b1 = tkn.Button(root1,text=u'進む',width = 10 , height = 4)
    b2 = tkn.Button(root1,text=u'右を向く',width = 10 , height = 4)
    b3 = tkn.Button(root1,text=u'左を向く',width = 10 , height = 4)
    b4 = tkn.Button(root1,text=u'後ろを向く',width = 10 , height = 4)
    var2 = tkn.StringVar()

    
    lab = tkn.Label(root1,textvariable = var1, bg = "#aaaaaa", font = ('Monaco') , justify = "left" )        #敵と通路のグラフィック
    lab.pack()
    var1.set(root(y,x,dire))
    
    lab2 = tkn.Label(root1 , textvariable = var2 , font = ('Monaco') , justify = "left" )
    
    b2.place(x = 521 , y = 600)
    b3.place(x = 187 , y = 600)
    b1.pack()
    b4.pack()
    lab2.place(x = 625 , y = 20)
           
    dialog1 = tkn.Listbox(root1 , width = 22 , height = 25)          #バトルのダイアログを作る
    dialog1.place(x = 0 , y = 30)
    dialog1.insert(tkn.END ,"こちらはもともとゲーム")
    dialog1.insert(tkn.END ,"を作っていた時に")
    dialog1.insert(tkn.END ,"ゲーム進行のログを表示させ")
    dialog1.insert(tkn.END ,"ていたリストボックスです")
    dialog1.insert(tkn.END ,"このプログラムには、コードなど")
    dialog1.insert(tkn.END ,"の見えない部分を含め、")
    dialog1.insert(tkn.END ,"様々なところにゲーム開発")
    dialog1.insert(tkn.END ,"の名残が残っています。")

    deletebutton = tkn.Button(root1 , text = u'◎' , font = (u'MS 明朝' , 45 , 'italic') )
    deletebutton.place(x = 650 , y = 700)
    
    
    
    var3 = tkn.StringVar()    #マップを表示
    Map = tkn.Label(root1,textvariable = var3 , font = ('Monaco',35 , 'bold') , justify = "left")
    Map.place(x = 600 , y =180)
    var3.set(graf(y,x,dire))
    
    
    
    
    
    
    
    
    def direction(e):                              #現在の方角をvar2に代入

        if dire == 0:
            var2.set("現在の方角:{}".format("北"))
        elif dire == 1:
            var2.set("現在の方角:{}".format("西"))
        elif dire == 2:
            var2.set("現在の方角:{}".format("南"))
        elif dire == 3:
            var2.set("現在の方角:{}".format("東"))
    
    
    direction(1)
    
    
    
    


    
    
    
    
    def susumu(e):      
        global x , y , dire
        if dstage(dire)[y-1][x] == 1:     #目の前が道なら前進(3の時もすでにroot関数によって3は1に変換されているので通路扱い)
            direction(1)
            y -= 1
 

            var1.set(root(y,x,dire))
                
        elif dstage(dire)[y-1][x] == 2:     #目の前がゴールのとき
            x , y , dire = 1 , 1 , 0
            delete(0)
            showMessage("ゴールです！")

            


         
            
        
        var3.set(graf(y,x,dire))
            
            
            
            
            
    def migi(e):
        global x,y
        dcminus1(y,x,dire)
        var1.set(root(y,x,dire))
        direction(1)
        var3.set(graf(y,x,dire))
        
    def hidari(e):
        global x,y
        dcplus1(y,x,dire)
        var1.set(root(y,x,dire))
        direction(1)
        var3.set(graf(y,x,dire))
    
        
        
        
    def back(e):
        global x,y
        dcplus1(y,x,dire)
        dcplus1(y,x,dire)
        var1.set(root(y,x,dire))
        direction(1)
        var3.set(graf(y,x,dire))
        
    
    def delete(e):
        global x , y , dire
        lab.place(x =114514 , y = 8101919)
        b2.place(x =114514 , y = 8101919)
        b3.place(x =114514 , y = 8101919)
        b1.place(x =114514 , y = 8101919)
        b4.place(x =114514 , y = 8101919)
        lab2.place(x =114514 , y = 8101919 )
        dialog1.place(x =114514 , y = 8101919 )
        deletebutton.place(x =114514 , y = 8101919 )
        Map.place(x =114514 , y = 8101919 )

        
    
    
    
    

    
    
    b2.bind("<Button-1>" , migi)
    b3.bind("<Button-1>" , hidari)
    b1.bind("<Button-1>" , susumu)
    b4.bind("<Button-1>" , back)
    deletebutton.bind("<Button-1>" , delete)
    
    
    
    
    
    


    f02.close()
    f12.close()
    f22.close()
    f30.close()
    f32.close()
    f33.close()
    f40.close()
    f41.close()
    f42.close()
    f50.close()
    f51.close()
    f52.close()
    f53.close()
    f60.close()
    f62.close()
    f63.close()
    f70.close()
    f71.close()
    f72.close()
    f80.close()
    f81.close()
    f82.close()
    f83.close()
    f90.close()
    f92.close()
    f93.close()
    f100.close()
    f101.close()
    f102.close()
    f110.close()
    f111.close()
    f112.close()
    f113.close()
    f120.close()
    f123.close()
    f130.close()
    f131.close()
    f140.close()
    f141.close()
    f143.close()
    fn.close()
    glll.close()
    do1.close()
    do2.close()
    i1.close()
    
    
    
    






if __name__ == "__main__":
    root = tkn.Tk()
    root.configure(bg = "#666666")
    root.geometry("800x800")
    root.title(u'試作品')

    
    adv(root)

    root.mainloop()





    

def rec(e):        #ヴィジェットを再生　他の関数内で使う
    lab.pack()
    b2.place(x = 521 , y = 600)
    b3.place(x = 187 , y = 600)
    b1.pack()
    b4.pack()
    lab2.place(x = 625 , y = 20)
    deletebutton.place(x = 650 , y = 700)
    Map.place(x = 600 , y = 180)

def delete(e):
    lab.place(x =114514 , y = 8101919)
    b2.place(x =114514 , y = 8101919)
    b3.place(x =114514 , y = 8101919)
    b1.place(x =114514 , y = 8101919)
    b4.place(x =114514 , y = 8101919)
    lab2.place(x =114514 , y = 8101919)
    deletebutton.place(x=114514,y=8101919)
    Map.place(x =114514 , y = 8101919)
