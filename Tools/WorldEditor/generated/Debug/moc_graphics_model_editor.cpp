/****************************************************************************
** Meta object code from reading C++ file 'graphics_model_editor.h'
**
** Created: Thu 10. May 21:28:30 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\source\editors\graphics_model_editor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'graphics_model_editor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ModelBatchHitProxy[] = {

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
      20,   19,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ModelBatchHitProxy[] = {
    "ModelBatchHitProxy\0\0slot_ShowProperties()\0"
};

const QMetaObject ModelBatchHitProxy::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ModelBatchHitProxy,
      qt_meta_data_ModelBatchHitProxy, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ModelBatchHitProxy::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ModelBatchHitProxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ModelBatchHitProxy::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ModelBatchHitProxy))
        return static_cast<void*>(const_cast< ModelBatchHitProxy*>(this));
    if (!strcmp(_clname, "AHitProxy"))
        return static_cast< AHitProxy*>(const_cast< ModelBatchHitProxy*>(this));
    return QObject::qt_metacast(_clname);
}

int ModelBatchHitProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slot_ShowProperties(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_Graphics_Model_Editor[] = {

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
      23,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Graphics_Model_Editor[] = {
    "Graphics_Model_Editor\0\0slot_Random_Action()\0"
};

const QMetaObject Graphics_Model_Editor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Graphics_Model_Editor,
      qt_meta_data_Graphics_Model_Editor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Graphics_Model_Editor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Graphics_Model_Editor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Graphics_Model_Editor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Graphics_Model_Editor))
        return static_cast<void*>(const_cast< Graphics_Model_Editor*>(this));
    if (!strcmp(_clname, "AObjectEditor"))
        return static_cast< AObjectEditor*>(const_cast< Graphics_Model_Editor*>(this));
    if (!strcmp(_clname, "APlaceable"))
        return static_cast< APlaceable*>(const_cast< Graphics_Model_Editor*>(this));
    return QObject::qt_metacast(_clname);
}

int Graphics_Model_Editor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slot_Random_Action(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
