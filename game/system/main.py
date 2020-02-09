#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import tkinter as tkn
import pickle
from tkinter import ttk
import random as rnd
import datetime

#メインキャラクターのクラス（ステータス）



jlist=["Knight","Witch","Commander","Scientist","Programmer"] 

itembelonging=[]
    
    
weaponlist = []
    
    
    



techlist = []



class Char:
    def __init__(self,name,job,level,wear,exp,money):
        self.n=name
        self.j=job
        self.l=level
        self.w=wear
        self.e=exp
        self.m=money
        self.i =[]
        self.x = 0        #マップ上での位置もここに格納
        self.y = 0
        self.dire = 0
        if self.j == "Knight":
            self.a = self.l**3 + self.l*50 + 900
            self.h = self.l**4 + self.l*410000 + 6000






        elif self.j == "Witch":
            self.a = self.l**3 + self.l*41 + 500
            self.h =self.l**3 + self.l*900000 + 6000
    
    
        elif self.j == "Commander":
            self.a = self.l**3 + self.l*20 + 600
            self.h = self.l**3 + self.l*1000000 +5000
    
    
    
        elif self.j == "Scientist":
            self.a = self.l**3 + self.l*12 +  900
            self.h = self.l**3 + self.l*1200000 + 5000
    
    
        elif self.j == "Programmer":
            self.a = self.l**3 + self.l*30 + 800
            self.h = self.l**3 + self.l*800000 + 4000
        
        if self.n.strip() == "志村" or self.n.strip() == "りりゅう":                     # 汚ったない条件分岐
            self.a = 11451481019191145148101919
            
            self.h = 11451481019191145148101919
            
            






#敵についてのシステム設定
    
    


#class E: 敵の　名前　レベル　アイテム　わざ　敵の種類　攻撃力　Hp
class Enemy:
    f1 = open("../enemy/terrible.txt","r")
    s1 = f1.read()
    f1.close()
    
    f4 = open("../enemy/knight.txt","r")
    s4 = f4.read()
    f4.close()
    
    f9 = open("../enemy/dragon.txt","r")
    s9 = f9.read()
    f9.close()
    
    
    
    
    
    
    enemylist=[]  #この中に入るのはスライム種のみ
    
  
    ee1 = "スライム"
    ee2 = "キングスライム"
    ee3 = "スライム希少種"
    ee4 = "ナイトの亡霊"
    ee9 = "ドラゴン"

    for t in range(25):
        enemylist.append(ee1)
    for t in range(4):
        enemylist.append(ee2)
    for t in range(1):
        enemylist.append(ee3)
        
    List = enemylist

    def __init__(self,name,level,item,tech): 
        

        self.n = name
        self.l = level
        self.i = item #list
        self.t = tech #list
        


        if self.n == Enemy.ee1:  #敵がスライムだった場合
            self.exp = self.l*6 +30
            self.a = self.l**2 +self.l*100+ 200
            self.h = self.l**2 + 100
            self.AA = Enemy.s1

        elif self.n == Enemy.ee2:#敵がキングスライムだった場合
            self.exp = self.l* 12 +300
            self.a = self.l**2 + self.l*300 +  1000
            self.h = self.l**2 + 360         
            self.AA = Enemy.s1

        elif self.n == Enemy.ee3:#敵がスライム希少種だった場合
            self.exp = self.l* 35 +800
            self.a = self.l**3 + self.l*300 +  3000
            self.h = self.l**3 + 1800    
            self.AA = Enemy.s1
            
        elif self.n == Enemy.ee4:#敵が...だった場合
            self.exp = self.l* 35 +800
            self.a = self.l**3 + self.l*300 +  16000
            self.h = self.l**3 + 1800    
            self.AA = Enemy.s4
            
        elif self.n == Enemy.ee9:#敵が...だった場合
            self.exp = self.l* 35 + 800
            self.a = self.l**3 + self.l*300 +  16000
            self.h = self.l**3 + 1800    
            self.AA = Enemy.s9

#敵の能力更新用の関数
def Eupd(name,level,item,tech):
    name.l = level
    name.i = item
    name.t = tech
    














import random as rnd







