/****************************************************************************
** Meta object code from reading C++ file 'friend.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../NetDiskTCPClient/friend.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'friend.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Friend_t {
    QByteArrayData data[17];
    char stringdata0[212];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Friend_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Friend_t qt_meta_stringdata_Friend = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Friend"
QT_MOC_LITERAL(1, 7, 21), // "showOrHideOnlineUserW"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 10), // "searchUser"
QT_MOC_LITERAL(4, 41, 15), // "flushFriendList"
QT_MOC_LITERAL(5, 57, 12), // "deleteFriend"
QT_MOC_LITERAL(6, 70, 11), // "privateChat"
QT_MOC_LITERAL(7, 82, 16), // "searchPriChatWid"
QT_MOC_LITERAL(8, 99, 15), // "PrivateChatWid*"
QT_MOC_LITERAL(9, 115, 11), // "const char*"
QT_MOC_LITERAL(10, 127, 8), // "chatName"
QT_MOC_LITERAL(11, 136, 20), // "insertPriChatWidList"
QT_MOC_LITERAL(12, 157, 7), // "priChat"
QT_MOC_LITERAL(13, 165, 16), // "groupChatSendMsg"
QT_MOC_LITERAL(14, 182, 20), // "updateGroupShowMsgTE"
QT_MOC_LITERAL(15, 203, 4), // "PDU*"
QT_MOC_LITERAL(16, 208, 3) // "pdu"

    },
    "Friend\0showOrHideOnlineUserW\0\0searchUser\0"
    "flushFriendList\0deleteFriend\0privateChat\0"
    "searchPriChatWid\0PrivateChatWid*\0"
    "const char*\0chatName\0insertPriChatWidList\0"
    "priChat\0groupChatSendMsg\0updateGroupShowMsgTE\0"
    "PDU*\0pdu"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Friend[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x0a /* Public */,
       3,    0,   60,    2, 0x0a /* Public */,
       4,    0,   61,    2, 0x0a /* Public */,
       5,    0,   62,    2, 0x0a /* Public */,
       6,    0,   63,    2, 0x0a /* Public */,
       7,    1,   64,    2, 0x0a /* Public */,
      11,    1,   67,    2, 0x0a /* Public */,
      13,    0,   70,    2, 0x0a /* Public */,
      14,    1,   71,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    0x80000000 | 8, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 8,   12,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15,   16,

       0        // eod
};

void Friend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Friend *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showOrHideOnlineUserW(); break;
        case 1: _t->searchUser(); break;
        case 2: _t->flushFriendList(); break;
        case 3: _t->deleteFriend(); break;
        case 4: _t->privateChat(); break;
        case 5: { PrivateChatWid* _r = _t->searchPriChatWid((*reinterpret_cast< const char*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PrivateChatWid**>(_a[0]) = std::move(_r); }  break;
        case 6: _t->insertPriChatWidList((*reinterpret_cast< PrivateChatWid*(*)>(_a[1]))); break;
        case 7: _t->groupChatSendMsg(); break;
        case 8: _t->updateGroupShowMsgTE((*reinterpret_cast< PDU*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PrivateChatWid* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Friend::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Friend.data,
    qt_meta_data_Friend,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Friend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Friend::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Friend.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Friend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
