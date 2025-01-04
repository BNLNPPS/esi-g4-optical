module custom

# - future dev
mutable struct Foo
  x::Float32
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


# # - future dev
# ---
function opstruct(x::Foo)
  return
end


function opstruct(t::Foo, v::Float64)
println("!!!")
t.x=v
println(t.x)
return
end

function newstruct()
println("new")
y = Foo(1.0)
return y
end



# ---
export test_func, operation, opvoid, Foo, opstruct

end
