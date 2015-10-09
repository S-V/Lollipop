/****************************************************************************
** Meta object code from reading C++ file 'create_asset_db_dialog.h'
**
** Created: Tue Apr 10 18:53:35 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\create_asset_db_dialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'create_asset_db_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Create_Asset_Db_Dialog[] = {

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
      24,   23,   23,   23, 0x0a,
      33,   23,   23,   23, 0x08,
      62,   23,   23,   23, 0x08,
      91,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Create_Asset_Db_Dialog[] = {
    "Create_Asset_Db_Dialog\0\0accept()\0"
    "slot_SelectPathToSrcAssets()\0"
    "slot_SelectPathToIntAssets()\0"
    "slot_SelectPathToBinAssets()\0"
};

const QMetaObject Create_Asset_Db_Dialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Create_Asset_Db_Dialog,
      qt_meta_data_Create_Asset_Db_Dialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Create_Asset_Db_Dialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Create_Asset_Db_Dialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Create_Asset_Db_Dialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Create_Asset_Db_Dialog))
        return static_cast<void*>(const_cast< Create_Asset_Db_Dialog*>(this));
    if (!strcmp(_clname, "DependsOnGlobal<AppSettings>"))
        return static_cast< DependsOnGlobal<AppSettings>*>(const_cast< Create_Asset_Db_Dialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Create_Asset_Db_Dialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accept(); break;
        case 1: slot_SelectPathToSrcAssets(); break;
        case 2: slot_SelectPathToIntAssets(); break;
        case 3: slot_SelectPathToBinAssets(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
