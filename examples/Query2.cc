#include <Misc/Query.h>
#include <Misc/dbconnect.h>

#include <iostream>
#include <sstream>

int main()
{
#if 0
  SQLerror e("test",100,"test2");
  std::cout << e << '\n';

  std::stringstream ss;
  ss << e;
#endif

  ManuProC::Connection conn("localhost", "test");
  conn.setType(ManuProC::Connection::C_PostgreSQL);
  ManuProC::dbconnect(conn);

  Query u_ok("UPDATE friends SET ort = 'Stanford' WHERE vorname = 'Willy'");
  Query::Row r;
  u_ok >> r;
  std::cout << "Update " << (r.good() ? "OK" : "nicht OK")
            << " Result=" << u_ok.Result()
            << "\n";

  Query u_nok("UPDATE friends SET ort = 'Stanford' WHERE vorname = 'XXX'");
  u_nok >> r;
  std::cout << "Update " << (r.good() ? "OK" : "nicht OK")
            << " Result=" << u_nok.Result()
            << "\n";

  return 0;
}
