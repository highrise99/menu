#include <cstdlib>
#include <iostream>
#include "menu.h"
using namespace std;
int main() {
  Menu foo("root.xml", "foo");
  switch (foo.RequestOrDie()) {
    case 0: {
      cout <<
      "Fatal error on line " <<
      foo.line <<
      " and column " <<
      foo.column <<
      ". Press Enter to exit.";
      return EXIT_FAILURE;
    }
    case 1: {
      return EXIT_SUCCESS;
    }
    default: {
      return EXIT_FAILURE;
    }
  }
}
