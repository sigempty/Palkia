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
  std::cout << "begin scope 1" << std::endl;
  {
    auto a_ref = a.deref();
    a_ref->a = 10;
    auto a_ref_clone = a_ref;
    (*a_ref).b = 42;
    a_ref_clone->a += 1;
    auto a_ref_move = std::move(a_ref);
    std::cout << a_ref_clone->a << " " << a_ref_move->b << std::endl;
  }
  std::cout << "end scope 1" << std::endl;
}
