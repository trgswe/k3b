/* 
 *
 * $Id$
 * Copyright (C) 2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */


#ifndef K3BDATADOC_H
#define K3BDATADOC_H

#include "../k3bdoc.h"

#include "../k3bisooptions.h"

#include <qptrlist.h>
#include <qfileinfo.h>
#include <qstringlist.h>
#include <qptrlist.h>

#include <kurl.h>
#include <kio/global.h>

class K3bDataItem;
class K3bRootItem;
class K3bDirItem;
class K3bFileItem;
class K3bJob;
class K3bBootItem;

class KProgressDialog;
class K3bView;
class QString;
class QStringList;
class QWidget;
class QDomDocument;
class QDomElement;



/**
 *@author Sebastian Trueg
 */

class K3bDataDoc : public K3bDoc
{
  Q_OBJECT

 public:
  K3bDataDoc( QObject* parent );
  virtual ~K3bDataDoc();

  virtual int docType() const { return DATA; }

  enum mutiSessionModes { NONE, START, CONTINUE, FINISH };

  K3bRootItem* root() const { return m_root; }

  /** reimplemented from K3bDoc */
  virtual K3bView* newView( QWidget* parent );
  /** reimplemented from K3bDoc */
  void addView(K3bView* view);

  virtual bool newDocument();
  virtual unsigned long long size() const;
  virtual unsigned long long length() const;

  const QString& name() const { return m_name; }

  void removeItem( K3bDataItem* item );
  void moveItem( K3bDataItem* item, K3bDirItem* newParent );
  void moveItems( QList<K3bDataItem> itemList, K3bDirItem* newParent );

  K3bDirItem* addEmptyDir( const QString& name, K3bDirItem* parent );
	
  /** writes a mkisofs-path-spec file with graft-points **/
  QString writePathSpec( const QString& fileName );

  QString treatWhitespace( const QString& );
	
  /** returns an empty dummy dir for use with K3bDirItems.
      Creates one if nessessary.
      The dummy dir is used to create empty dirs on the iso-filesystem! 
      TODO: should be moved to K3bMainWidget or K3bApp (if we ever create and need one) */
  const QString& dummyDir();

  const QString& isoImage() const { return m_isoImage; }
  void setIsoImage( const QString& s ) { m_isoImage = s; }
	
  virtual K3bBurnJob* newBurnJob();
	
  bool deleteImage() const { return m_deleteImage; }
  bool onlyCreateImage() const { return m_onlyCreateImage; }


  void setDeleteImage( bool b ) { m_deleteImage = b; }
  void setOnlyCreateImage( bool b ) { m_onlyCreateImage = b; }

  int multiSessionMode() const { return m_multisessionMode; }
  void setMultiSessionMode( int mode );

  static bool nameAlreadyInDir( const QString&, K3bDirItem* );

  K3bIsoOptions& isoOptions() { return m_isoOptions; }

  const QPtrList<K3bBootItem>& bootImages() { return m_bootImages; }
  K3bDataItem* bootCataloge() { return m_bootCataloge; }

  K3bDirItem* bootImageDir();
  K3bFileItem* createBootItem( const QString& filename );
  /** this will just remove it from the list of boot items, not remove it from the doc */
  void removeBootItem( K3bBootItem* );

 public slots:
  /** add urls to the compilation.
   * @param dir the directory where to add the urls, by default this is the root directory.
   **/
  void slotAddUrlsToDir( const KURL::List&, K3bDirItem* dir = 0 );
  virtual void addUrl( const KURL& url );
  virtual void addUrls( const KURL::List& urls );

  void importSession( const QString& path );
  void clearImportedSession();

 signals:
  void itemRemoved( K3bDataItem* );
  void newFileItems();
	
 private slots:
  void slotAddQueuedItems();

 protected:
  /** reimplemented from K3bDoc */
  virtual bool loadDocumentData( QDomElement* root );
  /** reimplemented from K3bDoc */
  virtual bool saveDocumentData( QDomElement* );

  virtual QString documentType() const;

  void loadDefaultSettings();

 private:
  void createSessionImportItems( const QString& path, K3bDirItem* parent, KProgressDialog* );
  K3bDataItem* createBootCatalogeItem();

  /**
   * load recursivly
   */
  bool loadDataItem( QDomElement& e, K3bDirItem* parent );
  /**
   * save recursivly
   */
  void saveDataItem( K3bDataItem* item, QDomDocument* doc, QDomElement* parent );

  K3bDirItem* createDirItem( QFileInfo& f, K3bDirItem* parent );
  K3bFileItem* createFileItem( QFileInfo& f, K3bDirItem* parent );

  void informAboutNotFoundFiles();

  class PrivateItemToAdd {
  public:
    PrivateItemToAdd( const QString& p, K3bDirItem* i )
      :fileInfo(p) { parent = i; }
    PrivateItemToAdd( const QFileInfo& f, K3bDirItem* i )
      :fileInfo(f) { parent = i; }
    QFileInfo fileInfo;
    K3bDirItem* parent;
  };

  QPtrList<PrivateItemToAdd> m_queuedToAddItems;
  QTimer* m_queuedToAddItemsTimer;
  int m_numberAddedItems;

  QStringList m_notFoundFiles;


  // mkisofs seems to have a bug that prevents us to use filenames 
  // that contain one or more backslashes
  // -----------------------------------------------------------------------
  QStringList m_mkisofsBuggyFiles;
  // -----------------------------------------------------------------------


  K3bRootItem* m_root;
  QString m_name;
  QString m_dummyDir;
  QString m_isoImage;

  bool m_deleteImage;
  bool m_onlyCreateImage;

  KIO::filesize_t m_size;
		
  K3bIsoOptions m_isoOptions;

  int m_multisessionMode;
  QPtrList<K3bDataItem> m_oldSession;

  // boot cd stuff
  K3bDataItem* m_bootCataloge;
  QPtrList<K3bBootItem> m_bootImages;

  friend class K3bMixedDoc;
};

#endif
