#ifndef _MY_INT_DEQUE_H_
#define _MY_INT_DEQUE_H_

#define MY_INT_DEQUE_CAPACITY 2048

class MyIntDeque
{
	public:
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
};


#endif