# Mはメインobject root1はtkinter parent Bはメニューに戻るボタン
def adv(M , root1 , B ):
    
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
    itemill = i1.read()
    
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
     
    x = M.x             #マップのデータをロードする
    y = M.y
    dire = M.dire
 
                                    #0:壁、 1:道、 2:ゴール、 4以上:扉、 3:アイテム
                                        
    variable = open("../var/variable_Stage" , "rb")

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
            encount= rnd.randint(1,13)

            var1.set(root(y,x,dire))
            
            M.x = x     #マップデータの保存
            M.y = y
            M.dire = dire
            # encount = 8かつ一歩進んだ後、目の前が通路のときに敵が出現したとみなす
            
            #前が壁だと敵のイラストが壁にめり込んでしまうので
            if encount == 8 and dstage(dire)[y-1][x] == 1:
                lab2.place(x=114514,y=8101919) # 余計なボタンやラベルを全て取り除く
                b1.place(x=114514,y=8101919)
                b2.place(x=114514,y=8101919)
                b3.place(x=114514,y=8101919)
                b4.place(x=114514,y=8101919)
                deletebutton.place(x=114514,y=8101919)
                Map.place(x =114514 , y = 8101919)
                
                ROOT = root(y,x,dire)
                Ename = Enemy.List[ rnd.randint( 1 , len(Enemy.List) - 1 )]   # 敵のステータスを決定
                if M.l >10:
                    Enn = Enemy( Ename , rnd.randint(M.l-10 , M.l + 10 ) , [ "50G" ] , [] ) 
                else:
                    Enn = Enemy( Ename , rnd.randint(1 , M.l + 10 ) , [ "50G" ] , [] )
                var1.set( f( [ Enn.AA , root(y,x,dire) ] ) )
                battlesys_from_wiz(M , root1 , Enn , dialog1 , var1 , ROOT)
                
                
                
                
                
                
            #encount = 7かつ一歩進んだ後、目の前が通路のときにアイテムが出現したとみなす
            elif encount == 7 and dstage(dire)[y-1][x] == 1: 
                var1.set( f ( [ itemill , root(y,x,dire)] ) )
                b2.place(x =114514 , y = 8101919)    #余計なボタンの処理
                b3.place(x =114514 , y = 8101919)
                b1.place(x =114514 , y = 8101919)
                b4.place(x =114514 , y = 8101919)
                deletebutton.place(x =114514 , y = 8101919 )
                Map.place(x =114514 , y = 8101919)
                def get(e): 
                    itm = ["回復薬" , "70G" , "50EXP"][rnd.randint(0,2)]    #拾うアイテムを代入
                    #showMessage("{}を手に入れた！！".format(itm))
                    dialog1.insert( tkn.END , "{}を手に入れた".format(itm))
                    if itm == "回復薬":
                        M.h += 1000
                        #showMessage("HPが1000回復した。")
                        dialog1.insert( tkn.END , "HPが1000回復した。")
                    elif itm == "70G":
                        M.m += 70
                    elif itm == "50EXP":
                        M.e += 50
                    


                    var1.set(root(y,x,dire))
                    getbutton.place(x = 114514 , y = 8101919)    #ボタンの処理
                    b2.place(x = 521 , y = 600)
                    b3.place(x = 187 , y = 600)
                    b1.pack()
                    b4.pack()
                    deletebutton.place(x = 650 , y = 700)
                    Map.place(x =600 , y = 180)
                    
                getbutton = tkn.Button(root1 , text = u'調べる', width = 10 ,height = 2)
                getbutton.pack()
                getbutton.bind("<Button-1>" , get)
                    
                
                
        elif dstage(dire)[y-1][x] == 2:     #目の前がゴールのとき
            x , y , dire = 1 , 1 , 0
            delete(0)
            M.m+= 300
            M.e+= M.l* 12 + 60
            #showMessage("経験値と所持金が増えた。")
            dialog1.insert(tkn.END , "経験値と所持金が増えた。")
            B.pack()


          
            
        #ここ以降はボスイベント

            
        elif dstage(dire)[y-1][x] == 4:         #目の前がボス   ボスとは何度でも戦える
            lab2.place(x=114514,y=8101919) # 余計なボタンやラベルを全て取り除く
            b1.place(x=114514,y=8101919)
            b2.place(x=114514,y=8101919)
            b3.place(x=114514,y=8101919)
            b4.place(x=114514,y=8101919)
            deletebutton.place(x=114514,y=8101919)
            Map.place(x =114514 , y = 8101919)
            
            Ename = Enemy.ee4   # 敵のステータスを決定
            
            Enn = Enemy( Ename , 25 , [ "50G" ] , [] ) 

            var1.set( Enn.AA)
            
            y -= 1       #前に進む
            
            ROOT = root(y,x,dire)
            
            battlesys_cannot_escape(M , root1 , Enn , dialog1 , var1 , ROOT)    #sdmtr_cannot_escpaeを呼び出しているため逃げれない
            
            
        elif dstage(dire)[y-1][x] == 9:         #目の前がボス
            lab2.place(x=114514,y=8101919) # 余計なボタンやラベルを全て取り除く
            b1.place(x=114514,y=8101919)
            b2.place(x=114514,y=8101919)
            b3.place(x=114514,y=8101919)
            b4.place(x=114514,y=8101919)
            deletebutton.place(x=114514,y=8101919)
            Map.place(x =114514 , y = 8101919)
            
            Ename = Enemy.ee9
            
            Enn = Enemy( Ename , 100 , [ "50G" ] , [] )

            var1.set( Enn.AA)
            
            y -= 1       #前に進む
            
            ROOT = root(y,x,dire)
            
            battlesys_cannot_escape(M , root1 , Enn , dialog1 , var1 , ROOT)    #sdmtr_cannot_escpaeを呼び出しているため逃げれない
            
            
        
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
        M.x = x     #マップデータの保存
        M.y = y
        M.dire = dire
        B.pack()
        
    
    
    
    

    
    
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
    
    
    
    
def rec(e):        #ヴィジェットを再生　
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







# Ennは敵  M は自分 Eは敵のクラス

