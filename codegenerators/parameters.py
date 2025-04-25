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

#include "koster-common/parameters_base.h"
#include <zephyr/kernel.h>

struct param_t;
struct param_category_t;

{enums}

#define PARAM_CATEGORY_NAME_MAX_LEN {category_max_len}
#define PARAM_NUM_CATEGORIES {n_categories}
#define PARAM_NUM_PARAMS {n_params}
#define PARAM_MAX_NUM_PARAMS_IN_CATEGORY {max_n_params_in_category}
#define PARAM_NAME_MAX_LEN {param_name_max_len}
#define PARAM_DESC_MAX_LEN {param_desc_max_len}
#define PARAM_VALUE_STRING_MAX_LEN {param_value_string_max_len}

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
int ParamSet{name}(const {type} value);
"""

source = """
#include "koster-common/parameters.h"
#include "parameters_private.h"
#include <string.h>
#include <stdlib.h>

extern struct k_mutex param_mutex;
static struct param_t params_[PARAM_NUM_PARAMS];
static struct param_category_t categories_[PARAM_NUM_CATEGORIES];

{get_value_string_funcs}

int ParamInit(){{
    int rc = -1;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {{
{parameter_initializers}
{category_initializers}
        k_mutex_unlock(&param_mutex);
        rc = 0;
    }}
    return rc;
}}

{getter_definitions}

{setter_definitions}

int ParamGetCategory(struct param_category_t** category, const unsigned int index) {{
    int rc = -1;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {{
        if (index < PARAM_NUM_CATEGORIES) {{
            *category = &categories_[index];
            rc = 0;
        }}
        k_mutex_unlock(&param_mutex);
    }}
    return rc;
}}

int ParamGetValueString(struct param_t* param, char* buf, const int32_t value) {{
    int rc = -1;
    if (param == NULL) {{
        return rc;
    }}

    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {{
        if (value >= param->min && value <= param->max ) {{
            switch (param->id) {{
{get_value_string_cases}
            default:
                if ( param->exponent >= 0 ) {{
                    snprintf(buf, PARAM_VALUE_STRING_MAX_LEN, "%i", value);
                }} else {{
                    char fmt_buf[PARAM_VALUE_STRING_MAX_LEN];
                    snprintf(fmt_buf, PARAM_VALUE_STRING_MAX_LEN, "%%.%uf", abs(param->exponent));
                    snprintf(buf, PARAM_VALUE_STRING_MAX_LEN, fmt_buf, value*pow(10.0, param->exponent));
                }}
                rc = 0;
                break;
            }}
        }}
        k_mutex_unlock(&param_mutex);
    }}
    return rc;
}}

"""
get_value_string_func = """
int get_value_string_{type}(const int32_t value, char *buf){{
    switch (value) {{
        {get_value_string_type_cases}
    }}
    return -1;
}}"""
get_value_string_type_case = """
        case {value}:
            strncpy(buf, "{string}", PARAM_VALUE_STRING_MAX_LEN);
            return 0;"""
get_value_string_case = """
            case {id}:
                rc = get_value_string_{type}(value, buf);
                break;"""

getter_definition = """{type} ParamGet{name}(){{
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {{
        uint32_t ret = params_[{index}].value;
        k_mutex_unlock(&param_mutex);
        return ret;
    }}
    return 0;
}}"""

setter_definition = """int ParamSet{name}(const {type} value){{
    int rc = -1;
    if ( !(value >= {min} && value < {max}) ){{
        return rc;
    }}

    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {{
        params_[{index}].value = (uint32_t)value;
        k_mutex_unlock(&param_mutex);
        rc = 0;
    }}
    return rc;
}}"""

parameter_initializer = """
        params_[{index}].id = {id};
        strncpy(params_[{index}].name, "{name}", PARAM_NAME_MAX_LEN);
        params_[{index}].type = {type};
        params_[{index}].access = {access};
        strncpy(params_[{index}].description, "{description}", PARAM_DESC_MAX_LEN);
        params_[{index}].value = {value};
        params_[{index}].min = {min};
        params_[{index}].max = {max};
        params_[{index}].exponent = {exponent};
