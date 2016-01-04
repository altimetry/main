#if !defined MODEL_H
#define MODEL_H

#include <QtCore>


class CModel : public QObject
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif
		
	using base_t = QObject;

	// construction / destruction

public:
	explicit CModel();

	virtual ~CModel();

	// access 

	// operations
};



#endif	//MODEL_H
