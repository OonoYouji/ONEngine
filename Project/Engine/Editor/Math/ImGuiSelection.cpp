#include "ImGuiSelection.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"

using namespace Editor;

namespace {

	/// ----- ImGuiSelection ----- ///
	std::unordered_set<ONEngine::Guid> gSelectedObjects;
	ONEngine::Guid gLastSelectedObject = ONEngine::Guid::kInvalid;
	SelectionType gSelectionType = SelectionType::None;

	/// ----- ImGuiInfo ----- ///
	std::string gInfo;

} /// namespace

 
const std::unordered_set<ONEngine::Guid>& ImGuiSelection::GetSelectedObjects() {
	return gSelectedObjects;
}

const ONEngine::Guid& ImGuiSelection::GetLastSelectedObject() {
	return gLastSelectedObject;
}

void ImGuiSelection::SetSelectedObject(const ONEngine::Guid& _guid, SelectionType _type) {
	gSelectedObjects.clear();
	gSelectedObjects.insert(_guid);
	gLastSelectedObject = _guid;
	gSelectionType = _type;
}

void ImGuiSelection::AddSelectedObject(const ONEngine::Guid& _guid, SelectionType _type) {
	gSelectedObjects.insert(_guid);
	gLastSelectedObject = _guid;
	gSelectionType = _type;
}

void ImGuiSelection::RemoveSelectedObject(const ONEngine::Guid& _guid) {
	gSelectedObjects.erase(_guid);
	if (gLastSelectedObject == _guid) {
		gLastSelectedObject = ONEngine::Guid::kInvalid;
	}
	if (gSelectedObjects.empty()) {
		gSelectionType = SelectionType::None;
	}
}

void ImGuiSelection::ClearSelection() {
	gSelectedObjects.clear();
	gLastSelectedObject = ONEngine::Guid::kInvalid;
	gSelectionType = SelectionType::None;
}

bool ImGuiSelection::IsSelected(const ONEngine::Guid& _guid) {
	return gSelectedObjects.contains(_guid);
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
