#coding utf-8
from flask import *

import pymysql

import os

import json

import requests

import datetime

from flask_socketio import *

import time

import pytz



from engineio.payload import Payload



#計算用ライブラリとか
import library





"""
    ========================================================================================================================================================================
    READMEを読みましょう  
    ========================================================================================================================================================================
"""






"""
    ========================================================================================================================================================================
    ルーティング開始
           +    
    双方向通信開始
    ========================================================================================================================================================================
    
"""




app = Flask(__name__)


#このSECRET_KEY大事らしい
app.config['SECRET_KEY'] = 'Ge28hrd5i_KJJ3fhj20sdhrE49jkj1mV2jsj7KhJqwA30J3lvmQhL9uxafH1pmVeS7J5lkTsntVlbtR38shwrRk39gQpxnjaliytbwubodicaihgdhieuiegi924819p2nc8pdhfjkl39jo2HJFhe992aPWRNCP98qth38dnsDkth'


"""
    cors_allowed_originはよくわからんが、変な場所からのアクセスを制限するものらしい
    双方向通信のObject
"""


Payload.max_decode_packets = 50
socketio = SocketIO(app, cors_allowed_origins='*')











"""
    ========================================================================================================================================================================
    グローバル変数たち  
    ========================================================================================================================================================================

"""





#データベースの各メッセージ用テーブルの保存データ数の限度
DataBaseSizeLimit = 30

#データベースから持ってきたメッセージのうち、最古のものとの時間間隔がMessageTimeLimit秒以内なら送信不可(サーバー負荷対策)
MessageTimeLimit = 3*DataBaseSizeLimit

#トップページでチャットルームの候補を表示する数の上限
ChatRoomDistplayLimit = 6




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



"""
    各メッセージに割り振る、全テーブル共通のID. サーバーを立てた際に、全テーブルを調べて最大値で初期化。
    新たにメッセージが送信されるとインクリメント 
"""
MessageGlobalID = -1


AllChatRoomList = []

Connection.connect()
Cursor.execute("SELECT * FROM ChatRoomList")
for roomdata in Cursor:
    AllChatRoomList.append(roomdata['RoomName'])


for RoomName_value in AllChatRoomList:
    
    Cursor.execute("SELECT * FROM " + RoomName_value)
    
    for message_data in Cursor:
        
        MessageGlobalID = max([MessageGlobalID , message_data['MessageGlobalID']])

    
#Connection.close()

#流石に大きすぎたら、-1に戻しても差し支えない
if(MessageGlobalID >= 1000000000000):
    MessageGlobalID = -1







"""========================================================================================================================================================================
    アクセス制限に関する部分。詳しくはREADME参照。 サーバーにリクエストが来るたびに isRequestLimited 関数を呼び出し、問題があれば続く最大IntervalCounterLimit秒間リクエストを拒否する
    具体的には、IntervalCounterLimit秒のインターバルごとに処理を行い、アクセス過多となったインターバル中はサーバーへのリクエストを受け付けない
    ========================================================================================================================================================================
"""



#IntervalCounterLimit秒ごとにリクエスト状況(リクエスト数)を確認し、混雑と判断した場合、リクエストを禁止する(さらに次からは再開)
IntervalCounterLimit = 1.000

#現在のインターバルの開始時間
IntervalStartTime = time.perf_counter()

#このインターバル中のリクエスト数
RequestsNum_WhileInterval = 0

#１回のインターバル中に通すリクエスト数の上限
AllowedRequestMax = 1000


#このインターバルのリクエストが制限されているかどうか + インターバルの更新処理
def isRequestLimited():
    global IntervalStartTime , RequestsNum_WhileInterval
    
    RequestsNum_WhileInterval+=1 #今回のアクセス分を加算
    
    TimeNow = time.perf_counter()
    isRequestLimited_res = False#リクエスト禁止にするかどうか
    
    if(TimeNow - IntervalStartTime > IntervalCounterLimit):
        RequestsNum_WhileInterval = 0#初期化
        IntervalStartTime = TimeNow
        isRequestLimited_res = False
    else:
        if(RequestsNum_WhileInterval > AllowedRequestMax):#許可されているリクエスト数の上限を超えた
            isRequestLimited_res = True
        else:
            isRequestLimited_res = False
        
    return isRequestLimited_res






