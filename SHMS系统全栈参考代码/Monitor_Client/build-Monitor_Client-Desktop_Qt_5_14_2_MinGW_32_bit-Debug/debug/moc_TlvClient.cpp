/****************************************************************************
** Meta object code from reading C++ file 'TlvClient.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../Monitor_Client/api/TlvClient.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TlvClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TlvClient_t {
    QByteArrayData data[40];
    char stringdata0[562];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TlvClient_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TlvClient_t qt_meta_stringdata_TlvClient = {
    {
QT_MOC_LITERAL(0, 0, 9), // "TlvClient"
QT_MOC_LITERAL(1, 10, 9), // "connected"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 12), // "disconnected"
QT_MOC_LITERAL(4, 34, 14), // "loginSucceeded"
QT_MOC_LITERAL(5, 49, 5), // "token"
QT_MOC_LITERAL(6, 55, 11), // "loginFailed"
QT_MOC_LITERAL(7, 67, 7), // "message"
QT_MOC_LITERAL(8, 75, 17), // "registerSucceeded"
QT_MOC_LITERAL(9, 93, 14), // "registerFailed"
QT_MOC_LITERAL(10, 108, 12), // "ptzSucceeded"
QT_MOC_LITERAL(11, 121, 7), // "ptzBusy"
QT_MOC_LITERAL(12, 129, 9), // "ptzFailed"
QT_MOC_LITERAL(13, 139, 15), // "camerasReceived"
QT_MOC_LITERAL(14, 155, 17), // "QList<CameraInfo>"
QT_MOC_LITERAL(15, 173, 7), // "cameras"
QT_MOC_LITERAL(16, 181, 22), // "streamMetadataReceived"
QT_MOC_LITERAL(17, 204, 14), // "StreamMetadata"
QT_MOC_LITERAL(18, 219, 8), // "metadata"
QT_MOC_LITERAL(19, 228, 20), // "streamPacketReceived"
QT_MOC_LITERAL(20, 249, 11), // "MediaPacket"
QT_MOC_LITERAL(21, 261, 6), // "packet"
QT_MOC_LITERAL(22, 268, 19), // "streamErrorReceived"
QT_MOC_LITERAL(23, 288, 22), // "recordSegmentsReceived"
QT_MOC_LITERAL(24, 311, 24), // "QList<RecordSegmentInfo>"
QT_MOC_LITERAL(25, 336, 8), // "segments"
QT_MOC_LITERAL(26, 345, 20), // "recordConfigReceived"
QT_MOC_LITERAL(27, 366, 30), // "recordPlaybackMetadataReceived"
QT_MOC_LITERAL(28, 397, 22), // "RecordPlaybackMetadata"
QT_MOC_LITERAL(29, 420, 19), // "recordChunkReceived"
QT_MOC_LITERAL(30, 440, 11), // "RecordChunk"
QT_MOC_LITERAL(31, 452, 5), // "chunk"
QT_MOC_LITERAL(32, 458, 19), // "recordErrorReceived"
QT_MOC_LITERAL(33, 478, 12), // "logsReceived"
QT_MOC_LITERAL(34, 491, 7), // "content"
QT_MOC_LITERAL(35, 499, 9), // "loggedOut"
QT_MOC_LITERAL(36, 509, 13), // "errorReceived"
QT_MOC_LITERAL(37, 523, 11), // "socketError"
QT_MOC_LITERAL(38, 535, 11), // "onReadyRead"
QT_MOC_LITERAL(39, 547, 14) // "onDisconnected"

    },
    "TlvClient\0connected\0\0disconnected\0"
    "loginSucceeded\0token\0loginFailed\0"
    "message\0registerSucceeded\0registerFailed\0"
    "ptzSucceeded\0ptzBusy\0ptzFailed\0"
    "camerasReceived\0QList<CameraInfo>\0"
    "cameras\0streamMetadataReceived\0"
    "StreamMetadata\0metadata\0streamPacketReceived\0"
    "MediaPacket\0packet\0streamErrorReceived\0"
    "recordSegmentsReceived\0QList<RecordSegmentInfo>\0"
    "segments\0recordConfigReceived\0"
    "recordPlaybackMetadataReceived\0"
    "RecordPlaybackMetadata\0recordChunkReceived\0"
    "RecordChunk\0chunk\0recordErrorReceived\0"
    "logsReceived\0content\0loggedOut\0"
    "errorReceived\0socketError\0onReadyRead\0"
    "onDisconnected"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TlvClient[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      24,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      22,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  134,    2, 0x06 /* Public */,
       3,    0,  135,    2, 0x06 /* Public */,
       4,    1,  136,    2, 0x06 /* Public */,
       6,    1,  139,    2, 0x06 /* Public */,
       8,    1,  142,    2, 0x06 /* Public */,
       9,    1,  145,    2, 0x06 /* Public */,
      10,    1,  148,    2, 0x06 /* Public */,
      11,    1,  151,    2, 0x06 /* Public */,
      12,    1,  154,    2, 0x06 /* Public */,
      13,    1,  157,    2, 0x06 /* Public */,
      16,    1,  160,    2, 0x06 /* Public */,
      19,    1,  163,    2, 0x06 /* Public */,
      22,    1,  166,    2, 0x06 /* Public */,
      23,    1,  169,    2, 0x06 /* Public */,
      26,    1,  172,    2, 0x06 /* Public */,
      27,    1,  175,    2, 0x06 /* Public */,
      29,    1,  178,    2, 0x06 /* Public */,
      32,    1,  181,    2, 0x06 /* Public */,
      33,    1,  184,    2, 0x06 /* Public */,
      35,    0,  187,    2, 0x06 /* Public */,
      36,    1,  188,    2, 0x06 /* Public */,
      37,    1,  191,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      38,    0,  194,    2, 0x08 /* Private */,
      39,    0,  195,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, 0x80000000 | 24,   25,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 28,   18,
    QMetaType::Void, 0x80000000 | 30,   31,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,   34,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TlvClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TlvClient *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->loginSucceeded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->loginFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->registerSucceeded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->registerFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->ptzSucceeded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->ptzBusy((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->ptzFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->camerasReceived((*reinterpret_cast< const QList<CameraInfo>(*)>(_a[1]))); break;
        case 10: _t->streamMetadataReceived((*reinterpret_cast< const StreamMetadata(*)>(_a[1]))); break;
        case 11: _t->streamPacketReceived((*reinterpret_cast< const MediaPacket(*)>(_a[1]))); break;
        case 12: _t->streamErrorReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->recordSegmentsReceived((*reinterpret_cast< const QList<RecordSegmentInfo>(*)>(_a[1]))); break;
        case 14: _t->recordConfigReceived((*reinterpret_cast< const QList<CameraInfo>(*)>(_a[1]))); break;
        case 15: _t->recordPlaybackMetadataReceived((*reinterpret_cast< const RecordPlaybackMetadata(*)>(_a[1]))); break;
        case 16: _t->recordChunkReceived((*reinterpret_cast< const RecordChunk(*)>(_a[1]))); break;
        case 17: _t->recordErrorReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: _t->logsReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: _t->loggedOut(); break;
        case 20: _t->errorReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 21: _t->socketError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 22: _t->onReadyRead(); break;
        case 23: _t->onDisconnected(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<CameraInfo> >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< StreamMetadata >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< MediaPacket >(); break;
            }
            break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<RecordSegmentInfo> >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<CameraInfo> >(); break;
            }
            break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< RecordPlaybackMetadata >(); break;
            }
            break;
        case 16:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< RecordChunk >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TlvClient::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::connected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::disconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::loginSucceeded)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::loginFailed)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::registerSucceeded)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::registerFailed)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::ptzSucceeded)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::ptzBusy)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::ptzFailed)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QList<CameraInfo> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::camerasReceived)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const StreamMetadata & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::streamMetadataReceived)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const MediaPacket & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::streamPacketReceived)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::streamErrorReceived)) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QList<RecordSegmentInfo> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::recordSegmentsReceived)) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QList<CameraInfo> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::recordConfigReceived)) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const RecordPlaybackMetadata & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::recordPlaybackMetadataReceived)) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const RecordChunk & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::recordChunkReceived)) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::recordErrorReceived)) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::logsReceived)) {
                *result = 18;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::loggedOut)) {
                *result = 19;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::errorReceived)) {
                *result = 20;
                return;
            }
        }
        {
            using _t = void (TlvClient::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TlvClient::socketError)) {
                *result = 21;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TlvClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_TlvClient.data,
    qt_meta_data_TlvClient,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TlvClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TlvClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TlvClient.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TlvClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    return _id;
}

// SIGNAL 0
void TlvClient::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void TlvClient::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void TlvClient::loginSucceeded(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void TlvClient::loginFailed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void TlvClient::registerSucceeded(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void TlvClient::registerFailed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void TlvClient::ptzSucceeded(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void TlvClient::ptzBusy(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void TlvClient::ptzFailed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void TlvClient::camerasReceived(const QList<CameraInfo> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void TlvClient::streamMetadataReceived(const StreamMetadata & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void TlvClient::streamPacketReceived(const MediaPacket & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void TlvClient::streamErrorReceived(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void TlvClient::recordSegmentsReceived(const QList<RecordSegmentInfo> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void TlvClient::recordConfigReceived(const QList<CameraInfo> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void TlvClient::recordPlaybackMetadataReceived(const RecordPlaybackMetadata & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void TlvClient::recordChunkReceived(const RecordChunk & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void TlvClient::recordErrorReceived(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void TlvClient::logsReceived(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void TlvClient::loggedOut()
{
    QMetaObject::activate(this, &staticMetaObject, 19, nullptr);
}

// SIGNAL 20
void TlvClient::errorReceived(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void TlvClient::socketError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 21, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
