/*
 *
 * $Id$
 * Copyright (C) 2003-2007 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2007 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#ifndef _K3B_PROJECT_MANAGER_H_
#define _K3B_PROJECT_MANAGER_H_

#include <qobject.h>
#include <qlist.h>
#include <k3bdoc.h>


class KUrl;

namespace K3b {

    class ProjectInterface;

    class ProjectManager : public QObject
    {
        Q_OBJECT

    public:
        ProjectManager( QObject* parent = 0 );
        virtual ~ProjectManager();

        QList<Doc*> projects() const;

        /**
         * Create a new project including loading user defaults and creating
         * the dcop interface.
         */
        Doc* createProject( Doc::DocType type );

        /**
         * Opens a K3b project.
         * \return 0 if url does not point to a valid k3b project file, the new project otherwise.
         */
        Doc* openProject( const KUrl &url );

        /**
         * saves the document under filename and format.
         */
        bool saveProject( Doc*, const KUrl &url );

        Doc* activeDoc() const { return activeProject(); }
        Doc* activeProject() const;
        Doc* findByUrl( const KUrl& url );
        bool isEmpty() const;

        /**
         * Will create if none exists.
         */
        //ProjectInterface* dcopInterface( Doc* doc );

    public Q_SLOTS:
        void addProject( Doc* );
        void removeProject( Doc* );
        void setActive( Doc* );
        void loadDefaults( Doc* );

    Q_SIGNALS:
        void newProject( Doc* );
        void projectSaved( Doc* );
        void closingProject( Doc* );
        void projectChanged( Doc* doc );
        void activeProjectChanged( Doc* );

    private Q_SLOTS:
        void slotProjectChanged( Doc* doc );

    private:
        // used internal
        Doc* createEmptyProject( Doc::DocType );

        class Private;
        Private* d;
    };
}

#endif
