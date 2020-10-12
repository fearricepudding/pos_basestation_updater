#include <iostream>
#include "gtest/gtest.h"
#include "../src/version.h"


class VersionTest : public ::testing::Test {};
class cliTest : public ::testing::Test {};

// TEST VERSION
TEST_F(VersionTest, CheckVersionOverdated) {
  version example_one("2.2.2");
  version example_two("1.1.1");
  EXPECT_FALSE(example_one.compare(example_two));
}
TEST_F(VersionTest, CheckVersionUpToDate){
	version example_one("2.2.2");
	version example_two("2.2.2");
	EXPECT_FALSE(example_one.compare(example_two));
}
TEST_F(VersionTest, CheckVersionOutdated){
	version example_one("1.1.1");
	version example_two("2.2.2");
	EXPECT_TRUE(example_one.compare(example_two));
}
TEST_F(VersionTest, CheckLongerLatestVersion){
	version example_one("1.1.1");
	version example_two("1.1.1.1");
	EXPECT_FALSE(example_one.compare(example_two));
}
TEST_F(VersionTest, CheckLongerLocalVersion){
	version example_one("1.1.1.1");
	version example_two("1.1.1");
	EXPECT_FALSE(example_one.compare(example_two));
}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}



/**

// The fixture for testing class Foo.
class FooTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  FooTest() {
     // You can do set-up work for each test here.
  }

  ~FooTest() override {
     // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  void SetUp() override {
     // Code here will be called immediately after the constructor (right
     // before each test).
  }

  void TearDown() override {
     // Code here will be called immediately after each test (right
     // before the destructor).
  }

  // Class members declared here can be used by all tests in the test suite
  //
  
*/