def command(M ,root , but2 , but3 , vE , vM  , Enn ,  label1 , statuslabel_Enemy , HPlabel_M , dialog , VAR , ROOT ):
    def r(e):   #ローカル
        but3.place(x = 144 , y = 480)
        but2.place(x = 432 , y = 480)
        


    
    
    
    but2.place(x = 114514 , y = 8101919)
    
    but3.place(x = 114514 , y = 8101919)
    
    
    var1 = tkn.StringVar()
    var1.set("チャージ")
    
    point = 0

    but1 = tkn.Button(root , textvariable = var1 , width = 10 , height = 4  )
    but1.pack()
    



















    def event1(e):         #スタート時の関数
        global point
        point = rnd.randint(7,17)
        #showMessage("{}秒間チャージします　\n{}秒たったら攻撃を押してください。".format(point , point))
        dialog.insert( tkn.END ,"{}秒間チャージします　".format(point , point))
        dialog.insert( tkn.END ,"{}秒間チャージ後に攻撃してください。　".format(point , point))
        dialog.insert( tkn.END ,"チャージ時間が近いほど")
        dialog.insert( tkn.END ,"攻撃力が上がります。")

        now1 = datetime.datetime.today()
        def event2(e):              # ストップ時の関数
            now2 = datetime.datetime.today()
            p = float(str(now2 - now1).split(":")[2][0:6])
            
            but1.bind("<Button-1>" , event1)   #攻撃ボタンをチャージボタンに変える
            var1.set("チャージ")

            
            judge = p - point
            
            if judge**2  < 0.01:
                Matk = M.a*10
                Eatk = Enn.a*0.3
                
            elif judge**2  < 0.025:
                Matk = M.a*5
                Eatk = Enn.a*0.5
 
            elif judge**2  < 1:
                Matk = M.a*2
                Eatk = Enn.a*0.75  
                
            elif judge**2  < 2.25:
                Matk = M.a*1.3
                Eatk = Enn.a*0.9

            elif judge**2  < 4:
                Matk = M.a
                Eatk = Enn.a*1.3

            elif judge**2  < 9:
                Matk = M.a**0.8
                Eatk = Enn.a*1.8

            elif judge**2  < 25:
                Matk = M.a*0.4
                Eatk = Enn.a*2.4

            else:
                Matk = M.a*0.15
                Eatk = Enn.a*6  
            
            
            Enn.h -= Matk
            vE.set("{}  Lv:{} \n HP : {} \n ATK : {}".format(Enn.n, Enn.l , Enn.h , Enn.a))
            
            #showMessage("チャージ秒数:{}".format(p))
            dialog.insert(tkn.END , "チャージ秒数:{}".format(p))
            
            #showMessage("敵に{}ダメージ与えた。".format(Matk))
            dialog.insert(tkn.END ,"敵に{}ダメージ与えた。".format(Matk))
            
            if Enn.h <= 0:      #買った場合
            
                
                    
                iplus = Enn.i[ rnd.randint(0,len(Enn.i)-1) ]
                    
                #showMessage("{}を倒した。\n{}を手に入れた。".format(Enn.n , iplus ) )
                dialog.insert(tkn.END ,"{}を倒した。 {}を手に入れた。".format(Enn.n , iplus ) )
                
                label1.place(x = 114514 , y = 8101919)     #余計なヴィジェットを全て削除
                statuslabel_Enemy.place(x = 114514 , y = 8101919) 
                HPlabel_M.place(x = 114514 , y = 8101919)
                but1.place(x = 114514 , y = 8101919) 

                
                if iplus == "50G":  #敵の落し物が５０Gなら　所持金＋５０
                        
                    M.m += 50                       
                    
                    
                #showMessage("経験値が{}上がった。".format(Enn.exp))
                dialog.insert(tkn.END , "経験値が{}上がった。".format(Enn.exp))
                M.e+= Enn.exp
                while M.e >= expfn(M.l):
                    M.l+=1
                    #showMessage("レベルが上った！！")
                    dialog.insert(tkn.END , "レベルが上がった！！")
                    upd( M.j , M.l ,M.w ,M.e , M.m)
                
                dialog.insert(tkn.END , "-----{}-----".format(str(datetime.datetime.now())))

                rec(0)          #敵出現時に取り除いたヴィジェットを再配置する 
                VAR.set(ROOT)      #敵を消す
                    

                    
                    
                    
                    
                    
                
                
            else:     
                M.h -= Eatk
                vM.set("HP:{}".format(M.h))
                #showMessage("{}ダメージ受けた".format(Eatk))
                dialog.insert(tkn.END ,"{}ダメージ受けた".format(Eatk))
                
                 
                if M.h <= 0:       #負けた場合
                    #showMessage("HPが０になった") 
                    #showMessage("所持金が少し減った")
                    #showMessage("Game Over")
                    dialog.insert(tkn.END ,"HPが０になった")
                    dialog.insert(tkn.END ,"所持金が少し減った")
                    dialog.insert(tkn.END ,"Game Over")
                    if M.m >= 150:
                        M.m -= 150
                    else:
                        M.m = 0
                    
                    label1.place(x = 114514 , y = 8101919)      #ヴィジェットの削除
                    statuslabel_Enemy.place(x = 114514 , y = 8101919) 
                    HPlabel_M.place(x = 114514 , y = 8101919)
                    but1.place(x = 114514 , y = 8101919)           
                    dialog.place(x = 114514 , y = 8101919)
                    
                       
                    delete(0)       #残ったヴィジェットを全て削除する

                    #負けた時点のデータをセーブする
                    save(M)  
                    var2 = tkn.StringVar()
                    lab2 = tkn.Label(root , textvariable = var2)
                    lab2.pack()
                    var2.set("    GAME  OVER !!!\n名前：{}\n   職業：{}\n   レベル：{}\n   装備：{}\n   経験値：{}\n    所持金:{}G \n \n アイテム：{} \n\n\n\n  データをセーブして終了します。".format(M.n , M.j , M.l , M.w ,  M.e , M.m , M.i))
    
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            

            
            
            
            
        but1.bind("<Button-1>" , event2)   #チャージボタンを攻撃ボタンに変える
        var1.set("攻撃")
        
    
    but1.bind("<Button-1>" , event1)   
    
    
    
    
    
    
    
    

    

    










#データをセーブ

import os
def save(M):
    os.remove("../savedata/prof.txt")
    

    
    f2 = open("../savedata/prof.txt","w")
    
    f2.write( "{}\n".format(M.n) )
    f2.write( "{}\n".format(M.j) )
    f2.write( "{}\n".format(M.l) )
    f2.write( "{}\n".format(M.w) )
    f2.write("{}\n".format(M.e) )
    f2.write( "{}\n".format(M.m) )
    for j in range(len(M.i)):
        f2.write("{}  ".format(M.i[j]))
    f2.write("\n")
    f2.write("{}\n".format(M.x))         #マップ上のデータを保存する
    f2.write("{}\n".format(M.y))
    f2.write("{}\n".format(M.dire))
    f2.flush()
    f2.close()




#バトル中に敵から逃げるシステム

           
        
