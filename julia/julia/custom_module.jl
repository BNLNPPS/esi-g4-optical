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

#function check_steering()
#  return 

# ---
function begin_run()
  println("Begin of Run Action")
  return
end

# ---
function begin_event(thread::Int8)
  # println("Begin of Event Action")
  return thread
end



# ---
function operation(x::Float64)
    x*2.0
    return x*2.0
end

# ---
function opvoid(x::Float64)
  x*2.0
  return
end





# ---
export operation, opvoid, begin_run, begin_event
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