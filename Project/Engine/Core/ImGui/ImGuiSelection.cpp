#include "ImGuiSelection.h"

/// engine
#include "Engine/Asset/Collection/AssetCollection.h"

namespace {

	const Guid* gSelectionObjectGuid = &Guid::kInvalid;
	SelectionType gSelectionType = SelectionType::None;

} /// namespace

 
const Guid& ImGuiSelection::GetSelectedObject() {
	return *gSelectionObjectGuid;
}

void ImGuiSelection::SetSelectedEntity(const Guid& _entityGuid, SelectionType _type) {
	gSelectionObjectGuid = &_entityGuid;
	gSelectionType = _type;
}

SelectionType ImGuiSelection::GetSelectionType() {
	return gSelectionType;
}
