/****************************************************************************
** Meta object code from reading C++ file 'engine_stats.h'
**
** Created: Tue Apr 10 18:56:22 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\inc\engine_stats.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'engine_stats.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EdStatsView[] = {

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

static const char qt_meta_stringdata_EdStatsView[] = {
    "EdStatsView\0"
};

const QMetaObject EdStatsView::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_EdStatsView,
      qt_meta_data_EdStatsView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdStatsView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdStatsView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdStatsView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdStatsView))
        return static_cast<void*>(const_cast< EdStatsView*>(this));
    if (!strcmp(_clname, "EdModule"))
        return static_cast< EdModule*>(const_cast< EdStatsView*>(this));
    if (!strcmp(_clname, "EdWidgetChild"))
        return static_cast< EdWidgetChild*>(const_cast< EdStatsView*>(this));
    if (!strcmp(_clname, "SingleInstance<EdStatsView>"))
        return static_cast< SingleInstance<EdStatsView>*>(const_cast< EdStatsView*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int EdStatsView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
