#include <iostrea>
#include <limits>

int main() {
  std::cout << "Hello World!";
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::cin.get();
  return 0;
}
