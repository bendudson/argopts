#include "gtest/gtest.h"

#include "argopts.hxx"

TEST(StringStoreTests, StringTest) {
  ArgOpts::StringStore s("sometext42");
  std::string str = s;
  ASSERT_EQ(str, "sometext42");
}

TEST(StringStoreTests, UnicodeStringTest) {
  ArgOpts::StringStore s("大家好");
  std::string str = s;
  
  EXPECT_GT(str.length(), 3);
  ASSERT_EQ(str, "大家好");  
}

TEST(StringStoreTests, EmptyTestFail) {
  ArgOpts::StringStore s;
  ASSERT_ANY_THROW(std::string str = s;);
}

TEST(StringStoreTests, IntTest) {
  ArgOpts::StringStore s("42");

  int val = s;
  ASSERT_EQ(val, 42);
}

TEST(StringStoreTests, IntTestFail) {
  ArgOpts::StringStore s("flibble");
  ASSERT_ANY_THROW ( int val = s; );
}

TEST(StringStoreTests, DoubleTest) {
  ArgOpts::StringStore s("3.1415");
  double val = s;
  ASSERT_DOUBLE_EQ(val, 3.1415);
}

TEST(StringStoreTests, DoubleToIntFail) {
  ArgOpts::StringStore s("3.1415");
  ASSERT_ANY_THROW ( int val = s; );
}

TEST(StringStoreTests, ConstructDoubleTest) {
  ArgOpts::StringStore s(32.726);
  double val = s;
  ASSERT_DOUBLE_EQ(val, 32.726);
}

///////////////////////////////////////////////////

TEST(ParserSimpleTests, EmptyParseTest) {
  char** argv = {};
  auto args = ArgOpts::Parser().parse(0, argv);

  ASSERT_EQ( args.size(), 0 );
}

TEST(ParserSimpleTests, SingleShortArg) {
  const char* argv[] = {"somecode", "-a"};
  auto args = ArgOpts::Parser().parse(2, const_cast<char**>(argv));

  ASSERT_EQ( args.size(), 1 );

  ArgOpts::Option& opt = args.front();
  EXPECT_EQ( opt.shortopt, 'a' );
  EXPECT_EQ( opt.longopt, "" );
  EXPECT_EQ( opt.help, "" );
  EXPECT_EQ( opt.index, 1 );
  EXPECT_ANY_THROW( std::string str = opt.arg; );
}

TEST(ParserSimpleTests, SingleShortArgValue) {
  const char* argv[] = {"somecode", "-a", "value"};
  auto args = ArgOpts::Parser().parse(3, const_cast<char**>(argv));

  ASSERT_EQ( args.size(), 1 );

  ArgOpts::Option& opt = args.front();
  EXPECT_EQ( opt.shortopt, 'a' );
  EXPECT_EQ( opt.longopt, "" );
  EXPECT_EQ( opt.help, "" );
  EXPECT_EQ( opt.index, 1 );
  std::string str = opt.arg;
  EXPECT_EQ( str, "value" );
}

TEST(ParserSimpleTests, SingleShortArgEquals) {
  const char* argv[] = {"somecode", "-a=value"};
  auto args = ArgOpts::Parser().parse(2, const_cast<char**>(argv));

  ASSERT_GT( args.size(), 0 );
  EXPECT_EQ( args.size(), 1 );

  ArgOpts::Option& opt = args.front();
  EXPECT_EQ( opt.shortopt, 'a' );
  EXPECT_EQ( opt.longopt, "" );
  EXPECT_EQ( opt.help, "" );
  EXPECT_EQ( opt.index, 1 );
  std::string str = opt.arg;
  EXPECT_EQ( str, "value" );
}

TEST(ParserSimpleTests, SingleLongArg) {
  const char* argv[] = {"somecode", "--thing"};
  auto args = ArgOpts::Parser().parse(2, const_cast<char**>(argv));

  ASSERT_EQ( args.size(), 1 );

  ArgOpts::Option& opt = args.front();
  EXPECT_EQ( opt.shortopt, 0 );
  EXPECT_EQ( opt.longopt, "thing" );
  EXPECT_EQ( opt.help, "" );
  EXPECT_EQ( opt.index, 1 );
  EXPECT_ANY_THROW( std::string str = opt.arg; );
}

TEST(ParserSimpleTests, SingleLongArgEquals) {
  const char* argv[] = {"somecode", "--thing=value"};
  auto args = ArgOpts::Parser().parse(2, const_cast<char**>(argv));

  ASSERT_EQ( args.size(), 1 );

  ArgOpts::Option& opt = args.front();
  EXPECT_EQ( opt.shortopt, 0 );
  EXPECT_EQ( opt.longopt, "thing" );
  EXPECT_EQ( opt.help, "" );
  EXPECT_EQ( opt.index, 1 );
  
  std::string str = opt.arg;
  EXPECT_EQ( str, "value" );
}

TEST(ParserSimpleTests, SingleLongArgValue) {
  const char* argv[] = {"somecode", "--thing", "value"};
  auto args = ArgOpts::Parser().parse(3, const_cast<char**>(argv));

  ASSERT_EQ( args.size(), 1 );

  ArgOpts::Option& opt = args.front();
  EXPECT_EQ( opt.shortopt, 0 );
  EXPECT_EQ( opt.longopt, "thing" );
  EXPECT_EQ( opt.help, "" );
  EXPECT_EQ( opt.index, 1 );
  
  std::string str = opt.arg;
  EXPECT_EQ( str, "value" );
}

TEST(ParserSimpleTests, TwoShortArgs) {
  const char* argv[] = {"somecode", "-ab"};
  auto args = ArgOpts::Parser().parse(2, const_cast<char**>(argv));

  ASSERT_EQ( args.size(), 2 );

  ArgOpts::Option& opt = args.front();
  EXPECT_EQ( opt.shortopt, 'a' );
  EXPECT_EQ( opt.longopt, "" );
  EXPECT_EQ( opt.help, "" );
  EXPECT_EQ( opt.index, 1 );
  EXPECT_ANY_THROW( std::string str = opt.arg; );

  opt = args.back();
  EXPECT_EQ( opt.shortopt, 'b' );
  EXPECT_EQ( opt.longopt, "" );
  EXPECT_EQ( opt.help, "" );
  EXPECT_EQ( opt.index, 1 );
  EXPECT_ANY_THROW( std::string str = opt.arg; );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
