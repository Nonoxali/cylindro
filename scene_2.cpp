#include "scene.hpp"
#include "island.hpp"
#include "cylinder.hpp"
#include <random>
#include <vector>
#include <algorithm>


using namespace cgp;
using namespace Island;
using namespace Cylinder;


// This function is called only once at the beginning of the program
// This function can contain any complex operation that can be pre-computed once
void scene_structure::initialize()
{
	std::cout << "Start function scene_structure::initialize()" << std::endl;

	// Set the behavior of the camera and its initial position
	// ********************************************** //
	camera_control.initialize(inputs, window); 
	camera_control.set_rotation_axis_z(); // camera rotates around z-axis
	//   look_at(camera_position, targeted_point, up_direction)
	camera_control.look_at(
		{ 5.0f, -4.0f, 3.5f } /* position of the camera in the 3D scene */,
		{0,0,0} /* targeted point in 3D scene */,
		{0,0,1} /* direction of the "up" vector */);

	// Display general information
	display_info();
	// Create the global (x,y,z) frame
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());


	// Create the shapes seen in the 3D scene
	// ********************************************** //

	mesh c_terrain_mesh = create_forest_cylinder_mesh(10.0f, 50.0f, 2000, 800);
	c_terrain_1.initialize_data_on_gpu(c_terrain_mesh);
	c_terrain_1.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sand.jpg");
	c_terrain_1.model.rotation = rotation_transform::from_axis_angle({1,0,0}, Pi / 2.0f);


	mesh s_island_mesh = create_fractal_cone(5.0f, 4.0f, 60, 10);
	mesh top_island_mesh = create_floatting_island_terrain(4.0f, 60, 10);
	//fractal_cone_adaptation(s_island_mesh, top_island_mesh, 60, 10);

	s_island_1.initialize_data_on_gpu(s_island_mesh);
	s_island_1.material.color = { 0.5f,0.5f,0.5f };
	s_island_1.model.translation = {0,13.0f,0 };
	s_island_1.model.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, Pi);
	
	top_island_1.initialize_data_on_gpu(top_island_mesh);
	top_island_1.material.color = { 0.0f,0.5f,0.0f };
	top_island_1.model.translation = { 0,13.0f,0 };
	//top_island_1.model.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, Pi);
	



	//Goat creation with hierarchy
	goat.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/Corps.obj"));
	goat.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/UVGoat_2.png");
	//goat.model.translation = { -5.0f,0,0 };
	goat_hd.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/Tete.obj"));
	goat_hd.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/UVGoat_2.png");
	//goat_hd.model.translation = { -5.0f,0,0 };
	goat_tl.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/Queue.obj"));
	goat_tl.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/UVGoat_2.png");
	//goat_tl.model.translation = { -5.0f,0,0 };
	goat_frl_R.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/Patte_av_R.obj"));
	goat_frl_R.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/UVGoat_2.png");
	//goat_frl_R.model.translation = { -5.0f,0,0 };
	goat_frf_R.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/Pied_av_R.obj"));
	goat_frf_R.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/UVGoat_2.png");
	//goat_frf_R.model.translation = { -5.0f,0,0 };
	goat_frl_L.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/Patte_av_L.obj"));
	goat_frl_L.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/UVGoat_2.png");
	//goat_frl_L.model.translation = { -5.0f,0,0 };
	goat_frf_L.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/Pied_av_L.obj"));
	goat_frf_L.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/UVGoat_2.png");
	//goat_frf_L.model.translation = { -5.0f,0,0 };
	goat_bkl_R.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/Patte_ar_R.obj"));
	goat_bkl_R.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/UVGoat_2.png");
	//goat_bkl_R.model.translation = { -5.0f,0,0 };
	goat_bkf_R.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/Pied_ar_R.obj"));
	goat_bkf_R.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/UVGoat_2.png");
	//goat_bkf_R.model.translation = { -5.0f,0,0 };
	goat_bkl_L.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/Patte_ar_L.obj"));
	goat_bkl_L.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/UVGoat_2.png");
	//goat_bkl_L.model.translation = { -5.0f,0,0 };
	goat_bkf_L.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/Pied_ar_L.obj"));
	goat_bkf_L.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/UVGoat_2.png");
	//goat_bkf_L.model.translation = { -5.0f,0,0 };
	

	hierarchy.add(goat, "corps");
	hierarchy.add(goat_hd, "tete", "corps", { 2.30f, 5.46f,0 });
	hierarchy.add(goat_tl, "queue", "corps", {-3.37f, 4.83f, 0});
	hierarchy.add(goat_frl_R, "patte_av_R", "corps", { 1.38f, 4.08f, 0.89f });
	hierarchy.add(goat_frl_L, "patte_av_L", "corps", {1.38f, 4.08f, -0.89f});
	hierarchy.add(goat_bkl_R, "patte_ar_R", "corps", { -2.31f, 3.96f, 1.01f });
	hierarchy.add(goat_bkl_L, "patte_ar_L", "corps", { -2.31f, 3.96f, -1.01f });
	hierarchy.add(goat_frf_R, "pied_av_R", "patte_av_R", { 0.49f, -2.33f, -0.42f });
	hierarchy.add(goat_frf_L, "pied_av_L", "patte_av_L", {0.49f, -2.33f, 0.42f});
	hierarchy.add(goat_bkf_R, "pied_ar_R", "patte_ar_R", { -0.72f, -1.61f, -0.21f });
	hierarchy.add(goat_bkf_L, "pied_ar_L", "patte_ar_L", {-0.72f, -1.61f, 0.21f});
	hierarchy["tete"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, Pi/10.0f);
	hierarchy["corps"].transform_local.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, Pi / 2.0f);
	/*
	float L = 5.0f;
	mesh terrain_mesh = mesh_primitive_grid({ -L,-L,0 }, { L,-L,0 }, { L,L,0 }, { -L,L,0 }, 100, 100);
	deform_terrain(terrain_mesh);
	terrain.initialize_data_on_gpu(terrain_mesh);
	terrain.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sand.jpg");

	float sea_w = 8.0;
	float sea_z = -0.8f;
	water.initialize_data_on_gpu(mesh_primitive_grid({ -sea_w,-sea_w,sea_z }, { sea_w,-sea_w,sea_z }, { sea_w,sea_w,sea_z }, { -sea_w,sea_w,sea_z }));
	water.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sea.png");

	tree.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/palm_tree/palm_tree.obj"));
	tree.model.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, Pi / 2.0f);
	tree.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/palm_tree/palm_tree.jpg", GL_REPEAT, GL_REPEAT);

	cube1.initialize_data_on_gpu(mesh_primitive_cube({ 0,0,0 }, 0.5f));
	cube1.model.rotation = rotation_transform::from_axis_angle({ -1,1,0 }, Pi / 7.0f);
	cube1.model.translation = { 1.0f,1.0f,-0.1f };
	cube1.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/wood.jpg");

	cube2 = cube1; */

}


