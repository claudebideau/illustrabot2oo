#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys, os
from PySide import QtGui
import RttraceBuffer
import traceback

class RttraceBuffers(QtGui.QTreeWidget):
    
    BUFFERS = { "default": RttraceBuffer.RttraceBuffer }

    def __init__(self, rpc, env=[None, None, None], parent=None):
        super(RttraceBuffers, self).__init__(parent)
        self.env      = env
        self.status   = env[1]
        self.progress = env[2]
        self.__treeExpanded = False

        self.setColumnCount(3)
        self.setHeaderLabels(['BUFFER', 'VALUE', 'INFO'])
        self.setFont("Courier New")
        self.setColumnWidth(0, 200)
        self.setColumnWidth(1, 400)
        self.setColumnWidth(2, 600)

        try:
            self.setrpc(rpc)
        except Exception, e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            sys.stderr.write(  "%s/%s[%d] %s %s\n"%(exc_type, fname, exc_tb.tb_lineno, self.__class__.__name__,e))
            pass

    def setrpc(self,rpc):
        try:
            self.__rpc__ = rpc
            self.__mth__ = "xml.rttrace.info"
            self.__info_mth__ = getattr(self.__rpc__,self.__mth__)
            self.__info__()
            if self.status != None:
                self.status.updateState(True)

        except Exception, e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            sys.stderr.write(  "%s/%s[%d] %s %s\n"%(exc_type, fname, exc_tb.tb_lineno, self.__class__.__name__,e))
            if self.status != None:
                self.status.updateState(False)

            pass
        self.itemExpanded.connect(self.treeExpanded)
        self.itemChanged.connect(self.updateLevel)

    def __info__(self):
        res = self.__info_mth__()
        self.__rttrace__= []
        for buffername in res:
            if buffername in self.BUFFERS:
                rttrace = self.BUFFERS[buffername](rpc=self.__rpc__,bname=buffername,env=self.env,parent=self)
            else:
                rttrace = RttraceBuffer.RttraceBuffer(rpc=self.__rpc__,bname=buffername,env=self.env,parent=self)
            self.__rttrace__.append(rttrace)
        return
        
    def treeExpanded(self,item):
        self.__treeExpanded= True
        try:
            item.update()
        except Exception,e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            sys.stderr.write(  "%s/%s[%d] %s %s\n"%(exc_type, fname, exc_tb.tb_lineno, self.__class__.__name__,e))
            traceback.print_exc(file=sys.stderr)
            
            if self.status != None:
                self.status.updateState(False)
            pass
        self.__treeExpanded= False

    def updateLevel(self,item,column):
        try:
            if not self.__treeExpanded:
                # print 'new value =',item.text(column)
                parent = item.parent()
                # print 'type(item) =',type(item), 'type(item)=', type(parent), parent.__rttrace__
                parent.level(item.text(column))
              
        except Exception,e:
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            sys.stderr.write(  "%s/%s[%d] %s %s\n"%(exc_type, fname, exc_tb.tb_lineno, self.__class__.__name__,e))
            traceback.print_exc(file=sys.stderr)
            if self.status != None:
                self.status.updateState(False)
            pass  
            
if __name__ == '__main__':


    import xmlrpclib
    proxy = xmlrpclib.ServerProxy("http://172.25.195.58:8000")
    
    app = QtGui.QApplication(sys.argv)


    viewTx = RttraceBuffers(rpc=proxy)
    viewTx.show()
    
    sys.exit(app.exec_())
