#include "protocol.h"


PDU *mkPDU(uint uiMsgLen)
{
    uint uiPDULen = sizeof (PDU) + uiMsgLen;
    PDU* pdu = (PDU*)malloc(uiPDULen);

    if(NULL == pdu)
    {
        exit(EXIT_FAILURE); // 错误退出程序
    }
    memset (pdu, 0, uiPDULen); // 数据初始化为0
    pdu -> uiPDULen = uiPDULen; // 数据参数初始化
    pdu -> uiMsgLen = uiMsgLen;

    return pdu;
}
