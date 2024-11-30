# line

LINE Messaging API を使って ESP32 や M5Stack から簡単にメッセージを送信してみよう

## ESP32 で LINE にメッセージを送信する

LINE Messaging API を使用するにはLINE公式アカウントを取得し、Messaging API を有効に設定し、APIに接続するための Channel 情報 (Channel ID と Channel secret) を取得します。  

以下、その手順を説明します。  

### LINE公式アカウントの取得

LINE Messaging APIを使うには LINE公式アカウントが必要です。
個人でも取得できますし、無料枠(メッセージ200件まで)もあります。  

LINE公式アカウントの取得：
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


## (参考情報) Raspberry Pi を使った方法

![LINEにメッセージを通知する](https://bokunimo.net/blog/wp-content/uploads/2024/11/line.jpg)  

[https://bokunimo.net/blog/information/5270/](https://bokunimo.net/blog/information/5270/)  

## GitHub Pages (This Document)

* [https://git.bokunimo.com/line/](https://git.bokunimo.com/line/)

by <https://bokunimo.net>
