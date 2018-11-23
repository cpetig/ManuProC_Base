

#include "mpc_base_sqlite.hh"

#include "gtest/gtest.h"

namespace {

// To use a test fixture, derive a class from testing::Test.
class mpcBaseSqliteFixture : public testing::Test {
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the variables.
  // Otherwise, this can be skipped.
  virtual void SetUp() {
    ManuProC::dbconnect(queryTest.m_conn);
    std::cout << "Test DB connectd\n";
  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
    ManuProC::dbdisconnect();
    std::cout << "Test DB disconnected\n";
  }


  // Declares the variables your tests want to use.
  mpcBaseSqliteTest queryTest;

};

// When you have a test fixture, you define a test using TEST_F
// instead of TEST.

// Tests the default c'tor.
TEST_F(mpcBaseSqliteFixture, DefaultConstructor) {
  // You can access data in the test fixture here.
  Query q;
  EXPECT_EQ(ManuProC::Connection::C_SQLite, q.getDBType());
}


}  // namespace
