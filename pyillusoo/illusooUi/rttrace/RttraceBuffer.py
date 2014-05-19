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



class RttraceBufferFpgaCap(RttraceBuffer):
    RT_TRACE_CAP_STORE     = 0x02
    RT_TRACE_CAP_WAIT      = 0x04

    RT_TRACE_CAP_SPECIFIC  = 0x08
    RT_TRACE_CAP_SEND_CFG  = 0x10    
    NBWORD =8

    def __init__(self, rpc, bname, env=[None, None, None], parent=None):
        super(RttraceBufferFpgaCap,self).__init__(rpc=rpc, bname=bname, env=env, parent=parent)
        
    def __arg__(self, arg1, arg2, arg3, arg4, arg5):
        cmd = (arg1 >> 24) & 0xFF
        time = arg2
        if (time & 0x80000000) != 0:
            time -= 0x100000000 

            
        tick = time/62500
        if cmd == self.RT_TRACE_CAP_STORE:
            out = " CAP  : curr time = %8d tick %4d CapIdx=%04x length=%04d"%(time,tick,arg4,arg5)
        elif cmd == self.RT_TRACE_CAP_WAIT:
            out = " WAIT : last time = %8d tick %4d LoopIdx =%08x"%(time,tick,arg2)
        elif cmd == self.RT_TRACE_CAP_SPECIFIC:
            out = " SPEC : last time = %8d idx=%4d length =%08x action=%08x"%(time,arg3,arg4,arg5)
        elif cmd == self.RT_TRACE_CAP_SEND_CFG:
            out = " SCFG : virtualAddr %08x length = %d"%(arg3,arg4)
        else:
            out = " IDLE"
        return out
    
    def __parse_info__(self, data=[]):
        out  = self.__delta_time__(data[0], data[1])
        out += self.__arg__(data[2], data[3], data[4], data[5], data[6])
        
        return out

class RttraceBufferRingRx(RttraceBuffer):
    RT_TRACE_RX_SEND = 0x02
    RT_TRACE_RX_WAIT = 0x04
    NBWORD =8

    def __init__(self, rpc, bname, env=[None, None, None], parent=None):
        super(RttraceBufferRingRx,self).__init__(rpc=rpc, bname=bname, env=env, parent=parent)
        


    def __parse_info__(self, data=[]):
        out  = self.__delta_time__(data[0], data[1])
        out += self.__arg__(data[2], data[3], data[4], data[5])
        
        return out
        
    def __arg__(self, arg1, arg2, arg3, arg4):
        ring = (arg1 >> 24) & 0x7F
        cmd = (arg1 >> 31) & 0x1

        ltime = arg2
        if ltime > 0x80000000:
            ltime -= 0x100000000        
        #~ if ltime > 0x80000000:
            #~ print ltime ,
            #~ ltime &= 0x7FFFFFFF
            #~ print ltime ,
            #~ ltime  = ~ltime
            #~ print ltime ,
            #~ ltime -=1
            #~ print ltime 
            # ltime -= 0x100000000
        
        size = arg3
        if cmd == self.RT_TRACE_RX_WAIT:
            out = " WAIT : ring %06x / time=%6d size=%04d"%(ring, ltime, size)
        else:
            out = " SEND : ring %06x / time=%6d size=%04d"%(ring, ltime, size)
        return out
        

