// property grid
//
#pragma once

#include <Core/Editor/EditableProperties.h>

namespace QtSupport
{
	namespace Properties
	{
		void Setup_Property_Editor_System();
		void Close_Property_Editor_System();

		// for editing properties in a property grid
		QWidget* CreatePropertyEditor( AEditableProperty* theProperty, QWidget* parent );
		//virtual QWidget* GetPropertyEditor( AEditableProperty* theProperty );

	}//namespace Properties

	struct Init_Property_Editor_System
	{
		Init_Property_Editor_System()
		{
			Properties::Setup_Property_Editor_System();
		}
		~Init_Property_Editor_System()
		{
			Properties::Close_Property_Editor_System();
		}
	};



	/*
	-----------------------------------------------------------------------------
		Property_Editor_Grid
	-----------------------------------------------------------------------------
	*/
	class Property_Editor_Grid : public QFrame
		//, DependsOnGlobal< Property_Editor_System >
	{
		Q_OBJECT

	public:
		typedef QFrame Super;

		Property_Editor_Grid( QWidget* parent = nil );
		~Property_Editor_Grid();


		void SetObject( AEditable* theObject );
		AEditable* GetObject();


		void AddWidget( QWidget* newWidget );

		void RemoveWidget( QWidget* theWidget );

	private:
		TPtr< QToolBox >	m_toolBox;
		TPtr< QVBoxLayout >	m_layout;
		QSpacerItem *		m_spacer;

		Init_Property_Editor_System		setupPropsFactory;

		EdPropertyList		m_properties;
		TPtr< AEditable >		m_currentObject;
	};


	/*
	-----------------------------------------------------------------------------
		Property_Editor_Dock_Widget
	-----------------------------------------------------------------------------
	*/
	class Property_Editor_Dock_Widget : public QDockWidget
	{
	public:
		typedef QDockWidget Super;

		Property_Editor_Dock_Widget( QWidget* parent = nil );
		~Property_Editor_Dock_Widget();

		void SetObject( AEditable* theObject );
		AEditable* GetObject();

	private:
		Property_Editor_Grid		m_propsEditorGrid;
	};


}//namespace QtSupport

