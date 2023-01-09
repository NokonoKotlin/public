# coding: utf-8
import math

POLE_RADIUS = 6356752    # 極半径(短半径)
EQUATOR_RADIUS = 6378137 # 赤道半径(長半径)
E = 0.081819191042815790 # 離心率
E2= 0.006694380022900788 # 離心率の２乗

class Coordinate:
    def __init__(self, latitude, longitude, altitude):
        self.latitude  = latitude
        self.longitude = longitude
        self.altitude  = altitude

def distance(point_a, point_b):
    a_rad_lat = math.radians(point_a.latitude)
    a_rad_lon = math.radians(point_a.longitude)
    b_rad_lat = math.radians(point_b.latitude)
    b_rad_lon = math.radians(point_b.longitude)
    m_lat = (a_rad_lat + b_rad_lat) / 2 # 平均緯度
    d_lat = a_rad_lat - b_rad_lat # 緯度差
    d_lon = a_rad_lon - b_rad_lon # 経度差
    W = math.sqrt(1-E2*math.pow(math.sin(m_lat),2))
    M = EQUATOR_RADIUS*(1-E2) / math.pow(W, 3) # 子午線曲率半径
    N = EQUATOR_RADIUS / W # 卯酉線曲率半径
    print(W , M , N , m_lat , d_lat**2 , d_lon**2)
    # d = math.sqrt(math.pow(M*d_lat,2) + math.pow(N*d_lon*math.cos(m_lat),2) + math.pow(point_a.altitude-point_b.altitude,2))
    d = math.sqrt(math.pow(M*d_lat,2) + math.pow(N*d_lon*math.cos(m_lat),2))
    return d

tokyo   = Coordinate(35.689608, 139.692080, 0)#東京都庁
nagoya  =Coordinate(35.1522116713572, 136.95865033949937, 0)
print(distance(tokyo,nagoya))
