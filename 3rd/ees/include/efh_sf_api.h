
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
	unsigned int	m_sequence;				///<�Ự���
	char			m_exchange_id;			///<�г�  0 ��ʾ�н�  1��ʾ����
	char			m_channel_id;			///<ͨ�����
	char			m_quote_flag;			///<�����־  0 ��time sale,��lev1, 
	///           1 ��time sale,��lev1, 
	///           2 ��time sale,��lev1, 
	///           3 ��time sale,��lev1
	char			m_symbol[8];			///<��Լ
	char			m_update_time[9];		///<������ʱ��(��)
	int				m_millisecond;			///<������ʱ��(����)

	double			m_last_px;				///<���¼�
	int				m_last_share;			///<���³ɽ���
	double			m_total_value;			///<�ɽ����
	union sl_pos			m_total_pos;			///<�ֲ���
	double			m_bid_px;				///<�������
	int				m_bid_share;			///<��������
	double			m_ask_px;				///<��������
	int				m_ask_share;			///<��������
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

/// ������efh quote ���
struct sl_efh_quote
{
	char					m_eth_id[32] ;		/// �����鲥�������������� �� eth1 eth2 �ȡ�
	char					m_mc_ip[32] ;		/// �鲥�ĵ�ַ
	unsigned short			m_mc_port ;			/// �鲥�Ķ˿�
	int						m_status ;			/// ״̬
	char					m_err_text[128] ;	/// �����ı�
};


/// ��������Ļص�
typedef void  (*sl_fun_on_efh_quote)(struct sl_efh_quote* p_efh, const struct guava_udp_normal* p_quote ) ;


/// ����һ�� efh sf api 
/// Parameter: const char * p_eth_id			�����鲥�������������� �� eth1 eth2 �ȡ�
/// Parameter: const char * p_mc_ip				�鲥�ĵ�ַ
/// Parameter: const unsigned short mc_port		�鲥�Ķ˿�
/// Return:	const struct  sl_efh_quote*			���ش����Ľ��
const struct  sl_efh_quote* sl_create_efh_sf_api(const char*  p_eth_id, const char * p_mc_ip, const unsigned short mc_port ) ; 

/// ��ʼ�����鲥����
/// Parameter: struct sl_efh_quote * p_SlEfh	efh quote ���
/// Parameter: sl_fun_on_efh_quote p_fun		����Ļص�������ַ
/// Return:		int			���ش�����
int sl_start_etf_quote( struct sl_efh_quote* p_SlEfh, sl_fun_on_efh_quote p_fun ) ; 

/// ֹͣ�����鲥����
/// Parameter: struct sl_efh_quote * p_SlEfh	efh quote ���
/// Return:		int			���ش�����
int sl_stop_efh_quote( struct sl_efh_quote* p_SlEfh ) ;

