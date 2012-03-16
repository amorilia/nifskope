/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2012, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the NIF File Format Library and Tools project may not be
   used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***** END LICENCE BLOCK *****/

#include "ns_base.h"

#include "spellbook.h"

#include <QDebug>
#include <QFileDialog>

#include "options.h"

// Brief description is deliberately not autolinked to class Spell
/*! \file animation.cpp
 * \brief Animation editing spells
 *
 * All classes here inherit from the Spell class.
 */

//! Attach a .KF to a .NIF
/*!
 * This only works for 10.0.1.0 onwards; prior to then a NiSequenceStreamHelper or NiSequence can be a root block.
 * The layout is different too; for 4.0.0.2 it appears that the chain of NiStringExtraData gives the names of
 * the block which the corresponding NiKeyframeController should attach to.
 * See Node (gl/glnode.cpp) for how controllers are handled
 */
class spAttachKf : public Spell
{
public:
	QString name() const { return Spell::tr("Attach .KF"); }
	QString page() const { return Spell::tr("Animation"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		return nif && ! index.isValid();
	}
	
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		if (nif == NULL)
			return index;

		QStringList kfnames = QFileDialog::getOpenFileNames( 0, Spell::tr("Choose .kf file(s)"), nif->getFolder(), FMASK_KF );
		foreach( QString kfname, kfnames )
		{
			try
			{
				//QString kfname = QFileDialog::getOpenFileName( 0, Spell::tr("Choose a .kf file"), nif->getFolder(), FMASK_KF );
				
				if ( kfname.isEmpty() )
					return index;
				
				NifModel kf;
				
				QFile kffile( kfname );
				if ( ! kffile.open( QFile::ReadOnly ) )
					throw QString( Spell::tr("failed to open .kf %1") ).arg( kfname );
				
				if ( ! kf.load( kffile ) )
					throw QString( Spell::tr("failed to load .kf from file %1") ).arg( kfname );
				
				QPersistentModelIndex iRoot;
				
				foreach ( qint32 l, kf.getRootLinks() )
				{
					QModelIndex iSeq = kf.getBlock( l, T_NICONTROLLERSEQUENCE );
					if ( ! iSeq.isValid() )
						throw QString( Spell::tr("this is not a normal .kf file; there should be only NiControllerSequences as root blocks") );
					
					QString rootName = kf.get<QString>( iSeq, TA_TARGETNAME );
					if (rootName.isEmpty())
						rootName = kf.get<QString>( iSeq, TA_TEXTKEYSNAME );// 10.0.1.0
					QModelIndex ir = findRootTarget( nif, rootName );
					
					if ( ! ir.isValid() )
						throw QString( Spell::tr("couldn't find the animation's root node (%1)") ).arg( rootName );
					
					if ( ! iRoot.isValid() )
						iRoot = ir;
					else if ( iRoot != ir )
						throw QString( Spell::tr("the animation root nodes differ; bailing out...") );
				}
				
				QPersistentModelIndex iMultiTransformer = findController( nif, iRoot, T_NIMULTITARGETTRANSFORMCONTROLLER );
				QPersistentModelIndex iCtrlManager = findController( nif, iRoot, T_NICONTROLLERMANAGER );
				
				QList<qint32> seqLinks = kf.getRootLinks();
				QStringList missingNodes;
				
				foreach ( qint32 lSeq, kf.getRootLinks() )
				{
					QModelIndex iSeq = kf.getBlock( lSeq, T_NICONTROLLERSEQUENCE );
					
					QList< QPersistentModelIndex > controlledNodes;
					
					QModelIndex iCtrlBlcks = kf.getIndex( iSeq, TA_CONTROLLEDBLOCKS );
					for ( int r = 0; r < kf.rowCount( iCtrlBlcks ); r++ )
					{
						QString nodeName = kf.string( iCtrlBlcks.child( r, 0 ), TA_NODENAME, false );
						if (nodeName.isEmpty())
							nodeName = kf.string( iCtrlBlcks.child( r, 0 ), TA_TARGETNAME, false );// 10.0.1.0
						if (nodeName.isEmpty()) {
							QModelIndex iNodeName = kf.getIndex( iCtrlBlcks.child( r, 0 ), TA_NODENAMEOFFSET );
							nodeName = iNodeName.sibling( iNodeName.row(), NifModel::ValueCol ).data( NifSkopeDisplayRole ).toString();
						}
						QModelIndex iCtrlNode = findChildNode( nif, iRoot, nodeName );
						if ( iCtrlNode.isValid() )
						{
							if ( ! controlledNodes.contains( iCtrlNode ) )
								controlledNodes.append( iCtrlNode );
						}
						else
						{
							if ( ! missingNodes.contains( nodeName ) )
								missingNodes << nodeName;
						}
					}

					bool oldHoldUpdates = nif->holdUpdates(true);

					if ( ! iMultiTransformer.isValid() )
						iMultiTransformer = attachController( nif, iRoot, T_NIMULTITARGETTRANSFORMCONTROLLER, true );
					if ( ! iCtrlManager.isValid() )
						iCtrlManager = attachController( nif, iRoot, T_NICONTROLLERMANAGER, true );
					
					setLinkArray( nif, iMultiTransformer, TA_EXTRATARGETS, controlledNodes );
					
					QPersistentModelIndex iObjPalette = nif->getBlock( nif->getLink( iCtrlManager, TA_OBJECTPALETTE ), "NiDefaultAVObjectPalette" );
					if ( ! iObjPalette.isValid() )
					{
						iObjPalette = nif->insertNiBlock( "NiDefaultAVObjectPalette", nif->getBlockNumber( iCtrlManager ) + 1, true );
						nif->setLink( iCtrlManager, TA_OBJECTPALETTE, nif->getBlockNumber( iObjPalette ) );
					}
					
					setNameLinkArray( nif, iObjPalette, TA_OBJS, controlledNodes );

					if (!oldHoldUpdates)
						nif->holdUpdates(false);
				}
				
				bool oldHoldUpdates = nif->holdUpdates(true);

				QMap<qint32,qint32> map = kf.moveAllNiBlocks( nif );
				int iMultiTransformerIdx = nif->getBlockNumber( iMultiTransformer );
				foreach ( qint32 lSeq, seqLinks )
				{
					qint32 nSeq = map.value( lSeq );
					int numSeq = nif->get<int>( iCtrlManager, TA_NUMCONTROLLERSEQUENCES );
					nif->set<int>( iCtrlManager, TA_NUMCONTROLLERSEQUENCES, numSeq+1 );
					nif->updateArray( iCtrlManager, TA_CONTROLLERSEQUENCES );
					nif->setLink( nif->getIndex( iCtrlManager, TA_CONTROLLERSEQUENCES ).child( numSeq, 0 ), nSeq );
					QModelIndex iSeq = nif->getBlock( nSeq, T_NICONTROLLERSEQUENCE );
					nif->setLink( iSeq, TA_MANAGER, nif->getBlockNumber( iCtrlManager ) );

					QModelIndex iCtrlBlcks = nif->getIndex( iSeq, TA_CONTROLLEDBLOCKS );
					for ( int r = 0; r < nif->rowCount( iCtrlBlcks ); r++ )
					{
						QModelIndex iCtrlBlck = iCtrlBlcks.child( r, 0 );
						if ( nif->getLink( iCtrlBlck, TA_CONTROLLER ) == -1 )
							nif->setLink( iCtrlBlck, TA_CONTROLLER, iMultiTransformerIdx );
					}
				}

				if (!oldHoldUpdates)
					nif->holdUpdates(false);

				if ( ! missingNodes.isEmpty() )
				{
					qWarning() << Spell::tr("The following controlled nodes were not found in the nif:");
					foreach ( QString nn, missingNodes )
						qWarning() << nn;
				}

				//return iRoot;
			}
			catch ( QString e )
			{
				qWarning( e.toAscii() );
			}
		}
		return index;
	}
	
