/* 
 *
 *
 * Copyright (C) 2003-2008 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2008 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#ifndef _K3B_EXTERNAL_ENCODER_H_
#define _K3B_EXTERNAL_ENCODER_H_

#include <k3baudioencoder.h>

#include <qprocess.h>

class K3bExternalEncoder : public K3b::AudioEncoder
{
    Q_OBJECT

public:
    K3bExternalEncoder( QObject* parent, const QVariantList& );
    ~K3bExternalEncoder();

    QStringList extensions() const;
  
    QString fileTypeComment( const QString& ) const;

    int pluginSystemVersion() const { return K3B_PLUGIN_SYSTEM_VERSION; }

    /**
     * reimplemented since the external program is intended to write the file
     * TODO: allow writing to stdout.
     */
    bool openFile( const QString& ext, const QString& filename, const K3b::Msf& length );
    void closeFile();

    
private Q_SLOTS:
    void slotExternalProgramFinished( int, QProcess::ExitStatus );
    void slotExternalProgramOutputLine( const QString& );

private:
    void finishEncoderInternal();
    bool initEncoderInternal( const QString& extension );
    long encodeInternal( const char* data, Q_ULONG len );
    void setMetaDataInternal( MetaDataField, const QString& );
    bool writeWaveHeader();

    class Private;
    Private* d;
};

#endif
