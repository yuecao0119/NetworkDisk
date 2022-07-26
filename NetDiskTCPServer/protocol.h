#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h> // Unix库函数，包含了read等系统服务函数
#include <string.h>

typedef unsigned int uint;

// 定义响应消息 TODO 之后改为ENUM形式
#define REGIST_OK "regist ok"                            // 注册
#define REGIST_FAILED "regist failed"

#define LOGIN_OK "login ok"                              // 登录
#define LOGIN_FAILED "login failed"

#define SEARCH_USER_OK "search user ok"                  // 查找
#define SEARCH_USER_OFFLINE "user offline"
#define SEARCH_USER_EMPTY "no such people"

#define ADD_FRIEND_OK "add friend ok"                    // 添加好友
#define ADD_FRIEND_OFFLINE "added friend offline"
#define ADD_FRIEND_EMPTY "no such people"
#define ADD_FRIEND_EXIST "added friend already exist"
// 1对方存在在线，0对方存在不在线，2不存在，3已是好友，4请求错误

#define DEL_FRIEND_OK "delete friend ok"                 // 删除好友
#define DEL_FRIEND_FAILED "delete friend failed"

#define UNKNOWN_ERROR "unknown error"                    // 通用未知错误

// 枚举方式，定义消息类型
enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN = 0,

    ENUM_MSG_TYPE_REGIST_REQUEST,       // 注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,       // 注册回复

    ENUM_MSG_TYPE_LOGIN_REQUEST,        // 登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,        // 登录回复

    ENUM_MSG_TYPE_ONLINE_USERS_REQUEST, // 所有在线用户请求
    ENUM_MSG_TYPE_ONLINE_USERS_RESPOND, // 所有在线用户响应

    ENUM_MSG_TYPE_SEARCH_USER_REQUEST,  // 查找用户请求
    ENUM_MSG_TYPE_SEARCH_USER_RESPOND,  // 查找用户响应

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,   // 添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,   // 添加好友响应

    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,     // 被添加好友消息回复同意
    ENUM_MSG_TYPE_ADD_FRIEND_REJECT,    // 被添加好友消息回复拒绝

    ENUM_MSG_TYPE_FLSUH_FRIEND_REQUEST, // 刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND, // 刷新好友响应

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST, // 删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND, // 删除好友响应

//    ENUM_MSG_TYPE_REQUEST,
//    ENUM_MSG_TYPE_RESPOND,

    ENUM_MSG_TYPE_MAX = 0x00ffffff, // uint最大值 0xffffffff
};

// 设计协议数据单元格式
struct PDU
{
    uint uiPDULen;       // 总的协议数据单元大小
    uint uiMsgType;      // 消息类型
    char caData[64];     // 其他数据
    uint uiMsgLen;       // 实际消息长度
    int caMsg[];         // 实际消息，主要通过caMsg访问消息数据
};

PDU *mkPDU(uint uiMsgLen); // 创建PDU，uiMsglen是可变的，总大小可有其计算得到

#endif // PROTOCOL_H
