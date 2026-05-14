import yaml
import os
import glob

# Get the directory where the script is located (Project/Tools)
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
# Repository root is two levels up
REPO_ROOT = os.path.dirname(os.path.dirname(SCRIPT_DIR))

# Configuration (Relative to REPO_ROOT)
INPUT_DIR = os.path.join(REPO_ROOT, "Project/Tools/Schema")
CPP_OUTPUT_DIR = os.path.join(REPO_ROOT, "Project/Schema")
HLSL_OUTPUT_DIR = os.path.join(REPO_ROOT, "Project/Assets/Shader/Schema")
CS_OUTPUT_DIR = os.path.join(REPO_ROOT, "SubProjects/ONEngine.Scripting/Generated")

TYPE_MAP_CPP = {
    "float4x4": "Engine::Math::Matrix4x4",
    "float4": "Engine::Math::Vector4",
    "float3": "Engine::Math::Vector3",
    "float2": "Engine::Math::Vector2",
    "float": "float",
    "uint32": "uint32_t",
    "uint64": "uint64_t",
    "string": "char",
}

DEFAULT_VALUES_CPP = {
    "float4x4": "Engine::Math::Matrix4x4::kIdentity",
    "float4": "{ 0, 0, 0, 0 }",
    "float3": "{ 0, 0, 0 }",
    "float2": "{ 0, 0 }",
    "float": "0.0f",
    "uint32": "0",
    "uint64": "0",
}

TYPE_MAP_HLSL = {
    "float4x4": "row_major float4x4",
    "float4": "float4",
    "float3": "float3",
    "float2": "float2",
    "float": "float",
    "uint32": "uint",
    "uint64": "uint2", 
}

TYPE_MAP_CS = {
    "float4x4": "Matrix4x4",
    "float4": "Vector4",
    "float3": "Vector3",
    "float2": "Vector2",
    "float": "float",
    "uint32": "uint",
    "uint64": "ulong",
}

TYPE_SIZES = {
    "float4x4": 64,
    "float4": 16,
    "float3": 12,
    "float2": 8,
    "float": 4,
    "uint32": 4,
    "uint64": 8,
    "string": 256,
}

def generate_serialization(type_name, fields, namespace):
    full_name = "{}::{}".format(namespace, type_name)
    to_json = "inline void to_json(nlohmann::json& j, const {}& v) {{\n".format(full_name)
    to_json += "    j = nlohmann::json{\n"
    
    from_json = "inline void from_json(const nlohmann::json& j, {}& v) {{\n".format(full_name)
    
    for field in fields:
        f_name = field["name"]
        f_type = field["type"]
        f_count = field.get("count", 1)
        
        if f_type == "string":
            to_json += "        {{\"{}\", std::string(v.{})}},\n".format(f_name, f_name)
            from_json += "    if (j.contains(\"{}\")) {{ std::string s = j.at(\"{}\").get<std::string>(); size_t len = (std::min)(s.length(), sizeof(v.{}) - 1); std::memcpy(v.{}, s.c_str(), len); v.{}[len] = '\\0'; }}\n".format(f_name, f_name, f_name, f_name, f_name)
        elif f_count > 1:
            to_json += "        {{\"{}\", v.{}}},\n".format(f_name, f_name)
            from_json += "    if (j.contains(\"{}\")) {{ auto& arr = j.at(\"{}\"); for(int i=0; i<{}; ++i) v.{}[i] = arr.at(i).get<{}>(); }}\n".format(f_name, f_name, f_count, f_name, TYPE_MAP_CPP[f_type])
        else:
            to_json += "        {{\"{}\", v.{}}},\n".format(f_name, f_name)
            from_json += "    if (j.contains(\"{}\")) v.{} = j.at(\"{}\").get<{}>();\n".format(f_name, f_name, f_name, TYPE_MAP_CPP[f_type])
            
    to_json += "    };\n}\n"
    from_json += "}\n"
    return to_json + "\n" + from_json

