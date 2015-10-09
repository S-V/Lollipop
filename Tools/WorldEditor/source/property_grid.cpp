#include <stdafx.h>
#pragma hdrstop

#include <Core/Resources.h>

#include <QtSupport/qt_common.h>
#include "QtSupport/DragAndDrop.h"

#include "editor_common.h"
#include "editor_system.h"
#include "widgets/tree_views.h"
#include "property_grid.h"

/*
-----------------------------------------------------------------------------
	EdPropertyPlugin
-----------------------------------------------------------------------------
*/
mxDEFINE_ABSTRACT_CLASS(EdPropertyPlugin)

EdPropertyPlugin::EdPropertyPlugin()
{
}

QString EdPropertyPlugin::GetPropertyName( AEditableProperty* theProperty )
{
	return theProperty->edGetName();
}

QWidget* EdPropertyPlugin::GetPropertyEditor( AEditableProperty* theProperty )
{
	//APropertyEditor* pPropertyEditor = theProperty->editor;
	//return checked_cast< QWidget*>( pPropertyEditor );
	UNDONE;
	return nil;
}

/*
-----------------------------------------------------------------------------
	EdPropertySystem
-----------------------------------------------------------------------------
*/
EdPropertySystem::EdPropertySystem()
{

}

EdPropertySystem::~EdPropertySystem()
{

}

QString EdPropertySystem::GetPropertyName( AEditableProperty* theProperty )
{
	EdPropertyPlugin* pPropertyPlugin = this->FindPropertyPlugin(theProperty);
	AssertPtr(pPropertyPlugin);
	if( pPropertyPlugin != nil ) {
		return pPropertyPlugin->GetPropertyName(theProperty);
	}
	return "NoName";
}

EdPropertyPlugin* EdPropertySystem::FindPropertyPlugin( AEditableProperty* aProperty )
{
	AssertPtr(aProperty);
	VRET_NIL_IF_NOT(aProperty != nil);

	const TypeGUID propertyClassId = aProperty->rttiGetTypeGuid();
	EdPropertyPlugin* pPropertyPlugin = m_propertyFactoriesByClassGuid.FindRef( propertyClassId );
	return pPropertyPlugin;
}



/*
-----------------------------------------------------------------------------
	EdBasePropertyEditor
-----------------------------------------------------------------------------
*/
EdBasePropertyEditor::EdBasePropertyEditor()
	: APropertyEditor( nil )
{
	UNDONE;
}
EdBasePropertyEditor::~EdBasePropertyEditor()
{

}











inline QColor FColor_to_QColor( const FColor& theColor )
{
	FColor	clampedColor;

	// avoid QT warning: QColor::fromRgbF: RGB parameters out of range
	clampedColor.R = clampf( theColor.R, 0.0001f, 0.999f );
	clampedColor.G = clampf( theColor.G, 0.0001f, 0.999f );
	clampedColor.B = clampf( theColor.B, 0.0001f, 0.999f );
	clampedColor.A = clampf( theColor.A, 0.0001f, 0.999f );

	return QColor::fromRgbF( clampedColor.R, clampedColor.G, clampedColor.B, clampedColor.A );
}
inline FColor QColor_to_FColor( const QColor& theColor )
{
	qreal r,g,b,a;
	theColor.getRgbF( &r, &g, &b, &a );
	return FColor(r,g,b,a);
}
inline QVariant ResourceIdToVariant( const UINT i )
{
	char	buf[32];
	mxSPrintfAnsi( buf, NUMBER_OF(buf), "%Xh", i );
	return QVariant(QString(buf));
}
inline QVariant ResourceIdToVariant( const ObjectGUID& resourceId )
{
	char	buf[32];
	mxSPrintfAnsi( buf, NUMBER_OF(buf), "%Xh", resourceId.v );
	return QVariant(QString(buf));
}
inline QVariant VoidPointerIdToVariant( const void* ptr )
{
	char	buf[32];
	mxSPrintfAnsi( buf, NUMBER_OF(buf), "%Xh", ptr );
	return QVariant(QString(buf));
}
inline QVariant ColorToVariant( const FColor& colorRGBA )
{
	//QColor	color;
	//color.setRgbF( colorRGBA.R, colorRGBA.G, colorRGBA.B, colorRGBA.A );
	//QVariant::Color
//	QVariant	variant( QVariant::Color );
//	variant.setValue( color );
//	return variant;
	return QVariant(FColor_to_QColor(colorRGBA));
}

