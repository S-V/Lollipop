/****************************************************************************
** Meta object code from reading C++ file 'property_grid.h'
**
** Created: Tue 17. Apr 18:01:21 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\inc\property_grid.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'property_grid.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BoolPropertyEditor[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_BoolPropertyEditor[] = {
    "BoolPropertyEditor\0"
};

const QMetaObject BoolPropertyEditor::staticMetaObject = {
    { &QCheckBox::staticMetaObject, qt_meta_stringdata_BoolPropertyEditor,
      qt_meta_data_BoolPropertyEditor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BoolPropertyEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BoolPropertyEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BoolPropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BoolPropertyEditor))
        return static_cast<void*>(const_cast< BoolPropertyEditor*>(this));
    if (!strcmp(_clname, "APropertyEditor"))
        return static_cast< APropertyEditor*>(const_cast< BoolPropertyEditor*>(this));
    return QCheckBox::qt_metacast(_clname);
}

int BoolPropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCheckBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_FloatPropertyEditor[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      30,   21,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FloatPropertyEditor[] = {
    "FloatPropertyEditor\0\0newValue\0"
    "OnValueChanged(double)\0"
};

const QMetaObject FloatPropertyEditor::staticMetaObject = {
    { &QDoubleSpinBox::staticMetaObject, qt_meta_stringdata_FloatPropertyEditor,
      qt_meta_data_FloatPropertyEditor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FloatPropertyEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FloatPropertyEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FloatPropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FloatPropertyEditor))
        return static_cast<void*>(const_cast< FloatPropertyEditor*>(this));
    if (!strcmp(_clname, "APropertyEditor"))
        return static_cast< APropertyEditor*>(const_cast< FloatPropertyEditor*>(this));
    return QDoubleSpinBox::qt_metacast(_clname);
}

int FloatPropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDoubleSpinBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OnValueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_Vec3DPropertyEditor[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      25,   21,   20,   20, 0x08,
      45,   21,   20,   20, 0x08,
      65,   21,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Vec3DPropertyEditor[] = {
    "Vec3DPropertyEditor\0\0val\0xValChanged(double)\0"
    "yValChanged(double)\0zValChanged(double)\0"
};

const QMetaObject Vec3DPropertyEditor::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_Vec3DPropertyEditor,
      qt_meta_data_Vec3DPropertyEditor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Vec3DPropertyEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Vec3DPropertyEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Vec3DPropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Vec3DPropertyEditor))
        return static_cast<void*>(const_cast< Vec3DPropertyEditor*>(this));
    if (!strcmp(_clname, "APropertyEditor"))
        return static_cast< APropertyEditor*>(const_cast< Vec3DPropertyEditor*>(this));
    return QFrame::qt_metacast(_clname);
}

int Vec3DPropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: xValChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: yValChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: zValChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_ColorRGBAPropertyEditor[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      34,   25,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ColorRGBAPropertyEditor[] = {
    "ColorRGBAPropertyEditor\0\0theColor\0"
    "OnColorChanged(QColor)\0"
};

const QMetaObject ColorRGBAPropertyEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ColorRGBAPropertyEditor,
      qt_meta_data_ColorRGBAPropertyEditor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ColorRGBAPropertyEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ColorRGBAPropertyEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ColorRGBAPropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ColorRGBAPropertyEditor))
        return static_cast<void*>(const_cast< ColorRGBAPropertyEditor*>(this));
    if (!strcmp(_clname, "APropertyEditor"))
        return static_cast< APropertyEditor*>(const_cast< ColorRGBAPropertyEditor*>(this));
    return QWidget::qt_metacast(_clname);
}

int ColorRGBAPropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OnColorChanged((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_EnumPropertyEditor[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      29,   20,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EnumPropertyEditor[] = {
    "EnumPropertyEditor\0\0newIndex\0"
    "slot_OnCurrentIndexChanged(int)\0"
};

const QMetaObject EnumPropertyEditor::staticMetaObject = {
    { &QComboBox::staticMetaObject, qt_meta_stringdata_EnumPropertyEditor,
      qt_meta_data_EnumPropertyEditor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EnumPropertyEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EnumPropertyEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EnumPropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EnumPropertyEditor))
        return static_cast<void*>(const_cast< EnumPropertyEditor*>(this));
    if (!strcmp(_clname, "APropertyEditor"))
        return static_cast< APropertyEditor*>(const_cast< EnumPropertyEditor*>(this));
    return QComboBox::qt_metacast(_clname);
}

int EnumPropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QComboBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slot_OnCurrentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_PropertyEditorDelegate[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_PropertyEditorDelegate[] = {
    "PropertyEditorDelegate\0"
};

const QMetaObject PropertyEditorDelegate::staticMetaObject = {
    { &QStyledItemDelegate::staticMetaObject, qt_meta_stringdata_PropertyEditorDelegate,
      qt_meta_data_PropertyEditorDelegate, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PropertyEditorDelegate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PropertyEditorDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PropertyEditorDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PropertyEditorDelegate))
        return static_cast<void*>(const_cast< PropertyEditorDelegate*>(this));
    return QStyledItemDelegate::qt_metacast(_clname);
}

int PropertyEditorDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
