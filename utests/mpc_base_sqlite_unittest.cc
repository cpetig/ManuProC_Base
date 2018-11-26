

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
    queryTest.m_dbType = ManuProC::Connection::C_SQLite;
    queryTest.m_conn.setType(queryTest.m_dbType);
    queryTest.m_conn.setDbase("test.db");
    ManuProC::dbconnect(queryTest.m_conn);
    std::cout << "Test DB connected\n";
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
  EXPECT_EQ(queryTest.m_dbType, q.getDBType());
}

TEST_F(mpcBaseSqliteFixture, FloatCompare) {
  GTEST_SKIP();
  // You can access data in the test fixture here.
  float f=0.2;
  int id(1);

  Query("delete from types");
  Query("insert into types (id,data) values (?,?)") << id << f;
  f=0;
  Query("select data from types where id=?") << id >> f;
  EXPECT_EQ(0.2,f);
  id = 0;
  Query("select id from types where data=?") << f >> id;
  EXPECT_EQ(1, id);
}



TEST_F(mpcBaseSqliteFixture, NoSelctedLines)
{
  float f=0.5;
  int idx=0;

  Query("delete from types");
  Query("insert into types (id,data) values (1,NULL)");


  Query q("select data from types where id=?");

  q << idx;

  try
  {
    q >> Query::Row::MapNull(f);
    EXPECT_EQ(0.0,f);
  }
  catch(SQLerror& e)
  {
    EXPECT_EQ(100,e.Code());
  }

  Query q2("select data from types where id=?");

  q2 << idx;

  try
  {
    Query::Row r;
    q2 >> r;
    if (r.good())
    {
      r >> Query::Row::MapNull(f);
      EXPECT_EQ(0.0,f);
    }
  }
  catch(SQLerror& e)
  {
    EXPECT_EQ(100,e.Code());
  }

// test what happens, if good check is ommited
  Query q3("select data from types where id=?");

  q3 << idx;

  try
  {
    Query::Row r;
    q3 >> r;
    r >> Query::Row::MapNull(f);
    EXPECT_EQ(0.0,f);
  }
  catch(SQLerror& e)
  {
    EXPECT_EQ(100,e.Code());
  }

}


}  // namespace
