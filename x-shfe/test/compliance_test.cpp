// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// A sample program demonstrating using Google C++ testing framework.
//
// Author: wan@google.com (Zhanyong Wan)


// This sample shows how to write a simple unit test for a function,
// using Google C++ testing framework.
//
// Writing a unit test using Google C++ testing framework is easy as 1-2-3:


// Step 1. Include necessary header files such that the stuff your
// test logic needs is declared.
//
// Don't forget gtest.h, which declares the testing framework.

#include "./compliance.h"
#include "gtest/gtest.h"


// Step 2. Use the TEST macro to define your tests.
//
// TEST has two parameters: the test case name and the test name.
// After using the macro, you should define your test logic between a
// pair of braces.  You can use a bunch of macros to indicate the
// success or failure of a test.  EXPECT_TRUE and EXPECT_EQ are
// examples of such macros.  For a complete list, see gtest.h.
//
// <TechnicalDetails>
//
// In Google Test, tests are grouped into test cases.  This is how we
// keep test code organized.  You should put logically related tests
// into the same test case.
//
// The test case name and the test name should both be valid C++
// identifiers.  And you should not use underscore (_) in the names.
//
// Google Test guarantees that each test you define is run exactly
// once, but it makes no guarantee on the order the tests are
// executed.  Therefore, you should write your tests in such a way
// that their results don't depend on their order.
//
// </TechnicalDetails>


// 

/*
 * 验证构造函数
 */ 
TEST(ComplianceTest, Constructor) {
	Compliance com;

	ASSERT_STREQ("", contracts_[0]);
	ASSERT_STREQ("", contracts_[2]);
	ASSERT_STREQ("", contracts_[MAX_CONTRACT_NUMBER - 1]);

	EXPECT_EQ(0, cur_cancel_times_[0]);
	EXPECT_EQ(0, cur_cancel_times_[1]);
	EXPECT_EQ(0, cur_cancel_times_[MAX_CONTRACT_NUMBER - 1]);

	EXPECT_FALSE(ord_buffer_[0].valid);
	EXPECT_FALSE(ord_buffer_[COUNTER_UPPER_LIMIT - 1].valid);
}

/*
 * 首次下单
 *
 */ 
TEST(ComplianceTest, TryReqOrderInsert_MatchWithSelf_1) {
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);
}

/*
 * 下2次单，是价格相等情况下的自成交
 */
