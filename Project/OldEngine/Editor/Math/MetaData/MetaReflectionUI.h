#pragma once

/// externals
#include <imgui/imgui.h>
#include <magic_enum/magic_enum.hpp>


namespace Editor {



template<typename T>
struct MetaReflection {};



template<typename Class, typename Member>
struct Field {
	const char* name;
	Member Class::* member;
};


template<typename Class, typename Member>
constexpr Field<Class, Member> MakeField(const char* name, Member Class::* member) {
	return { name, member };
}



template<typename T>
concept HasReflection = requires {
	MetaReflection<T>::GetFields();
};

template<typename T>
void DrawMetaUI(T& data) {
	if constexpr(HasReflection<T>) {
		auto fields = MetaReflection<T>::GetFields();

		std::apply([&](auto&&... field) {
			((DrawField(data, field)), ...);
		}, fields);
	}
}


template<typename T, typename FieldType>
void DrawField(T& obj, Field<T, FieldType> field) {
	auto& value = obj.*(field.member);

	if constexpr(std::is_same_v<FieldType, std::string>) {
		char buffer[256];
		strncpy(buffer, value.c_str(), sizeof(buffer));
		buffer[sizeof(buffer) - 1] = '\0';

		if(ImGui::InputText(field.name, buffer, sizeof(buffer))) {
			value = buffer;
		}
	} else if constexpr(std::is_same_v<FieldType, int>) {
		ImGui::DragInt(field.name, &value);
	} else if constexpr(std::is_same_v<FieldType, float>) {
		ImGui::DragFloat(field.name, &value);
	} else if constexpr(std::is_same_v<FieldType, bool>) {
		ImGui::Checkbox(field.name, &value);
	} else if constexpr(std::is_enum_v<FieldType>) {
		std::string preview = std::string(magic_enum::enum_name(value));

		if(ImGui::BeginCombo(field.name, preview.c_str())) {
			for(auto v : magic_enum::enum_values<FieldType>()) {
				bool selected = (v == value);
				std::string name = std::string(magic_enum::enum_name(v));

				if(ImGui::Selectable(name.c_str(), selected)) {
					value = v;
				}
			}
			ImGui::EndCombo();
		}
	}
}





} /// namespace Editor