namespace Properties
{
	QString GetPropertyName( AEditableProperty* theProperty )
	{
		return QString(theProperty->edToChars(0));
	}
	QVariant GetPropertyValue( AEditableProperty* theProperty )
	{
		if (theProperty->Is<EdProperty_Bool>())
		{
			EdProperty_Bool* pProperty = static_cast< EdProperty_Bool* >( theProperty );
			return QVariant(pProperty->Get());
		}
		else if (theProperty->Is<EdProperty_UInt32>())
		{
			EdProperty_UInt32* pProperty = static_cast< EdProperty_UInt32* >( theProperty );
			return QVariant(pProperty->Get());
		}
		else if (theProperty->Is<EdProperty_Float>())
		{
			EdProperty_Float* pProperty = static_cast< EdProperty_Float* >( theProperty );
			return QVariant(pProperty->Get());
		}
		else if (theProperty->Is<EdProperty_Vector3D>())
		{
			EdProperty_Vector3D* pProperty = static_cast< EdProperty_Vector3D* >( theProperty );
			return ToVariant(pProperty->Get());
		}
		else if (theProperty->Is<EdProperty_Quat>())
		{
			EdProperty_Quat* pProperty = static_cast< EdProperty_Quat* >( theProperty );
			return ToVariant(pProperty->Get());
		}
		else if (theProperty->Is<EdProperty_Array>())
		{
			EdProperty_Array* pPropertyArray = static_cast< EdProperty_Array* >( theProperty );
			mxUNUSED(pPropertyArray);
			return QVariant();
		}
		else if (theProperty->Is<EdProperty_AssetReference>())
		{
			EdProperty_AssetReference* pProperty = static_cast< EdProperty_AssetReference* >( theProperty );
			const SResPtrBase& pResHandle = pProperty->GetHandle();
			return QVariant(pResHandle.GetPath().ToChars());
		}
		else if (theProperty->Is<EdProperty_String>())
		{
			EdProperty_String* pProperty = static_cast< EdProperty_String* >( theProperty );
			return ToVariant(pProperty->Get());
		}
		else if (theProperty->Is<EdProperty_ClassGuid>())
		{
			EdProperty_ClassGuid* pProperty = static_cast< EdProperty_ClassGuid* >( theProperty );
			return ToVariant(pProperty->rttiGetTypeName());
		}
		else if (theProperty->Is<EdProperty_FileTime>())
		{
			EdProperty_FileTime* pProperty = static_cast< EdProperty_FileTime* >( theProperty );
			return ToVariant(pProperty->Get());
		}
		else if (theProperty->Is<Property_32BitHexId>())
		{
			Property_32BitHexId* pProperty = static_cast< Property_32BitHexId* >( theProperty );
			return ResourceIdToVariant(pProperty->Get());
		}
		else if (theProperty->Is<EdProperty_ColorRGBA>())
		{
			EdProperty_ColorRGBA* pProperty = static_cast< EdProperty_ColorRGBA* >( theProperty );
			return ColorToVariant(pProperty->Get());
		}
		else if (theProperty->Is<EdProperty_VoidPointer>())
		{
			EdProperty_VoidPointer* pProperty = static_cast< EdProperty_VoidPointer* >( theProperty );
			return VoidPointerIdToVariant(pProperty->Get());
		}
		else if (theProperty->Is<EdProperty_Enum>())
		{
			EdProperty_Enum* pProperty = static_cast< EdProperty_Enum* >( theProperty );
			return pProperty->GetCurrEnumItem().name;
		}
		else if (theProperty->Is<EdPropertyList>())
		{
			EdPropertyList* pProperty = static_cast< EdPropertyList* >( theProperty );
			return QVariant("");
		}
		else
		{
			mxDBG_UNREACHABLE_X("Unknown property '%s' of type '%s'\n",theProperty->edGetName(),theProperty->rttiGetTypeName());
			return QVariant(theProperty->edGetName());
		}
	}