"""
    ========================================================================================================================================================================
    関数たち
    ========================================================================================================================================================================

"""










"""
    今自分が参加権限のある部屋の名前のリストを取得
    Client_json_dataはクライアントのデータ
"""
def get_ChatRoomNameList_now(Client_json_data):

    ChatRoomList_now = []
    
    #クライアントの経度/緯度
    ClientLongitude = Client_json_data['ClientLongitude']
    ClientLatitude = Client_json_data['ClientLatitude']
    
    
    
    Connection.connect()
    
    Cursor.execute("SELECT * FROM ChatRoomList")
    
     
    
    for ChatRoom_data in Cursor:
        #部屋位置情報のの範囲
        ChatRoomRadius = ChatRoom_data['Radius']
        #library.pyの関数：位置情報から距離(m)を計算
        DistBetweenClientAndRoom = library.realDistance( ClientLongitude , ClientLatitude , ChatRoom_data['Longitude'] , ChatRoom_data['Latitude'])
        #print(DistBetweenClientAndRoom)
        if(DistBetweenClientAndRoom <= ChatRoomRadius):
            #クライアントが部屋の範囲内にいるような部屋を追加
            ChatRoomList_now.append(ChatRoom_data['RoomName'])
            
    
    
    #Connection.close()
    
    
    return ChatRoomList_now
    









#今の自分の部屋内での権限を取得
def get_UserType():
    return 0
    





#引数で与えられた部屋名が存在するかどうか
def is_Exist_Such_RoomName(chatRoomName):
    res = False
    
    Connection.connect()
            
    Cursor.execute("SELECT * from ChatRoomList where RoomName = %s" , [chatRoomName] )
    for roomdata in Cursor:
        if(chatRoomName == roomdata['RoomName']):
            res = True
    #Connection.close()
     
    
    return res
    
    
    
    
    
    
    


"""
    データベースの古いデータを削除
    
    Client_json_dataはクライアントのデータ
"""
def delete_old_data(Client_json_data):
    
    
    ChatRoomName = Client_json_data['ChatRoomName']
    
    #部屋名が存在しないときは何もしない
    if(is_Exist_Such_RoomName(ChatRoomName) == False):
        return 0
    
    #ChatRoomのレコード数を確認
    #なんかネットの記事と違うのですが、SELECT COUNT(*) で取得した Cursorは
    # [ { 'COUNT(*)' : レコード数 } ] みたいに、Keyのディクショナリーのリストになっていて、
    #もしかしたら環境によって違うのかもしれないので、バグに注意
    
    
    
    order = 'SELECT * FROM '+ ChatRoomName
    Connection.connect()
    

        
    Cursor.execute(order , [])
    #Connection.close()
    
    
    
    
    messageCount = 0
    
    for x in Cursor:
    
        messageCount+=1
    
    #デーブルのレコード上限を超えた分だけ削除
    delete_limit = max( [ 0 , messageCount - DataBaseSizeLimit ] )
    
    #消去しなければFalse
    if(delete_limit <= 0):
        return False
    
    Connection.connect()
    Cursor.execute('DELETE FROM ' + ChatRoomName + ' LIMIT ' + str(delete_limit) + ' ; ' )#制限に引っかかった分は削除
    #Connection.close()
    
    
    #１つでも削除したらtrue
    return True
    







        
        
        
        
        
        
        
        
        

"""
    MessageListを取得
    ChatRoomNameのテーブルからメッセージ一覧を取ってくる
    Client_json_dataはクライアントのデータ
"""
def get_updated_MessageList(Client_json_data):
    
    
    #データベースの中の情報を整理 + 今いる部屋を取得
    delete_old_data(Client_json_data)
    
    
    ChatRoomName = Client_json_data['ChatRoomName']
    
    #部屋名が存在しないときは何もしない
    if(is_Exist_Such_RoomName(ChatRoomName) == False):
        return []
        
        
    #新たにデータベースからメッセージを取得
    newMessageList = []
    
    #テーブルにアクセス
    order = 'SELECT * FROM '+ChatRoomName
    Connection.connect()
    
   
    
    Cursor.execute(order , [])
    #Connection.close()
    
    #テーブルの中身を走査して,dictを追加
    for dict_record in Cursor:
        newMessageList.append( dict_record )

    
    
    return newMessageList
    
        
        
        
        
        
        
        
    
    
  




