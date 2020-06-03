#include "gtest/gtest.h"
#include "password.h"

class PasswordTest : public ::testing::Test
{
protected:
    std::string password = "right password";
};

TEST_F(PasswordTest, CreateAndVerify)
{
    std::string hash = Password::generate_hash(password);
    bool ret = Password::check_password(password, hash);

    EXPECT_TRUE(ret);
}

TEST_F(PasswordTest, WrongVerify)
{
    std::string wrong_password = "wrong password";
    
    std::string hash = Password::generate_hash(password);
    bool ret = Password::check_password(wrong_password, hash);

    EXPECT_FALSE(ret);
}