	QWidget* CreatePropertyEditor( AEditableProperty* theProperty, QWidget* parent )
	{
		if (theProperty->Is<EdProperty_Bool>())
		{
			EdProperty_Bool* pProperty = static_cast< EdProperty_Bool* >( theProperty );
			BoolPropertyEditor* pEditor = new BoolPropertyEditor( pProperty, parent );
			return pEditor;
		}
		else if (theProperty->Is<EdProperty_UInt32>())
		{
			EdProperty_UInt32* pProperty = static_cast< EdProperty_UInt32* >( theProperty );
			UInt32PropertyEditor* pEditor = new UInt32PropertyEditor( pProperty, parent );
			return pEditor;
		}
		else if (theProperty->Is<EdProperty_Float>())
		{
			EdProperty_Float* pProperty = static_cast< EdProperty_Float* >( theProperty );
			FloatPropertyEditor* pEditor = new FloatPropertyEditor( pProperty, parent );
			return pEditor;
		}
		else if (theProperty->Is<EdProperty_Vector3D>())
		{
			EdProperty_Vector3D* pProperty = static_cast< EdProperty_Vector3D* >( theProperty );
			Vec3DPropertyEditor* pEditor = new Vec3DPropertyEditor( pProperty, parent );
			return pEditor;
		}
		else if (theProperty->Is<EdProperty_Quat>())
		{
			EdProperty_Quat* pProperty = static_cast< EdProperty_Quat* >( theProperty );
			QuaternionPropertyEditor* pEditor = new QuaternionPropertyEditor( pProperty, parent );
			return pEditor;
		}
		else if (theProperty->Is<EdProperty_Array>())
		{
			// each item of the array will have a separate editor
			return nil;
		}
		else if (theProperty->Is<EdProperty_AssetReference>())
		{
			EdProperty_AssetReference* pProperty = static_cast< EdProperty_AssetReference* >( theProperty );
			AssetReferencePropertyEditor* pEditor = new AssetReferencePropertyEditor( pProperty, parent );
			return pEditor;
		}
		else if (theProperty->Is<EdProperty_String>())
		{
			EdProperty_String* pProperty = static_cast< EdProperty_String* >( theProperty );
			StringPropertyEditor* pEditor = new StringPropertyEditor( pProperty, parent );
			return pEditor;
		}
		else if (theProperty->Is<EdProperty_ColorRGBA>())
		{
			EdProperty_ColorRGBA* pProperty = static_cast< EdProperty_ColorRGBA* >( theProperty );
			ColorRGBAPropertyEditor* pEditor = new ColorRGBAPropertyEditor( pProperty, parent );
			return pEditor;
		}
		else if (theProperty->Is<EdProperty_Enum>())
		{
			EdProperty_Enum* pProperty = static_cast< EdProperty_Enum* >( theProperty );
			EnumPropertyEditor* pEditor = new EnumPropertyEditor( pProperty, parent );
			return pEditor;
		}
		else
		{
			mxDBG_UNREACHABLE_X("Unknown property '%s' of type '%s'\n",theProperty->edGetName(),theProperty->rttiGetTypeName());
			return nil;
		}
	}
	QWidget* GetPropertyEditor( AEditableProperty* theProperty )
	{
		if (theProperty->Is<EdProperty_Bool>())
		{
			return static_cast< BoolPropertyEditor*>( theProperty->editor.Ptr );
		}
		else if (theProperty->Is<EdProperty_UInt32>())
		{
			return static_cast< UInt32PropertyEditor*>( theProperty->editor.Ptr );
		}
		else if (theProperty->Is<EdProperty_Float>())
		{
			return static_cast< FloatPropertyEditor*>( theProperty->editor.Ptr );
		}
		else if (theProperty->Is<EdProperty_Vector3D>())
		{
			return static_cast< Vec3DPropertyEditor*>( theProperty->editor.Ptr );
		}
		else if (theProperty->Is<EdProperty_Quat>())
		{
			return static_cast< QuaternionPropertyEditor*>( theProperty->editor.Ptr );
		}
		else if (theProperty->Is<EdProperty_Array>())
		{
			// each item of the array will have a separate editor
			return nil;
		}
		else if (theProperty->Is<EdProperty_AssetReference>())
		{
			return static_cast< AssetReferencePropertyEditor*>( theProperty->editor.Ptr );
		}
		else
		{
			APropertyEditor* pEditor = theProperty->editor;
			AssertPtr(pEditor);
			return static_cast<QWidget*>(static_cast<void*>(pEditor));
			/*DBGOUT("Unknown property: '%s'\n",theProperty->edToChars(0));
			Unimplemented;
			return nil;*/
		}
	}
	QWidget* EditorFromIndex( const QModelIndex& index )
	{
		AEditableProperty* node = PropertyEditorModel::PropertyFromIndex(index);
		if( node != nil )
		{
			QWidget* pEditorWidget = Properties::GetPropertyEditor( node );
			return pEditorWidget;
		}
		return nil;
	}
	// reads data from the model and writes it to the editor widget.
	void SetEditorData( QWidget *editor, const AEditableProperty* theProperty )
	{
		theProperty->editor->SetEditorData();
	}
	// reads the contents of the editor, and writes it to the model.
	void SetModelData( QWidget* editor, AEditableProperty *theProperty )
	{
		theProperty->editor->SetModelData();
	}

	QString ComposeObjectName( const AEditable* pObject )
	{
		return QString( pObject->rttiGetClass().GetTypeName() )
			+ " - "
			+ QString( pObject->edToChars(0) );
	}

}//namespace Properties

/*
-----------------------------------------------------------------------------
	BoolPropertyEditor
-----------------------------------------------------------------------------
*/
#if 1
BoolPropertyEditor::BoolPropertyEditor( EdProperty_Bool* theProperty, QWidget* parent )
	: Super( parent )
	, APropertyEditor( theProperty )
{
	m_property = theProperty;
}
// reads data from the model and writes it to the editor widget.
void BoolPropertyEditor::SetEditorData()
{
	this->setChecked(m_property->Get());
}
// reads the contents of the editor, and writes it to the model.
void BoolPropertyEditor::SetModelData()
{
	m_property->Set(this->isChecked());
}
#else
BoolPropertyEditor::BoolPropertyEditor( EdProperty_Bool* theProperty, QWidget* parent )
	: Super( parent )
	, APropertyEditor( theProperty )
{
	m_property = theProperty;

	m_comboBox.setEditable(false);
	m_comboBox.insertItem(ComboItem_True,"true");
	m_comboBox.insertItem(ComboItem_False,"false");

	m_layout.addWidget(&m_comboBox);

	this->setLayout(&m_layout);
}
// reads data from the model and writes it to the editor widget.
void BoolPropertyEditor::SetEditorData()
{
	const bool bValue = m_property->Get();
	m_comboBox.setCurrentIndex(bValue ? ComboItem_True : ComboItem_False);
}
// reads the contents of the editor, and writes it to the model.
void BoolPropertyEditor::SetModelData()
{
	const bool bValue = m_comboBox.currentIndex() == ComboItem_True;
	m_property->Set(bValue);
}
#endif

/*
-----------------------------------------------------------------------------
	FloatPropertyEditor
-----------------------------------------------------------------------------
*/
FloatPropertyEditor::FloatPropertyEditor( EdProperty_Float* theProperty, QWidget* parent )
	: Super( parent )
	, APropertyEditor( theProperty )
{
	m_property = theProperty;
	this->setRange( m_property->GetMinimum(), m_property->GetMaximum() );
	this->setSingleStep( m_property->GetSingleStepSize() );
	this->setDecimals(5);

	connect( this, SIGNAL(valueChanged(double)), this, SLOT(OnValueChanged(double)) );
}
// reads data from the model and writes it to the editor widget.
void FloatPropertyEditor::SetEditorData()
{
	this->setValue(m_property->Get());
}
// reads the contents of the editor, and writes it to the model.
void FloatPropertyEditor::SetModelData()
{
	m_property->Set(this->value());
}
void FloatPropertyEditor::OnValueChanged( double newValue )
{
	m_property->Set(newValue);
}

