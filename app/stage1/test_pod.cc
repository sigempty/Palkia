#include <palkia/palkia.h>
#include <stdio.h>
#include <iostream>
struct Foo {
  int a;
  int b;
};

int main(int argc, char* argv[]) {
  palkia::Init(&argc, &argv);
  auto a = palkia::Remoteable<Foo>();
  auto a_ref = a.deref();
  a_ref->a = 10;
  (*a_ref).b = 42;
  std::cout << a_ref->b << std::endl;
}
