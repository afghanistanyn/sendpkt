/*
 * File: linux/exlibnet.c
 * Date: 2007-09-18
 * Author: gashero
 * Copyright@1999-2007, Harry Gashero Liu.
 */

#include <Python.h>
#include <libnet.h>

//----------------------实体函数--------------------------------------------

unsigned int exlibnet_sendpacket(unsigned char* packet,char* device) {
    libnet_t* l;
    char errbuf[LIBNET_ERRBUF_SIZE];
    l=libnet_init(LIBNET_LINK_ADV,
            device,
            errbuf);
    if (l==NULL) {
    }
    return 0;
}

//----------------------Python接口函数--------------------------------------

static PyObject* sendpacket(PyObject* self, PyObject* args, PyObject* kwargs) {
    //获取输入参数
    PyObject* packetobj=NULL;
    unsigned char* packet=NULL;
    unsigned int packet_s=0;
    static char* kwlist[]={"packet",NULL};
    if (!PyArg_ParseTupleAndKeywords(args,kwargs,"O",kwlist,&packetobj)) {
        return NULL;
    }
    //printf("Address: 0x%08x\n",(unsigned int)packetobj);
    if (!PyString_Check(packetobj)) {
        PyErr_SetString(PyExc_TypeError,"packet must be string object");
        return NULL;
    }
    packet=(unsigned char*)PyString_AsString(packetobj);
    packet_s=PyString_Size(packetobj);
    //初始化libnet
    libnet_t* l;
    char* device=NULL;  //TODO 以后支持选择设备名
    char errbuf[LIBNET_ERRBUF_SIZE];
    l=libnet_init(LIBNET_LINK_ADV,device,errbuf);
    if (l==NULL) {
        PyErr_SetString(PyExc_RuntimeError,errbuf);
        return NULL;
    }
    //发送数据包
    int bytes=0;
    bytes=libnet_adv_write_link(l,packet,packet_s);
    if (bytes<0) {
        PyErr_SetString(PyExc_RuntimeError,libnet_geterror(l));
        return NULL;
    }
    //销毁libnet句柄
    libnet_destroy(l);
    Py_RETURN_NONE;
}

static PyMethodDef SendPktMethods[]={
    {"sendpacket",(PyCFunction)sendpacket,METH_VARARGS | METH_KEYWORDS,"Send a packet to ethernet"},
    {NULL,NULL,0,NULL}
};

PyMODINIT_FUNC initsendpkt(void) {
    Py_InitModule("sendpkt",SendPktMethods);
}