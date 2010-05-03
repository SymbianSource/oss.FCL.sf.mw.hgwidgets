/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*  Version     : %version: 1 %
*/
#ifndef HGLOGGER_H_
#define HGLOGGER_H_

//To enable tracking define ENABLETRACE
//if TRACE_FILENAME is set then trace will go to file, otherwise to RDebug

#include <QObject>
#include <qglobal.h>

#define XQCONNECT_ASSERT(a, b, c, d) if (!(QObject::connect((a),(b), (c), (d)))) { qFatal("Connection failed: connect(%s, %s, %s, %s)", #a, #b, #c, #d); }

// stolen from qt-music :)
#ifdef ENABLETRACE
    #include <QString>
    #include <QDebug>
    #include <QTime>
    #include <stdio.h>
    #include <stdlib.h>
    #include <e32debug.h>

    #ifdef TRACE_FILE
        #define _TRACE_FILENAME "c:/trace.txt"
        #define _TX_INIT void __tx_myMessageOutput(QtMsgType type, const char *msg) {\
             static const QString timeFmt("hh:mm:ss.zzz");\
             FILE *f = fopen(_TRACE_FILENAME, "a");\
             fprintf(f, "%s ", QTime::currentTime().toString(timeFmt).toLatin1().data() );\
             switch (type) {\
             case QtDebugMsg: fprintf(f, "[DEB] [TX] %s\n", msg); break;\
             case QtWarningMsg: fprintf(f, "[WRN] [TX] %s\n", msg); break;\
             case QtCriticalMsg: fprintf(f, "[CRT] [TX] %s\n", msg); break;\
             case QtFatalMsg: fprintf(f, "[FTL] [TX] %s\n", msg); fclose(f); abort();\
             } fclose(f);\
        }
    #else
        #define _TX_INIT void __tx_myMessageOutput(QtMsgType /*type*/, const char *msg) {\
            RDebug::Printf("[TX] %s", msg);\
            }
    #endif // TRACE_FILE

    #define _TX_INSTALL qInstallMsgHandler(__tx_myMessageOutput);
    #define TX_MAIN(a, b) _TX_INIT \
            int __tx__main(int, char**); int main(int (a), char **(b)) { _TX_INSTALL return __tx__main(a, b); } int __tx__main(int (a), char **(b))

    #define TX_UNUSED(name);
    #define TX_STATIC_ENTRY qDebug() << __PRETTY_FUNCTION__  << "entry";
    #define TX_STATIC_ENTRY_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << "entry," << args;
    #define TX_STATIC_EXIT qDebug() << __PRETTY_FUNCTION__ << "exit";
    #define TX_STATIC_EXIT_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << "exit," << args; 
    #define TX_ENTRY qDebug() << __PRETTY_FUNCTION__ << "this" << (void *)this << "entry";
    #define TX_ENTRY_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << "this" << (void *)this << "entry," << args;
    #define TX_EXIT qDebug() << __PRETTY_FUNCTION__ << "exit";
    #define TX_EXIT_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << "exit," << args;
    #define TX_LOG qDebug() << __PRETTY_FUNCTION__ << "this" << (void *)this;
    #define TX_LOG_ARGS(args) qDebug() << __PRETTY_FUNCTION__ << args;
#else
    #define TX_MAIN(a,b) int main(int (a), char **(b))
    #define TX_UNUSED(name) Q_UNUSED(name);
    #define TX_STATIC_ENTRY ;
    #define TX_STATIC_ENTRY_ARGS(args) ;
    #define TX_STATIC_EXIT ;
    #define TX_STATIC_EXIT_ARGS(args) ;
    #define TX_ENTRY ;
    #define TX_ENTRY_ARGS(args) ;
    #define TX_EXIT ;
    #define TX_EXIT_ARGS(args) ;
    #define TX_LOG ;
    #define TX_LOG_ARGS(args) ;
#endif // ENABLETRACE


#endif // HGLOGGER_H_
