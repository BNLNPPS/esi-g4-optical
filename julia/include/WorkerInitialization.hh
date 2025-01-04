#ifndef WorkerInitializaiton_h
#define WorkerInitializaiton 1

// Keep here temporarily
#include "G4UserWorkerInitialization.hh"
#include <globals.hh>
#include <julia.h>

//---
#include "Steering.hh"

class WorkerInitialization : public G4UserWorkerInitialization {
  public:
    WorkerInitialization() = default;
    virtual ~WorkerInitialization() = default;

    virtual void WorkerInitialize() const override {
      if (jl_get_pgcstack() == NULL) {
        jl_adopt_thread();
        if(Steering::verbose) {G4cout << "=====> WORKER INIT, THREAD ADOPTED <===========" << G4endl;}
      }
    }

    virtual void WorkerStart() const override {}

    virtual void WorkerRunStart() const override {
      G4cout << "=====> WORKER RUN START <===========" << G4endl;

      jl_function_t *test_func = jl_get_function(jl_main_module, "test_func");
      if (test_func == NULL) {
        G4cout << "Worker run start --  test_func is null, exiting..." << G4endl;      
        jl_atexit_hook(0);
        exit(0);
      }
      G4cout << "Calling test_func " << G4endl;
      jl_call0(test_func);


      jl_value_t *argument = jl_box_float64(2.0);
      jl_function_t *operation = jl_get_function(jl_main_module, "operation");

      jl_value_t *op_ret = jl_call1(operation, argument);

      if (jl_typeis(op_ret, jl_float64_type)) {
        double ret_unboxed = jl_unbox_float64(op_ret);
        G4cout << "WORKER RUN START - op_ret in C: " <<  ret_unboxed << G4endl;
      }
      else {
        G4cout << "ERROR: unexpected return type from op_ret" << G4endl;
      }
  
    
    }

    virtual void WorkerRunEnd() const override {
    jl_value_t *argument = jl_box_float64(3.0);
    jl_function_t *operation = jl_get_function(jl_main_module, "operation");

    jl_value_t *op_ret = jl_call1(operation, argument);

    if (jl_typeis(op_ret, jl_float64_type)) {
      double ret_unboxed = jl_unbox_float64(op_ret);
      G4cout << "Worker RUN END op_ret in C: " <<  ret_unboxed << G4endl;
    }
    else {
      G4cout << "ERROR: unexpected return type from op_ret" << G4endl;
    }   


    jl_ptls_t ptls = jl_current_task->ptls;
    jl_gc_safe_enter(ptls);
    }
    virtual void WorkerStop() const override {}
};



#endif