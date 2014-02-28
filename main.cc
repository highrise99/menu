#include <iostream>
#include "menu.h"
int main() {
  Menu foo("root.xml", "foo");
  if(!foo.RequestOrDie()) return 1;
}
