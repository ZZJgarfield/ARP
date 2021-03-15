#include <stdio.h>
#include <stdlib.h>
#include <Winsock2.h>
#include <Iphlpapi.h>
#include <tchar.h>
#include <conio.h>
//#pragma comment (lib,"Iphlpapi") //pragma只支持在VC中使用，在GCC中不支持
//#pragma comment (lib,"Ws2_32")
int _tmain(int argc,_TCHAR *argv[])
{
    ULONG ulHostIp;
    ULONG I;
    MIB_IPADDRTABLE* pIPAddrTable = (MIB_IPADDRTABLE*)malloc(sizeof(MIB_IPADDRTABLE));
    ULONG dwSize = 0,dwRetVal = 0;
    int choose = 0;
    if (GetIpAddrTable(pIPAddrTable,&dwSize,0) == ERROR_INSUFFICIENT_BUFFER)
    {
        //释放内存并获取dwSize值的内存大小
        free(pIPAddrTable);
        pIPAddrTable = (MIB_IPADDRTABLE*)malloc(dwSize);
    }
    if ((dwRetVal = GetIpAddrTable(pIPAddrTable,&dwSize,0)) == NO_ERROR)
    {
        unsigned char* strIpAddr[6];
        int temp;
        //dwNumEntries保存着网卡个数
        for(temp = 0;temp<pIPAddrTable->dwNumEntries;temp++)
        {
            strIpAddr[temp] = (unsigned char*)(&pIPAddrTable->table[temp].dwAddr);
            printf("table[%d]:%d.%d.%d.%d\n",temp,strIpAddr[temp][0],strIpAddr[temp][1],strIpAddr[temp][2],strIpAddr[temp][3]);
        }
        printf("CHOOSE:");
        scanf("%d",&choose);
        ulHostIp = ntohl(pIPAddrTable->table[choose].dwAddr);
        //ulHostMask是子网掩码
        ULONG ulHostMask = ntohl(pIPAddrTable->table[choose].dwMask);
        //取反得到主机个数
        for(I = 1;I < (~ulHostMask);I++)
        {
            static ULONG uNo = 0;
            HRESULT hr;
            IPAddr iPAddr;
            ULONG pulMac[2];
            ULONG ulLen;
            iPAddr = htonl(I+(ulHostIp&ulHostMask));
            memset(pulMac, 0xff, sizeof(pulMac));
            ulLen = 6;
            hr = SendARP(iPAddr, 0, pulMac, &ulLen);
            PBYTE pbHexMac;
            pbHexMac=(PBYTE)pulMac;
            unsigned char * strIpAddr = (unsigned char *)(&iPAddr);
            if(ulLen == 6)
            {
                uNo++;
                printf("%d:MAC address %02X:%02x:%02X:%02X:%02X:%02X\nIPAddress %d.%d.%d.%d\n",uNo,pbHexMac[0],pbHexMac[1],pbHexMac[2],pbHexMac[3],pbHexMac[4],pbHexMac[5],strIpAddr[0],strIpAddr[1],strIpAddr[2],strIpAddr[3]);
            }
            else
            {
                printf("%d.%d.%d.%d NOTHING FOUND!\n",strIpAddr[0],strIpAddr[1],strIpAddr[2],strIpAddr[3]);
            }
        }
    }
    else
    {
        printf("Call to GetIpAddrTable failed.\n");
    }
    printf("OVER!\n");
    free(pIPAddrTable);
    return 0;
}
