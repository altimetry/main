#include "stdafx.h"

#include <QMainWindow>

#include <qgsapplication.h>

#include "../QtInterface.h"
#include "../Common/QtFileUtils.h"
#include "Workspaces/Dataset.h"

/*	
	TODO: unit tests out of this

	std::string dir = "L:\\project\\workspaces\\S3A - Copy";			//l:/P/Q/R/xyz
	//std::string dir = "l:////P\\\\Q\\\\F//..\\R\\xyz/\\/\\\\//";		//l:/P/Q/R/xyz
	std::string path = "/A/B";

	//CleanPath( dir );
	//qDebug() << CannonicalPath( dir ).c_str();
	//qDebug() << CleanedPath( dir ).c_str();
	NormalizePath( dir );
	qDebug() << dir.c_str();

	MakeDirectory( dir + path );

	//NormalizePath( dir );
	//qDebug() << dir.c_str();

	//QDir d( directory.c_str() );
	//QDir d;
    //return d.rmpath( (dir + path).c_str() );
	DeleteDirectory( dir );

	//DeletePath( dir, path );

	return 0;
*/

int main(int argc, char *argv[])
{
    QgsApplication a(argc, argv, true);

	//CConfiguration* m_config = new CConfiguration( "L:\\project\\workspaces\\W1\\Datasets\\setup.ini" );
	CConfiguration* m_config = new CConfiguration( "L:\\project\\workspaces\\S3A\\Datasets\\setup.ini" );
	CDataset *data = new CDataset("Datasets_FM_1");
	data->LoadConfig( m_config );


    QMainWindow w;
    w.show();

    return a.exec();
}
