#pragma once

#include <Core/Editor/EditableProperties.h>

#include "widgets/tree_views.h"


/*
-----------------------------------------------------------------------------
	EdPropertyPlugin

	is used to generate a simple editing interface for a property
-----------------------------------------------------------------------------
*/
struct EdPropertyPlugin : public AEditableRefCounted
{
	mxDECLARE_ABSTRACT_CLASS(EdPropertyPlugin,AEditableRefCounted);
	typedef TRefPtr<EdPropertyPlugin>	Ref;

	// for displaying properties in tree views
	virtual QString GetPropertyName( AEditableProperty* theProperty );
	virtual QVariant GetPropertyValue( AEditableProperty* theProperty ) = 0;

	// for editing properties in a property grid
	virtual QWidget* CreatePropertyEditor( AEditableProperty* theProperty, QWidget* parent ) = 0;
	virtual QWidget* GetPropertyEditor( AEditableProperty* theProperty );

protected:
	EdPropertyPlugin();
};

/*
-----------------------------------------------------------------------------
	EdPropertySystem
-----------------------------------------------------------------------------
*/
class EdPropertySystem : public TGlobal<EdPropertySystem>
{
	TMap< TypeGUID, EdPropertyPlugin::Ref >		m_propertyFactoriesByClassGuid;

public:
	EdPropertySystem();
	~EdPropertySystem();

	QString GetPropertyName( AEditableProperty* theProperty );

	EdPropertyPlugin* FindPropertyPlugin( AEditableProperty* aProperty );
};



/*
-----------------------------------------------------------------------------
	EdBasePropertyEditor
-----------------------------------------------------------------------------
*/
class EdBasePropertyEditor : public APropertyEditor
{
protected:
	EdBasePropertyEditor();
	~EdBasePropertyEditor();
};




















class EdProperty_String;
class EdProperty_ClassGuid;
class EdProperty_FileTime;
class Property_32BitHexId;





namespace Properties
{
	QString GetPropertyName( AEditableProperty* theProperty );
	QVariant GetPropertyValue( AEditableProperty* theProperty );

	QWidget* CreatePropertyEditor( AEditableProperty* theProperty, QWidget* parent );
	QWidget* GetPropertyEditor( AEditableProperty* theProperty );

	// reads data from the model and writes it to the editor widget.
	void SetEditorData( QWidget *editor, const AEditableProperty* theProperty );

	// reads the contents of the editor, and writes it to the model.
	void SetModelData( QWidget* editor, AEditableProperty *theProperty );

	QString ComposeObjectName( const AEditable* pObject );

}//namespace Properties


/*
-----------------------------------------------------------------------------
	BoolPropertyEditor
-----------------------------------------------------------------------------
*/
#if 1
class BoolPropertyEditor : public QCheckBox, public APropertyEditor
{
	Q_OBJECT

public:
	typedef QCheckBox Super;

	BoolPropertyEditor( EdProperty_Bool* theProperty, QWidget* parent = nil );
	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData();
	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData();

private:
	TPtr<EdProperty_Bool>	m_property;
};
#else
class BoolPropertyEditor : public QWidget, public APropertyEditor
{
	Q_OBJECT

public:
	typedef QWidget Super;

	BoolPropertyEditor( EdProperty_Bool* theProperty, QWidget* parent = nil );
	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData();
	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData();

private:
	enum {
		ComboItem_True = 0,
		ComboItem_False = 1,
	};
	QComboBox	m_comboBox;
	QHBoxLayout	m_layout;

	TPtr<EdProperty_Bool>	m_property;
};
#endif



/*
-----------------------------------------------------------------------------
	UInt32PropertyEditor
-----------------------------------------------------------------------------
*/
class UInt32PropertyEditor : public QSpinBox, public APropertyEditor
{
	TPtr<EdProperty_UInt32>	m_property;

public:
	typedef QSpinBox Super;

	UInt32PropertyEditor( EdProperty_UInt32* theProperty, QWidget* parent = nil )
		: Super( parent )
		, APropertyEditor( theProperty )
	{
		m_property = theProperty;
		this->setMinimum(m_property->GetMinimum());
		this->setMaximum(m_property->GetMaximum());
	}
	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData()
	{
		this->setValue((int)m_property->Get());
	}
	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData()
	{
		m_property->Set((U4)this->value());
	}
};

/*
-----------------------------------------------------------------------------
	FloatPropertyEditor
-----------------------------------------------------------------------------
*/
class FloatPropertyEditor : public QDoubleSpinBox, public APropertyEditor
{
	Q_OBJECT

public:
	typedef QDoubleSpinBox Super;

	FloatPropertyEditor( EdProperty_Float* theProperty, QWidget* parent = nil );

	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData();
	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData();

private slots:
	void OnValueChanged( double newValue );

private:
	TPtr<EdProperty_Float>	m_property;
};

