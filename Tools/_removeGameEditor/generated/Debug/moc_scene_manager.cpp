/****************************************************************************
** Meta object code from reading C++ file 'scene_manager.h'
**
** Created: Tue 17. Apr 18:01:20 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\inc\scene_manager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scene_manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EdSceneManager[] = {

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
      16,   15,   15,   15, 0x08,
      38,   15,   15,   15, 0x08,
      63,   15,   15,   15, 0x08,
      90,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EdSceneManager[] = {
    "EdSceneManager\0\0slot_CreateNewWorld()\0"
    "slot_LoadWorldFromFile()\0"
    "slot_LoadRedFactionLevel()\0"
    "slot_ShowEntityProperties()\0"
};

const QMetaObject EdSceneManager::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_EdSceneManager,
      qt_meta_data_EdSceneManager, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdSceneManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdSceneManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdSceneManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdSceneManager))
        return static_cast<void*>(const_cast< EdSceneManager*>(this));
    if (!strcmp(_clname, "EdModule"))
        return static_cast< EdModule*>(const_cast< EdSceneManager*>(this));
    if (!strcmp(_clname, "EdWidgetChild"))
        return static_cast< EdWidgetChild*>(const_cast< EdSceneManager*>(this));
    if (!strcmp(_clname, "TGlobal<EdSceneManager>"))
        return static_cast< TGlobal<EdSceneManager>*>(const_cast< EdSceneManager*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int EdSceneManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slot_CreateNewWorld(); break;
        case 1: slot_LoadWorldFromFile(); break;
        case 2: slot_LoadRedFactionLevel(); break;
        case 3: slot_ShowEntityProperties(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
