"""
こんちゃ、Twitter ID:NokonoKotlin です！
このコードはmacOSでの動作のみ確認しております。
他OSではファイルパスの書き方が異なる場合がありますので、その場合は
VScodeなどの文字列置換機能を用いるなどで各自解決してください。

バグの原因でもありますのでファイル読み込みの部分だけコピーして置き換えることを推奨します。

"""


import random
from collections import deque
import sys,time
import os
import tkinter

#グローバル変数初期化
# #スタート地点と方向を設定
     
x = 1        #最初のx座標、y座標、向きdire
y = 1
dire = 0     #方向を定義 0:上、１：左、２：下、３：右
INF = 100000000 #プログラム内で使われる値の目標最大値

#0:壁、 1:道、 2:ゴール、 4以上:扉、 3:アイテム

stage_dire0 = []   #st0は 0 , 1 , 2 .... でかかれた二次元配列
stage_dire1 = []   #st2: st0を上から見たもの
stage_dire3 = []   #st3: st0を左から見たもの               
stage_dire2 = []   #st1: st0を右から見たもの 




grafic_data = nil = door1 = door2 = goal = "init" 


def file_open():
    global grafic_data , nil , door1 ,door2 ,goal

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
    grafic_data = {2:f02.read(),12:f12.read(),22:f22.read(),30:f30.read(),32:f32.read(),33:f33.read(),40:f40.read(),41:f41.read(),42:f42.read(),50:f50.read(),51:f51.read(),52:f52.read(),53:f53.read(),60:f60.read(),62:f62.read(),63:f63.read(),70:f70.read(),71:f71.read(),72:f72.read(),80:f80.read(),81:f81.read(),82:f82.read(),83:f83.read(),90:f90.read(),92:f92.read(),93:f93.read(),100:f100.read(),101:f101.read(),102:f102.read(),110:f110.read(),111:f111.read(),112:f112.read(),113:f113.read(),120:f120.read(),123:f123.read(),130:f130.read(),131:f131.read(),140:f140.read(),141:f141.read(),143:f143.read()}
    nil = fn.read()
    goal = glll.read()
    do1 = open("./kabe/door1.txt" , "r")
    door1 = do1.read()
    do2 = open("./kabe/door2.txt" , "r")
    door2 = do2.read()
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
    



def BFS(map):
    #mapが左上から右下まで到達可能かどうかを返す
    #到達可能なら最短距離を、不可能ならINFを返す
    #mapは0(壁)と1(通路)からなる２重リストで、外側は全て０である
    #幅優先探索
    r = len(map)
    c = len(map[0])
    sx = 1  #スタートは(1,1)
    sy = 1
    gx = len(map[0])-2
    gy = len(map)-2 #ゴールは右下
    maze = []
    """for t in range(len(map)):
        print(map[t])"""
    for i in range(r):
        pushlist = []
        for j in range(c):
            pushlist.append('?')
        maze.append(pushlist)

    #print(r)
    #print(c)
    for i in range(r):
        for j in range(c):
            if(map[i][j] == 0):
                maze[i][j] = '#'
            else:
                maze[i][j] = '.'

    """for t in maze:
        print(t)"""

    # yが下方向，xが右方向
    
    memo = [[INF] * c for _ in range(r)]

    dy = [0, 1, 0, -1]
    dx = [1, 0, -1, 0]

    que = deque([])
    que.append((sy, sx))
    memo[sy][sx] = 0

    while que:
        
        p = que.popleft()

        if p[0] == gy and p[1] == gx:
            break

        for i in range(4):
            ny = p[0] + dy[i]
            nx = p[1] + dx[i]

            if 0 <= ny < r and 0 <= nx < c and maze[ny][nx] != "#" and memo[ny][nx] == INF:
                que.append((ny, nx))
                memo[ny][nx] = memo[p[0]][p[1]] + 1

    return memo[gy][gx]






