#include "gtest/gtest.h"
extern "C" {
#include "fff/fff.h"
#include "koster-common/recipe.h"
}

DEFINE_FFF_GLOBALS;

class RecipeTests : public testing::Test {
  protected:
    void SetUp() override { ASSERT_EQ(RecipeInit(), 0); };
};

TEST_F(RecipeTests, RecipeInit_AddsDefaultRecipes) {
    ASSERT_EQ(RecipeGetNumRecipes(), 2);  // 2 default recipes

    // Validate recipe names
    recipe_t *recipe;
    char name[RECIPE_NAME_MAX_SIZE];
    ASSERT_EQ(RecipeGet(&recipe, 0), 0);
    ASSERT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    ASSERT_STREQ(name, "recipe 0");

    ASSERT_EQ(RecipeGet(&recipe, 1), 0);
    ASSERT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    ASSERT_STREQ(name, "recipe 1");
}

TEST_F(RecipeTests, RecipeDelete_DeleteLastRecipe) {
    recipe_t *recipe;
    char name[RECIPE_NAME_MAX_SIZE];

    // Delete recipe 1
    ASSERT_EQ(RecipeGet(&recipe, 1), 0);
    ASSERT_EQ(RecipeDelete(recipe), 0);

    ASSERT_EQ(RecipeGetNumRecipes(), 1);

    ASSERT_EQ(RecipeGet(&recipe, 0), 0);
    ASSERT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    ASSERT_STREQ(name, "recipe 0");

    ASSERT_EQ(RecipeGet(&recipe, 1), -1);
}

TEST_F(RecipeTests, RecipeDelete_DeleteFirstRecipe) {
    recipe_t *recipe;
    char name[RECIPE_NAME_MAX_SIZE];

    // Delete recipe 0
    ASSERT_EQ(RecipeGet(&recipe, 0), 0);
    ASSERT_EQ(RecipeDelete(recipe), 0);

    ASSERT_EQ(RecipeGetNumRecipes(), 1);

    ASSERT_EQ(RecipeGet(&recipe, 0), 0);
    ASSERT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    ASSERT_STREQ(name, "recipe 1");

    ASSERT_EQ(RecipeGet(&recipe, 1), -1);
}

TEST_F(RecipeTests, RecipeNew_AddMaxRecipes) {
    ASSERT_EQ(RecipeGetNumRecipes(), 2);  // 2 default recipes

    recipe_t *recipe;
    for (int i = 0; i < RECIPE_MAX_RECIPES - 2; ++i) {
        ASSERT_EQ(RecipeNew(&recipe), 0);
        ASSERT_EQ(RecipeGetNumRecipes(), i + 3);
    }
    ASSERT_EQ(RecipeNew(&recipe), -1);
}