/*
-----------------------------------------------------------------------------
	Vec3DPropertyEditor
-----------------------------------------------------------------------------
*/
Vec3DPropertyEditor::Vec3DPropertyEditor( EdProperty_Vector3D* theProperty, QWidget* parent )
	: Super( parent )
	, APropertyEditor( theProperty )
{
	m_property = theProperty;

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing( 0 );
	layout->setMargin( 0 );
	layout->setContentsMargins( 0, 0, 0, 0 );

	//layout->addWidget(new QLabel("X:", this));
	xVal = new QDoubleSpinBox(this); layout->addWidget(xVal);
	xVal->setRange( -FLT_MAX, FLT_MAX );

	//layout->addWidget(new QLabel("Y:", this));
	yVal = new QDoubleSpinBox(this); layout->addWidget(yVal);
	yVal->setRange( -FLT_MAX, FLT_MAX );

	//layout->addWidget(new QLabel("Z:", this));
	zVal = new QDoubleSpinBox(this); layout->addWidget(zVal);
	zVal->setRange( -FLT_MAX, FLT_MAX );

	connect(xVal, SIGNAL(valueChanged(double)), this, SLOT(xValChanged(double)));
	connect(yVal, SIGNAL(valueChanged(double)), this, SLOT(yValChanged(double)));
	connect(zVal, SIGNAL(valueChanged(double)), this, SLOT(zValChanged(double)));
}
// reads data from the model and writes it to the editor widget.
void Vec3DPropertyEditor::SetEditorData()
{
	const Vec3D	v = m_property->Get();
	xVal->setValue(v.x);
	yVal->setValue(v.y);
	zVal->setValue(v.z);
}
// reads the contents of the editor, and writes it to the model.
void Vec3DPropertyEditor::SetModelData()
{
	const Vec3D	v(
		xVal->value(),
		yVal->value(),
		zVal->value()
	);
	m_property->Set( v );
}

void Vec3DPropertyEditor::xValChanged(double val)
{
	Vec3D v = m_property->Get();
	v.x = val;
	m_property->Set( v );
}

void Vec3DPropertyEditor::yValChanged(double val)
{
	Vec3D v = m_property->Get();
	v.y = val;
	m_property->Set( v );
}

void Vec3DPropertyEditor::zValChanged(double val)
{
	Vec3D v = m_property->Get();
	v.z = val;
	m_property->Set( v );
}

/*
-----------------------------------------------------------------------------
	AssetReferencePropertyEditor
-----------------------------------------------------------------------------
*/
AssetReferencePropertyEditor::AssetReferencePropertyEditor( EdProperty_AssetReference* theProperty, QWidget* parent )
	: Super( parent )
	, APropertyEditor( theProperty )
{
	m_property = theProperty;

	//m_lineEdit = new QLineEdit();
	//m_button = new QPushButton();

	//QHBoxLayout* horizLayout = new QHBoxLayout;
	//horizLayout->addWidget(m_lineEdit);
	//horizLayout->addWidget(m_button);
	//this->setLayout(horizLayout);
}
// reads data from the model and writes it to the editor widget.
void AssetReferencePropertyEditor::SetEditorData()
{
	this->setText( m_property->GetHandle().GetPath().ToChars() );
}
// reads the contents of the editor, and writes it to the model.
void AssetReferencePropertyEditor::SetModelData()
{
	if( this->text().isEmpty() )
	{
		m_property->SetDefaultInstance();
	}
	else
	{
		const String assetPath( this->text().toAscii().data() );
		const ObjectGUID assetGuid = Resources::AssetPathToGuid( assetPath );
		m_property->Set( assetGuid );
	}
}

void AssetReferencePropertyEditor::dragEnterEvent(QDragEnterEvent* theEvent)
{
	const EdMimeData* myData =
		qobject_cast< const EdMimeData* >( theEvent->mimeData() );

	if( myData != nil )
	{
		theEvent->acceptProposedAction();
	}
	else
	{
		theEvent->ignore();
	}
}

void AssetReferencePropertyEditor::dropEvent(QDropEvent* theEvent)
{
	const EdMimeData* myData =
		qobject_cast< const EdMimeData* >( theEvent->mimeData() );

	if( myData != nil )
	{
		const ObjectGUID assetGuid = F_URL_To_Asset_Guid( myData );
		m_property->Set( assetGuid );

		theEvent->acceptProposedAction();
	}
	theEvent->ignore();
}

void AssetReferencePropertyEditor::mouseDoubleClickEvent( QMouseEvent* theEvent )
{
	Super::mouseDoubleClickEvent( theEvent );

	SResourceObject* pResource = m_property->Get();
	if( pResource == nil ) {
		return;
	}

	AEditable* pEditor = F_Get_Editor_For_Resource( m_property->GetHandle(), m_property->GetAssetType() );
	if( pEditor != nil )
	{
		Util_NewPropertyEditorDockWidget( pEditor, this );
	}
}

/*
-----------------------------------------------------------------------------
	StringPropertyEditor
-----------------------------------------------------------------------------
*/
StringPropertyEditor::StringPropertyEditor( EdProperty_String* theProperty, QWidget* parent )
	: Super( parent )
	, APropertyEditor( theProperty )
{
	m_property = theProperty;
}
// reads data from the model and writes it to the editor widget.
void StringPropertyEditor::SetEditorData()
{
	this->setText( m_property->Get() );
}
// reads the contents of the editor, and writes it to the model.
void StringPropertyEditor::SetModelData()
{
	m_property->Set( this->text().toAscii().data() );
}