class Stages():
    global  stage_dire0 ,stage_dire1 , stage_dire3,stage_dire2      
                  
    def __init__(self , height , weight):#サイズは3×3以上
        
        while(1):#ゴールに到達できるステージが生成されるまでループ
            stage = []
            for i in range(height):
                pushlist = []
                for j in range(weight):
                    pushlist.append('?')
                stage.append(pushlist) 

            for i in range(height):
                for j in range(weight):
                    if(i==0 or j == 0 or i == height-1 or j == weight -1):#外壁
                        stage[i][j] = 0
                    else:
                        stage[i][j] = random.randint(0,1)
            
            stage[1][1] = 1#スタートとゴール
            stage[height-2][weight-2] = 2

            if(BFS(stage) != INF ):#ゴールに到達できるかどうか

                for i in range(height):
                    pushlist = []
                    for j in range(weight):
                        pushlist.append('?')
                    stage_dire0.append(pushlist)
                for i in range(height):
                    for j in range(weight):
                        stage_dire0[i][j] = stage[i][j]
                break#stage_dire0にstageをコピーして終了

            else:
                continue
        


    def make_stage_dire1(self):#stage_dire1をセット
        for t in range(len(stage_dire0[0])):
            stage_dire1.append([])
            
        for t in range(len(stage_dire0[0])):
            for u in range(len(stage_dire0 )):
                stage_dire1[t].append(stage_dire0[len(stage_dire0 )-u-1][t])               


    def make_stage_dire3(self):#stage_dire3をセット
        for t in range(len(stage_dire0[0])):
            stage_dire3.append([])
            
        for t in range(len(stage_dire0[0])):
            for u in range(len(stage_dire0)):
                stage_dire3[t].append(stage_dire0[u][len(stage_dire0[0])-t-1])
                

        
    def make_stage_dire2(self):#stage_dire2をセット
        for t in range(len(stage_dire0)):
            stage_dire2.append([])
            
        for t in range(len(stage_dire0)):
            for u in range(len(stage_dire0[0])):
                stage_dire2[t].append(stage_dire0[len(stage_dire0)-t-1][len(stage_dire0[0])-u-1])


    def set_stage(self):
        self.make_stage_dire1()
        self.make_stage_dire2()
        self.make_stage_dire3()



