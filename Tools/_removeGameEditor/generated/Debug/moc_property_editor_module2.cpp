/****************************************************************************
** Meta object code from reading C++ file 'property_editor_module2.h'
**
** Created: Tue 17. Apr 18:01:21 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\inc\property_editor_module2.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'property_editor_module2.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PropertyEditorPlugin2[] = {

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

static const char qt_meta_stringdata_PropertyEditorPlugin2[] = {
    "PropertyEditorPlugin2\0"
};

const QMetaObject PropertyEditorPlugin2::staticMetaObject = {
    { &QtSupport::Property_Editor_Dock_Widget::staticMetaObject, qt_meta_stringdata_PropertyEditorPlugin2,
      qt_meta_data_PropertyEditorPlugin2, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PropertyEditorPlugin2::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PropertyEditorPlugin2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PropertyEditorPlugin2::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PropertyEditorPlugin2))
        return static_cast<void*>(const_cast< PropertyEditorPlugin2*>(this));
    if (!strcmp(_clname, "EdModule"))
        return static_cast< EdModule*>(const_cast< PropertyEditorPlugin2*>(this));
    if (!strcmp(_clname, "EdWidgetChild"))
        return static_cast< EdWidgetChild*>(const_cast< PropertyEditorPlugin2*>(this));
    if (!strcmp(_clname, "SingleInstance<PropertyEditorPlugin2>"))
        return static_cast< SingleInstance<PropertyEditorPlugin2>*>(const_cast< PropertyEditorPlugin2*>(this));
    typedef QtSupport::Property_Editor_Dock_Widget QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int PropertyEditorPlugin2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef QtSupport::Property_Editor_Dock_Widget QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
