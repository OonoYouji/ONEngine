# -*- coding: utf-8 -*-
import yaml
import os

# 型のサイズ定義 (byte)
TYPE_SIZES = {
    'float': 4,
    'int': 4,
    'uint32': 4,
    'uint64': 8,
    'bool': 4, # HLSL互換のため4バイト
    'vec2': 8,
    'vec3': 12,
    'vec4': 16,
    'mat4': 64,
    'guid': 16,
}

CPP_TYPE_MAP = {
    'float': 'float',
    'int': 'int32_t',
    'uint32': 'uint32_t',
    'uint64': 'uint64_t',
    'bool': 'bool',
    'vec2': 'Engine::Vector2',
    'vec3': 'Engine::Vector3',
    'vec4': 'Engine::Vector4',
    'mat4': 'Engine::Matrix4x4',
    'guid': 'Engine::Guid',
}

CS_TYPE_MAP = {
    'float': 'float',
    'int': 'int',
    'uint32': 'uint',
    'uint64': 'ulong',
    'bool': 'uint', # C#のboolは1バイトでBlittableでない場合があるため、4バイトのuintとして扱う
    'vec2': 'Vector2',
    'vec3': 'Vector3',
    'vec4': 'Vector4',
    'mat4': 'Matrix4x4',
    'guid': 'Engine.Core.Guid',
}

HLSL_TYPE_MAP = {
    'float': 'float',
    'int': 'int',
    'uint32': 'uint',
    'uint64': 'uint2', 
    'bool': 'bool',
    'vec2': 'float2',
    'vec3': 'float3',
    'vec4': 'float4',
    'mat4': 'float4x4',
    'guid': 'uint4',
}

def calculate_alignment(fields, is_gpu):
    layout = []
    current_offset = 0
    max_align = 8 # 基本は8byteアライメント
    
    for name, info in fields.items():
        size = TYPE_SIZES.get(info['type'], 4)
        field_align = 8 if size >= 8 else 4
        if info['type'] == 'vec3': field_align = 4 # Vector3自体は12バイトだが4バイト境界
        if info['type'] == 'vec4': field_align = 16 # Vector4は16バイト境界 (GPU最適化)
        if info['type'] == 'mat4': field_align = 16 # 行列は16バイト境界
        if info['type'] == 'guid': field_align = 16 # GUIDは16バイト境界
        
        # 自然アライメントの調整
        if current_offset % field_align != 0:
            pad_size = field_align - (current_offset % field_align)
            layout.append({
                'name': f'_pad_align_{current_offset}',
                'size': pad_size,
                'offset': current_offset,
                'is_padding': True
            })
            current_offset += pad_size

        if is_gpu:
            max_align = 16
            # HLSLパッキング規則: 要素が16バイト境界を跨いではならない
            remaining_in_slot = 16 - (current_offset % 16)
            if remaining_in_slot < size and info['type'] not in ['mat4', 'guid']: 
                pad_size = remaining_in_slot
                layout.append({
                    'name': f'_pad_gpu_{current_offset}',
                    'size': pad_size,
                    'offset': current_offset,
                    'is_padding': True
                })
                current_offset += pad_size
        
        layout.append({
            'name': name,
            'type': info['type'],
            'size': size,
            'offset': current_offset,
            'is_padding': False
        })
        current_offset += size
    
    # 構造体全体のサイズをアライメントに合わせる
    if current_offset % max_align != 0:
        pad_size = max_align - (current_offset % max_align)
        layout.append({
            'name': f'_pad_final',
            'size': pad_size,
            'offset': current_offset,
            'is_padding': True
        })
        current_offset += pad_size
        
    return layout, current_offset

