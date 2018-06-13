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

#if !defined (COMMON_BRAT_VERSION_H)
#define COMMON_BRAT_VERSION_H

#include "Version.h"

#define BRAT_VERSION_STRING	VERSION_STRING
#define BRAT_VERSION_NUMBER VERSION_NUMBER


inline const std::string& brat_about_info()
{
	static const std::string info = R"***(<p align='center'><small>
More information can be found in <a href=http://www.altimetry.info>www.altimetry.info</a>
<br>
For support please contact us at <a href=mailto:brat.helpdesk@esa.int>brat.helpdesk@esa.int</a>
 </small></p>)***";

	return info;
}


#endif  //COMMON_BRAT_VERSION_H
