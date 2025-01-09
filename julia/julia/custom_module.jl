module custom
using ..steering
using Parameters

println("=====> Loading custom_module.jl")
println("=====> ", nthreads())


@with_kw mutable struct MyData
  edep::Float64 = 0.0
  # edepHist = H1D("Event total Edep distribution", 100, 0., 110.)
end


# const simdata = [MyData() for i in 1:(MySteering.nthreads()+1)] 

# FIXME -- need to find a way to init this with the actual number of threads.
const simdata = [MyData() for i in 1:16] 

function getMyData(thread::Int8)
  simdata[thread+1]
end 

# ---
function begin_run()
  println("Begin of Run Action")
  return
end

# ---
function begin_event(thread::Int8)
  datum = simdata[thread+1]
  datum.edep = 0.0
  return datum.edep
end

# ---
function end_event(thread::Int8)
  datum = simdata[thread+1]
  return datum.edep
end


# ---
function stepping_action(thread::Int8, energy::Float64)
  datum = simdata[thread+1]
  datum.edep+=energy
  return datum.edep
return
end



# ---
function operation(x::Float64)
    x*2.0
    return x*2.0
end






# ---
export operation, stepping_action, begin_run, begin_event, end_event, getMyData
# , Foo, opstruct

end



# # # - future dev

# function steering()
#   println("new steering")
#   s = Steering(0)
#   return s
# end

# function set_nthreads(s::Steering, nthreads::Int8)
#   s.nthreads=nthreads
#   return
# end

# # ---
# function opstruct(x::Foo)
#   return
# end


# function opstruct(t::Foo, v::Float64)
# println("!!!")
# t.x=v
# println(t.x)
# return
# end

# function newstruct()
# println("new")
# y = Foo(1.0)
# return y
# end