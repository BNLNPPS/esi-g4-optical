module custom

# ---
function test_func()
    println("Test function")
  end

# ---
function operation(x::Float64)
    x*2.0
end


# ---
export test_func, operation

end