def escape(root ,label, but2 , but3 , dialog , VAR , ROOT):

    def rec_in_escape(e):    #下３つ　ローカル
        label.pack()
        but3.place(x = 192 , y = 480)
        delete_in_escape(e)
        but2.place(x = 492 , y = 480)
        lab2.place(x = 114514 , y = 8101919)
        
        
        
    def delete_in_escape(e):
        but4.place(x = 114514 , y = 8101919)
        but5.place(x = 114514 , y = 8101919)
        but2.place(x = 114514 , y = 8101919)
        
        
    def esdel(e):
        but2.place(x = 114514 , y = 8101919)
        but3.place(x = 114514 , y = 8101919)
        delete_in_escape(e)
        lab2.place(x = 114514 , y = 8101919)
        #showMessage("敵から逃げた")
        dialog.insert(tkn.END , "敵から逃げた。")
        
        rec(0)          #敵出現時に取り除いたヴィジェットを再配置する 
        VAR.set(ROOT)      #敵を消す

        
    label.place(x = 114514 , y = 8101919)

    but2.place(x = 114514 , y = 8101919)
    
    but3.place(x = 114514 , y = 8101919)
    


            
            
    lab2 = tkn.Label(root, text=u"本当に逃げますか？",width = 20)
    lab2.pack()
    but4 =  tkn.Button(root, text=u"はい",width = 10  , height = 4)
    but5 =  tkn.Button(root, text=u"いいえ",width = 10 , height = 4)
    but4.pack()
    but5.pack()
    but4.bind("<Button-1>" ,  esdel)
    but5.bind("<Button-1>" , rec_in_escape )







#スクロールバーを使ってますが、開発者はその文法をあまり理解していません、、、
#メインobject:M　tkinter parent:root ,EnemyClass：E　メニューボタン：Bを入れると　敵選択画面に切り替わる



def selectbattle(M , root , E , B):


    def button_click():
        battlecheck(0)
    
    def battlecheck(e):
        try:
            for i in lb.curselection():
                info = lb.get(i).split(":Lv")
            Enn = E(info[0] , int(info[1]) , [ "50G" ] , [] )     #敵を定義
            frame1.place(x = 114514 , y = 8101919)
            
            button.place(x = 114514 , y = 8101919)
            
            
            yes = tkn.Button(root ,text=u'はい' , width = 14 ,height = 3)
            
            no = tkn.Button(root , text=u'いいえ' , width = 14 ,height = 3)   
            
            kakuninn = tkn.Label(root , text = u'{}:Lv{}と戦いますか？'.format(Enn.n , Enn.l)) 
            
            yes.place( x = 310 , y = 450)
            
            no.place( x = 310 , y = 490)
            
            kakuninn.place(x = 310 , y = 390)
            
            def Y(e):
                
                yes.place(x = 114514 , y = 8101919)
                no.place(x = 114514 , y = 8101919)
                kakuninn.place(x = 114514 , y = 8101919)
                frame1.place(x = 114514 , y = 8101919)
                label.place(x = 114514 , y = 8101919)
                label2.place(x = 114514 , y = 8101919)
                battle(M , root , E , Enn , B)
                
                
            def N(e):
                frame1.place(x = 0 , y = 30)
                button.place(x = 290 , y = 600)
                yes.place(x = 114514 , y = 8101919)
                no.place(x = 114514 , y = 8101919)
                kakuninn.place(x = 114514 , y = 8101919)            
                    
            yes.bind("<Button-1>" , Y)
            no.bind("<Button-1>" , N)
        except:
            pass

    

    
    # Frame
    frame1 = tkn.ttk.Frame(root, padding=10)
    frame1.place(x = 0 , y = 30)
    
    # Listbox
    Enemies = [] 
    for t in range(M.l + 6):                            #  敵の名前とレベルをリストに追加
        En = E.List[ rnd.randint( 1 , len(E.List) - 1 )]
        if M.l >10:
            El = rnd.randint(M.l -10  , M.l + 10 ) 
        else:
            El = rnd.randint(1 , M.l + 10 )
        Enemies.append("{}:Lv{}".format( En , El ))
        
    v1 = tkn.StringVar(value=Enemies )
    lb = tkn.Listbox(frame1, listvariable=v1,height=30)     
    lb.grid(row=0, column=0)
    
    # Scrollbar
    scrollbar = tkn.ttk.Scrollbar(
        frame1, 
        orient=tkn.VERTICAL, 
        command=lb.yview)
    lb['yscrollcommand'] = scrollbar.set
    scrollbar.grid(row=0,column=1,sticky=(tkn.N,tkn.S))
    
    #Button
    button1 = tkn.ttk.Button(frame1, text='OK', command=button_click)
    button1.grid(row=1, column=0, columnspan=10)
    
    #ここまでがスクロールバーのコード
    
    f = open("../ill/lady.txt" , "r")
    lady = f.read()
    f.close()
    
    f2 = open("../ill/lady_comment.txt" , "r")
    lady_comment = f2.read()
    f2.close()
    label = tkn.Label(root , text = lady )
    label.place(x = 400 , y = 100)
    label2 = tkn.Label(root , text = lady_comment )
    label2.place(x = 220 , y = 100)
    

    
    def back():
        frame1.place(x = 114514 , y = 8101919)
        label.place(x = 114514 , y = 8101919)
        label2.place(x = 114514 , y = 8101919)
        button.place(x = 114514 , y = 8101919)
        B.pack()
        
    button=tkn.Button(root , text = u'exit' , width = 20 , height = 3 , command = back)
    button.place(x = 290 , y = 600)    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    




