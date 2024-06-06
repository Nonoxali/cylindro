#include "scene.hpp"
#include <iostream> 
#include <math.h>
#include "cylinder.hpp"
#include "island.hpp"
using namespace std;
using namespace cgp;
//using namespace Cylinder;
//using namespace Island;


float angle_up = -Pi/3;
float angle_right = 0;
float length = 0.5;
float worldx = 0;
float worldy = 0;
float const max_ran = float(RAND_MAX);
int planet_r = 10;
int planet_h = planet_r* 7.5;
float angle_current = Pi / 2.0f;
int day_length = 60;

//Day cycle rgb colors 
vec3 DAY = { 173 / 256.0f,213 / 256.0f,230 / 256.0f };
vec3 DUSK = { 213 / 256.0f,125 / 256.0f,106 / 256.0f };
vec3 DAWN = { 173 / 256.0f, 163 / 256.0f, 245 / 256.0f };
vec3 NIGHT = { 18 / 256.0f,57 / 256.0f,135 / 256.0f };

// This function is called only once at the beginning of the program
// This function can contain any complex operation that can be pre-computed once

int sign(int x) {
	return (x > 0) - (x < 0);
}


std::vector<vec3> generate_positions_on_terrain(int N, float cyl_legnth)
{
	std::vector<cgp::vec3> tree_position;
	for (int i = 0; i < N; ++i) {
		float z = rand_uniform(-cyl_legnth / 2, cyl_legnth / 2);
		float ang = rand_uniform(0, 2*Pi);

		tree_position.push_back({ z,ang,0});
	}
	return tree_position;

}



void scene_structure::initialize()
{
	std::cout << "Start function scene_structure::initialize()" << std::endl;

	// Set the behavior of the camera and its initial position
	// ********************************************** //
	camera_control.initialize(inputs, window); 
	camera_control.set_rotation_axis_z(); // camera rotates around z-axis
	//   look_at(camera_position, targeted_point, up_direction)
	camera_control.look_at(
		{ 0, -2*planet_r, 0.5*planet_r } /* position of the camera in the 3D scene */,
		{ 0,planet_h,planet_r } /* targeted point in 3D scene */,
		{0,0,1} /* direction of the "up" vector */);

	// Display general information
	display_info();
	// Create the global (x,y,z) frame
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());



	//add custom water shader from a file 
	// 

	opengl_shader_structure water_shader;
	water_shader.load(		project::path + "shaders/mesh_water/mesh_water.vert.glsl",		project::path + "shaders/mesh_water/mesh_water.frag.glsl");

	// Affect the loaded shader to the mesh_drawable

	// Create the shapes seen in the 3D scene
	// ********************************************** //



	cube1.initialize_data_on_gpu(mesh_primitive_cube({ 0,0,0 }, 0.5f));
	cube1.model.rotation = rotation_transform::from_axis_angle({ -1,1,0 }, Pi / 7.0f);
	cube1.model.translation = { 1.0f,1.0f,-0.1f };
	cube1.model.translation = { 0,planet_r/sqrt(2),planet_r / sqrt(2)};
	cube1.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/wood.jpg");

	//mesh planet_mesh = create_cylinder_mesh(planet_r, planet_h);
	mesh planet_mesh = Cylinder::create_forest_cylinder_mesh(planet_r, planet_h, 2000, 800);
	planet.initialize_data_on_gpu(planet_mesh);
	planet.model.rotation = rotation_transform::from_axis_angle({ 0,1,0 }, Pi / 2.0f);
	planet.model.translation = { -planet_h/2.0f,0,0 };
	planet.material.phong = { 0.4f, 0.6f, 0, 1 };  // remove specular effect for the billboard
	planet.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/grass1.png", GL_REPEAT, GL_REPEAT);

	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ZERO);
	glEnable(GL_BLEND);

	mesh water_mesh = Cylinder::create_simple_cylinder_mesh(0.9*planet_r, planet_h);
	water.initialize_data_on_gpu(water_mesh);
	water.model.rotation = rotation_transform::from_axis_angle({ 0,1,0 }, Pi / 2.0f);
	water.model.translation = { -planet_h / 2.0f,0,0 };
	water.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sea.png", GL_REPEAT, GL_REPEAT);
	water.material.alpha = 0.8f;
	water.shader = water_shader;



	mesh edge_mesh = Cylinder::disc_with_texture(planet_r);
	edge1.initialize_data_on_gpu(edge_mesh);
	edge1.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/wall.jpeg");
	edge2.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/wall.jpeg");
	edge1.model.rotation = rotation_transform::from_axis_angle({ 0,1,0 }, Pi / 2.0f);
	edge2.model.rotation = rotation_transform::from_axis_angle({ 0,1,0 }, Pi / 2.0f);
	edge2 = edge1;
	edge1.model.translation = { -planet_h/2.0f,0,0 };
	edge2.model.translation = { planet_h/2.0f,0,0 };
	

	environment.background_color = DUSK;
	

	mesh cone_mesh = Island::create_fractal_cone(5, 4, 60, 10);
	mesh top_mesh = Island::create_floatting_island_terrain(4, 60, 10);
	cone.initialize_data_on_gpu(cone_mesh);
	top.initialize_data_on_gpu(top_mesh);
	Island::fractal_cone_adaptation(cone_mesh, top_mesh, 60, 10);
	cone.model.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, Pi);
	cone.model.translation = { 0,planet_h,planet_r };
	top.model.translation = { 0,planet_h,planet_r };

	//didnt draw ittttt


	tree.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/palm_tree/obj__tree1.obj"));
	tree.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/palm_tree/tree2.png", GL_REPEAT, GL_REPEAT);
	tree.material.phong = { 0.4f, 0.6f, 1, 1 };  // remove specular effect for the billboard
	tree.model.scaling =  0.07f;

	


	//where to place trees
 tree_position = generate_positions_on_terrain(20, planet_h);


}


