# M5StickC Ticker

Simple M5StickC project to show basic info for particular tickers. Tested on M5StickC Plus.

## How it looks

<img src="https://raw.githubusercontent.com/vsevolod-oparin/m5stick-ticker/main/pics/demo.jpg" width=40% height=40%>

## Libraries to install

- ArduinoJson 6.19.3
- AsyncHTTPRequest_Generic 1.8.1
- HttpCleint 2.2.0
- M5GFX 0.0.19
- M5StickC 0.2.5
- WiFi 1.2.6

## Configs to change

1. Make a copy of `ticker/secrets-fake.h` to `ticker/secrets.h` and update consts there.
2. Update wifi settings (ssid/password).
3. Update api_token. I use https://finnhub.io service. So you can grab one there.
4. You can also play with udapte timings (in milliseconds) in `ticker/constants.h`.
5. In `ticker/ticker.ino` update `tickers` array with tickers you want to track.

You're done. If you installed all libraries and updated parameters, the project is ready to run. Compile and upload, and have fun! 

P.S. Use buttons to go back and forward.
