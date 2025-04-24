#!/usr/bin/python3
"""Generator of 'Parameters' """
# pylint: disable=missing-function-docstring,import-error
import os
import sys
import argparse
from pathlib import Path
import xml.etree.ElementTree as ET

header = """
#ifndef KOSTER_COMMON_PARAMETERS
#define KOSTER_COMMON_PARAMETERS

#include <stdint.h>
#include <stdbool.h>

{enums}

#define PARAM_CATEGORY_NAME_MAX_LEN {category_max_len}
#define PARAM_NUM_CATEGORIES {n_categories}
#define PARAM_NUM_PARAMS {n_params}
#define PARAM_MAX_NUM_PARAMS_IN_CATEGORY {max_n_params_in_category}
#define PARAM_NAME_MAX_LEN {param_name_max_len}

void ParamInit();

/**
* Get parameter name sorted alphabetically
*
* @return 0 on success, -1 on failure
* @param index the alphabetical index of the category
* @param name  will be filled with the category name
*/
int ParamGetCategoryNameAlpha(uint8_t index, char* name);

/**
* Get the number of parameters in a category
*
* @return number of parameters in category on success, -1 on failure
* @param index the alphabetical index of the category
*/
int ParamNumParamsInCategory(uint8_t index);

/**
* Get parameter ID in alphabetical order from a category
*
* @return the parameter ID or -1 on failure
* @param category_index   the alphabetical index of the category
* @param parameter_index  the alphabetical index of the parameter in the category
*/
int ParamParamIdInCategoryAlpha(uint8_t category_index, uint8_t parameter_index);

/**
* Get name of parameter
*
* @return 0 on success, -1 on failure
* @param parameter_id parameter Id
* @param name         will be filled with the parameter name
*/
int ParamGetName(uint8_t parameter_id, char* name);

/**
* Get string representation of parameter value
*
* @return 0 on success, -1 on failure
* @param parameter_id parameter Id
* @param name         will be filled with the string representation of the parameter value
*/
int ParamGetValueString(uint8_t parameter_id, char* name);

/**
* Get parameter by ID
*
* @return 0 on success, -1 on failure
* @param id     the ID of the parameter to get
* @param value  will be filled with the parameter value
*/
int ParamGet(uint8_t id, int32_t *value);

/**
* Increase parameter value. Will wrap-around.
*
* @return 0 on success, -1 on failure
* @param id  the ID of the parameter to increase the value of
*/
int ParamIncr(uint8_t id);

/**
* Decrease parameter value. Will wrap-around.
*
* @return 0 on success, -1 on failure
* @param id  the ID of the parameter to decrease the value of
*/
int ParamDecr(uint8_t id);

/**
* Set parameter by ID
*
* @return 0 on success, -1 on failure
* @param value  the value to set
* @param id     the ID of the parameter to set
*/
int ParamSet(uint8_t id, const int32_t value);

{getter_declarations}

{setter_declarations}

#endif
"""

enum_definition = """typedef enum {{
{values}
    kParamN_{name} = {n_values}
}} param_{name};
"""
enum_value = "    kParam{name} = {value},"

getter_declaration = """/**
* Get parameter {name}
*
* @return The value of parameter {name}
*/
{type} ParamGet{name}();
"""
setter_declaration = """/**
* Set parameter {name}
*
* @return 0 on success, -1 on failure (e.g. out of range)
* @param value the value to set
*/
int ParamSet{name}({type} value);
"""

