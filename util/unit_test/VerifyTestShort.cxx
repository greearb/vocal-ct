#include "Verify.hxx"
int main()
{
    test_verify(0 == 0);  // this should pass
    test_bug(1 == 0);  // this should fail
    return test_return_code(2);
}
