#include <iostream>
#include <memory>

int main() {
  int *p = new int (1);
  {
    std::unique_ptr<int> p2(p);
    std::unique_ptr<int> p3;
    p3.reset(p);

  }
  if (p) {
    std::cout << "p is not null" << std::endl;
  } else {
    std::cout << "p is null" << std::endl;
  }
  return 0;
}
