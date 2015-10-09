#include "stdafx.h"

#include <QtSupport/property_editor.h>

namespace QtSupport
{

	namespace Properties
	{

		typedef QWidget* F_PropertyEditorWidgetFactory();

		template< typename PROPERTY_KLASS >
		QWidget* F_CreatePropertyEditorWidget_Template()
		{
			UNDONE;
		}

		namespace
		{
			struct Property_Editor_System_Data
			{
				//m_propertyFactoriesByClassGuid
				TMap< TypeGUID, F_PropertyEditorWidgetFactory* >		m_factory;

			public:
				Property_Editor_System_Data()
				{
					//
				}
			};
			static TPtr< Property_Editor_System_Data >	gData;

			static NiftyCounter gInitCounter;

		}//namespace

		void Setup_Property_Editor_System()
		{
			if( gInitCounter.IncRef() )
			{
				gData.ConstructInPlace();
			}
		}
		void Close_Property_Editor_System()
		{
			if( gInitCounter.DecRef() )
			{
				gData.Destruct();
			}
		}

	}//namespace Properties

	/*
	-----------------------------------------------------------------------------
		Property_Editor_Grid
	-----------------------------------------------------------------------------
	*/
	Property_Editor_Grid::Property_Editor_Grid( QWidget* parent )
		: Super( parent )
	{
		m_layout = new QVBoxLayout(this);
		m_layout->setSpacing(0);
		m_layout->setMargin(0);

		this->setFrameStyle(QFrame::StyledPanel);

		const QString propertyName = "Property_Editor_Grid : Test Property";

		if( !propertyName.isEmpty() )
		{
			QLabel* label = new QLabel( propertyName, this );
			label->setPalette(QPalette(qRgb(191, 191, 191)));
			label->setAutoFillBackground(true);

			label->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
			m_layout->addWidget(label);
		}

		m_spacer = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Expanding );
		m_layout->addSpacerItem( m_spacer );
	}

	Property_Editor_Grid::~Property_Editor_Grid()
	{
	}

	void Property_Editor_Grid::SetObject( AEditable* theObject )
	{
		mxDBG_CHECK_VTBL( theObject );

		if ( m_currentObject == theObject ) {
			return;
		}

		m_properties.Clear();

		m_currentObject = theObject;

		if( theObject != nil )
		{
			theObject->edCreateProperties( &m_properties );
		}
		else
		{
			//
		}
	}

	AEditable* Property_Editor_Grid::GetObject()
	{
		return m_currentObject.Ptr;
	}

	void Property_Editor_Grid::AddWidget( QWidget* newWidget )
	{
		newWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
		m_toolBox->addItem( newWidget, "???" );
	}

	void Property_Editor_Grid::RemoveWidget( QWidget* theWidget )
	{

	}

	/*
	-----------------------------------------------------------------------------
		Property_Editor_Dock_Widget
	-----------------------------------------------------------------------------
	*/
	Property_Editor_Dock_Widget::Property_Editor_Dock_Widget( QWidget* parent )
		: Super( parent )
	{
		this->setWidget( &m_propsEditorGrid );
	}

	Property_Editor_Dock_Widget::~Property_Editor_Dock_Widget()
	{

	}

	void Property_Editor_Dock_Widget::SetObject( AEditable* theObject )
	{
		m_propsEditorGrid.SetObject( theObject );
	}

	AEditable* Property_Editor_Dock_Widget::GetObject()
	{
		return m_propsEditorGrid.GetObject();
	}

}//namespace QtSupport

NO_EMPTY_FILE
