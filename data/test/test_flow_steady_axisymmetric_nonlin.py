import agros2d

# problem
problem = agros2d.problem(clear = True)
problem.coordinate_type = "axisymmetric"
problem.mesh_type = "gmsh_quad"
problem.matrix_solver = "umfpack"

# fields
# flow
flow = agros2d.field("flow")
flow.analysis_type = "steadystate"
flow.number_of_refinements = 1
flow.polynomial_order = 2
flow.adaptivity_type = "disabled"
flow.linearity_type = "newton"
flow.nonlinear_tolerance = 0.0001
flow.nonlinear_steps = 10
flow.automatic_damping = True
flow.automatic_damping_coeff = 0.8
flow.damping_number_to_increase = 1
flow.maximum_steps_with_reused_Jacobian = 10
flow.sufficient_improvement_factor_Jacobian = 0.8

# boundaries
flow.add_boundary("inlet", "flow_velocity", {"flow_velocity_x" : 0, "flow_velocity_y" : { "expression" : "-cos(r/0.25*pi/2)" }})
flow.add_boundary("outlet", "flow_outlet", {})
flow.add_boundary("wall", "flow_velocity", {"flow_velocity_x" : 0, "flow_velocity_y" : 0})
flow.add_boundary("symmetry", "flow_symmetry", {})

# materials
flow.add_material("fluid", {"flow_viscosity" : 0.001, "flow_density" : 1, "flow_force_x" : 0, "flow_force_y" : 0})

# geometry
geometry = agros2d.geometry
geometry.add_edge(0, 0.5, 0.25, 0.5, boundaries = {"flow" : "inlet"})
geometry.add_edge(0.25, 0.5, 0.25, -0.05, boundaries = {"flow" : "wall"})
geometry.add_edge(0.25, -0.05, 0.15, -0.15, boundaries = {"flow" : "wall"})
geometry.add_edge(0.15, -0.15, 0.15, -0.5, boundaries = {"flow" : "wall"})
geometry.add_edge(0.05, -0.5, 0.05, -0.05, boundaries = {"flow" : "wall"})
geometry.add_edge(0.05, -0.05, 0.15, -0.05, boundaries = {"flow" : "wall"})
geometry.add_edge(0.15, -0.05, 0, 0.1, angle = 90, boundaries = {"flow" : "wall"})
geometry.add_edge(0, 0.1, 0, 0.5, boundaries = {"flow" : "symmetry"})
geometry.add_edge(0.05, -0.5, 0, -0.5, boundaries = {"flow" : "wall"})
geometry.add_edge(0, -0.5, 0, -1.3, boundaries = {"flow" : "symmetry"})
geometry.add_edge(0, -1.3, 0.15, -1.3, boundaries = {"flow" : "outlet"})
geometry.add_edge(0.15, -1.3, 0.15, -0.55, boundaries = {"flow" : "wall"})
geometry.add_edge(0.15, -0.55, 0.15, -0.5, boundaries = {"flow" : "wall"})

geometry.add_label(0.127882, 0.157169, materials = {"flow" : "fluid"})

agros2d.view.zoom_best_fit()

# solve problem
problem.solve()

# point value
point = flow.local_values(2.137e-01, 8.173e-03)
testp = agros2d.test("Pressure", point["p"], 2.880471)
testv = agros2d.test("Velocity", point["v"], 0.652625)
testvx = agros2d.test("Velocity - r", point["vr"], 0.02563)
testvy = agros2d.test("Velocity - z", point["vz"], -0.652122)

# volume integral
volume = flow.volume_integrals([0])
# testPj = agros2d.test("Losses", volume["Pj"], 10070.23937)

# surface integral
surface = flow.surface_integrals([0])
# testI = agros2d.test("Current", surface["Ir"], 3629.425713)

print("Test: Incompressible flow - axisymmetric: " + str(testp and testv and testvx and testvy))
