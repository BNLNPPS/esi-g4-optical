module custom

using FHist
using Plots
using Parameters

println("=====> Loading my_module.jl")

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
  return 3
end

# --------------------------------------------------------------------------

@with_kw mutable struct MyData
  edep::Float64 = 0.0
  h1 = Hist1D(; binedges = 0.0:0.5:10.0)
  # edepHist = H1D("Event total Edep distribution", 100, 0., 110.)
end

# FIXME -- need to find a way to init this with the actual number of threads.
const simdata = [MyData() for i in 1:16] 

total_h1 = Hist1D(; binedges = 0.0:0.5:100.0)

function getMyData(thread::Int8)
  simdata[thread+1]
end 

# ---
function begin_run()
  println("=====> MASTER Julia: Begin of Run Action")
  return
end

function end_run()
  println("=====> MASTER Julia: End of Run Action!")

  tot = FHist.integral(total_h1)

  println("-------------------> ", tot)
  img = plot(total_h1, title="foo")
  savefig(img, "foo.png")
  println("=====> edepHist.png saved")
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
  atomic_push!(total_h1, datum.edep)
  tot = FHist.integral(total_h1)
  return tot # datum.edep
end

# ---
function stepping_action(thread::Int8, energy::Float64)
  datum = simdata[thread+1]
  datum.edep+=energy
  atomic_push!(datum.h1, energy*1000000.0)
  return datum.edep
end

export stepping_action, begin_run, end_run, begin_event, end_event, getMyData, set_nthreads, nthreads, test_func





println("=====> Loaded my_module.jl")

end