/*
-----------------------------------------------------------------------------
	TypeGuidPropertyEditor
-----------------------------------------------------------------------------
*/
TypeGuidPropertyEditor::TypeGuidPropertyEditor( EdProperty_ClassGuid* theProperty, QWidget* parent )
	: Super( parent )
	, APropertyEditor( theProperty )
{
	m_property = theProperty;
}
// reads data from the model and writes it to the editor widget.
void TypeGuidPropertyEditor::SetEditorData()
{
	this->setText( m_property->rttiGetTypeName() );
}
// reads the contents of the editor, and writes it to the model.
void TypeGuidPropertyEditor::SetModelData()
{
	//m_property->Set( this->text().toAscii().data() );
}


/*
-----------------------------------------------------------------------------
	ColorRGBAPropertyEditor
-----------------------------------------------------------------------------
*/
ColorRGBAPropertyEditor::ColorRGBAPropertyEditor( EdProperty_ColorRGBA* theProperty, QWidget* parent )
	: Super( parent )
	, APropertyEditor( theProperty )
{
	m_property = theProperty;


}
void ColorRGBAPropertyEditor::SetEditorData()
{
	const FColor& theColor = m_property->Get();
	QPalette pal = this->palette();
	pal.setColor(this->backgroundRole(), FColor_to_QColor(theColor));
	this->setPalette(pal);
	//this->setText(tr("Change color"));
}
void ColorRGBAPropertyEditor::SetModelData()
{
	//Unimplemented;
	//const FColor newColor = FColor::RED;
	//m_property->Set( newColor );
}
void ColorRGBAPropertyEditor::mousePressEvent( QMouseEvent* theEvent )
{
	//Super::mousePressEvent( theEvent );
	if( theEvent->buttons() & Qt::LeftButton )
	{
		const FColor theColor = m_property->Get();
		const FColor prevColor = theColor;
		const QColor initialColor = FColor_to_QColor(theColor);

#if 0
		QColor color = QColorDialog::getColor(initialColor, this);
		if (color.isValid())
		{
			const FColor newColor = QColor_to_FColor(color);
			m_property->Set( newColor );
		}
#endif
		QColorDialog	colorDialog( initialColor, this );
		connect( &colorDialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(OnColorChanged(QColor)) );
		if( colorDialog.exec() )
		{
			const FColor newColor = QColor_to_FColor( colorDialog.selectedColor() );
			m_property->Set( newColor );
		}
		else
		{
			m_property->Set( prevColor );
		}
	}
}
void ColorRGBAPropertyEditor::OnColorChanged( const QColor& theColor )
{
	const FColor newColor = QColor_to_FColor( theColor );
	m_property->Set( newColor );
}
void ColorRGBAPropertyEditor::paintEvent( QPaintEvent* theEvent )
{
	Super::paintEvent( theEvent );
	//QRect region = theEvent->rect();
	//QPainter painter(this);
	//const FColor& theColor = m_property->Get();
	//QPen pen(FColor_to_QColor(theColor));
	//painter.setPen(pen);
	//painter.fillRect(region,Qt::BrushStyle::SolidPattern);
}

/*
-----------------------------------------------------------------------------
	EnumPropertyEditor
-----------------------------------------------------------------------------
*/
EnumPropertyEditor::EnumPropertyEditor( PropertyClass* theProperty, QWidget* parent )
	: Super( parent )
	, APropertyEditor( theProperty )
{
	m_property = theProperty;

	const mxEnumType& pEnumReflection = m_property->GetEnumReflection();
	for (UINT iEnumItem = 0; iEnumItem < pEnumReflection.m_numMembers; iEnumItem++ )
	{
		const mxEnumType::Member& rEnumItem = pEnumReflection.m_members[ iEnumItem ];
		this->addItem(rEnumItem.name, QVariant(rEnumItem.value));
	}

	connect( this, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_OnCurrentIndexChanged(int)) );
}
// reads data from the model and writes it to the editor widget.
void EnumPropertyEditor::SetEditorData()
{
	const mxEnumType::Member& currItem = m_property->GetCurrEnumItem();
	QVariant	itemData(currItem.value);
	int index = this->findData(itemData);
	Assert(index!=INDEX_NONE);
	this->setCurrentIndex(index);
	//this->setCurrentIndex(currItem.value);
}
// reads the contents of the editor, and writes it to the model.
void EnumPropertyEditor::SetModelData()
{
	QVariant	itemData( this->itemData(this->currentIndex()) );
	UINT	enumVal = itemData.toUInt();
	m_property->Set(enumVal);
}

void EnumPropertyEditor::slot_OnCurrentIndexChanged( int newIndex )
{
	//m_property->Set(newIndex);
	this->SetModelData();
}

