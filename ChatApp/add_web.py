
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
    
        
            
    #テンプレ：Cursor.execute("""INSERT INTO RecommendedWebSite (Name , Link , Explanation , Category) VALUES('' , '' , '' , '')""")

    print("オススメwebサイトのデータをdatabaseに追加します:\n")
    name = input("webサイトの名前を入力してください:\n")
    link = input("webサイトのリンクを入力してください:\n")
    explanation = input("webサイトの説明を入力してください:\n")
    category_list = input("{ 2 : 学業 , 3 : 勉強 , 5 : エンターテイメント , 7 : 便利 , 11 : SNS , 13 : 公式 , 17 : 生活 } をカテゴリの番号割とする。当てはまる番号を1行で入力してください:\n").split()
    category_int = 1
    for x in category_list:
        category_int*=int(x)
        
    Cursor.execute("""INSERT INTO RecommendedWebSite (Name , Link , Explanation , Category) VALUES(%s , %s , %s , %s)""" , (name , link , explanation , str(category_int)))

    f = open("./db.txt", 'a')
    f.write("INSERT INTO RecommendedWebSite (Name , Link , Explanation , Category) VALUES('" + name + "' , '" +link+"' , '" + explanation +"' , '" +str(category_int) +"');")
    f.write("\n")
    f.flush()
    f.close()
        
    Connection.close()