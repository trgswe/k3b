/*
 *
 * Copyright (C) 2003-2009 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2009 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#ifndef _K3B_DVD_FORMATTING_JOB_H_
#define _K3B_DVD_FORMATTING_JOB_H_


#include <qprocess.h>
#include <k3bjob.h>
#include "k3b_export.h"

namespace K3b {
    namespace Device {
        class Device;
        class DeviceHandler;
    }

    class LIBK3B_EXPORT DvdFormattingJob : public BurnJob
    {
        Q_OBJECT

    public:
        DvdFormattingJob( JobHandler*, QObject* parent = 0 );
        ~DvdFormattingJob();

        QString jobDescription() const;
        QString jobDetails() const;

        Device::Device* writer() const;

    public Q_SLOTS:
        void start();

        /**
         * Use this to force the start of the formatting without checking for a usable medium.
         */
        void start( const Device::DiskInfo& );

        void cancel();

        void setDevice( Device::Device* );

        /**
         * One of: WRITING_MODE_INCR_SEQ, WRITING_MODE_RES_OVWR
         * Ignored for DVD+RW
         */
        void setMode( int );

        /**
         * Not all writers support this
         */
        void setQuickFormat( bool );

        /**
         * @param b If true empty DVDs will also be formatted
         */
        void setForce( bool b );

        /**
         * If set true the job ignores the global K3b setting
         * and does not eject the CD-RW after finishing
         */
        void setForceNoEject( bool );

    private Q_SLOTS:
        void slotStderrLine( const QString& );
        void slotProcessFinished( int, QProcess::ExitStatus );
        void slotDeviceHandlerFinished( Device::DeviceHandler* );
        void slotEjectingFinished( Device::DeviceHandler* );

    private:
        void startFormatting( const Device::DiskInfo& );

        class Private;
        Private* d;
    };
}


#endif