// This function is called permanently at every new frame
// Note that you should avoid having costly computation and large allocation defined there. This function is mostly used to call the draw() functions on pre-existing data.
void scene_structure::display_frame()
{
	environment.uniform_generic.uniform_float["time"] = timer.t;
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	// Update time
	timer.update();

	// conditional display of the global frame (set via the GUI)
	if (gui.display_frame)
		draw(global_frame, environment);
	
	// set sky color
	float day_time = fmodf(timer.t, day_length)/ day_length;
	//cout << day_time<< "\n";
	
	 if ((0 <= day_time) && ( day_time  < 0.075)) {
		float coeff = (day_time) / 0.075;
		environment.background_color = (1 - coeff) * NIGHT + coeff * DAWN;
		environment.uniform_generic.uniform_vec3["light_color"] = (1 - coeff) * NIGHT + coeff * DAWN;
	}
	 else if ((0.075 <= day_time) && (day_time < 0.15)) {
		 float coeff = (day_time - 0.075) / 0.075;
		 environment.background_color = (1 - coeff) * DAWN + coeff * DAY;
		 environment.uniform_generic.uniform_vec3["light_color"] = (1 - coeff) * DAWN + coeff * vec3({ 1,1,1 });
	 }
	else if ((0.15 <= day_time) && (day_time < 0.5)) {
		environment.background_color = DAY;
		environment.uniform_generic.uniform_vec3["light_color"] = vec3({ 1,1,1 });
	}
	else if ((0.5 <= day_time) && (day_time < 0.575)) {
		float coeff = (day_time - 0.5) / 0.075;
		environment.background_color = (1-coeff)* DAY + coeff *DUSK;
		environment.uniform_generic.uniform_vec3["light_color"] = (1-coeff)* vec3({ 1,1,1 }) + coeff *DUSK;
	}
	else if ((0.575 <= day_time) && (day_time < 0.65)) {
		float coeff = (day_time - 0.575) / 0.075;
		environment.background_color = (1-coeff)* DUSK + coeff *NIGHT;
		environment.uniform_generic.uniform_vec3["light_color"] = (1-coeff)* DUSK + coeff *NIGHT;

	}
	else if ((0.65 <= day_time) && (day_time < 1)) {
		environment.background_color = NIGHT;
		environment.uniform_generic.uniform_vec3["light_color"] = NIGHT;
	}



	rotation_transform R1 = rotation_transform::from_axis_angle({ 0,1,0 }, Pi / 2.0f);
	vec3 T1 = { -planet_h / 2.0f,0,0 };

	//transformations once cylinder world 
	rotation_transform cube_z_orientation = rotation_transform::from_axis_angle({ 1,0,0 }, -angle_up);
	cube1.model.translation = {angle_right,sin(angle_up)*planet_r,cos(angle_up)*planet_r };

	rotation_transform R2 = rotation_transform::from_axis_angle({ 0,0,-1 }, worldy);

	//wolrd transformations
	planet.model.rotation = R1*R2;
	planet.model.translation = { -planet_h/2 + worldx,0,0 };

	edge1.model.rotation = R1 * R2;
	edge1.model.translation = { -planet_h / 2 + worldx,0,0 };

	edge2.model.rotation = R1 * R2;
	edge2.model.translation = { +planet_h / 2 + worldx,0,0 };

	water.model.rotation = R1 * R2 * rotation_transform::from_axis_angle({ 0,0,-1 }, -0.01*timer.t );
	water.model.translation = { -planet_h / 2 + worldx,0,0 };
		

	//draw the trees
	for (int j = 0; j != tree_position.size(); j++) {
		//   
		tree.model.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, -worldy - tree_position[j][1] + Pi / 2.0f);
		tree.model.translation = {tree_position[j][0] + worldx,sin(tree_position[j][1] + worldy) * planet_r, cos(tree_position[j][1] + worldy) * planet_r };
		draw(tree, environment);

		if (gui.display_wireframe)
			draw_wireframe(tree, environment);
	}

	//sky island mvmt 
	cone.model.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, 0.1*timer.t)* rotation_transform::from_axis_angle({ 1,0,0 }, Pi);
	top.model.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, 0.1*timer.t);
	cone.model.translation = { 0,planet_h,planet_r + cos(timer.t)};
	top.model.translation = { 0,planet_h,planet_r + cos(timer.t)};
	//character rotations
	if (inputs.keyboard.right || inputs.keyboard.left || inputs.keyboard.up || inputs.keyboard.down) {
		vec2 final_orientation = { inputs.keyboard.right - inputs.keyboard.left, inputs.keyboard.up - inputs.keyboard.down };
		float angle_final = atan2(final_orientation[1], final_orientation[0]);
		angle_final = (angle_final >= 0 ? angle_final : (2 * Pi + angle_final));

		float diff = angle_final - angle_current;
		if (std::abs(diff) >= 0.2) {
			cout <<"diss"<< std::abs(diff) << "\n";
			cout <<"final"<< angle_final << "\n";
			cout <<"current"<< angle_current << "\n";
			if (std::abs(diff) >= Pi) {
				//aller sens contraire au signe 
				cout << "sub\n";
				angle_current = angle_current - sign(diff) * 0.1;
			}
			else {
				//aller sens signe 
				cout << "add\n";
				angle_current = (angle_current + sign(diff) * 0.1 <2*Pi ? angle_current + sign(diff) * 0.1 : angle_current + sign(diff) * 0.1 - 2 * Pi);
			}
			//cout << angle_current << "\n";
			if (angle_current < 0)
				angle_current += 2 * Pi;

			if (angle_current >= 2 * Pi)
				angle_current -= 2 * Pi;
		}
		
		rotation_transform cube_xy_orientation = rotation_transform::from_axis_angle({ 0,0,1 }, angle_current);

		cube1.model.rotation = cube_z_orientation * cube_xy_orientation;
	}

	




	// Draw all the shapes
	draw(terrain, environment);
	draw(planet, environment);
	draw(water, environment);
	draw(cube1, environment);
	draw(edge1, environment);
	draw(edge2, environment);
	draw(top, environment);
	draw(cone, environment);
	

	if (gui.display_wireframe) {
		draw_wireframe(terrain, environment);
		draw_wireframe(water, environment);
		draw_wireframe(cube1, environment);
		draw_wireframe(cube2, environment);
		draw_wireframe(planet, environment);
		draw_wireframe(edge1, environment);
		draw_wireframe(edge2, environment);
	}
	

}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	
	if (inputs.keyboard.down) {
		//cout << "down";
		if (angle_up >= -Pi/2.5)
			angle_up -= Pi/100;// do this rotation
		else
			worldy += Pi/100;  //inverse operation for world
	}

	if (inputs.keyboard.up) {
		//cout << "up";
		if (angle_up <= -Pi/3.5)
			angle_up += Pi/100;// do this rotation
		else
			worldy -= Pi/100;

	}

	if (inputs.keyboard.left) {
		//cout << "left";
		if (angle_right >= -length)
			angle_right -= planet_h/200.0f;// do this rotation
		else if ((angle_right - worldx) > - 0.87* planet_h / 2 )
			worldx += planet_h/200.0f;
	}

	if (inputs.keyboard.right) {
		//cout << "right";
		if (angle_right <= length)
			angle_right += planet_h/200.0f;// do this rotation
		else if ((angle_right - worldx) < 0.87 * planet_h / 2)
			worldx -= planet_h/200.0f;
	}


}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}


void scene_structure::display_info()
{
	std::cout << "\nCAMERA CONTROL:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << camera_control.doc_usage() << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;


	std::cout << "\nSCENE INFO:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "Display here the information you would like to appear at the start of the program on the command line (file scene.cpp, function display_info())." << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;
}