source = """
#include "koster-common/parameters.h"
#include <string.h>

struct param_category_t{{
    uint8_t id;
    char name[PARAM_CATEGORY_NAME_MAX_LEN];
}};
static struct param_category_t param_categories[PARAM_NUM_CATEGORIES];

{parameters}

void ParamInit(){{
{initializers}
}}

{validators}

int ParamGetCategoryNameAlpha(uint8_t index, char* name){{
    if (index < PARAM_NUM_CATEGORIES) {{
        strncpy(name, param_categories[index].name, PARAM_CATEGORY_NAME_MAX_LEN);
        return 0;
    }}
    return -1;
}}

int ParamNumParamsInCategory(uint8_t index){{
    switch (index) {{
{n_params_in_category_cases}
    }}
    return -1;
}}

int ParamParamIdInCategoryAlpha(uint8_t category_index, uint8_t parameter_index){{
    switch (category_index) {{
{param_in_category_cases}
    }}
    return -1;
}}

int ParamGetName(uint8_t parameter_id, char* name) {{
    switch (parameter_id) {{
{param_get_name_cases}
    }}
    return -1;
}}

int ParamGetValueString(uint8_t parameter_id, char* name) {{
    switch (parameter_id) {{
{param_get_name_cases}
    }}
    return -1;
}}

int ParamGet(uint8_t id, int32_t *value){{
    switch (id) {{
{get_cases}
    }}
    return -1;
}}

int ParamSet(uint8_t id, const int32_t value){{
    switch (id) {{
{set_cases}
    }}
    return -1;
}}

int ParamIncr(uint8_t id){{
    switch (id) {{
{incr_cases}
    }}
    return -1;
}}

int ParamDecr(uint8_t id){{
    switch (id) {{
{decr_cases}
    }}
    return -1;
}}

{getter_definitions}

{setter_definitions}


"""
getter_definition = """{type} ParamGet{name}(){{
    return param_{name};
}}"""
setter_definition = """int ParamSet{name}(const {type} value){{
    if ( !({validators}) ){{
        return -1;
    }}
    param_{name} = value;
    return 0;
}}"""

parameter = "static {type} param_{name};"
initializer = "    param_{name} = {value};"
validator = """static bool validate_{type}(const param_{type} value){{
    return value >= {min} && value < {max};
}}
"""
get_case = """
        case {id}:
            *value = (int32_t)param_{name};
            return 0;"""
set_case = """
        case {id}:
            return ParamSet{name}(({type})value);"""
init_category_name = '    strcpy(param_categories[{index}].name, "{name}");'
init_category_id = '    param_categories[{index}].id = {id};'
incr_case = """
        case {id}:
            if( (int32_t)param_{name} >= {max} ){{
                param_{name} = {min};
            }} else {{
                ++param_{name};
            }}
            return 0;"""
decr_case = """
        case {id}:
            if( (int32_t)param_{name} <= {min} ){{
                param_{name} = {max};
            }} else {{
                --param_{name};
            }}
            return 0;"""

n_params_in_category_case = """
        case {index}:
            return {n_params};"""

param_in_category_case = """
        case {category_index}:
            switch (parameter_index) {{
{parameter_cases}
            }}
            return -1;"""
parameter_case = """
            case {parameter_index}:
                return {parameter_id};
"""
param_get_name_case = """
        case {id}:
            strncpy(name, "{name}", PARAM_NAME_MAX_LEN);
            return 0;
"""

