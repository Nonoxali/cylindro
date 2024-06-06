#include "cylinder.hpp"
#include <random>
#include <vector>
#include <algorithm>


using namespace cgp;

float const max_ran = float(RAND_MAX);

void Cylinder::deform_terrain(mesh& m)
{
	// Set the terrain to have a gaussian shape
	for (int k = 0; k < m.position.size(); ++k)
	{
		vec3& p = m.position[k];
		float d2 = p.x*p.x + p.y * p.y;
		float z = exp(-d2 / 4)-1;

		z = z + 0.05f*noise_perlin({ p.x,p.y });

		p = { p.x, p.y, z };
	}

	m.normal_update();
}


mesh Cylinder::disc_with_texture(int planet_r)
{

	mesh disc;
	int N = 20;

	for (int k = 0; k < N; ++k)
	{
		float u = k / (N - 1.0f);
		vec3 p = 1.5f * planet_r * vec3(std::cos(2 * Pi * u), std::sin(2 * Pi * u), 0.0f);
		disc.position.push_back(p);
		if (k % 2 == 0)
			disc.uv.push_back({ 0,0.25f });
		else
			disc.uv.push_back({ 0,0.75f });


	}
	// middle point
	disc.position.push_back({ 0,0,0 });
	disc.uv.push_back({ 0.5f,0.5f });

	for (int k = 0; k < N - 1; ++k)
		disc.connectivity.push_back(uint3{ N, k, k + 1 });

	disc.fill_empty_field();
	return disc;
}

mesh Cylinder::create_simple_cylinder_mesh(float radius, float height)
{
	// Create a cylinder, especially used for c_terrains
	mesh m;

	// Number of floors
	int N_f = 40;

	// Number of samples by floor
	int N_s = 50;


	// Geometry
	for (int i = 0; i < N_f; i++)
	{
		for (int k = 0; k < N_s; ++k)
		{
			float max = float(RAND_MAX);
			float ran = rand() / max;
			float u = k / float(N_s);
			vec3 p = { (radius + 0.2f * ran) * std::cos(2 * 3.14f * u), (radius + 0.2f * ran) * std::sin(2 * 3.14f * u), i * height / N_f };
			m.position.push_back(p);
			m.uv.push_back({ (float)i / 3.0 ,(float)k / 3.0 });
		}
	}


	// Connectivity
	for (int k = 0; k < N_s * (N_f - 2); k++)
	{
		int u00 = k;
		int u01 = k + N_s;
		int u10 = k + 1;
		if (u10 % N_s == 0) {
			u10 = u10 - N_s;
		}
		int u11 = u10 + N_s;

		uint3 t1 = { u00, u10, u11 };
		uint3 t2 = { u00, u11, u01 };

		m.connectivity.push_back(t1);
		m.connectivity.push_back(t2);
	}

	m.fill_empty_field();
	return m;
}

mesh Cylinder::create_cylinder_mesh(float radius, float height, int N_f, int N_s)
{

	// Create a cylinder, especially used for c_terrains, N_f the number of floors of points and N_s the number of points by floor
    mesh m;

    // Geometry
	for(int i = 0; i < N_f; i++)
	{
		for(int k=0; k<N_s; ++k)
    	{
			float max = float(RAND_MAX);
			float ran = 0.03f * rand() / max ;
        	float u = k/float(N_s);
        	vec3 p = {(radius + 0.2f*ran)*std::cos(2*3.14f*u), (radius + 0.2f* ran)*std::sin(2*3.14f*u), i * height/N_f};
        	m.position.push_back(p);

			//float random_angle =  static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 3.14f));
			//for 1 triangle 
			//float x = cos(random_angle) * i / 60 - sin(random_angle) * k / 60;
			//float y = sin(random_angle) * i / 60 + cos(random_angle) * k / 60);


			m.uv.push_back({ (float)i / 60.0 ,(float)k / 60.0 });
    	}
	}
    

    // Connectivity
	for(int k=0; k<N_s*(N_f-2); k++)
	{
		int u00 = k;
        int u01 = k+N_s;
		int u10 = k+1;
		if(u10%N_s == 0){
			u10 = u10 - N_s;
		}
		int u11 = u10 + N_s;

		uint3 t1 = {u00, u10, u11}	;
        uint3 t2 = {u00, u11, u01};

		m.connectivity.push_back(t1);
        m.connectivity.push_back(t2);
	}

    m.fill_empty_field();
    return m;
}