class RttraceBufferRingTx(RttraceBuffer):
    NBWORD =8
    RT_TRACE_RING_TX_MSG    = 0x02
    RT_TRACE_RING_TX_WAIT   = 0x04
    RT_TRACE_RING_TX_FREEZE = 0x08
    RT_TRACE_RING_TX_FULL   = 0x10
    
    def __init__(self, rpc, bname, env=[None, None, None], parent=None):
        super(RttraceBufferRingTx,self).__init__(rpc=rpc, bname=bname, env=env, parent=parent)

    def __delta_time__(self, nsec):
        if getattr(self, "__delta__",None) == None:
            self.__delta__ = { 'nsec' : nsec }

        delta_nsec = nsec - self.__delta__['nsec']
        if delta_nsec < 0:
            delta_nsec += 1000000000
        
        out = "%10d ns"%(delta_nsec)
        
        self.__delta__ = { 'nsec' : nsec }
        return out

    def __parse_info__(self, data=[]):
        out  = self.__delta_time__(data[0])
        out += self.__arg__( data[1], data[2], data[3], data[4], data[5], data[6])

        return out
        
    def __arg__(self, arg1, arg2, arg3, arg4=0, arg5=0, arg6=0):
        ring         = (arg1 >> 24) & 0x7F
        #cmd          = (arg1 >> 31) & 0x1
        filter_value = arg1&0xFFFFFF
        ltime = arg2
        if ltime > 0x80000000:
            ltime -= 0x100000000
        tickmsg = arg3
        if tickmsg > 0x80000000:
            tickmsg -= 0x100000000
        
        size = arg4
        if filter_value == self.RT_TRACE_RING_TX_MSG:
            out = " SEND : ring %02x / time=%6d msg tick=%d size=%04d"%(ring, ltime,tickmsg, size)
        elif filter_value == self.RT_TRACE_RING_TX_WAIT:
            out = " WAIT : ring %02x / time=%6d msg tick=%d size=%04d"%(ring, ltime,tickmsg, size)
        elif filter_value == self.RT_TRACE_RING_TX_FREEZE:
            out = " FRZE : ring %02x / time=%6d msg tick=%d size=%04d"%(ring, ltime,tickmsg, size)
        elif filter_value == self.RT_TRACE_RING_TX_FULL:
            out = " FULL : ring %02x / time=%6d rd_offset=%08x "%(ring, ltime,arg3)
        else:
            out = " IDLE"
        return out
        
class RttraceBufferFpgaTx(RttraceBuffer):

    RT_TRACE_TX_DBELL     = 0x40
    CB_BUFFER_ID   = 0
    ARI_BUFFER_ID  = 1
    CQI_BUFFER_ID  = 2
    DBG_BUFFER_ID  = 3
    ALB_BUFFER_ID  = 4
    RCMD_BUFFER_ID = 5
    RT_TRACE_BUFFER_NAME = ['CB_BUFFER', 'ARI_BUFFER', 'CQI_BUFFER', 'DBG_BUFFER', 'ALB_BUFFER','RCMD_BUFFER'] 

       
    NBWORD =8

    def __init__(self, rpc, bname, env=[None, None, None], parent=None):
        super(RttraceBufferFpgaTx,self).__init__(rpc=rpc, bname=bname, env=env, parent=parent)

    def __delta_time_1_(self, nsec):
        if getattr(self, "__delta__",None) == None:
            self.__delta__ = { 'nsec' : nsec }

        delta_nsec = nsec - self.__delta__['nsec']
        if delta_nsec < 0:
            delta_nsec += 1000000000
        
        out = "%10d ns "%(delta_nsec)
        
        self.__delta__ = { 'nsec' : nsec }
        return out

    def __parse_info__(self, data=[]):
        out  = self.__delta_time__(data[0], data[1])
        out += self.__arg__( data[2], data[3], data[4], data[5], data[6], data[7])
        
        return out
        
    def __arg__(self, arg1, arg2, arg3, arg4, arg5, arg6):
        cmd          = (arg1 >> 24) & 0x7F
        bufferid     =  (arg1 >> 16) & 0xFF
        filter_value = arg1&0xFFFFFF
        ltime        = arg3
        if ltime > 0x80000000:
            ltime -= 0x100000000
        tickmsg = arg2&0xFFFF
        if tickmsg > 0x8000:
            tickmsg -= 0x10000
        
        if filter_value == self.RT_TRACE_TX_DBELL:
            info = arg5 & 0xFF
            out = " ltime=%8d tick=%d DBEL : cmd %02x /  info =%04x"%(ltime,tickmsg, cmd,  info)
        else:
            bname        = self.RT_TRACE_BUFFER_NAME[bufferid]
            # M_FPGA_TX_TRACE(FPGA_TX,RT_TRACE_TX_SEND,RT_TRACE_TX_SEND, L_ptsFpgaBufferHandle->scn.offset, L_ptsFpgaToDspMsg->tick,L_ptsFpgaToDspMsg->size );
            out = " ltime=%8d tick=%d SEND : cmd %02x %s /"%(ltime,tickmsg, cmd, bname)
            out += " Addr = %08x ( base = %08x -offset=%08x )"%(arg5,arg6,arg5-arg6)
        return out