template< typename TYPE >
inline QVariant ToVariant( const TYPE& theValue )
{
	return QVariant(theValue);
}
template<>
inline QVariant ToVariant<Vec3D>( const Vec3D& v )
{
	return QVariant(QString("%1, %2, %3").arg(v.x).arg(v.y).arg(v.z));
}
template<>
inline QVariant ToVariant<Quat>( const Quat& q )
{
	return QVariant(QString("%1, %2, %3, %4").arg(q.x).arg(q.y).arg(q.z).arg(q.w));
}
template<>
inline QVariant ToVariant<FileTime>( const FileTime& fileTime )
{
	char	buf[32];
	FS_Win32_FileTimeStampToDateTimeString( fileTime.time, buf, NUMBER_OF(buf) );
	return QVariant(QString(buf));
}


/*
-----------------------------------------------------------------------------
	Vec3DPropertyEditor
-----------------------------------------------------------------------------
*/
class Vec3DPropertyEditor : public QFrame, public APropertyEditor
{
	Q_OBJECT

public:
	typedef QFrame Super;

	Vec3DPropertyEditor( EdProperty_Vector3D* theProperty, QWidget* parent = nil );

	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData();
	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData();

private slots:
	void xValChanged(double val);
	void yValChanged(double val);
	void zValChanged(double val);

private:
	TPtr< EdProperty_Vector3D >	m_property;

	QDoubleSpinBox* xVal;
	QDoubleSpinBox* yVal;
	QDoubleSpinBox* zVal;
};

/*
-----------------------------------------------------------------------------
	QuaternionPropertyEditor
-----------------------------------------------------------------------------
*/
class QuaternionPropertyEditor : public QLabel, public APropertyEditor
{
	TPtr<EdProperty_Quat>	m_property;

public:
	typedef QLabel Super;

	QuaternionPropertyEditor( EdProperty_Quat* theProperty, QWidget* parent = nil )
		: Super( parent )
		, APropertyEditor( theProperty )
	{
		m_property = theProperty;
	}
	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData()
	{
		const Quat	q = m_property->Get();
		this->setText(QString("%1, %2, %3, %4").arg(q.x).arg(q.y).arg(q.z).arg(q.w));
	}
	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData()
	{
		//m_property->Set(this->value());
	}
};

/*
-----------------------------------------------------------------------------
	AssetReferencePropertyEditor
-----------------------------------------------------------------------------
*/
class AssetReferencePropertyEditor : public QLineEdit, public APropertyEditor
{
	TPtr<EdProperty_AssetReference>	m_property;

	//TPtr<QLineEdit>		m_lineEdit;
	//TPtr<QPushButton>	m_button;

public:
	typedef QLineEdit Super;

	AssetReferencePropertyEditor( EdProperty_AssetReference* theProperty, QWidget* parent = nil );
	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData();
	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData();

private:
	virtual void mouseDoubleClickEvent( QMouseEvent* theEvent ) override;

protected:
	virtual void dragEnterEvent(QDragEnterEvent* theEvent);
	virtual void dropEvent(QDropEvent* theEvent);
};

/*
-----------------------------------------------------------------------------
	StringPropertyEditor
-----------------------------------------------------------------------------
*/
class StringPropertyEditor : public QLineEdit, public APropertyEditor
{
	TPtr<EdProperty_String>	m_property;

public:
	typedef QLineEdit Super;

	StringPropertyEditor( EdProperty_String* theProperty, QWidget* parent = nil );
	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData();
	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData();
};


/*
-----------------------------------------------------------------------------
	TypeGuidPropertyEditor
-----------------------------------------------------------------------------
*/
class TypeGuidPropertyEditor : public QLineEdit, public APropertyEditor
{
	TPtr<EdProperty_ClassGuid>	m_property;

public:
	typedef QLineEdit Super;

	TypeGuidPropertyEditor( EdProperty_ClassGuid* theProperty, QWidget* parent = nil );
	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData();
	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData();
};

/*
-----------------------------------------------------------------------------
	FileTimePropertyEditor
-----------------------------------------------------------------------------
*/
class FileTimePropertyEditor : public QLineEdit, public APropertyEditor
{
	TPtr<EdProperty_FileTime>	m_property;

public:
	typedef QLineEdit Super;

	FileTimePropertyEditor( EdProperty_FileTime* theProperty, QWidget* parent = nil );
	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData();
	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData();
};

/*
-----------------------------------------------------------------------------
	ColorRGBAPropertyEditor
-----------------------------------------------------------------------------
*/
class ColorRGBAPropertyEditor : public QWidget, public APropertyEditor
{
	Q_OBJECT

public:
	typedef QWidget Super;

	ColorRGBAPropertyEditor( EdProperty_ColorRGBA* theProperty, QWidget* parent = nil );
	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData();
	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData();

protected:
	virtual void mousePressEvent( QMouseEvent* theEvent ) override;
	virtual void paintEvent( QPaintEvent* theEvent ) override;

private slots:
	void OnColorChanged( const QColor& theColor );

private:
	TPtr<EdProperty_ColorRGBA>	m_property;
};




