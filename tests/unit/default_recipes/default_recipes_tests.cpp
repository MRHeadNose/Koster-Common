#include <string.h>

#include "gtest/gtest.h"

extern "C" {

#include "default_recipes.h"
#include "default_recipes_generated.h"
#include "fff/fff.h"
#include "parameters.h"
#include "recipe_types.h"

DEFINE_FFF_GLOBALS;
}

class DefaultRecipeTests : public testing::Test {
  protected:
    void SetUp() override {};
};

/*
 * Test that all values are generated correctly from test_default_recipes.xml
 */
TEST_F(DefaultRecipeTests, DefaultRecipesGet_RecipeValuesAreCorrect) {
    struct recipes_t recipes;
    DefaultRecipesGet(&recipes);

    ASSERT_EQ(recipes.n_recipes, 5);

    // IDs
    ASSERT_EQ(recipes.recipes[0].id, 0);
    ASSERT_EQ(recipes.recipes[1].id, 1);
    ASSERT_EQ(recipes.recipes[2].id, 2);
    ASSERT_EQ(recipes.recipes[3].id, 3);
    ASSERT_EQ(recipes.recipes[4].id, 4);

    // Names
    ASSERT_EQ(std::string{recipes.recipes[0].name}, "Filler bright");
    ASSERT_EQ(std::string{recipes.recipes[1].name}, "UV Filler");
    ASSERT_EQ(std::string{recipes.recipes[2].name}, "UV Putty");
    ASSERT_EQ(std::string{recipes.recipes[3].name}, "Special Glue");
    ASSERT_EQ(std::string{recipes.recipes[4].name}, "Special IR-UV clear coat");

    // UV
    ASSERT_EQ(recipes.recipes[0].uv_time, 0);
    ASSERT_EQ(recipes.recipes[1].uv_time, 360);
    ASSERT_EQ(recipes.recipes[2].uv_time, 300);
    ASSERT_EQ(recipes.recipes[3].uv_time, 0);
    ASSERT_EQ(recipes.recipes[4].uv_time, 0);

    // Pyro Off
    ASSERT_EQ(recipes.recipes[0].pyro_off_time[0], 480);
    ASSERT_EQ(recipes.recipes[0].pyro_off_time[1], 420);
    ASSERT_EQ(recipes.recipes[0].pyro_off_power[0], 40);
    ASSERT_EQ(recipes.recipes[0].pyro_off_power[1], 60);
    ASSERT_EQ(recipes.recipes[1].pyro_off_time[0], 0);
    ASSERT_EQ(recipes.recipes[1].pyro_off_time[1], 0);
    ASSERT_EQ(recipes.recipes[1].pyro_off_power[0], 0);
    ASSERT_EQ(recipes.recipes[1].pyro_off_power[1], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_off_time[0], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_off_time[1], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_off_power[0], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_off_power[1], 0);
    ASSERT_EQ(recipes.recipes[3].pyro_off_time[0], 0);
    ASSERT_EQ(recipes.recipes[3].pyro_off_time[1], 0);
    ASSERT_EQ(recipes.recipes[3].pyro_off_power[0], 0);
    ASSERT_EQ(recipes.recipes[3].pyro_off_power[1], 0);
    ASSERT_EQ(recipes.recipes[4].pyro_off_time[0], 300);
    ASSERT_EQ(recipes.recipes[4].pyro_off_time[1], 540);
    ASSERT_EQ(recipes.recipes[4].pyro_off_power[0], 50);
    ASSERT_EQ(recipes.recipes[4].pyro_off_power[1], 80);

    // Pyro On
    ASSERT_EQ(recipes.recipes[0].pyro_on_time[0], 480);
    ASSERT_EQ(recipes.recipes[0].pyro_on_time[1], 420);
    ASSERT_EQ(recipes.recipes[0].pyro_on_time[2], 0);
    ASSERT_EQ(recipes.recipes[0].pyro_on_rise[0], 15);
    ASSERT_EQ(recipes.recipes[0].pyro_on_rise[1], 20);
    ASSERT_EQ(recipes.recipes[0].pyro_on_rise[2], 0);
    ASSERT_EQ(recipes.recipes[0].pyro_on_temp[0], 60);
    ASSERT_EQ(recipes.recipes[0].pyro_on_temp[1], 80);
    ASSERT_EQ(recipes.recipes[0].pyro_on_temp[2], 0);
    ASSERT_EQ(recipes.recipes[1].pyro_on_time[0], 0);
    ASSERT_EQ(recipes.recipes[1].pyro_on_time[1], 0);
    ASSERT_EQ(recipes.recipes[1].pyro_on_time[2], 0);
    ASSERT_EQ(recipes.recipes[1].pyro_on_rise[0], 0);
    ASSERT_EQ(recipes.recipes[1].pyro_on_rise[1], 0);
    ASSERT_EQ(recipes.recipes[1].pyro_on_rise[2], 0);
    ASSERT_EQ(recipes.recipes[1].pyro_on_temp[0], 0);
    ASSERT_EQ(recipes.recipes[1].pyro_on_temp[1], 0);
    ASSERT_EQ(recipes.recipes[1].pyro_on_temp[2], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_on_time[0], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_on_time[1], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_on_time[2], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_on_rise[0], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_on_rise[1], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_on_rise[2], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_on_temp[0], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_on_temp[1], 0);
    ASSERT_EQ(recipes.recipes[2].pyro_on_temp[2], 0);
    ASSERT_EQ(recipes.recipes[3].pyro_on_time[0], 240);
    ASSERT_EQ(recipes.recipes[3].pyro_on_time[1], 480);
    ASSERT_EQ(recipes.recipes[3].pyro_on_time[2], 900);
    ASSERT_EQ(recipes.recipes[3].pyro_on_rise[0], 15);
    ASSERT_EQ(recipes.recipes[3].pyro_on_rise[1], 20);
    ASSERT_EQ(recipes.recipes[3].pyro_on_rise[2], 25);
    ASSERT_EQ(recipes.recipes[3].pyro_on_temp[0], 60);
    ASSERT_EQ(recipes.recipes[3].pyro_on_temp[1], 80);
    ASSERT_EQ(recipes.recipes[3].pyro_on_temp[2], 120);
    ASSERT_EQ(recipes.recipes[4].pyro_on_time[0], 241);
    ASSERT_EQ(recipes.recipes[4].pyro_on_time[1], 481);
    ASSERT_EQ(recipes.recipes[4].pyro_on_time[2], 0);
    ASSERT_EQ(recipes.recipes[4].pyro_on_rise[0], 15);
    ASSERT_EQ(recipes.recipes[4].pyro_on_rise[1], 20);
    ASSERT_EQ(recipes.recipes[4].pyro_on_rise[2], 0);
    ASSERT_EQ(recipes.recipes[4].pyro_on_temp[0], 60);
    ASSERT_EQ(recipes.recipes[4].pyro_on_temp[1], 80);
    ASSERT_EQ(recipes.recipes[4].pyro_on_temp[2], 0);
}

