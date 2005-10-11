// $Id: Interval.h,v 1.13 2005/09/26 12:38:57 christof Exp $

#ifndef TA734B8C_4F66_4FEF_BEA5_8A6C8FC2C017
#define TA734B8C_4F66_4FEF_BEA5_8A6C8FC2C017

#include <string>
#include <Misc/TimeStamp.h>

namespace ManuProC {
struct Interval
{ int days;
  int seconds;
  int microseconds;
  
  struct NoDays { NoDays() {}};
  
  Interval(const std::string &i);
  Interval() : days(), seconds(), microseconds() {}
  Interval(int d, int s, int m);
  Interval(const TimeStamp &a, const TimeStamp &b); // a<b (normally)
  
  std::string str() const;
  std::string str(const NoDays &dummy) const;
  bool operator==(const Interval &b) const;
  bool operator!() const { return !days && !seconds && !microseconds; }
  void operator+=(const Interval &b);
  Interval operator-(const Interval &b) const;
  Interval operator+(const Interval &b) const;
  bool negative() const;
  void normalize();
  Interval operator-() const;
  long Sekunden() const;
};
}

class Query_Row;
Query_Row &operator>>(Query_Row &is, ManuProC::Interval &v);
class ArgumentList;
ArgumentList &operator<<(ArgumentList &, const ManuProC::Interval &v);

#endif

