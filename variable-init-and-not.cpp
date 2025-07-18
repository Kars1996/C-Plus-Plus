#include <iostream>;

void doesNothing(int&) {}
// tbh i have no idea what the & does apparently tricks 
// compiler into thinking its used


int main()
{
  int x;               // unitlized var

  doesNothing(x);      // compiler now thinks its been used

  std::cout << x << "\n"; // u get an error :c

  return 0;
}