def generate_cpp(schemas):
    lines = ["#pragma once", "#include <cstdint>", "#include \"EngineMath.h\"", ""]
    for name, schema in schemas.items():
        is_gpu = schema.get('is_gpu_buffer', False)
        layout, size = calculate_alignment(schema['fields'], is_gpu)
        
        lines.append(f"// {schema.get('description', '')}")
        if is_gpu:
            lines.append(f"struct alignas(16) {name} {{")
        else:
            lines.append(f"struct {name} {{")
            
        for field in layout:
            if field['is_padding']:
                lines.append(f"    uint8_t {field['name']}[{field['size']}];")
            else:
                cpp_type = CPP_TYPE_MAP.get(field['type'], 'float')
                lines.append(f"    {cpp_type} {field['name']};")
        
        lines.append(f"}}; // size: {size}")
        lines.append("")
    return "\n".join(lines)

def generate_cs(schemas):
    lines = ["using System;", "using System.Runtime.InteropServices;", "using Engine.Core;", "", "namespace Engine.Generated {"]
    
    # 構造体定義
    for name, schema in schemas.items():
        is_gpu = schema.get('is_gpu_buffer', False)
        layout, size = calculate_alignment(schema['fields'], is_gpu)
        
        lines.append(f"    // {schema.get('description', '')}")
        lines.append("    [StructLayout(LayoutKind.Explicit, Size = " + str(size) + ")]")
        lines.append(f"    public struct {name} {{")
        
        for field in layout:
            if field['is_padding']: continue
            cs_type = CS_TYPE_MAP.get(field['type'], 'float')
            lines.append(f"        [FieldOffset({field['offset']})]")
            lines.append(f"        public {cs_type} {field['name']};")
            
        lines.append("    }")
        lines.append("")

    # 便利機能 (Worldの拡張メソッド)
    lines.append("    public static class EcsExtensions {")
    for name, schema in schemas.items():
        fields = schema['fields']
        arg_list = [f"{CS_TYPE_MAP[f['type']]} {n}" for n, f in fields.items()]
        args = ", ".join(arg_list)
        if args: args = ", " + args
        
        vals = ", ".join([f"{n} = {n}" for n in fields.keys()])
        
        lines.append(f"        public static void Add{name}(this EcsWorld world, ulong entityId{args}) {{")
        lines.append(f"            world.AddComponent(entityId, new {name} {{ {vals} }});")
        lines.append("        }")
    lines.append("    }")
    lines.append("")

    # JSONからコンポーネントを生成するためのレジストリ
    lines.append("    public static class ComponentRegistry {")
    lines.append("        public static void AddComponentFromJson(EcsWorld world, ulong entityId, string typeName, System.Text.Json.JsonElement data) {")
    lines.append("            UpdateComponentFromJson(world, entityId, typeName, data);")
    lines.append("        }")
    lines.append("")
    lines.append("        public static void UpdateComponentFromJson(EcsWorld world, ulong entityId, string typeName, System.Text.Json.JsonElement data) {")
    lines.append("            switch (typeName) {")
    for name, schema in schemas.items():
        lines.append(f"                case \"{name}\": {{")
        lines.append(f"                    var comp = world.HasComponent<{name}>(entityId) ? world.GetComponent<{name}>(entityId) : new {name}();")
        for field_name, field_info in schema['fields'].items():
            field_type = field_info['type']
            cs_type = CS_TYPE_MAP[field_type]
            default_val = field_info.get('default', 0)
            
            lines.append(f"                    if (data.TryGetProperty(\"{field_name}\", out var prop_{field_name})) {{")
            if field_type == 'vec2':
                lines.append(f"                        comp.{field_name} = new Vector2(prop_{field_name}[0].GetSingle(), prop_{field_name}[1].GetSingle());")
            elif field_type == 'vec3':
                lines.append(f"                        comp.{field_name} = new Vector3(prop_{field_name}[0].GetSingle(), prop_{field_name}[1].GetSingle(), prop_{field_name}[2].GetSingle());")
            elif field_type == 'vec4':
                lines.append(f"                        comp.{field_name} = new Vector4(prop_{field_name}[0].GetSingle(), prop_{field_name}[1].GetSingle(), prop_{field_name}[2].GetSingle(), prop_{field_name}[3].GetSingle());")
            elif field_type == 'mat4':
                lines.append(f"                        float[] m = new float[16]; for(int i=0; i<16; i++) m[i] = prop_{field_name}[i].GetSingle();")
                lines.append(f"                        comp.{field_name} = new Matrix4x4(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);")
            elif field_type == 'guid':
                lines.append(f"                        string s_{field_name} = prop_{field_name}.GetString() ?? \"\";")
                lines.append(f"                        comp.{field_name} = System.Guid.TryParse(s_{field_name}, out var g_{field_name}) ? (Engine.Core.Guid)g_{field_name} : new Engine.Core.Guid();")
            elif cs_type == 'float':
                lines.append(f"                        comp.{field_name} = prop_{field_name}.GetSingle();")
            elif cs_type == 'int':
                lines.append(f"                        comp.{field_name} = prop_{field_name}.GetInt32();")
            elif cs_type == 'uint' and field_type == 'bool':
                lines.append(f"                        comp.{field_name} = prop_{field_name}.GetBoolean() ? 1u : 0u;")
            elif cs_type == 'uint':
                lines.append(f"                        comp.{field_name} = prop_{field_name}.GetUInt32();")
            elif cs_type == 'ulong':
                lines.append(f"                        comp.{field_name} = prop_{field_name}.GetUInt64();")
            elif cs_type == 'bool':
                lines.append(f"                        comp.{field_name} = prop_{field_name}.GetBoolean();")
            else:
                lines.append(f"                        comp.{field_name} = ({cs_type})prop_{field_name}.GetDouble();")
            
            # 既存コンポーネントがない場合のみデフォルト値を設定
            lines.append("                    } else if (!world.HasComponent<" + name + ">(entityId)) {")
            if field_type == 'vec2':
                v = default_val if isinstance(default_val, list) else [0, 0]
                lines.append(f"                        comp.{field_name} = new Vector2({v[0]}f, {v[1]}f);")
            elif field_type == 'vec3':
                v = default_val if isinstance(default_val, list) else [0, 0, 0]
                lines.append(f"                        comp.{field_name} = new Vector3({v[0]}f, {v[1]}f, {v[2]}f);")
            elif field_type == 'vec4':
                v = default_val if isinstance(default_val, list) else [0, 0, 0, 0]
                lines.append(f"                        comp.{field_name} = new Vector4({v[0]}f, {v[1]}f, {v[2]}f, {v[3]}f);")
            elif field_type == 'mat4':
                lines.append(f"                        comp.{field_name} = Matrix4x4.Identity;")
            elif field_type == 'guid':
                lines.append(f"                        comp.{field_name} = new Engine.Core.Guid();")
            elif cs_type == 'uint' and field_type == 'bool':
                val = "1u" if str(default_val).lower() == "true" else "0u"
                lines.append(f"                        comp.{field_name} = {val};")
            elif cs_type == 'float':
                lines.append(f"                        comp.{field_name} = {default_val}f;")
            elif cs_type == 'bool':
                lines.append(f"                        comp.{field_name} = {str(default_val).lower()};")
            else:
                lines.append(f"                        comp.{field_name} = {default_val};")
            lines.append("                    }")
            
        lines.append(f"                    world.AddComponent(entityId, comp);")
        lines.append("                    break;")
        lines.append("                }")
    lines.append("                default: throw new ArgumentException($\"Unknown component type: {typeName}\");")
    lines.append("            }")
    lines.append("        }")
    lines.append("    }")
    lines.append("")

    # 自動生成された型ID管理クラス
    lines.append("    public static class ComponentID<T> {")
    lines.append("        public static readonly uint ID = ComponentID.GetInternal<T>();")
    lines.append("    }")
    lines.append("")
    lines.append("    public static class ComponentID {")
    lines.append("        private const string DLL_NAME = \"Temp.exe\";")
    for name in schemas.keys():
        lines.append(f"        [DllImport(DLL_NAME, CallingConvention = CallingConvention.Cdecl)] private static extern uint Ecs_GetTypeId_{name}();")
    
    lines.append("")
    lines.append("        public static uint Get<T>() => ComponentID<T>.ID;")
    lines.append("")
    lines.append("        internal static uint GetInternal<T>() {")
    for name in schemas.keys():
        lines.append(f"            if (typeof(T) == typeof({name})) return Ecs_GetTypeId_{name}();")
    lines.append("            return 0;")
    lines.append("        }")
    lines.append("    }")
    
    lines.append("}")
    return "\n".join(lines)

