#include <iostrea>

int main()
{
  int x { 5 };

  std::cout << "Normal printing\n";
  std::cout << x; // can also print out variables
  std::cout << "\nCan link " << "Or Chain Variables " << x;

  std::cout << "Hi" << std::endl; // end line
  // due to the wai the cpp compiler iostrea
  // std::endl is kinda inneficiant
  // so in most cases just banging a "\n" is 
  // good enough :3

}