def battlesys_cannot_escape(M , root , Enn , dialog , VAR , ROOT ):               #dialogはListbox
    global  M_HP , En_ST , escapebutton , battlebutton             



    varE = tkn.StringVar()
    varE.set("{}  Lv:{} \n HP : {} \n ATK : {}".format(Enn.n, Enn.l , Enn.h , Enn.a))
        
        
    En_ST = tkn.Label(root , textvariable = varE , width = 24 , bg = "#cc2100")

        
    varM = tkn.StringVar()
    varM.set("{}\nLV:{}\nHP:{}\nATK:{}\nexp:{}".format(M.n , M.l , M.h , M.a , M.e))
        
    M_HP = tkn.Label(root , textvariable = varM, width = 32,bg = "#117643")
       

    

    #showMessage("{}:Lv{}が現れた!!".format(Enn.n , Enn.l))


    dialog.insert(tkn.END , "{}が現れた。".format(Enn.n))
        

    
    
    
    
    
    
    
       
    def act1():    #逃げるボタン
        escape(root ,lab1, escapebutton , battlebutton , dialog , VAR , ROOT)
        
    def act2():        #戦うボタン

        escapebutton.place(x = 114514 , y = 8101919)
        En_ST.place( x = 208.5 , y = 57)
        M_HP.place( x = 40 , y = 460 )
        command(M ,  root , escapebutton , battlebutton , varE , varM , Enn  , lab1 , En_ST , M_HP , dialog , VAR , ROOT)
        lab1.place(x = 114514 , y = 8101919)
        
        
        
        
        
    lab1 = tkn.Label( root , text = u"あなたはどうする？" , foreground='#ff0000', width = 16 )
                 
    lab1.pack()
        
    escapebutton = tkn.Button(root , text = u"逃げる" , width = 10 , height = 4 , command = act1)
    
    escapebutton.config(state = "disable")

    escapebutton.place(x = 492 , y = 480)


    battlebutton = tkn.Button(root , text = u"戦う" , width = 10 , height = 4 , command = act2)

    battlebutton.place(x = 192 , y = 480)

    




 #メインのclass:Charに属する変数M と tkinterのparent と敵の object;Enn を入れると敵が出現した画面に切り替わる(Eはクラス)


def battlesys_from_wiz(M , root , Enn , dialog , VAR , ROOT ):               #dialogはListbox
    global  M_HP , En_ST , escapebutton , battlebutton             
    


    varE = tkn.StringVar()
    varE.set("{}  Lv:{} \n HP : {} \n ATK : {}".format(Enn.n, Enn.l , Enn.h , Enn.a))
        
        
    En_ST = tkn.Label(root , textvariable = varE , width = 24 , bg = "#cc2100")

        
    varM = tkn.StringVar()
    varM.set("{}\nLV:{}\nHP:{}\nATK:{}\nexp:{}".format(M.n , M.l , M.h , M.a , M.e))
        
    M_HP = tkn.Label(root , textvariable = varM, width = 32,bg = "#117643")
       

    

    #showMessage("{}:Lv{}が現れた!!".format(Enn.n , Enn.l))


    dialog.insert(tkn.END , "{}が現れた。".format(Enn.n))

    
    
    
       
    def act1(w):    #逃げるボタン
        escape(root ,lab1, escapebutton , battlebutton , dialog , VAR , ROOT)
        
    def act2(w):        #戦うボタン

        escapebutton.place(x = 114514 , y = 8101919)
        En_ST.place( x = 208.5 , y = 57)
        M_HP.place( x = 40 , y = 460 )
        command(M ,  root , escapebutton , battlebutton , varE , varM , Enn  , lab1 , En_ST , M_HP , dialog , VAR , ROOT)
        lab1.place(x = 114514 , y = 8101919)
        
        
        
        
    lab1 = tkn.Label( root , text = u"あなたはどうする？" , foreground='#ff0000', width = 16 )
                 
    lab1.pack()
        
    escapebutton = tkn.Button(root , text = u"逃げる" , width = 10 , height = 4)

    escapebutton.place(x = 492 , y = 480)


    battlebutton = tkn.Button(root , text = u"戦う" , width = 10 , height = 4)

    battlebutton.place(x = 192 , y = 480)

    escapebutton.bind("<Button-1>" , act1 )

    battlebutton.bind("<Button-1>" , act2)



    






def upd(a1,a2,a3,a4,a5):#自分の能力更新用の関数
    global M ,charname
    M = Char(charname,a1,a2,a3,a4,a5)


def showMessage(text):
    tkm.showinfo('メッセージ',text)
    
    
