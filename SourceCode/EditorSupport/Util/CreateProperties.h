#pragma once

#include <Core/Editor/EditableProperties.h>

#include <Renderer/Scene/Model.h>

namespace Properties
{
	void For_rxAABB( rxAABB & rAABB, const char* name, EdPropertyList *properties );
	void For_rxModel( rxModel & rModel, const char* name, EdPropertyList *properties );
	void For_rxModel_Batch( rxModelBatch & modelBatch, const char* name, EdPropertyList *properties );

}//namespace Properties
