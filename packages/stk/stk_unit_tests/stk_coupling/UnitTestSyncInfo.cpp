// Copyright 2002 - 2008, 2010, 2011 National Technology Engineering
// Solutions of Sandia, LLC (NTESS). Under the terms of Contract
// DE-NA0003525 with NTESS, the U.S. Government retains certain rights
// in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of NTESS nor the names of its contributors
//       may be used to endorse or promote products derived from this
//       software without specific prior written permission.
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
// 

#include <gtest/gtest.h>                // for AssertHelper, ASSERT_TRUE, etc
#include <stk_util/parallel/Parallel.hpp>
#include <stk_coupling/Utils.hpp>
#include <stk_coupling/CommSplitting.hpp>
#include <stk_coupling/SyncInfo.hpp>
#include <stdexcept>

namespace {

TEST(UnitTestSyncInfo, get_and_set)
{
  stk::coupling::SyncInfo info;

  bool boolValue = true;
  int intValue = 3;
  double doubleValue = 3.14;
  std::string stringValue("foo");

  info.set_value("boolValue", boolValue);
  info.set_value("intValue", intValue);
  info.set_value("doubleValue", doubleValue);
  info.set_value("stringValue", stringValue);

  EXPECT_FALSE(info.has_value<bool>("garbage"));
  EXPECT_FALSE(info.has_value<int>("garbage"));
  EXPECT_FALSE(info.has_value<double>("garbage"));
  EXPECT_FALSE(info.has_value<std::string>("garbage"));

  EXPECT_TRUE(info.has_value<bool>("boolValue"));
  EXPECT_TRUE(info.has_value<int>("intValue"));
  EXPECT_TRUE(info.has_value<double>("doubleValue"));
  EXPECT_TRUE(info.has_value<std::string>("stringValue"));

  EXPECT_ANY_THROW(info.get_value<bool>("garbage"));
  EXPECT_ANY_THROW(info.get_value<int>("garbage"));
  EXPECT_ANY_THROW(info.get_value<double>("garbage"));
  EXPECT_ANY_THROW(info.get_value<std::string>("garbage"));

  EXPECT_EQ(boolValue, info.get_value<bool>("boolValue"));
  EXPECT_EQ(intValue, info.get_value<int>("intValue"));
  EXPECT_NEAR(doubleValue, info.get_value<double>("doubleValue"), 1.e-12);
  EXPECT_EQ(stringValue, info.get_value<std::string>("stringValue"));
}

TEST(UnitTestSyncInfo, exchange)
{
  if (stk::parallel_machine_size(MPI_COMM_WORLD) != 4) { return; }

  int myRank = stk::parallel_machine_rank(MPI_COMM_WORLD);
  int color = myRank < 2 ? 0 : 1;
  MPI_Comm commApp = stk::coupling::split_comm(MPI_COMM_WORLD, color);

  stk::coupling::SyncInfo myInfo;

  bool boolValue = color == 0;
  int intValue = color == 0 ? 3 : 99;
  double doubleValue = color == 0 ? 3.14 : 99.99;
  std::string stringValue("foo"+std::to_string(color));

  myInfo.set_value("boolValue", boolValue);
  myInfo.set_value("intValue", intValue);
  myInfo.set_value("doubleValue", doubleValue);
  myInfo.set_value("stringValue", stringValue);

  stk::coupling::SyncInfo otherInfo = myInfo.exchange(MPI_COMM_WORLD, commApp);

  int otherColor = color == 0 ? 1 : 0;
  bool expectedBoolValue = otherColor == 0;
  int expectedIntValue = color == 0 ? 99 : 3;
  double expectedDoubleValue = color == 0 ? 99.99 : 3.14;
  std::string expectedStringValue("foo"+std::to_string(otherColor));

  EXPECT_EQ(expectedBoolValue, otherInfo.get_value<bool>("boolValue"));
  EXPECT_EQ(expectedIntValue, otherInfo.get_value<int>("intValue"));
  EXPECT_NEAR(expectedDoubleValue, otherInfo.get_value<double>("doubleValue"), 1.e-12);
  EXPECT_EQ(expectedStringValue, otherInfo.get_value<std::string>("stringValue"));
}

}
