1. 退出系统：pkill -SIGUSR2 program name
2. 品种配置文件（文件存储品种，空格分隔）：
	(1) 大商所：dce_products.txt
	(2) 上期所：shfe_products.txt
	这2个文件通过shell 脚本通过上期服务器，大商所服务器的主力合约文件进行更新
3. 主力合约文件
	通过shell 脚本通过上期服务器，大商所服务器的主力合约文件进行更新
4. 每个策略在log下创建以策略名命名的子文件夹，存放该策略日志。
	该文件夹由shell脚本创建