class Display:

    def __init__(self):#サイズは3×3以上
        pass

    grafic_display = ""
    def grafic_plus(self,a):           #描画に関する約束事
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
        


    def grafic_minus(self,a,b):   #a から bと被った部分を消す関数
        p = ""
        for t in range(len(a)):
            
            if a[t] == b[t] and( b[t] != "　" and b[t] != "\n"):
                p+="　"
            else:
                p+=a[t]
        return p
        

        
    def direction_stage(self,direction):        #方角にマッチしたステージを返す.
        
        if direction == 0:
            s = stage_dire0
        elif direction == 1:
            s = stage_dire1
        elif direction == 2:
            s = stage_dire2
        elif direction == 3:
            s = stage_dire3

        return s



    def direction_plus(self,b,a,direction):   #方角を１増やし,そのときのx,yを返す。   この時ステージも変わる
        global x , y , dire
        x = len(self.direction_stage(direction))-b-1
        y = a
        if direction == 3:
            dire = 0
        else:
            dire +=1



    def direction_minus(self,b,a,direction):            #方角を１減らし,そのときのx,yを返す。　この時、ステージも変わる。
        global x , y , dire
        x = b
        y = len(self.direction_stage(direction)[0]) - a - 1   
        if direction == 0:
            dire = 3
        else:
            dire -= 1



    def route_view(self,b,a,direction):         #a座標　b座標　向きdi　を入れると　目の前の道の図を返す
                            # 道:1 以外は基本的に壁のイラストを追加する
                            # アイテム：４　の場所にも壁のイラストを追加し、その１つ前の位置にアイテムのイラストを追加する 
        s = self.direction_stage(direction)        #方角に合わせた向きのステージを返す
            
            
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
                p = self.grafic_plus([p,grafic_data[123]])
            elif s[b][a-1] == 1:
                if s[b-1][a-1] != 1: 
                    p = self.grafic_plus([p,grafic_data[120]])                   #左前の壁を作る
                else:
                    p = p
            
            
            if s[b][a+1] != 1:
                p = self.grafic_plus([p , grafic_data[131]])
            elif s[b][a+1] == 1:
                if s[b-1][a+1] != 1:
                    p = self.grafic_plus([p , grafic_data[130]])                   #右前の壁を作る
                else:
                    p = p
                    
            if cp == 1:                             #フタをする1
                if s[b-1][a] == 2:
                    p = self.grafic_plus([p , goal])
                elif s[b-1][a] == 3:
                    p = self.grafic_plus([p , door1])                     #目の前がドアだったらドア開けてして3を1にする、同じ方向だったら二度とドアは表示されない
                    s[b-1][a] = 1                       #違う方向から見るとドアは開いてない
                elif s[b-1][a] >= 4:      #4以上は全てドア
                    p = self.grafic_plus([p , door1])                     #ドア
                p = self.grafic_plus([p , grafic_data[112]])
                
            
            else:
                if s[b-1][a-1] != 1:
                    p = self.grafic_plus([p,grafic_data[93]])
                elif s[b-1][a-1] == 1:
                    if s[b][a-1] == 1:              #空間が２つ以上ある場合、１つ前の壁を消去
                        p = self.grafic_minus(p,grafic_data[120])
                    p = self.grafic_plus([p,grafic_data[90]])                   #左前の壁を作る
            
                if s[b-1][a+1] != 1:
                    p = self.grafic_plus([p , grafic_data[101]])
                elif s[b-1][a+1] == 1:
                    if s[b][a+1] == 1:              #空間が２つ以上ある場合、１つ前の壁を消去
                        p = self.grafic_minus(p,grafic_data[130])
                    p = self.grafic_plus([p , grafic_data[100]])                   #右前の壁を作る
                    
                
                
                if cp == 2:                            #フタをする2
                    if s[b-2][a] == 2:    #ゴールの場合 
                        p = self.grafic_plus([grafic_data[52],p])
                    elif s[b-2][a] == 3:
                        p = self.grafic_plus([p, door2])
                    elif s[b-2][a] >= 4:            #ドア
                        p = self.grafic_plus([p, door2])                    
                    if s[b-2][a-1] == 1 and s[b-1][a-1] == 1:
                        p = self.grafic_minus(p,grafic_data[62])
                        if s[b][a-1] != 1:
                            p = self.grafic_plus([p,grafic_data[123]])                # 誤って消した壁の部分を元に戻す
                    if s[b-2][a+1] == 1 and s[b-1][a+1] == 1:
                        p = self.grafic_minus(p , grafic_data[72])
                        if s[b][a+1] != 1:
                            p = self.grafic_plus([p , grafic_data[131]])               # 誤って消した壁の部分を元に戻す
                    p = self.grafic_plus([p,grafic_data[82]])
                else:
                    if s[b-2][a-1] != 1:
                        p = self.grafic_plus([p,grafic_data[63]])
                    elif s[b-2][a-1] == 1:
                        if s[b-1][a-1] == 1:              #空間が２つ以上ある場合、１つ前の壁を消去
                            p = self.grafic_minus(p,grafic_data[90])
                            if s[b][a-1] != 1:
                                p = self.grafic_plus([p , grafic_data[123]])              # 誤って消した壁の部分を元に戻す
                        p = self.grafic_plus([p,grafic_data[60]])                   #左前の壁を作る
            
                    if s[b-2][a+1] != 1:
                        p = self.grafic_plus([p , grafic_data[71]])
                    elif s[b-2][a+1] == 1:
                        if s[b-1][a+1] == 1:              #空間が２つ以上ある場合、１つ前の壁を消去
                            p = self.grafic_minus(p,grafic_data[100])
                            if s[b][a+1] != 1:
                                p = self.grafic_plus([p , grafic_data[131]])              # 誤って消した壁の部分を元に戻す
                        p = self.grafic_plus([p , grafic_data[70]])                   #右前の壁を作る
            
                    if cp == 3:                            #フタをする3
                        if s[b-3][a] == 2:    #ゴールの場合 
                            p = self.grafic_plus([grafic_data[22],p])
                        if s[b-3][a-1] == 1 and s[b-2][a-1] == 1:
                            p = self.grafic_minus(p,grafic_data[32])
                            if s[b-1][a-1] != 1:
                                p = self.grafic_plus([p , grafic_data[93]])               # 誤って消した壁の部分を元に戻す
                        if s[b-3][a+1] == 1 and s[b-2][a+1] == 1:
                            p = self.grafic_minus(p , grafic_data[42])
                            if s[b-1][a+1] != 1:
                                p = self.grafic_plus([p , grafic_data[101]])               # 誤って消した壁の部分を元に戻す
                        p = self.grafic_plus([p,grafic_data[52]])

                    else:
                        if s[b-3][a-1] != 1:
                            p = self.grafic_plus([p,grafic_data[33]])
                        elif s[b-3][a-1] == 1:
                            if s[b-2][a-1] == 1:              #空間が２つ以上ある場合、１つ前の壁を消去
                                p = self.grafic_minus(p,grafic_data[60])
                                if s[b-1][a-1] != 1:
                                    p = self.grafic_plus([p , grafic_data[93]])                # 誤って消した壁の部分を元に戻す
                            p = self.grafic_plus([p,grafic_data[30]])                   #左前の壁を作る
            
                        if s[b-3][a+1] != 1:
                            p = self.grafic_plus([p , grafic_data[41]])
                        elif s[b-3][a+1] == 1:
                            if s[b-2][a+1] == 1:              #空間が２つ以上ある場合、１つ前の壁を消去
                                p = self.grafic_minus(p,grafic_data[70])
                                if s[b-1][a+1] != 1:
                                    p = self.grafic_plus([p , grafic_data[101]])              # 誤って消した壁の部分を元に戻す
                            p = self.grafic_plus([p , grafic_data[40]])                   #右前の壁を作る
            
                        if cp == 4:                  #フタをする4
                            if s[b-4][a-1] == 1 and s[b-3][a-1] == 1:
                                p = self.grafic_minus(p,grafic_data[2])
                                if s[b-2][a-1] != 1:               # 誤って消した壁の部分を元に戻す
                                    p = self.grafic_plus([p , grafic_data[63]])
                            if s[b-4][a+1] == 1 and s[b-3][a+1] == 1:
                                p = self.grafic_minus(p , grafic_data[12])
                                if s[b-2][a+1] != 1:
                                    p = self.grafic_plus([p , grafic_data[71]])              # 誤って消した壁の部分を元に戻す
                            p = self.grafic_plus([p,grafic_data[22]])
                        else:                     #フタしをない
                            if s[b-4][a-1] == 1 and s[b-3][a-1] ==1 :
                                p = self.grafic_minus(p,grafic_data[2])
                                if s[b-2][a-1] != 1:
                                    p = self.grafic_plus([p , grafic_data[63]]) # 誤って消した壁の部分を元に戻す
                                if s[b-1][a-1] != 1:
                                    p = self.grafic_plus([p , grafic_data[93]]) # 誤って消した壁の部分を元に戻す
                            if s[b-4][a+1] == 1 and s[b-3][a+1] ==1 :
                                p = self.grafic_minus(p,grafic_data[12])
                                if s[b-2][a+1] != 1:
                                    p = self.grafic_plus([p , grafic_data[71]]) # 誤って消した壁の部分を元に戻す
                                if s[b-1][a+1] != 1:
                                    p = self.grafic_plus([p , grafic_data[101]]) # 誤って消した壁の部分を元に戻す
                            
                                

        self.grafic_display = p
        return p









    
        

    