TEST_F(RecipeTests, AddGetAndDeleteAFew) {
    ASSERT_EQ(RecipeGetNumRecipes(), 2);  // 2 default recipes

    // Create 4 recipes
    recipe_t *recipe;
    ASSERT_EQ(RecipeNew(&recipe), 0);
    ASSERT_EQ(RecipeSetName(recipe, "IR recipe"), 0);
    ASSERT_EQ(RecipeSetType(recipe, kRecipeIR), 0);
    ASSERT_EQ(RecipeSetPyroOffPower(recipe, 0, 1), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 0, 2), 0);
    ASSERT_EQ(RecipeSetPyroOffPower(recipe, 1, 3), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 1, 4), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 0, 5), 0);
    ASSERT_EQ(RecipeSetPyroOnRise(recipe, 0, 6), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 0, 7), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 1, 8), 0);
    ASSERT_EQ(RecipeSetPyroOnRise(recipe, 1, 9), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 1, 10), 0);

    ASSERT_EQ(RecipeNew(&recipe), 0);
    ASSERT_EQ(RecipeSetName(recipe, "UV recipe"), 0);
    ASSERT_EQ(RecipeSetType(recipe, kRecipeUV), 0);
    ASSERT_EQ(RecipeSetUVTime(recipe, 11), 0);

    ASSERT_EQ(RecipeNew(&recipe), 0);
    ASSERT_EQ(RecipeSetName(recipe, "3 step IR"), 0);
    ASSERT_EQ(RecipeSetType(recipe, kRecipe3StepIR), 0);
    ASSERT_EQ(RecipeSetPyroOffPower(recipe, 0, 12), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 0, 13), 0);
    ASSERT_EQ(RecipeSetPyroOffPower(recipe, 1, 14), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 1, 15), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 0, 16), 0);
    ASSERT_EQ(RecipeSetPyroOnRise(recipe, 0, 17), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 0, 18), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 1, 19), 0);
    ASSERT_EQ(RecipeSetPyroOnRise(recipe, 1, 20), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 1, 21), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 2, 22), 0);
    ASSERT_EQ(RecipeSetPyroOnRise(recipe, 2, 23), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 2, 24), 0);

    ASSERT_EQ(RecipeNew(&recipe), 0);
    ASSERT_EQ(RecipeSetName(recipe, "UVIR recipe"), 0);
    ASSERT_EQ(RecipeSetType(recipe, kRecipeUVIR), 0);
    ASSERT_EQ(RecipeSetPyroOffPower(recipe, 0, 25), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 0, 26), 0);
    ASSERT_EQ(RecipeSetPyroOffPower(recipe, 1, 27), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 1, 28), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 0, 29), 0);
    ASSERT_EQ(RecipeSetPyroOnRise(recipe, 0, 30), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 0, 31), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 1, 32), 0);
    ASSERT_EQ(RecipeSetPyroOnRise(recipe, 1, 33), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 1, 34), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 2, 35), 0);
    ASSERT_EQ(RecipeSetPyroOnRise(recipe, 2, 36), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 2, 37), 0);
    ASSERT_EQ(RecipeSetUVTime(recipe, 38), 0);

    // Read back all recipes
    char name[RECIPE_NAME_MAX_SIZE];
    ASSERT_EQ(RecipeGet(&recipe, 2), 0);
    ASSERT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    ASSERT_STREQ(name, "IR recipe");
    ASSERT_EQ(RecipeGetType(recipe), kRecipeIR);
    ASSERT_EQ(RecipeGetPyroOffPower(recipe, 0), 1);
    ASSERT_EQ(RecipeGetPyroOffTime(recipe, 0), 2);
    ASSERT_EQ(RecipeGetPyroOffPower(recipe, 1), 3);
    ASSERT_EQ(RecipeGetPyroOffTime(recipe, 1), 4);
    ASSERT_EQ(RecipeGetPyroOnTime(recipe, 0), 5);
    ASSERT_EQ(RecipeGetPyroOnRise(recipe, 0), 6);
    ASSERT_EQ(RecipeGetPyroOnTemp(recipe, 0), 7);
    ASSERT_EQ(RecipeGetPyroOnTime(recipe, 1), 8);
    ASSERT_EQ(RecipeGetPyroOnRise(recipe, 1), 9);
    ASSERT_EQ(RecipeGetPyroOnTemp(recipe, 1), 10);

    ASSERT_EQ(RecipeGet(&recipe, 3), 0);
    ASSERT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    ASSERT_STREQ(name, "UV recipe");
    ASSERT_EQ(RecipeGetType(recipe), kRecipeUV);
    ASSERT_EQ(RecipeGetUVTime(recipe), 11);

    ASSERT_EQ(RecipeGet(&recipe, 4), 0);
    ASSERT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    ASSERT_STREQ(name, "3 step IR");
    ASSERT_EQ(RecipeGetType(recipe), kRecipe3StepIR);
    ASSERT_EQ(RecipeGetPyroOffPower(recipe, 0), 12);
    ASSERT_EQ(RecipeGetPyroOffTime(recipe, 0), 13);
    ASSERT_EQ(RecipeGetPyroOffPower(recipe, 1), 14);
    ASSERT_EQ(RecipeGetPyroOffTime(recipe, 1), 15);
    ASSERT_EQ(RecipeGetPyroOnTime(recipe, 0), 16);
    ASSERT_EQ(RecipeGetPyroOnRise(recipe, 0), 17);
    ASSERT_EQ(RecipeGetPyroOnTemp(recipe, 0), 18);
    ASSERT_EQ(RecipeGetPyroOnTime(recipe, 1), 19);
    ASSERT_EQ(RecipeGetPyroOnRise(recipe, 1), 20);
    ASSERT_EQ(RecipeGetPyroOnTemp(recipe, 1), 21);
    ASSERT_EQ(RecipeGetPyroOnTime(recipe, 2), 22);
    ASSERT_EQ(RecipeGetPyroOnRise(recipe, 2), 23);
    ASSERT_EQ(RecipeGetPyroOnTemp(recipe, 2), 24);

    ASSERT_EQ(RecipeGet(&recipe, 5), 0);
    ASSERT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    ASSERT_STREQ(name, "UVIR recipe");
    ASSERT_EQ(RecipeGetType(recipe), kRecipeUVIR);
    ASSERT_EQ(RecipeGetPyroOffPower(recipe, 0), 25);
    ASSERT_EQ(RecipeGetPyroOffTime(recipe, 0), 26);
    ASSERT_EQ(RecipeGetPyroOffPower(recipe, 1), 27);
    ASSERT_EQ(RecipeGetPyroOffTime(recipe, 1), 28);
    ASSERT_EQ(RecipeGetPyroOnTime(recipe, 0), 29);
    ASSERT_EQ(RecipeGetPyroOnRise(recipe, 0), 30);
    ASSERT_EQ(RecipeGetPyroOnTemp(recipe, 0), 31);
    ASSERT_EQ(RecipeGetPyroOnTime(recipe, 1), 32);
    ASSERT_EQ(RecipeGetPyroOnRise(recipe, 1), 33);
    ASSERT_EQ(RecipeGetPyroOnTemp(recipe, 1), 34);
    ASSERT_EQ(RecipeGetPyroOnTime(recipe, 2), 35);
    ASSERT_EQ(RecipeGetPyroOnRise(recipe, 2), 36);
    ASSERT_EQ(RecipeGetPyroOnTemp(recipe, 2), 37);
    ASSERT_EQ(RecipeGetUVTime(recipe), 38);

    // Delete recipes "IR recipe"
    ASSERT_EQ(RecipeGet(&recipe, 2), 0);
    ASSERT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    ASSERT_STREQ(name, "IR recipe");
    ASSERT_EQ(RecipeDelete(recipe), 0);

    // Delete recipe "3 step IR",
    ASSERT_EQ(RecipeGet(&recipe, 3), 0);
    ASSERT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    ASSERT_STREQ(name, "3 step IR");
    ASSERT_EQ(RecipeDelete(recipe), 0);

    // Read "UV recipe"
    ASSERT_EQ(RecipeGet(&recipe, 2), 0);
    ASSERT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    ASSERT_STREQ(name, "UV recipe");
    ASSERT_EQ(RecipeGetType(recipe), kRecipeUV);
    ASSERT_EQ(RecipeGetUVTime(recipe), 11);

    // Read "UVIR recipe"
    ASSERT_EQ(RecipeGet(&recipe, 3), 0);
    ASSERT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    ASSERT_STREQ(name, "UVIR recipe");
    ASSERT_EQ(RecipeGetType(recipe), kRecipeUVIR);
    ASSERT_EQ(RecipeGetPyroOffPower(recipe, 0), 25);
    ASSERT_EQ(RecipeGetPyroOffTime(recipe, 0), 26);
    ASSERT_EQ(RecipeGetPyroOffPower(recipe, 1), 27);
    ASSERT_EQ(RecipeGetPyroOffTime(recipe, 1), 28);
    ASSERT_EQ(RecipeGetPyroOnTime(recipe, 0), 29);
    ASSERT_EQ(RecipeGetPyroOnRise(recipe, 0), 30);
    ASSERT_EQ(RecipeGetPyroOnTemp(recipe, 0), 31);
    ASSERT_EQ(RecipeGetPyroOnTime(recipe, 1), 32);
    ASSERT_EQ(RecipeGetPyroOnRise(recipe, 1), 33);
    ASSERT_EQ(RecipeGetPyroOnTemp(recipe, 1), 34);
    ASSERT_EQ(RecipeGetPyroOnTime(recipe, 2), 35);
    ASSERT_EQ(RecipeGetPyroOnRise(recipe, 2), 36);
    ASSERT_EQ(RecipeGetPyroOnTemp(recipe, 2), 37);
    ASSERT_EQ(RecipeGetUVTime(recipe), 38);
}