#このコード内では メインのオブジェクトMを宣言し、そのデータをファイルvariable_Mに渡す
#もしもprof.txtにセーブデータが存在するなら、そこからデータを読み取ってMを与える
def charmake(root , B , dialog):

    
    
    
    global  charname , M           
    #グローバルに指定することで関数charmakeの外でも使えるようになった
    #例えば他のコードにChar_makeをインポートした時も　Mなどのグローバル変数は使用できる
    variable = open("../var/variable_Main_M" , "wb")
    
    
    
    
    
    b1 = tkn.Button(root , text = u'新しいデータで始める' , width = 28 ,height = 4)
    b2 = tkn.Button(root , text = u'そのままのデータで始める', width = 28 ,height = 4)
    name = tkn.Label(root , text = u'A Game Of Mine', font = (u'MS 明朝' , 45 , 'italic') , bg = "red")
    name.place(x = 270 , y = 140)
    
    b1.place(x = 290 , y = 400)
    b2.place(x = 290, y = 470)
    var1 = tkn.StringVar()
    var1.set("データをロードします")
    
    lab1 = tkn.Label(root , textvariable = var1 , width = 30)
    lab1.place(x = 290 , y = 300)
    
    def delete(e):
        b1.place(x = 114514 , y = 8101919)
        b2.place(x = 114514 , y = 8101919)
        
        asdfghjkl = open("../savedata/prof.txt","w")
        asdfghjkl.write("New data")
        asdfghjkl.flush()
        asdfghjkl.close()
        
        entry = tkn.Entry(root , width = 18)
        entry.place(x = 386 , y = 400)
        var1.set("名前を入力してください")
        b3 = tkn.Button(root , text = u'決定' , width = 10 , height = 4)
        b3.place(x = 386 , y = 450)
        def getentry(e):
            global  charname , M 
            if entry.get()=="":
                charname = "Guest114514"
            else:
                charname = entry.get()           
            entry.place(x = 114514 , y = 8101919)
            b3.place(x = 114514 , y = 8101919)
            lab1.place(x = 114514 , y = 8101919)
            name.place(x = 114514 , y = 8101919)
            upd(jlist[rnd.randint(0,4)],1,"平民の服",0,0)
            #showMessage("あなたのプロフィールは\n{}\n{}\nLv:{}\nです".format(M.n , M.j , M.l))
            dialog.insert(tkn.END , "あなたのプロフィールは")
            dialog.insert(tkn.END , "\n{}\n{}\nLv:{}\nです".format(M.n , M.j , M.l))
            M.i = []
            M.x = 1
            M.y = 1
            M.dire = 0
            
            B.pack()
            #メインオブジェクトMをファイル：variable_Mに格納してどのコードでも使えるようにした
            pickle.dump(M , variable)

            
            variable.close()
            
        b3.bind("<Button-1>" , getentry)
        
        
    def check(e):
        b1.place(x = 114514 , y = 8101919)
        b2.place(x = 114514 , y = 8101919)
        
        yes = tkn.Button(root ,text=u'はい' , width = 14 ,height = 3)
        no = tkn.Button(root , text=u'いいえ' , width = 14 ,height = 3)   
        kakuninn = tkn.Label(root , text = u'以前のデータは全て消去されます。\n本当に新しいデータで始めますか？') 
        yes.place( x = 290 , y = 450)
        no.place( x = 290 , y = 490)
        kakuninn.place(x = 290 , y = 390)
        def Y(e):
            yes.place(x = 114514 , y = 8101919)
            no.place(x = 114514 , y = 8101919)
            kakuninn.place(x = 114514 , y = 8101919)
            delete(0)
            
        def N(e):
            b1.place(x = 290 , y = 400)
            b2.place(x = 290, y = 470)
            yes.place(x = 114514 , y = 8101919)
            no.place(x = 114514 , y = 8101919)
            kakuninn.place(x = 114514 , y = 8101919)            
              
        yes.bind("<Button-1>" , Y)
        no.bind("<Button-1>" , N)
        
        
        
    b1.bind("<Button-1>" , check)
    
    
    
    
    
    
    
    def loadsave(e):
        global   charname , M 
        try:
            b1.place(x = 114514 , y = 8101919)
            b2.place(x = 114514 , y = 8101919) 
            f1 = open("../savedata/prof.txt","r")                                 
            listf = f1.read().split("\n")
            M = Char(listf[0] , listf[1] , int(listf[2]) , listf[3], int(listf[4]), int(listf[5]) )
            charname=listf[0]
            f1.flush()
            f1.close()
            name.place(x = 114514 , y = 8101919)
            try:
                M.i = listf[6].strip().split() # ファイルの str オブジェクトをリスト化した

            except:
                M.i = []
            M.x = int(listf[7].strip())        #マップデータを読み取る
            M.y = int(listf[8].strip())
            M.dire = int(listf[9].strip())
            
            
            
            B.pack() 
            #メインオブジェクトMをファイル：variable_Mに格納してどのコードでも使えるようにした              
            pickle.dump(M , variable)
            variable.close()
            #showMessage("\n{}\n{}\nLv:{}\nとして始めます。".format(M.n , M.j , M.l))
            dialog.insert(tkn.END , "\n{}\n{}\nLv:{}\nとして始めます。".format(M.n , M.j , M.l))

            lab1.place(x = 114514 , y = 8101919)
            
            
            
                
        except:
            b1.place(x = 114514 , y = 8101919)
            b2.place(x = 114514 , y = 8101919) 
            var1.set("情報を読み込めませんでした。\n自動でキャラクターの設定を行います。\n\n        名前を入力してください")
            
        
            asdfghjkl = open("../savedata/prof.txt","w")
            asdfghjkl.write("New data")
            asdfghjkl.flush()
            asdfghjkl.close()
            
            entry = tkn.Entry(root , width = 18)
            entry.place(x = 386 , y = 400)
            
            b3 = tkn.Button(root , text = u'決定' , width = 10 , height = 4)
            b3.place(x = 386 , y = 450)
            
            def getentry(e):
                global M , charname 
                
                if entry.get()=="":
                    charname = "Guest114514"
                else:
                    charname = entry.get()        
                entry.place(x = 114514 , y = 8101919)
                b3.place(x = 114514 , y = 8101919)
                lab1.place(x = 114514 , y = 8101919)
                name.place(x = 114514 , y = 8101919)
                upd(jlist[rnd.randint(0,4)],1,"平民の服",0,0)
                #showMessage("あなたのプロフィールは\n{}\n{}\nLv:{}\nです".format(M.n , M.j , M.l))
                dialog.insert(tkn.END , "あなたのプロフィールは")
                dialog.insert(tkn.END , "\n{}\n{}\nLv:{}\nです".format(M.n , M.j , M.l))
                M.i = []
                M.x = 1
                M.y = 1
                M.dire = 0
                
                
                B.pack()
                #メインオブジェクトMをファイル：variable_Mに格納してどのコードでも使えるようにした
                pickle.dump(M , variable)

                variable.close()
                
                
            b3.bind("<Button-1>" , getentry)
        
    b2.bind("<Button-1>" , loadsave)
    
    
    
    
    
    
    


#経験値計算用の関数
def expfn(l):
    s = 0
    for i in range(1,l+1):
        s+=200*i
    return s



    

    
    
    
    
    
