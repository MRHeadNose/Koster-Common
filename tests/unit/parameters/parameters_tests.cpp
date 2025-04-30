#include "gtest/gtest.h"
extern "C" {
#include "fff/fff.h"
#include "koster-common/parameters.h"
}

DEFINE_FFF_GLOBALS;

class ParametersTests : public testing::Test {
  protected:
    void SetUp() override { ParamInit(); };
    void GetEnumParam(const struct param_t** param) {
        struct param_category_t* category;
        ASSERT_EQ(ParamGetCategory(&category, 2), 0);  // Cat
        ASSERT_EQ(ParamCategoryGetNParams(category), 1);
        ASSERT_EQ(ParamCategoryGetParam(category, param, 0), 0);  // EnumParam
    }
    void GetInt32Param(const struct param_t** param) {
        struct param_category_t* category;
        ASSERT_EQ(ParamGetCategory(&category, 1), 0);  // B
        ASSERT_EQ(ParamCategoryGetNParams(category), 1);
        ASSERT_EQ(ParamCategoryGetParam(category, param, 0), 0);  // Int32Param
    }
    void GetUInt8Param(const struct param_t** param) {
        struct param_category_t* category;
        ASSERT_EQ(ParamGetCategory(&category, 0), 0);  // Ape
        ASSERT_EQ(ParamCategoryGetNParams(category), 1);
        ASSERT_EQ(ParamCategoryGetParam(category, param, 0), 0);  // UInt8Param
    }
};

TEST_F(ParametersTests, Get_ReturnsDefault) {
    ASSERT_EQ(ParamGetEnumparam(), kParamValue1);
    ASSERT_EQ(ParamGetUint8param(), 123);
    ASSERT_EQ(ParamGetInt32param(), 1337000);
}

TEST_F(ParametersTests, GetSetUInt8ParamByName) {
    // New, valid value
    ASSERT_EQ(ParamSetUint8param(150), 0);
    ASSERT_EQ(ParamGetUint8param(), 150);

    // Out of range
    ASSERT_EQ(ParamSetUint8param(99), -1);
    ASSERT_EQ(ParamSetUint8param(201), -1);
    ASSERT_EQ(ParamGetUint8param(), 150);

    // Valid, minimum
    ASSERT_EQ(ParamSetUint8param(100), 0);
    ASSERT_EQ(ParamGetUint8param(), 100);

    // Valid, maximum
    ASSERT_EQ(ParamSetUint8param(200), 0);
    ASSERT_EQ(ParamGetUint8param(), 200);
}

TEST_F(ParametersTests, GetSetInt32ParamByName) {
    // New, valid value
    ASSERT_EQ(ParamSetInt32param(1500000), 0);
    ASSERT_EQ(ParamGetInt32param(), 1500000);

    // Out of range
    ASSERT_EQ(ParamSetInt32param(999999), -1);
    ASSERT_EQ(ParamSetInt32param(2000001), -1);
    ASSERT_EQ(ParamGetInt32param(), 1500000);
}

TEST_F(ParametersTests, GetSetEnumParamByName) {
    // New, valid value
    ASSERT_EQ(ParamSetEnumparam(kParamValue2), 0);
    ASSERT_EQ(ParamGetEnumparam(), kParamValue2);

    // Out of range
    ASSERT_EQ(ParamSetEnumparam(kParamN_enum_t), -1);
    ASSERT_EQ(ParamGetEnumparam(), kParamValue2);
}

TEST_F(ParametersTests, GetSetUInt8ParamFromCategory) {
    struct param_category_t* category;
    ASSERT_EQ(ParamGetCategory(&category, 0), 0);  // Ape
    ASSERT_EQ(ParamCategoryGetNParams(category), 1);
    const struct param_t* param;
    ASSERT_EQ(ParamCategoryGetParam(category, &param, 0), 0);  // UInt8param

    // New, valid value
    ASSERT_EQ(ParamSetValue(param, 150), 0);
    ASSERT_EQ(ParamGetValue(param), 150);

    // Out of range
    ASSERT_EQ(ParamSetValue(param, 99), -1);
    ASSERT_EQ(ParamSetValue(param, 201), -1);
    ASSERT_EQ(ParamGetValue(param), 150);
}