TEST_F(RecipeTests, RecipeTotalTimeIR) {
    recipe_t *recipe;
    ASSERT_EQ(RecipeNew(&recipe), 0);
    ASSERT_EQ(RecipeSetType(recipe, kRecipeIR), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 0, 1), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 1, 2), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 0, 10), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 1, 20), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 2, 100), 0);
    ASSERT_EQ(RecipeSetUVTime(recipe, 200), 0);

    ASSERT_EQ(RecipeTotalTime(recipe, true), 30);  // pyro on
    ASSERT_EQ(RecipeTotalTime(recipe, false), 3);  // pyro off
}

TEST_F(RecipeTests, RecipeTotalTime3StepIR) {
    recipe_t *recipe;
    ASSERT_EQ(RecipeNew(&recipe), 0);
    ASSERT_EQ(RecipeSetType(recipe, kRecipe3StepIR), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 0, 1), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 1, 2), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 0, 10), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 1, 20), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 2, 100), 0);
    ASSERT_EQ(RecipeSetUVTime(recipe, 200), 0);

    ASSERT_EQ(RecipeTotalTime(recipe, true), 130);   // pyro on
    ASSERT_EQ(RecipeTotalTime(recipe, false), 130);  // pyro off
}

TEST_F(RecipeTests, RecipeTotalTimeUv) {
    recipe_t *recipe;
    ASSERT_EQ(RecipeNew(&recipe), 0);
    ASSERT_EQ(RecipeSetType(recipe, kRecipeUV), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 0, 1), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 1, 2), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 0, 10), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 1, 20), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 2, 100), 0);
    ASSERT_EQ(RecipeSetUVTime(recipe, 200), 0);

    ASSERT_EQ(RecipeTotalTime(recipe, true), 200);   // pyro on
    ASSERT_EQ(RecipeTotalTime(recipe, false), 200);  // pyro off
}

