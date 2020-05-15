x-ees-smdp:
1. 通道：盛立EES
2. 行情：上期二代行情(SHFE Market Data Platform，SMDP2.0)）,使用盛立的上期二代五档行情 
3. 该github分支的shfe-ees-lev2：
		行情是以一档行情为基准：收到一档行情后	
	去查缓存的五档行情，如有五档行情，则发送合并的行情，否则，不发送行情。
		限制：该分支行情的特征是收到一档行情才会驱动行情合并逻辑的执行；而
	一档行情是通过合约订阅文件订阅的，所以如此一来，新的合约的行情将永远收不到。

4. 废除 github 的hzp-first-send-lev2分支，将其处理行情的代码
合并到bdy-first-send-ctp-lev1（保留bdy-first-send-ctp-lev1
分支的原因是：该分支存储了最新版的zce，dce的代码）.
最新的bdy-first-send-ctp-lev1分支：其行情采用适合hzp的行情处理方式（废除bdy要求的方式，
因bdy的策略失效了）
