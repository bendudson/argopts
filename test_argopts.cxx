#include "gtest/gtest.h"

#include "argopts.hxx"

TEST(StringStoreTest, IntTest) {
  ArgOpts::StringStore s("42");

  int val = s;
  ASSERT_EQ(val, 42);
}

TEST(StringStoreTest, IntTestFail) {
  ArgOpts::StringStore s("flibble");
  ASSERT_ANY_THROW ( int val = s; );
}

TEST(StringStoreTest, DoubleTest) {
  ArgOpts::StringStore s("3.1415");
  double val = s;
  ASSERT_DOUBLE_EQ(val, 3.1415);
}

TEST(StringStoreTest, DoubleToIntFail) {
  ArgOpts::StringStore s("3.1415");
  ASSERT_ANY_THROW ( int val = s; );
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
