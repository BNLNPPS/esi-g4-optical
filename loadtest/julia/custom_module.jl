module custom

# using FHist

using Plots

# using Parameters

println("=====> Loading custom_module.jl")

# ---
function test_func() # just to check if the loading works -- a summy function
  return 3
end

# ---
function plotter()
  println("foo")
  img = plot(rand(1000))
  savefig(img, "f.png")
  return
end


# --------------------------------------------------------------------------

export test_func, plotter
end