"""
    ========================================================================================================================================================================
    ルーティングやsocket.ioの窓口
    ========================================================================================================================================================================

"""








"""
    トップページにルーティング
    最初にアクセスした時に変数の初期化もしておく
    
"""
@app.route('/')
def index():    
    global MessageList , timestamp_now
    
    MyIPAddress = request.environ['REMOTE_ADDR']
    
    return render_template('Index.html' , myIP = MyIPAddress )



#プライバシーポリシーのページ
@app.route('/PrivacyPolicy')
def PrivacyPolicy():    
    return render_template('PrivacyPolicy.html')




#おしらせページ
@app.route('/Information')
def Information():    

    return render_template('Information_room.html', 
            chatRoomName = 'INFORMATION_ROOM' , 
            chatRoomName_JP = 'おしらせ',
            chatRoomExplanation = 'adminからのお知らせ。' ,
            usrType = 2,
            chatRoomLongitude = 136 ,
            chatRoomLatitude = 36,
            chatRoomRadius = 10000000 
            )





#おしらせページ
@app.route('/News')
def News():    

    return render_template('NU_news.html', 
            chatRoomName = 'NU_news' , 
            chatRoomName_JP = '名大新着ニュース',
            chatRoomExplanation = '名大の新着ニュースをお知らせします。' ,
            usrType = 2,
            chatRoomLongitude = 136 ,
            chatRoomLatitude = 36,
            chatRoomRadius = 10000000 
            )




"""
    GETでChatRoomNameを受け取って、そこにルーティング
"""
@app.route("/ChatRoom/<ChatRoomName>" , methods = ['GET'] )
def ChatRoom(ChatRoomName):
    #お知らせ部屋などは別で処理
    
    if(ChatRoomName == 'INFORMATION_ROOM'):
        return redirect('/Information')
    
    if(ChatRoomName == 'NU_news'):
        return redirect('/News')
    
    #部屋名が存在しないときはindexに戻す
    if(is_Exist_Such_RoomName(ChatRoomName) == False):
        return redirect('/')
    
    
    #部屋のデータから必要なものを持ってくる    
    Connection.connect()
    Cursor.execute("SELECT * from ChatRoomList where RoomName = %s"  , [ChatRoomName])
    #Connection.close()
    
    #以下、取ってくるデータ例を並べます
    ChatRoomName_JP = 'NULL'
    ChatRoomExplanation_forClient = 'NULL'
    ChatRoom_Longitude = -1
    ChatRoom_Latitude = -1
    ChatRoom_Radius = -1

    
    for roomdata in Cursor:
        ChatRoomName_JP = roomdata['RoomName_JP']
        ChatRoomExplanation_forClient = roomdata['RoomExplanation']
        ChatRoom_Longitude = roomdata['Longitude']
        ChatRoom_Latitude = roomdata['Latitude']
        ChatRoom_Radius = roomdata['Radius']
        
        
     
    
    
    return render_template('ChatRoom.html' , 
            chatRoomName = ChatRoomName , 
            chatRoomName_JP = ChatRoomName_JP,
            chatRoomExplanation = ChatRoomExplanation_forClient ,
            usrType = get_UserType(),
            chatRoomLongitude = ChatRoom_Longitude,
            chatRoomLatitude = ChatRoom_Latitude,
            chatRoomRadius = ChatRoom_Radius
            )
















