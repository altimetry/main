/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Solutions component.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#pragma clang diagnostic ignored "-Wall"
#endif

#include <QCoreApplication>
#include <QStringList>
#include <QFile>
#include <QTimer>
#include <QDir>
#include <pwd.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <sys/stat.h>
#include <QMap>
#include <QSettings>
#include <QProcess>

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif


#include "common/QtUtilsIO.h"
#include "qtservice.h"
#include "qtservice_p.h"
#include "qtunixsocket.h"
#include "qtunixserversocket.h"




std::string QtServiceController::ServiceAutoStartFile() const
{
#if defined(Q_OS_MAC)
    return q2a( QDir::homePath() + "/Library/LaunchAgents/" + serviceName() + ".plist" );
#else
    return "/etc/init.d/" + q2a( serviceName() );
#endif
}



static const std::string full_path_place_holder_to_find = "#QtServiceFullPath#";
static const bool chkconfig_system = IsFile( "/usr/sbin/chkconfig" );
static const QString template_resource =
#if defined (Q_OS_MAC)
        "://qt-service.plist"
#else
        chkconfig_system ? "://qt-sysv-service.sh" : "://qt-service.sh"
#endif
        ;



#if defined (Q_OS_MAC)


bool QtServiceController::EnableAutoStart( bool enable, const std::string &user_name )
{
    const std::string service_auto_start_file = ServiceAutoStartFile();

    bool ok = !IsFile( service_auto_start_file ) || RemoveFile( service_auto_start_file );
    if ( !enable || !ok )
    {
        return ok;
    }
    
    // Create service auto-start file from template

    QFile template_file( template_resource );
    if ( !template_file.open( QFile::ReadOnly ) )
    {
        assert__( false );      //should never happen
        return false;
    }
    QString template_file_content( template_file.readAll() );
    std::string new_content = q2a( template_file_content );

    const std::string service_path = q2a( serviceFilePath() );
    new_content = replace( new_content, full_path_place_holder_to_find, service_path );

    ok =
            MakeDirectory( GetDirectoryFromPath( service_auto_start_file ) ) && 
            Write2File( new_content, service_auto_start_file );

    //only change mAutoStartEnabled state if no errors ocurred
    //
    if ( ok )
        mAutoStartEnabled = enable;

    return ok;
}

#else       //Q_OS_MAC



static const std::string service_name_place_holder_to_find = "#QtServiceName#";
static const std::string user_name_place_holder_to_find = "#QtServiceUserName#";

// With System V, to test in command line, use:
//
//  systemctl status RadsService.service
//  systemctl start RadsService.service
//  systemctl stop RadsService.service
//
//  (where "RadsService" is a service name example)
//
bool QtServiceController::EnableAutoStart( bool enable, const std::string &user_name )
{
    const std::string service_name = q2a( serviceName() );
    const std::string install_script = "/tmp/" + service_name + "-install-auto-start.sh";
    const std::string uninstall_script = "/tmp/" + service_name + "-uninstall-auto-start.sh";
    const std::string service_auto_start_file = ServiceAutoStartFile();
    const std::string temporary_service_auto_start_file = "/tmp/" + service_name;

    const std::string &script_to_execute = enable ? install_script : uninstall_script;

    //lambdas

    auto clean_up = [&enable, &uninstall_script, &install_script, &temporary_service_auto_start_file]( bool result )
    {
        system( ( "rm " + uninstall_script ).c_str() );
        if ( enable )
        {
            system( ( "rm " + install_script ).c_str() );
            system( ( "rm " + temporary_service_auto_start_file ).c_str() );
        }

        return result;
    };


    // Create uninstall script

    std::string script_content;
    if ( chkconfig_system )
        script_content = "chkconfig --del " + service_name;
    else
        script_content = "update-rc.d -f " + service_name + " remove";
    script_content += "\n";
    if ( IsFile( service_auto_start_file ) )
    {
        script_content += ( "rm " + service_auto_start_file );
        script_content += "\n";
    }
    if ( !Write2File( script_content, uninstall_script ) ||
         system( ( "chmod 755 "+ uninstall_script ).c_str() ) != 0 )
        return clean_up( false );

    if ( enable )
    {
        // Create service auto-start file from template

        QFile template_file( template_resource );
        if ( !template_file.open( QFile::ReadOnly ) )
        {
            assert__( false );      //should never happen
            return clean_up( false );
        }
        QString template_file_content( template_file.readAll() );
        std::string new_content = q2a( template_file_content );

        const std::string service_path = q2a( serviceFilePath() );
        new_content = replace( new_content, full_path_place_holder_to_find, service_path );
        new_content = replace( new_content, service_name_place_holder_to_find, service_name );
        new_content = replace( new_content, user_name_place_holder_to_find, user_name );

        if ( !Write2File( new_content, temporary_service_auto_start_file ) ||
             system( ( "chmod 755 "+ temporary_service_auto_start_file ).c_str() ) != 0 )
            return clean_up( false );


        // Create install script

        script_content = uninstall_script;
        script_content += "\n";
        script_content += ( "cp " + temporary_service_auto_start_file + " " + service_auto_start_file );
        script_content += "\n";
        script_content += "chmod 755 " + service_auto_start_file;
        script_content += "\n";
        if ( chkconfig_system )
            script_content += "chkconfig --add " + service_name;
        else
            script_content += "update-rc.d " + service_name + " defaults";
        script_content += "\n";
        if ( !Write2File( script_content, install_script ) ||
             system( ( "chmod 755 "+ install_script ).c_str() ) != 0 )
            return clean_up( false );
    }

    if ( !ExecuteCommand( true, script_to_execute ) )
        return clean_up( false );

    mAutoStartEnabled = enable;

    return clean_up( true );
}

