/*******************************************************************************
Example 2: ESP32 (IoTセンサ) Wi-Fi ボタン for M5Sack Core
・中央ボタンを押下するとUDPでLAN内に文字列"Ping"を送信します。
・ON状態の時に左ボタンを押すと"Pong"を送信します。
・OFF状態の時に右ボタンを押すと"Pong"を送信します。
・LINE用トークンを設定すれば、LINEアプリに「ボタンが押されました」等を通知。
・別の子機となる Wi-Fi コンシェルジェ照明担当(ワイヤレスLED子機)のIPアドレスを
　設定すれば、右ボタンを押下したときに子機のLEDをON、左ボタンでOFFに制御。

    使用機材(例)：M5Sack Core

                                          Copyright (c) 2021-2024 Wataru KUNINO
********************************************************************************
描画が遅い場合は、ボードマネージャでM5Stackバージョン1.0.9等を選んでください。
*******************************************************************************/

#include <M5Stack.h>                            // M5Stack用ライブラリの組み込み
#include <WiFi.h>                               // ESP32用WiFiライブラリ
#include <WiFiUdp.h>                            // UDP通信を行うライブラリ
#include <HTTPClient.h>                         // HTTPクライアント用ライブラリ
#include "on_sw_jpg.h"                          // ON状態のスイッチのJPEGデータ
#include "off_sw_jpg.h"                         // OFF状態のスイッチのJPEGデータ

/******************************************************************************
 Wi-Fi の設定
 *****************************************************************************/
#define SSID "1234ABCD"                         // 無線LANアクセスポイント SSID
#define PASS "password"                         // パスワード

/******************************************************************************
 LINE Messaging API 設定
 ******************************************************************************
  LINE 公式アカウントと Messaging API 用のChannel情報が必要です。
    1. https://entry.line.biz/start/jp/ からLINE公式アカウントを取得する
    2. https://manager.line.biz/ の設定で「Messaging APIを利用する」を実行する
    3. Channel 情報 (Channel ID と Channel secret) を取得する
    4. スクリプト内の変数 line_ch_id にChannel IDを記入する
    5. スクリプト内の変数 line_ch_pw にChannel secretを記入する
 *****************************************************************************/
#define line_ch_id "0000000000"                         // Channel ID
#define line_ch_pw "00000000000000000000000000000000"   // Channel secret

/******************************************************************************
 Wi-Fi コンシェルジェ照明担当（ワイヤレスLED子機） の設定
 ******************************************************************************
 ※ex01_led または ex01_led_io が動作する、別のESP32C3搭載デバイスが必要です
    1. ex01_led/ex01_led_io搭載デバイスのシリアルターミナルでIPアドレスを確認
    2. 下記のLED_IPのダブルコート(")内に貼り付け
 *****************************************************************************/
#define LED_IP "192.168.1.0"                    // LED搭載子のIPアドレス

/******************************************************************************
 UDP 宛先 IP アドレス設定
 ******************************************************************************
 カンマ区切りでUPD宛先IPアドレスを設定してください。
 末尾を255にすると接続ネットワーク(アクセスポイント)にブロードキャスト
 *****************************************************************************/
#define PORT 1024                               // 送信のポート番号
IPAddress UDPTO_IP = {255,255,255,255};         // UDP宛先 IPアドレス

String btn_S[]={"No","OFF","Ping","ON"};        // 送信要否状態0～3の名称

String get_token(){                             /* LINE用 トークン取得部 */
    HTTPClient http;                            // HTTPリクエスト用インスタンス
    http.setConnectTimeout(15000);              // タイムアウトを15秒に設定する
    if(strcmp(line_ch_id,"0000000000") == 0){   // line_ch_id 未入力時
        M5.Lcd.println("ERROR: Pls set line_ch_id and pwd");
        return "";
    }
    String url = "https://api.line.me/oauth2/v3/token";
    String body = "grant_type=client_credentials&";
    body += "client_id=" + String(line_ch_id) + "&";
    body += "client_secret=" + String(line_ch_pw);
    http.begin(url);                            // HTTPリクエスト先を設定する
    http.addHeader("Content-Type","application/x-www-form-urlencoded");
    int httpCode = http.POST(body);             // HTTP送信を行う
    String token="";
    if(httpCode == 200){
        String S = http.getString();            // HTTPデータを変数Sへ
        int i = S.indexOf("\"access_token\"");
        if((i>0) && (S.substring(i+15, i+16).equals("\""))){
            token = S.substring(i+16, i+16+174);
            // M5.Lcd.println(token);           // 取得したトークンを表示する
        }
    }else{
        M5.Lcd.println("HTTP ERROR: "+String(httpCode));
    }
    http.end();                                 // HTTP通信を終了する
    return token;                               // トークンを応答
}

int message_to_line(String message){            /* LINE用 メッセージ送信部 */
    String token = get_token();                 // トークンを取得する
    int token_len = token.length();             // 取得したトークン長
    if(token_len != 174){                       // トークン長が174以外の時
        M5.Lcd.println("ERROR: Token Length Error; " + String(token_len));
        return 0;
    }
    HTTPClient http;                            // HTTPリクエスト用インスタンス
    http.setConnectTimeout(15000);              // タイムアウトを15秒に設定する
    String url = "https://api.line.me/v2/bot/message/broadcast";
    M5.Lcd.println(url);                        // 送信URLを表示
    http.begin(url);                            // HTTPリクエスト先を設定する
    http.addHeader("Content-Type","application/json");
    http.addHeader("Authorization","Bearer " + token);
    String json = "{\"messages\":[{\"type\":\"text\",\"text\":\"";
    json += message + "\"}]}";                  // メッセージを送信用jsonに代入
    M5.Lcd.println(json);                       // HTTP送信内容を表示する
    int httpCode = http.POST(json);             // HTTPでメッセージを送信する
    http.end();                                 // HTTP通信を終了する
    return httpCode;                            // HTTPリザルトを応答
}

