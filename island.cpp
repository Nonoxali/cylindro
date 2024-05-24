#include "island.hpp"
#include <random>
#include <vector>
#include <algorithm>


using namespace cgp;

float const max_ran = float(RAND_MAX);


void Island::fractal_recursive(cgp::mesh& m, int i, int j, int N_s, float height) {
	if (i < j - 1 ) {
		int med = (i + j) / 2;
		for (int k = 0; k < N_s; k++) {
			m.position[N_s*med + k] = {(m.position[N_s*i+k].x + m.position[N_s*j+k].x) / 2.0f, (m.position[N_s*i+k].y + m.position[N_s*j+k].y) / 2.0f, (m.position[N_s*i+k].z + m.position[N_s*j+k].z) / 2.0f + std::fmax((-m.position[N_s * i + k].z + m.position[N_s * j + k].z) *(rand() - max_ran / 2.0f)/(1.5f*max_ran), 0) };
		}
		std::cout << "tour fractal " << i << "," << med << "," << j << std::endl;
		fractal_recursive(m, i, med, N_s, height);
		fractal_recursive(m, med, j, N_s, height);
	}
}



float Island::evaluate_terrain_height_bis(float x, float y, int N) //Procedural height for terrain
{
	std::array<vec2,4> p_i = { vec2{-10,-10}, vec2{5,5}, vec2{-3,4}, vec2{6,4} };
	std::array<float,4> h_i = {3.0f, -1.5f, 1.0f, 2.0f};
	std::array<float,4> sigma_i = {10.0f, 3.0f, 4.0f, 4.0f};
    
	float z = -0.3f;
    for(int i=0; i<N; i++){
        float d = norm(vec2(x,y) - p_i[i])/sigma_i[i];
        z += (h_i[i] * std::exp(-d * d)); 
    }
    return z;
}

mesh Island::create_fractal_cone(float height, float radius, int N_s, int N_it) //Creation of a procedural modified cone by fractal process
{	
	
	mesh m;

	//Initialization of vector position 
	int const size = (std::pow(2, N_it) + 1)* N_s; 
	for (int k = 0; k < size; k++) {
		m.position.push_back({ 0,0,0 });
	}
	for (int k = 0; k < N_s; k++) {
		float u = k / float(N_s);
		m.position[k] = { radius * std::cos(2 * Pi * u), radius * std::sin(2 * Pi * u), 0 };
		m.position[std::pow(2, N_it) * N_s + k] = { 0, 0, height };
	}


	//Iteration of the fractal process
	fractal_recursive(m, 0, std::pow(2, N_it), N_s, height);


	for (int k = 0; k < size-N_s; k++) {
		int u00 = k;
		int u01 = k + N_s;
		int u10 = k + 1;
		int u11 = k + 1 + N_s;

		if (u10 % N_s == 0) {
			u10 = k + 1 - N_s;
			u11 = k + 1;
		}

		uint3 t1 = { u00, u10, u11 };
		uint3 t2 = { u00, u11, u01 };

		m.connectivity.push_back(t1);
		m.connectivity.push_back(t2);
	}

	m.fill_empty_field();
	return m;
}


mesh Island::create_floatting_island_terrain(float radius, int N_s, int N_it){ //Creation of the flat terrain of the floatting island
	mesh m;

	int const stages = std::pow(2, N_it);
	
	//Points of terrain
	m.position.push_back({0,0,evaluate_terrain_height_bis(0, 0, 4)});
	for(int k=1; k <= stages; k++){ 
		for(int i=0; i < N_s; i++){
			float u = i / float(N_s);
			float x = k*radius/stages * std::cos(2*Pi*u);
			float y = k*radius/stages * std::sin(2*Pi*u);
			m.position.push_back({x, y, evaluate_terrain_height_bis(x, y, 4)*(std::pow(std::sin((Pi/2.0f) *(1 + k/float(stages))),0.3f))});
		}
	}

	//Connectivity at the center
	for(int k=0; k<N_s-1; k++){
		m.connectivity.push_back({0,k+1,k+2}); 
	}
	m.connectivity.push_back({0, N_s,1});

	//Connectivity
	for(int k = 0; k<stages-1; k++){
		for(int i=0; i < N_s; i++){
			int u00 = 1 + k*N_s+i;
			int u10 = 1 + (k+1)*N_s+i;
			int u01 = 1 + k*N_s+i +1;
			if((u01-1)%N_s == 0){
				u01 = 1 + k*N_s;
			}
			int u11 = u01 + N_s;

			uint3 t1 = { u00, u10, u11 };
			uint3 t2 = { u00, u11, u01 };

			m.connectivity.push_back(t1);
			m.connectivity.push_back(t2);
		}
	}

	m.fill_empty_field();
	return m;

}

void Island::fractal_cone_adaptation(cgp::mesh& cone, cgp::mesh top_terrain, int N_s, int N_it){ //Modification of the cone to fit with the top terrain

	int const stages = std::pow(2, N_it);
	

	std::vector<float> height;
	for(int k=1; k<=N_s; k++){
		height.push_back(top_terrain.position[(stages-1)*N_s + k].z);
	}

	for(int k=0; k<stages+1; k++){
		for(int i=0; i<N_s; i++){
			cone.position[k*N_s + i].z = cone.position[k*N_s + i].z * std::pow((k/float(stages)),0.1) + height[i] * (1-std::pow((k/float(stages)),0.1));
		}
	}

} 
