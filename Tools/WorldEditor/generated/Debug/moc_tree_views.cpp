/****************************************************************************
** Meta object code from reading C++ file 'tree_views.h'
**
** Created: Thu 3. May 19:29:51 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\source\widgets\tree_views.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tree_views.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EdTreeViewModel[] = {

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

static const char qt_meta_stringdata_EdTreeViewModel[] = {
    "EdTreeViewModel\0"
};

const QMetaObject EdTreeViewModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_EdTreeViewModel,
      qt_meta_data_EdTreeViewModel, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdTreeViewModel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdTreeViewModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdTreeViewModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdTreeViewModel))
        return static_cast<void*>(const_cast< EdTreeViewModel*>(this));
    if (!strcmp(_clname, "EdProjectChild"))
        return static_cast< EdProjectChild*>(const_cast< EdTreeViewModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int EdTreeViewModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_EdTreeView[] = {

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
      18,   12,   11,   11, 0x08,
      50,   12,   11,   11, 0x08,
      82,   12,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EdTreeView[] = {
    "EdTreeView\0\0index\0slot_OnItemPressed(QModelIndex)\0"
    "slot_OnItemClicked(QModelIndex)\0"
    "slot_OnItemDoubleClicked(QModelIndex)\0"
};

const QMetaObject EdTreeView::staticMetaObject = {
    { &QTreeView::staticMetaObject, qt_meta_stringdata_EdTreeView,
      qt_meta_data_EdTreeView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdTreeView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdTreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdTreeView))
        return static_cast<void*>(const_cast< EdTreeView*>(this));
    return QTreeView::qt_metacast(_clname);
}

int EdTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slot_OnItemPressed((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: slot_OnItemClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: slot_OnItemDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