/*
-----------------------------------------------------------------------------
	PropertyEditorModel
-----------------------------------------------------------------------------
*/
PropertyEditorModel::PropertyEditorModel()
{
	this->setSupportedDragActions(Qt::ActionMask);
}
//-----------------------------------------------------------------------------
PropertyEditorModel::~PropertyEditorModel()
{
}
//-----------------------------------------------------------------------------
//static
AEditableProperty* PropertyEditorModel::PropertyFromIndex(const QModelIndex& index)
{
	AEditable* node = static_cast< AEditable* >( index.internalPointer() );
	if( node->IsA<AEditableProperty>() )
	{
		return static_cast< AEditableProperty* >( node );
	}
	return nil;
}
//-----------------------------------------------------------------------------
int PropertyEditorModel::columnCount(const QModelIndex &parent) const
{
	return Column_Count;
}
//-----------------------------------------------------------------------------
QVariant PropertyEditorModel::data( const QModelIndex &index, int role ) const
{
	if( !index.isValid() ) {
		return QVariant();
	}

	if( Qt::DecorationRole == role )
	{
		return this->GetIcon(index);
	}
	//if( Qt::CheckStateRole == role )
	//{
	//	return QVariant(true?false);
	//}
	if( Qt::SizeHintRole == role )
	{
		//QWidget* pEditorWidget = Properties::EditorFromIndex(index);
		//if( pEditorWidget != nil )
		//{
		//	return pEditorWidget->sizeHint();
		//}

		//return QSize(0,QFontMetrics(QFont()).lineSpacing()+6);
	}

	const int	rowIndex	= index.row();
	const int	columnIndex = index.column();

	if (role == Qt::DisplayRole)
	{
		AEditableProperty* propertyNode = PropertyFromIndex( index );

		if (propertyNode != nil)
		{
			switch( columnIndex )
			{
			case Column_Name :	return Properties::GetPropertyName( propertyNode );
			case Column_Value :	return Properties::GetPropertyValue( propertyNode );
			default:	Unreachable;
			}
		}
	}
	mxUNUSED(rowIndex);
	//HACK
	if (role == Qt::BackgroundRole || role == Qt::ForegroundRole)
	{
		AEditableProperty* propertyNode = PropertyFromIndex( index );
		EdProperty_ColorRGBA* pColorProperty = SafeCast<EdProperty_ColorRGBA>(propertyNode);
		if( pColorProperty != nil )
		{
			if( columnIndex == Column_Value )
			{
				return FColor_to_QColor(pColorProperty->Get());
			}
		}
	}

	return QVariant();
}
//-----------------------------------------------------------------------------
bool PropertyEditorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	return Super::setData(index, value, role);
}
//-----------------------------------------------------------------------------
QVariant PropertyEditorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch( section )
		{
		case Column_Name :	return QString("Name");
		case Column_Value :	return QString("Value");
		default:	Unreachable;
		}
	}
	return QVariant();
}
//-----------------------------------------------------------------------------
QModelIndex PropertyEditorModel::buddy( const QModelIndex & index ) const 
{
	if (index.isValid() && index.column() == 0)
		return this->createIndex(index.row(), 1, index.internalPointer());
	return index;
}
//-----------------------------------------------------------------------------
Qt::ItemFlags PropertyEditorModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return 0;
	}

	//const bool isReadOnly = false;

	//if (isReadOnly)
	//	return Qt::ItemIsDragEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable
		| Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled 
		;
}
//-----------------------------------------------------------------------------

Qt::DropActions PropertyEditorModel::supportedDropActions() const
{
	return Qt::ActionMask;
}

QMimeData* PropertyEditorModel::mimeData( const QModelIndexList& indexes ) const
{
	//return Super::mimeData( indexes );
	//return ZZ_Template_CreateMimeData( this, indexes );
	return nil;
}

bool PropertyEditorModel::dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent )
{
	if (action == Qt::IgnoreAction) {
		return true;
	}
	return true;
}

#if 0
static QRect GetCheckBoxRect(const QStyleOptionViewItem &view_item_style_options) {
	QStyleOptionButton check_box_style_option;
	QRect check_box_rect = QApplication::style()->subElementRect(
		QStyle::SE_CheckBoxIndicator,
		&check_box_style_option);
	QPoint check_box_point(view_item_style_options.rect.x() +
		view_item_style_options.rect.width() / 2 -
		check_box_rect.width() / 2,
		view_item_style_options.rect.y() +
		view_item_style_options.rect.height() / 2 -
		check_box_rect.height() / 2);
	return QRect(check_box_point, check_box_rect.size());
}
#endif
/*
-----------------------------------------------------------------------------
	PropertyEditorDelegate
-----------------------------------------------------------------------------
*/
PropertyEditorDelegate::PropertyEditorDelegate( QObject *parent )
	: Super( parent )
{
}
//-----------------------------------------------------------------------------
QWidget* PropertyEditorDelegate::createEditor(QWidget* parent,
									   const QStyleOptionViewItem &/* option */,
									   const QModelIndex& index ) const
{
	AEditableProperty* theProperty= PropertyEditorModel::PropertyFromIndex(index);

	if( theProperty != nil && !theProperty->isReadOnly() )
	{
		QWidget* editor = Properties::CreatePropertyEditor( theProperty, parent );
		if( editor != nil )
		{
			editor->setMinimumHeight( editor->sizeHint().height() * 0.8f );
			return editor;
		}
	}

	return nil;
}
//-----------------------------------------------------------------------------
void PropertyEditorDelegate::setEditorData(QWidget *editor,
									const QModelIndex &index) const
{
	AEditableProperty* node = PropertyEditorModel::PropertyFromIndex(index);

	if( node != nil )
	{
		Properties::SetEditorData( editor, node );
	}
}
//-----------------------------------------------------------------------------
void PropertyEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
								   const QModelIndex &index) const
{
	AEditableProperty* node = PropertyEditorModel::PropertyFromIndex(index);

	if( node != nil )
	{
		Properties::SetModelData( editor, node );
	}
}
//-----------------------------------------------------------------------------
void PropertyEditorDelegate::updateEditorGeometry(
	QWidget *editor,
	const QStyleOptionViewItem &option,
	const QModelIndex &/* index */) const
{
	editor->setGeometry( option.rect );
}
//-----------------------------------------------------------------------------
QSize PropertyEditorDelegate::sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	//QWidget* pEditorWidget = this->EditorFromIndex(index);
	//if( pEditorWidget != nil )
	//{
	//	return pEditorWidget->sizeHint();
	//}
	return Super::sizeHint( option, index ) + QSize(2, 2);
}
//-----------------------------------------------------------------------------
void PropertyEditorDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option,
		   const QModelIndex &index ) const
{
	AEditableProperty* theProperty = PropertyEditorModel::PropertyFromIndex(index);
	Assert( theProperty != nil );

#if 0
	QWidget* pEditorWidget = Properties::EditorFromIndex(index);
	if( pEditorWidget != nil )
	{
		BoolPropertyEditor* pBoolEditor = qobject_cast< BoolPropertyEditor* >( pEditorWidget );
		if( pBoolEditor != nil )
		{
			QStyle *style=qApp->style();

			QRect checkBoxRect=style->subElementRect(QStyle::SE_CheckBoxIndicator, &option);
			const int chkWidth=checkBoxRect.width();
			const int chkHeight=checkBoxRect.height();

			const int centerX=option.rect.left() + qMax(option.rect.width()/2-chkWidth/2, 0);
			const int centerY=option.rect.top() + qMax(option.rect.height()/2-chkHeight/2, 0);
			QStyleOptionViewItem modifiedOption(option);
			modifiedOption.rect.moveTo(centerX, centerY);
			modifiedOption.rect.setSize(QSize(chkWidth, chkHeight));
			if(pBoolEditor->isChecked()){
				modifiedOption.state |= QStyle::State_On;
			}

			style->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &modifiedOption, painter);
			return;
		}
	}
