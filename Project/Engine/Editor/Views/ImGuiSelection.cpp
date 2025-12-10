#include "ImGuiSelection.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"

using namespace ONEngine;

namespace {

	/// ----- ImGuiSelection ----- ///
	const Guid* gSelectionObjectGuid = &Guid::kInvalid;
	SelectionType gSelectionType = SelectionType::None;

	/// ----- ImGuiInfo ----- ///
	std::string gInfo;

} /// namespace

 
const Guid& ImGuiSelection::GetSelectedObject() {
	return *gSelectionObjectGuid;
}

void ImGuiSelection::SetSelectedObject(const Guid& _entityGuid, SelectionType _type) {
	gSelectionObjectGuid = &_entityGuid;
	gSelectionType = _type;
}

SelectionType ImGuiSelection::GetSelectionType() {
	return gSelectionType;
}



const std::string& ImGuiInfo::GetInfo() {
	return gInfo;
}

void ImGuiInfo::SetInfo(const std::string& _info) {
	gInfo = _info;
}
