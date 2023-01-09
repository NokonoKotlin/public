



function isPC() {
    if (navigator.userAgent.match(/iPhone|Android.+Mobile/)) {
        return false;
    } else {
        return true;
    }
}



// Geolocation APIに対応しているかどうか
function check_able_to_get_position(){

    if( navigator.geolocation ){

        return true;

    }else{

        return false;

    }
}









function check_and_display_UserPosition(){
    if(check_able_to_get_position()){
        navigator.geolocation.getCurrentPosition(function(position){


            alert("測定しました。" + "\n" 
            + " 経度 : "+ position.coords.latitude + " , \n" 
            + " 緯度 : "+ position.coords.longitude + "  \n"  );


            

        });
    }else{
        alert("位置情報を取得できませんでした。")
    }
}





/*
    longitude : 経度(x座標方向)
    latitude : 緯度(y座標方向)
    _s ... 始点
    _g ... 終点
    実際の距離(メートル)を計算
*/
function realDistance(longitude_s ,  latitude_s , longitude_g , latitude_g ){

    const PI = 3.14159265359;

    //角度をラジアンに直す
    const rad_longitude_s = longitude_s*PI/180;
    const rad_latitude_s = latitude_s*PI/180;
    const rad_longitude_g = longitude_g*PI/180;
    const rad_latitude_g = latitude_g*PI/180;

    //各ラジアンの差
    const deltaX = Math.abs(rad_longitude_s-rad_longitude_g);
    const deltaY = Math.abs(rad_latitude_s-rad_latitude_g);


    //緯度の平均
    const Ave = (rad_latitude_g + rad_latitude_s)/2; 
    
    //測地 : WGS84
    const R_longitude =  6378137.000; //長半径(地球の横方向)
    const R_latitude =  6356752.314245; //短半径(地球の縦方向)

    //離心率
    const Eccentricity = Math.sqrt( (R_longitude**2 - R_latitude**2) / R_longitude**2);

    const W = Math.sqrt(1 - (Eccentricity**2)*(Math.sin(Ave)**2));
    
    //子午線曲率半径
    const Meridian = ( R_longitude*(1 - Eccentricity**2) )/(W**3);

    //卯酉線
    const Vertical = R_longitude/W;

    //答え
    const res = Math.sqrt((deltaY*Meridian)**2 + (deltaX*Vertical*Math.cos(Ave))**2);


    


    return res;
}


