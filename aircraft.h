#pragma once

#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <array>

#include "airspace.h"



class Aircraft {

public:
	Airspace* airspace;

	sf::Vector2f position;
	sf::Vector2f velocity;

	std::array<std::string, 3> callsigns = { "ABC1000", "ABC1001" , "ABC1002" };
	std::string callsign;

	float heading;
	float groundspeed;
	float altitude;

	bool change_required = false;
	int state_to_change = -1;
	bool owned = false;
	bool collision_course = false;


	// STATES TO CHANGE                 HEADING  |   SPEED |    ALTITUDE |   WAYPOINT |  RUNWAY/Landing
	std::array<bool, 5> states_to_change = { false,		false,		false,		false,		false };
	bool landing_seq = false;

	float target_heading;
	float target_speed;
	float target_altitude;
	std::string target_waypoint;
	std::string target_runway;
	Airspace::final_approach_fix target_faf;

	/*
	Only store target FAF(Final Approach Fixes) as they also contain
	the information about the runway.
	*/


	// CONSTRAINTS
	float turn_rate = 0.01f;
	float accel_rate = 0.001f;
	float climb_rate = 0.001f;

	int ils_dist_threshold = 40;
	int ils_angle_threshold = 20;

	int rw_dist_threshold = 5;
	int rw_angle_threshold = 5;

	int approach_speed = 4;
	int landing_speed = 2;

	//COLORS, Graphics and assets
	sf::Color unowned_color = sf::Color(75, 99, 75);
	sf::Color owned_color = sf::Color::White;
	sf::Color warning_color = sf::Color::Red;
	sf::Color target_color = sf::Color(75, 99, 75);
	float rect_size = 7;

	sf::Vector2f label_point;
	std::string fontpath = "assets/B612-Regular.ttf";
	sf::Font font;

	const float deg_to_rad = 3.14159265f / 180.f;
	const float rad_to_deg = 180.f / 3.14159265f;


	Aircraft(Airspace& airspace, int ID, sf::Vector2f position, float heading, float groundspeed, float altitude);

	//MATH
	sf::Vector2f heading_to_vector();
	float random_range(float lower, float upper);
	float distance(sf::Vector2f vec1, sf::Vector2f vec2);
	int vec_to_angle(sf::Vector2f vec1, sf::Vector2f vec2);

	//init
	void assign_callsign(int ID);

	//TAKE-OFF / LANDING SEQUENCES
	void takeoff();
	void land(Airspace::final_approach_fix);

	//controls
	void change_heading();
	void change_speed();
	void change_altitude();

	//routing
	void direct_to_runway();
	void direct_to_waypoint();

	//
	void parse_command(std::string command, std::vector<std::string>& parsed_command);

	void listen(std::string command);

	void update_label(sf::RenderWindow& window);
	
	void draw(sf::RenderWindow& window);
};