TEST_F(ParametersTests, GetSetInt32ParamFromCategory) {
    struct param_category_t* category;
    ASSERT_EQ(ParamGetCategory(&category, 1), 0);  // B
    ASSERT_EQ(ParamCategoryGetNParams(category), 1);
    const struct param_t* param;
    ASSERT_EQ(ParamCategoryGetParam(category, &param, 0), 0);  // Int32Param

    // New, valid value
    ASSERT_EQ(ParamSetValue(param, 1500000), 0);
    ASSERT_EQ(ParamGetValue(param), 1500000);

    // Out of range
    ASSERT_EQ(ParamSetValue(param, 999999), -1);
    ASSERT_EQ(ParamSetValue(param, 2000001), -1);
    ASSERT_EQ(ParamGetValue(param), 1500000);
}

TEST_F(ParametersTests, GetSetEnumParamFromCategory) {
    struct param_category_t* category;
    ASSERT_EQ(ParamGetCategory(&category, 2), 0);  // Cat
    ASSERT_EQ(ParamCategoryGetNParams(category), 1);
    const struct param_t* param;
    ASSERT_EQ(ParamCategoryGetParam(category, &param, 0), 0);  // EnumParam

    // New, valid value
    ASSERT_EQ(ParamSetValue(param, (int32_t)kParamValue2), 0);
    ASSERT_EQ(ParamGetValue(param), (int32_t)kParamValue2);

    // Out of range
    ASSERT_EQ(ParamSetValue(param, (int32_t)kParamN_enum_t), -1);
    ASSERT_EQ(ParamSetValue(param, -1), -1);
    ASSERT_EQ(ParamGetValue(param), (int32_t)kParamValue2);
}

TEST_F(ParametersTests, IncreaseEnumParam) {
    ASSERT_EQ(ParamGetEnumparam(), kParamValue1);
    const struct param_t* param;
    GetEnumParam(&param);
    ASSERT_EQ(ParamIncreaseValue(param), 0);
    ASSERT_EQ(ParamGetEnumparam(), kParamValue2);
}

TEST_F(ParametersTests, IncreaseEnumParam_Wrap) {
    ASSERT_EQ(ParamGetEnumparam(), kParamValue1);
    const struct param_t* param;
    GetEnumParam(&param);
    ASSERT_EQ(ParamSetEnumparam(kParamValue2), 0);
    ASSERT_EQ(ParamIncreaseValue(param), 0);
    ASSERT_EQ(ParamGetEnumparam(), kParamValue0);
}

TEST_F(ParametersTests, DecreaseEnumParam) {
    ASSERT_EQ(ParamGetEnumparam(), kParamValue1);
    const struct param_t* param;
    GetEnumParam(&param);
    ASSERT_EQ(ParamDecreaseValue(param), 0);
    ASSERT_EQ(ParamGetEnumparam(), kParamValue0);
}

TEST_F(ParametersTests, DecreaseEnumParam_Wrap) {
    ASSERT_EQ(ParamGetEnumparam(), kParamValue1);
    const struct param_t* param;
    GetEnumParam(&param);
    ASSERT_EQ(ParamSetEnumparam(kParamValue0), 0);
    ASSERT_EQ(ParamDecreaseValue(param), 0);
    ASSERT_EQ(ParamGetEnumparam(), kParamValue2);
}

TEST_F(ParametersTests, IncreaseUInt8Param) {
    ASSERT_EQ(ParamGetUint8param(), 123);
    const struct param_t* param;
    GetUInt8Param(&param);
    ASSERT_EQ(ParamIncreaseValue(param), 0);
    ASSERT_EQ(ParamGetUint8param(), 124);
}

TEST_F(ParametersTests, IncreaseUInt8Param_Wrap) {
    ASSERT_EQ(ParamGetUint8param(), 123);
    const struct param_t* param;
    GetUInt8Param(&param);
    ASSERT_EQ(ParamSetUint8param(200), 0);
    ASSERT_EQ(ParamIncreaseValue(param), 0);
    ASSERT_EQ(ParamGetUint8param(), 100);
}

