/*******************************************************************************
Example 1: ESP32 で LINE Messaging API にメッセージを送信する

                                                Copyright (c) 2024 Wataru KUNINO
*******************************************************************************/

#include <WiFi.h>                               // ESP32用WiFiライブラリ
#include <HTTPClient.h>                         // HTTPクライアント用ライブラリ

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

#define SSID "1234ABCD"                         // 無線LANアクセスポイント SSID
#define PASS "password"                         // パスワード

String get_token(){                             /* LINE用 トークン取得部 */
    HTTPClient http;                            // HTTPリクエスト用インスタンス
    if(strcmp(line_ch_id,"0000000000") == 0){   // line_ch_id 未入力時
        Serial.println("ERROR: Pls set line_ch_id and pwd");
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
            Serial.println(token);              // 取得したトークンを表示する
        }
    }else{
        Serial.println("HTTP ERROR: "+String(httpCode));
    }
    http.end();                                 // HTTP通信を終了する
    return token;                               // トークンを応答
}

int message_to_line(String message){            /* LINE用 メッセージ送信部 */
    String token = get_token();                 // トークンを取得する
    int token_len = token.length();             // 取得したトークン長
    if(token_len != 174){                       // トークン長が174以外の時
        Serial.println("ERROR: Token Length Error; " + String(token_len));
        return 0;
    }
    HTTPClient http;                            // HTTPリクエスト用インスタンス
    String url = "https://api.line.me/v2/bot/message/broadcast";
    Serial.println(url);                        // 送信URLを表示
    http.begin(url);                            // HTTPリクエスト先を設定する
    http.addHeader("Content-Type","application/json");
    http.addHeader("Authorization","Bearer " + token);
    String json = "{\"messages\":[{\"type\":\"text\",\"text\":\"";
    json += message + "\"}]}";                  // メッセージを送信用jsonに代入
    Serial.println(json);                       // HTTP送信内容を表示する
    int httpCode = http.POST(json);             // HTTPでメッセージを送信する
    http.end();                                 // HTTP通信を終了する
    return httpCode;                            // HTTPリザルトを応答
}

void setup(){                                   // 起動時に一度だけ実行する関数
    Serial.begin(115200);                       // 動作確認のためのシリアル出力
    Serial.println("LINE Messaging API");       // シリアル出力表示

    WiFi.mode(WIFI_STA);                        // 無線LANをSTAモードに設定
    WiFi.begin(SSID,PASS);                      // 無線LANアクセスポイント接続
    while(WiFi.status() != WL_CONNECTED){       // 接続に成功するまで待つ
        Serial.print('.');                      // 接続試行中シリアル出力
        delay(500);                             // 待ち時間処理
    }
    Serial.println(WiFi.localIP());             // 本機のアドレスをシリアル出力
}

void loop(){                                    // 繰り返し実行する関数
    message_to_line("[esp32] こんにちは");      // メッセージ送信「こんにちは」
    delay(10*60*1000);                          // 10分の待ち時間処理
}

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
