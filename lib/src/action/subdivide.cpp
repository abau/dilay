#include "action/subdivide.hpp"

struct ActionSubdivide::Impl {
  ActionSubdivide*          self;

  Impl (ActionSubdivide* s) : self (s) {}

  void runUndoBeforePostProcessing (WingedMesh&) {}
  void runRedoBeforePostProcessing (WingedMesh&) {}

  void run (WingedMesh&, WingedFace&, std::vector <Id>*) { 
  }
};

DELEGATE_BIG3_SELF (ActionSubdivide)
DELEGATE3          (void, ActionSubdivide, run, WingedMesh&, WingedFace&, std::vector <Id>*)
DELEGATE1          (void, ActionSubdivide, runUndoBeforePostProcessing, WingedMesh&)
DELEGATE1          (void, ActionSubdivide, runRedoBeforePostProcessing, WingedMesh&)
