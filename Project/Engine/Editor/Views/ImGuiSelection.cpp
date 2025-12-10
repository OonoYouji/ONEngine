#include "ImGuiSelection.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"

using namespace Editor;

namespace {

	/// ----- ImGuiSelection ----- ///
	const ONEngine::Guid* gSelectionObjectGuid = &ONEngine::Guid::kInvalid;
	SelectionType gSelectionType = SelectionType::None;

	/// ----- ImGuiInfo ----- ///
	std::string gInfo;

} /// namespace

 
const ONEngine::Guid& ImGuiSelection::GetSelectedObject() {
	return *gSelectionObjectGuid;
}

void ImGuiSelection::SetSelectedObject(const ONEngine::Guid& _entityGuid, SelectionType _type) {
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
