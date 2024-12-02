# line

LINE Messaging API を使って ESP32 や M5Stack から簡単にメッセージを送信してみよう

## ESP32 で LINE にメッセージを送信する

LINE Messaging API を使用するにはLINE公式アカウントを取得し、Messaging API を有効に設定し、APIに接続するための Channel 情報 (Channel ID と Channel secret) を取得します。  

![LINEアプリにメッセージを送信](https://bokunimo.net/blog/wp-content/uploads/2024/12/line_mes_api_m5_rpi.png)  

以下、その手順を説明します。  

### LINE公式アカウントの開設

LINE Messaging APIを使うには LINE公式アカウントの開設が必要です。
個人でも開設できますし、無料枠(メッセージ200件まで)もあります。  

LINE公式アカウントの開設：
[https://entry.line.biz/start/jp/](https://entry.line.biz/start/jp/)  

### Messaging API の有効化

LINE Official Account Manager を開き、LINE公式アカウント名を選択すると、下図のような画面が開きます。  

LINE Official Account Manager：
[https://manager.line.biz/](https://manager.line.biz/)  

![APIの有効化](https://bokunimo.net/blog/wp-content/uploads/2024/11/line_mes_00_64.png)  

この画面右上の[設定]をクリックしてから、画面左側の歯車内の[Messaging API]をクリックすると、ボタン[Messaging APIを利用する]が表示されます。  

このボタンをクリックし、開発者登録やプロバイダ名の設定を行います。
プロバイダ名は、このサービスの提供者名です。自由に入力できますが、一度、設定すると変更できません。
将来的にLINE利用サービスの提供を考えている場合は、考慮した名前を入力してください。  

### Channel 情報を取得する

設定を完了すると、Channel 情報が表示されます。
これらをコピーして、それぞれをスクリプト内で定義している変数line_ch_idと変数line_ch_pwに代入してください。  

![Channel情報表示](https://bokunimo.net/blog/wp-content/uploads/2024/11/line_mes_07_64.png)  

## LINE に「こんにちは」を送信してみよう

下図は、M5Stack CORE 上のESP32用プログラムでメッセージ「こんにちは」を10分ごとに送信した時のようすです。  

![「こんにちは」を送信する](https://bokunimo.net/blog/wp-content/uploads/2024/12/ex01_basic.png)  

プログラムは下記に保存してあります。

* 10分ごとに送信するESP32用プログラム：
[https://github.com/bokunimowakaru/line/blob/master/esp32/ex01_basic/ex01_basic.ino](esp32/ex01_basic/ex01_basic.ino)  
* ボタンで送信するM5Stack用プログラム：
[https://github.com/bokunimowakaru/line/blob/master/m5_core/ex01_basic/ex01_basic.ino](m5_core/ex01_basic/ex01_basic.ino)  

プログラムの内容については、各プログラム内に記載しています。  
また、下記のブログ記事でも紹介しています。  

ESP32 や M5Stack で LINE Messaging API を使う：
[https://bokunimo.net/blog/esp/5331/](https://bokunimo.net/blog/esp/5331/)  

## ESP32 IoT操作機器 Wi-Fi ボタン

IO制御を行うIoT操作機器「Wi-Fi ボタン」にLINEへの通知機能を追加してみました。
Wi-Fi ボタンは、照明などのON/OFF操作を行うための端末です。
ON/OFF操作をするたびに、LINEに通知します。

![https://bokunimo.net/blog/wp-content/uploads/2024/12/ex02_sw_line.png](https://bokunimo.net/blog/wp-content/uploads/2024/12/ex02_sw_line.png)  

プログラムは下記に保存してあります。

* Wi-Fi ボタンESP32用プログラム：
[https://github.com/bokunimowakaru/line/blob/master/esp32/ex02_sw/](esp32/ex02_sw/)  
* Wi-Fi ボタンM5Stack用プログラム：
[https://github.com/bokunimowakaru/line/blob/master/m5_core/ex02_sw/](m5_core/ex02_sw/)  

照明側のプログラムは、別のリポジトリにあります。  

* Wi-Fi LED照明 ESP32用プログラム： 
[https://github.com/bokunimowakaru/m5/tree/master/atom/ex01_led](https://github.com/bokunimowakaru/m5/tree/master/atom/ex01_led)
* Wi-Fi LED照明 ESP32C3用プログラム： 
[https://github.com/bokunimowakaru/m5/tree/master/atom/ex01_led](https://github.com/bokunimowakaru/esp32c3/tree/master/learning/ex01_led)
* Wi-Fi LED照明 M5Stack用プログラム： 
[https://github.com/bokunimowakaru/m5/tree/master/core/ex01_led](https://github.com/bokunimowakaru/m5/tree/master/core/ex01_led)

## ESP32 IoT機器 Wi-Fi 人感センサ子機

人体などの動きを検出した時に、LINEへ通知する IoT機器 Wi-Fi 人感センサ子機の製作例を下図に示します。  

![https://bokunimo.net/blog/wp-content/uploads/2024/12/ex02_sw_line.png](https://bokunimo.net/blog/wp-content/uploads/2024/12/ex06_pir_line.png)  

プログラムは下記に保存してあります。  

* Wi-Fi ボタンESP32用プログラム： [https://github.com/bokunimowakaru/line/blob/master/esp32/ex06_pir/](esp32/ex06_pir/)  
* Wi-Fi ボタンM5Stack用プログラム： [https://github.com/bokunimowakaru/line/blob/master/m5_core/ex06_pir/](m5_core/ex06_pir/)  


## 注意事項

* メッセージ送信回数の無料枠は200回/月です。超過分は有料となります。
* プログラムの繰り返し動作や誤作動などで無料枠を超過する場合があります。
* 15分間だけ有効なステートレスチャネルアクセストークンを使用しています。
* 各サンプル・スクリプトでは、実行の度にTokenを取得しています。
* 本スクリプトでは、開設した公式アカウントに参加する全アカウントにメッセージを通知します。
* このため、送信したメッセージの内容が他者に漏洩する場合があります。

## (参考情報) Raspberry Pi を使った方法

![LINEにメッセージを通知する](https://bokunimo.net/blog/wp-content/uploads/2024/11/line.jpg)  

[https://bokunimo.net/blog/information/5270/](https://bokunimo.net/blog/information/5270/)  

## GitHub Pages (This Document)

* [https://git.bokunimo.com/line/](https://git.bokunimo.com/line/)

by <https://bokunimo.net>
