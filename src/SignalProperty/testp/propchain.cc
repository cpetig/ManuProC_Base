#include "MpcProperty.hh"
#include <sstream>

int main() {
  MpcProperty<float> InputValue;
  MpcProperty<float> OutputValue;
  MpcProperty<bool>  CriticalSituation;

  OutputValue.connect_from(InputValue);

  OutputValue.on_change().connect([&](float val) {
    std::cout << "Output: " << val << std::endl;
    if (val > 0.5f) CriticalSituation = true;
    else            CriticalSituation = false;
  });

  CriticalSituation.on_change().connect([](bool val) {
    if (val) std::cout << "Danger danger!" << std::endl;
  });

  InputValue = 0.2;
  InputValue = 0.4;
  InputValue = 0.6;

  return 0;
}
