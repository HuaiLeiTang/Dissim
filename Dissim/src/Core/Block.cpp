/*
 * Block.cpp
 *
 *  Created on: Jun 9, 2016
 *      Author: peer23peer
 */

#include "Core/Block.h"
#include "Core/Solver.h"

#include <algorithm>

namespace dissim {

Block::Block()
    : saveHistory_(true) {
  DissimType::Dissim_ptr o(new DissimType);
  OutputPorts.push_back(o);
  OutputPort = OutputPorts[0];
  DissimType::Value_ptr v(new DissimType::Value_t);
  History.push_back(v);
  InitialValue = History[0];
  solver_ = Solver::Solver_ptr(new Newton());
}

Block::Block(std::string name) {
  this->Name = name;
  Block();
}

Block::~Block() {

}

dissim::DissimType::Dissim_ptr Block::Run() {
  //solver_->Solve(shared_from_this());
  if (saveHistory_) {
    DissimType::Value_ptr v = boost::make_shared<DissimType::Value_t>(OutputPort->Value);
    History.push_back(v);
  }
  return OutputPort;
}

void Block::push_back(DissimType::Dissim_ptr dissimVar, int operation) {
  InputPorts.push_back(dissimVar);
  Operations.push_back(operation);
}

void Block::push_back(int operation) {
  dissim::DissimType::Dissim_ptr I(new DissimType);
  push_back(I, operation);
}

void Block::push_back(Block_ptr blck) {

}

void Block::RescaleInputsMatrix(int cols, int rows) {
  RescaleInputsMatrix(this, cols, rows);
  if (SystemBlocks.size() > 0) {
    std::for_each(SystemBlocks.begin(), SystemBlocks.end(), [&](Block_ptr &SB) {
      RescaleInputsMatrix(&*SB, cols, rows);
    });
  }
}

void Block::RescaleInputsMatrix(Block *obj, int cols, int rows) {
  std::for_each(obj->InputPorts.begin(), obj->InputPorts.end(), [&](DissimType::Dissim_ptr &IP) {
    IP->Value.resize(rows, cols);
  });

  obj->History[0]->resize(rows, cols);
  obj->range.first.resize(rows, cols);
  obj->range.second.resize(rows, cols);
  obj->OutputPort->Value.resize(rows, cols);
}

Block::Block_ptr Block::getSubBlock(std::string name) {
  for (int i = 0; i < SystemBlocks.size(); i++) {
    if (SystemBlocks[i]->Name.compare(name) == 0) {
      return SystemBlocks[i];
    }
  }
  throw new BlockException("No specified block found exception!");
  return SystemBlocks[0];
}

DissimType::Dissim_ptr Block::getInputPort(std::string symbol) {
  for (int i = 0; i < InputPorts.size(); i++) {
    if (InputPorts[i]->Symbol.compare(symbol) == 0) {
      return InputPorts[i];
    }
  }
  throw new BlockException("No specified DissimType found exception!");
  return InputPorts[0];
}

DissimType::Dissim_ptr Block::getOutputPort(std::string symbol) {
  for (int i = 0; i < OutputPorts.size(); i++) {
    if (OutputPorts[i]->Symbol.compare(symbol) == 0) {
      return OutputPorts[i];
    }
  }
  throw new BlockException("No specified DissimType found exception!");
  return OutputPorts[0];
}

void Block::setTime(boost::shared_ptr<Chronos> time) {
  this->Time = time;
  std::for_each(SystemBlocks.begin(), SystemBlocks.end(), [&](Block_ptr &S) {
    S->setTime(time);
  });
}
void Block::push_back(DissimType::Dissim_ptr dissimVar) {
  InputPorts.push_back(dissimVar);
  Operations.push_back(0);
}
void Block::reRouteInput(Block::Block_ptr blck) {
  std::for_each(blck->InputPorts.begin(), blck->InputPorts.end(), [&](auto bio) {
    auto IO = std::find(InputPorts.begin(), InputPorts.end(), bio);
    if (IO != blck->InputPorts.end()) {
      InputPorts.push_back(bio);
    }
  });
}
void Block::setSolver(Solver::Solver_ptr solver) {
  solver_ = solver;
}

Solver::Solver_ptr Block::getSolver() {
  return solver_;
}
void Block::setSaveHistory(bool saveHistory) {
  saveHistory_ = saveHistory;
}
bool Block::getSaveHistory() {
  return saveHistory_;
}

}


