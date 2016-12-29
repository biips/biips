#include "compiler/LogicalFactory.hpp"

namespace Biips
{

  Bool lt(const Types<NodeId>::Array & par1, const Types<NodeId>::Array & par2)
  {
    if (par1.size() == par2.size())
    {
      //Equal sized vectors: Sort by ordering of elements
      for (Size i = 0; i < par1.size(); ++i)
      {
        if (par1[i] < par2[i])
          return true;
        else if (par2[i] < par1[i])
          return false;
      }
      return false;
    }
    else
      return par1.size() < par2.size();
  }

  Bool lt(LogicalPair const & arg1, LogicalPair const & arg2)
  {
    if (arg1.first == arg2.first)
    {
      //Same function: check parameters
      return lt(arg1.second, arg2.second);
    }
    else
    {
      //Diferent functions: sort by address (arbitrary)
      return arg1.first.get() < arg2.first.get();
    }
  }

  NodeId LogicalFactory::NewNode(const Function::Ptr & pFunc,
                                 const Types<NodeId>::Array & parents)
  {
//    else if (LINK(func))
//    {
//      bool ok = true;
//      for (unsigned int i = 0; i < parents.size(); ++i)
//      {
//        if (parents[i]->length() != 1)
//        {
//          ok = false;
//          break;
//        }
//      }
//      if (ok)
//      {
//        node = new LinkNode(LINK(func), parents);
//      }
//      else
//      {
//        throw runtime_error("Invalid vector argument to " + func.name());
//      }
//    }
    return graph_.AddLogicalNode(pFunc, parents);
  }

  NodeId LogicalFactory::GetNode(const Function::Ptr & pFunc,
                                 Types<NodeId>::Array const & parents)
  {
    if (!pFunc)
      throw LogicError("NULL function pointer passed to LogicalFactory::GetNode");

    LogicalPair lpair(pFunc, parents);
    std::map<LogicalPair, NodeId, ltlogical>::iterator it =
        logicalmap_.find(lpair);

    if (it != logicalmap_.end())
      return it->second;

    NodeId id = NewNode(pFunc, parents);
    logicalmap_[lpair] = id;
    return id;

  }

} /* namespace Biips */
