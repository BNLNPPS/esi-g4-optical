module custom

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
mutable struct Foo
  x::Float32
end

# ---
function opstruct(x::Foo)
  return
end

# ---
export test_func, operation, opvoid, Foo, opstruct

end
