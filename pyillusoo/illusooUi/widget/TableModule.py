#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@date: Created on 2013/02/04
@author: bideau1
"""
from PySide import QtGui
from CheckBoxGroup import CheckBoxGroup, CheckBoxGroupMerge


    

class TableModule(QtGui.QWidget):

    TABLE_NAME=''
    TABLE_COLUMN = []
    TABLE_COLUMN_NATURE = { }
    XML_KEY=''

    def __init__(self, parent=None):
        super(TableModule, self).__init__(parent)
        self.name = self.TABLE_NAME

    def display(self):         
        layout = QtGui.QGridLayout()

        self.__table__()
        self.__buttons__()

        # map layout
        layout.addWidget(self.table, 0, 0)
        layout.addLayout(self.buttonLayout, 0, 1)

        #connection
        self.AddButton.clicked.connect(self.appendRow)
        self.RmButton.clicked.connect(self.removeRow)
        self.ResizeButton.clicked.connect(self.refresh)

        self.table.itemClicked.connect(self.setRmButton)
        self.setLayout(layout)

    def __table__(self):
        self.table = QtGui.QTableWidget()
        self.table.setColumnCount(len(self.TABLE_COLUMN))
        self.table.setHorizontalHeaderLabels(self.TABLE_COLUMN)
        # Do not insert row ( default empty )
        self.table.setRowCount(0)

        

    def __buttons__(self):
        # insert button(s)
        self.buttonLayout=QtGui.QGridLayout()
        self.AddButton = QtGui.QPushButton("&Add")
        self.AddButton.show()
        self.RmButton = QtGui.QPushButton("&Remove")
        self.RmButton.show()
        self.ResizeButton = QtGui.QPushButton("re&Size")
        self.ResizeButton.show()
        
        self.buttonLayout.addWidget(self.AddButton, 0, 0)
        self.buttonLayout.addWidget(self.RmButton, 1, 0)
        self.buttonLayout.addWidget(self.ResizeButton, 2, 0)
        

    def clear(self):
        # suppress table row
        print 'remove table row', self.table.rowCount()
        for i in range(self.table.rowCount())[::-1]:
            self.table.removeRow(i)
        return

    def setEnabled(self,state=True):
        #button
        self.AddButton.setEnabled(state)
        self.RmButton.setEnabled(state)
        self.ResizeButton.setEnabled(state)
        # table
        self.table.setEnabled(state)
    
    def setValues(self, rowid, attr=[]):
        for i in range(len(attr)):
            k,v = attr[i]
            try:
                colid = self.TABLE_COLUMN.index(k)
                nature = self.TABLE_COLUMN_NATURE[k]['type']
                if nature in [CheckBoxGroup]:
                    cell= self.table.cellWidget(rowid, colid)
                    cell.setValues(v)
                elif nature in [QtGui.QCheckBox]:
                    cell= self.table.cellWidget(rowid, colid)
                    if (bool(int(v)) and not cell.isChecked()) or (not bool(int(v)) and cell.isChecked()):
                        cell.click()
                elif nature in [QtGui.QTableWidgetItem]:
                    item = self.table.item(rowid, colid)
                    item.setText(v)
            except Exception,e:
                print k,v,colid,nature
                print 'Warning [%s]: Unsupported field "%s" with value = "%s"'%(self.XML_KEY,str(k),str(v))
                pass
        return

    def appendRow(self,attr=[]):
        rowid = self.table.rowCount()
        self.table.setRowCount(rowid+1)        
        #build element
        for k in self.TABLE_COLUMN:
            kidx = self.TABLE_COLUMN.index(k)
            element = self.TABLE_COLUMN_NATURE[k]['type']
            if self.TABLE_COLUMN_NATURE[k]['type'] in [CheckBoxGroup]:
                names=self.TABLE_COLUMN_NATURE[k]['names']
                default=self.TABLE_COLUMN_NATURE[k]['default']
                values = [(names[i],default[i]) for i in range(len(names))]
                element = self.TABLE_COLUMN_NATURE[k]['type'](names=names)
                element.setValues(values)
                self.table.setCellWidget(rowid,kidx,element)

            elif self.TABLE_COLUMN_NATURE[k]['type'] in [CheckBoxGroupMerge]:
                names=self.TABLE_COLUMN_NATURE[k]['names']
                default=self.TABLE_COLUMN_NATURE[k]['default']
                attrName=self.TABLE_COLUMN_NATURE[k]['attrName']
                values = [(names[i],default[i]) for i in range(len(names))]
                element = self.TABLE_COLUMN_NATURE[k]['type'](names=names,mergeAttrName=attrName)
                element.setValues(values)
                self.table.setCellWidget(rowid,kidx,element)

            elif self.TABLE_COLUMN_NATURE[k]['type'] in [QtGui.QCheckBox]:
                element = self.TABLE_COLUMN_NATURE[k]['type']()
                value=self.TABLE_COLUMN_NATURE[k]['default']
                element.setChecked(bool(int(value)))
                self.table.setCellWidget(rowid,kidx,element)
            elif self.TABLE_COLUMN_NATURE[k]['type'] in [QtGui.QTableWidgetItem]:
                value=self.TABLE_COLUMN_NATURE[k]['default']
                element = self.TABLE_COLUMN_NATURE[k]['type'](value)
                self.table.setItem(rowid, kidx, element)

            else:
                raise Exception('type <%> not support yet'%type(self.TABLE_COLUMN_NATURE[k]['type']))

        # update attribute
        self.setValues(rowid,attr)
        # Also optional. Will fit the cells to its contents.
        self.refresh()
        return rowid

    def getValues(self):
        """ @return list of tuple (chkboxname, value) """
        return [self.__getrowInfo__(rowid).items() for rowid in range(self.table.rowCount())]

    def fromXml(self, parent):

        child= parent.firstChildElement()
        #~ print 'TableModule.fromXml child.tagName()=', child.tagName()
        while not child.isNull():
            #~ print 'input TableModule.fromXml', child.tagName()
            if child.tagName() == self.XML_KEY:
                # create empty row before to append
                rowid = self.appendRow()
                # update attr
                attr=[]
                for name in self.TABLE_COLUMN:
                    kidx = self.TABLE_COLUMN.index(name)
                    #~ print kidx, name
                    if self.TABLE_COLUMN_NATURE[name]['type'] in [CheckBoxGroup, CheckBoxGroupMerge]:
                        widget = self.table.cellWidget(rowid,kidx)
                        self.__fromXmlChild__(widget, child,name)
                    else:
                        attr.append((name,child.attribute(name)))
                #~ print 'fromXml ', child.tagName(),attr
                self.setValues(rowid,attr)
            child= child.nextSiblingElement()
            #~ print 'next TableModule.fromXml', child.tagName()
        return

    def __fromXmlChild__(self,widget,parent,name):
        child=parent.firstChildElement(name)
        if not child.isNull(): 
            #~ print '__fromXmlChild__ child(%s) = '%(name),child.tagName() 
            widget.fromXml(child)
        

    def toXml(self, doc, father):
       
        for data in self.getValues():
            elt=doc.createElement(self.XML_KEY)
            # update attribute
            for k,v in data:
                if type(v)==list:
                    subelt=doc.createElement(k)
                    print v
                    if len(v) >0:
                        for ks,vs in v:
                            subelt.setAttribute(ks, vs)
                    elt.appendChild(subelt)
                else:
                    elt.setAttribute(k, v)
           
            father.appendChild(elt)
        return 

    def __getrowInfo__(self, rowid):
        data={}
        for j in range(len(self.TABLE_COLUMN)):
            k = self.TABLE_COLUMN[j]
            if self.TABLE_COLUMN_NATURE[k]['type'] in [CheckBoxGroup,CheckBoxGroupMerge]:
                element=self.table.cellWidget(rowid,j)
                if element != None:
                    data[k]=element.getValues()
            elif self.TABLE_COLUMN_NATURE[k]['type'] in [QtGui.QCheckBox]:
                data[k]=self.table.cellWidget(rowid,j).isChecked()
            elif self.TABLE_COLUMN_NATURE[k]['type'] in [QtGui.QTableWidgetItem]:
                data[k]=self.table.item(rowid,j).text()
        return data

    def addNewRow(self,param={}):
        nbrow = self.table.rowCount()
        self.table.setRowCount(nbrow+1)

        #build element
        i=0
        for k in self.TABLE_COLUMN:
            element = self.TABLE_COLUMN_NATURE[k]['type']
            if self.TABLE_COLUMN_NATURE[k]['type'] in [CheckBoxGroup]:
                values=self.TABLE_COLUMN_NATURE[k]['values']
                element = self.TABLE_COLUMN_NATURE[k]['type'](param=values)
                # set value
                if type(param[k])== list:
                    element.update(param[k])

                self.table.setCellWidget(nbrow,i,element)
            elif self.TABLE_COLUMN_NATURE[k]['type'] in [QtGui.QCheckBox]:
                element = self.TABLE_COLUMN_NATURE[k]['type']()
                value=self.TABLE_COLUMN_NATURE[k]['default']
                if k in param:
                    value = param[k]
                element.setChecked(bool(value))
                self.table.setCellWidget(nbrow,i,element)
            elif self.TABLE_COLUMN_NATURE[k]['type'] in [QtGui.QTableWidgetItem]:
                value=self.TABLE_COLUMN_NATURE[k]['default']
                if k in param:
                    value = param[k]
                element = self.TABLE_COLUMN_NATURE[k]['type'](value)
                self.table.setItem(nbrow, i, element)
            else:
                raise Exception('type <%> not support yet'%type(self.TABLE_COLUMN_NATURE[k]['type']))
            i+=1
        # Also optional. Will fit the cells to its contents.
        self.refresh()

    def setRmButton(self,item):
        self.RmButton.setEnabled(True)
        return
    
    def removeRow(self):

        row= self.table.currentRow()
        if row >=0:
            name =self.table.item(row,0).text()
            
            button = QtGui.QMessageBox.question(self, "Confirm Remove",
                        "Are you sure you want to remove \"%s\"?" % name,
                        QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
    
            if button == QtGui.QMessageBox.Yes:
                self.table.removeRow(row)
    
                QtGui.QMessageBox.information(self, "Remove Successful",
                            "\"%s\" has been removed from the table." % name)
        else:
                QtGui.QMessageBox.information(self, "Can't Remove", 
                                              "To remove a row, you must select one")                                              


        return

    def refresh(self):
        self.table.resizeColumnsToContents()
        self.table.resizeRowsToContents()


if __name__ == "__main__":

    from PySide import QtXml 
    import sys

    class TableModuleTest(TableModule):
        TABLE_NAME='TableLayoutCl'
        TABLE_COLUMN = ['name', 'capable', 'description', 'Show', 'Store']
        TABLE_COLUMN_NATURE = { 'name' : { 'type' :QtGui.QTableWidgetItem, 'default' : 'new ioctl' },
                                'capable' : { 'type' :QtGui.QCheckBox, 'default' : True },
                                'description' : { 'type' :QtGui.QTableWidgetItem, 'default' : 'insert comment' },
                                'Show' : { 'type' :CheckBoxGroup, 'names' : ['U', 'G', 'O'], 'default' : [False, False, False] },
                                'Store' : { 'type' :CheckBoxGroup, 'names' : ['U', 'G', 'O'], 'default' : [False, False, False] },
                              }
        XML_KEY='IoctlMethod'

    app = QtGui.QApplication(sys.argv)
    mainWin = TableModuleTest()
    mainWin.resize(820, 640)


    doc = QtXml.QDomDocument("MyML")
    root = doc.createElement("Ioctl")
    doc.appendChild(root)
    
    elt = doc.createElement("IoctlMethod")
    elt.setAttribute('name','SET_OUTWIN')
    elt.setAttribute('capable',True)
    elt.setAttribute('description',"set outbound SRIO windows to DSP")
    root.appendChild(elt)
    elt2 = doc.createElement("IoctlMethod")
    elt2.setAttribute('name','GET_OUTWIN')
    elt2.setAttribute('capable',True)
    elt2.setAttribute('description',"get outbound SRIO windows to DSP")
    root.appendChild(elt2)
    elt3 = doc.createElement("IoctlMethod")
    elt3.setAttribute('name','GET_INWIN')
    elt3.setAttribute('capable',False)
    elt3.setAttribute('description',"get inbound SRIO windows to DSP")
    root.appendChild(elt3)    
    xml = doc.toString()
    print xml

    info = mainWin.getValues()

    mainWin.show()
    mainWin.fromXml(root)

    info = mainWin.getValues()
    print info
    k,v = info[0][1]
    info[0][1] = (k,'UNKNOWN')
    print info
    mainWin.set(0,info[0])
    
    newdoc = QtXml.QDomDocument("MyNewDoc")
    newroot = newdoc.createElement("Ioctl")
    mainWin.toXml(newdoc,newroot)
    newdoc.appendChild(newroot)   
    newres = newdoc.toString()
    print newres
    
    sys.exit(app.exec_())
