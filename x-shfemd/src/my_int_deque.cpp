#include <stdexcept>      // std::logic_error
#include "my_int_deque.h"

const int MyIntDeque::npos = -1;

MyIntDeque::MyIntDeque()
:front_index_(npos),back_index_(npos)
{
}

MyIntDeque::~MyIntDeque()
{
}

int MyIntDeque::Front()
{
	if(Empty()){
		throw logic_error("MyIntDeque is empty, and you can't access Front().");
	}else{
		return buffer_[front_index_];
	}
}

int MyIntDeque::PopFront()
{
	if(!Empty()){
		if(back_index_ > front_index_){
			int pop_value = buffer_[front_index_];
			front_index_ = front_index_ + 1;
			return pop_value;

		}else if(back_index_ == front_index_){
			int pop_value = buffer_[front_index_];
			front_index_ = npos;
			back_index_ = npos;
			return pop_value;

		}else{
			throw logic_error("MyIntDeque:logic error 'back_index_ "
				"should not be less than front_index_.'");
		}
	}else{
		throw logic_error("MyIntDeque is empty, and you can't access PopFront().");
	}
}

int MyIntDeque::Back()
{
	if(Empty()){
		throw logic_error("MyIntDeque is empty, and you can't access Back().");
	}else{
		return buffer_[back_index_];
	}
}

int MyIntDeque::Size()
{
	if(Empty()){
		throw logic_error("MyIntDeque is empty, and you can't access Size().");
	}else{
		return back_index_ - front_index_ + 1;
	}
}

int MyIntDeque::At(int position)
{
	if(Empty()){
		return 0;
	}else if(position >= Size()){
		throw logic_error("MyIntDeque:out of range accessing to At().");
	}else{
		return buffer_[front_index_+position];
	}
}

void MyIntDeque::PushBack(int data)
{
	if(Empty()){
		front_index_ = 0;
		back_index_ = 0;		
	}else{
		back_index_ = back_index_ + 1;	
	}

	buffer_[back_index_] = data;
}

void MyIntDeque::Clear()
{
	front_index_ = npos;
	back_index_ = npos;
}

bool MyIntDeque::Empty()
{
	if(front_index_==npos && back_index_==npos){
		return true;
	}else{
		return false;
	}
}
