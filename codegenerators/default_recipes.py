#!/usr/bin/python3
"""Generator of 'Default Recipes' """
# pylint: disable=missing-function-docstring,import-error
import os
import sys
import argparse
from dataclasses import dataclass
from pathlib import Path
import xml.etree.ElementTree as ET

header = """
#ifndef KOSTER_COMMON_DEFAULT_RECIPES_GENERATED_H
#define KOSTER_COMMON_DEFAULT_RECIPES_GENERATED_H

#include "recipe_types.h"

{function_declarations}

void DefaultRecipesFill(struct recipes_t *recipes);

#endif
"""

source = """
#include "default_recipes_generated.h"
#include "koster-common/recipe.h"

#include <string.h>

static struct recipes_t default_recipes = {{
    .n_recipes = {n_recipes},
    .recipes = {{
{recipes}
    }}
}};

{function_definitions}

void DefaultRecipesFill(struct recipes_t *recipes){{
    memcpy(recipes, &default_recipes, sizeof(struct recipes_t));
}}

"""

function_declaration = "void DefaultRecipesSet{lang}();"

recipe = '        {{{id}, "{name}", kRecipe{type}, {{{pyro_off_time}}}, {{{pyro_off_power}}}, {{{pyro_on_time}}}, {{{pyro_on_rise}}}, {{{pyro_on_temp}}}, {uv_time}}}'

function_definition = """
void DefaultRecipesSet{lang}(){{
{lang_setters}
}}
"""
lang_setter='    strncpy(default_recipes.recipes[{id}].name, "{name}", RECIPE_NAME_MAX_SIZE);'


@dataclass
class Recipe:
    id : int
    default_name : str
    names : dict()
    type : str
    pyro_off_time : list[int]
    pyro_off_power : list[int]
    pyro_on_time : list[int]
    pyro_on_rise : list[int]
    pyro_on_temp : list[int]
    uv_time : int

valid_types = ["IR", "UV", "3StepIR", "UVIR", "UVLED"]

class Configuration:
    """
    Parser for default recipes
    """
    def __init__(self):
        self.recipes : list[Recipe] = []
        self.langs : list[str] = ["EN"]
        
    def read(self, config_path: Path) -> None:
        
        root = ET.parse(config_path).getroot()

        for i, element in enumerate(root.iter("Recipe")):
            default_name = element.get("Name")
            if default_name in [r.default_name for r in self.recipes]:
                error_txt = f'Duplicate recipe {default_name}'
                raise RuntimeError(error_txt)
            
            recipe_type = element.get("Type")
            if recipe_type not in valid_types:
                error_txt = f'Invalid type "{recipe_type}" for recipe "{default_name}". Valid values are {", ".join(valid_types)}'
                raise RuntimeError(error_txt)

            pyro_on_time = ['0', '0', '0']
            pyro_on_rise = ['0', '0', '0']
            pyro_on_temp = ['0', '0', '0']
            if recipe_type in ["IR", "3StepIR", "UVIR"]:
                t1 = element.find("PyroOnTimer1")
                if t1 is None:
                    error_txt = f'Missing element "PyroOnTimer1" for "{default_name}".'
                    raise RuntimeError(error_txt)
                t2 = element.find("PyroOnTimer2")
                if t2 is None:
                    error_txt = f'Missing element "PyroOnTimer2" for "{default_name}".'
                    raise RuntimeError(error_txt)
                pyro_on_time[0] = t1.get('Time')
                pyro_on_time[1] = t2.get('Time')
                pyro_on_rise[0] = t1.get('Rise')
                pyro_on_rise[1] = t2.get('Rise')
                pyro_on_temp[0] = t1.get('End')
                pyro_on_temp[1] = t2.get('End')

            if recipe_type == "3StepIR":
                t3 = element.find("PyroOnTimer3")
                if t3 is None:
                    error_txt = f'Missing element "PyroOnTimer3" for "{default_name}".'
                    raise RuntimeError(error_txt)
                pyro_on_time[2] = t3.get('Time')
                pyro_on_rise[2] = t3.get('Rise')
                pyro_on_temp[2] = t3.get('End')

            pyro_off_time = ['0', '0']
            pyro_off_power = ['0', '0']
            if recipe_type in ["IR", "UVIR"]:
                t1 = element.find("PyroOffTimer1")
                if t1 is None:
                    error_txt = f'Missing element "PyroOffTimer1" for "{default_name}".'
                    raise RuntimeError(error_txt)
                t2 = element.find("PyroOffTimer2")
                if t2 is None:
                    error_txt = f'Missing element "PyroOffTimer2" for "{default_name}".'
                    raise RuntimeError(error_txt)
                pyro_off_time[0] = t1.get('Time')
                pyro_off_time[1] = t2.get('Time')
                pyro_off_power[0] = t1.get('Power')
                pyro_off_power[1] = t2.get('Power')

            uv_time = '0'
            if recipe_type in ["UV", "IRUV", "UVLED"]:
                uv = element.find("UV")
                if uv is None:
                    error_txt = f'Missing element "UV" for "{default_name}".'
                    raise RuntimeError(error_txt)
                uv_time = uv.get("Time")

            names = {"EN" : default_name}
            for lang_override in element.iter("LanguageOverride"):
                lang = lang_override.get("Lang")
                if lang in names:
                    error_txt = f'Duplicate LanguageOverride "{lang}"'
                    raise RuntimeError(error_txt)

                if lang not in self.langs:
                    self.langs.append(lang)

                name = lang_override.get("Name")
                names[lang] = name


            self.recipes.append(
                Recipe(i,
                       default_name,
                       names,
                       recipe_type,
                       pyro_off_time,
                       pyro_off_power,
                       pyro_on_time,
                       pyro_on_rise,
                       pyro_on_temp,
                       uv_time)
            )

