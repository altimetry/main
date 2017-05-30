/*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#if !defined COMMON_APPLICATION_LOGGER_INTERFACE_H
#define COMMON_APPLICATION_LOGGER_INTERFACE_H



struct ApplicationLoggerInterface : non_copyable
{
	virtual void SetEnabled( bool enable ) = 0;

	
	virtual int NotifyLevel() const = 0;

	virtual void SetNotifyLevel( int level ) = 0;


	virtual void LogMessage( const QString &msg, QtMsgType level ) = 0;


	virtual const std::vector< std::string >& LevelNames() const = 0;

	virtual const QHash< int, QColor >& SeverityToColorTable() const = 0;

	virtual const QHash< int, QString >& SeverityToPromptTable() const = 0;
};





#endif	//COMMON_APPLICATION_LOGGER_INTERFACE_H
