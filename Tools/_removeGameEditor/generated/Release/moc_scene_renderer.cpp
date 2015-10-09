/****************************************************************************
** Meta object code from reading C++ file 'scene_renderer.h'
**
** Created: Tue Apr 10 18:55:01 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\inc\scene_renderer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scene_renderer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EdMyViewportWidget[] = {

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

static const char qt_meta_stringdata_EdMyViewportWidget[] = {
    "EdMyViewportWidget\0\0slot_EditViewportProperties()\0"
};

const QMetaObject EdMyViewportWidget::staticMetaObject = {
    { &EdViewportWidget::staticMetaObject, qt_meta_stringdata_EdMyViewportWidget,
      qt_meta_data_EdMyViewportWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdMyViewportWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdMyViewportWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdMyViewportWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdMyViewportWidget))
        return static_cast<void*>(const_cast< EdMyViewportWidget*>(this));
    return EdViewportWidget::qt_metacast(_clname);
}

int EdMyViewportWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EdViewportWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slot_EditViewportProperties(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_EdSceneRenderer[] = {

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

static const char qt_meta_stringdata_EdSceneRenderer[] = {
    "EdSceneRenderer\0"
};

const QMetaObject EdSceneRenderer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_EdSceneRenderer,
      qt_meta_data_EdSceneRenderer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdSceneRenderer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdSceneRenderer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdSceneRenderer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdSceneRenderer))
        return static_cast<void*>(const_cast< EdSceneRenderer*>(this));
    if (!strcmp(_clname, "EdModule"))
        return static_cast< EdModule*>(const_cast< EdSceneRenderer*>(this));
    if (!strcmp(_clname, "TGlobal<EdSceneRenderer>"))
        return static_cast< TGlobal<EdSceneRenderer>*>(const_cast< EdSceneRenderer*>(this));
    if (!strcmp(_clname, "EdWidgetChild"))
        return static_cast< EdWidgetChild*>(const_cast< EdSceneRenderer*>(this));
    return QObject::qt_metacast(_clname);
}

int EdSceneRenderer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
