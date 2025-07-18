#include <iostream>
#include <string>
using namespace std;


int better_input()
{
  string a {}; // SO BASICALLY
               // A string is not a primative data type like
               // int float char etc
               // so it has to be imported from the "string"
               // lib. but since chatgpt is so cool
               // it told me that if i use the lil line
               // "using namespace std;" to access it!
               // pretty cool right
  std::getline(std::cin << "Please enter a phrase ", a);
  std::cout << "You inputed" << a << "\n";
  // btw the whole "getline" shit is used to get the entire
  // line including spaces
  return 0;
}


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

  // INPUT TIME !!!1
  std::cout << "Plz enter number ";
  int y{};                             // empty variable init
  std::cin >> y;                       // put user input into variable
  std::cout << "you entered" << y << "\n";

  return 0;
}

