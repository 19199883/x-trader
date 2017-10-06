#ifndef MDCLIENT_H_
#define MDCLIENT_H_

//#define MDCLIENT_API_EXPORT  __attribute__ ((visibility("default")))

///����г�����
struct CDepthMarketDataField
{
	///������
	char	TradingDay[9];
	///���������
	char	SettlementGroupID[9];
	///������
	int	SettlementID;
	///���¼�
	double	LastPrice;
	///�����
	double	PreSettlementPrice;
	///������
	double	PreClosePrice;
	///��ֲ���
	double	PreOpenInterest;
	///����
	double	OpenPrice;
	///��߼�
	double	HighestPrice;
	///��ͼ�
	double	LowestPrice;
	///����
	int	Volume;
	///�ɽ����
	double	Turnover;
	///�ֲ���
	double	OpenInterest;
	///������
	double	ClosePrice;
	///�����
	double	SettlementPrice;
	///��ͣ���
	double	UpperLimitPrice;
	///��ͣ���
	double	LowerLimitPrice;
	///����ʵ��
	double	PreDelta;
	///����ʵ��
	double	CurrDelta;
	///����޸�ʱ��
	char	UpdateTime[9];
	///����޸ĺ���
	int	UpdateMillisec;
	///��Լ����
	char	InstrumentID[31];
	///�����һ
	double	BidPrice1;
	///������һ
	int	BidVolume1;
	///������һ
	double	AskPrice1;
	///������һ
	int	AskVolume1;
	///����۶�
	double	BidPrice2;
	///��������
	int	BidVolume2;
	///�����۶�
	double	AskPrice2;
	///��������
	int	AskVolume2;
	///�������
	double	BidPrice3;
	///��������
	int	BidVolume3;
	///��������
	double	AskPrice3;
	///��������
	int	AskVolume3;
	///�������
	double	BidPrice4;
	///��������
	int	BidVolume4;
	///��������
	double	AskPrice4;
	///��������
	int	AskVolume4;
	///�������
	double	BidPrice5;
	///��������
	int	BidVolume5;
	///��������
	double	AskPrice5;
	///��������
	int	AskVolume5;
	///ҵ��������
	char	ActionDay[9];
};

class CMdclientSpi
{
public:
	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CDepthMarketDataField *pDepthMarketData)=0;
};

class CMdclientApi
{
public:
    ///����Api����pSpiΪ�ͻ�Ӧ��ʵ�ֵ�CMdclientSpi�ӿ�ָ�룬port��ip�ֱ�Ϊ����㲥�Ķ˿ں͹㲥��ַ���������������֤����ַ�ϵͳ����Ա����ȡ��һ������¿�ʹ��ȱʡֵ��
	static CMdclientApi *Create(CMdclientSpi* pSpi,int port=10074,char* ip="");
	///�����������
	virtual bool Start() = 0;
	///ֹͣ�������
	virtual bool Stop() = 0;
	///����ĳ��Լ���飬instrumentΪ�趩�ĵĺ�Լ���롣�÷����ɶԲ�ͬ��Լ�ظ����ã������еĶ���Ӧ�����������������ǰ���
	virtual void Subscribe(char* instrument) = 0;
};

#endif