"""

category_initializer = """
        categories_[{index}].id = {id};
        strncpy(categories_[{index}].name, "{name}", PARAM_NAME_MAX_LEN);
        categories_[{index}].n_params = {n_params_in_category};
{category_parameter_ptrs}
"""
category_parameter_ptr = "        categories_[{category_index}].params[{parameter_index}] = &params_[{parameter_ptr}];"

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
                
                if not "Exponent" in self.parameters[id]:
                    error_txt = f'Attribute Exponent (required by attribute Unit) missing for Parameter for Parameter with Id "{id}"'
                    raise RuntimeError(error_txt)
                
                if not "Min" in self.parameters[id]:
                    error_txt = f'Attribute Min (required by attribute Unit) missing for Parameter for Parameter with Id "{id}"'
                    raise RuntimeError(error_txt)
                
                if not "Max" in self.parameters[id]:
                    error_txt = f'Attribute Max (required by attribute Unit) missing for Parameter for Parameter with Id "{id}"'
                    raise RuntimeError(error_txt)

                exponent = self.parameters[id]["Exponent"]
                try:
                    int(exponent)
                except ValueError:
                    error_txt = f'Attribute Exponent ("{exponent}") must be an integer (Parameter with Id "{id}")'
                    raise RuntimeError(error_txt)
                
            elif any([attrib in self.parameters[id] for attrib in ["Min", "Max", "Exponent"]]):
                error_txt = f'Parameter with Id "{id}" must have all or none of attributes "Min", "Max", "Exponent", "Unit"'
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

                if int(maximum) <= int(minimum):
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

        get_value_string_funcs = []
        param_value_string_len = [10] # 10 chars for uint32_t
        for enum_name in self.config.enums:
            enum_values = []
            get_value_string_type_cases = []
            for member_name in self.config.enums[enum_name]:
                value = self.config.enums[enum_name][member_name]
                enum_values.append(enum_value.format(name=member_name, value=value))
                get_value_string_type_cases.append(get_value_string_type_case.format(
                    value=value,
                    string=member_name
                ))
                param_value_string_len.append(len(member_name))

            enums.append(enum_definition.format(
                name=enum_name,
                values='\n'.join(enum_values),
                n_values=len(enum_values)
            ))
            get_value_string_funcs.append(get_value_string_func.format(
                type=f"param_{enum_name}",
                get_value_string_type_cases="\n".join(get_value_string_type_cases)
            ))

        getter_declarations = []
        setter_declarations = []
        getter_definitions = []
        setter_definitions = []
        parameter_initializers = []
        category_initializers = []
        get_value_string_cases = []
        id_to_index = {}
        for i,param in enumerate(self.config.parameters):
            id_to_index[param] = i
            type = self.config.parameters[param]["Type"]
            name = to_camelcase(self.config.parameters[param]["Name"])
            minimum = None
            maximum = None
            if type in self.config.enums:
                minimum = 0
                maximum = f"kParamN_{type} - 1"
            else:
                minimum = self.config.parameters[param]["Min"]
                maximum = self.config.parameters[param]["Max"]
            default = self.config.parameters[param]["Default"]
            if type in self.config.enums:
                default = f"kParam{default}"
                type_name = f"param_{type}"
                setter_definitions.append(setter_definition.format(
                    index=i,
                    type=type_name,
                    name=name,
                    min=minimum,
                    max=maximum
                ))
                get_value_string_cases.append(get_value_string_case.format(id=param, type=type_name))
                
            parameter_initializers.append(parameter_initializer.format(
                index=i,
                id=param,
                name=name,
                value=default,
                type="kParamTypeEnum" if type in self.config.enums else "kParamTypeNumeric",
                access=self.config.access_levels[self.config.parameters[param]["AccessLevel"]],
                description=self.config.parameters[param]["Description"],
                min=minimum,
                max=maximum,
                exponent=self.config.parameters[param]["Exponent"] if "Exponent" in self.config.parameters[param] else 0
            ))
            getter_declarations.append(getter_declaration.format(type=type_name, name=name))
            setter_declarations.append(setter_declaration.format(type=type_name, name=name))
            getter_definitions.append(getter_definition.format(type=type_name, name=name, index=i))
            
            
        categories = []
        n_params_in_category = []
        category_initializers = []
        for i,name in enumerate(sorted(self.config.categories)):
            filtered_parameters = [ self.config.parameters[key] for key in self.config.parameters
                                    if self.config.parameters[key]["Category"] == name ]
            sorted_filtered_parameters = sorted(filtered_parameters, key=lambda d: d["Name"])
            sorted_filtered_parameters_indices = [ id_to_index[p["Id"]] for p in sorted_filtered_parameters ]
            n_params = len(sorted_filtered_parameters_indices)
            category_initializers.append(category_initializer.format(
                id=self.config.categories[name],
                index=i,
                name=name,
                n_params_in_category=n_params,
                category_parameter_ptrs="\n".join([category_parameter_ptr.format(category_index=i, parameter_index=p_i, parameter_ptr=p) for p_i,p in enumerate(sorted_filtered_parameters_indices)])
            ))
            n_params_in_category.append(n_params)
        
        header_content = header.format(
            enums='\n'.join(enums),
            getter_declarations="\n".join(getter_declarations),
            setter_declarations="\n".join(setter_declarations),
            n_params=len(self.config.parameters),
            categories="\n".join(categories),
            n_categories=len(self.config.categories.keys()),
            category_max_len=max([len(c) for c in self.config.categories.keys()]) + 1, # +1 for null termination
            max_n_params_in_category=max(n_params_in_category),
            param_name_max_len=max([len(p["Name"]) for p in self.config.parameters.values()]) + 1, # +1 for null termination
            param_desc_max_len=max([len(p["Description"]) for p in self.config.parameters.values()]) + 1, # +1 for null termination
            param_value_string_max_len=max(param_value_string_len) + 2, # +1 for null termination, +1 for decimal
        )
        with open(header_path, 'w') as file_:
            file_.write(header_content)
            
        source_content = source.format(
            parameter_initializers='\n'.join(parameter_initializers),
            category_initializers='\n'.join(category_initializers),
            getter_definitions="\n".join(getter_definitions),
            setter_definitions="\n".join(setter_definitions),
            get_value_string_funcs="\n".join(get_value_string_funcs),
            get_value_string_cases="\n".join(get_value_string_cases)
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
    