TEST(ComplianceTest, TryReqOrderInsert_MathWithSelf_2) 
{
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(2, "rb1805", 3470.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_FALSE(rtn);
}

/*
 *
 * 下2次单，是价格不等情况下的自成交
 */
TEST(ComplianceTest, TryReqOrderInsert_MatchWithSelf_3) {
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(2, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_FALSE(rtn);
}

/*
 *
 * 下2次单，价格不满足自成交条件
 */
TEST(ComplianceTest, TryReqOrderInsert_MatchWithSelf_4) {
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(2, "rb1805", 3471.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);
}


/*
 * 其它都满足自成交，只是合约不同
 */
TEST(ComplianceTest, TryReqOrderInsert_MatchWithSelf_4) 
{
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(2, "rb1801", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);
}

/*
 * 下一个单，下第二个单（与第一个单自成交），下第三个单（不发生自成交）
 */
TEST(ComplianceTest, TryReqOrderInsert_MatchWithSelf_5) 
{
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(2, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_FALSE(rtn);

	rtn = com.TryReqOrderInsert(3, "rb1805", 3471.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);
}

/*
 * 其它都满足自成交，只是方向不满足
 */
TEST(ComplianceTest, TryReqOrderInsert_MatchWithSelf_6) 
{
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(2, "rb1801", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);
}

/*
 * 满足自成交，看开平仓的影响
 */
TEST(ComplianceTest, TryReqOrderInsert_MatchWithSelf_7)
{
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(2, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_FALSE(rtn);

	rtn = com.TryReqOrderInsert(3, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Close);
	EXPECT_FALSE(rtn);

	rtn = com.TryReqOrderInsert(4, "rb1805", 3471.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(5, "rb1805", 3471.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Close);
	EXPECT_TRUE(rtn);
}

/*
 * 看最终状态的单是否会影响自成交
 */
TEST(ComplianceTest, TryReqOrderInsert_MatchWithSelf_8)
{
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(2, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_FALSE(rtn);

	com.End(1);
	rtn = com.TryReqOrderInsert(3, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);
}

// TODO: here
/*
 * 有多个挂单，不同合约，与第一个单自成交，与最后单自成交
 */
TEST(ComplianceTest, TryReqOrderInsert_MatchWithSelf_9) {
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(2, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_FALSE(rtn);

	com.End(1);
	rtn = com.TryReqOrderInsert(3, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);
}
/*
 * 单，结束，下单，自成交，下单，不自成交，结束等复杂场景
 */
TEST(ComplianceTest, TryReqOrderInsert_5) {
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(2, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_FALSE(rtn);

	com.End(1);
	rtn = com.TryReqOrderInsert(3, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);
}

/*
 * 证min_counter_, max_counter_
 */
TEST(ComplianceTest, TryReqOrderInsert_5) {
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(2, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_FALSE(rtn);

	com.End(1);
	rtn = com.TryReqOrderInsert(3, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);
}

/*
 * 证撤单合规
 */
TEST(ComplianceTest, TryReqOrderInsert_CancelTimes_1) {
	Compliance com;

	bool rtn = com.TryReqOrderInsert(1, "rb1805", 3470.000, USTP_FTDC_D_Buy, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);

	rtn = com.TryReqOrderInsert(2, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_FALSE(rtn);

	com.End(1);
	rtn = com.TryReqOrderInsert(3, "rb1805", 3469.000, USTP_FTDC_D_Sell, USTP_FTDC_OF_Open);
	EXPECT_TRUE(rtn);
}
//TEST(MyIntDequeTest, PopFront) {
//	MyIntDeque my_deque;
//	ASSERT_THROW(my_deque.PopFront(), std::logic_error);
//
//	my_deque.PushBack(1);
//	my_deque.PushBack(2);
//	EXPECT_EQ(1, my_deque.PopFront());
//	EXPECT_EQ(2, my_deque.PopFront());
//	EXPECT_TRUE(my_deque.Empty());
//}
//
//TEST(MyIntDequeTest, Back) {
//	MyIntDeque my_deque;
//	ASSERT_THROW(my_deque.Back(), std::logic_error);
//
//	my_deque.PushBack(1);
//	my_deque.PushBack(2);
//	EXPECT_EQ(2, my_deque.Back());
//}
//
//TEST(MyIntDequeTest, Size) {
//	MyIntDeque my_deque;
//	EXPECT_EQ(0, my_deque.Size());
//
//	my_deque.PushBack(1);
//	EXPECT_EQ(1, my_deque.Size());
//	my_deque.PushBack(2);
//	EXPECT_EQ(2, my_deque.Size());
//}
//
//TEST(MyIntDequeTest, At) {
//	MyIntDeque my_deque;
//	ASSERT_THROW(my_deque.At(0), std::logic_error);
//
//	my_deque.PushBack(1);
//	EXPECT_EQ(1, my_deque.At(0));
//	my_deque.PushBack(2);
//	EXPECT_EQ(1, my_deque.At(0));
//	EXPECT_EQ(2, my_deque.At(1));
//
//	ASSERT_THROW(my_deque.At(3), std::logic_error);
//}
//
//TEST(MyIntDequeTest, PushBack) {
//	MyIntDeque my_deque;
//	my_deque.PushBack(1);
//	EXPECT_EQ(1, my_deque.At(0));
//	EXPECT_EQ(1, my_deque.Size());
//	EXPECT_EQ(1, my_deque.Front());
//	EXPECT_EQ(1, my_deque.Back());
//
//	my_deque.PushBack(2);
//	EXPECT_EQ(1, my_deque.At(0));
//	EXPECT_EQ(2, my_deque.At(1));
//	EXPECT_EQ(2, my_deque.Size());
//	EXPECT_EQ(1, my_deque.Front());
//	EXPECT_EQ(2, my_deque.Back());
//}
//
//TEST(MyIntDequeTest, Clear) {
//	MyIntDeque my_deque;
//
//	my_deque.Clear();
//	EXPECT_TRUE(my_deque.Empty());
//
//	my_deque.PushBack(1);
//	my_deque.Clear();
//	EXPECT_TRUE(my_deque.Empty());
//
//	my_deque.PushBack(1);
//	my_deque.PushBack(1);
//	my_deque.Clear();
//	EXPECT_TRUE(my_deque.Empty());
//}
//
//TEST(MyIntDequeTest, Empty) {
//	MyIntDeque my_deque;
//	EXPECT_TRUE(my_deque.Empty());
//
//	my_deque.PushBack(1);
//	EXPECT_FALSE(my_deque.Empty());
//	my_deque.Clear();
//	EXPECT_TRUE(my_deque.Empty());
//
//	my_deque.PushBack(1);
//	my_deque.PushBack(1);
//	EXPECT_FALSE(my_deque.Empty());
//	my_deque.Clear();
//	EXPECT_TRUE(my_deque.Empty());
//}