"""
    メッセージが送信されたら発動
    データベースにメッセージを追加し、全員のチャットエリアにメッセージを表示
    
    正常に作動したら(メッセージ送信の制約(README)に引っ掛からなければ)databaseに追加して
    エラーが出たら(規約に引っ掛かったら) Javascriptのエラー窓口にアクセス
    
    
    
"""
@socketio.on('send_message')
def send_message(Client_json_data ):
    global MessageGlobalID
    
    if(isRequestLimited()):#アクセス制限のチェック
        emit('catch_error_when_send_request-' + Client_json_data['ChatRoomName'] , 3 )
        return False
    
    
    ChatRoomName = Client_json_data['ChatRoomName']
    message_text = Client_json_data['message_text']


    #まずはチャットルームの情報と、ユーザーの情報を照会し、書き込み可能かどうかを確認する
    Connection.connect()
    Cursor.execute('SELECT * FROM ChatRoomList where RoomName = %s' , [ChatRoomName])
    #Connection.close()
    
    #ユーザーデータと部屋データを照らし合わせて、発言可能な部屋なのかどうか。
    isAbleToSendMessage = False#そもそも対応する部屋がなかった場合もFalse
    
    for roomdata in Cursor:
        #ユーザの位置が部屋の範囲内かどうか
        if(
            library.realDistance(Client_json_data['ClientLongitude'] , Client_json_data['ClientLatitude'] , 
            roomdata['Longitude'] , roomdata['Latitude']) 
            < roomdata['Radius']
        ):
            isAbleToSendMessage = True
                
    if(isAbleToSendMessage == False):
        #エラーコード 2
        emit('catch_error_when_send_request-' + Client_json_data['ChatRoomName'] , 2 )
             
        
        return False
        
        
        
     
    

    
    
    MyIPAddress =  request.environ['REMOTE_ADDR']
    
    #日本時間を取得
    datetime_now = datetime.datetime.now(pytz.timezone('Asia/Tokyo'))
    
    
    
    Connection.connect()
    
    
    #最新のテーブルのみ取得
    Cursor.execute('SELECT * FROM '+ChatRoomName + ' ORDER BY timestamp DESC LIMIT 1;');
    
    #Connection.close()
    
    
    #最新のメッセージ
    LatestMessage = 'NULL'
    
    #現在のタイムスタンプ
    timestamp_now = time.time()
    
    
    #一番古いメッセージのタイムスタンプ(INFで初期化して、データを探索しながら更新)
    #まだ実装してないけど、のちに使うかも
    timestamp_oldest = 1000000000000000000000.00
    
    
    #最新のメッセージを取得
    for message_data in Cursor:
        LatestMessage = message_data['message']

    
    

    #送信したばかりなので、Likesは0
    MessageGlobalID+=1 #IDのインクリメントを忘れずに

    
    #アクセスしている自分以外のクライアント全員にメッセージを表示(自分以外のクライアントなので、タイプは0で左側に表示)
    emit('update_ChatArea-'+ChatRoomName, 
    { 
    'Text' : message_text ,
    'Type' : 0 , 
    'Date' : datetime_now.strftime("%Y/%m/%d %H:%M:%S") ,
    'TimeStamp' : str(timestamp_now),
    'MessageGlobalID' : MessageGlobalID,
    'Likes' : 0
    }
    , broadcast=True ,include_self = False)


    
    #自分のブラウザにメッセージを表示(タイプは２で右側に表示)
    emit('update_ChatArea-'+ChatRoomName, 
    { 
    'Text' : message_text ,
    'Type' : 2 , 
    'Date' : datetime_now.strftime("%Y/%m/%d %H:%M:%S") ,
    'TimeStamp' : str(timestamp_now),
    'MessageGlobalID' : MessageGlobalID,
    'Likes' : 0
    }
    , broadcast=False , include_self = True )
    
    
    
    Connection.connect()
    
    """
        timestamp_nowやgetUserTypeなどの数値もstrを使うことでsql文の文字列に直しているだけで、
        データベースではちゃんとdouble型やint型にになってる
        sql文に結合するため、文字列に直してあげる
    """
    
    Cursor.execute(
        'INSERT INTO ' 
        + ChatRoomName 
        + ' (timestamp,  message , IPAddress , usertype , date , MessageGlobalID , Likes ) VALUES ( %s , %s , %s  , %s , %s , %s , %s )'
        , [ str(timestamp_now) ,message_text , MyIPAddress  , str(get_UserType()) , datetime_now.strftime("%Y/%m/%d %H:%M:%S") , str(MessageGlobalID) , str(0)]
    )
    
     
    #Connection.close()
    
    
    
    #上限数を超えたデータを削除
    delete_old_data(Client_json_data)


    

    return True
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
"""
    あしあとがついた部屋の足跡を更新

"""
@socketio.on('send_FootPrints_for_database')
def send_FootPrints_for_database(Client_json_data):
    
    if(isRequestLimited()):#アクセス制限のチェック
        emit('catch_error_when_send_request-' + Client_json_data['ChatRoomName'] , 3 )
        return False
    
    
    if(is_Exist_Such_RoomName(Client_json_data['ChatRoomName']) == False):
        return False
    ChatRoomName = Client_json_data['ChatRoomName']
    
     #まずはチャットルームの情報と、ユーザーの情報を照会し、送信可能かどうかを確認する
    Connection.connect()
    Cursor.execute('SELECT * FROM ChatRoomList where RoomName = %s' , [ChatRoomName])
    #Connection.close()
    #ユーザーデータと部屋データを照らし合わせて、送信可能な部屋なのかどうか。
    isAbleToSendMessage = False#そもそも対応する部屋がなかった場合もFalse
    
    for roomdata in Cursor:
        #ユーザの位置が部屋の範囲内かどうか
        if(
            library.realDistance(Client_json_data['ClientLongitude'] , Client_json_data['ClientLatitude'] , 
            roomdata['Longitude'] , roomdata['Latitude']) 
            < roomdata['Radius']
        ):
            isAbleToSendMessage = True
                
    if(isAbleToSendMessage == False):
        #エラーコード 2
        emit('catch_error_when_send_request-' + Client_json_data['ChatRoomName'] , 2 )
             
        
        return False
        
        
    #ルーム名に対応したレコードの足跡を+1
    Connection.connect()
    Cursor.execute("UPDATE ChatRoomList SET FootPrintsNum =  FootPrintsNum + 1 where RoomName = %s" , [Client_json_data['ChatRoomName']])
    
    
     
    #Connection.close()
    
    return True
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
"""
    メッセージに対してLikeが押されたら、対応する部屋のテーブルの対応するIDのいいねカウントを+1する

"""
@socketio.on('send_Like_for_database')
def send_Like_for_database(Client_json_data):
    
    if(isRequestLimited()):#アクセス制限のチェック
        emit('catch_error_when_send_request-' + Client_json_data['ChatRoomName'] , 3 )
        return False
    
    
    if(is_Exist_Such_RoomName(Client_json_data['ChatRoomName']) == False):
        return False
    ChatRoomName = Client_json_data['ChatRoomName']
    
     #まずはチャットルームの情報と、ユーザーの情報を照会し、送信可能かどうかを確認する
    Connection.connect()
    Cursor.execute('SELECT * FROM ChatRoomList where RoomName = %s' , [ChatRoomName])
    #Connection.close()
    
    #ユーザーデータと部屋データを照らし合わせて、送信可能な部屋なのかどうか。
    isAbleToSendMessage = False#そもそも対応する部屋がなかった場合もFalse
    
    for roomdata in Cursor:
        #ユーザの位置が部屋の範囲内かどうか
        if(
            library.realDistance(Client_json_data['ClientLongitude'] , Client_json_data['ClientLatitude'] , 
            roomdata['Longitude'] , roomdata['Latitude']) 
            < roomdata['Radius']
        ):
            isAbleToSendMessage = True
                
    if(isAbleToSendMessage == False):
        #エラーコード 2
        emit('catch_error_when_send_request-' + Client_json_data['ChatRoomName'] , 2 )
             
        
        return False
        
    #ルーム名に対応したレコードの足跡を+1
    Connection.connect()
    Cursor.execute("UPDATE "+ Client_json_data['ChatRoomName'] + "  SET Likes =  Likes + 1 where MessageGlobalID = " + str(Client_json_data['MessageGlobalID']) )
    #Connection.close()
        
     
    
    
    return True
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    

