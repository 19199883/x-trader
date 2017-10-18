#ifndef _MY_INT_DEQUE_H_
#define _MY_INT_DEQUE_H_

#define MY_INT_DEQUE_CAPACITY 2048

using namespace std;

class MyIntDeque
{
	public:
		MyIntDeque();
		virtual ~MyIntDeque();
		int Front();
		int PopFront();
		int Back();
		int Size();
		int At(int position);
		void PushBack(int data);
		void Clear();
		bool Empty();

	private:
		int buffer_[MY_INT_DEQUE_CAPACITY];
		int front_index_;
		int back_index_;
		static const int npos;
};


#endif
