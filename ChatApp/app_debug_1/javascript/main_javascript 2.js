



// Geolocation APIに対応しているかどうか
function check_able_to_get_position(){

    if( navigator.geolocation ){

        return true;

    }else{

        return false;

    }
}



//デバイスがPCかどうか
function isPC() {
    if (navigator.userAgent.match(/iPhone|Android.+Mobile/)) {
        return false;
    } else {
        return true;
    }
}




//------------------------------------------------------------------位置情報関連//------------------------------------------------------------------
//------------------------------------------------------------------位置情報関連//------------------------------------------------------------------
//------------------------------------------------------------------位置情報関連//------------------------------------------------------------------






/*
    東京の軽度と緯度
*/

const longitude_Tokyo = 139.44288869;
const latitude_Tokyo = 35.39291572;

/*
    自分の経度と緯度
*/
var User_position_X ,User_position_Y;

/*
    精度が良くないので、直近１０回の平均を取る。
*/
const neardata_limit = 10;
var UserPosition_List = new Array;





/*
    自分の位置情報を取得
*/
function GetUserPosition(){
    if(check_able_to_get_position()){
        navigator.geolocation.getCurrentPosition(function(position){
            User_position_Y = position.coords.latitude;//緯度
            User_position_X = position.coords.longitude;//経度


            if(UserPosition_List.length >= neardata_limit){
                UserPosition_List.shift();
            }
            UserPosition_List.push(new Array(User_position_X , User_position_Y));
            
            var average_User_position_X = 0;
            var average_User_position_Y = 0;
            
            
            for(var i = 0 ; i <UserPosition_List.length;i++){
                average_User_position_X += UserPosition_List[i][0];
                average_User_position_Y += UserPosition_List[i][1];
            }
            average_User_position_X/=UserPosition_List.length;
            average_User_position_Y/=UserPosition_List.length;

            
            alert("あなたの位置情報　\n" 
                + " 経度 : "+ average_User_position_X + " , \n" 
                + " 緯度 : "+ average_User_position_Y + " , \n" 
                + " 東京からの距離 : "+ realDistance(longitude_Tokyo , latitude_Tokyo , User_position_X , User_position_Y) + "m\n");
        });
    }else{
        alert("位置情報を取得できませんでした。")
    }
}





function calc_move(){
    if(check_able_to_get_position()){
        navigator.geolocation.getCurrentPosition(function(position){
            User_position_Y = position.coords.latitude;//緯度
            User_position_X = position.coords.longitude;//経度

            if(UserPosition_List.length>=1){
                const bef_longitude = UserPosition_List[UserPosition_List.length-1][0];
                const bef_latitude = UserPosition_List[UserPosition_List.length-1][1];
                alert("測定しました。　\n" 
                + " 経度 : "+ User_position_X + " , \n" 
                + " 緯度 : "+ User_position_Y + " , \n" 
                + " 前回測定時からの移動の距離 : "+ realDistance(bef_longitude , bef_latitude , User_position_X , User_position_Y) + "m\n");

            }else{
                alert("測定しました。" + "\n" 
                + " 経度 : "+ User_position_X + " , \n" 
                + " 緯度 : "+ User_position_Y + " , \n"  );
            }

            if(UserPosition_List.length >= neardata_limit){
                UserPosition_List.shift();
            }
            UserPosition_List.push(new Array(User_position_X , User_position_Y));
            


            

        });
    }else{
        alert("位置情報を取得できませんでした。")
    }
}





/*

    軽度と緯度の精度が良くないから、直近何回かの平均をとったらいいと思う。

*/

//setInterval(GetUserPosition,8000);



/*
    longitude : 経度(x座標方向)
    latitude : 緯度(y座標方向)
    _s ... 始点
    _g ... 終点
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



//------------------------------------------------------------------位置情報関連終了//------------------------------------------------------------------
//------------------------------------------------------------------位置情報関連終了//------------------------------------------------------------------
//------------------------------------------------------------------位置情報関連終了//------------------------------------------------------------------
//------------------------------------------------------------------位置情報関連終了//------------------------------------------------------------------




//------------------------------------------------------------------html、ブラウザ画面関連//------------------------------------------------------------------
//------------------------------------------------------------------html、ブラウザ画面関連//------------------------------------------------------------------
//------------------------------------------------------------------html、ブラウザ画面関連//------------------------------------------------------------------
//------------------------------------------------------------------html、ブラウザ画面関連//------------------------------------------------------------------










//------------------------------------------------------------------メイン記述部//------------------------------------------------------------------
//------------------------------------------------------------------メイン記述部//------------------------------------------------------------------
//------------------------------------------------------------------メイン記述部//------------------------------------------------------------------
//------------------------------------------------------------------メイン記述部//------------------------------------------------------------------


function main(){
    
}

main()


