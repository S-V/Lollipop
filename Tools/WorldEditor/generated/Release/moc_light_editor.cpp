/****************************************************************************
** Meta object code from reading C++ file 'light_editor.h'
**
** Created: Tue 15. May 17:18:40 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\source\editors\light_editor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'light_editor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Scene_Editor_Local_Lights[] = {

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
      27,   26,   26,   26, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Scene_Editor_Local_Lights[] = {
    "Scene_Editor_Local_Lights\0\0"
    "Create_New_Local_Light()\0"
};

const QMetaObject Scene_Editor_Local_Lights::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Scene_Editor_Local_Lights,
      qt_meta_data_Scene_Editor_Local_Lights, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Scene_Editor_Local_Lights::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Scene_Editor_Local_Lights::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Scene_Editor_Local_Lights::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Scene_Editor_Local_Lights))
        return static_cast<void*>(const_cast< Scene_Editor_Local_Lights*>(this));
    if (!strcmp(_clname, "TRefCountedObjectList<Local_Light_Editor>"))
        return static_cast< TRefCountedObjectList<Local_Light_Editor>*>(const_cast< Scene_Editor_Local_Lights*>(this));
    return QObject::qt_metacast(_clname);
}

int Scene_Editor_Local_Lights::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: Create_New_Local_Light(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_Scene_Editor_Global_Lights[] = {

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
      28,   27,   27,   27, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Scene_Editor_Global_Lights[] = {
    "Scene_Editor_Global_Lights\0\0"
    "Create_New_Dir_Light()\0"
};

const QMetaObject Scene_Editor_Global_Lights::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Scene_Editor_Global_Lights,
      qt_meta_data_Scene_Editor_Global_Lights, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Scene_Editor_Global_Lights::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Scene_Editor_Global_Lights::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Scene_Editor_Global_Lights::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Scene_Editor_Global_Lights))
        return static_cast<void*>(const_cast< Scene_Editor_Global_Lights*>(this));
    if (!strcmp(_clname, "TRefCountedObjectList<Dir_Light_Editor>"))
        return static_cast< TRefCountedObjectList<Dir_Light_Editor>*>(const_cast< Scene_Editor_Global_Lights*>(this));
    return QObject::qt_metacast(_clname);
}

int Scene_Editor_Global_Lights::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: Create_New_Dir_Light(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
