#!/usr/bin/env python
# -*- coding: utf-8 -*-


from PySide import QtCore, QtGui
import array, re


class RttraceBuffer(QtGui.QTreeWidgetItem):

    
    #~ FLOW_MSG = {'tx': RingBufferMsg.RingTxBufferMsg, 'rx' :RingBufferMsg.RingRxBufferMsg }
    PARAM=['buffer', 'level', 'ptr', 'format']

    def __init__(self, rpc, bname, env=[None, None, None], parent=None):
        super(RttraceBuffer,self).__init__(parent)
        self.__bname__  = bname
        self.__rpc__     = rpc
        self.env      = env
        self.status   = env[1]
        self.progress = env[2]
        self.NBWORD =4

        self.__getlevel_mth__ = "xml.rttrace.level.get"
        self.__setlevel_mth__ = "xml.rttrace.level.set"
        self.__dump__ = "xml.rttrace.dump"
        self.__info_level__  = getattr(self.__rpc__,self.__getlevel_mth__)
        self.__set_level__  = getattr(self.__rpc__,self.__setlevel_mth__)
        self.__info_dump__ = getattr(self.__rpc__,self.__dump__)
            
        self.setText(0, self.__bname__)
        for k in self.PARAM:
            item = QtGui.QTreeWidgetItem()
            item.setText(0,k)
            self.addChild(item)

    def level(self, value=None):
        if value == None:
            res = self.__info_level__(*[self.__bname__])
            return res
        else:
            if value[0:2] in ['0x']:
                res =self.__set_level__(*[self.__bname__, int(value,16)])
            else:
                res =self.__set_level__(*[self.__bname__, int(value)])

    def update(self):
        res = self.__info_dump__(*[self.__bname__])

        for kidx in range(len(self.PARAM)):
            k= self.PARAM[kidx]
            item = self.child(kidx)
            if k in ['level', 'ptr']:
                item.setText(1, "0x%08x"%res[k])
                if k == 'level':
                    item.setFlags(item.flags()| QtCore.Qt.ItemIsEditable)
            elif k == 'format':
                self.field = re.split(r'\|', str(res[k]))
                self.NBWORD = len(self.field)
                item.setText(1, str(self.field))
                # not yet exploited
            else:
                item.setText(1, str(res[k]))
                
        # split data per group of NBWORD
        size    = len(res['data'])
        nbdata  = size/self.NBWORD
        nbparam = len(self.PARAM)
        data=[]
        for k in range(nbdata):
            kidx = self.NBWORD*k
            #~ if res['data'][kidx] != 0:
            data.append([ res['data'][i] for i in range(kidx,kidx+self.NBWORD)])
        
        start =0
        for k in range(len(data)):
            if data[k][0] == -1:
                start=k
        dbuffer = data[start:len(data)+1] + data[0:start+1]
        
        # data management
        nbchild = self.childCount()
        nbchildExp = nbdata + nbparam
        # cleanup:
        if nbchild > nbchildExp:
            for cidx in range(nbchild-1,nbchildExp,-1):
                child = self.child(cidx)
                self.removeChild(child)
        if nbchild != nbchildExp:
            idx=[nbchildExp,nbchild]
            idx.sort()

            for cidx in range(idx[0],idx[1]):
                if nbchild < nbchildExp:
                    #create child
                    # id = cidx-nbparam
                    item = QtGui.QTreeWidgetItem()
                    self.addChild(item)

        #re update item:
        if self.progress != None:
            self.progress.setMaximum(nbdata)
            self.progress.setValue(0)
        
        nbchild = self.childCount()
        for cidx in range(nbparam,nbchild):

            item = self.child(cidx)
            # update progress bar
            kidx = cidx-nbparam+1
            if self.progress != None:
                self.progress.setValue(kidx)
            #~ print data[kidx]
            pdata = self.__display_binary__(dbuffer[kidx])
            item.setText(1, pdata[0])
            item.setText(2, pdata[1])

          

    def __delta_time__(self,sec, nsec):
        if getattr(self, "__delta__",None) == None:
            self.__delta__ = { 'sec' : sec, 'nsec' : nsec }

        delta_sec = sec - self.__delta__['sec']
        if delta_sec !=  0:
            delta_nsec = nsec + 1000000000 - self.__delta__['nsec']
            delta_sec  -= 1
        else:
            delta_nsec = nsec - self.__delta__['nsec']
            if delta_nsec < 0:
                delta_nsec += 1000000000
                delta_sec  -= 1
        
        out = "%5d s / %10d ns"%(delta_sec,delta_nsec)
        
        self.__delta__ = { 'sec' : sec, 'nsec' : nsec }
        return out


    def __arg__(self, arg1, arg2):
        cmd           = (arg1 >> 24) & 0xFF
        filter_value  = (arg1 & 0xFFFFFF)
        out = " cmd =%2x filter = %06x v1 = %04x v2=%04x"%(cmd, filter_value ,arg2>>16,arg2&0xFFFF)
        return out

    def __parse_info__(self, data=[]):
        out  = self.__delta_time__(data[0], data[1])
        out += self.__arg__(data[2], data[3])
        
        return out

    def __display_binary__(self, binary):
        data = array.array('I', [d & 0xFFFFFFFF for d in binary])
        outhex = ''
        outmsg = ''
        nbword = len(data)
        mask = ~(self.NBWORD -1)
        nbloop = ((nbword + (self.NBWORD-1)) &mask)/self.NBWORD
        
        for i in range(nbloop):
            #~ ascii = []
            for j in range(self.NBWORD):
                idx = i*self.NBWORD + j 
                if idx < nbword:
                    outhex +=  '%08x '%data[idx]
                else:
                    outhex +=  '-------- '

            idx=i*self.NBWORD
            outmsg += self.__parse_info__(data[idx:(idx+self.NBWORD)])


        return (outhex, outmsg)


