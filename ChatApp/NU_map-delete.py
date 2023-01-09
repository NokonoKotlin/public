
import pymysql
import library

    

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
    
    
    
    
    #地点s , t , uに対して、ベクトルst , su が造る二等辺三角形を等分し、そこをチャットルームの区画とする
    s_Latitude , s_Longitude = 35.15369069670362, 136.96021445133778
    t_Latitude , t_Longitude = 35.15860518690398, 136.9623781894616
    u_Latitude , u_Longitude = 35.15079442213633, 136.96863577221325
    

    #各ベクトルの要素
    st_Latitude = t_Latitude - s_Latitude
    st_Longitude = t_Longitude - s_Longitude
    
    su_Latitude = u_Latitude - s_Latitude
    su_Longitude = u_Longitude - s_Longitude
    
    
    
    
    #区画をst方向に何分割するか
    div_st = 9;
    
    #区画をsu方向に何分割すか
    div_su = 16;
    
    alphabet = ['A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z']
    cnt = 0;
    
    
        
    #一区画のチャットルームの範囲(中心からの半径)
    radius = library.realDistance(s_Longitude , s_Latitude , s_Longitude + st_Longitude/div_st + su_Longitude/div_su , s_Latitude + st_Latitude/div_st + su_Latitude/div_su )
    
    
            
    for i in range(div_st+1):
        for j in range(div_su+1):
            Latitude = s_Latitude + (st_Latitude/div_st + su_Latitude/div_su)*i
            Longitude = s_Longitude + (st_Longitude/div_st + su_Longitude/div_su)*j
            
            
            
            Latitude_ID = str(i)
            Longitude_ID = alphabet[j]
            cnt+=1
            ChatRoomName = Latitude_ID + "_" + Longitude_ID
            
            Cursor.execute("delete from ChatRoomList where RoomName = '{}_{}'".format( Latitude_ID,Longitude_ID))
            Cursor.execute("drop table {}_{}".format( Latitude_ID,Longitude_ID))

            
                    

    

        
    Connection.close()