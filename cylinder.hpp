#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"

namespace Cylinder{
	void deform_terrain(cgp::mesh& m);
	cgp::mesh create_cylinder_mesh(float radius, float height, int N_f, int N_s);
	cgp::mesh create_forest_cylinder_mesh(float radius, float height, int N_f, int N_s);
}