class SourceGenerator:
    """
    Creates C header and source file for parameters
    """
    def __init__(self, config : Configuration):
        self.config = config

    def generate(self, header_path: Path, source_path: Path):

        function_declarations = []
        for lang in self.config.langs:
            function_declarations.append(function_declaration.format(lang=lang))
            
        recipes = []
        function_definitions = []
        for r in self.config.recipes:
            recipes.append(recipe.format(
                id=r.id,
                name=r.default_name,
                type=r.type,
                pyro_off_time=", ".join(r.pyro_off_time),
                pyro_off_power=", ".join(r.pyro_off_power),
                pyro_on_time=", ".join(r.pyro_on_time),
                pyro_on_rise=", ".join(r.pyro_on_rise),
                pyro_on_temp=", ".join(r.pyro_on_temp),
                uv_time=r.uv_time
            ))

        for lang in self.config.langs:
            lang_setters = []
            for r in self.config.recipes:
                for lang_override in r.names:
                    if lang_override == lang:
                        lang_setters.append(lang_setter.format(id=r.id, lang=lang, name=r.names[lang]))
                
            function_definitions.append(function_definition.format(lang=lang, lang_setters="\n".join(lang_setters)))
            
        header_content = header.format(
            function_declarations="\n".join(function_declarations)
        )
        with open(header_path, 'w') as file_:
            file_.write(header_content)

        source_content = source.format(
            n_recipes=len(self.config.recipes),
            recipes=",\n".join(recipes),
            function_definitions="\n".join(function_definitions)
        )
        with open(source_path, 'w') as file_:
            file_.write(source_content)

            
if __name__ == "__main__":
    scriptPath = Path(sys.argv[0]).parent

    parser = argparse.ArgumentParser()
    parser.add_argument('--config-file', help="Config folder", type=Path, required=True)
    parser.add_argument('--output-header', help="Path to output header file", type=Path, required=True)
    parser.add_argument('--output-source', help="Path to output source file", type=Path, required=True)
    args = parser.parse_args()

    config_file = args.config_file.resolve()
    header_file = args.output_header.resolve()
    source_file = args.output_source.resolve()

    print(f"Config file:   {config_file}")
    print(f"Output header: {header_file}")
    print(f"Output source: {source_file}")

    config = Configuration()
    config.read(config_file)
        
    header_file.parent.mkdir(parents=True, exist_ok=True)
    source_file.parent.mkdir(parents=True, exist_ok=True)

    generator = SourceGenerator(config)
    generator.generate(header_file, source_file)
    