#endif

#if 0
	QWidget* pEditorWidget = Properties::EditorFromIndex(index);
	if( pEditorWidget != nil )
	{
		BoolPropertyEditor* pBoolEditor = qobject_cast< BoolPropertyEditor* >( pEditorWidget );
		if( pBoolEditor != nil )
		{
			const bool checked = index.model()->data(index, Qt::DisplayRole).toBool();

			QStyleOptionButton check_box_style_option;
			check_box_style_option.state |= QStyle::State_Enabled;
			if (checked) {
				check_box_style_option.state |= QStyle::State_On;
			} else {
				check_box_style_option.state |= QStyle::State_Off;
			}
			check_box_style_option.rect = GetCheckBoxRect(option);

			QApplication::style()->drawControl(QStyle::CE_CheckBox,
				&check_box_style_option,
				painter);
			return;
		}
	}
#endif

#if 1
	QStyleOptionViewItem newOption(option);

	if( index.column() != PropertyEditorModel::Column_Name )
	{
		if( theProperty->isReadOnly() )
		{
			newOption.font.setWeight(QFont::Light);
		}
		else
		{
			newOption.font.setBold(true);
		}
	}

	const QBrush br = painter->brush();

	painter->save();
	painter->setBrushOrigin(newOption.rect.x(), newOption.rect.y());
	painter->fillRect(newOption.rect, br);
	painter->restore();

	Super::paint(painter, newOption, index);
	const QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &newOption));
	const QPen oldPen = painter->pen();
	painter->setPen(QPen(color));

	painter->drawLine(newOption.rect.right(), newOption.rect.y(),
		newOption.rect.right(), newOption.rect.bottom());
	painter->drawLine(newOption.rect.x(), newOption.rect.bottom(),
		newOption.rect.right(), newOption.rect.bottom());
	painter->setPen(oldPen);
	return;
#endif

	//return Super::paint( painter, option, index );
}

/*
-----------------------------------------------------------------------------
	ProperyEditorWidget
-----------------------------------------------------------------------------
*/
ProperyEditorWidget::ProperyEditorWidget( QWidget* parent )
	: Super( parent )
{
	this->setSelectionMode(QTreeView::SingleSelection);
	this->setSelectionBehavior(QTreeView::SelectRows);

	this->setAutoScroll(true);
	this->setRootIsDecorated( false );
	this->setAlternatingRowColors(true);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	this->setEditTriggers( QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked );
	//this->setEditTriggers( QAbstractItemView::AllEditTriggers );

	this->header()->setResizeMode(QHeaderView::ResizeToContents);


	this->setAcceptDrops(true);
	this->setDragEnabled(true);
	//this->setAutoExpandDelay(1000);	// 1 sec
	//this->setDragDropMode(DragDropMode::DropOnly);
	this->setDragDropMode(DragDropMode::DragDrop);
	this->setDropIndicatorShown(true);


	//this->setUniformRowHeights( true );

	mxCONNECT_THIS( EdSystem::Get().Event_ObjectBeingDestroyed, ProperyEditorWidget, _OnDataBeingDestroyed );

	this->setModel( &m_model );
	this->setItemDelegate( &m_delegate );
}

ProperyEditorWidget::~ProperyEditorWidget()
{
	if ( m_object != nil ) {
		m_object->edSaveChanges();
	}

	mxDISCONNECT_THIS( EdSystem::Get().Event_ObjectBeingDestroyed );
}