def battle(M ,root , E ,  Enn , B ):   

        

    
    
    
    
    
    dialog = tkn.Listbox(root , width = 22 , height = 25)  
    dialog.place(x = 0 , y = 30)
    
    var1 = tkn.StringVar()
    var1.set("チャージ")
    
    var2 = tkn.StringVar()
    
    var3 = tkn.StringVar()
    var3.set("HP:{}".format(M.h))
    
    
    var4 = tkn.StringVar()
    var4.set("{}  Lv:{} \n HP : {} \n ATK : {}".format(Enn.n, Enn.l , Enn.h , Enn.a))
    
            
    label = tkn.Label(root , textvariable = var2, font = ('Monaco') , justify = "left" )
    label.pack()
    
    
    M_HP = tkn.Label(root , textvariable = var3, width = 32,bg = "#117643")
    
    En_ST = tkn.Label(root , textvariable = var4 , width = 24 , bg = "#cc2100")
    
    
    En_ST.place( x = 208.5 , y = 57)
    M_HP.place( x = 40 , y = 460 )
    
    point = 0

    but1 = tkn.Button(root , textvariable = var1 , width = 10 , height = 4  )
    but1.pack()

    var2.set(Enn.AA)

















    def event1(e):         #スタート時の関数
        global point
        point = rnd.randint(7,17)
        #showMessage("{}秒間チャージします　\n{}秒たったら攻撃を押してください。".format(point , point))
        dialog.insert( tkn.END , "{}秒間チャージ　".format(point))


        now1 = datetime.datetime.today()
        def event2(e):              # ストップ時の関数
            now2 = datetime.datetime.today()
            p = float(str(now2 - now1).split(":")[2][0:6])
            
            but1.bind("<Button-1>" , event1)   #攻撃ボタンをチャージボタンに変える
            var1.set("チャージ")

            
            judge = p - point
            
            if judge**2  < 0.01:
                Matk = M.a*10
                Eatk = Enn.a*0.3
                
            elif judge**2  < 0.025:
                Matk = M.a*5
                Eatk = Enn.a*0.5
 
            elif judge**2  < 1:
                Matk = M.a*2
                Eatk = Enn.a*0.75  
                
            elif judge**2  < 2.25:
                Matk = M.a*1.3
                Eatk = Enn.a*0.9

            elif judge**2  < 4:
                Matk = M.a
                Eatk = Enn.a*1.3

            elif judge**2  < 9:
                Matk = M.a**0.8
                Eatk = Enn.a*1.8

            elif judge**2  < 25:
                Matk = M.a*0.4
                Eatk = Enn.a*2.4

            else:
                Matk = M.a*0.15
                Eatk = Enn.a*6  
            
            
            Enn.h -= Matk
            var4.set("{}  Lv:{} \n HP : {} \n ATK : {}".format(Enn.n, Enn.l , Enn.h , Enn.a))
            
            #showMessage("チャージ秒数:{}".format(p))
            dialog.insert( tkn.END ,"{}秒間チャージします　".format(point , point))
            dialog.insert( tkn.END ,"{}秒間チャージ後に攻撃してください。　".format(point , point))
            dialog.insert( tkn.END ,"チャージ時間が近いほど")
            dialog.insert( tkn.END ,"攻撃力が上がります。")
            
            #showMessage("敵に{}ダメージ与えた。".format(Matk))
            dialog.insert(tkn.END ,"敵に{}ダメージ与えた。".format(Matk))
            
            if Enn.h <= 0:      #勝った場合
            
                
                    
                iplus = Enn.i[ rnd.randint(0,len(Enn.i)-1) ]
                    
                #showMessage("{}を倒した。\n{}を手に入れた。".format(Enn.n , iplus ) )
                dialog.insert(tkn.END ,"{}を倒した。 {}を手に入れた。".format(Enn.n , iplus ) )
                
                 #余計なヴィジェットを全て削除
                En_ST.place(x = 114514 , y = 8101919) 
                M_HP.place(x = 114514 , y = 8101919)
                but1.place(x = 114514 , y = 8101919) 

                
                if iplus == "50G":  #敵の落し物が５０Gなら　所持金＋５０
                        
                    M.m += 50                       
                    
                    
                #showMessage("経験値が{}上がった。".format(Enn.exp))
                dialog.insert(tkn.END , "経験値が{}上がった。".format(Enn.exp))
                M.e+= Enn.exp
                while M.e >= expfn(M.l):
                    M.l+=1
                    #showMessage("レベルが上った！！")
                    dialog.insert(tkn.END , "レベルが上がった！！")
                    upd( M.j , M.l ,M.w ,M.e , M.m)
                
                dialog.insert(tkn.END , "-----{}-----".format(str(datetime.datetime.now())))
                label.place(x = 114514 , y = 8101919)
                dialog.place(x = 114514 , y = 8101919)
                B.pack()
                    

                    
                    
                    
                    
                    
                
                
            else:     
                M.h -= Eatk
                var3.set("HP:{}".format(M.h))
                #showMessage("{}ダメージ受けた".format(Eatk))
                dialog.insert(tkn.END ,"{}ダメージ受けた".format(Eatk))
                
                 
                if M.h <= 0:       #負けた場合
                    #showMessage("HPが０になった") 
                    #showMessage("所持金が少し減った")
                    #showMessage("Game Over")
                    dialog.insert(tkn.END ,"HPが０になった")
                    dialog.insert(tkn.END ,"所持金が少し減った")
                    dialog.insert(tkn.END ,"Game Over")
                    
                    if M.m >= 150:
                        M.m -= 150
                    else:
                        M.m = 0
                    
                    #ヴィジェットの削除
                    En_ST.place(x = 114514 , y = 8101919) 
                    M_HP.place(x = 114514 , y = 8101919)
                    but1.place(x = 114514 , y = 8101919)           
                    label.place(x = 114514 , y = 8101919)
                    
                       

                    #負けた時点のデータをセーブする
                    save(M)  
                    var2 = tkn.StringVar()
                    lab2 = tkn.Label(root , textvariable = var2)
                    lab2.pack()
                    var2.set("   GAME  OVER !!!\n名前：{}\n   職業：{}\n   レベル：{}\n   装備：{}\n   経験値：{}\n    所持金:{}G \n \n アイテム：{} \n\n\n\n  データをセーブして終了します。".format(M.n , M.j , M.l , M.w ,  M.e , M.m , M.i))

            
            
        but1.bind("<Button-1>" , event2)   #チャージボタンを攻撃ボタンに変える
        var1.set("攻撃")
        
    
    but1.bind("<Button-1>" , event1)   
    

    















