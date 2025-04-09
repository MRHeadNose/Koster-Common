#include "gtest/gtest.h"
extern "C" {
#include "fff/fff.h"
#include "koster-common/recipe.h"
}

DEFINE_FFF_GLOBALS;

class RecipeTests : public testing::Test {
  protected:
    void SetUp() override { EXPECT_EQ(RecipeInit(), 0); };
};

TEST_F(RecipeTests, RecipeInit_AddsDefaultRecipes) {
    EXPECT_EQ(RecipeGetNRecipes(), 2);  // 2 default recipes

    // Validate recipe names
    recipe_t *recipe;
    char name[RECIPE_NAME_MAX_SIZE];
    EXPECT_EQ(RecipeGet(&recipe, 0), 0);
    EXPECT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    EXPECT_STREQ(name, "recipe 0");

    EXPECT_EQ(RecipeGet(&recipe, 1), 0);
    EXPECT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    EXPECT_STREQ(name, "recipe 1");
}

TEST_F(RecipeTests, RecipeDelete_DeleteLastRecipe) {
    recipe_t *recipe;
    char name[RECIPE_NAME_MAX_SIZE];

    // Delete recipe 1
    EXPECT_EQ(RecipeGet(&recipe, 1), 0);
    EXPECT_EQ(RecipeDelete(recipe), 0);

    EXPECT_EQ(RecipeGetNRecipes(), 1);

    EXPECT_EQ(RecipeGet(&recipe, 0), 0);
    EXPECT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    EXPECT_STREQ(name, "recipe 0");

    EXPECT_EQ(RecipeGet(&recipe, 1), -1);
}

TEST_F(RecipeTests, RecipeDelete_DeleteFirstRecipe) {
    recipe_t *recipe;
    char name[RECIPE_NAME_MAX_SIZE];

    // Delete recipe 0
    EXPECT_EQ(RecipeGet(&recipe, 0), 0);
    EXPECT_EQ(RecipeDelete(recipe), 0);

    EXPECT_EQ(RecipeGetNRecipes(), 1);

    EXPECT_EQ(RecipeGet(&recipe, 0), 0);
    EXPECT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    EXPECT_STREQ(name, "recipe 1");

    EXPECT_EQ(RecipeGet(&recipe, 1), -1);
}

TEST_F(RecipeTests, RecipeNew_AddMaxRecipes) {
    EXPECT_EQ(RecipeGetNRecipes(), 2);  // 2 default recipes

    recipe_t *recipe;
    for (int i = 0; i < RECIPE_MAX_RECIPES - 2; ++i) {
        EXPECT_EQ(RecipeNew(&recipe), 0);
        EXPECT_EQ(RecipeGetNRecipes(), i + 3);
    }
    EXPECT_EQ(RecipeNew(&recipe), -1);
}

