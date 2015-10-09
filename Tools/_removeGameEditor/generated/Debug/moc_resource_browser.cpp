/****************************************************************************
** Meta object code from reading C++ file 'resource_browser.h'
**
** Created: Tue 17. Apr 18:01:20 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\inc\resource_browser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'resource_browser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AssetsViewModel[] = {

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

static const char qt_meta_stringdata_AssetsViewModel[] = {
    "AssetsViewModel\0"
};

const QMetaObject AssetsViewModel::staticMetaObject = {
    { &QFileSystemModel::staticMetaObject, qt_meta_stringdata_AssetsViewModel,
      qt_meta_data_AssetsViewModel, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AssetsViewModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AssetsViewModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AssetsViewModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AssetsViewModel))
        return static_cast<void*>(const_cast< AssetsViewModel*>(this));
    return QFileSystemModel::qt_metacast(_clname);
}

int AssetsViewModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFileSystemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_AssetsViewDockWidget[] = {

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
      33,   22,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_AssetsViewDockWidget[] = {
    "AssetsViewDockWidget\0\0modelIndex\0"
    "slot_OnItemDoubleClicked(QModelIndex)\0"
};

const QMetaObject AssetsViewDockWidget::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_AssetsViewDockWidget,
      qt_meta_data_AssetsViewDockWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AssetsViewDockWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AssetsViewDockWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AssetsViewDockWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AssetsViewDockWidget))
        return static_cast<void*>(const_cast< AssetsViewDockWidget*>(this));
    if (!strcmp(_clname, "EdProjectChild"))
        return static_cast< EdProjectChild*>(const_cast< AssetsViewDockWidget*>(this));
    if (!strcmp(_clname, "TGlobal<AssetsViewDockWidget>"))
        return static_cast< TGlobal<AssetsViewDockWidget>*>(const_cast< AssetsViewDockWidget*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int AssetsViewDockWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slot_OnItemDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_EdResourceBrowserModule[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      25,   24,   24,   24, 0x0a,
      57,   24,   24,   24, 0x08,
      87,   24,   24,   24, 0x08,
     118,   24,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EdResourceBrowserModule[] = {
    "EdResourceBrowserModule\0\0"
    "slot_GenerateFileGuidsCppCode()\0"
    "slot_CreateResourceDatabase()\0"
    "slot_RebuildResourceDatabase()\0"
    "slot_OpenResourceDatabase()\0"
};

const QMetaObject EdResourceBrowserModule::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_EdResourceBrowserModule,
      qt_meta_data_EdResourceBrowserModule, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdResourceBrowserModule::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdResourceBrowserModule::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdResourceBrowserModule::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdResourceBrowserModule))
        return static_cast<void*>(const_cast< EdResourceBrowserModule*>(this));
    if (!strcmp(_clname, "EdModule"))
        return static_cast< EdModule*>(const_cast< EdResourceBrowserModule*>(this));
    if (!strcmp(_clname, "TGlobal<EdResourceBrowserModule>"))
        return static_cast< TGlobal<EdResourceBrowserModule>*>(const_cast< EdResourceBrowserModule*>(this));
    if (!strcmp(_clname, "EdWidgetChild"))
        return static_cast< EdWidgetChild*>(const_cast< EdResourceBrowserModule*>(this));
    return QObject::qt_metacast(_clname);
}

int EdResourceBrowserModule::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slot_GenerateFileGuidsCppCode(); break;
        case 1: slot_CreateResourceDatabase(); break;
        case 2: slot_RebuildResourceDatabase(); break;
        case 3: slot_OpenResourceDatabase(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
