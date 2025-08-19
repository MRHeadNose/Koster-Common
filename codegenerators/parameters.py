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
#define PARAM_ACCESS_LEVELS {n_access_levels}

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
#include "{parameters_header}"
#include "parameters_private.h"
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <stdlib.h>
#include <math.h>
LOG_MODULE_DECLARE(koster_common);

extern struct k_mutex param_mutex;
static struct settings_handler handler_;

{param_names}
{param_descriptions}
{category_names}

static int32_t param_values_[PARAM_NUM_PARAMS];

static const struct param_t params_[PARAM_NUM_PARAMS] = {{
{parameter_initializers}
}};

static const struct param_category_t categories_[PARAM_NUM_CATEGORIES] = {{
{category_initializers}
}};


{get_value_string_funcs}

static int handle_set(const char* name, size_t len, settings_read_cb read_cb, void* cb_arg) {{
    const char* next;
    size_t name_len;

    name_len = settings_name_next(name, &next);

    {handle_set_cases}

    return -ENOENT;
}}

static int handle_export(int (*storage_func)(const char* name, const void* value, size_t val_len)) {{
    int ret = -EBUSY;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {{
{handle_export_cases}
        k_mutex_unlock(&param_mutex);
    }}
    return ret;
}}

static void load_production_defaults() {{
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {{
    {param_production_default_values}
        k_mutex_unlock(&param_mutex);
    }}
}}

void ParamLoadDefaults(const int32_t machine_type) {{
    load_production_defaults();
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {{
        param_values_[0] = machine_type;
        switch ( machine_type ) {{
{param_default_overrides}
            default:
                LOG_ERR("[parameter] Unknown machine type (%o). Loaded production defaults.", machine_type);
        k_mutex_unlock(&param_mutex);
        }}
    }}
}}

int ParamInit() {{
    k_mutex_init(&param_mutex);

    load_production_defaults();

    handler_.name = "parameters";
    handler_.h_get = NULL;
    handler_.h_set = handle_set;
    handler_.h_commit = NULL;
    handler_.h_export = handle_export;

    int rc = settings_register(&handler_);
    if (rc != 0) {{
        LOG_ERR("[parameters] settings_register failed (err %d)", rc);
        return -1;
    }}

    rc = settings_load();
    if (rc != 0) {{
        LOG_ERR("[parameters] settings_load failed (err %d)", rc);
        return -1;
    }}

    return 0;
}}

{getter_definitions}

{setter_definitions}