int btnUpdate(){                                // ボタン状態に応じて画面切換
    M5.update();                                // M5Stack用IO状態の更新
    delay(10);                                  // ボタンの誤作動防止用
    int tx_en = 0;                              // 送信要否tx_en(0:送信無効)
    if( M5.BtnA.wasPressed() ){                 // ボタンAが押されたとき
        M5.Lcd.drawJpg(off_sw_jpg,off_sw_jpg_len);  // LCDにJPEG画像off_swを表示
        tx_en = 1;                              // 送信要否tx_en(1:OFFを送信)
    }else if( M5.BtnB.wasPressed() ){           // ボタンBが押されたとき
        tx_en = 2;                              // 送信要否tx_en(2:Ping)
    }else if( M5.BtnC.wasPressed() ){           // ボタンCが押されたとき
        M5.Lcd.drawJpg(on_sw_jpg,on_sw_jpg_len);   // LCDにJPEG画像on_swを表示
        tx_en = 3;                              // 送信要否tx_en(3:ONを送信)
    }
    if(tx_en) M5.Lcd.setCursor(0, 0);           // LCD文字表示位置を原点に
    return tx_en;                               // 送信要否を応答する
}

void setup(){                                   // 起動時に一度だけ実行する関数
    M5.begin();                                 // M5Stack用ライブラリの起動
    M5.Lcd.setBrightness(31);                   // 輝度を下げる（省エネ化）
    M5.Lcd.drawJpg(off_sw_jpg,off_sw_jpg_len);  // LCDにJPEGファイルoff_sw表示
    M5.Lcd.println("M5 SW UDP LINE LED");       // 「SW UDP」をシリアル出力表示
    WiFi.mode(WIFI_STA);                        // 無線LANをSTAモードに設定
    WiFi.begin(SSID,PASS);                      // 無線LANアクセスポイント接続
    while(WiFi.status() != WL_CONNECTED){       // 接続に成功するまで待つ
        delay(50);                              // 待ち時間処理
        btnUpdate();                            // ボタン状態を確認
    }
    M5.Lcd.print(WiFi.localIP());               // 本機のアドレスをシリアル出力
    M5.Lcd.print(" -> ");                       // 矢印をシリアル出力
    M5.Lcd.println(UDPTO_IP);                   // UDPの宛先IPアドレスを出力
}

void loop(){                                    // 繰り返し実行する関数
    int tx_en = btnUpdate();                    // ボタン状態と送信要否の確認
    if(tx_en==0) return;                        // 送信要求が無い(0)の時に戻る

    WiFiUDP udp;                                // UDP通信用のインスタンス定義
    udp.beginPacket(UDPTO_IP, PORT);            // UDP送信先を設定
    M5.Lcd.print("udp: ");                      // 「udp:」をLCDに表示
    if(tx_en == 1){                             // OFFを送信の時
        udp.println("Pong");                    // メッセージ"Pong"を送信
        M5.Lcd.println("Pong");                 // "Pong"をLCD表示
    }else{                                      // その他の送信の時
        udp.println("Ping");                    // メッセージ"Ping"を送信
        M5.Lcd.println("Ping");                 // "Ping"をLCD表示
    }
    udp.endPacket();                            // UDP送信の終了(実際に送信)
    delay(200);                                 // 送信待ち時間

    if(strcmp(line_ch_id,"0000000000")){        // LINE_TOKEN設定時
        message_to_line("ボタン(" + btn_S[tx_en]  + ")が押されました");
    }
    if(strcmp(LED_IP,"192.168.1.0")){           // 子機IPアドレス設定時
        String url = "http://" + String(LED_IP) + "/?L="; // アクセス先URL
        url += String(tx_en == 1 ? 0 : 1);      // L=OFF時0、その他1
        HTTPClient http;                        // HTTPリクエスト用インスタンス
        http.setConnectTimeout(15000);          // タイムアウトを15秒に設定する
        M5.Lcd.println(url);                    // 送信URLをLCD表示
        http.begin(url);                        // HTTPリクエスト先を設定する
        http.GET();                             // ワイヤレスLEDに送信する
        http.end();                             // HTTP通信を終了する
    }
}

/******************************************************************************
【参考文献】
Arduino IDE 開発環境イントール方法：
https://docs.m5stack.com/en/quick_start/m5core/arduino

M5Stack Arduino Library API 情報：
https://docs.m5stack.com/en/api/core/system

【引用コード】
https://github.com/bokunimowakaru/esp/tree/master/2_example/example02_sw
https://github.com/bokunimowakaru/esp/tree/master/2_example/example34_sw
https://github.com/bokunimowakaru/esp32c3/tree/master/learning/ex02_sw
******************************************************************************/
/*
###############################################################################
# 参考文献：下記のcurl文の一部を引用しました
# LINE DevelopersLINE Developers, Messaging APIリファレンス
# https://developers.line.biz/ja/reference/messaging-api/
###############################################################################
# 注意事項
# ・メッセージ送信回数の無料枠は200回/月です。超過分は有料となります。
# ・プログラムの繰り返し動作や誤作動などで無料枠を超過する場合があります。
# ・15分間だけ有効なステートレスチャネルアクセストークンを使用しています。
# ・本スクリプトでは、実行の度にTokenを取得しています。
###############################################################################
*/
