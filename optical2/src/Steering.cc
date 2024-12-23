// ---

#include "Steering.hh"
#include <julia.h>

double Steering::julia_arith() {


    jl_value_t *argument = jl_box_float64(2.0);
    // jl_function_t *operation = jl_get_function(jl_main_module, "operation");

    // jl_value_t *op_ret = jl_call1(operation, argument);

    // if (jl_typeis(op_ret, jl_float64_type)) {
    //   double ret_unboxed = jl_unbox_float64(op_ret);
    //   G4cout << "op_ret in C: " <<  ret_unboxed << G4endl;
    // }
    // else {
    //   G4cout << "ERROR: unexpected return type from op_ret" << G4endl;
    // }

    return 0.0;
}