/****************************************************************************
** Meta object code from reading C++ file 'app.h'
**
** Created: Tue Apr 10 18:56:27 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\inc\app.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'app.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EdApp[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,    7,    6,    6, 0x05,
      39,    6,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
      66,    6,   61,    6, 0x0a,
      80,    6,    6,    6, 0x08,
      92,    6,    6,    6, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EdApp[] = {
    "EdApp\0\0bLoaded\0projectLoadedFlag(bool)\0"
    "signal_FrameStarted()\0bool\0RequestExit()\0"
    "UpdateAll()\0slot_ImportStyleSheet()\0"
};

const QMetaObject EdApp::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_EdApp,
      qt_meta_data_EdApp, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdApp::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdApp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdApp::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdApp))
        return static_cast<void*>(const_cast< EdApp*>(this));
    if (!strcmp(_clname, "TGlobal<EdApp>"))
        return static_cast< TGlobal<EdApp>*>(const_cast< EdApp*>(this));
    if (!strcmp(_clname, "DependsOnGlobal<EdSystem>"))
        return static_cast< DependsOnGlobal<EdSystem>*>(const_cast< EdApp*>(this));
    return QObject::qt_metacast(_clname);
}

int EdApp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: projectLoadedFlag((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: signal_FrameStarted(); break;
        case 2: { bool _r = RequestExit();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: UpdateAll(); break;
        case 4: slot_ImportStyleSheet(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void EdApp::projectLoadedFlag(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void EdApp::signal_FrameStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
