/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    OptionList.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __OPTIONLIST_H_
#define __OPTIONLIST_H_

#include <itkMacro.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "itkArray.h"

class OptionList
{
public:
  typedef std::multimap<std::string, std::string> OptionMap ;
  typedef std::vector<std::string> StringVector ;

  OptionList(int argc, char* argv[]) ;
  ~OptionList() {}

  class RequiredOptionMissing 
  {
  public:
    RequiredOptionMissing(const std::string tag)
    {
      OptionTag = tag ;
    }

    std::string OptionTag ;
  } ;
 
  int GetOption(const std::string option_tag, StringVector* values) ;
  int DumpOption(const std::string option_tag, bool withTag = true,
                 bool withNewLine = false) ;

  int GetMultiDoubleOption(const std::string tag,
                           std::vector<double>* args, 
                           bool required) ;

  int GetMultiDoubleOption(const std::string tag,
                           itk::Array<double>* args, 
                           bool required) ;


  double GetDoubleOption(const std::string tag, double default_value, bool required);

  bool GetBooleanOption(const std::string tag, bool default_value, bool required);

  int GetMultiIntOption(const std::string tag,
                        std::vector<int>* args,
                        bool required);

  int GetIntOption(const std::string tag, int default_value, bool required) ;
  
  int GetMultiUCharOption(const std::string tag,
                          std::vector< unsigned char >* args,
                          bool required);

  unsigned char GetUCharOption(const std::string tag, unsigned char default_value,
                               bool required) ;

  int GetMultiUIntOption(const std::string tag,
                          std::vector< unsigned int >* args,
                          bool required);

  unsigned int GetUIntOption(const std::string tag, unsigned int default_value,
                               bool required) ;

  int GetStringOption(const std::string tag, std::string* ret, bool required);
  
  int GetMultiStringOption(const std::string tag, std::vector< std::string >* ret,
                           bool required);
protected:

private:
  OptionMap m_Map ;
} ; // end of class

#endif 
