#include "gtest/gtest.h"
extern "C" {
#include "fff/fff.h"
#include "koster-common/parameters.h"
}

DEFINE_FFF_GLOBALS;

class ParametersTests : public testing::Test {
  protected:
    void SetUp() override { ParamInit(); };
};

TEST_F(ParametersTests, Get_ReturnsDefault) {
    EXPECT_EQ(ParamGetEnumparam(), kParamValue1);
    EXPECT_EQ(ParamGetUint8param(), 123);
    EXPECT_EQ(ParamGetInt32param(), 1337000);
}

TEST_F(ParametersTests, SetGetEnumparam) {
    EXPECT_EQ(ParamSetEnumparam(kParamValue0), 0);
    EXPECT_EQ(ParamGetEnumparam(), kParamValue0);
}

TEST_F(ParametersTests, SetEnumParamOutOfRange_ReturnNegative) {
    EXPECT_EQ(ParamSetEnumparam(kParamN_enum_t), -1);
    EXPECT_EQ(ParamGetEnumparam(), kParamValue1);
}

TEST_F(ParametersTests, SetGetInt32param) {
    EXPECT_EQ(ParamSetInt32param(1999999), 0);
    EXPECT_FLOAT_EQ(ParamGetInt32param(), 1999999);
}

TEST_F(ParametersTests, SetInt32ParamOutOfRange_ReturnNegative) {
    EXPECT_EQ(ParamSetInt32param(999999), -1);
    EXPECT_EQ(ParamGetInt32param(), 1337000);
    EXPECT_EQ(ParamSetInt32param(2000001), -1);
    EXPECT_EQ(ParamGetInt32param(), 1337000);
}

TEST_F(ParametersTests, SetGetUInt8param) {
    EXPECT_EQ(ParamSetUint8param(150), 0);
    EXPECT_EQ(ParamGetUint8param(), 150);
}

TEST_F(ParametersTests, SetUint8ParamOutOfRange_ReturnNegative) {
    EXPECT_EQ(ParamSetUint8param(99), -1);
    EXPECT_EQ(ParamSetUint8param(201), -1);
    EXPECT_EQ(ParamGetUint8param(), 123);
}

TEST_F(ParametersTests, GetCategoryNameAlpha) {
    char name[PARAM_CATEGORY_NAME_MAX_LEN];
    EXPECT_EQ(ParamGetCategoryNameAlpha(0, name), 0);
    EXPECT_EQ(std::string(name), "Ape");
    EXPECT_EQ(ParamGetCategoryNameAlpha(1, name), 0);
    EXPECT_EQ(std::string(name), "B");
    EXPECT_EQ(ParamGetCategoryNameAlpha(2, name), 0);
    EXPECT_EQ(std::string(name), "Cat");
    EXPECT_EQ(ParamGetCategoryNameAlpha(3, name), -1);
}

TEST_F(ParametersTests, GetById_ReturnsDefaultValues) {
    int32_t value;
    EXPECT_EQ(ParamGet(0, &value), 0);
    EXPECT_EQ(value, kParamValue1);

    EXPECT_EQ(ParamGet(10, &value), 0);
    EXPECT_EQ(value, 123);

    EXPECT_EQ(ParamGet(2, &value), 0);
    EXPECT_EQ(value, 1337000);
}

TEST_F(ParametersTests, GetByIdIndexOutOfRange_ReturnsNegative) {
    int32_t value;
    EXPECT_EQ(ParamGet(-1, &value), -1);
    EXPECT_EQ(ParamGet(3, &value), -1);
    EXPECT_EQ(ParamGet(11, &value), -1);
}

TEST_F(ParametersTests, SetById_GetReturnsNewValues) {
    EXPECT_EQ(ParamSet(0, kParamValue0), 0);
    EXPECT_EQ(ParamSet(10, 124), 0);
    EXPECT_EQ(ParamSet(2, 1337001), 0);

    int32_t value;
    EXPECT_EQ(ParamGet(0, &value), 0);
    EXPECT_EQ(value, kParamValue0);

    EXPECT_EQ(ParamGet(10, &value), 0);
    EXPECT_EQ(value, 124);

    EXPECT_EQ(ParamGet(2, &value), 0);
    EXPECT_EQ(value, 1337001);
}

