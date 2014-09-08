#!/bin/bash

CDIR=`pwd` 
LOCAL_PLT=`uname -m`

TARGET=${LOCAL_PLT}
BUILD_PATH="${CDIR}/build"
BUILD_LIB_PATH="${BUILD_PATH}/${TARGET}/lib"
BUILD_INC_PATH="${BUILD_PATH}/${TARGET}/include"
INSTALL_PATH="${BUILD_PATH}/${TARGET}"

function build_local_xmlrpc {
	XMLRPC_NAME="xmlrpc-c-stable"

    INSTALL_PATH=${PACKAGE_DIR}/../build/${LOCAL_PLT}
    # build localy the xml-rpc due to non compatibility with cross compile environment
    if [ -d ${LOCAL_BUILD_PATH} ]; then
        printf " Folder ${LOCAL_BUILD_PATH} exist \n CLEANUP\n=======================\n"
        #rm -rf ${LOCAL_BUILD_PATH}
    fi
    mkdir -pv ${LOCAL_BUILD_PATH}
    mkdir -pv ${INSTALL_PATH}
    cd ${LOCAL_BUILD_PATH}
    printf " Untar\n"
    tar xfz ${PACKAGE_DIR}/${XMLRPC_NAME}.tgz
    mv stable ${XMLRPC_NAME}
    cd ${XMLRPC_NAME}
    printf " configure\n"
    ./configure  --enable-tools --disable-curl-client --prefix=${INSTALL_PATH} 
    cd lib/expat/gennmtab/
    make clean && make gennmtab
    cp gennmtab ${LOCAL_BUILD_PATH}
    cd ../../..
    make CADD=-fPIC
    find . -name "*.so" -print
    make install    
    cd ${CDIR}
}

#---------------------- help ----------------------
#
# Procedure help()
#
# Objet : display the help
#
help()
{
   #clear
   printf "\nNAME\n\n"
   printf "     %s - Compile illusoo program and xmlrpc-c library \n\n" $0 
   printf "SYNOPSYS\n\n"
   printf "     %s [-h :help] [-x] [-c] [-b] [-t] [-g] [-s] [-l]\n"
   printf "\nDESCRIPTION\n\n"
   printf "\nOPTIONS\n\n"
   printf "     -x    : build the xmlrpc library for local target\n"
   printf "     -c    : clean illusoo\n"
   printf "     -b    : build illusoo\n"
   printf "     -t    : build illusoo with __TEST flag\n"
   printf "     -g    : execute GPIO config with sudo ( beaglebone case only)\n"
   printf "     -s    : execute illusoo with sudo ( beaglebone case only)\n"
   printf "     -l    : execute illusoo\n"
   printf "\nEXAMPLE\n\n"
   printf "\nEXIT STATUS\n\n"
   printf "     0 Successful completion\n"
   printf "\nIDENTIFICATION\n\n"
   printf "$0 : "                  
   printf "$RELEASED_VERSION\n";  

   return
}

#---------------------- help ----------------------
#
# Procedure xmlrpc_build()
#
# Objet : build xmlrpc package 
#
xmlrpc_build()
{
	cd package/
    PACKAGE_DIR=`pwd`
	LOCAL_BUILD_PATH=${CDIR}/package/${LOCAL_PLT}
	echo $PACKAGE_DIR
	echo $LOCAL_BUILD_PATH
	build_local_xmlrpc
	cd ${CDIR}
}


test_script()
{
   echo "#!/bin/bash


list_IO=( 23 24 25 26 27 28 29 75 76 77 78 )

for i in ${list_IO[*]} ; do
    echo $i
    folder="/tmp/gpio/gpio${i}/"
    if [ ! -d $folder ]; then
       mkdir -pv $folder
       echo "1" > $folder/value
       echo "1" > $folder/direction
    fi
done"> ./test.sh
chmod u+x test.sh
}

#---------------------- help ----------------------
#
# Procedure socketlib()
#
# Objet : build xmlrpc package 
#
socketlib_build()
{
	cd src/socketlib/
    make clean all
    cp libsocket.so $BUILD_LIB_PATH
    cp *.h $BUILD_INC_PATH
	cd ${CDIR}
}



main=$0
XMLRPC=0
MAKE=0
TEST=0
GPIO=0
LOAD=0
LSUDO=0
CLEAN=0

if [[ "${main}" == "./illusoo.sh" ]]; then
    CDIR=`pwd`

    NO_ARGS=0 
    E_OPTERROR=85

    if [ $# -eq "$NO_ARGS" ]    # Script invoked with no command-line args?
    then
      help
      exit $E_OPTERROR          # Exit and explain usage.
                                # Usage: scriptname -options
                                # Note: dash (-) necessary
    fi  
    while getopts "xbgtslc" Option
    do
      case $Option in
        h     ) help; exit 0;;
        x     ) printf "BUILD XML-RPC LIBRARY\n===========================\n"; XMLRPC=1 ;;
        c     ) printf "CLEAN ILLUSOO\n===========================\n"; CLEAN=1 ;;
        b     ) printf "BUILD ILLUSOO\n===========================\n"; MAKE=1 ;;
        t     ) printf "BUILD ILLUSOO TEST mode\n===========================\n"; MAKE=1; TEST=1 ; test_script ;;
        g     ) printf "load ILLUSOO GPIO config\n===========================\n"; GPIO=1 ;;
        s     ) printf "load ILLUSOO in sudoers mode\n===========================\n"; LOAD=1; LSUDO=1 ;;
        l     ) printf "load ILLUSOO\n===========================\n"; LOAD=1 ;;
        *     ) printf "Unimplemented option chosen.\n"; help; exit 1;;   # Default.
      esac
    done
fi

if [[ $XMLRPC -eq 1 ]]; then
	xmlrpc_build
fi 

if [[ $MAKE -eq 1 ]]; then
    socketlib_build
    export LD_LIBRARY_PATH=${BUILD_LIB_PATH}
    if [[ $CLEAN -eq 1 ]]; then
            make clean 
    fi
    make all TEST=${TEST}
    echo ""
    echo "================================================"
    echo " PLEASE update LD_LIBRARY_PATH as following to map xmlrpc-c library:"
    echo " LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${BUILD_LIB_PATH}"
    echo "================================================"
fi

if [[ $GPIO -eq 1 ]]; then
	CDIR=`pwd`
	cd config
	pwd
	sudo ./config.sh
	cd $CDIR
fi

if [[ $LOAD -eq 1 ]]; then
   
   	echo "'${LSUDO}'"
	if [[ $LSUDO -eq 1 ]]; then
		echo " execute in SUDO mode "
        	sudo LD_LIBRARY_PATH=${BUILD_LIB_PATH}  ./illustrabot2srv -i config/illusoo.ini
   	else
		LD_LIBRARY_PATH=${BUILD_LIB_PATH}  ./illustrabot2srv -i config/illusoo.ini
   	fi
fi 
