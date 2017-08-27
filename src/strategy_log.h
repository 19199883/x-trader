#pragma once

struct Log1{
};

struct Log2{
};

class StrategyLog
{ 
	public:
		void static Log1(int strategy_id, Log1 &content);
		void static Log2(int strategy_id, Log2 &content);
		void Save();

	private:
		static struct Log1 log1_[8388608];
		static struct Log2 log2_[8388608];
};
