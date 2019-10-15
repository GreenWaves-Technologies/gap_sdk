#PMSIS example repository

## To contribute, please follow a few simple rules:
### First, adopt pmsis coding style, which is basically gnu-c style with 4 character wide expanded tabs
### Second, follow freertos like hierachy for tests, mainly test_os/DDD for 0S tets, test_features for specific gap features (see freertos repository for details)
### Third, all tests and directory names must be in lower case with words separated by underscores, and the "int main(...)" function must reside in a c file with the same
name as the directory
### Finally, all test names must be of the form test_XXX.c where XXX describes what the test is actually testing
