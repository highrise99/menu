#include <cstdlib>
#include <iostream>
#include "new_menu.h"
using namespace std;
int main() {
  Menu foo("root.xml", "foo");
  switch (foo.LoadOrDie()) {
    case 0: {
      cout <<
      "Fatal error on line " <<
      foo.line() <<
      " and column " <<
      foo.column() <<
      ". Press Enter to exit.";
      cin.ignore();
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
