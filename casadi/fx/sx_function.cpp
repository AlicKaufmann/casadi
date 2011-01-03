/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010 by Joel Andersson, Moritz Diehl, K.U.Leuven. All rights reserved.
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "sx_function_internal.hpp"
#include <cassert>
#include <limits>
#include <stack>
#include <deque>
#include <fstream>
#include <sstream>
#include "../stl_vector_tools.hpp"
#include "../expression_tools.hpp"
#include "../sx/sx_node.hpp"
#include "../mx/evaluation.hpp"

#define SMALL_WORK_VECTOR 0

namespace CasADi{

using namespace std;


SXFunction::SXFunction(){
}

SXFunction::SXFunction(const SXMatrix& arg, const SXMatrix& res){
  vector<SXMatrix> argv(1,arg);
  vector<SXMatrix> resv(1,res);
  assignNode(new SXFunctionInternal(argv,resv));
}

SXFunction::SXFunction(const vector< vector<SX> >& arg, const vector< vector<SX> >& res){
  vector<SXMatrix> argv(arg.begin(),arg.end());
  vector<SXMatrix> resv(res.begin(),res.end());
  assignNode(new SXFunctionInternal(argv,resv));
}

SXFunction::SXFunction(const vector< SXMatrix>& arg, const vector<SXMatrix>& res){
  assignNode(new SXFunctionInternal(arg,res));
}

SXFunction::SXFunction(const vector< vector<SX> >& arg, const SXMatrix& res){
  vector<SXMatrix> argv(arg.begin(),arg.end());
  vector<SXMatrix> resv(1,res);
  assignNode(new SXFunctionInternal(argv,resv));
}

SXFunction::SXFunction(const vector< SXMatrix>& arg, const SXMatrix& res){
  vector<SXMatrix> resv(1,res);
  assignNode(new SXFunctionInternal(arg,resv));
}






#if 0
void SXFunction::createNode(const vector<SXMatrix>& ip, const vector<SXMatrix>& op){
  assignNode(new SXFunctionInternal(ip,op));
}
#endif

const SXFunctionInternal* SXFunction::operator->() const{
  return (const SXFunctionInternal*)FX::operator->();
}

SXFunctionInternal* SXFunction::operator->(){
  return (SXFunctionInternal*)FX::operator->();
}

SXMatrix SXFunction::getArgumentIn(int iind) const {
	vector<SXMatrix> argv = (*this)->inputv;
	return argv[iind];
}

SXMatrix SXFunction::getArgumentOut(int iind) const {
	vector<SXMatrix> argv = (*this)->outputv;
	return argv[iind];
}


vector<SXMatrix> SXFunction::eval(const vector<SXMatrix>& arg){
  vector<SXMatrix> res;
  (*this)->eval(arg,res);
  return res;
}

SXMatrix SXFunction::eval(const SXMatrix& arg){
  return eval(vector<SXMatrix>(1,arg)).at(0);
}

vector< vector<SX> > SXFunction::eval(const vector< vector<SX> >& arg){
  // Convert input
  vector<SXMatrix> argv = (*this)->inputv;
  if(arg.size() != argv.size()) throw CasadiException("SXFunction::eval: wrong number of inputs");
  for(int i=0; i<argv.size(); ++i){
    if(arg[i].size() != argv[i].size()) throw CasadiException("SXFunction::eval: wrong number of non-zeros");
    
    // Copy the non-zeros
    copy(arg[i].begin(),arg[i].end(),argv[i].begin());
  }
  
  // Evaluate
  vector<SXMatrix> resv;
  (*this)->eval(argv,resv);
  
  // Convert result
  vector< vector<SX> > res(resv.size());
  for(int i=0; i<res.size(); ++i){
    res[i] = resv[i];
  }
  
  return res;
}

vector<SX> SXFunction::eval(const vector<SX>& arg){
  return eval(vector< vector<SX> >(1,arg)).at(0);
}

SXFunction SXFunction::jacobian(int iind, int oind){
  return shared_cast<SXFunction>(FX::jacobian(iind,oind));  
}

SXFunction SXFunction::hessian(int iind, int oind){
  return shared_cast<SXFunction>(FX::hessian(iind,oind));
}

bool SXFunction::checkNode() const{
  return dynamic_cast<const SXFunctionInternal*>(get());
}

SXMatrix SXFunction::jac(int iind, int oind){
  return (*this)->jac(iind,oind);
}

SXMatrix SXFunction::grad(int iind, int oind){
  return (*this)->grad(iind,oind);
}

SXMatrix SXFunction::hess(int iind, int oind){
  return (*this)->hess(iind,oind);  
}

SXMatrix SXFunction::getInputSX(int ind) {
  return (*this)->inputv[ind];
}

SXMatrix SXFunction::getOutputSX(int ind) {
  return (*this)->outputv[ind];
}


} // namespace CasADi

