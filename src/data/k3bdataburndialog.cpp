/***************************************************************************
                          k3bdataburndialog.cpp  -  description
                             -------------------
    begin                : Wed May 16 2001
    copyright            : (C) 2001 by Sebastian Trueg
    email                : trueg@informatik.uni-freiburg.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "k3bdataburndialog.h"
#include "k3bdatadoc.h"
#include "../k3b.h"
#include "../device/k3bdevice.h"
#include "../k3bwriterselectionwidget.h"
#include "../k3btempdirselectionwidget.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qpoint.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qfileinfo.h>
#include <qvalidator.h>
#include <qregexp.h>

#include <kmessagebox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kfiledialog.h>



K3bDataBurnDialog::K3bDataBurnDialog(K3bDataDoc* _doc, QWidget *parent, const char *name, bool modal )
  : K3bProjectBurnDialog( _doc, parent, name, modal )
{
  setupBurnTab( addPage( i18n("Burning") ) );
  setupSettingsTab( addPage( i18n("Settings") ) );
  setupMultisessionTab( addPage( i18n("Multisession") ) );
  setupAdvancedTab( addPage( i18n("Advanced") ) );
	
  readSettings();
  slotLoadPreSettings( i18n("K3b Default" ) );

  if( K3bDevice* dev = m_writerSelectionWidget->writerDevice() )
    m_checkBurnProof->setEnabled( dev->burnproof() );

  QFileInfo fi( m_tempDirSelectionWidget->tempPath() );
  if( fi.isFile() )
    m_tempDirSelectionWidget->setTempPath( fi.dirPath() + "image.iso" );
  else
    m_tempDirSelectionWidget->setTempPath( fi.filePath() + "image.iso" );

  m_tempDirSelectionWidget->setNeededSize( doc()->size() );
}

K3bDataBurnDialog::~K3bDataBurnDialog(){
}


void K3bDataBurnDialog::saveSettings()
{
  doc()->setDao( m_checkDao->isChecked() );
  doc()->setDummy( m_checkDummy->isChecked() );
  doc()->setOnTheFly( m_checkOnTheFly->isChecked() );
  doc()->setBurnproof( m_checkBurnProof->isChecked() );
  ((K3bDataDoc*)doc())->setOnlyCreateImage( m_checkOnlyCreateImage->isChecked() );
  ((K3bDataDoc*)doc())->setDeleteImage( m_checkDeleteImage->isChecked() );
			
  // -- saving current speed --------------------------------------
  doc()->setSpeed( m_writerSelectionWidget->writerSpeed() );
	
  // -- saving current device --------------------------------------
  doc()->setBurner( m_writerSelectionWidget->writerDevice() );
	
  // -- saving mkisofs-options -------------------------------------
  ((K3bDataDoc*)doc())->setCreateRockRidge( m_checkCreateRR->isChecked() );
  ((K3bDataDoc*)doc())->setCreateJoliet( m_checkCreateJoliet->isChecked() );
  ((K3bDataDoc*)doc())->setISOallowLowercase( m_checkLowercase->isChecked() );
  ((K3bDataDoc*)doc())->setISOallowPeriodAtBegin( m_checkBeginPeriod->isChecked() );
  ((K3bDataDoc*)doc())->setISOallow31charFilenames( m_checkAllow31->isChecked() );
  ((K3bDataDoc*)doc())->setISOomitVersionNumbers( m_checkOmitVersion->isChecked() );
  ((K3bDataDoc*)doc())->setISOmaxFilenameLength( m_checkMaxNames->isChecked() );
  ((K3bDataDoc*)doc())->setISOrelaxedFilenames( m_checkRelaxedNames->isChecked() );
  ((K3bDataDoc*)doc())->setISOnoIsoTranslate( m_checkNoISOTrans->isChecked() );
  ((K3bDataDoc*)doc())->setISOallowMultiDot( m_checkMultiDot->isChecked() );
  ((K3bDataDoc*)doc())->setISOuntranslatedFilenames( m_checkUntranslatedNames->isChecked() );
  ((K3bDataDoc*)doc())->setNoDeepDirectoryRelocation( m_checkNoDeepDirRel->isChecked() );
  //	((K3bDataDoc*)doc())->setFollowSymbolicLinks( m_check->isChecked() );
  ((K3bDataDoc*)doc())->setHideRR_MOVED( m_checkHideRR_MOVED->isChecked() );
  ((K3bDataDoc*)doc())->setCreateTRANS_TBL( m_checkCreateTRANS_TBL->isChecked() );
  ((K3bDataDoc*)doc())->setHideTRANS_TBL( m_checkHideTRANS_TBL->isChecked() );
  ((K3bDataDoc*)doc())->setPadding( m_checkPadding->isChecked() );
	
  ((K3bDataDoc*)doc())->setVolumeID( m_editVolumeID->text() );
  ((K3bDataDoc*)doc())->setVolumeSetId( m_editVolumeSetId->text() );
  ((K3bDataDoc*)doc())->setApplicationID( m_editApplicationID->text() );
  ((K3bDataDoc*)doc())->setSystemId( m_editSystemId->text() );
  ((K3bDataDoc*)doc())->setPublisher( m_editPublisher->text() );
  ((K3bDataDoc*)doc())->setPreparer( m_editPreparer->text() );
  // ------------------------------------- saving mkisofs-options --

  // save iso-level
  if( m_groupIsoLevel->selected() == m_radioIsoLevel3 )
    ((K3bDataDoc*)doc())->setISOLevel( 3 );
  else if( m_groupIsoLevel->selected() == m_radioIsoLevel2 )
    ((K3bDataDoc*)doc())->setISOLevel( 2 );
  else
    ((K3bDataDoc*)doc())->setISOLevel( 1 );
	
  // save whitespace-treatment
  if( m_groupWhiteSpace->selected() == m_radioSpaceStrip )
    ((K3bDataDoc*)doc())->setWhiteSpaceTreatment( K3bDataDoc::strip );
  else if( m_groupWhiteSpace->selected() == m_radioSpaceExtended )
    ((K3bDataDoc*)doc())->setWhiteSpaceTreatment( K3bDataDoc::extendedStrip );
  else if( m_groupWhiteSpace->selected() == m_radioSpaceReplace )
    ((K3bDataDoc*)doc())->setWhiteSpaceTreatment( K3bDataDoc::convertToUnderScore );
  else
    ((K3bDataDoc*)doc())->setWhiteSpaceTreatment( K3bDataDoc::normal );

  // save image file path
  ((K3bDataDoc*)doc())->setIsoImage( m_tempDirSelectionWidget->tempPath() );  

  // save multisession settings
  if( m_groupMultiSession->selected() == m_radioMultiSessionStart )
    ((K3bDataDoc*)doc())->setMultiSessionMode( K3bDataDoc::START );
  else if( m_groupMultiSession->selected() == m_radioMultiSessionContinue )
    ((K3bDataDoc*)doc())->setMultiSessionMode( K3bDataDoc::CONTINUE );
  else if( m_groupMultiSession->selected() == m_radioMultiSessionFinish )
    ((K3bDataDoc*)doc())->setMultiSessionMode( K3bDataDoc::FINISH );
  else
    ((K3bDataDoc*)doc())->setMultiSessionMode( K3bDataDoc::NONE );
}


void K3bDataBurnDialog::readSettings()
{
  m_checkDao->setChecked( doc()->dao() );
  m_checkDummy->setChecked( doc()->dummy() );
  m_checkOnTheFly->setChecked( doc()->onTheFly() );
  m_checkBurnProof->setChecked( doc()->burnproof() );
  m_checkOnlyCreateImage->setChecked( ((K3bDataDoc*)doc())->onlyCreateImage() );
  m_checkDeleteImage->setChecked( ((K3bDataDoc*)doc())->deleteImage() );
	
  m_editVolumeID->setText(  ((K3bDataDoc*)doc())->volumeID() );
  m_editVolumeSetId->setText(  ((K3bDataDoc*)doc())->volumeSetId() );
  m_editApplicationID->setText(  ((K3bDataDoc*)doc())->applicationID() );
  m_editSystemId->setText(  ((K3bDataDoc*)doc())->systemId() );
  m_editPublisher->setText(  ((K3bDataDoc*)doc())->publisher() );
  m_editPreparer->setText(  ((K3bDataDoc*)doc())->preparer() );


  // read multisession 
  switch( ((K3bDataDoc*)doc())->multiSessionMode() ) {
  case K3bDataDoc::START:
    m_radioMultiSessionStart->setChecked(true);
    break;
  case K3bDataDoc::CONTINUE:
    m_radioMultiSessionContinue->setChecked(true);
    break;
  case K3bDataDoc::FINISH:
    m_radioMultiSessionFinish->setChecked(true);
    break;
  default:
    m_radioMultiSessionNone->setChecked(true);
    break;
  }


  // -- read mkisofs-options -------------------------------------
  m_checkCreateRR->setChecked( ((K3bDataDoc*)doc())->createRockRidge() );
  m_checkCreateJoliet->setChecked( ((K3bDataDoc*)doc())->createJoliet() );
  m_checkLowercase->setChecked( ((K3bDataDoc*)doc())->ISOallowLowercase() );
  m_checkBeginPeriod->setChecked( ((K3bDataDoc*)doc())->ISOallowPeriodAtBegin() );
  m_checkAllow31->setChecked( ((K3bDataDoc*)doc())->ISOallow31charFilenames() );
  m_checkOmitVersion->setChecked( ((K3bDataDoc*)doc())->ISOomitVersionNumbers() );
  m_checkMaxNames->setChecked( ((K3bDataDoc*)doc())->ISOmaxFilenameLength() );
  m_checkRelaxedNames->setChecked( ((K3bDataDoc*)doc())->ISOrelaxedFilenames() );
  m_checkNoISOTrans->setChecked( ((K3bDataDoc*)doc())->ISOnoIsoTranslate() );
  m_checkMultiDot->setChecked( ((K3bDataDoc*)doc())->ISOallowMultiDot() );
  m_checkUntranslatedNames->setChecked( ((K3bDataDoc*)doc())->ISOuntranslatedFilenames() );
  m_checkNoDeepDirRel->setChecked( ((K3bDataDoc*)doc())->noDeepDirectoryRelocation() );

  m_checkHideRR_MOVED->setChecked( ((K3bDataDoc*)doc())->hideRR_MOVED() );
  m_checkCreateTRANS_TBL->setChecked( ((K3bDataDoc*)doc())->createTRANS_TBL() );
  m_checkHideTRANS_TBL->setChecked( ((K3bDataDoc*)doc())->hideTRANS_TBL() );
  m_checkPadding->setChecked( ((K3bDataDoc*)doc())->padding() );
  // ------------------------------------- read mkisofs-options --


  // read iso-level
  switch( ((K3bDataDoc*)doc())->ISOLevel() ) {
  case 1:
    m_radioIsoLevel1->setChecked(true);
    break;
  case 2:
    m_radioIsoLevel2->setChecked(true);
    break;
  case 3:
    m_radioIsoLevel3->setChecked(true);
    break;
  }


  // read whitespace treatment
  switch( ((K3bDataDoc*)doc())->whiteSpaceTreatment() ) {
  case K3bDataDoc::strip:
    m_radioSpaceStrip->setChecked(true);
    break;
  case K3bDataDoc::extendedStrip:
    m_radioSpaceExtended->setChecked(true);
    break;
  case K3bDataDoc::convertToUnderScore:
    m_radioSpaceReplace->setChecked(true);
    break;
  default:
    m_radioSpaceLeave->setChecked(true);
  }

	
  K3bProjectBurnDialog::readSettings();
}


void K3bDataBurnDialog::setupBurnTab( QFrame* frame )
{
  QGridLayout* frameLayout = new QGridLayout( frame );
  frameLayout->setSpacing( spacingHint() );
  frameLayout->setMargin( marginHint() );

  m_writerSelectionWidget = new K3bWriterSelectionWidget( frame );
  m_tempDirSelectionWidget = new K3bTempDirSelectionWidget( frame );
  m_tempDirSelectionWidget->setSelectionMode( K3bTempDirSelectionWidget::FILE );

  frameLayout->addMultiCellWidget( m_writerSelectionWidget, 0, 0, 0, 1 );
  frameLayout->addWidget( m_tempDirSelectionWidget, 1, 1 );

  m_groupOptions = new QGroupBox( frame, "m_groupOptions" );
  m_groupOptions->setTitle( i18n( "Options" ) );
  m_groupOptions->setColumnLayout(0, Qt::Vertical );
  m_groupOptions->layout()->setSpacing( 0 );
  m_groupOptions->layout()->setMargin( 0 );
  QVBoxLayout* m_groupOptionsLayout = new QVBoxLayout( m_groupOptions->layout() );
  m_groupOptionsLayout->setAlignment( Qt::AlignTop );
  m_groupOptionsLayout->setSpacing( spacingHint() );
  m_groupOptionsLayout->setMargin( marginHint() );

  m_checkDummy = new QCheckBox( m_groupOptions, "m_checkDummy" );
  m_checkDummy->setText( i18n( "Simulate writing" ) );
  m_groupOptionsLayout->addWidget( m_checkDummy );

  m_checkOnTheFly = new QCheckBox( m_groupOptions, "m_checkOnTheFly" );
  m_checkOnTheFly->setText( i18n( "Writing on the fly" ) );
  m_groupOptionsLayout->addWidget( m_checkOnTheFly );

  m_checkOnlyCreateImage = new QCheckBox( m_groupOptions, "m_checkOnlyCreateImage" );
  m_checkOnlyCreateImage->setText( i18n( "Only create image" ) );
  m_groupOptionsLayout->addWidget( m_checkOnlyCreateImage );

  m_checkDeleteImage = new QCheckBox( m_groupOptions, "m_checkDeleteImage" );
  m_checkDeleteImage->setText( i18n( "Delete image" ) );
  m_groupOptionsLayout->addWidget( m_checkDeleteImage );

  m_checkDao = new QCheckBox( m_groupOptions, "m_checkDao" );
  m_checkDao->setText( i18n( "Disc at once" ) );
  m_groupOptionsLayout->addWidget( m_checkDao );

  m_checkBurnProof = new QCheckBox( m_groupOptions, "m_checkBurnProof" );
  m_checkBurnProof->setText( i18n( "use BURN-PROOF" ) );
  m_groupOptionsLayout->addWidget( m_checkBurnProof );

  frameLayout->addWidget( m_groupOptions, 1, 0 );

  // we do not need a tempdir or image settings when writing on-the-fly
  connect( m_checkOnTheFly, SIGNAL(toggled(bool)), m_checkDeleteImage, SLOT(setDisabled(bool)) );
  connect( m_checkOnTheFly, SIGNAL(toggled(bool)), m_tempDirSelectionWidget, SLOT(setDisabled(bool)) );
  connect( m_checkOnTheFly, SIGNAL(toggled(bool)), m_checkOnlyCreateImage, SLOT(setDisabled(bool)) );

  // we do not need writer settings when only creating the image
  connect( m_checkOnlyCreateImage, SIGNAL(toggled(bool)), m_writerSelectionWidget, SLOT(setDisabled(bool)) );
  connect( m_checkOnlyCreateImage, SIGNAL(toggled(bool)), m_checkOnTheFly, SLOT(setDisabled(bool)) );
  connect( m_checkOnlyCreateImage, SIGNAL(toggled(bool)), m_checkBurnProof, SLOT(setDisabled(bool)) );
  connect( m_checkOnlyCreateImage, SIGNAL(toggled(bool)), m_checkDao, SLOT(setDisabled(bool)) );
  connect( m_checkOnlyCreateImage, SIGNAL(toggled(bool)), m_checkDummy, SLOT(setDisabled(bool)) );

  frameLayout->setRowStretch( 1, 1 );
  frameLayout->setColStretch( 1, 1 );


  connect( m_writerSelectionWidget, SIGNAL(writerChanged()), this, SLOT(slotWriterChanged()) );
}


void K3bDataBurnDialog::setupAdvancedTab( QFrame* frame )
{
  QGridLayout* frameLayout = new QGridLayout( frame );
  frameLayout->setSpacing( spacingHint() );
  frameLayout->setMargin( marginHint() );

  m_buttonSaveAsDefault = new QPushButton( frame, "m_buttonSaveAsDefault" );
  m_buttonSaveAsDefault->setText( i18n( "Save as default" ) );
  QToolTip::add(  m_buttonSaveAsDefault, i18n( "Saves the current settings into \"K3b Default\"" ) );

  frameLayout->addWidget( m_buttonSaveAsDefault, 0, 2 );

  m_groupPreSettings = new QGroupBox( frame, "m_groupPreSettings" );
  m_groupPreSettings->setTitle( i18n( "Predefined Settings" ) );
  m_groupPreSettings->setColumnLayout(0, Qt::Vertical );
  m_groupPreSettings->layout()->setSpacing( 0 );
  m_groupPreSettings->layout()->setMargin( 0 );
  QVBoxLayout* m_groupPreSettingsLayout = new QVBoxLayout( m_groupPreSettings->layout() );
  m_groupPreSettingsLayout->setAlignment( Qt::AlignTop );
  m_groupPreSettingsLayout->setSpacing( spacingHint() );
  m_groupPreSettingsLayout->setMargin( marginHint() );

  m_comboPreSettings = new QComboBox( FALSE, m_groupPreSettings, "m_comboPreSettings" );
  m_comboPreSettings->insertItem( i18n( "For Win9x/NT use only" ) );
  m_comboPreSettings->insertItem( i18n( "K3b Default" ), 0 );
  m_comboPreSettings->insertItem( i18n( "For Linux/UNIX use only" ), 1 );
  //    m_comboPreSettings->insertItem( i18n( "Max Compatibility" ) );
  m_comboPreSettings->insertItem( i18n( "Custom" ) );
  m_groupPreSettingsLayout->addWidget( m_comboPreSettings );

  frameLayout->addWidget( m_groupPreSettings, 0, 0 );
  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  frameLayout->addItem( spacer, 0, 1 );

  frameSettings = new QFrame( frame, "frameSettings" );
  frameSettings->setFrameShape( QFrame::Box );
  frameSettings->setFrameShadow( QFrame::Sunken );
  QGridLayout* frameSettingsLayout = new QGridLayout( frameSettings );
  frameSettingsLayout->setSpacing( spacingHint() );
  frameSettingsLayout->setMargin( marginHint() );

  m_groupIsoLevel = new QButtonGroup( frameSettings, "m_groupIsoLevel" );
  m_groupIsoLevel->setTitle( i18n( "ISO Level" ) );
  m_groupIsoLevel->setColumnLayout(0, Qt::Vertical );
  m_groupIsoLevel->layout()->setSpacing( 0 );
  m_groupIsoLevel->layout()->setMargin( 0 );
  QVBoxLayout* m_groupIsoLevelLayout = new QVBoxLayout( m_groupIsoLevel->layout() );
  m_groupIsoLevelLayout->setAlignment( Qt::AlignTop );
  m_groupIsoLevelLayout->setSpacing( spacingHint() );
  m_groupIsoLevelLayout->setMargin( marginHint() );

  m_radioIsoLevel1 = new QRadioButton( m_groupIsoLevel, "m_radioIsoLevel1" );
  m_radioIsoLevel1->setText( "Level 1" );
  m_radioIsoLevel1->setChecked( TRUE );
  m_groupIsoLevelLayout->addWidget( m_radioIsoLevel1 );

  m_radioIsoLevel2 = new QRadioButton( m_groupIsoLevel, "m_radioIsoLevel2" );
  m_radioIsoLevel2->setText( "Level 2" );
  m_groupIsoLevelLayout->addWidget( m_radioIsoLevel2 );

  m_radioIsoLevel3 = new QRadioButton( m_groupIsoLevel, "m_radioIsoLevel3" );
  m_radioIsoLevel3->setText( "Level 3" );
  m_groupIsoLevelLayout->addWidget( m_radioIsoLevel3 );

  frameSettingsLayout->addWidget( m_groupIsoLevel, 0, 0 );

  QVBoxLayout* Layout1 = new QVBoxLayout;
  Layout1->setSpacing( spacingHint() );
  Layout1->setMargin( 0 );

  m_checkNoDeepDirRel = new QCheckBox( frameSettings, "m_checkNoDeepDirRel" );
  m_checkNoDeepDirRel->setText( i18n( "no deep directory relocation" ) );
  Layout1->addWidget( m_checkNoDeepDirRel );

  m_checkPadding = new QCheckBox( frameSettings, "m_checkPadding" );
  m_checkPadding->setText( i18n( "use padding" ) );
  Layout1->addWidget( m_checkPadding );

  m_checkHideRR_MOVED = new QCheckBox( frameSettings, "m_checkHideRR_MOVED" );
  m_checkHideRR_MOVED->setText( i18n( "hide RR_MOVED" ) );
  Layout1->addWidget( m_checkHideRR_MOVED );

  m_checkCreateTRANS_TBL = new QCheckBox( frameSettings, "m_checkCreateTRANS_TBL" );
  m_checkCreateTRANS_TBL->setText( i18n( "create TRANS_TBL entries" ) );
  Layout1->addWidget( m_checkCreateTRANS_TBL );

  m_checkHideTRANS_TBL = new QCheckBox( frameSettings, "m_checkHideTRANS_TBL" );
  m_checkHideTRANS_TBL->setText( i18n( "hide TRANS_TBL in Joliet" ) );
  Layout1->addWidget( m_checkHideTRANS_TBL );

  QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  Layout1->addItem( spacer_2 );


  frameSettingsLayout->addMultiCellLayout( Layout1, 0, 1, 1, 1 );
  QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  frameSettingsLayout->addItem( spacer_3, 1, 0 );

  QVBoxLayout* Layout3 = new QVBoxLayout;
  Layout3->setSpacing( spacingHint() );
  Layout3->setMargin( 0 );

  m_checkUntranslatedNames = new QCheckBox( frameSettings, "m_checkUntranslatedNames" );
  m_checkUntranslatedNames->setText( i18n( "allow untranslated filenames" ) );
  Layout3->addWidget( m_checkUntranslatedNames );
  QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  Layout3->addItem( spacer_4 );

  m_checkAllow31 = new QCheckBox( frameSettings, "m_checkAllow31" );
  m_checkAllow31->setText( i18n( "allow 31 character filenames" ) );
  Layout3->addWidget( m_checkAllow31 );

  m_checkMaxNames = new QCheckBox( frameSettings, "m_checkMaxNames" );
  m_checkMaxNames->setText( i18n( "max length (37) filenames" ) );
  Layout3->addWidget( m_checkMaxNames );

  m_checkBeginPeriod = new QCheckBox( frameSettings, "m_checkBeginPeriod" );
  m_checkBeginPeriod->setText( i18n( "allow beginning period" ) );
  Layout3->addWidget( m_checkBeginPeriod );

  m_checkRelaxedNames = new QCheckBox( frameSettings, "m_checkRelaxedNames" );
  m_checkRelaxedNames->setText( i18n( "relaxed filenames" ) );
  Layout3->addWidget( m_checkRelaxedNames );

  m_checkOmitVersion = new QCheckBox( frameSettings, "m_checkOmitVersion" );
  m_checkOmitVersion->setText( i18n( "omit version numbers" ) );
  Layout3->addWidget( m_checkOmitVersion );

  m_checkNoISOTrans = new QCheckBox( frameSettings, "m_checkNoISOTrans" );
  m_checkNoISOTrans->setText( i18n( "allow # and ~" ) );
  Layout3->addWidget( m_checkNoISOTrans );

  m_checkMultiDot = new QCheckBox( frameSettings, "m_checkMultiDot" );
  m_checkMultiDot->setText( i18n( "allow multible dots" ) );
  Layout3->addWidget( m_checkMultiDot );

  m_checkLowercase = new QCheckBox( frameSettings, "m_checkLowercase" );
  m_checkLowercase->setText( i18n( "allow lowercase filenames" ) );
  Layout3->addWidget( m_checkLowercase );

  frameSettingsLayout->addMultiCellLayout( Layout3, 0, 1, 2, 2 );

  frameLayout->addMultiCellWidget( frameSettings, 1, 1, 0, 2 );

  // signals and slots connections
  connect( m_checkUntranslatedNames, SIGNAL( toggled(bool) ), m_checkAllow31, SLOT( setDisabled(bool) ) );
  connect( m_checkUntranslatedNames, SIGNAL( toggled(bool) ), m_checkBeginPeriod, SLOT( setDisabled(bool) ) );
  connect( m_checkUntranslatedNames, SIGNAL( toggled(bool) ), m_checkOmitVersion, SLOT( setDisabled(bool) ) );
  connect( m_checkUntranslatedNames, SIGNAL( toggled(bool) ), m_checkRelaxedNames, SLOT( setDisabled(bool) ) );
  connect( m_checkUntranslatedNames, SIGNAL( toggled(bool) ), m_checkLowercase, SLOT( setDisabled(bool) ) );
  connect( m_checkUntranslatedNames, SIGNAL( toggled(bool) ), m_checkNoISOTrans, SLOT( setDisabled(bool) ) );
  connect( m_checkUntranslatedNames, SIGNAL( toggled(bool) ), m_checkMultiDot, SLOT( setDisabled(bool) ) );
  connect( m_checkUntranslatedNames, SIGNAL( toggled(bool) ), m_checkMaxNames, SLOT( setDisabled(bool) ) );

  //	connect( m_checkCreateJoliet, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  //	connect( m_checkCreateRR, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkNoDeepDirRel, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkPadding, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkHideRR_MOVED, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkCreateTRANS_TBL, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkHideTRANS_TBL, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkUntranslatedNames, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkAllow31, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkMaxNames, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkBeginPeriod, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkRelaxedNames, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkOmitVersion, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkNoISOTrans, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkMultiDot, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect(	m_checkLowercase, SIGNAL(clicked()), this, SLOT(slotSelectCustom()) );
  connect( m_groupIsoLevel, SIGNAL(clicked(int)), this, SLOT(slotSelectCustom()) );

  connect( m_comboPreSettings, SIGNAL(activated(const QString&)), this, SLOT(slotLoadPreSettings(const QString&)) );
  connect( m_buttonSaveAsDefault, SIGNAL(clicked()), this, SLOT(slotSaveDefaults()) );
}


void K3bDataBurnDialog::setupSettingsTab( QFrame* frame )
{
  QGridLayout* frameLayout = new QGridLayout( frame );
  frameLayout->setSpacing( spacingHint() );
  frameLayout->setMargin( marginHint() );

  QGroupBox* _groupVolumeInfo = new QGroupBox( 0, Qt::Vertical, i18n( "Information" ), frame, "_groupVolumeInfo" );
  _groupVolumeInfo->layout()->setSpacing( 0 );
  _groupVolumeInfo->layout()->setMargin( 0 );
  QGridLayout* _groupVolumeInfoLayout = new QGridLayout( _groupVolumeInfo->layout() );
  _groupVolumeInfoLayout->setAlignment( Qt::AlignTop );
  _groupVolumeInfoLayout->setSpacing( spacingHint() );
  _groupVolumeInfoLayout->setMargin( marginHint() );

  _groupVolumeInfoLayout->addWidget( new QLabel( i18n( "Volume ID" ), _groupVolumeInfo, "m_labelVolumeID" ), 0, 0 );
  _groupVolumeInfoLayout->addWidget( new QLabel( i18n( "Volume Set ID" ), _groupVolumeInfo, "m_labelVolumeSetID" ), 1, 0 );
  _groupVolumeInfoLayout->addWidget( new QLabel( i18n( "Publisher" ), _groupVolumeInfo, "m_labelPublisher" ), 2, 0 );
  _groupVolumeInfoLayout->addWidget( new QLabel( i18n( "Preparer" ), _groupVolumeInfo, "m_labelPreparer" ), 3, 0 );
  _groupVolumeInfoLayout->addWidget( new QLabel( i18n( "System ID" ), _groupVolumeInfo, "m_labelSystemID" ), 4, 0 );
  _groupVolumeInfoLayout->addWidget( new QLabel( i18n( "Application ID" ), _groupVolumeInfo, "m_labelApplicationID" ), 5, 0 );

  // are this really the allowed characters? What about Joliet or UDF?
  QRegExpValidator* isoValidator = new QRegExpValidator( QRegExp("([a-z]|[A-Z]|[0-9]|_)*"), this, "isoValidator" );

  m_editVolumeID = new KLineEdit( _groupVolumeInfo, "m_editVolumeID" );
  m_editVolumeID->setValidator( isoValidator );
  m_editVolumeID->setMaxLength( 32 );
  m_editVolumeSetId = new KLineEdit( _groupVolumeInfo, "m_editVolumeSetID" );
  m_editVolumeSetId->setValidator( isoValidator );
  m_editVolumeSetId->setMaxLength( 128 );
  m_editPublisher = new KLineEdit( _groupVolumeInfo, "m_editPublisher" );
  m_editPublisher->setValidator( isoValidator );
  m_editPublisher->setMaxLength( 128 );
  m_editPreparer = new KLineEdit( _groupVolumeInfo, "m_editPreparer" );
  m_editPreparer->setValidator( isoValidator );
  m_editPreparer->setMaxLength( 128 );
  m_editSystemId = new KLineEdit( _groupVolumeInfo, "m_editSystemID" );
  m_editSystemId->setValidator( isoValidator );
  m_editSystemId->setMaxLength( 32 );
  m_editApplicationID = new KLineEdit( _groupVolumeInfo, "m_editApplicationID" );
  m_editApplicationID->setValidator( isoValidator );
  m_editApplicationID->setMaxLength( 128 );

  _groupVolumeInfoLayout->addWidget( m_editVolumeID, 0, 1 );
  _groupVolumeInfoLayout->addWidget( m_editVolumeSetId, 1, 1 );
  _groupVolumeInfoLayout->addWidget( m_editPublisher, 2, 1 );
  _groupVolumeInfoLayout->addWidget( m_editPreparer, 3, 1 );
  _groupVolumeInfoLayout->addWidget( m_editSystemId, 4, 1 );
  _groupVolumeInfoLayout->addWidget( m_editApplicationID, 5, 1 );

  frameLayout->addMultiCellWidget( _groupVolumeInfo, 0, 3, 1, 1 );

  m_checkCreateRR = new QCheckBox( frame, "m_checkCreateRR" );
  m_checkCreateRR->setText( i18n( "Generate Rockridge Entries" ) );

  frameLayout->addWidget( m_checkCreateRR, 1, 0 );

  m_checkCreateJoliet = new QCheckBox( frame, "m_checkCreateJoliet" );
  m_checkCreateJoliet->setText( i18n( "Generate Joilet entries" ) );

  frameLayout->addWidget( m_checkCreateJoliet, 0, 0 );

  m_groupWhiteSpace = new QButtonGroup( frame, "m_groupWhiteSpace" );
  m_groupWhiteSpace->setTitle( i18n( "Whitespace treatment" ) );
  m_groupWhiteSpace->setColumnLayout(0, Qt::Vertical );
  m_groupWhiteSpace->layout()->setSpacing( 0 );
  m_groupWhiteSpace->layout()->setMargin( 0 );
  QVBoxLayout* m_groupWhiteSpaceLayout = new QVBoxLayout( m_groupWhiteSpace->layout() );
  m_groupWhiteSpaceLayout->setAlignment( Qt::AlignTop );
  m_groupWhiteSpaceLayout->setSpacing( spacingHint() );
  m_groupWhiteSpaceLayout->setMargin( marginHint() );

  m_radioSpaceLeave = new QRadioButton( m_groupWhiteSpace, "m_radioSpaceLeave" );
  m_radioSpaceLeave->setText( i18n( "leave them" ) );
  m_radioSpaceLeave->setChecked( TRUE );
  m_groupWhiteSpaceLayout->addWidget( m_radioSpaceLeave );

  m_radioSpaceReplace = new QRadioButton( m_groupWhiteSpace, "m_radioSpaceReplace" );
  m_radioSpaceReplace->setText( i18n( "replace with underscores" ) );
  m_groupWhiteSpaceLayout->addWidget( m_radioSpaceReplace );

  m_radioSpaceStrip = new QRadioButton( m_groupWhiteSpace, "m_radioSpaceStrip" );
  m_radioSpaceStrip->setText( i18n( "strip" ) );
  m_groupWhiteSpaceLayout->addWidget( m_radioSpaceStrip );

  m_radioSpaceExtended = new QRadioButton( m_groupWhiteSpace, "m_radioSpaceExtended" );
  m_radioSpaceExtended->setText( i18n( "extended strip" ) );
  m_groupWhiteSpaceLayout->addWidget( m_radioSpaceExtended );

  frameLayout->addWidget( m_groupWhiteSpace, 3, 0 );
  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  frameLayout->addItem( spacer, 2, 0 );

//   connect( m_editVolumeID, SIGNAL(textChanged(const QString&)), this, SLOT(slotConvertAllToUpperCase()) );
//   connect( m_editApplicationID, SIGNAL(textChanged(const QString&)), this, SLOT(slotConvertAllToUpperCase()) );
//   connect( m_editPreparer, SIGNAL(textChanged(const QString&)), this, SLOT(slotConvertAllToUpperCase()) );
//   connect( m_editPublisher, SIGNAL(textChanged(const QString&)), this, SLOT(slotConvertAllToUpperCase()) );
}


void K3bDataBurnDialog::setupMultisessionTab( QFrame* frame )
{
  QGridLayout* frameLayout = new QGridLayout( frame );
  frameLayout->setSpacing( spacingHint() );
  frameLayout->setMargin( marginHint() );

  m_groupMultiSession = new QButtonGroup( 4, Qt::Vertical, QString::null, frame );
  m_groupMultiSession->setInsideSpacing( spacingHint() );
  m_groupMultiSession->setInsideMargin( 0 );
  m_groupMultiSession->setFrameStyle( QFrame::NoFrame | QFrame::Plain );

  m_radioMultiSessionNone = new QRadioButton( i18n("&No Multisession"), m_groupMultiSession );
  m_radioMultiSessionStart = new QRadioButton( i18n("&Start Multisession"), m_groupMultiSession );
  m_radioMultiSessionContinue = new QRadioButton( i18n("&Continue Multisession"), m_groupMultiSession );
  m_radioMultiSessionFinish = new QRadioButton( i18n("&Finish Multisession"), m_groupMultiSession );

  frameLayout->addWidget( m_groupMultiSession, 0, 0 );
}


void K3bDataBurnDialog::slotLoadPreSettings( const QString& pre )
{

  // TODO: load/save iso level

  if( pre == i18n("K3b Default") ) {
    // load defaults from config
    kapp->config()->setGroup( "Default ISO Settings" );
    m_checkCreateJoliet->setChecked( kapp->config()->readBoolEntry( "create Joliet", false ) );
    m_checkCreateRR->setChecked( kapp->config()->readBoolEntry( "create Rockridge", true ) );
    m_checkNoDeepDirRel->setChecked( kapp->config()->readBoolEntry( "no deep dir relocation", false ) );
    m_checkPadding->setChecked( kapp->config()->readBoolEntry( "padding", false ) );
    m_checkHideRR_MOVED->setChecked( kapp->config()->readBoolEntry( "hide RR_MOVED", false ) );
    m_checkCreateTRANS_TBL->setChecked( kapp->config()->readBoolEntry( "create TRANS_TBL", false ) );
    m_checkHideTRANS_TBL->setChecked( kapp->config()->readBoolEntry( "hide TRANS_TBL", false ) );
    m_checkUntranslatedNames->setChecked( kapp->config()->readBoolEntry( "untranslated filenames", false ) );
    m_checkAllow31->setChecked( kapp->config()->readBoolEntry( "allow 31 character filenames", false ) );
    m_checkMaxNames->setChecked( kapp->config()->readBoolEntry( "max ISO filenames", false ) );
    m_checkBeginPeriod->setChecked( kapp->config()->readBoolEntry( "allow beginning period", false ) );
    m_checkRelaxedNames->setChecked( kapp->config()->readBoolEntry( "relaxed filenames", false ) );
    m_checkOmitVersion->setChecked( kapp->config()->readBoolEntry( "omit version numbers", false ) );
    m_checkNoISOTrans->setChecked( kapp->config()->readBoolEntry( "no iSO translation", false ) );
    m_checkMultiDot->setChecked( kapp->config()->readBoolEntry( "allow multible dots", false ) );
    m_checkLowercase->setChecked( kapp->config()->readBoolEntry( "allow lowercase filenames", false ) );
  }
  else if( pre == i18n("For Linux/UNIX use only") ) {
    m_checkCreateJoliet->setChecked( false );
    m_checkCreateRR->setChecked( true );
    m_checkNoDeepDirRel->setChecked( false );
    m_checkPadding->setChecked( false );
    m_checkHideRR_MOVED->setChecked( true );
    m_checkCreateTRANS_TBL->setChecked( false );
    m_checkHideTRANS_TBL->setChecked( false );
    m_checkUntranslatedNames->setChecked( false );
    m_checkAllow31->setChecked( false );
    m_checkMaxNames->setChecked( false );
    m_checkBeginPeriod->setChecked( false );
    m_checkRelaxedNames->setChecked( false );
    m_checkOmitVersion->setChecked( false );
    m_checkNoISOTrans->setChecked( false );
    m_checkMultiDot->setChecked( false );
    m_checkLowercase->setChecked( false );
  }
  else if( pre == i18n("For Win9x/NT use only") ) {
    m_checkCreateJoliet->setChecked( true );
    m_checkCreateRR->setChecked( false );
    m_checkNoDeepDirRel->setChecked( false );
    m_checkPadding->setChecked( false );
    m_checkHideRR_MOVED->setChecked( true );
    m_checkCreateTRANS_TBL->setChecked( true );
    m_checkHideTRANS_TBL->setChecked( false );
    m_checkUntranslatedNames->setChecked( false );
    m_checkAllow31->setChecked( true );
    m_checkMaxNames->setChecked( false );
    m_checkBeginPeriod->setChecked( false );
    m_checkRelaxedNames->setChecked( false );
    m_checkOmitVersion->setChecked( false );
    m_checkNoISOTrans->setChecked( true );
    m_checkMultiDot->setChecked( false );
    m_checkLowercase->setChecked( false );
  }
  //	else if( pre == i18n("Max Compatibility") ) {
  //		m_checkCreateJoliet->setChecked(  );
  //		m_checkCreateRR->setChecked(  );
  //		m_checkNoDeepDirRel->setChecked(  );
  //		m_checkPadding->setChecked(  );
  //		m_checkHideRR_MOVED->setChecked(  );
  //		m_checkCreateTRANS_TBL->setChecked(  );
  //		m_checkHideTRANS_TBL->setChecked(  );
  //		m_checkUntranslatedNames->setChecked(  );
  //		m_checkAllow31->setChecked(  );
  //		m_checkMaxNames->setChecked(  );
  //		m_checkBeginPeriod->setChecked(  );
  //		m_checkRelaxedNames->setChecked(  );
  //		m_checkOmitVersion->setChecked(  );
  //		m_checkNoISOTrans->setChecked(  );
  //		m_checkMultiDot->setChecked(  );
  //		m_checkLowercase->setChecked(  );
  //	}
}


void K3bDataBurnDialog::slotSaveDefaults()
{

  // TODO: load/save iso level

  kapp->config()->setGroup( "Default ISO Settings" );
  kapp->config()->writeEntry( "create Joliet", m_checkCreateJoliet->isChecked( ) );
  kapp->config()->writeEntry( "create Rockridge", m_checkCreateRR->isChecked( ) );
  kapp->config()->writeEntry( "no deep dir relocation", m_checkNoDeepDirRel->isChecked( ) );
  kapp->config()->writeEntry( "padding" , m_checkPadding->isChecked( ) );
  kapp->config()->writeEntry( "hide RR_MOVED", m_checkHideRR_MOVED->isChecked( ) );
  kapp->config()->writeEntry( "create TRANS_TBL", m_checkCreateTRANS_TBL->isChecked( ) );
  kapp->config()->writeEntry( "hide TRANS_TBL", m_checkHideTRANS_TBL->isChecked( ) );
  kapp->config()->writeEntry( "untranslated filenames", m_checkUntranslatedNames->isChecked( ) );
  kapp->config()->writeEntry( "allow 31 character filenames", m_checkAllow31->isChecked() );
  kapp->config()->writeEntry( "max ISO filenames", m_checkMaxNames->isChecked() );
  kapp->config()->writeEntry( "allow beginning period", m_checkBeginPeriod->isChecked() );
  kapp->config()->writeEntry( "relaxed filenames", m_checkRelaxedNames->isChecked( ) );
  kapp->config()->writeEntry( "omit version numbers", m_checkOmitVersion->isChecked() );
  kapp->config()->writeEntry( "no iSO translation", m_checkNoISOTrans->isChecked() );
  kapp->config()->writeEntry( "allow multible dots", m_checkMultiDot->isChecked() );
  kapp->config()->writeEntry( "allow lowercase filenames", m_checkLowercase->isChecked( ) );
}


void K3bDataBurnDialog::slotSelectCustom()
{
  m_comboPreSettings->setCurrentItem( 3 );
}


void K3bDataBurnDialog::slotWriterChanged()
{
  if( K3bDevice* dev = m_writerSelectionWidget->writerDevice() )
    m_checkBurnProof->setEnabled( dev->burnproof() );
}


void K3bDataBurnDialog::slotUser1()
{
  // check if enough space in tempdir if not on-the-fly
  if( !m_checkOnTheFly->isChecked() && doc()->size()/1024 > m_tempDirSelectionWidget->freeTempSpace() ) {
    KMessageBox::sorry( this, "Not enough space in temp directory. Either change the directory or select on-the-fly burning." );
    return;
  }
  else if( !m_checkOnTheFly->isChecked() ) {
    QFileInfo fi( m_tempDirSelectionWidget->tempPath() );
    if( fi.isDir() )
      m_tempDirSelectionWidget->setTempPath( fi.filePath() + "/image.iso" );

    if( QFile::exists( m_tempDirSelectionWidget->tempPath() ) ) {
      if( KMessageBox::questionYesNo( this, i18n("Do you want to overwrite %1").arg(m_tempDirSelectionWidget->tempPath()), i18n("File exists...") ) 
	  != KMessageBox::Yes )
	return;
    }
  }
    
  K3bProjectBurnDialog::slotUser1();
}


void K3bDataBurnDialog::slotConvertAllToUpperCase()
{
  m_editApplicationID->setText( m_editApplicationID->text().upper() );
  m_editSystemId->setText( m_editSystemId->text().upper() );
  m_editVolumeID->setText( m_editVolumeID->text().upper() );
  m_editVolumeSetId->setText( m_editVolumeSetId->text().upper() );
  m_editPublisher->setText( m_editPublisher->text().upper() );
  m_editPreparer->setText( m_editPreparer->text().upper() );
}


#include "k3bdataburndialog.moc"
