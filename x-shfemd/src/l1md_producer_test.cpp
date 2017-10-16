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

#include "./l1md_producer.h"
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

TEST(L1MDProducerTest, GetLastDataImp) {
	const char *contract = "pb1712";
	int32_t dominant_contract_count = 3;
	CDepthMarketDataField buffer[12];
	int32_t buffer_size = sizeof(buffer];

	// 初始状态，还没有接收行情场景
	memset(&buffer, 0, sizeof(buffer));
	int32_t last_index = 0;
	CDepthMarketDataField* data = GetLastDataImp(contract,last_index,buffer,buffer_size,
		dominant_contract_count);
	EXPECT_EQ(NULL, data);
	
	// 满足条件合约在查找范围外，验证是否只在范伟内查找
	int32_t last_index = 4;
	memset(&buffer, 0, sizeof(buffer));
	strcpy(buffer[1].InstrumentID, contract);
	CDepthMarketDataField* data = GetLastDataImp(contract,last_index,buffer,buffer_size,
		dominant_contract_count);
	EXPECT_EQ(NULL, data);

	// 要查找的行情在最新情第一个位置(不跨越首尾连接点)
	int32_t last_index = 3;
	memset(&buffer, 0, sizeof(buffer));
	strcpy(buffer[last_index].InstrumentID, contract);
	CDepthMarketDataField* data = GetLastDataImp(contract,last_index,buffer,buffer_size,
		dominant_contract_count);
	EXPECT_NE(NULL, data);

	// 要查找的行情在最后位置(不跨越首尾连接点)
	int32_t last_index = 3;
	memset(&buffer, 0, sizeof(buffer));
	strcpy(buffer[1].InstrumentID, contract);
	CDepthMarketDataField* data = GetLastDataImp(contract,last_index,buffer,buffer_size,
		dominant_contract_count);
	EXPECT_NE(NULL, data);

	// 要查找的节点在最后位置，且位于缓存的0位置
	int32_t last_index = 2;
	memset(&buffer, 0, sizeof(buffer));
	strcpy(buffer[0].InstrumentID, contract);
	CDepthMarketDataField* data = GetLastDataImp(contract,last_index,buffer,buffer_size,
		dominant_contract_count);
	EXPECT_NE(NULL, data);

	// 要查找的节点在最新主力合约最后位置，且位于缓存的最大索引位置(跨越首尾临界点)
	int32_t last_index = 1;
	memset(&buffer, 0, sizeof(buffer));
	strcpy(buffer[sizeof(buffer) - 1].InstrumentID, contract);
	CDepthMarketDataField* data = GetLastDataImp(contract,last_index,buffer,buffer_size,
		dominant_contract_count);
	EXPECT_NE(NULL, data);


	// 最新行情位置从缓存尾部开始，且有2个目标合约相邻，验证是否查到目标合约立刻返回
	int32_t last_index = sizeof(buffer) - 1;
	memset(&buffer, 0, sizeof(buffer));
	strcpy(buffer[sizeof(buffer) - 1].InstrumentID, contract);
	buffer[sizeof(buffer) - 1].BidVolume1 = sizeof(buffer) - 1;
	strcpy(buffer[sizeof(buffer) - 2].InstrumentID, contract);
	buffer[sizeof(buffer) - 2].BidVolume1 = sizeof(buffer) - 2;
	CDepthMarketDataField* data = GetLastDataImp(contract,last_index,buffer,buffer_size,
		dominant_contract_count);
	EXPECT_NE(NULL, data);
	EXPECT_EQ(sizeof(buffer), data.BidVolume1);
}

