#include <config.h>
#include <compiler/ParseTree.h>
#include <stdexcept>

using std::string;
using std::vector;
using std::logic_error;

ParseTree::ParseTree(TreeClass tclass, int line)
    : _tclass(tclass), _parameters(), _parent(0), _name(), _value(0), 
      _line(line)
{
}

ParseTree::~ParseTree()
{
  for (vector<ParseTree*>::iterator p = _parameters.begin();
       p != _parameters.end(); p++) 
    {
      if (*p != 0) {
	delete *p;
      }
    }
}

void ParseTree::setName(string const &name)
{
  switch(_tclass) {
  case P_VAR: case P_COUNTER: case P_FUNCTION: case P_DENSITY: case P_LINK:
  case P_ARRAY:
    _name = name;
    break;
  default:
    throw logic_error("Can't set name of ParseTree object");
  }
}

void ParseTree::setValue(double value)
{
  if (_tclass == P_VALUE) {
    _value = value;
  }
  else {
    throw logic_error("Can't set value of ParseTree");
  }
}

vector<ParseTree*> const &ParseTree::parameters() const
{
  return _parameters;
}

TreeClass ParseTree::treeClass() const
{
  return _tclass;
}

std::string const &ParseTree::name() const
{
  switch(_tclass) {
  case P_VAR: case P_COUNTER: case P_FUNCTION: case P_DENSITY: case P_LINK:
  case P_ARRAY:
    break;
  default:
    throw logic_error("Can't get name of ParseTree: invalid treeClass");
  }
  return _name;
}

double ParseTree::value() const
{
  if (_tclass != P_VALUE) {
    throw logic_error ("Can't get value of ParseTree: invalid treeClass");
  }
  return _value;
}

int ParseTree::line() const
{
    return _line;
}

void ParseTree::setParameters(std::vector<ParseTree *> const &parameters)
{
  if (!_parameters.empty()) {
    throw logic_error("Parameters already set in ParseTree");
  }
  if (_parent != 0) {
    throw logic_error("Can't set parameters of ParseTree: node already has parent");
  }
  for (unsigned int i = 0; i < parameters.size(); ++i) {
    if (parameters[i] == this) {
      throw logic_error("ParseTree cannot be a parameter of itself");
    }
    if (parameters[i] != 0) {
      if (parameters[i]->_parent == 0) {
	parameters[i]->_parent = this;
      }
      else {
	throw logic_error("Can't set parameters of ParseTree: parameter already has parent");
      }
    }
  }
  _parameters = parameters;
}