if __name__ == "__main__":
        



    #rootのウィジェットは大文字で書かれたオブジェクトはグローバル、小文字ならローカルとする
    #他のモジュールの関数には基本的にメインキャラobject の　Mと　tkinter parentのroot メニューボタンのBを渡す　（オプションでEm.Enemy)
    root = tkn.Tk()
    root.configure(bg = "#666666")
    root.geometry("800x800")
    root.title(u'試作品')
    VAR = tkn.StringVar()
    VAR.set("メニュー")

    B = tkn.Button(root , text = u'メニュー' , width = 20 , height = 3)

    LAB = tkn.Label(root , textvariable = VAR , width = 20 , height = 3)

    B1 = tkn.Button(root , text = u'マップ' , width = 20 , height = 3)

    B2 = tkn.Button(root , text = u'闘技場' , width = 20 , height = 3)


    B3 = tkn.Button(root , text = u'Exit' , width = 20 , height = 3)



    dialog = tkn.Listbox(root , width = 22 , height = 25)
    dialog.place(x = 0 , y = 30)

    charmake(root , B , dialog)



    variable = open("../var/variable_Main_M" , "rb")


    def menu(e):       #ここでメインのobject:Mが定義された
        #Cm.charmakeでMが定義された後にファイル：variableからMを読み込む
        global M 
        M = pickle.load(variable)
        LAB.pack()
        B.place(x = 114514 , y = 8101919)
        B1.pack()
        B2.pack()
        B3.pack()
        B.bind("<Button-1>" , menu2)#Bの仕様変更

        
        
        
        
        
        
        
        
    def menu2(e):       #メニューに移動
        LAB.pack()
        B.place(x = 114514 , y = 8101919)
        B1.pack()
        B2.pack()
        B3.pack()
        dialog.place(x = 0 , y = 30)
        
        
        
        
        
    def battle_menu(e):          #sellectbattle => battle
        B1.place(x = 114514 , y = 8101919)
        B2.place(x = 114514 , y = 8101919)
        B3.place(x = 114514 , y = 8101919)    
        LAB.place(x = 114514 , y = 8101919)
        dialog.place(x = 114514 , y = 8101919)
        selectbattle(M , root , Enemy , B)

        

        
        


    def dangeon(e):              #こっちはwiz => sdmtr => escape or battle
        B1.place(x = 114514 , y = 8101919)
        B2.place(x = 114514 , y = 8101919)
        B3.place(x = 114514 , y = 8101919)    
        dialog.place(x = 114514 , y = 8101919)
        LAB.place(x = 114514 , y = 8101919)
        adv(M , root , B )    
        
        
    def savedata(e):
        save(M)
        B1.place(x = 114514 , y = 8101919)
        B2.place(x = 114514 , y = 8101919)
        B3.place(x = 114514 , y = 8101919)
        dialog.place(x = 114514 , y = 8101919)
        LAB.place(x = 114514 , y = 8101919)
        var2 = tkn.StringVar()
        lab2 = tkn.Label(root , textvariable = var2)
        lab2.pack()

        var2.set("\n名前：{}\n   職業：{}\n   レベル：{}\n   装備：{}\n   経験値：{}\n    所持金:{}G \n \n アイテム：{} \n\n\n\n  データをセーブして終了します。".format(M.n , M.j , M.l , M.w ,  M.e , M.m , M.i))
        
        
        
        
    def check(e):
        dialog.place(x = 114514 , y = 8101919)
        LAB.place(x = 114514 , y = 8101919)
        B1.place(x = 114514 , y = 8101919)
        B2.place(x = 114514 , y = 8101919)
        B3.place(x = 114514 , y = 8101919)
        yes = tkn.Button(root ,text=u'はい' , width = 14 ,height = 3)
        no = tkn.Button(root , text=u'いいえ' , width = 14 ,height = 3)   
        kakuninn = tkn.Label(root , text = u'ゲームを終了しますか？') 
        yes.place( x = 290 , y = 450)
        no.place( x = 290 , y = 490)
        kakuninn.place(x = 290 , y = 390)
        def Y(e):
            yes.place(x = 114514 , y = 8101919)
            no.place(x = 114514 , y = 8101919)
            kakuninn.place(x = 114514 , y = 8101919)
            savedata(0)

        def N(e):
            LAB.pack()
            B1.pack()
            B2.pack()
            B3.pack()
            yes.place(x = 114514 , y = 8101919)
            no.place(x = 114514 , y = 8101919)
            kakuninn.place(x = 114514 , y = 8101919)            

        yes.bind("<Button-1>" , Y)
        no.bind("<Button-1>" , N)

                
            




        
        
    B.bind("<Button-1>" , menu)       #メニューに行くボタン


    B1.bind("<Button-1>" , dangeon)      #バトルが始まるボタン

        
    B2.bind("<Button-1>" , battle_menu)     #ダンジョンに入るボタン
        
        
    B3.bind("<Button-1>" , check)       #セーブするボタン



        
    root.mainloop()

    variable.close()