void put_lake(mesh& m, float radius, int N_f, int N_s, int where){
	//Mettre un lac sur un cylindre, where indiaue sur quel etage le placer
	int size = int(float(N_f)/20);
	std::cout << "hello" << std::endl;
	for(int k = 0; k<2*size; k++){
		//Centre du lac
		int verif = 0;
		if(k != 0) {
			verif = 1;
		}
		m.position[(where + k) *N_s].x -= radius*0.25f* std::cos(0);
		m.position[(where + k) *N_s].y -= radius*0.25f* std::sin(0);
		m.position[(where - k) *N_s].x -= verif*radius*0.25f* std::cos(0);
		m.position[(where - k) *N_s].y -= verif*radius*0.25f* std::sin(0);

		float squared_width = 0.25f*((size*size) - (k*k)/4.0f); //Ellipse demi grand axe = 2*size, demi petit axe = size
		float width = std::sqrt(squared_width);
		for(int i=1; i<int(width);i++){
			float u = i / float(N_s);
			float v = (N_s-i) / float(N_s);
			m.position[(where + k) *N_s + i].x -= radius*0.25f* std::cos(2*3.14f*u);
			m.position[(where + k) *N_s + i].y -= radius*0.25f* std::sin(2*3.14f*u);
			m.position[(where + k + 1) *N_s - i].x -= radius*0.25f* std::cos(2*3.14f*v);
			m.position[(where + k + 1) *N_s - i].y -= radius*0.25f* std::sin(2*3.14f*v);
			m.position[(where - k) *N_s + i].x -= verif * radius*0.25f* std::cos(2*3.14f*u);
			m.position[(where - k) *N_s + i].y -= verif * radius*0.25f* std::sin(2*3.14f*u);
			m.position[(where - k + 1) *N_s - i].x -= verif * radius*0.25f* std::cos(2*3.14f*v);
			m.position[(where - k + 1) *N_s - i].y -= verif * radius*0.25f* std::sin(2*3.14f*v);
		}
	}

	// Bords du lac
	for(int k = 0; k<int(5*size/2.0f); k++){
		int verif = 0;
		if(k != 0) {
			verif = 1;
		}
		float squared_width_int = std::max(0.25f*((size*size) - (k*k)/4.0f), 0.0f); //Ellipse demi grand axe = 2*size, demi petit axe = size
		float width_int = std::sqrt(squared_width_int);
		float squared_width_ext = std::max(0.25f*((9*size*size)/4.0f - 9*(k*k)/25.0f), 0.0f); //Ellipse demi grand axe = 5*size/2, demi petit axe = 3*size/2
		float width_ext = std::sqrt(squared_width_ext);
		for(int i = int(width_int); i < int(width_ext); i++){
			float u = i / float(N_s);
			float v = (N_s-i) / float(N_s);
			if(i == 0) {
				verif =0;
			}
			//std::cout << "k " << k << " i " << i << std::endl;
			m.position[(where + k) *N_s + i].x += radius*0.5f*(std::sin(-Pi*(0.5f - ((i-int(width_int))/float(int(width_ext)-int(width_int)))))-1)*0.25f* std::cos(2*3.14f*u);
			m.position[(where + k) *N_s + i].y += radius*0.5f*(std::sin(-Pi*(0.5f - ((i-int(width_int))/float(int(width_ext)-int(width_int)))))-1)*0.25f* std::sin(2*3.14f*u);
			m.position[(where + k + 1) *N_s - i].x += radius*0.5f*(std::sin(-Pi*(0.5f - ((i-int(width_int))/float(int(width_ext)-int(width_int)))))-1)*0.25f* std::cos(2*3.14f*v);
			m.position[(where + k + 1) *N_s - i].y += radius*0.5f*(std::sin(-Pi*(0.5f - ((i-int(width_int))/float(int(width_ext)-int(width_int)))))-1)*0.25f* std::sin(2*3.14f*v);
			m.position[(where - k) *N_s + i].x += verif * radius*0.5f*(std::sin(-Pi*(0.5f - ((i-int(width_int))/float(int(width_ext)-int(width_int)))))-1)*0.25f* std::cos(2*3.14f*u);
			m.position[(where - k) *N_s + i].y += verif * radius*0.5f*(std::sin(-Pi*(0.5f - ((i-int(width_int))/float(int(width_ext)-int(width_int)))))-1)*0.25f* std::sin(2*3.14f*u);
			m.position[(where - k + 1) *N_s - i].x += verif * radius*0.5f*(std::sin(-Pi*(0.5f - ((i-int(width_int))/float(int(width_ext)-int(width_int)))))-1)*0.25f* std::cos(2*3.14f*v);
			m.position[(where - k + 1) *N_s - i].y += verif * radius*0.5f*(std::sin(-Pi*(0.5f - ((i-int(width_int))/float(int(width_ext)-int(width_int)))))-1)*0.25f* std::sin(2*3.14f*v);
		}
	}

}



