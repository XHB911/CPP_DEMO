#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leptjson.h"

using namespace leptjson;

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
	do { \
		++ test_count; \
		if (equality) \
			++ test_pass; \
		else { \
			fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual); \
			main_ret = 1; \
		} \
	} while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")
#define EXPECT_EQ_STRING(expect, actual, alength) \
    EXPECT_EQ_BASE(sizeof(expect) - 1 == alength && memcmp(expect, actual, alength) == 0, expect, actual, "%s")
#define EXPECT_TRUE(actual) EXPECT_EQ_BASE((actual) != 0, "true", "false", "%s")
#define EXPECT_FALSE(actual) EXPECT_EQ_BASE((actual) == 0, "false", "true", "%s")

#define TEST_ERROR(error, json) \
	do { \
		lept_value v; \
		v.type = JSON_FALSE; \
		EXPECT_EQ_INT(error, lept_json::parse(v, json)); \
		EXPECT_EQ_INT(JSON_NULL, lept_json::get_type(v)); \
	} while(0)

static void test_parse_null() {
	lept_value v;
	v.type = JSON_FALSE;
	EXPECT_EQ_INT(LEPT_PARSE_OK, lept_json::parse(v, "null"));
	EXPECT_EQ_INT(JSON_NULL, lept_json::get_type(v));
}

#define TEST_NUMBER(expect, json) \
	do { \
		lept_value v; \
		EXPECT_EQ_INT(LEPT_PARSE_OK, lept_json::parse(v, json)); \
		EXPECT_EQ_INT(JSON_NUMBER, lept_json::get_type(v)); \
		EXPECT_EQ_DOUBLE(expect, lept_json::get_number(v)); \
	} while(0)

static void test_parse_true() {
	lept_value v;
	v.type = JSON_FALSE;
	EXPECT_EQ_INT(LEPT_PARSE_OK, lept_json::parse(v, "true"));
	EXPECT_EQ_INT(JSON_TRUE, lept_json::get_type(v));
}

static void test_parse_false() {
	lept_value v;
	v.type = JSON_TRUE;
	EXPECT_EQ_INT(LEPT_PARSE_OK, lept_json::parse(v, "false"));
	EXPECT_EQ_INT(JSON_FALSE, lept_json::get_type(v));
}

static void test_parse_expect_value() {
	TEST_ERROR(LEPT_PARSE_EXPECT_VALUE, "");
	TEST_ERROR(LEPT_PARSE_EXPECT_VALUE, " ");
}

static void test_parse_invalid_value() {
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "nul");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "?");

    /* invalid number */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "nan");
}

static void test_parse_root_not_singular() {
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "null x");

    /* invalid number */
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' or nothing */
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x123");
}

static void test_parse_number_too_big() {
    TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG, "-1e309");
}

static void test_parse_number() {
	TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
	TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
	TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
	TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
	TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
	TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
	TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
	TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
	TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
	TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

static void test_access_string() {
    lept_value v;
	lept_json::init(v);
	lept_json::set_string(v, "", 0);
    EXPECT_EQ_STRING("", lept_json::get_string(v), lept_json::get_string_length(v));
	lept_json::set_string(v, "Hello", 5);
    EXPECT_EQ_STRING("Hello", lept_json::get_string(v), lept_json::get_string_length(v));
	lept_json::set_null(v);
}

static void test_access_boolean() {
    lept_value v;
	lept_json::init(v);
	lept_json::set_string(v, "a", 1);
	lept_json::set_boolean(v, 1);
    EXPECT_TRUE(lept_json::get_boolean(v));
	lept_json::set_boolean(v, 0);
    EXPECT_FALSE(lept_json::get_boolean(v));
	lept_json::set_null(v);
}

static void test_access_number() {
    lept_value v;
	lept_json::init(v);
	lept_json::set_string(v, "a", 1);
	lept_json::set_number(v, 1234.5);
    EXPECT_EQ_DOUBLE(1234.5, lept_json::get_number(v));
	lept_json::set_null(v);
}

static void test_parse() {
	test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
	test_parse_number();
	test_parse_invalid_value();
	test_parse_number_too_big();
	test_access_number();
	test_access_boolean();
	test_access_string();
}

int main() {
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}