	static QModelIndex findChildNode( const NifModel * nif, const QModelIndex & parent, const QString & name )
	{
		if ( ! nif->inherits( parent, T_NIAVOBJECT ) )
			return QModelIndex();
		
		QString thisName = nif->get<QString>( parent, TA_NAME );
		if ( thisName == name )
			return parent;
		
		foreach ( qint32 l, nif->getChildLinks( nif->getBlockNumber( parent ) ) )
		{
			QModelIndex child = findChildNode( nif, nif->getBlock( l ), name );
			if ( child.isValid() )
				return child;
		}
		
		return QModelIndex();
	}
	
	static QModelIndex findRootTarget( const NifModel * nif, const QString & name )
	{
		foreach ( qint32 l, nif->getRootLinks() )
		{
			QModelIndex root = findChildNode( nif, nif->getBlock( l ), name );
			if ( root.isValid() )
				return root;
		}
		
		return QModelIndex();
	}
	
	static QModelIndex findController( const NifModel * nif, const QModelIndex & node, const QString & ctrltype )
	{
		foreach ( qint32 l, nif->getChildLinks( nif->getBlockNumber( node ) ) )
		{
			QModelIndex iCtrl = nif->getBlock( l, T_NITIMECONTROLLER );
			if ( iCtrl.isValid() )
			{
				if ( nif->inherits( iCtrl, ctrltype ) )
					return iCtrl;
				else
				{
					iCtrl = findController( nif, iCtrl, ctrltype );
					if ( iCtrl.isValid() )
						return iCtrl;
				}
			}
		}
		return QModelIndex();
	}
	
	static QModelIndex attachController( NifModel * nif, const QPersistentModelIndex & iNode, const QString & ctrltype, bool fast = false )
	{
		QModelIndex iCtrl = nif->insertNiBlock( ctrltype, nif->getBlockNumber( iNode ) + 1, fast );
		if ( ! iCtrl.isValid() )
			return QModelIndex();
		
		qint32 oldctrl = nif->getLink( iNode, TA_CONTROLLER );
		nif->setLink( iNode, TA_CONTROLLER, nif->getBlockNumber( iCtrl ) );
		nif->setLink( iCtrl, TA_NEXTCONTROLLER, oldctrl );
		nif->setLink( iCtrl, TA_TARGET, nif->getBlockNumber( iNode ) );
		nif->set<int>( iCtrl, TA_FLAGS, 8 );
		
		return iCtrl;
	}
	
