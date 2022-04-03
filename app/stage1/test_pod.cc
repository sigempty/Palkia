#include <palkia/palkia.h>
#include <stdio.h>
#include <iostream>
struct Foo {
  int a;
  int b;
};

int main() {
  auto a = Remoteable<Foo>();
  a->a = 10;
  (*a).b = 42;
  std::cout << a->b << std::endl;
}
