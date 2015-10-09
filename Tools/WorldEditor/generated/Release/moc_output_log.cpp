/****************************************************************************
** Meta object code from reading C++ file 'output_log.h'
**
** Created: Tue 15. May 17:18:40 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\source\output_log.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'output_log.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EdOutputLog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EdOutputLog[] = {
    "EdOutputLog\0\0signal_ConsoleChanged()\0"
    "slot_ScrollToBottom()\0"
};

const QMetaObject EdOutputLog::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_EdOutputLog,
      qt_meta_data_EdOutputLog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdOutputLog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdOutputLog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdOutputLog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdOutputLog))
        return static_cast<void*>(const_cast< EdOutputLog*>(this));
    if (!strcmp(_clname, "EdModule"))
        return static_cast< EdModule*>(const_cast< EdOutputLog*>(this));
    if (!strcmp(_clname, "mxOutputDevice"))
        return static_cast< mxOutputDevice*>(const_cast< EdOutputLog*>(this));
    if (!strcmp(_clname, "EdWidgetChild"))
        return static_cast< EdWidgetChild*>(const_cast< EdOutputLog*>(this));
    if (!strcmp(_clname, "SingleInstance<EdOutputLog>"))
        return static_cast< SingleInstance<EdOutputLog>*>(const_cast< EdOutputLog*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int EdOutputLog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: signal_ConsoleChanged(); break;
        case 1: slot_ScrollToBottom(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void EdOutputLog::signal_ConsoleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
