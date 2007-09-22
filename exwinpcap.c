/*
 * File: exwinpcap.c
 * Date: 2007-09-22
 * Author: gashero
 * Copyright@1999-2007, Harry Gashero Liu.
 */

#include <Python.h>
#include <pcap.h>

#define MAX_DEVICE_NAME_LENGTH 255

static PyObject* sendpacket(PyObject* self, PyObject* args, PyObject* kwargs) {
    //获取输入参数
    PyObject* packetobj=NULL;
    unsigned char* packet=NULL;
    unsigned int packet_s=0;
    char* device=NULL;
    static char* kwlist[]={"packet","device",NULL};
    if (!PyArg_ParseTupleAndKeywords(args,kwargs,"s#s",kwlist,&packet,&packet_s,&device)) {
        return NULL;
    }
    //printf("Address: 0x%08x\n",(unsigned int)packetobj);
    if (!PyString_Check(packetobj)) {
        PyErr_SetString(PyExc_TypeError,"packet must be string object");
        return NULL;
    }
    packet=(unsigned char*)PyString_AsString(packetobj);
    packet_s=PyString_Size(packetobj);
    //初始化Winpcap
    pcap_t* p=NULL;
    //char* device=NULL;
    //char device[]="\\Device\\NPF_GenericDialupAdapter";
    //char device[]="\\Device\\NPF_{F8F5ED1E-36B4-4E50-83B6-8CAA999A5DE4}";
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 net_mask,net_ip;
    //device=pcap_lookupdev(errbuf);
    if (device==NULL) {
        PyErr_SetString(PyExc_RuntimeError,errbuf);
        return NULL;
    }
    printf("Device: %s\n",device);
    pcap_lookupnet(device,&net_ip,&net_mask,errbuf);
    //XXX xxx
    printf("ip=%u,mask=%u\n",net_ip,net_mask);
    p=pcap_open_live(device,BUFSIZ,1,0,errbuf);
    if (p==NULL) {
        PyErr_SetString(PyExc_RuntimeError,errbuf);
        return NULL;
    }
    //发送数据包
    int bytes=0;
    bytes=pcap_sendpacket(p,packet,packet_s);
    if (bytes<0) {
        PyErr_SetString(PyExc_RuntimeError,pcap_geterr(p));
        return NULL;
    }else {
        printf("Bytes=%d\n",bytes);
    }
    //销毁winpcap句柄
    pcap_close(p);
    Py_RETURN_NONE;
}

static PyObject* findalldevs(PyObject* self, PyObject* args) {
    //确保没有输入参数
    if (!PyArg_ParseTuple(args,""))
        return NULL;
    //获取设备列表
    int c=0;
    pcap_if_t* first;
    pcap_if_t* now;
    char errbuf[PCAP_ERRBUF_SIZE];
    c=pcap_findalldevs(&first,errbuf);
    if (c!=0) {
        PyErr_SetString(PyExc_RuntimeError,errbuf);
        return NULL;
    }
    //循环插入设备名字符串
    PyObject* devlist=NULL;
    PyObject* devname=NULL;
    devlist=PyList_New(0);
    now=first;
    while (1) {
        //printf("DEV: %s\n",now->name);
        //XXX Python的字符串构造函数看来是可以利用已有的字符串，而不必自己重新申请空间
        devname=PyString_FromString(now->name);
        PyList_Append(devlist,devname);
        now=now->next;
        if (now==NULL)
            break;
    }
    //释放原设备列表占用的资源
    pcap_freealldevs(first);
    return devlist;
}

static PyMethodDef SendPktMethods[]={
    {"sendpacket",(PyCFunction)sendpacket,METH_VARARGS | METH_KEYWORDS,"Send a packet to ethernet"},
    {"findalldevs",findalldevs,METH_VARARGS,"Find all net interfaces"},
    {NULL,NULL,0,NULL}
};

PyMODINIT_FUNC initsendpkt(void) {
    Py_InitModule("sendpkt",SendPktMethods);
}