// This function is called permanently at every new frame
// Note that you should avoid having costly computation and large allocation defined there. This function is mostly used to call the draw() functions on pre-existing data.
void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();

	// Update time
	timer.update();

	//Everytime animation
	hierarchy["queue"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,1,0 }, std::cos(2 * timer.t) / 6.0f);
	hierarchy["tete"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, (std::cos(0.5*timer.t)-1)/16.0f);

	//Running goat animation
	if (gui.running_goat) {
		hierarchy["tete"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,1,1 }, std::cos(2 * timer.t) / 5.0f);
		hierarchy["patte_ar_L"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, std::cos(2 * timer.t) / 2.0f);
		hierarchy["pied_ar_L"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, -std::min(0.0f, std::cos(2 * timer.t) / 4.0f));
		hierarchy["patte_ar_R"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, std::cos(2 * timer.t + Pi) / 2.0f);
		hierarchy["pied_ar_R"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, -std::min(0.0f, std::cos(2 * timer.t + Pi) / 4.0f));
		hierarchy["patte_av_L"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, std::cos(2 * timer.t + Pi) / 2.0f);
		hierarchy["pied_av_L"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, -std::max(0.0f, std::cos(2 * timer.t + Pi) / 4.0f));
		hierarchy["patte_av_R"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, std::cos(2 * timer.t) / 2.0f);
		hierarchy["pied_av_R"].transform_local.rotation = rotation_transform::from_axis_angle({ 0,0,1 }, -std::max(0.0f, std::cos(2 * timer.t) / 4.0f));
	}
	
	
	// conditional display of the global frame (set via the GUI)
	if (gui.display_frame)
		draw(global_frame, environment);
	

	// Draw all the shapes
	if(gui.display_cylinder)
		draw(c_terrain_1, environment);

	if(gui.display_island){
		draw(s_island_1, environment);
		draw(top_island_1, environment);
	}

	// This function must be called before the drawing in order to propagate the deformations through the hierarchy
	hierarchy.update_local_to_global_coordinates();

	// Draw the hierarchy as a single mesh
	draw(hierarchy, environment);
	if (gui.display_wireframe)
		draw_wireframe(hierarchy, environment);



	/* draw(water, environment);
	draw(tree, environment);
	draw(cube1, environment); */

	// Animate the second cube in the water
	/* cube2.model.translation = { -1.0f, 6.0f+0.1*sin(0.5f*timer.t), -0.8f + 0.1f * cos(0.5f * timer.t)};
	cube2.model.rotation = rotation_transform::from_axis_angle({1,-0.2,0},Pi/12.0f*sin(0.5f*timer.t));
	draw(cube2, environment); */

	
	if (gui.display_wireframe) {
		draw_wireframe(c_terrain_1, environment);
		draw_wireframe(s_island_1, environment);
		draw_wireframe(top_island_1, environment);

		/* draw_wireframe(water, environment);
		draw_wireframe(tree, environment);
		draw_wireframe(cube1, environment);
		draw_wireframe(cube2, environment); */
	} 
	







}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
	ImGui::Checkbox("Cylinder", &gui.display_cylinder);
	ImGui::Checkbox("Island", &gui.display_island);
	ImGui::Checkbox("Running Goat", &gui.running_goat);

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
	camera_control.action_keyboard(environment.camera_view);
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