#pragma once

template <typename T>
class IsIntegerT {
  public:
    enum { Yes = 0, No = 1};
};

template<> class IsIntegerT<int> {
	public:
	enum { Yes = 1, No = 0 };
};