if(__name__ == "__main__"):
    
    S_main = Stages(10,10)

    S_main.set_stage()

    file_open()

    D_main = Display()
    D_main.route_view(y,x,dire)


    window_width = 800
    window_height = 800



    game_window = tkinter.Tk()
    game_window.title("Maze_Game created by @NokonoKotlin")
    game_window.geometry(f"{window_width}x{window_height}")
    game_display_Var = tkinter.StringVar()
    game_display_Var.set(D_main.grafic_display)
    game_display_Label = tkinter.Label(game_window,textvariable = game_display_Var, bg = "#aaaaaa")


    #ボタンの記述

    def go_ahead(self):
        global x, y , dire
        if D_main.direction_stage(dire)[y-1][x] != 0 and D_main.direction_stage(dire)[y-1][x] != 2:
            y -= 1
            game_display_Var.set(D_main.route_view(y,x,dire))
        else:
            game_display_Var.set(D_main.route_view(y,x,dire))

    def turn_left(self):
        D_main.direction_plus(y,x,dire)
        game_display_Var.set(D_main.route_view(y,x,dire))

    def turn_right(self):
        D_main.direction_minus(y,x,dire)
        game_display_Var.set(D_main.route_view(y,x,dire))

    def turn_back(self):
        D_main.direction_plus(y,x,dire)
        D_main.direction_plus(y,x,dire)
        game_display_Var.set(D_main.route_view(y,x,dire))


    #初期化
    button_ahead = button_left = button_right = button_back = "init"

    

    button_ahead = tkinter.Button(game_window,text=u'進む',width = 10)
    button_right = tkinter.Button(game_window,text=u'右を向く',width = 10)
    button_left = tkinter.Button(game_window,text=u'左を向く',width = 10)
    button_back = tkinter.Button(game_window,text=u'後ろを向く',width = 10)

    

    button_ahead.place(x = window_width/2 - 60 , y = 0)
    button_back.place(x = window_width/2 - 60 , y = window_height - 60)
    button_left.place(x = 0, y = window_height/2 - 60)
    button_right.place(x = window_width -120 , y = window_height/2 -60)
   
    game_display_Label.place(x = window_width/2 - 190 , y = 30)

    button_ahead.bind("<Button-1>",go_ahead)
    button_back.bind("<Button-1>",turn_back)
    button_right.bind("<Button-1>",turn_right)
    button_left.bind("<Button-1>",turn_left)





    game_window.mainloop()

            