TEST_F(ParametersTests, DecreaseUInt8Param) {
    ASSERT_EQ(ParamGetUint8param(), 123);
    const struct param_t* param;
    GetUInt8Param(&param);
    ASSERT_EQ(ParamDecreaseValue(param), 0);
    ASSERT_EQ(ParamGetUint8param(), 122);
}

TEST_F(ParametersTests, DecreaseUInt8Param_Wrap) {
    ASSERT_EQ(ParamGetUint8param(), 123);
    const struct param_t* param;
    GetUInt8Param(&param);
    ASSERT_EQ(ParamSetUint8param(100), 0);
    ASSERT_EQ(ParamDecreaseValue(param), 0);
    ASSERT_EQ(ParamGetUint8param(), 200);
}

TEST_F(ParametersTests, IncreaseInt32Param) {
    ASSERT_EQ(ParamGetInt32param(), 1337000);
    const struct param_t* param;
    GetInt32Param(&param);
    ASSERT_EQ(ParamIncreaseValue(param), 0);
    ASSERT_EQ(ParamGetInt32param(), 1337001);
}

TEST_F(ParametersTests, IncreaseInt32Param_Wrap) {
    ASSERT_EQ(ParamGetInt32param(), 1337000);
    const struct param_t* param;
    GetInt32Param(&param);
    ASSERT_EQ(ParamSetInt32param(2000000), 0);
    ASSERT_EQ(ParamIncreaseValue(param), 0);
    ASSERT_EQ(ParamGetInt32param(), 1000000);
}

TEST_F(ParametersTests, DecreaseInt32Param) {
    ASSERT_EQ(ParamGetInt32param(), 1337000);
    const struct param_t* param;
    GetInt32Param(&param);
    ASSERT_EQ(ParamDecreaseValue(param), 0);
    ASSERT_EQ(ParamGetInt32param(), 1336999);
}

TEST_F(ParametersTests, DecreaseInt32Param_Wrap) {
    ASSERT_EQ(ParamGetInt32param(), 1337000);
    const struct param_t* param;
    GetInt32Param(&param);
    ASSERT_EQ(ParamSetInt32param(1000000), 0);
    ASSERT_EQ(ParamDecreaseValue(param), 0);
    ASSERT_EQ(ParamGetInt32param(), 2000000);
}

TEST_F(ParametersTests, GetValueStringInt32Param) {
    const struct param_t* param;
    GetInt32Param(&param);

    char str[PARAM_VALUE_STRING_MAX_LEN];

    ASSERT_EQ(ParamGetValueString(param, str, 1337000), 0);
    ASSERT_EQ(std::string(str), "1.337000");

    ASSERT_EQ(ParamGetValueString(param, str, 1000000), 0);
    ASSERT_EQ(std::string(str), "1.000000");

    ASSERT_EQ(ParamGetValueString(param, str, 2000000), 0);
    ASSERT_EQ(std::string(str), "2.000000");

    // Out of range
    ASSERT_EQ(ParamGetValueString(param, str, 999999), -1);
    ASSERT_EQ(ParamGetValueString(param, str, 2000001), -1);
}

TEST_F(ParametersTests, GetValueStringEnumParam) {
    const struct param_t* param;
    GetEnumParam(&param);

    char str[PARAM_VALUE_STRING_MAX_LEN];

    ASSERT_EQ(ParamGetValueString(param, str, (int32_t)kParamValue0), 0);
    ASSERT_EQ(std::string(str), "Value0");

    ASSERT_EQ(ParamGetValueString(param, str, (int32_t)kParamValue1), 0);
    ASSERT_EQ(std::string(str), "Value1");

    ASSERT_EQ(ParamGetValueString(param, str, (int32_t)kParamValue2), 0);
    ASSERT_EQ(std::string(str), "Value2");

    // Out of range
    ASSERT_EQ(ParamGetValueString(param, str, -1), -1);
    ASSERT_EQ(ParamGetValueString(param, str, (int32_t)kParamN_enum_t), -1);
}
