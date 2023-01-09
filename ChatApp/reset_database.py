#coding utf-8
"""
    実行する前に、DataBaseMainというデータベースを初期状態にしておく

"""

import pymysql


    

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
    
    
    
    
    
    
    
    """===============================================================================================================================================================================
        ChatRoomListというtableを作って、ChatRoomたちを追加していく
        Longitude_Latitude : その場所の中心地の経度/緯度
        Radius : その場所の中心からどれだけの距離(メートル)がチャットルームエリアか
        RoomName : その場所の名前(プライマリーキー)
        RoomName_JP : 場所の名前(ページに表示する用)
        RoomExplanation : 説明(文字列長が超えないように要注意)
        ===============================================================================================================================================================================
    """
    
    
    """===============================================================================================================================================================================
        ChatRoomListに追加したChatRoomたちのtableを作っていく
        レコードはその部屋での発言(message)の情報
        timestamp : そのメッセージのタイムスタンプ
        message : 発言の内容(最大文字列長も決まっている)
        usertype : その部屋における発言者の権限(adminなど)
        IPAddress : その名の通り
        date : メッセージの投稿時間(秒単位)
        MessageGlobalID : 全テーブル共通のID
        Likes :メッセージに対するいいね数
        ===============================================================================================================================================================================
    """
    
    
    
    
    
    
    
    
    
    
    
    
    
    Cursor.execute("CREATE TABLE ChatRoomList(Longitude double , Latitude double , Radius double , RoomName varchar(127) PRIMARY KEY , RoomName_JP varchar(127) , FootPrintsNum int default 0 , RoomExplanation varchar(500) )")    
    
        
    explain_for_NU_general = """
        名古屋大学の一般チャットルーム。マナーよく利用しましょう、見てますからね。
    """
    
    Cursor.execute("""INSERT INTO ChatRoomList 
        (Longitude , Latitude , Radius , RoomName , Roomname_JP , RoomExplanation)  
        VALUES ( 136.9640823428945 , 35.1547540071169 , 500 , 'NU_general' , '名古屋大学-general' , %s)"""
        , [explain_for_NU_general])
        
    Cursor.execute("CREATE TABLE NU_general(timestamp double , message varchar(255)  , usertype INT, IPAddress varchar(63) , date varchar(63) , MessageGlobalID INT, Likes INT default 0)")    
    

    
    
    
    
    
    
    explain_for_DEBUG_PLACE = """デバッグ用のチャットルーム。何故かよく接続が切れるのだ。"""
    Cursor.execute("""INSERT INTO ChatRoomList
        (Longitude , Latitude , Radius , RoomName , Roomname_JP ,RoomExplanation) 
        VALUES ( 136.95864365037605 , 35.15224218350844 , 15 , 'DEBUG_PLACE' , 'テスト用チャットルーム' , %s)"""
        , [explain_for_DEBUG_PLACE])
        
    Cursor.execute("CREATE TABLE DEBUG_PLACE(timestamp double , message varchar(255)  , usertype INT, IPAddress varchar(63) , date varchar(63) , MessageGlobalID INT, Likes INT default 0)")    
    
    
    
    

    
    
    
    explain_for_NAGOYA_U_LIBRARY = """
        名古屋大学の建物の一つ。最初に1階と思っていた階が実は2階だったことが一度はあるはず。
    """
    
    Cursor.execute("""INSERT INTO ChatRoomList 
        (Longitude , Latitude , Radius , RoomName , Roomname_JP , RoomExplanation)  
        VALUES ( 136.9640823428945 , 35.1547540071169 , 70 , 'NAGOYA_U_LIBRARY' , '名古屋大学-中央図書館' , %s)"""
        , [explain_for_NAGOYA_U_LIBRARY])
        
    Cursor.execute("CREATE TABLE NAGOYA_U_LIBRARY(timestamp double , message varchar(255)  , usertype INT, IPAddress varchar(63) , date varchar(63) , MessageGlobalID INT, Likes INT default 0)")    
    

    

    
    
    
    
    
    explain_for_INFORMATION_ROOM = """
        adminからのお知らせ専用ルームです。
    """
    
    Cursor.execute("""INSERT INTO ChatRoomList 
        (Longitude , Latitude , Radius , RoomName , Roomname_JP , RoomExplanation)  
        VALUES ( 136.95864365037605 , 35.15224218350844 , 10000000 , 'INFORMATION_ROOM' , 'おしらせ' , %s)"""
        , [explain_for_INFORMATION_ROOM])
    
    Cursor.execute("CREATE TABLE INFORMATION_ROOM(timestamp double , message varchar(511)  , usertype INT, IPAddress varchar(63) , date varchar(63) , MessageGlobalID INT, Likes INT default 0)")    
    
    
    
    
    
        
    explain_for_NU_news = """
       名大の新着ニュースです。
    """
    
    Cursor.execute("""INSERT INTO ChatRoomList 
        (Longitude , Latitude , Radius , RoomName , Roomname_JP , RoomExplanation)  
        VALUES ( 136.95864365037605 , 35.15224218350844 , 10000000 , 'NU_news' , '名大新着ニュース' , %s)"""
        , [explain_for_NU_news])
    
    Cursor.execute("CREATE TABLE NU_news(timestamp double , message varchar(511)  , usertype INT, IPAddress varchar(63) , date varchar(63) , MessageGlobalID INT, Likes INT default 0)")    
    
    
    
    

    
    
    
    
    
    
    
    
    
    
    
    
    
    """===============================================================================================================================================================================
        RecommendedWebSiteというデータベース    
        
        名大関連のオススメWEBサイトのデータを管理
        カラムは {名前 : Name , サイトのURL: Link , サイトの説明:Explanation}
        文字列長が要注意！超えてしまう場合は要対処
        
        ===============================================================================================================================================================================
    """
    
    Cursor.execute("CREATE TABLE RecommendedWebSite(Name varchar(63) , Link varchar(255) PRIMARY KEY, Explanation varchar(255), Category BIGINT default 1)")    

    
    
    
    
    
    
        
    """===============================================================================================================================================================================
        RecommendedPlayGroundというデータベース    
        
        名大生へのオススメの遊び場のデータを管理
        カラムは {名前 : Name , 遊び場のURLや位置情報: Link , 遊び場の説明:Explanation}
        文字列長が要注意！超えてしまう場合は要対処
        
        ===============================================================================================================================================================================
    """
    
    Cursor.execute("CREATE TABLE RecommendedPlayGround(Name varchar(63) , Link varchar(255) PRIMARY KEY, Explanation varchar(255), Category BIGINT default 1)")    
    #テンプレ：Cursor.execute("""INSERT INTO RecommendedPlayGround (Name , Link , Explanation) VALUES('' , '' , '')""")
    
    
    
    
    
    
    
    
    
    
    
    Connection.close()



















    

