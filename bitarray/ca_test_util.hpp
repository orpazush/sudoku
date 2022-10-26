
/******************************************************************************
 *                              Test utility tool			                  *
 *                        Created by Aviv L. and Chanan K.	                  *
 *                        Modified to C++ by Chanan k.						  *
 *                              All rights reserved							  *
 ******************************************************************************/
/******************************************************************************
 * how to use this test?
 *
 * Test() is a function that accepts two parameters and checks them for equality.
 * the function also contain internal variables for statistic usage.
 *
 * BoolTest() works similarly, but accepts a single statement which returns true
 * or false, similarly to assert() function. BoolTest should be used for
 * test which examine elements which are not primitive types.
 *
 * TestSummary() should be called after all tests in test file, and will output
 * some usefull statistics about the tests executed.
 *
 * NOTE: any object of any class can use these functions as long as the these
 * operators are defined for the class: equality operators(!= , ==),
 * logical negation operator(!) and output stream operator (<<).
 * using the util fucntions with objects of class type without these operators
 * pre-defined will result in an undefined behaviour.
 ******************************************************************************/


#ifndef CA_TEST_UTIL_LIBRARY_H
#define CA_TEST_UTIL_LIBRARY_H

#include <string>
#include <iostream>


#define Test(x,y) (imp_template_check((x),(y),(__LINE__)))
#define BoolTest(x) (imp_boolean_check((x),(__LINE__)))


using namespace std;

namespace ca_test_util
{


static size_t CHECK_FAILED_COUNT = 0;
static size_t CHECK_SUCCESS_COUNT = 0;

const string RED = "\033[1;31m";
const string GREEN = "\033[1;32m";
const string YELLOW = "\033[1;33m";
const string BLUE = "\033[1;34m";
const string MAGENTA = "\033[1;35m";
const string CYAN = "\033[1;36m";
const string DEFUALT_COLOR = "\033[0m";

template<typename T>
static inline void imp_template_check(T x, T y, int line)
{
	if(x != y)
	{
		cout << endl << MAGENTA <<  "test failed in line: " << line <<  endl;
		cout << RED << "expected: " << x << endl <<
		     "actual: " << y << endl << endl << DEFUALT_COLOR;

		++CHECK_FAILED_COUNT;
	}

	else
	{
		++CHECK_SUCCESS_COUNT;
	}

}

static void TestSummary()
{
	double __RES__ = (CHECK_FAILED_COUNT + CHECK_SUCCESS_COUNT);

	cout << "OVERALL TESTS: " << (size_t)__RES__ << endl;
	cout << "SUCCESED TESTS: " << CHECK_SUCCESS_COUNT << endl;
	if (__RES__)
	{
		__RES__ = (double)((CHECK_SUCCESS_COUNT)/(__RES__));
		__RES__ *= 100;
		(__RES__ != 100) ? cout << MAGENTA << "SUCCESS RATE %: " <<  __RES__
		                 : cout << GREEN << "SUCCESS RATE %: " <<  __RES__;
		cout << DEFUALT_COLOR << endl;
	}
}


static void imp_boolean_check(bool b, int line)
{
	if(!b)
	{
		cout << endl << MAGENTA << "bool test failed in line "
		      << line << DEFUALT_COLOR << endl;
		cout << RED << "expression is false" << DEFUALT_COLOR << endl << endl;
		++CHECK_FAILED_COUNT;
	}
	else
	{
		++CHECK_SUCCESS_COUNT;
	}
}


} // namespace ca_test_util


#endif //CA_TEST_UTIL_LIBRARY_H
