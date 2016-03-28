#include "new-gui/brat/stdafx.h"

#include <osg/Group>

#include "Globe.h"

#include "GlobeUpdateCallback.h"



//called from OSG (update) thread
//
//virtual
void CNodeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
	QMutexLocker locker( &m_mutex );

	pre_update( node, nv );

	if ( !m_callbacks.empty() )
	{
		//QMutexLocker locker( &m_mutex );		//prevent child callbacks map assignment/deletion while executing its items...

		for ( auto &call : m_callbacks )
			call.second->pre_update( node, nv );
	}

	base_t::operator()( node, nv ); //equivalent to traverse( node, nv );

	post_update( node, nv );
}





//called from GUI (main) thread
//
bool CRootUpdateCallback::AssignNodeToAdd( osg::Node *node_to_add )
{
    return AssignData( node_to_add, m_node_to_add );
}
bool CRootUpdateCallback::AssignNodeToDelete( osg::Node *node_to_delete )
{
    return AssignData( node_to_delete, m_node_to_delete );
}
bool CRootUpdateCallback::AssignImageLayersChanged()
{
	if ( mImageLayersChanged )
		return false;

    return AssignData( true, mImageLayersChanged );
}



//virtually called by base from OSG (update) thread
//
//virtual
bool CRootUpdateCallback::pre_update( osg::Node* node, osg::NodeVisitor* nv )
{																				assert__( dynamic_cast<osg::Group*>( node ) == mGlobeIn.mRootNode );
	if ( !base_t::pre_update( node, nv ) )
		return false;

	//QMutexLocker locker( &m_mutex );											

	bool result = true;
	try {
		if ( m_node_to_add )
		{
			mGlobeIn.mRootNode->addChild( m_node_to_add );

			osg::notify( osg::NOTICE ) << "ROOT update callback (pre traverse): " << "ADDED node " << m_node_to_add->getName() << std::endl;

			m_node_to_add = nullptr;
		}

		if ( m_node_to_delete )
		{
			std::string name = m_node_to_delete->getName();

			mGlobeIn.mRootNode->removeChild( m_node_to_delete );

			osg::notify( osg::NOTICE ) << "ROOT update callback (pre traverse): " << "DELETED node " << name << std::endl;

			m_node_to_delete = nullptr;
		}

		//if ( mImageLayersChanged )
		//{
		//	mGlobeIn.ImageLayersChanged();
		//	mImageLayersChanged = false;
		//}
	}
	catch ( ... )
	{
		result = false;

		m_node_to_add = nullptr;
		m_node_to_delete = nullptr;

		osg::notify( osg::WARN ) << "ROOT update callback (pre traverse): " << "!!! EXCEPTION !!!" << std::endl;
		emit Root_UpdateCallback_exception_Signal( "Could not execute the graphic scene operation." );
	}

	return result;
}



////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////

#include "moc_GlobeUpdateCallback.cpp"