/*
 * Test that language overrides are applied
 */
TEST_F(DefaultRecipeTests, DefaultRecipesGet_LanguageOverrides) {
    struct recipes_t recipes;
    ParamSetLanguage(kParamSwedish);
    DefaultRecipesGet(&recipes);

    ASSERT_EQ(recipes.n_recipes, 5);

    // Names
    ASSERT_EQ(std::string{recipes.recipes[0].name}, "Filler ljus");  // overriden
    ASSERT_EQ(std::string{recipes.recipes[1].name}, "UV Filler");
    ASSERT_EQ(std::string{recipes.recipes[2].name}, "UV Putty");
    ASSERT_EQ(std::string{recipes.recipes[3].name}, "Speciallim");  // overriden
    ASSERT_EQ(std::string{recipes.recipes[4].name}, "Special IR-UV clear coat");

    ParamSetLanguage(kParamEnglish);
    DefaultRecipesGet(&recipes);

    ASSERT_EQ(recipes.n_recipes, 5);

    // Names
    ASSERT_EQ(std::string{recipes.recipes[0].name}, "Filler bright");
    ASSERT_EQ(std::string{recipes.recipes[1].name}, "UV Filler");
    ASSERT_EQ(std::string{recipes.recipes[2].name}, "UV Putty");
    ASSERT_EQ(std::string{recipes.recipes[3].name}, "Special Glue");
    ASSERT_EQ(std::string{recipes.recipes[4].name}, "Special IR-UV clear coat");
}
