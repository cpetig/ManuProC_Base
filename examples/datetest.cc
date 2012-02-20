#include <iostream>
#include <Misc/Datum.h>
#include <Misc/create_parse.h>
#include <Misc/TimeStamp.h>

int main()
{
  std::cout << ManuProC::Datum::from_access("14.04.2012 00:00:00") << '\n';
  ManuProC::TimeStamp ts("2012-04-01 12:00:00+2");
  std::cout << ts << '\n';
  std::cout << time_t(ts) << ' ' << ManuProC::TimeStamp(time_t(ts)) << '\n';
  std::cout << ManuProC::TimeStamp(ManuProC::Datum(14,4,2012),12,0,0) << '\n';
  std::cout << ManuProC::parse<float>("-24.8") << '\n';
  return 0;
}
