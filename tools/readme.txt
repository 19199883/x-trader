[wangying@localhost tools]$  tar -czvf - -C ./  monitor_trader_night.sh | openssl des3 -salt -k 617999 -out ./out/monitor_trader_night.txt