def generate_editor_ui(type_name, fields, namespace):
    full_name = "{}::{}".format(namespace, type_name)
    ui_code = "template<typename TProp>\n"
    ui_code += "inline void DrawUI_{}({}& v, TProp Prop) {{\n".format(type_name, full_name)
    
    for field in fields:
        if field.get("hide_in_editor", False):
            continue

        f_name = field["name"]
        f_type = field["type"]
        asset_type = field.get("asset_type")
        
        is_color = "color" in f_name.lower()
        label = f_name[0].upper() + f_name[1:]
        
        if asset_type:
            index_field_name = f_name.replace("Guid", "Index")
            has_index_field = any(f["name"] == index_field_name for f in fields)
            
            if has_index_field:
                if asset_type == "Model":
                    update_logic = "v.{} = ::Engine::Asset::AssetManager::GetInstance().LoadModel(std::to_string(v.{}));".format(index_field_name, f_name)
                elif asset_type == "Material":
                    update_logic = "v.{} = (uint32_t)::Engine::Asset::MaterialManager::GetInstance().LoadMaterial(v.{});".format(index_field_name, f_name)
                elif asset_type == "Texture":
                    update_logic = "v.{} = (uint32_t)::Engine::Asset::TextureManager::GetInstance().LoadTexture(v.{});".format(index_field_name, f_name)
                elif asset_type == "Font":
                    update_logic = "v.{} = (uint32_t)::Engine::Asset::FontManager::GetInstance().LoadFont(v.{});".format(index_field_name, f_name)
                else:
                    update_logic = ""

                ui_code += "    Prop(\"{}\", [&]() {{ bool changed = ::Engine::Editor::EditorUI::AssetPicker(\"{}\", \"{}\", &v.{}); if (changed) {{ {} }} return changed; }});\n".format(f_name, label, asset_type, f_name, update_logic)
            else:
                ui_code += "    Prop(\"{}\", [&]() {{ return ::Engine::Editor::EditorUI::AssetPicker(\"{}\", \"{}\", &v.{}); }});\n".format(f_name, label, asset_type, f_name)
        elif f_type == "float":
            ui_code += "    Prop(\"{}\", [&]() {{ return ImGui::DragFloat(\"{}\", &v.{}, 0.1f); }});\n".format(label, label, f_name)
        elif f_type == "float2":
            ui_code += "    Prop(\"{}\", [&]() {{ return ImGui::DragFloat2(\"{}\", &v.{}.x, 0.1f); }});\n".format(label, label, f_name)
        elif f_type == "float3":
            if is_color:
                ui_code += "    Prop(\"{}\", [&]() {{ return ImGui::ColorEdit3(\"{}\", &v.{}.x); }});\n".format(label, label, f_name)
            else:
                ui_code += "    Prop(\"{}\", [&]() {{ return ImGui::DragFloat3(\"{}\", &v.{}.x, 0.1f); }});\n".format(label, label, f_name)
        elif f_type == "float4":
            if is_color:
                ui_code += "    Prop(\"{}\", [&]() {{ return ImGui::ColorEdit4(\"{}\", &v.{}.x); }});\n".format(label, label, f_name)
            else:
                ui_code += "    Prop(\"{}\", [&]() {{ return ImGui::DragFloat4(\"{}\", &v.{}.x, 0.1f); }});\n".format(label, label, f_name)
        elif f_type == "uint32":
            if "index" in f_name.lower() or "id" in f_name.lower() or "count" in f_name.lower():
                ui_code += "    Prop(\"{}\", [&]() {{ return ImGui::InputScalar(\"{}\", ImGuiDataType_U32, &v.{}); }});\n".format(label, label, f_name)
            elif "is" in f_name.lower() or "enabled" in f_name.lower() or "flag" in f_name.lower():
                ui_code += "    Prop(\"{}\", [&]() {{ return ImGui::Checkbox(\"{}\", (bool*)&v.{}); }});\n".format(label, label, f_name)
            else:
                ui_code += "    Prop(\"{}\", [&]() {{ return ImGui::InputScalar(\"{}\", ImGuiDataType_U32, &v.{}); }});\n".format(label, label, f_name)
        elif f_type == "uint64":
            ui_code += "    Prop(\"{}\", [&]() {{ return ImGui::InputScalar(\"{}\", ImGuiDataType_U64, &v.{}); }});\n".format(label, label, f_name)
        elif f_type == "string":
            ui_code += "    Prop(\"{}\", [&]() {{ return ImGui::InputText(\"{}\", v.{}, sizeof(v.{})); }});\n".format(label, label, f_name, f_name)
        elif f_type == "float4x4":
            ui_code += "    ImGui::Text(\"{}: Matrix4x4\", \"Matrix\");\n".format(label)

    ui_code += "}\n\n"
    return ui_code

