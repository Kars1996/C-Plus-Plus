#include <iostream>


int initializatoin_tests() 
{
  // btw i dont know if this causes a compile error
  // haven't got to the functions bit of the tutorial

  int a; // no default value

  // Old initialization stuff 
  int b = 5;
  int c ( 6 );

  // new initialization stuff
  int d { 7 }; // this is better cause it is stricter?. use this
  int e {};
}

int main()
{
  int a, b;             // Init  2 variablex, a, b of int
  int width { 10 };     // Width has default var of 10
  a = 5;
  std::cout << a;

  a = 7;                // Variables are immutabel :3
  std::cout << a;
  return 0;
}
