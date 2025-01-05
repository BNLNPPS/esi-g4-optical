#ifndef WorkerInitializaiton_h
#define WorkerInitializaiton 1

// ---
#include "G4UserWorkerInitialization.hh"
#include <julia.h>

// ---
class WorkerInitialization : public G4UserWorkerInitialization {
  public:
    WorkerInitialization()            = default;
    virtual ~WorkerInitialization()   = default;

    virtual void WorkerInitialize()   const override;

    virtual void WorkerRunStart()     const override;
    virtual void WorkerRunEnd()       const override;

    virtual void WorkerStart()        const override;
    virtual void WorkerStop()         const override;
};



#endif