TEST_F(RecipeTests, AddGetAndDeleteAFew) {
    EXPECT_EQ(RecipeGetNRecipes(), 2);  // 2 default recipes

    // Create 4 recipes
    recipe_t *recipe;
    EXPECT_EQ(RecipeNew(&recipe), 0);
    EXPECT_EQ(RecipeSetName(recipe, "IR recipe"), 0);
    EXPECT_EQ(RecipeSetType(recipe, kRecipeIR), 0);
    EXPECT_EQ(RecipeSetPyroOffPower(recipe, 0, 1), 0);
    EXPECT_EQ(RecipeSetPyroOffTime(recipe, 0, 2), 0);
    EXPECT_EQ(RecipeSetPyroOffPower(recipe, 1, 3), 0);
    EXPECT_EQ(RecipeSetPyroOffTime(recipe, 1, 4), 0);
    EXPECT_EQ(RecipeSetPyroOnTime(recipe, 0, 5), 0);
    EXPECT_EQ(RecipeSetPyroOnRise(recipe, 0, 6), 0);
    EXPECT_EQ(RecipeSetPyroOnTemp(recipe, 0, 7), 0);
    EXPECT_EQ(RecipeSetPyroOnTime(recipe, 1, 8), 0);
    EXPECT_EQ(RecipeSetPyroOnRise(recipe, 1, 9), 0);
    EXPECT_EQ(RecipeSetPyroOnTemp(recipe, 1, 10), 0);

    EXPECT_EQ(RecipeNew(&recipe), 0);
    EXPECT_EQ(RecipeSetName(recipe, "UV recipe"), 0);
    EXPECT_EQ(RecipeSetType(recipe, kRecipeUV), 0);
    EXPECT_EQ(RecipeSetUVTime(recipe, 11), 0);

    EXPECT_EQ(RecipeNew(&recipe), 0);
    EXPECT_EQ(RecipeSetName(recipe, "3 step IR"), 0);
    EXPECT_EQ(RecipeSetType(recipe, kRecipe3StepIR), 0);
    EXPECT_EQ(RecipeSetPyroOffPower(recipe, 0, 12), 0);
    EXPECT_EQ(RecipeSetPyroOffTime(recipe, 0, 13), 0);
    EXPECT_EQ(RecipeSetPyroOffPower(recipe, 1, 14), 0);
    EXPECT_EQ(RecipeSetPyroOffTime(recipe, 1, 15), 0);
    EXPECT_EQ(RecipeSetPyroOnTime(recipe, 0, 16), 0);
    EXPECT_EQ(RecipeSetPyroOnRise(recipe, 0, 17), 0);
    EXPECT_EQ(RecipeSetPyroOnTemp(recipe, 0, 18), 0);
    EXPECT_EQ(RecipeSetPyroOnTime(recipe, 1, 19), 0);
    EXPECT_EQ(RecipeSetPyroOnRise(recipe, 1, 20), 0);
    EXPECT_EQ(RecipeSetPyroOnTemp(recipe, 1, 21), 0);
    EXPECT_EQ(RecipeSetPyroOnTime(recipe, 2, 22), 0);
    EXPECT_EQ(RecipeSetPyroOnRise(recipe, 2, 23), 0);
    EXPECT_EQ(RecipeSetPyroOnTemp(recipe, 2, 24), 0);

    EXPECT_EQ(RecipeNew(&recipe), 0);
    EXPECT_EQ(RecipeSetName(recipe, "UVIR recipe"), 0);
    EXPECT_EQ(RecipeSetType(recipe, kRecipeUVIR), 0);
    EXPECT_EQ(RecipeSetPyroOffPower(recipe, 0, 25), 0);
    EXPECT_EQ(RecipeSetPyroOffTime(recipe, 0, 26), 0);
    EXPECT_EQ(RecipeSetPyroOffPower(recipe, 1, 27), 0);
    EXPECT_EQ(RecipeSetPyroOffTime(recipe, 1, 28), 0);
    EXPECT_EQ(RecipeSetPyroOnTime(recipe, 0, 29), 0);
    EXPECT_EQ(RecipeSetPyroOnRise(recipe, 0, 30), 0);
    EXPECT_EQ(RecipeSetPyroOnTemp(recipe, 0, 31), 0);
    EXPECT_EQ(RecipeSetPyroOnTime(recipe, 1, 32), 0);
    EXPECT_EQ(RecipeSetPyroOnRise(recipe, 1, 33), 0);
    EXPECT_EQ(RecipeSetPyroOnTemp(recipe, 1, 34), 0);
    EXPECT_EQ(RecipeSetPyroOnTime(recipe, 2, 35), 0);
    EXPECT_EQ(RecipeSetPyroOnRise(recipe, 2, 36), 0);
    EXPECT_EQ(RecipeSetPyroOnTemp(recipe, 2, 37), 0);
    EXPECT_EQ(RecipeSetUVTime(recipe, 38), 0);

    // Read back all recipes
    char name[RECIPE_NAME_MAX_SIZE];
    EXPECT_EQ(RecipeGet(&recipe, 2), 0);
    EXPECT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    EXPECT_STREQ(name, "IR recipe");
    EXPECT_EQ(RecipeGetType(recipe), kRecipeIR);
    EXPECT_EQ(RecipeGetPyroOffPower(recipe, 0), 1);
    EXPECT_EQ(RecipeGetPyroOffTime(recipe, 0), 2);
    EXPECT_EQ(RecipeGetPyroOffPower(recipe, 1), 3);
    EXPECT_EQ(RecipeGetPyroOffTime(recipe, 1), 4);
    EXPECT_EQ(RecipeGetPyroOnTime(recipe, 0), 5);
    EXPECT_EQ(RecipeGetPyroOnRise(recipe, 0), 6);
    EXPECT_EQ(RecipeGetPyroOnTemp(recipe, 0), 7);
    EXPECT_EQ(RecipeGetPyroOnTime(recipe, 1), 8);
    EXPECT_EQ(RecipeGetPyroOnRise(recipe, 1), 9);
    EXPECT_EQ(RecipeGetPyroOnTemp(recipe, 1), 10);

    EXPECT_EQ(RecipeGet(&recipe, 3), 0);
    EXPECT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    EXPECT_STREQ(name, "UV recipe");
    EXPECT_EQ(RecipeGetType(recipe), kRecipeUV);
    EXPECT_EQ(RecipeGetUVTime(recipe), 11);

    EXPECT_EQ(RecipeGet(&recipe, 4), 0);
    EXPECT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    EXPECT_STREQ(name, "3 step IR");
    EXPECT_EQ(RecipeGetType(recipe), kRecipe3StepIR);
    EXPECT_EQ(RecipeGetPyroOffPower(recipe, 0), 12);
    EXPECT_EQ(RecipeGetPyroOffTime(recipe, 0), 13);
    EXPECT_EQ(RecipeGetPyroOffPower(recipe, 1), 14);
    EXPECT_EQ(RecipeGetPyroOffTime(recipe, 1), 15);
    EXPECT_EQ(RecipeGetPyroOnTime(recipe, 0), 16);
    EXPECT_EQ(RecipeGetPyroOnRise(recipe, 0), 17);
    EXPECT_EQ(RecipeGetPyroOnTemp(recipe, 0), 18);
    EXPECT_EQ(RecipeGetPyroOnTime(recipe, 1), 19);
    EXPECT_EQ(RecipeGetPyroOnRise(recipe, 1), 20);
    EXPECT_EQ(RecipeGetPyroOnTemp(recipe, 1), 21);
    EXPECT_EQ(RecipeGetPyroOnTime(recipe, 2), 22);
    EXPECT_EQ(RecipeGetPyroOnRise(recipe, 2), 23);
    EXPECT_EQ(RecipeGetPyroOnTemp(recipe, 2), 24);

    EXPECT_EQ(RecipeGet(&recipe, 5), 0);
    EXPECT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    EXPECT_STREQ(name, "UVIR recipe");
    EXPECT_EQ(RecipeGetType(recipe), kRecipeUVIR);
    EXPECT_EQ(RecipeGetPyroOffPower(recipe, 0), 25);
    EXPECT_EQ(RecipeGetPyroOffTime(recipe, 0), 26);
    EXPECT_EQ(RecipeGetPyroOffPower(recipe, 1), 27);
    EXPECT_EQ(RecipeGetPyroOffTime(recipe, 1), 28);
    EXPECT_EQ(RecipeGetPyroOnTime(recipe, 0), 29);
    EXPECT_EQ(RecipeGetPyroOnRise(recipe, 0), 30);
    EXPECT_EQ(RecipeGetPyroOnTemp(recipe, 0), 31);
    EXPECT_EQ(RecipeGetPyroOnTime(recipe, 1), 32);
    EXPECT_EQ(RecipeGetPyroOnRise(recipe, 1), 33);
    EXPECT_EQ(RecipeGetPyroOnTemp(recipe, 1), 34);
    EXPECT_EQ(RecipeGetPyroOnTime(recipe, 2), 35);
    EXPECT_EQ(RecipeGetPyroOnRise(recipe, 2), 36);
    EXPECT_EQ(RecipeGetPyroOnTemp(recipe, 2), 37);
    EXPECT_EQ(RecipeGetUVTime(recipe), 38);

    // Delete recipes "IR recipe" and "3 step IR", then read "UV recipe" and "UVIR recipe":
    EXPECT_EQ(RecipeGet(&recipe, 2), 0);  // "IR recipe"
    EXPECT_EQ(RecipeDelete(recipe), 0);
    EXPECT_EQ(RecipeGet(&recipe, 3), 0);  // "3 step IR"
    EXPECT_EQ(RecipeDelete(recipe), 0);

    EXPECT_EQ(RecipeGet(&recipe, 2), 0);
    EXPECT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    EXPECT_STREQ(name, "UV recipe");
    EXPECT_EQ(RecipeGetType(recipe), kRecipeUV);
    EXPECT_EQ(RecipeGetUVTime(recipe), 11);

    EXPECT_EQ(RecipeGet(&recipe, 3), 0);
    EXPECT_EQ(RecipeGetName(recipe, name, RECIPE_NAME_MAX_SIZE), 0);
    EXPECT_STREQ(name, "UVIR recipe");
    EXPECT_EQ(RecipeGetType(recipe), kRecipeUVIR);
    EXPECT_EQ(RecipeGetPyroOffPower(recipe, 0), 25);
    EXPECT_EQ(RecipeGetPyroOffTime(recipe, 0), 26);
    EXPECT_EQ(RecipeGetPyroOffPower(recipe, 1), 27);
    EXPECT_EQ(RecipeGetPyroOffTime(recipe, 1), 28);
    EXPECT_EQ(RecipeGetPyroOnTime(recipe, 0), 29);
    EXPECT_EQ(RecipeGetPyroOnRise(recipe, 0), 30);
    EXPECT_EQ(RecipeGetPyroOnTemp(recipe, 0), 31);
    EXPECT_EQ(RecipeGetPyroOnTime(recipe, 1), 32);
    EXPECT_EQ(RecipeGetPyroOnRise(recipe, 1), 33);
    EXPECT_EQ(RecipeGetPyroOnTemp(recipe, 1), 34);
    EXPECT_EQ(RecipeGetPyroOnTime(recipe, 2), 35);
    EXPECT_EQ(RecipeGetPyroOnRise(recipe, 2), 36);
    EXPECT_EQ(RecipeGetPyroOnTemp(recipe, 2), 37);
    EXPECT_EQ(RecipeGetUVTime(recipe), 38);
}