//void OpenPersisentEditors( Editable* pRoot, QTreeView* pTreeView, PropertyEditorModel* pModel )
//{
//	for( UINT iRow = 0; iRow < pRoot->edNumRows(); iRow++ )
//	{
//		Editable* pChild = pRoot->edItemAt( iRow );
//		const QModelIndex modelIndex = pModel->IndexFromNode( pChild );
//		pTreeView->openPersistentEditor( modelIndex );
//
//		OpenPersisentEditors( pChild, pTreeView, pModel );
//	}
//}

void ProperyEditorWidget::SetObject( AEditable* pObject )
{
	mxDBG_CHECK_VTBL( pObject );

	if ( m_object == pObject ) {
		return;
	}

	if ( m_object != nil ) {
		m_object->edSaveChanges();
	}

	m_object = pObject;

	if ( pObject != nil )
	{
		pObject->edCreateProperties( &m_properties );
		m_properties.SetParent( pObject );

		m_model.SetRootObject( &m_properties );

		this->setModel( &m_model );
		this->setItemDelegate( &m_delegate );
		//OpenPersisentEditors( m_object, this, &m_model );

		for( UINT iColumn = 0; iColumn < PropertyEditorModel::Column_Count; iColumn++ )
		{
			this->resizeColumnToContents(iColumn);
		}

		//this->expandAll();
		this->expandToDepth(1);
	}
	else
	{
		//m_properties.Clear();
		m_properties.SetParent( nil );

		m_model.SetRootObject( nil );
	}
}

void ProperyEditorWidget::reset()
{
	//if (m_object != nil)
	//{
	//	m_object->edCreateProperties( m_properties );
	//}
	Super::reset();
}

void ProperyEditorWidget::OnProjectUnloaded()
{
	this->SetObject(nil);
}

void ProperyEditorWidget::_OnDataBeingDestroyed( AEditable* pObject )
{
	if( m_object == pObject )
	{
		this->SetObject(nil);
	}
}

void ProperyEditorWidget::contextMenuEvent( QContextMenuEvent* theEvent )
{
	Super::contextMenuEvent( theEvent );
}

void ProperyEditorWidget::dragEnterEvent( QDragEnterEvent* theEvent )
{
	theEvent->acceptProposedAction();
}

void ProperyEditorWidget::dragLeaveEvent( QDragLeaveEvent* theEvent )
{
	Super::dragLeaveEvent( theEvent );
}

void ProperyEditorWidget::dragMoveEvent( QDragMoveEvent* theEvent )
{
	//Super::dragMoveEvent( theEvent );
	theEvent->accept();
}

void ProperyEditorWidget::dropEvent( QDropEvent* theEvent )
{
	const QModelIndex modelIndex = this->indexAt( theEvent->pos() );
	if( modelIndex.isValid() )
	{
		AEditable* theObject = static_cast< AEditable* >( modelIndex.internalPointer() );
		if( theObject != nil )
		{
			EdAssetGuidContainer	args;
			args.assGuid = F_URL_To_Asset_Guid( theEvent->mimeData() );

			if( args.assGuid.IsValid() )
			{
				theObject->edOnItemDropped( &args );
				theEvent->acceptProposedAction();
				return;
			}
		}
	}

	theEvent->ignore();
}

/*
-----------------------------------------------------------------------------
	ProperyEditorDockWidget
-----------------------------------------------------------------------------
*/
ProperyEditorDockWidget::ProperyEditorDockWidget( QWidget* parent )
	: Super( parent )
{
	this->setAcceptDrops(true);
	this->setObjectName(QString("DockWidget-%1").arg(m__serialNumber));
	this->setWidget( &m_editorWidget );

	EditorApp::ValidOnlyWhenProjectIsLoaded( this );
}

ProperyEditorDockWidget::~ProperyEditorDockWidget()
{
}

void ProperyEditorDockWidget::SetObject( AEditable* pObject )
{
	mxDBG_CHECK_VTBL( pObject );

	m_editorWidget.SetObject( pObject );

	if( pObject != nil )
	{
		this->setWindowTitle( Properties::ComposeObjectName( pObject ) );
	}
	else
	{
		this->setWindowTitle( "Properties" );
	}
}

void ProperyEditorDockWidget::RefreshView()
{
	m_editorWidget.reset();
}

void ProperyEditorDockWidget::OnProjectUnloaded()
{
	this->close();
}

void Util_CenterWidget(QWidget *widget, QWidget *relativeTo, bool useSizeHint )
{
	QSize size;
	if(useSizeHint) {
		size = widget->sizeHint();
	} else {
		size = widget->size();
	}

	int w = relativeTo->width();   // returns screen width
	int h = relativeTo->height();  // returns screen height
	int mw = size.width();
	int mh = size.height();
	int cw = (w/2) - (mw/2);
	int ch = (h/2) - (mh/2);
	widget->move(cw,ch);
}

void Util_CenterWidget(QWidget *widget, bool useSizeHint )
{
	QDesktopWidget *d = QApplication::desktop();
	Util_CenterWidget( widget, d, true );
}

void Util_NewPropertyEditorDockWidget( AEditable* pObject, QWidget* parent )
{
	CHK_VRET_IF_NIL(pObject);

	ProperyEditorDockWidget* newPropsEditor = new ProperyEditorDockWidget( parent );
	newPropsEditor->SetObject( pObject );

	EditorApp::GetMainFrame()->addDockWidget(Qt::LeftDockWidgetArea,newPropsEditor,Qt::Horizontal);
	newPropsEditor->setFloating(true);
	newPropsEditor->show();

	Util_CenterWidget( newPropsEditor, EditorApp::GetMainFrame() );

	//@todo: don't hardcode it:
	newPropsEditor->resize(300,400);
}

