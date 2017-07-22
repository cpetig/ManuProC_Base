
#include <Misc/Query.h>
#include <iostream>
#include <sstream>

int main()
{
  ManuProC::Connection conn("","test.db");  
  ManuProC::dbconnect(conn);
  

  Query q("select col from testtab where col!=0");
  Query::Row r;
  while((q >>r).good())
  {
    int col=0;
    r >> col;
    std::cout << col << std::endl;
  }
  

  Query q2("select col from testtab where col!=?");
  q2 << 0;
  while((q2 >>r).good())
  {
    int col;
    r >> col;
    std::cout << col << std::endl;
  }  
  

  Query q3("select col from testtab");
  while((q3 >>r).good())
  {
    int col;
    r >> col;
    
    std::cout << col << std::endl;
    
    Query qin("select col from testtab where col=?");
    qin << col;
    while((qin>>r).good())
      {
       int col2;
        r >> col2;
        std::cout << col2 << std::endl;
      }

  }    
 
  ManuProC::dbdisconnect();
  return 0;
}