	static void setLinkArray( NifModel * nif, const QModelIndex & iParent, const QString & array, const QList< QPersistentModelIndex > & iBlocks )
	{
		QModelIndex iNum = nif->getIndex( iParent, QString( "Num %1" ).arg( array ) );
		QModelIndex iArray = nif->getIndex( iParent, array );
		
		if ( ! iNum.isValid() || ! iArray.isValid() )
			throw QString( Spell::tr("array %1 not found") ).arg( array );
		
		QVector<qint32> links = nif->getLinkArray( iArray );
		
		foreach ( QModelIndex iBlock, iBlocks )
			if ( ! links.contains( nif->getBlockNumber( iBlock ) ) )
				links.append( nif->getBlockNumber( iBlock ) );
		
		nif->set<int>( iNum, links.count() );
		nif->updateArray( iArray );
		nif->setLinkArray( iArray, links );
	}
	
	static void setNameLinkArray( NifModel * nif, const QModelIndex & iParent, const QString & array, const QList< QPersistentModelIndex > & iBlocks )
	{
		QModelIndex iNum = nif->getIndex( iParent, QString( "Num %1" ).arg( array ) );
		QModelIndex iArray = nif->getIndex( iParent, array );
		
		if ( ! iNum.isValid() || ! iArray.isValid() )
			throw QString( Spell::tr("array %1 not found") ).arg( array );
		
		QList< QPersistentModelIndex > blocksToAdd;
		
		foreach ( QPersistentModelIndex idx, iBlocks )
		{
			QString name = nif->get<QString>( idx, TA_NAME );
			int r;
			for ( r = 0; r < nif->rowCount( iArray ); r++ )
			{
				if ( nif->get<QString>( iArray.child( r, 0 ), TA_NAME ) == name )
					break;
			}
			if ( r == nif->rowCount( iArray ) )
				blocksToAdd << idx;
		}
		
		int r = nif->get<int>( iNum );
		nif->set<int>( iNum, r + blocksToAdd.count() );
		nif->updateArray( iArray );
		foreach ( QPersistentModelIndex idx, blocksToAdd )
		{
			nif->set<QString>( iArray.child( r, 0 ), TA_NAME, nif->get<QString>( idx, TA_NAME ) );
			nif->setLink( iArray.child( r, 0 ), TA_AVOBJECT, nif->getBlockNumber( idx ) );
			r++;
		}
	}
};

REGISTER_SPELL( spAttachKf )

//! Convert quaternions to euler rotations.
/*!
 * There doesn't seem to be much use for this - most official meshes use
 * Quaternions, and a spell going the other way (ZYX to Quat) might be
 * of more use. Or, they can be converted in a modelling program.
 *
 * Also, since Quaternions can't store tangents (or can they?), quadratic
 * keys are out, leaving linear and tension-bias-continuity to be converted.
 */
class spConvertQuatsToEulers : public Spell
{
public:
	QString name() const { return Spell::tr("Convert Quat- to ZYX-Rotations"); }
	QString page() const { return Spell::tr("Animation"); }
	
	bool isApplicable( const NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iBlock = nif->getBlock( index, T_NIKEYFRAMEDATA );
		return iBlock.isValid() && nif->get<int>( iBlock, TA_ROTATIONTYPE ) != 4;
	}
	
	/*
	QModelIndex cast( NifModel * nif, const QModelIndex & index )
	{
		QModelIndex iQuats = nif->getIndex( index, TA_QUATERNIONKEYS );
		int rotationType = nif->get<int>( index, TA_ROTATIONTYPE );
		nif->set<int>( index, TA_ROTATIONTYPE, 4 );
		nif->updateArray( index, TA_XYZROTATIONS );
		QModelIndex iRots = nif->getIndex( index, TA_XYZROTATIONS );

		for( int i = 0; i < 3; i++ )
		{
			QModelIndex iRot = iRots.child( i, 0 );
			nif->set<int>( iRot, "Num Keys", nif->get<int>(index, "Num Rotation Keys") );
			nif->set<int>( iRot, TA_INTERPOLATION, rotationType );
			nif->updateArray( iRot, TA_KEYS );
		}

		for ( int q = 0; q < nif->rowCount( iQuats ); q++ )
		{
			QModelIndex iQuat = iQuats.child( q, 0 );
			
			float time = nif->get<float>( iQuat, TA_TIME );
			Quat value = nif->get<Quat>( iQuat, TA_VALUE );

			Matrix tlocal;
			tlocal.fromQuat( value );
			
			float x, y, z;
			tlocal.toEuler( x, y, z );

			QModelIndex xRot = iRots.child( 0, 0 );
			QModelIndex yRot = iRots.child( 1, 0 );
			QModelIndex zRot = iRots.child( 2, 0 );
		
			xRot = nif->getIndex( xRot, TA_KEYS );

		}

		return index;
	}*/
};

//REGISTER_SPELL( spConvertQuatsToEulers )