"""
    HTMLのチャット欄にこれまでのメッセージを表示する
    
    HTMLから、HTMLで表示されている最新メッセージのタイムスタンプとチャットルーム名をjsonで受け取って
    メッセージをデータベースから持ってきて、チャットルーム名に対応した窓口に投げる(jsonデータで投げる)
    
    部屋をリロードした時とかに、チャット欄にこれまでのチャットを表示する用
"""
@socketio.on('return_updated_data')
def return_updated_data(Client_json_data):
    
    if(isRequestLimited()):#アクセス制限のチェック
        emit('catch_error_when_send_request-' + Client_json_data['ChatRoomName'] , 3 )
        return False
    
    
    ChatRoomName = Client_json_data['ChatRoomName']
    
    
    timestamp_now = Client_json_data['LatestMessageTimeStamp']
    
    #ChatRoomNameのテーブルからメッセージ一覧を取ってくる
    MessageList = get_updated_MessageList(Client_json_data)

        
    
    
    """
        MessageListを走査して、タイムスタンプが表示されている要素よりも新しい
        message要素のみをjavascriptに投げる
        
    """
    for message_data in MessageList:
        
        #タイムスタンプが不明なものは表示しない(バグによってデータベースに追加されたものだから)
        if(message_data['timestamp'] == None):
            continue;
        
    
        if(message_data['timestamp'] > timestamp_now):
            
            #print(message_data['message'])
            #'update_ChatArea-' + ChatRoomName　とあるが、どの部屋に送信するのかを区別するためにつけている
            emit('update_ChatArea-'+ChatRoomName, 
                { 
                'Text' : message_data['message'] ,
                'Type' : message_data['usertype'] , 
                'Date' :message_data['date'] ,
                'TimeStamp' : message_data['timestamp'],
                'MessageGlobalID' : message_data['MessageGlobalID'],
                'Likes' : message_data['Likes']
                }
            , broadcast=False, include_self=True)
            #bloadcastは、自分のブラウザのみに対してemitするという意味だと思うけど、
            #これで本当に他のブラウザに作用しないのか心配
            #socket.ioの勉強しないと
    
    return True
            

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
"""
    クライアントの参加している部屋名をjsonで受け取り、
    クライアントが、現在の部屋の参加権限があるかを判定し、なければエラーコード1をエラー窓口に返す
    ここはアクセス制限チェックを行わない
"""
@socketio.on('reload_Client_ChatRoom')
def reload_Client_ChatRoom(Client_json_data):
    
    
    #クライアントの参加権限のある部屋名(PRIMARY KEYの方)のリスト
    ChatRoomNameList_now = get_ChatRoomNameList_now(Client_json_data)
    
    #クライアントが今いる部屋名
    #今設定している値はデバッグ用です
    ClientChatRoomName = Client_json_data['ChatRoomName']#デバッグ用です
    
    
    
    if( ClientChatRoomName in ChatRoomNameList_now):
        
        pass
    else :
        #エラーコード 1
        emit('catch_error_when_send_request-' + Client_json_data['ChatRoomName'] , 1 )
    
    
    
    return True









