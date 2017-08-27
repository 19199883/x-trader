#pragma once

struct Log1{
};

struct Log2{
};

struct Log1Ex{
	int strategy_id;
	struct Log1 content;
};

struct Log2Ex{
	int strategy_id;
	struct Log2 content;
};

class StrategyLog
{ 
	public:
		void static Log1(int strategy_id, Log1 &content);
		void static Log2(int strategy_id, Log2 &content);
		void Save();

	private:
		static int log1_cnt_;
		static struct Log1Ex log1_[8388608];
		static int log2_cnt_;
		static struct Log2Ex log2_[8388608];
};
