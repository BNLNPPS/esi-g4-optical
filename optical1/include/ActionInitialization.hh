#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

/// Action initialization class.

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization();
    virtual ~ActionInitialization();

    //This is called by the master thread.
    //You may want to create here an instance of RunAction
    //To allow merging of results from threads
    virtual void BuildForMaster() const;
    
    //This is called by each worker thread.
    //Create here the user-actions needed by each thread:
    // RunAction, EventAction, SteppingAction, StackingAction
    virtual void Build() const;

};

// --

#endif

    