#if 0
static const std::string bash_profile = q2a( QDir::homePath() + "/.bash_profile" );

bool EnableAutoStartWithBashProfile( const std::string &service_path, bool enable, const std::string &plist_template, const std::string &user_name )
{
    Q_UNUSED( plist_template );    Q_UNUSED( user_name );

    // If the user manually changed bash_profile, this is the minimal content
    //  that enables auto-start, and that we want to erase
    //
    const std::string minimal_content_to_find = service_path;

    // Full content to delete/write if the user did not change it
    //
    const std::string content_to_write = "\n\n# RadsService\n" + minimal_content_to_find + " &\n\n";

    std::string new_bash_profile_content;

    if ( IsFile( bash_profile ) )
    {
        std::string bash_profile_content;
        if ( !Read2String( bash_profile_content, bash_profile ) )
            return false;

        // Always start by deleting any existing service reference, as far as we can find it
        //  if the user manually changed .bash_profile

        new_bash_profile_content = replace( bash_profile_content, content_to_write, empty_string() );
        if ( new_bash_profile_content.length() == bash_profile_content.length() )   //full content not found
        {
            //try to find minimal content
            //
            auto pos = bash_profile_content.find( minimal_content_to_find );
            if ( pos != std::string::npos )
            {
                auto end_pos = bash_profile_content.find( "\n", pos + minimal_content_to_find.length() );
                std::string to_delete;
                if ( end_pos == std::string::npos )
                    to_delete = minimal_content_to_find;
                else
                    to_delete = bash_profile_content.substr( pos, end_pos - pos );

                new_bash_profile_content = replace( bash_profile_content, to_delete, empty_string() );
            }
        }
    }

    if ( enable )
    {
        new_bash_profile_content += content_to_write;
    }

    return Write2File( new_bash_profile_content, bash_profile );
}
#endif       //0

#endif       //Q_OS_MAC



