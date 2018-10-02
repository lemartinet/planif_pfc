//
// mystr.cc
//  
// Made by fouque
// Login   <fouque@column>
// 
// Started on  Thu Sep 20 11:02:32 2007 fouque
// Started on  Thu Sep 20 11:02:32 2007 fouque
//

#include <iostream>
#include <sstream>
#include "mystr.hh"

int str2i (string str)
{
  stringstream stream;
  int res = 0;

  stream << str;
  stream >> res;
  return res;
}

double str2d (string str)
{
  stringstream  stream;
  double res = 0.0;

  stream << str;
  stream >> res;
  return res;
}

string i2str (int i)
{
  stringstream stream;

  stream << i;
  return stream.str ();
}

string d2str (double d)
{
  stringstream stream;

  stream << d;
  return stream.str ();	
}
