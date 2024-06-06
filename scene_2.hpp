#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"

// This definitions allow to use the structures: mesh, mesh_drawable, etc. without mentionning explicitly cgp::
using cgp::mesh;
using cgp::mesh_drawable;
using cgp::vec3;
using cgp::numarray;
using cgp::timer_basic;

// Variables associated to the GUI (buttons, etc)
struct gui_parameters {
	bool display_frame = true;
	bool display_cylinder = false;
	bool display_island = false;
	bool display_wireframe = false;
	bool running_goat = false;
};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit_euler camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	timer_basic timer;

	mesh_drawable c_terrain_1; //c_terrain : cylinder terrain, base of the world
	mesh_drawable s_island_1; //floatting island
	mesh_drawable top_island_1;
	mesh_drawable goat; //Goat's corpse
	mesh_drawable goat_hd; //Goat's head
	mesh_drawable goat_tl; //Goat's tail
	mesh_drawable goat_frl_R; //Goat's front right leg
	mesh_drawable goat_frl_L; //Goat's front left leg
	mesh_drawable goat_frf_R; //Goat's front right foot
	mesh_drawable goat_frf_L; //Goat's front left foot
	mesh_drawable goat_bkl_R; //Goat's back right leg
	mesh_drawable goat_bkl_L; //Goat's back left leg
	mesh_drawable goat_bkf_R; //Goat's back right foot
	mesh_drawable goat_bkf_L; //Goat's back left foot

	cgp::hierarchy_mesh_drawable hierarchy;
	/* ALREADY HERE
	mesh_drawable terrain;
	mesh_drawable water;
	mesh_drawable tree;
	mesh_drawable cube1;
	mesh_drawable cube2; */


	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop


	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

	void display_info();

};





