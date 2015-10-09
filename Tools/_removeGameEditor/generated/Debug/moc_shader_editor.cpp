/****************************************************************************
** Meta object code from reading C++ file 'shader_editor.h'
**
** Created: Tue 17. Apr 18:01:19 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\inc\shader_editor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'shader_editor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EdShaderOptionsListView[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      36,   25,   24,   24, 0x0a,
      84,   72,   24,   24, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_EdShaderOptionsListView[] = {
    "EdShaderOptionsListView\0\0modelIndex\0"
    "OnShaderOptionSelected(QModelIndex)\0"
    "currentItem\0OnShaderOptionSelected(QListWidgetItem*)\0"
};

const QMetaObject EdShaderOptionsListView::staticMetaObject = {
    { &QListWidget::staticMetaObject, qt_meta_stringdata_EdShaderOptionsListView,
      qt_meta_data_EdShaderOptionsListView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdShaderOptionsListView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdShaderOptionsListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdShaderOptionsListView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdShaderOptionsListView))
        return static_cast<void*>(const_cast< EdShaderOptionsListView*>(this));
    return QListWidget::qt_metacast(_clname);
}

int EdShaderOptionsListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OnShaderOptionSelected((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: OnShaderOptionSelected((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_EdDockWidgetShadersList[] = {

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

static const char qt_meta_stringdata_EdDockWidgetShadersList[] = {
    "EdDockWidgetShadersList\0"
};

const QMetaObject EdDockWidgetShadersList::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_EdDockWidgetShadersList,
      qt_meta_data_EdDockWidgetShadersList, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdDockWidgetShadersList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdDockWidgetShadersList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdDockWidgetShadersList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdDockWidgetShadersList))
        return static_cast<void*>(const_cast< EdDockWidgetShadersList*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int EdDockWidgetShadersList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_CodeEditor[] = {

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
      26,   12,   11,   11, 0x08,
      57,   11,   11,   11, 0x08,
      82,   80,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CodeEditor[] = {
    "CodeEditor\0\0newBlockCount\0"
    "updateLineNumberAreaWidth(int)\0"
    "highlightCurrentLine()\0,\0"
    "updateLineNumberArea(QRect,int)\0"
};

const QMetaObject CodeEditor::staticMetaObject = {
    { &QPlainTextEdit::staticMetaObject, qt_meta_stringdata_CodeEditor,
      qt_meta_data_CodeEditor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CodeEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CodeEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CodeEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CodeEditor))
        return static_cast<void*>(const_cast< CodeEditor*>(this));
    return QPlainTextEdit::qt_metacast(_clname);
}

int CodeEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateLineNumberAreaWidth((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: highlightCurrentLine(); break;
        case 2: updateLineNumberArea((*reinterpret_cast< const QRect(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_EdModule_ShaderEditor[] = {

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

static const char qt_meta_stringdata_EdModule_ShaderEditor[] = {
    "EdModule_ShaderEditor\0"
};

const QMetaObject EdModule_ShaderEditor::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_EdModule_ShaderEditor,
      qt_meta_data_EdModule_ShaderEditor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdModule_ShaderEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdModule_ShaderEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdModule_ShaderEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdModule_ShaderEditor))
        return static_cast<void*>(const_cast< EdModule_ShaderEditor*>(this));
    if (!strcmp(_clname, "EdModule"))
        return static_cast< EdModule*>(const_cast< EdModule_ShaderEditor*>(this));
    if (!strcmp(_clname, "TGlobal<EdModule_ShaderEditor>"))
        return static_cast< TGlobal<EdModule_ShaderEditor>*>(const_cast< EdModule_ShaderEditor*>(this));
    if (!strcmp(_clname, "EdWidgetChild"))
        return static_cast< EdWidgetChild*>(const_cast< EdModule_ShaderEditor*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int EdModule_ShaderEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