class Configuration:
    """
    Parser for parameters configuration
    """
    def __init__(self):
        self.access_levels = {} # {str : int}, {"Name" : "Id"}
        self.categories = {} # {str : str}, {"Name" : "Id"}
        self.units = {} # {str : str}, {"Name" : "Id"}
        self.enums = {} # {str : {str : str}}, {"Name" : {"Name" : "Value"}}
        self.parameters = {} # {str : {str : str}}, {"Id" : {<attribute> : <value>}}

        
    def _elements_to_unique_key_value_dict(self, element_tree, element_name : str, key_attrib : str, value_attrib : str):
        key_value_dict = {}
        for element in element_tree.iter(element_name):
            key = element.get(key_attrib)
            value = element.get(value_attrib)
            
            if key in key_value_dict.keys():
                error_txt = f'Duplicate attribute {key_attrib} for {element_name}: "{key}"'
                raise RuntimeError(error_txt)
            
            if value in key_value_dict.values():
                error_txt = f'Duplicate attribute {value_attrib} for {element_name}: "{value}"'
                raise RuntimeError(error_txt)
            
            key_value_dict[key] = value
            
        return key_value_dict

        
    def read(self, config_path: Path) -> None:
        
        root = ET.parse(config_path).getroot()

        self.access_levels = self._elements_to_unique_key_value_dict(root, "AccessLevel", "Name", "Id")
        self.categories = self._elements_to_unique_key_value_dict(root, "Category", "Name", "Id")
        self.units = self._elements_to_unique_key_value_dict(root, "Unit", "Name", "Id")

        for element in root.iter("Enum"):
            name = element.get("Name")
            if name in self.enums.keys():
                error_txt = f'Duplicate enum {name}'
                raise RuntimeError(error_txt)
            
            self.enums[name] = self._elements_to_unique_key_value_dict(element, "EnumValue", "Name", "Value")


        for element in root.iter("Parameter"):
            id = element.get("Id")
            if id in self.parameters:
                error_txt = f'Duplicate Parameter Id: {id}'
                raise RuntimeError(error_txt)

            self.parameters[id] = element.attrib

        # parameter validation
        for id in self.parameters:
            
            if "Default" not in self.parameters[id]:
                error_txt = f'Missing default value for Parameter with Id "{id}"'
                raise RuntimeError(error_txt)
            
            if self.parameters[id]["Category"] not in self.categories:
                error_txt = f'Unknown Category "{self.parameters[id]["Category"]}" for Parameter with Id "{id}"'
                raise RuntimeError(error_txt)
            
            if self.parameters[id]["AccessLevel"] not in self.access_levels:
                error_txt = f'Unknown AccessLevel "{self.parameters[id]["AccessLevel"]}" for Parameter with Id "{id}"'
                raise RuntimeError(error_txt)

            if "Unit" in self.parameters[id]:
                if self.parameters[id]["Unit"] not in self.units:
                    error_txt = f'Unknown Unit "{self.parameters[id]["Unit"]}" for Parameter with Id "{id}"'
                    raise RuntimeError(error_txt)
                
                if not "Precision" in self.parameters[id]:
                    error_txt = f'Attribute Precision (required by attribute Unit) missing for Parameter for Parameter with Id "{id}"'
                    raise RuntimeError(error_txt)
                
                if not "Min" in self.parameters[id]:
                    error_txt = f'Attribute Min (required by attribute Unit) missing for Parameter for Parameter with Id "{id}"'
                    raise RuntimeError(error_txt)
                
                if not "Max" in self.parameters[id]:
                    error_txt = f'Attribute Max (required by attribute Unit) missing for Parameter for Parameter with Id "{id}"'
                    raise RuntimeError(error_txt)

                precision = self.parameters[id]["Precision"]
                try:
                    int(precision)
                except ValueError:
                    error_txt = f'Attribute Precision ("{precision}") must be an integer (Parameter with Id "{id}")'
                    raise RuntimeError(error_txt)
                
            elif any([attrib in self.parameters[id] for attrib in ["Min", "Max", "Precision"]]):
                error_txt = f'Parameter with Id "{id}" must have all or none of attributes "Min", "Max", "Precision", "Unit"'
                raise RuntimeError(error_txt)

            
            type = self.parameters[id]["Type"]
            if type in ["uint8_t", "uint16_t", "int8_t", "int16_t", "int32_t"]:
                minimum = self.parameters[id]["Min"]
                try:
                    int(minimum)
                except ValueError:
                    error_txt = f'Attribute Min ("{minimum}") must be an integer for Type "{type}" (Parameter with Id "{id}")'
                    raise RuntimeError(error_txt)

                maximum = self.parameters[id]["Max"]
                try:
                    int(maximum)
                except ValueError:
                    error_txt = f'Attribute Max ("{maximum}") must be an integer for Type "{type}" (Parameter with Id "{id}")'
                    raise RuntimeError(error_txt)

                if maximum <= minimum:
                    error_txt = f'Attribute Max ("{maximum}") must be larger than Min ("{minimum}") (Parameter with Id "{id}")'
                    raise RuntimeError(error_txt)

            elif type not in self.enums.keys():
                error_txt = f'Unknown Type "{type}" for Parameter with Id "{id}"'
                raise RuntimeError(error_txt)



def to_camelcase(string : str) -> str:
    return "".join([s.capitalize() for s in string.split()])
    