TEST_F(RecipeTests, RecipeTotalTimeUVIR) {
    recipe_t *recipe;
    ASSERT_EQ(RecipeNew(&recipe), 0);
    ASSERT_EQ(RecipeSetType(recipe, kRecipeUVIR), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 0, 1), 0);
    ASSERT_EQ(RecipeSetPyroOffTime(recipe, 1, 2), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 0, 10), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 1, 20), 0);
    ASSERT_EQ(RecipeSetPyroOnTime(recipe, 2, 100), 0);
    ASSERT_EQ(RecipeSetUVTime(recipe, 200), 0);

    ASSERT_EQ(RecipeTotalTime(recipe, true), 230);   // pyro on
    ASSERT_EQ(RecipeTotalTime(recipe, false), 203);  // pyro off
}

TEST_F(RecipeTests, RecipeMaxTemperatureIR) {
    recipe_t *recipe;
    ASSERT_EQ(RecipeNew(&recipe), 0);
    ASSERT_EQ(RecipeSetType(recipe, kRecipeIR), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 0, 10), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 1, 20), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 2, 100), 0);

    ASSERT_EQ(RecipeMaxTemperature(recipe, true), 20);  // pyro on
    // No temperature defined for pyro off
    ASSERT_EQ(RecipeMaxTemperature(recipe, false), -1);  // pyro off
}

TEST_F(RecipeTests, RecipeMaxTemperatureUVIR) {
    recipe_t *recipe;
    ASSERT_EQ(RecipeNew(&recipe), 0);
    ASSERT_EQ(RecipeSetType(recipe, kRecipeUVIR), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 0, 10), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 1, 20), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 2, 100), 0);

    ASSERT_EQ(RecipeMaxTemperature(recipe, true), 20);  // pyro on
    // No temperature defined for pyro off
    ASSERT_EQ(RecipeMaxTemperature(recipe, false), -1);  // pyro off
}

TEST_F(RecipeTests, RecipeMaxTemperatureUV) {
    recipe_t *recipe;
    ASSERT_EQ(RecipeNew(&recipe), 0);
    ASSERT_EQ(RecipeSetType(recipe, kRecipeUV), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 0, 10), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 1, 20), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 2, 100), 0);

    // No temperature defined for UV recipe
    ASSERT_EQ(RecipeMaxTemperature(recipe, true), -1);   // pyro on
    ASSERT_EQ(RecipeMaxTemperature(recipe, false), -1);  // pyro off
}

TEST_F(RecipeTests, RecipeMaxTemperature3StepIR) {
    recipe_t *recipe;
    ASSERT_EQ(RecipeNew(&recipe), 0);
    ASSERT_EQ(RecipeSetType(recipe, kRecipe3StepIR), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 0, 10), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 1, 20), 0);
    ASSERT_EQ(RecipeSetPyroOnTemp(recipe, 2, 100), 0);

    ASSERT_EQ(RecipeMaxTemperature(recipe, true), 100);   // pyro on
    ASSERT_EQ(RecipeMaxTemperature(recipe, false), 100);  // pyro off
}
