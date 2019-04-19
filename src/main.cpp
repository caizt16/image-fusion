#include "Test.h"

int main() {
  // Test *test1 = new Test("test1", "jpg");
  // test1->getMasked();
  // test1->getGrad();
  // test1->getFused(40, 20);
  // test1->saveResult();
  Test *test2 = new Test("test2", "png");
  test2->getMasked();
  test2->getGrad();
  test2->getFused(150, 150);
}