"""
    クライアントのデータを受け取り、
    クライアントの入室可能な部屋のリストをjsonにしてクライアントの窓口に返す
    indexの方のファイルから使われることが多そう
    
    所得するデータ:ルーム名(key) , 日本語名, 足跡数をリストにする
    で、インデックス iに対応する部屋が各リストで同じになるようにしてjsonで渡す
    
    表示する候補数の制限があるので、近い順にChatRoomDisplayLimit個だけ表示する
"""
@socketio.on('reload_Client_ChatRoomCandidateList')
def reload_Client_ChatRoomCandidateList(Client_json_data):
    
    if(isRequestLimited()):
        return False
    
    
    
    #参加可能なルーム名の　PRIMARY KEYの方の名前一覧
    ChatRoomNameList = get_ChatRoomNameList_now(Client_json_data)
    
    #日本語名のルーム名リスト
    ChatRoomName_JP_List = []
    
    #各チャットルームの足跡数
    ChatRoomFootPrintsNum = []
    
    Connection.connect()
    
    DistList = []
    
    for ChatRoom_PrimaryKey in ChatRoomNameList:
        Cursor.execute("SELECT * FROM ChatRoomList where RoomName = %s" , [ChatRoom_PrimaryKey] )
        #Primary Keyは一意なので、ChatRoomList と ChatRoomName_JP_Listは順番に1対1対応している 
        for ChatRoom_data in Cursor:
            DistList.append(library.realDistance(Client_json_data['ClientLongitude'],Client_json_data['ClientLatitude']
            ,ChatRoom_data['Longitude'],ChatRoom_data['Latitude']))
    
    
    DistList = sorted(DistList)
    DistLimit = 0#コレよりも近い部屋のみを表示する
    if(len(DistList) > 0):
        DistLimit = DistList[min([len(DistList) , ChatRoomDistplayLimit])-1]
    
    
    for ChatRoom_PrimaryKey in ChatRoomNameList:
        Cursor.execute("SELECT * FROM ChatRoomList where RoomName = %s" , [ChatRoom_PrimaryKey] )
        #Primary Keyは一意なので、ChatRoomList と ChatRoomName_JP_Listは順番に1対1対応している 
        for ChatRoom_data in Cursor:
            if(library.realDistance(Client_json_data['ClientLongitude'],Client_json_data['ClientLatitude']
            ,ChatRoom_data['Longitude'],ChatRoom_data['Latitude']) <= DistLimit):
                ChatRoomName_JP_List.append(ChatRoom_data['RoomName_JP']) 
                ChatRoomFootPrintsNum.append(ChatRoom_data['FootPrintsNum'])
            
     
    
    #クライアントに投げるjsonデータには、Keyのルーム名、日本語名、足跡数をそれぞれ同順にリスト化して渡す
    
    json_data_for_Client = {
        'ChatRoomNameList' : ChatRoomNameList , 
        'ChatRoomName_JP_List' : ChatRoomName_JP_List ,
        'ChatRoomFootPrintsNum' : ChatRoomFootPrintsNum
    }
    
    
    #indexの窓口に投げる
    emit('get_ChatRoomCandidate' , json_data_for_Client,bloadcast = False )
    #Connection.close()
    return True
    
    
    
    
    
    
    
    
    
    
    
    
    
    


