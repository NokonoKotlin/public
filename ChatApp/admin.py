#coding utf-8

#DataBaseMainというデータベースの、指定したチャットルームテーブルに書き込みを行う

import pymysql
import pytz



import datetime



import time

if __name__ == '__main__':
    #データベース(MySQL)
    Connection = pymysql.connect(
         host='localhost',
         user='root',
         password='',
         db='DataBaseMain',
         autocommit=True,
         cursorclass=pymysql.cursors.DictCursor
         )
    
    
    """
        接続したデータベース上の操作を行い、
        Cursor自身には取得したテーブルなどの中身が入っている?     
    """
    Cursor = Connection.cursor()
    

    
    Connection.connect()
    
    Cursor.execute('SELECT * FROM ChatRoomList')
    
    
    name_list = []
    for room_data in Cursor:
        if(len(room_data['RoomName']) >= 6):
            name_list.append(room_data['RoomName'])    
        
        
        
    
    print("書き込みたい部屋を入力してください(下のように、番号と対応しているので、番号を入力してください)。")
    for ind in range(len(name_list)):
        print("{}  :  {}".format(name_list[ind],ind))
    RoomName_target_id = int(input())
    
    if(RoomName_target_id<0 or RoomName_target_id >= len(name_list)):
        print("適切な番号を入力してください")
        exit()
    RoomName_target = name_list[RoomName_target_id]
    
    
    
    
    
    
        
    print("あなたのユーザー権限を入力してください... 1-> admin , 3 -> サクラ , その他->パンピー")
    type = input()
    
    if(type != '0' and type != '1' and type != '2' and type != '3'):
        print("権限が無効です")
        exit()
    
    #type = int(type)
    
    
    
    
    while 1:

        print("メッセージを入力してください。 'end' と入力すればメッセージ入力は終了します。")
        message = input()
        
        if(message == 'end'):
            break
        #日本時間を取得
        datetime_now = datetime.datetime.now(pytz.timezone('Asia/Tokyo'))
        
        #タイムスタンプ
        timestamp_now = time.time() 
        
        #現在の日付
        datetime_now = datetime.datetime.now(pytz.timezone('Asia/Tokyo'))
        

        Cursor.execute("INSERT INTO " + RoomName_target + " (timestamp , message , usertype , date , MessageGlobalID) VALUES( " + str(timestamp_now) + " , %s , %s , %s , %s )" , [ message , type ,datetime_now.strftime("%Y/%m/%d %H:%M:%S") ,"-1" ] )
    
    
    
        #======================#======================#======================#======================#======================#======================
        #======================#======================#======================#======================#======================#======================
        #======================#======================#======================#======================#======================#======================
        #多くなったデータを整理(大体最新30こ分だけ残しておく)        
    
        
        
        
        ChatRoomName =  RoomName_target
        DataBaseSizeLimit = 30
        
        if(ChatRoomName == 'INFORMATION_ROOM' or ChatRoomName == 'NU_news'):
            DataBaseSizeLimit = 50;#(お知らせなどは少し多め)
    
        
        order = 'SELECT * FROM '+ ChatRoomName
        Connection.connect()
        
    
            
        Cursor.execute(order , [])
        #Connection.close()
        
        
        
        
        messageCount = 0
        
        for x in Cursor:
        
            messageCount+=1
        
        #デーブルのレコード上限を超えた分だけ削除
        delete_limit = max( [ 0 , messageCount - DataBaseSizeLimit ] )
        
    
        if(delete_limit > 0):
            
            Connection.connect()
            Cursor.execute('DELETE FROM ' + ChatRoomName + ' LIMIT ' + str(delete_limit) + ' ; ' )#制限に引っかかった分は削除
            
            

    
    
    Connection.close()