static QString encodeName(const QString &name, bool allowUpper = false)
{
    QString n = name.toLower();
    QString legal = QLatin1String("abcdefghijklmnopqrstuvwxyz1234567890");
    if (allowUpper)
        legal += QLatin1String("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    int pos = 0;
    while (pos < n.size()) {
	if (legal.indexOf(n[pos]) == -1)
	    n.remove(pos, 1);
	else
	    ++pos;
    }
    return n;
}

static QString login()
{
    QString l;
    uid_t uid = getuid();
    passwd *pw = getpwuid(uid);
    if (pw)
        l = QString(pw->pw_name);
    return l;
}

static QString socketPath(const QString &serviceName)
{
    QString sn = encodeName(serviceName);
    return QString(QLatin1String("/var/tmp/") + sn + QLatin1String(".") + login());
}

static bool sendCmd(const QString &serviceName, const QString &cmd)
{
    bool retValue = false;
    QtUnixSocket sock;
    if (sock.connectTo(socketPath(serviceName))) {
        sock.write(QString(cmd+"\r\n").toLatin1().constData());
	sock.flush();
        sock.waitForReadyRead(-1);
        QString reply = sock.readAll();
        if (reply == QLatin1String("true"))
            retValue = true;
	sock.close();
    }
    return retValue;
}

static QString absPath(const QString &path)
{
    QString ret;
    if (path[0] != QChar('/')) { // Not an absolute path
        int slashpos;
        if ((slashpos = path.lastIndexOf('/')) != -1) { // Relative path
            QDir dir = QDir::current();
            dir.cd(path.left(slashpos));
            ret = dir.absolutePath();
        } else { // Need to search $PATH
            char *envPath = ::getenv("PATH");
            if (envPath) {
                QStringList envPaths = QString::fromLocal8Bit(envPath).split(':');
                for (int i = 0; i < envPaths.size(); ++i) {
                    if (QFile::exists(envPaths.at(i) + QLatin1String("/") + QString(path))) {
                        QDir dir(envPaths.at(i));
                        ret = dir.absolutePath();
                        break;
                    }
                }
            }
        }
    } else {
        QFileInfo fi(path);
        ret = fi.absolutePath();
    }
    return ret;
}

QString QtServiceBasePrivate::filePath() const
{
    QString ret;
    if (args.isEmpty())
        return ret;
    QFileInfo fi(args[0]);
    QDir dir(absPath(args[0]));
    return dir.absoluteFilePath(fi.fileName());
}


QString QtServiceController::serviceDescription() const
{
    QSettings settings(QSettingsServiceScope, QtServiceController::smOrganizationKey);
    settings.beginGroup(QtServiceController::smServicesGroupKey);
    settings.beginGroup(serviceName());

    QString desc = settings.value(QtServiceController::smDescriptionKey).toString();

    settings.endGroup();
    settings.endGroup();

    return desc;
}

QtServiceController::StartupType QtServiceController::startupType() const
{
    QSettings settings(QSettingsServiceScope, QtServiceController::smOrganizationKey);
    settings.beginGroup(QtServiceController::smServicesGroupKey);
    settings.beginGroup(serviceName());

    StartupType startupType = (StartupType)settings.value("startupType").toInt();

    settings.endGroup();
    settings.endGroup();

    return startupType;
}

QString QtServiceController::serviceFilePath() const
{
    QSettings settings(QSettingsServiceScope, QtServiceController::smOrganizationKey);
    settings.beginGroup(QtServiceController::smServicesGroupKey);
    settings.beginGroup(serviceName());

    QString path = settings.value(QtServiceController::smPathKey).toString();

    settings.endGroup();
    settings.endGroup();

    return path;
}

bool QtServiceController::uninstall()
{
    QSettings settings(QSettingsServiceScope, QtServiceController::smOrganizationKey);
    settings.beginGroup(QtServiceController::smServicesGroupKey);

    QString account = settings.value(QtServiceController::smAccountKey).toString();

    const bool remove_auto_start_ok =
            ( startupType() != AutoStartup )
            ||
            EnableAutoStart( false, q2a( account ) );

    if ( !remove_auto_start_ok )
    {
        fprintf( stderr, "Cannot remove auto-start of \"%s\". Cannot read/write to: %s. Check permissions.\n",
                serviceName().toLatin1().constData(),
                ServiceAutoStartFile().c_str() );
    }


    settings.remove(serviceName());

    settings.endGroup();
    settings.sync();

    QSettings::Status ret = settings.status();
    if (ret == QSettings::AccessError) {
        fprintf(stderr, "Cannot uninstall \"%s\". Cannot write to: %s. Check permissions.\n",
                serviceName().toLatin1().constData(),
                settings.fileName().toLatin1().constData());
    }

    // Do not return remove_auto_start_ok &&... Auto-start status can be queried from controller,
    //  and if auto-start fails, the service can still be operational
    //
    return ret == QSettings::NoError;
}


bool QtServiceController::start(const QStringList &arguments)
{
    if (!isInstalled())
        return false;
    if (isRunning())
        return false;
    return QProcess::startDetached(serviceFilePath(), arguments);
}

bool QtServiceController::stop()
{
    return sendCmd(serviceName(), QLatin1String("terminate"));
}

bool QtServiceController::pause()
{
    return sendCmd(serviceName(), QLatin1String("pause"));
}

bool QtServiceController::resume()
{
    return sendCmd(serviceName(), QLatin1String("resume"));
}

bool QtServiceController::sendCommand(int code)
{
    return sendCmd(serviceName(), QString(QLatin1String("num:") + QString::number(code)));
}

bool QtServiceController::isInstalled() const
{
    QSettings settings(QSettingsServiceScope, QtServiceController::smOrganizationKey);
    settings.beginGroup(QtServiceController::smServicesGroupKey);

    QStringList list = settings.childGroups();

    settings.endGroup();

    QStringListIterator it(list);
    while (it.hasNext()) {
        if (it.next() == serviceName())
            return true;
    }

    return false;
}

bool QtServiceController::isRunning() const
{
    QtUnixSocket sock;
    if (sock.connectTo(socketPath(serviceName())))
	return true;
    return false;
}




///////////////////////////////////

class QtServiceSysPrivate : public QtUnixServerSocket
{
    Q_OBJECT
public:
    QtServiceSysPrivate();
    ~QtServiceSysPrivate();

    char *ident;

    QtServiceBase::ServiceFlags serviceFlags;

protected:
#if QT_VERSION >= 0x050000
    void incomingConnection(qintptr socketDescriptor);
#else
    void incomingConnection(int socketDescriptor);
#endif

private slots:
    void slotReady();
    void slotClosed();

private:
    QString getCommand(const QTcpSocket *socket);
    QMap<const QTcpSocket *, QString> cache;
};

QtServiceSysPrivate::QtServiceSysPrivate()
    : QtUnixServerSocket(), ident(0), serviceFlags(0)
{
}

QtServiceSysPrivate::~QtServiceSysPrivate()
{
    if (ident)
	delete[] ident;
}

#if QT_VERSION >= 0x050000
void QtServiceSysPrivate::incomingConnection(qintptr socketDescriptor)
#else
void QtServiceSysPrivate::incomingConnection(int socketDescriptor)
#endif
{
    QTcpSocket *s = new QTcpSocket(this);
    s->setSocketDescriptor(socketDescriptor);
    connect(s, SIGNAL(readyRead()), this, SLOT(slotReady()));
    connect(s, SIGNAL(disconnected()), this, SLOT(slotClosed()));
}

void QtServiceSysPrivate::slotReady()
{
    QTcpSocket *s = (QTcpSocket *)sender();
    cache[s] += QString(s->readAll());
    QString cmd = getCommand(s);
    while (!cmd.isEmpty()) {
        bool retValue = false;
	if (cmd == QLatin1String("terminate")) {
            if (!(serviceFlags & QtServiceBase::CannotBeStopped)) {
                QtServiceBase::instance()->stop();
                QCoreApplication::instance()->quit();
                retValue = true;
            }
        } else if (cmd == QLatin1String("pause")) {
            if (serviceFlags & QtServiceBase::CanBeSuspended) {
                QtServiceBase::instance()->pause();
                retValue = true;
            }
        } else if (cmd == QLatin1String("resume")) {
            if (serviceFlags & QtServiceBase::CanBeSuspended) {
                QtServiceBase::instance()->resume();
                retValue = true;
            }
        } else if (cmd == QLatin1String("alive")) {
            retValue = true;
        } else if (cmd.length() > 4 && cmd.left(4) == QLatin1String("num:")) {
	    cmd = cmd.mid(4);
            QtServiceBase::instance()->processCommand(cmd.toInt());
            retValue = true;
	}
        QString retString;
        if (retValue)
            retString = QLatin1String("true");
        else
            retString = QLatin1String("false");
        s->write(retString.toLatin1().constData());
        s->flush();
	cmd = getCommand(s);
    }
}

void QtServiceSysPrivate::slotClosed()
{
    QTcpSocket *s = (QTcpSocket *)sender();
    s->deleteLater();
}

QString QtServiceSysPrivate::getCommand(const QTcpSocket *socket)
{
    int pos = cache[socket].indexOf("\r\n");
    if (pos >= 0) {
	QString ret = cache[socket].left(pos);
	cache[socket].remove(0, pos+2);
	return ret;
    }
    return "";
}

#include "qtservice_unix.moc"

bool QtServiceBasePrivate::sysInit()
{
    sysd = new QtServiceSysPrivate;
    sysd->serviceFlags = serviceFlags;
    // Restrict permissions on files that are created by the service
    ::umask(027);

    return true;
}

void QtServiceBasePrivate::sysSetPath()
{
    if (sysd)
        sysd->setPath(socketPath(controller.serviceName()));
}

void QtServiceBasePrivate::sysCleanup()
{
    if (sysd) {
        sysd->close();
        delete sysd;
        sysd = 0;
    }
}

bool QtServiceBasePrivate::start()
{
    if (sendCmd(controller.serviceName(), "alive")) {
        // Already running
        return false;
    }
    // Could just call controller.start() here, but that would fail if
    // we're not installed. We do not want to strictly require installation.
    ::setenv("QTSERVICE_RUN", "1", 1);  // Tell the detached process it's it
    return QProcess::startDetached(filePath(), args.mid(1), "/");
}

bool QtServiceBasePrivate::install(const QString &account, const QString &password)
{
    Q_UNUSED(password)
    QSettings settings(QSettingsServiceScope, QtServiceController::smOrganizationKey);
    settings.beginGroup(QtServiceController::smServicesGroupKey);
    settings.beginGroup(controller.serviceName());

    settings.setValue(QtServiceController::smPathKey, filePath());
    settings.setValue(QtServiceController::smDescriptionKey, serviceDescription);
    settings.setValue(QtServiceController::smAutomaticStartupKey, startupType);
    settings.setValue(QtServiceController::smAccountKey, account);

    settings.endGroup();
    settings.endGroup();
    settings.sync();

    QSettings::Status ret = settings.status();
    if (ret == QSettings::AccessError) {
        fprintf(stderr, "Cannot install \"%s\". Cannot write to: %s. Check permissions.\n",
                controller.serviceName().toLatin1().constData(),
                settings.fileName().toLatin1().constData());
    }

    const bool auto_start_ok =
            ( startupType != QtServiceController::AutoStartup )
            ||
            controller.EnableAutoStart( true, q2a( account ) );

    if ( !auto_start_ok )
    {
        fprintf( stderr, "Cannot configure auto-start of \"%s\". Cannot read/write to: %s. Check permissions.\n",
                controller.serviceName().toLatin1().constData(),
                controller.ServiceAutoStartFile().c_str() );
    }

    // Do not return auto_start_ok && ... Auto-start status can be queried from controller,
    //  and if auto-start fails, the service can still be operational
    //
    return ret == QSettings::NoError;
}

void QtServiceBase::logMessage(const QString &message, QtServiceBase::MessageType type,
			    int, uint, const QByteArray &)
{
    if (!d_ptr->sysd)
        return;
    int st;
    switch(type) {
        case QtServiceBase::Error:
	    st = LOG_ERR;
	    break;
        case QtServiceBase::Warning:
            st = LOG_WARNING;
	    break;
        default:
	    st = LOG_INFO;
    }
    if (!d_ptr->sysd->ident) {
        QString tmp = encodeName(serviceName(), true);
	int len = tmp.toLocal8Bit().size();
	d_ptr->sysd->ident = new char[len+1];
	d_ptr->sysd->ident[len] = '\0';
	::memcpy(d_ptr->sysd->ident, tmp.toLocal8Bit().constData(), len);
    }
    openlog(d_ptr->sysd->ident, LOG_PID, LOG_DAEMON);
    foreach(QString line, message.split('\n'))
        syslog(st, "%s", line.toLocal8Bit().constData());
    closelog();
}

void QtServiceBase::setServiceFlags(QtServiceBase::ServiceFlags flags)
{
    if (d_ptr->serviceFlags == flags)
        return;
    d_ptr->serviceFlags = flags;
    if (d_ptr->sysd)
        d_ptr->sysd->serviceFlags = flags;
}