def generate_hlsl(schemas):
    lines = ["// Generated HLSL Components", ""]
    for name, schema in schemas.items():
        if not schema.get('is_gpu_buffer', False): continue
        
        layout, size = calculate_alignment(schema['fields'], True)
        lines.append(f"struct {name} {{")
        
        for field in layout:
            if field['is_padding']:
                count = field['size'] // 4
                if count == 1: lines.append(f"    float {field['name']};")
                else: lines.append(f"    float{count} {field['name']};")
            else:
                hlsl_type = HLSL_TYPE_MAP.get(field['type'], 'float')
                lines.append(f"    {hlsl_type} {field['name']};")
        
        lines.append("};")
        lines.append("")
    return "\n".join(lines)

def main():
    # 実行スクリプトのディレクトリ (tools/)
    tools_dir = os.path.dirname(os.path.abspath(__file__))
    # プロジェクトルート (tools/ の親)
    project_root = os.path.dirname(tools_dir)
    schema_path = os.path.join(tools_dir, 'schema.yaml')

    if not os.path.exists(schema_path):
        print(f"Error: {schema_path} not found.")
        return

    with open(schema_path, 'r', encoding='utf-8') as f:
        config = yaml.safe_load(f)
    
    schemas = config.get('components', {})
    
    # 出力先ディレクトリの設定
    cpp_out_dir = os.path.join(project_root, 'source', 'core')
    cs_out_dir = os.path.join(project_root, 'source', 'script_api')

    # 各言語のファイルを生成
    files = {
        os.path.join(cpp_out_dir, 'GeneratedComponents.h'): generate_cpp(schemas),
        os.path.join(cs_out_dir, 'GeneratedComponents.cs'): generate_cs(schemas),
        os.path.join(cpp_out_dir, 'GeneratedComponents.hlsli'): generate_hlsl(schemas),
        os.path.join(project_root, 'source', 'interop', 'GeneratedInterop.cpp'): generate_interop_cpp(schemas)
    }
    
    for path, content in files.items():
        # ディレクトリが存在しない場合は作成
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"Generated: {path}")

    print("Code generation successful (C++, C#, HLSL, Interop)!")

def generate_interop_cpp(schemas):
    lines = [
        "#include \"EcsInterop.h\"",
        "#include \"EcsWorld.h\"",
        "#include \"GeneratedComponents.h\"",
        "",
        "// 自動生成された型登録コード",
        "#define REGISTER_TYPE(Type) \\",
        "    extern \"C\" EXPORT uint32_t Ecs_GetTypeId_##Type() { return TypeID::get<Type>(); } \\",
        "    static inline uint32_t _reg_##Type = []() { \\",
        "        uint32_t id = TypeID::get<Type>(); \\",
        "        TypeRegistry::Register(id, []() { return std::make_unique<SparseSet<Type>>(); }); \\",
        "        return id; \\",
        "    }();",
        ""
    ]
    for name in schemas.keys():
        lines.append(f"REGISTER_TYPE({name})")
    
    return "\n".join(lines)

if __name__ == "__main__":
    main()
