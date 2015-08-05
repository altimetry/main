/*
* 
*
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


#ifndef _Callback_h_
#define _Callback_h_

#include "BratObject.h"
using namespace brathl;

namespace brathl
{


template < class Class, typename ReturnType, typename Parameter1 , typename Parameter2 , typename Parameter3 , typename Parameter4 >
class CallBack4 : public CBratObject
{

   public:

    typedef ReturnType (Class::*Method)(Parameter1, Parameter2, Parameter3, Parameter4);

    CallBack4(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3, Parameter4 parameter4)
    {
       return (class_instance->*method)(parameter1, parameter2, parameter3, parameter4);
    };

    ReturnType execute(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3, Parameter4 parameter4)
    {
       return operator()(parameter1, parameter2, parameter3, parameter4);
    };

    private:

      Class*  class_instance;
      Method  method;

};


template < class Class, typename ReturnType, typename Parameter1 , typename Parameter2, typename Parameter3>
//class CallBack  < Class, ReturnType, Parameter1, Parameter2, Parameter3>
class CallBack3 : public CBratObject
{

   public:

    typedef ReturnType (Class::*Method)(Parameter1, Parameter2, Parameter3);

    CallBack3(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3)
    {
       return (class_instance->*method)(parameter1, parameter2, parameter3);
    };

    ReturnType execute(Parameter1 parameter1, Parameter2 parameter2, Parameter3 parameter3)
    {
       return operator()(parameter1, parameter2, parameter3);
    };

    private:

      Class*  class_instance;
      Method  method;

};


template < class Class, typename ReturnType, typename Parameter1, typename Parameter2 >
//class CallBack < Class, ReturnType, Parameter1, Parameter2, void >
class CallBack2 : public CBratObject
{

   public:

    typedef ReturnType (Class::*Method)(Parameter1,Parameter2);

    CallBack2(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()(Parameter1 parameter1, Parameter2 parameter2)
    {
       return (class_instance->*method)(parameter1, parameter2);
    };

    ReturnType execute(Parameter1 parameter1, Parameter2 parameter2)
    {
       return operator()(parameter1, parameter2);
    };

    private:

      Class*  class_instance;
      Method  method;

};


template < class Class, typename ReturnType, typename Parameter>
//class CallBack < Class, ReturnType, Parameter, void>
class CallBack1 : public CBratObject
{

   public:

    typedef ReturnType (Class::*Method)(Parameter);

    CallBack1(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()(Parameter parameter)
    {
       return (class_instance->*method)(parameter);
    };

    ReturnType execute(Parameter parameter)
    {
       return operator()(parameter);
    };

    private:

      Class*  class_instance;
      Method  method;

};

template < class Class, typename ReturnType>
//class CallBack < Class, ReturnType, void>
class CallBack0 : public CBratObject
{

   public:

    typedef ReturnType (Class::*Method)(void);

    CallBack0(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()()
    {
       return (class_instance->*method)();
    };

    ReturnType execute()
    {
       return operator()();
    };

    private:

      Class*  class_instance;
      Method  method;

};

template < class Class, typename ReturnType>
class CallBackv : public CBratObject
{

   public:

    typedef ReturnType (Class::*Method)(void);

    CallBackv(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()()
    {
       (class_instance->*method)();
    };

    ReturnType execute()
    {
       operator()();
    };

    private:

      Class*  class_instance;
      Method  method;

};

template < class Class, typename ReturnType, typename Parameter>
class CallBackv1 : public CBratObject
{

   public:

    typedef ReturnType (Class::*Method)(Parameter);

    CallBackv1(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()(Parameter parameter)
    {
       (class_instance->*method)(parameter);
    };

    ReturnType execute(Parameter parameter)
    {
       operator()(parameter);
    };

    private:

      Class*  class_instance;
      Method  method;

};

template < class Class, typename ReturnType, typename Parameter1, typename Parameter2 >
class CallBackv2 : public CBratObject
{

   public:

    typedef ReturnType (Class::*Method)(Parameter1,Parameter2);

    CallBackv2(Class* _class_instance, Method _method)
    {
       class_instance = _class_instance;
       method         = _method;
    };

    ReturnType operator()(Parameter1 parameter1, Parameter2 parameter2)
    {
      (class_instance->*method)(parameter1, parameter2);
    };

    ReturnType execute(Parameter1 parameter1, Parameter2 parameter2)
    {
       operator()(parameter1, parameter2);
    };

    private:

      Class*  class_instance;
      Method  method;

};



} // end namespace

#endif
