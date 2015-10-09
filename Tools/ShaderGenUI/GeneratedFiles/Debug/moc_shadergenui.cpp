/****************************************************************************
** Meta object code from reading C++ file 'shadergenui.h'
**
** Created: Wed Mar 28 20:03:31 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\shadergenui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'shadergenui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ShaderGenUI[] = {

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
      13,   12,   12,   12, 0x0a,
      32,   12,   12,   12, 0x0a,
      57,   12,   12,   12, 0x0a,
      81,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ShaderGenUI[] = {
    "ShaderGenUI\0\0SelectInputFiles()\0"
    "SelectOutputFolderHLSL()\0"
    "SelectOutputFolderCPP()\0GenerateCode()\0"
};

const QMetaObject ShaderGenUI::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ShaderGenUI,
      qt_meta_data_ShaderGenUI, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ShaderGenUI::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ShaderGenUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ShaderGenUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ShaderGenUI))
        return static_cast<void*>(const_cast< ShaderGenUI*>(this));
    return QDialog::qt_metacast(_clname);
}

int ShaderGenUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: SelectInputFiles(); break;
        case 1: SelectOutputFolderHLSL(); break;
        case 2: SelectOutputFolderCPP(); break;
        case 3: GenerateCode(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