class SourceGenerator:
    """
    Creates C header and source file for parameters
    """
    def __init__(self, config : Configuration):
        self.config = config

    def generate(self, header_path: Path, source_path: Path):
        enums = []

        for enum_name in self.config.enums:
            enum_values = []
            for member_name in self.config.enums[enum_name]:
                value = self.config.enums[enum_name][member_name]
                enum_values.append(enum_value.format(name=member_name, value=value))

            enums.append(enum_definition.format(
                name=enum_name,
                values='\n'.join(enum_values),
                n_values=len(enum_values)
            ))

        getter_declarations = []
        setter_declarations = []
        getter_definitions = []
        setter_definitions = []
        parameters = []
        initializers = []
        get_cases = []
        set_cases = []
        incr_cases = []
        decr_cases = []
        param_get_name_cases = []
        for param in self.config.parameters:
            type = self.config.parameters[param]["Type"]
            name = to_camelcase(self.config.parameters[param]["Name"])
            minimum = None
            maximum = None
            if type in self.config.enums:
                setter_definitions.append(setter_definition.format(
                    type=f"param_{type}",
                    name=name,
                    validators=f"validate_{type}(value)"
                ))
                minimum = 0
                maximum = f"kParamN_{type} - 1"
            else:
                minimum = self.config.parameters[param]["Min"]
                maximum = self.config.parameters[param]["Max"]
                setter_definitions.append(setter_definition.format(
                    type=type,
                    name=name,
                    validators=f"value >= {minimum} && value <= {maximum}"
                ))
            default = self.config.parameters[param]["Default"]
            if type in self.config.enums:
                default = f"kParam{default}"
                type = f"param_{type}"
                
            parameters.append(parameter.format(type=type, name=name))
            initializers.append(initializer.format(name=name, value=default))
            getter_declarations.append(getter_declaration.format(type=type, name=name))
            setter_declarations.append(setter_declaration.format(type=type, name=name))
            getter_definitions.append(getter_definition.format(type=type, name=name))
            get_cases.append(get_case.format(id=param, name=name))
            set_cases.append(set_case.format(id=param, name=name, type=type))
            incr_cases.append(incr_case.format(
                id=param,
                name=name,
                min=minimum,
                max=maximum
            ))
            decr_cases.append(decr_case.format(
                id=param,
                name=name,
                min=minimum,
                max=maximum
            ))
            param_get_name_cases.append(param_get_name_case.format(id=param, name=self.config.parameters[param]["Name"]))
            
        validators = []
        for type in self.config.enums:
            validators.append(validator.format(type=type, min=0, max=f"kParamN_{type}"))

        categories = []
        n_params_in_category_cases = []
        n_params_in_category = []
        param_in_category_cases = []
        for i,name in enumerate(sorted(self.config.categories)):
            initializers.append(init_category_id.format(id=self.config.categories[name], index=i))
            initializers.append(init_category_name.format(name=name, index=i))
            filtered_parameter_ids = [ p["Id"] for p in self.config.parameters.values() if p["Category"] == name ]
            n_params = len(filtered_parameter_ids)
            n_params_in_category_cases.append(
                n_params_in_category_case.format(
                    index=i,
                    n_params=n_params
                ))
            n_params_in_category.append(n_params)
            parameter_cases = []
            for alphabetical_index, parameter_id in enumerate(sorted(filtered_parameter_ids)):
                parameter_cases.append(parameter_case.format(parameter_index=alphabetical_index, parameter_id=parameter_id))
            param_in_category_cases.append(param_in_category_case.format(
                category_index=i,
                parameter_cases="\n".join(parameter_cases)
            ))
        
        header_content = header.format(
            enums='\n'.join(enums),
            getter_declarations="\n".join(getter_declarations),
            setter_declarations="\n".join(setter_declarations),
            n_params=len(self.config.parameters),
            categories="\n".join(categories),
            n_categories=len(self.config.categories.keys()),
            category_max_len=max([len(c) for c in self.config.categories.keys()]) + 1, # +1 for null termination
            max_n_params_in_category=max(n_params_in_category),
            param_name_max_len=max([len(p["Name"]) for p in self.config.parameters.values()])
        )
        with open(header_path, 'w') as file_:
            file_.write(header_content)
            
        source_content = source.format(
            parameters='\n'.join(parameters),
            initializers='\n'.join(initializers),
            getter_definitions="\n".join(getter_definitions),
            setter_definitions="\n".join(setter_definitions),
            validators="\n".join(validators),
            set_cases="\n".join(set_cases),
            get_cases="\n".join(get_cases),
            incr_cases="\n".join(incr_cases),
            decr_cases="\n".join(decr_cases),
            n_params_in_category_cases="\n".join(n_params_in_category_cases),
            param_in_category_cases="\n".join(param_in_category_cases),
            param_get_name_cases='\n'.join(param_get_name_cases)
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
    