"""
    
    クライアント(index.html)からリクエストがあったら、
    データベースからオススメ名大関連webサイトのデータを持ってきて、
    それぞれ(名前、URL、説明 ,カテゴリに対応するint)をリストにして(i番目のデータはそれぞれ同じものに対応している)、
    クライアントにjsonデータで返す
    
"""
@socketio.on('return_WebSiteRecommendation')
def return_WebSiteRecommendation(json_data):
    
    
    WebSiteList_Name = []#名前のリスト
    WebSiteList_Link = []#URLのリスト
    WebSiteList_Explanation = []#説明文のリスト
    WebSiteList_Category = []#カテゴリに対応するintのリスト
    
    Connection.connect()
    Cursor.execute("SELECT * FROM RecommendedWebSite")
    #Connection.close()
    for WebSite_data in Cursor:
        #pushする順番を変えてはいけない
        WebSiteList_Name.append(WebSite_data['Name'])
        WebSiteList_Link.append(WebSite_data['Link'])
        WebSiteList_Explanation.append(WebSite_data['Explanation'])
        WebSiteList_Category.append(WebSite_data['Category'])
    
     

    #クライアントに返すjsonデータ
    json_data_for_Client = {
        'WebSiteList_Name' : WebSiteList_Name ,
        'WebSiteList_Link' : WebSiteList_Link ,
        'WebSiteList_Explanation' : WebSiteList_Explanation,
        'WebSiteList_Category' : WebSiteList_Category
    }
    
    emit('get_WebSiteRecommendation' , json_data_for_Client , bloadcast = False)















"""
    
    クライアント(index.html)からリクエストがあったら、
    データベースから名大生にオススメの遊び場のデータを持ってきて、
    それぞれ(名前、URL、説明 ,カテゴリに対応するint)をリストにして(i番目のデータはそれぞれ同じものに対応している)、
    クライアントにjsonデータで返す
    
"""
@socketio.on('return_PlayGroundRecommendation')
def return_PlayGroundRecommendation(json_data):
    
    
    PlayGroundList_Name = []#名前のリスト
    PlayGroundList_Link = []#URLのリスト
    PlayGroundList_Explanation = []#説明文のリスト
    PlayGroundList_Category = []#カテゴリに対応するintのリスト
    
    
    Connection.connect()
    Cursor.execute("SELECT * FROM RecommendedPlayGround")
    #Connection.close()
    for PlayGround_data in Cursor:
        #pushする順番を変えてはいけない
        PlayGroundList_Name.append(PlayGround_data['Name'])
        PlayGroundList_Link.append(PlayGround_data['Link'])
        PlayGroundList_Explanation.append(PlayGround_data['Explanation'])
        PlayGroundList_Category.append(PlayGround_data['Category'])
        
    
     

    #クライアントに返すjsonデータ
    json_data_for_Client = {
        'PlayGroundList_Name' : PlayGroundList_Name ,
        'PlayGroundList_Link' : PlayGroundList_Link ,
        'PlayGroundList_Explanation' : PlayGroundList_Explanation,
        'PlayGroundList_Category' : PlayGroundList_Category
    }
    
    emit('get_PlayGroundRecommendation' , json_data_for_Client , bloadcast = False)

    


















    



import logging 
l = logging.getLogger()
l.addHandler(logging.FileHandler("./log.txt"))


if __name__ == '__main__':
    app.run(host='0.0.0.0' , debug=False)