int ParamGetCategory(const struct param_category_t** category, const unsigned int index) {{
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

int ParamGetValueString(const struct param_t* param, char* buf, const int32_t value) {{
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

int ParamSave(const struct param_t* param) {{
    int rc = -1;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {{
        if ( param == NULL ) {{
            return -1;
        }}
        switch (param->id) {{
{settings_save_cases}
        }}
        k_mutex_unlock(&param_mutex);
    }}
    return rc;
}}


"""
get_value_string_func = """
int get_value_string_{type}(const int32_t value, char *buf) {{
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
        int32_t ret = param_values_[{index}];
        k_mutex_unlock(&param_mutex);
        return ({type})ret;
    }}
    return ({type})0;
}}"""

setter_definition = """int ParamSet{name}(const {type} value) {{
    int rc = -1;
    if ( !(value >= {min} && value <= {max}) ){{
        return rc;
    }}

    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {{
        param_values_[{index}] = (int32_t)value;
        k_mutex_unlock(&param_mutex);
        rc = 0;
    }}
    return rc;
}}"""

parameter_initializer = "    {{{id}, kParamName_{name}, {type}, {access}, kParamDescription_{name}, &param_values_[{index}], {min}, {max}, {exponent}}},"
param_name = 'static const char kParamName_{name}[] = "{display_name}";';
param_description = 'static const char kParamDescription_{name}[] = "{description}";';
category_name = 'static const char kCategoryName_{name}[] = "{display_name}";';

category_initializer = "    {{{id}, kCategoryName_{name}, {{{n_params_in_category_per_access_level}}}, {{{category_parameter_ptrs}}}}},"
category_parameter_ptr = "&params_[{parameter_ptr}]"

handle_set_case = """
    if (!strncmp(name, "{name}", name_len)) {{
        if (len != sizeof(int32_t)) {{
            LOG_ERR("[parameters] handle_set: read size (%o) different from size of int32_t (parameter {name})", len);
            return -EINVAL;
        }}

        int rc = -EBUSY;
        if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {{
            rc = read_cb(cb_arg, &param_values_[{index}], sizeof(int32_t));
            k_mutex_unlock(&param_mutex);
            if (rc >= 0) {{
                return 0;
            }}
        }}

        return rc;
    }}"""
handle_export_case = """
        ret = storage_func("parameters/{name}", &param_values_[{index}], sizeof(int32_t));
        if ( ret != 0 ) {{
            k_mutex_unlock(&param_mutex);
            return ret;
        }}
    """
settings_save_case = """
        case {id}:
            rc = settings_save_one("parameters/{name}", &param_values_[{index}], sizeof(int32_t));
            if ( rc != 0 ) {{
                LOG_ERR("[parameters] Unable to save parameter {name}");
            }}
            break;
"""
param_default_override = """
        case {machine_type_id}:
{param_value_setters}
            break;
"""
param_value_setter = "            param_values_[{index}] = {value};"


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
        self.overrides = {} # {str : {str : str}}, {"MachineType" : {"Parameter Id" : <value>}}
        
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
            
        for element in root.iter("DefaultOverride"):
            machine = element.get("MachineType")
            if machine in self.overrides.keys():
                error_txt = f'Duplicate override for MachineType {machine}'
                raise RuntimeError(error_txt)
            
            self.overrides[machine] = self._elements_to_unique_key_value_dict(element, "DefaultValue", "ParameterId", "Value")

        for element in root.iter("Parameter"):
            id = element.get("Id")
            if id in self.parameters:
                error_txt = f'Duplicate Parameter Id: {id}'
                raise RuntimeError(error_txt)

            self.parameters[id] = element.attrib

        for category in self.categories:
            if len([1 for p in self.parameters.values() if p["Category"] == category]) == 0:
                error_txt = f'Category "{category}" has no parameters'
                raise RuntimeError(error_txt)

        # parameter validation
        for id in self.parameters:

            if id == "0" and self.parameters[id]["Name"] != "Machine type":
                error_txt = f'Parameter with Id 0 must be "Machine type"'
                raise RuntimeError(error_txt)
            
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
                
            if any([attrib in self.parameters[id] for attrib in ["Min", "Max", "Exponent"]]):
                if not all([attrib in self.parameters[id] for attrib in ["Min", "Max", "Exponent"]]):
                    error_txt = f'Parameter with Id "{id}" must have all or none of attributes "Min", "Max", "Exponent"'
                    raise RuntimeError(error_txt)

            
            type = self.parameters[id]["Type"]
            if type in ["uint8_t", "uint16_t", "int8_t", "int16_t", "int32_t"]:
                try:
                    minimum = self.parameters[id]["Min"]
                    int(minimum)
                except ValueError:
                    error_txt = f'Attribute Min ("{minimum}") must be an integer for Type "{type}" (Parameter with Id "{id}")'
                    raise RuntimeError(error_txt)
                except KeyError:
                    error_txt = f'Missing Attribute Min for Parameter with Id "{id}"'
                    raise RuntimeError(error_txt)

                try:
                    maximum = self.parameters[id]["Max"]
                    int(maximum)
                except ValueError:
                    error_txt = f'Attribute Max ("{maximum}") must be an integer for Type "{type}" (Parameter with Id "{id}")'
                    raise RuntimeError(error_txt)
                except KeyError:
                    error_txt = f'Missing Attribute Max for Parameter with Id "{id}"'
                    raise RuntimeError(error_txt)

                if int(maximum) <= int(minimum):
                    error_txt = f'Attribute Max ("{maximum}") is smaller than Min ("{minimum}") (Parameter with Id "{id}")'
                    raise RuntimeError(error_txt)

            elif type not in self.enums.keys():
                error_txt = f'Unknown Type "{type}" for Parameter with Id "{id}"'
                raise RuntimeError(error_txt)

        if not "machine_type_t" in self.enums:
            error_txt = f'An enum named "machine_type_t" is required DefaultOverride'
            raise RuntimeError(error_txt)

        for machine_type in self.overrides:
            if machine_type not in self.enums["machine_type_t"]:
                error_txt = f'Unknown MachineType "{machine_type}" in DefaultOverride'
                raise RuntimeError(error_txt)
            for param_id in self.overrides[machine_type]:
                if param_id not in self.parameters:
                    error_txt = f'Unknown ParameterId "{param_id}" in DefaultOverride for MachineType "{machine_type}"'
                    raise RuntimeError(error_txt)



def to_camelcase(string : str) -> str:
    return "".join([s.capitalize() for s in string.split()]).replace(".", "")
    
class SourceGenerator:
    """
    Creates C header and source file for parameters
    """
    def __init__(self, config : Configuration):
        self.config = config

    def generate(self, header_path: Path, source_path: Path):
        enums = []

        get_value_string_funcs = []
        param_value_string_len = [10] # 10 chars for int32_t
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
        handle_set_cases = []
        handle_export_cases = []
        settings_save_cases = []
        param_names = []
        param_descriptions = []
        param_default_values = []
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
            type_name = f"{type}"
            if type in self.config.enums:
                default = f"kParam{default}"
                type_name = f"param_{type}"
                get_value_string_cases.append(get_value_string_case.format(id=param, type=type_name))
                
            setter_definitions.append(setter_definition.format(
                index=i,
                type=type_name,
                name=name,
                min=minimum,
                max=maximum
            ))
            parameter_initializers.append(parameter_initializer.format(
                index=i,
                id=param,
                name=name,
                type="kParamTypeEnum" if type in self.config.enums else "kParamTypeNumeric",
                access=self.config.access_levels[self.config.parameters[param]["AccessLevel"]],
                min=minimum,
                max=maximum,
                exponent=self.config.parameters[param]["Exponent"] if "Exponent" in self.config.parameters[param] else 0
            ))
            getter_declarations.append(getter_declaration.format(type=type_name, name=name))
            setter_declarations.append(setter_declaration.format(type=type_name, name=name))
            getter_definitions.append(getter_definition.format(type=type_name, name=name, index=i))
            handle_set_cases.append(handle_set_case.format(name=name, index=i))
            handle_export_cases.append(handle_export_case.format(name=name, index=i))
            settings_save_cases.append(settings_save_case.format(name=name, index=i, id=param))
            param_names.append(param_name.format(name=name, display_name=self.config.parameters[param]["Name"]))
            param_descriptions.append(param_description.format(name=name, description=self.config.parameters[param]["Description"]))
            param_default_values.append(param_value_setter.format(index=i, value=default))
            
        categories = []
        n_params_in_category = []
        category_initializers = []
        category_names = []
        for i,name in enumerate(sorted(self.config.categories)):
            filtered_parameters = [ self.config.parameters[key] for key in self.config.parameters
                                    if self.config.parameters[key]["Category"] == name ]
            sorted_filtered_parameters = sorted(filtered_parameters, key=lambda d: d["Name"])
            sorted_filtered_parameters_indices = [ id_to_index[p["Id"]] for p in sorted_filtered_parameters ]
            n_params_per_access_level = [ len([ param for param in filtered_parameters
                                                if int(self.config.access_levels[param["AccessLevel"]]) <= int(access)])
                                          for access in sorted(self.config.access_levels.values()) 
                                         ]
            category_initializers.append(category_initializer.format(
                id=self.config.categories[name],
                index=i,
                name=to_camelcase(name),
                n_params_in_category_per_access_level=", ".join(f"{n}" for n in n_params_per_access_level),
                category_parameter_ptrs=", ".join([category_parameter_ptr.format(parameter_ptr=p) for p in sorted_filtered_parameters_indices])
            ))
            n_params_in_category.append(n_params_per_access_level[-1]) # last element is the highest access level which has access to all parameters
            category_names.append(category_name.format(name=to_camelcase(name), display_name=name))

        param_default_overrides = []
        for machine_type in self.config.overrides:
            param_value_setters = []
            for param_id in self.config.overrides[machine_type]:
                default = self.config.overrides[machine_type][param_id]
                if self.config.parameters[param_id]["Type"] in self.config.enums:
                    default = f"kParam{default}"
                param_value_setters.append(param_value_setter.format(index=id_to_index[param_id], value=default))
            param_default_overrides.append(param_default_override.format(
                machine_type_id=self.config.enums["machine_type_t"][machine_type],
                param_value_setters="\n".join(param_value_setters))
            )
        
        header_content = header.format(
            n_access_levels=len(self.config.access_levels),
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
            get_value_string_cases="\n".join(get_value_string_cases),
            handle_export_cases="\n".join(handle_export_cases),
            handle_set_cases="\n".join(handle_set_cases),
            settings_save_cases="\n".join(settings_save_cases),
            param_names="\n".join(param_names),
            param_descriptions="\n".join(param_descriptions),
            category_names="\n".join(category_names),
            param_production_default_values="\n    ".join(param_default_values),
            param_default_overrides="\n".join(param_default_overrides),
            parameters_header=header_path.relative_to(os.path.commonprefix([source_path.parent, header_path.parent])),
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
    
