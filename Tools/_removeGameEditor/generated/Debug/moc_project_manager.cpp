/****************************************************************************
** Meta object code from reading C++ file 'project_manager.h'
**
** Created: Tue 17. Apr 18:01:21 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StdAfx.h"
#include "..\..\inc\project_manager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'project_manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EdProjectManager[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x0a,
      37,   17,   17,   17, 0x0a,
      59,   17,   17,   17, 0x0a,
      80,   17,   17,   17, 0x0a,
     102,   17,   17,   17, 0x0a,
     122,   17,   17,   17, 0x0a,
     153,  144,   17,   17, 0x0a,
     211,  202,  197,   17, 0x0a,
     235,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EdProjectManager[] = {
    "EdProjectManager\0\0CreateNewProject()\0"
    "OpenExistingProject()\0SaveCurrentProject()\0"
    "CloseCurrentProject()\0slot_BuildProject()\0"
    "slot_PublishProject()\0settings\0"
    "CreateNewProject(EdProjectCreationSettings)\0"
    "bool\0filePath\0OpenProjectFile(String)\0"
    "MaybeSaveCurrentProject()\0"
};

const QMetaObject EdProjectManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_EdProjectManager,
      qt_meta_data_EdProjectManager, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EdProjectManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EdProjectManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EdProjectManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EdProjectManager))
        return static_cast<void*>(const_cast< EdProjectManager*>(this));
    if (!strcmp(_clname, "EdModule"))
        return static_cast< EdModule*>(const_cast< EdProjectManager*>(this));
    if (!strcmp(_clname, "EdWidgetChild"))
        return static_cast< EdWidgetChild*>(const_cast< EdProjectManager*>(this));
    if (!strcmp(_clname, "TGlobal<EdProjectManager>"))
        return static_cast< TGlobal<EdProjectManager>*>(const_cast< EdProjectManager*>(this));
    if (!strcmp(_clname, "RecentFilesUtil::IClient"))
        return static_cast< RecentFilesUtil::IClient*>(const_cast< EdProjectManager*>(this));
    return QObject::qt_metacast(_clname);
}

int EdProjectManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: CreateNewProject(); break;
        case 1: OpenExistingProject(); break;
        case 2: SaveCurrentProject(); break;
        case 3: CloseCurrentProject(); break;
        case 4: slot_BuildProject(); break;
        case 5: slot_PublishProject(); break;
        case 6: CreateNewProject((*reinterpret_cast< const EdProjectCreationSettings(*)>(_a[1]))); break;
        case 7: { bool _r = OpenProjectFile((*reinterpret_cast< const String(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 8: MaybeSaveCurrentProject(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}
static const uint qt_meta_data_NewProjectWizard[] = {

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
      18,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NewProjectWizard[] = {
    "NewProjectWizard\0\0accept()\0"
};

const QMetaObject NewProjectWizard::staticMetaObject = {
    { &QWizard::staticMetaObject, qt_meta_stringdata_NewProjectWizard,
      qt_meta_data_NewProjectWizard, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NewProjectWizard::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NewProjectWizard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NewProjectWizard::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NewProjectWizard))
        return static_cast<void*>(const_cast< NewProjectWizard*>(this));
    return QWizard::qt_metacast(_clname);
}

int NewProjectWizard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizard::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accept(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_ProjectInfoPage[] = {

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
      17,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ProjectInfoPage[] = {
    "ProjectInfoPage\0\0selectProjectDir()\0"
};

const QMetaObject ProjectInfoPage::staticMetaObject = {
    { &QWizardPage::staticMetaObject, qt_meta_stringdata_ProjectInfoPage,
      qt_meta_data_ProjectInfoPage, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ProjectInfoPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ProjectInfoPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ProjectInfoPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProjectInfoPage))
        return static_cast<void*>(const_cast< ProjectInfoPage*>(this));
    return QWizardPage::qt_metacast(_clname);
}

int ProjectInfoPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: selectProjectDir(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