def process_file(yaml_path):
    base_name = os.path.splitext(os.path.basename(yaml_path))[0]
    
    with open(yaml_path, "r") as f:
        schema = yaml.safe_load(f)

    cpp_content = "#pragma once\n#include \"Engine/Core/Math/Math.h\"\n#include <cstdint>\n#include <nlohmann/json.hpp>\n#include <cstring>\n#include <string>\n#include <algorithm>\n\n"
    hlsl_content = "// Generated by codegen.py\n\n"
    cs_content = "using System.Runtime.InteropServices;\nusing ONEngine.Scripting.Math;\n\nnamespace ONEngine.Scripting.Generated\n{\n"

    is_component_file = base_name.lower() == "components"
    namespace = "Engine::ECS" if is_component_file else "Engine::GeneratedSchema"
    
    cpp_content += "namespace {} {{\n\n".format(namespace)
    
    serialization_content = ""
    ui_content = ""
    has_hlsl_data = False

    for type_name, info in schema.get("types", {}).items():
        type_category = info.get("type")
        is_comp = type_category == "Component"
        fields = info.get("fields", [])
        
        cpp_struct_body = ""
        hlsl_struct_body = ""
        cs_struct_body = ""
        current_offset = 0
        pad_index = 0
        
        for field in fields:
            f_name = field["name"]
            f_type = field["type"]
            f_size = TYPE_SIZES[f_type]
            
            if f_type != "string":
                remaining_in_chunk = 16 - (current_offset % 16)
                if f_size > remaining_in_chunk and remaining_in_chunk < 16:
                    pad_size = remaining_in_chunk
                    cpp_struct_body += "    uint8_t _pad{}[{}];\n".format(pad_index, pad_size)
                    hlsl_struct_body += "    uint _pad{}[{}];\n".format(pad_index, pad_size // 4)
                    cs_struct_body += "        private unsafe fixed byte _pad{}[{}];\n".format(pad_index, pad_size)
                    pad_index += 1
                    current_offset += pad_size

            # Special case for Transform scale/rotation/world default values
            cpp_default = DEFAULT_VALUES_CPP.get(f_type, "")
            if type_name == "Transform":
                if f_name == "scale": cpp_default = "{ 1.0f, 1.0f, 1.0f }"
                elif f_name == "world": cpp_default = "Engine::Math::Matrix4x4::kIdentity"
            elif "color" in f_name.lower():
                cpp_default = "{ 1.0f, 1.0f, 1.0f, 1.0f }" if f_type == "float4" else "{ 1.0f, 1.0f, 1.0f }"

            if f_type == "string":
                cpp_struct_body += "    char {}[256];\n".format(f_name)
                hlsl_struct_body += "    uint {}[64];\n".format(f_name)
                cs_struct_body += "        public unsafe fixed byte {}[256];\n".format(f_name)
            else:
                f_count = field.get("count", 1)
                if f_count > 1:
                    cpp_struct_body += "    {} {}[{}]".format(TYPE_MAP_CPP[f_type], f_name, f_count)
                    if cpp_default and "{" in cpp_default:
                        cpp_struct_body += " = " + cpp_default
                    cpp_struct_body += ";\n"

                    hlsl_struct_body += "    {} {}[{}];\n".format(TYPE_MAP_HLSL[f_type], f_name, f_count)
                    cs_struct_body += "        public unsafe fixed {} {}[{}];\n".format(TYPE_MAP_CS[f_type], f_name, f_count)
                    f_size *= f_count
                else:
                    cpp_struct_body += "    {} {}".format(TYPE_MAP_CPP[f_type], f_name)
                    if cpp_default:
                        cpp_struct_body += " = " + cpp_default
                    cpp_struct_body += ";\n"

                    hlsl_struct_body += "    {} {};\n".format(TYPE_MAP_HLSL[f_type], f_name)
                    cs_struct_body += "        public {} {};\n".format(TYPE_MAP_CS[f_type], f_name)
            current_offset += f_size

        if current_offset % 16 != 0:
            pad_size = 16 - (current_offset % 16)
            cpp_struct_body += "    uint8_t _final_pad{}[{}];\n".format(pad_index, pad_size)
            hlsl_struct_body += "    uint _final_pad{}[{}];\n".format(pad_index, pad_size // 4)
            cs_struct_body += "        private unsafe fixed byte _final_pad{}[{}];\n".format(pad_index, pad_size)
            current_offset += pad_size

        cpp_content += "struct {} {{\n{}}};\n\n".format(type_name, cpp_struct_body)
        cs_content += "    [StructLayout(LayoutKind.Sequential)]\n    public unsafe struct {}\n    {{\n{}{}    }}\n\n".format(type_name, "", cs_struct_body)
        
        if type_category in ["ConstantBuffer", "StructuredBuffer"]:
            hlsl_content += "struct {} {{\n{}}};\n\n".format(type_name, hlsl_struct_body)
            has_hlsl_data = True
            
        serialization_content += generate_serialization(type_name, fields, namespace)
        if is_comp:
            ui_content += generate_editor_ui(type_name, fields, namespace)

    cpp_content += "}} // namespace {}\n\n".format(namespace)
    cpp_content += serialization_content
    
    if is_component_file:
        cpp_content += "\n#ifdef ENGINE_EDITOR\n#include \"imgui.h\"\n#include \"Editor/EditorUI.h\"\n"
        cpp_content += "#include \"Engine/Asset/AssetManager.h\"\n"
        cpp_content += "#include \"Engine/Asset/MaterialManager.h\"\n"
        cpp_content += "#include \"Engine/Asset/TextureManager.h\"\n"
        cpp_content += "#include \"Engine/Asset/FontManager.h\"\n"
        cpp_content += "namespace {} {{\n".format(namespace)
        cpp_content += ui_content
        cpp_content += "}} // namespace {}\n".format(namespace)
        cpp_content += "#endif // ENGINE_EDITOR\n"

    cs_content += "}\n"

    cpp_path = os.path.join(CPP_OUTPUT_DIR, base_name + ".h")
    with open(cpp_path, "w") as f: f.write(cpp_content)
    
    if has_hlsl_data:
        hlsl_path = os.path.join(HLSL_OUTPUT_DIR, base_name + ".hlsli")
        with open(hlsl_path, "w") as f: f.write(hlsl_content)

    cs_path = os.path.join(CS_OUTPUT_DIR, base_name + ".cs")
    with open(cs_path, "w") as f: f.write(cs_content)
    
    print("Processed {}: Generated .h, .cs{}".format(base_name, ", .hlsli" if has_hlsl_data else ""))

def generate():
    if not os.path.exists(INPUT_DIR):
        print("Error: {} not found (calculated as {}).".format(INPUT_DIR, os.path.abspath(INPUT_DIR)))
        return

    yaml_files = glob.glob(os.path.join(INPUT_DIR, "*.yaml"))
    for yaml_path in yaml_files:
        process_file(yaml_path)

if __name__ == "__main__":
    generate()
