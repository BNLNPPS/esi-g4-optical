#module steering
#using Plots
# ----

println("=====> Loading steering.jl")

mutable struct Steering
  nthreads::Int8
end

MySteering = Steering(0)

function nthreads()
  return MySteering.nthreads
end

function set_nthreads(n::Int8)
  MySteering.nthreads=n
  return
end


# ---
function test_func() # just to check if the loading works -- a summy function
  return
end

#export set_nthreads, nthreads, test_func

#end