/*
-----------------------------------------------------------------------------
	EnumPropertyEditor
-----------------------------------------------------------------------------
*/
class EnumPropertyEditor : public QComboBox, public APropertyEditor
{
	Q_OBJECT

public:
	typedef QComboBox Super;
	typedef EdProperty_Enum PropertyClass;

	EnumPropertyEditor( PropertyClass* theProperty, QWidget* parent = nil );
	// reads data from the model and writes it to the editor widget.
	virtual void SetEditorData();
	// reads the contents of the editor, and writes it to the model.
	virtual void SetModelData();

private slots:
	void slot_OnCurrentIndexChanged( int newIndex );

private:
	TPtr<PropertyClass>	m_property;
};









/*
-----------------------------------------------------------------------------
	PropertyEditorModel
-----------------------------------------------------------------------------
*/
class PropertyEditorModel : public EdTreeViewModel
{
public:
	typedef EdTreeViewModel Super;

	PropertyEditorModel();
	~PropertyEditorModel();

	enum EColumns
	{
		Column_Name = 0,
		Column_Value = 1,
		Column_Count
	};

	static AEditableProperty* PropertyFromIndex(const QModelIndex& index);

public: // Overrides

	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual QModelIndex buddy( const QModelIndex & index ) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;

	virtual Qt::DropActions supportedDropActions() const override;
	virtual QMimeData* mimeData( const QModelIndexList& indexes ) const override;
	virtual bool dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) override;
};

/*
-----------------------------------------------------------------------------
	PropertyEditorDelegate
-----------------------------------------------------------------------------
*/
class PropertyEditorDelegate : public QStyledItemDelegate
{ 
	Q_OBJECT

public:
	typedef QStyledItemDelegate Super;

	PropertyEditorDelegate( QObject *parent = nil );

	// The createEditor() function returns an editor widget.
	virtual QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem &option,
		const QModelIndex &index ) const;

	// The setEditorData() function reads data from the model and writes it to the editor widget.
	virtual void setEditorData( QWidget *editor, const QModelIndex &index ) const;

	// The setModelData() function reads the contents of the editor, and writes it to the model.
	virtual void setModelData( QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index ) const;

	// The updateEditorGeometry() function updates the editor widget's geometry using the information supplied in the style option.
	virtual void updateEditorGeometry( QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	virtual QSize sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

	virtual void paint( QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index ) const;
};

/*
-----------------------------------------------------------------------------
	ProperyEditorWidget
-----------------------------------------------------------------------------
*/
class ProperyEditorWidget : public QTreeView, public EdProjectChild
{
public:
	typedef QTreeView Super;

	ProperyEditorWidget( QWidget* parent = nil );
	~ProperyEditorWidget();

	void SetObject( AEditable* pObject );

public:
	virtual void reset() override;

protected:
	virtual void OnProjectUnloaded() override;

protected:
	virtual void contextMenuEvent(QContextMenuEvent* theEvent) override;

	virtual void dragEnterEvent(QDragEnterEvent* theEvent) override;
	virtual void dragLeaveEvent(QDragLeaveEvent* theEvent) override;
	virtual void dragMoveEvent(QDragMoveEvent* theEvent) override;
	virtual void dropEvent(QDropEvent* theEvent) override;

private:
	void _OnDataBeingDestroyed( AEditable* pObject );

private:
	PropertyEditorModel		m_model;
	PropertyEditorDelegate	m_delegate;
	EdPropertyList		m_properties;

	TPtr< AEditable >	m_object;
};

/*
-----------------------------------------------------------------------------
	ProperyEditorDockWidget
-----------------------------------------------------------------------------
*/
class ProperyEditorDockWidget
	: public QDockWidget
	, public EdProjectChild
	, public EdWidgetChild
	, TCountedObject<ProperyEditorDockWidget>
{
public:
	typedef QDockWidget Super;

	ProperyEditorDockWidget( QWidget* parent = nil );
	~ProperyEditorDockWidget();

	void SetObject( AEditable* pObject );
	void RefreshView();

	ProperyEditorWidget& GetTreeView()
	{
		return m_editorWidget;
	}
	PropertyEditorModel* GetModel()
	{
		return checked_cast<PropertyEditorModel*>(m_editorWidget.model());
	}

protected:
	virtual void OnProjectUnloaded() override;

private:
	ProperyEditorWidget		m_editorWidget;
};




// taken from:
// http://www.qtforum.org/article/14204/how-to-center-a-widget-on-the-screen-in-qt-4-before-show-is-called.html#post58933
//
void Util_CenterWidget(QWidget *widget, QWidget *relativeTo, bool useSizeHint = true );
void Util_CenterWidget(QWidget *widget, bool useSizeHint = true );

void Util_NewPropertyEditorDockWidget( AEditable* pObject, QWidget* parent );

