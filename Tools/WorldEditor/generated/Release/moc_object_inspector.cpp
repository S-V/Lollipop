/****************************************************************************
** Meta object code from reading C++ file 'object_inspector.h'
**
** Created: Tue 15. May 17:18:40 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\source\widgets\object_inspector.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'object_inspector.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EdWidget_ObjectInspector[] = {

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

static const char qt_meta_stringdata_EdWidget_ObjectInspector[] = {
    "EdWidget_ObjectInspector\0"
};

const QMetaObject EdWidget_ObjectInspector::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_EdWidget_ObjectInspector,
      qt_meta_data_EdWidget_ObjectInspector, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdWidget_ObjectInspector::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdWidget_ObjectInspector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdWidget_ObjectInspector::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdWidget_ObjectInspector))
        return static_cast<void*>(const_cast< EdWidget_ObjectInspector*>(this));
    return QTreeView::qt_metacast(_clname);
}

int EdWidget_ObjectInspector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_EdObjectInspector[] = {

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

static const char qt_meta_stringdata_EdObjectInspector[] = {
    "EdObjectInspector\0"
};

const QMetaObject EdObjectInspector::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_EdObjectInspector,
      qt_meta_data_EdObjectInspector, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdObjectInspector::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdObjectInspector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdObjectInspector::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdObjectInspector))
        return static_cast<void*>(const_cast< EdObjectInspector*>(this));
    if (!strcmp(_clname, "EdModule"))
        return static_cast< EdModule*>(const_cast< EdObjectInspector*>(this));
    if (!strcmp(_clname, "EdWidgetChild"))
        return static_cast< EdWidgetChild*>(const_cast< EdObjectInspector*>(this));
    if (!strcmp(_clname, "SingleInstance<EdObjectInspector>"))
        return static_cast< SingleInstance<EdObjectInspector>*>(const_cast< EdObjectInspector*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int EdObjectInspector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
