#pragma once


#include "cgp/cgp.hpp"

namespace Island{


	void fractal_recursive(cgp::mesh& m, int i, int j, int N_s, float height);
	float evaluate_terrain_height_bis(float x, float y, int N);
	cgp::mesh create_fractal_cone(float height, float radius, int N_s, int N_it);
	cgp::mesh create_floatting_island_terrain(float radius, int N_s, int N_it);
	void fractal_cone_adaptation(cgp::mesh& cone, cgp::mesh top_terrain, int N_s, int N_it);

}



