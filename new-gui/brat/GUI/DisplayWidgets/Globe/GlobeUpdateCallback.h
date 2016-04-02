#ifndef GUI_DISPLAY_WIDGETS_GLOBE_GLOBE_UPDATE_CALLBACK_H
#define GUI_DISPLAY_WIDGETS_GLOBE_GLOBE_UPDATE_CALLBACK_H

#include <osg/Callback>

#include "new-gui/Common/QtUtils.h"

class CGlobeWidget;



class COSGUpdateCallback : public QObject, public osg::NodeCallback
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types
		
protected:
	typedef QObject				qt_base_t;
	typedef osg::NodeCallback	osg_base_t;

	QMutex	m_mutex;

public:
	COSGUpdateCallback() : qt_base_t(), osg_base_t(), m_mutex( QMutex::NonRecursive )
	{}
	virtual ~COSGUpdateCallback()
	{}

protected:
	template< typename DATA >
	bool AssignData( const DATA &source, DATA &dest )
	{
		QMutexLocker locker( &m_mutex );		//assert__( assigned( source ) );

		dest = source;

		return true;
	}
	template< typename DATA >
	bool AssignData( DATA *source, std::vector< DATA* > &dest )
	{
		QMutexLocker locker( &m_mutex );		//assert__( assigned( source ) );

		dest.push_back( source );
		return true;
	}

    template< typename ID, typename DATA >
    bool AssignData( ID id, DATA *source, std::map< ID, DATA* > &dest )
    {
        QMutexLocker locker( &m_mutex );		//assert__( assigned( source ) );

        if ( dest.find( id ) != dest.end() )
             return false;

        dest[ id ] = source;

        return true;
    }
    template< typename ID, typename DATA >
    bool RemoveData( ID id, std::map< ID, DATA* > &dest )
    {
        QMutexLocker locker( &m_mutex );

        auto it = dest.find( id );
        if ( it == dest.end() )
             return false;

        delete it->second;
        dest.erase( it );

        return true;
    }

public:
	virtual bool pre_update( osg::Node* node, osg::NodeVisitor* nv ) = 0;		//does the scene graph update work (for node)
	virtual bool post_update( osg::Node* node, osg::NodeVisitor* nv ) = 0;		//does the scene graph update work (for node)

signals:

	void objectChangedSignal( const void *pnode );
};




class CNodeUpdateCallback : public COSGUpdateCallback
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	typedef COSGUpdateCallback base_t;

public:
    typedef size_t child_key_t;

protected:
    std::map< child_key_t, COSGUpdateCallback* >	m_callbacks;

public:
	CNodeUpdateCallback() : base_t()
    {}
    virtual ~CNodeUpdateCallback()
    {
		DestroyPointersAndContainer( m_callbacks );
	}

    bool AddChildCallback( child_key_t key, COSGUpdateCallback *callback_to_add )
    {
        return AssignData( key, callback_to_add, m_callbacks );
    }
    bool RemoveChildCallback( child_key_t key )
    {
        return RemoveData( key, m_callbacks );
    }

public:

    virtual bool empty() const
    {
        return m_callbacks.size() == 0;
    }

protected:
	virtual bool pre_update( osg::Node* node, osg::NodeVisitor* nv ) override
	{
		return true;
	}

public:
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv ) override;

signals:

    void objectDeletedSignal( const std::wstring &name );
	void Node_UpdateCallback_exception_Signal( const QString &msg );
};



class CRootUpdateCallback : public CNodeUpdateCallback
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	//types

	typedef CNodeUpdateCallback base_t;

	//instance data

	CGlobeWidget &mGlobeIn;

    osg::Node *m_node_to_add = nullptr;
    osg::Node *m_node_to_delete = nullptr;

	bool mImageLayersChanged = false;


public:
	CRootUpdateCallback( CGlobeWidget *globe ) 
		: base_t()
		, mGlobeIn( *globe )
    {}
    virtual ~CRootUpdateCallback()
    {}

public:
    bool AssignNodeToDelete( osg::Node *node_to_delete );
    bool AssignNodeToAdd( osg::Node *node_to_add );
    bool AssignImageLayersChanged();

protected:
    virtual bool pre_update( osg::Node* node, osg::NodeVisitor* nv ) override;
	virtual bool post_update( osg::Node* node, osg::NodeVisitor* nv ) override
	{
		return true;
	}

signals:

	void Root_UpdateCallback_exception_Signal( const QString &msg );
};




#endif // GUI_DISPLAY_WIDGETS_GLOBE_GLOBE_UPDATE_CALLBACK_H
