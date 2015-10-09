/****************************************************************************
** Meta object code from reading C++ file 'material_editor.h'
**
** Created: Wed 2. May 20:09:09 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\material_editor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'material_editor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MaterialEditor[] = {

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
      16,   15,   15,   15, 0x08,
      46,   15,   15,   15, 0x08,
      79,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MaterialEditor[] = {
    "MaterialEditor\0\0slot_CreateNewAssetDatabase()\0"
    "slot_OpenExistingAssetDatabase()\0"
    "slot_RebuildCurrentAssetDatabase()\0"
};

const QMetaObject MaterialEditor::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MaterialEditor,
      qt_meta_data_MaterialEditor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MaterialEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MaterialEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MaterialEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MaterialEditor))
        return static_cast<void*>(const_cast< MaterialEditor*>(this));
    if (!strcmp(_clname, "RecentFilesUtil::IClient"))
        return static_cast< RecentFilesUtil::IClient*>(const_cast< MaterialEditor*>(this));
    if (!strcmp(_clname, "DependsOnGlobal<AppSettings>"))
        return static_cast< DependsOnGlobal<AppSettings>*>(const_cast< MaterialEditor*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MaterialEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slot_CreateNewAssetDatabase(); break;
        case 1: slot_OpenExistingAssetDatabase(); break;
        case 2: slot_RebuildCurrentAssetDatabase(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
