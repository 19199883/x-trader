
#pragma pack(push, 1)

struct long_pos 
{
	int		m_low;
	int		m_high;	
};

union sl_pos
{
	struct long_pos	m_dce;
	double		m_shfe;
};

struct guava_udp_normal
{
	unsigned int	m_sequence;				///<会话编号
	char			m_exchange_id;			///<市场  0 表示中金  1表示上期
	char			m_channel_id;			///<通道编号
	char			m_quote_flag;			///<行情标志  0 无time sale,无lev1, 
	///           1 有time sale,无lev1, 
	///           2 无time sale,有lev1, 
	///           3 有time sale,有lev1
	char			m_symbol[8];			///<合约
	char			m_update_time[9];		///<最后更新时间(秒)
	int				m_millisecond;			///<最后更新时间(毫秒)

	double			m_last_px;				///<最新价
	int				m_last_share;			///<最新成交量
	double			m_total_value;			///<成交金额
	union sl_pos			m_total_pos;			///<持仓量
	double			m_bid_px;				///<最新买价
	int				m_bid_share;			///<最新买量
	double			m_ask_px;				///<最新卖价
	int				m_ask_share;			///<最新卖量
};

#pragma pack(pop)

#define		SL_EFH_NO_ERROR					0 
#define		SL_EFH_OPEN_DRIVER				-1 
#define		SL_EFH_ALLOC_VPORT_ERROR		-2
#define		SL_EFH_ALLOC_ETH_NAME_ERROR		-3
#define		SL_EFH_ALLOC_PD_ERROR			-4
#define		SL_EFH_RX_MERGE_ERROR			-5
#define		SL_EFH_MEMREG_ALLOC_ERROR		-6
#define		SL_EFH_ADD_UDP_FILTER_ERROR		-7
#define		SL_EFH_CREATE_THREAD_FAILED		-8
#define		SL_EFH_FILTER_ERROR				-9
#define		SL_EFH_ADD_FILTER_ERROR			-10
#define		SL_EFH_EVENT_WAIT_ERROR			-11
#define		SL_EFH_VI_PRIM_ERROR			-12
#define		SL_EFH_EVENT_POOL_ERROR			-13

/// 创建的efh quote 结点
struct sl_efh_quote
{
	char					m_eth_id[32] ;		/// 接收组播数据网卡的名称 如 eth1 eth2 等。
	char					m_mc_ip[32] ;		/// 组播的地址
	unsigned short			m_mc_port ;			/// 组播的端口
	int						m_status ;			/// 状态
	char					m_err_text[128] ;	/// 错误文本
};


/// 接收行情的回调
typedef void  (*sl_fun_on_efh_quote)(struct sl_efh_quote* p_efh, const struct guava_udp_normal* p_quote ) ;


/// 创建一个 efh sf api 
/// Parameter: const char * p_eth_id			接收组播数据网卡的名称 如 eth1 eth2 等。
/// Parameter: const char * p_mc_ip				组播的地址
/// Parameter: const unsigned short mc_port		组播的端口
/// Return:	const struct  sl_efh_quote*			返回创建的结点
const struct  sl_efh_quote* sl_create_efh_sf_api(const char*  p_eth_id, const char * p_mc_ip, const unsigned short mc_port ) ; 

/// 开始接收组播数据
/// Parameter: struct sl_efh_quote * p_SlEfh	efh quote 结点
/// Parameter: sl_fun_on_efh_quote p_fun		行情的回调函数地址
/// Return:		int			返回错误码
int sl_start_etf_quote( struct sl_efh_quote* p_SlEfh, sl_fun_on_efh_quote p_fun ) ; 

/// 停止接收组播数据
/// Parameter: struct sl_efh_quote * p_SlEfh	efh quote 结点
/// Return:		int			返回错误码
int sl_stop_efh_quote( struct sl_efh_quote* p_SlEfh ) ;