TEST_F(ParametersTests, SetEnumParamByIdOutOfRange_ReturnNegative) { EXPECT_EQ(ParamSet(0, kParamN_enum_t), -1); }

TEST_F(ParametersTests, SetInt32ParamByIdOutOfRange_ReturnNegative) {
    EXPECT_EQ(ParamSet(2, 999999), -1);
    EXPECT_EQ(ParamSet(2, 2000001), -1);
}

TEST_F(ParametersTests, SetUint8ParamByIdOutOfRange_ReturnNegative) {
    EXPECT_EQ(ParamSet(10, 99), -1);
    EXPECT_EQ(ParamSet(10, 201), -1);
}

TEST_F(ParametersTests, IncrEnumParam) {
    EXPECT_EQ(ParamGetEnumparam(), kParamValue1);
    EXPECT_EQ(ParamIncr(0), 0);
    EXPECT_EQ(ParamGetEnumparam(), kParamValue2);
}

TEST_F(ParametersTests, IncrEnumParam_Wrap) {
    EXPECT_EQ(ParamSetEnumparam(kParamValue2), 0);
    EXPECT_EQ(ParamIncr(0), 0);
    EXPECT_EQ(ParamGetEnumparam(), kParamValue0);
}

TEST_F(ParametersTests, DecrEnumParam) {
    EXPECT_EQ(ParamGetEnumparam(), kParamValue1);
    EXPECT_EQ(ParamDecr(0), 0);
    EXPECT_EQ(ParamGetEnumparam(), kParamValue0);
}

TEST_F(ParametersTests, DecrEnumParam_Wrap) {
    EXPECT_EQ(ParamSetEnumparam(kParamValue0), 0);
    EXPECT_EQ(ParamDecr(0), 0);
    EXPECT_EQ(ParamGetEnumparam(), kParamValue2);
}

TEST_F(ParametersTests, IncrUint8Param) {
    EXPECT_EQ(ParamGetUint8param(), 123);
    EXPECT_EQ(ParamIncr(10), 0);
    EXPECT_EQ(ParamGetUint8param(), 124);
}

TEST_F(ParametersTests, IncrUint8Param_Wrap) {
    EXPECT_EQ(ParamSetUint8param(200), 0);
    EXPECT_EQ(ParamIncr(10), 0);
    EXPECT_EQ(ParamGetUint8param(), 100);
}

TEST_F(ParametersTests, DecrUint8Param) {
    EXPECT_EQ(ParamGetUint8param(), 123);
    EXPECT_EQ(ParamDecr(10), 0);
    EXPECT_EQ(ParamGetUint8param(), 122);
}

TEST_F(ParametersTests, DecrUint8Param_Wrap) {
    EXPECT_EQ(ParamSetUint8param(100), 0);
    EXPECT_EQ(ParamDecr(10), 0);
    EXPECT_EQ(ParamGetUint8param(), 200);
}

TEST_F(ParametersTests, IncrInt32Param) {
    EXPECT_EQ(ParamGetInt32param(), 1337000);
    EXPECT_EQ(ParamIncr(2), 0);
    EXPECT_EQ(ParamGetInt32param(), 1337001);
}

TEST_F(ParametersTests, IncrInt32Param_Wrap) {
    EXPECT_EQ(ParamSetInt32param(2000000), 0);
    EXPECT_EQ(ParamIncr(2), 0);
    EXPECT_EQ(ParamGetInt32param(), 1000000);
}

TEST_F(ParametersTests, DecrInt32Param) {
    EXPECT_EQ(ParamGetInt32param(), 1337000);
    EXPECT_EQ(ParamDecr(2), 0);
    EXPECT_EQ(ParamGetInt32param(), 1336999);
}

TEST_F(ParametersTests, DecrInt32Param_Wrap) {
    EXPECT_EQ(ParamSetInt32param(1000000), 0);
    EXPECT_EQ(ParamDecr(2), 0);
    EXPECT_EQ(ParamGetInt32param(), 2000000);
}
