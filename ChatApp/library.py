from math import *



"""
    始点の(経度、緯度)と終点の(経度、緯度)を入れて、メートル単位で実際の距離を計算する
"""
def realDistance(longitude_s , latitude_s , longitude_g , latitude_g):
    PI = 3.141592653589793238462643383279
    
    #緯度、軽度をラジアンになおす
    
    rad_longitude_s = longitude_s*PI/180
    rad_latitude_s = latitude_s*PI/180
    rad_longitude_g = longitude_g*PI/180
    rad_latitude_g = latitude_g*PI/180

    #各ラジアンの差 
    deltaX = abs(rad_longitude_g - rad_longitude_s)
    deltaY = abs(rad_latitude_g - rad_latitude_s)
        
    #緯度の平均
    Ave = (rad_latitude_s + rad_latitude_g)/2
    
    #測地：WGS84での地球の半径
    R_longitude = 6378137.000 #横方向の半径
    R_latitude = 6356752.314245 #縦方向の半径
    
    #離心率
    Eccentricity = sqrt((R_longitude**2 - R_latitude**2 )/(R_longitude**2))
    
    W = sqrt(1 - (Eccentricity**2)*(sin(Ave)**2));
    
    #子午線曲率半径
    Meridian = ( R_longitude*(1 - Eccentricity**2) )/(W**3);
    
    #卯酉線
    Vertical = R_longitude/W;
    
    #答え
    res = sqrt((deltaY*Meridian)**2 + (deltaX*Vertical*cos(Ave))**2);

    return res
    
    
    
    
    
    
    
    
    
    
    
    
    
    