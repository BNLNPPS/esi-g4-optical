module custom

# ----
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
function test_func()
    # println("Test function")
    return
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
export operation, opvoid, test_func, set_nthreads, nthreads, begin_run, begin_event
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