mesh Cylinder::create_forest_cylinder_mesh(float radius, float height, int N_f, int N_s)
{
	// Create a cylinder, especially used for c_terrains
    mesh m = Cylinder::create_cylinder_mesh(radius, height, N_f, N_s);
	
	int floor_river = int((N_f/6.0f) * rand()/max_ran + 200.0f);

	std::vector<int> floor_river_indicator;
	for(int k=0; k<N_s; k++){
		floor_river_indicator.push_back(floor_river + int(20.0f * sin(6.0f*Pi * k/float(N_s))));
		//floor_river_indicator.push_back(floor_river);
	}
	for(int k = 0; k<N_s; k++){
		float u = k / float(N_s);
		int lim_floor = int(float(N_f)/50); //largeur fond riviere
		//Centre de la riviere
		m.position[floor_river_indicator[k]*N_s + k].x -= radius*0.25f* std::cos(2*3.14f*u);
		m.position[floor_river_indicator[k]*N_s + k].y -= radius*0.25f* std::sin(2*3.14f*u);
		for(int i = 1; i<lim_floor; i++){
			m.position[(floor_river_indicator[k]+i)*N_s + k].x -= radius*0.25f* std::cos(2*3.14f*u);
			m.position[(floor_river_indicator[k]+i)*N_s + k].y -= radius*0.25f* std::sin(2*3.14f*u);
			m.position[(floor_river_indicator[k]-i)*N_s + k].x -= radius*0.25f* std::cos(2*3.14f*u);
			m.position[(floor_river_indicator[k]-i)*N_s + k].y -= radius*0.25f* std::sin(2*3.14f*u);
		}
		
		//Bords de la riviere
		for(int i = lim_floor; i<4*lim_floor; i++){
			m.position[(floor_river_indicator[k]+i)*N_s + k].x += radius*0.5f*(std::sin(-Pi*(0.5f - ((i-lim_floor)/3.0f)/float(lim_floor)))-1)*0.25f * std::cos(2*3.14f*u);
			m.position[(floor_river_indicator[k]+i)*N_s + k].y += radius*0.5f*(std::sin(-Pi*(0.5f - ((i-lim_floor)/3.0f)/float(lim_floor)))-1)*0.25f* std::sin(2*3.14f*u);
			m.position[(floor_river_indicator[k]-i)*N_s + k].x += radius*0.5f*(std::sin(-Pi*(0.5f - ((i-lim_floor)/3.0f)/float(lim_floor)))-1)*0.25f* std::cos(2*3.14f*u);
			m.position[(floor_river_indicator[k]-i)*N_s + k].y += radius*0.5f*(std::sin(-Pi*(0.5f - ((i-lim_floor)/3.0f)/float(lim_floor)))-1)*0.25f* std::sin(2*3.14f*u);
		}
	}

	put_lake(m, radius, N_f, N_s, N_f/2);

    m.fill_empty_field();
    return m;
}

