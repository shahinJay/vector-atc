#pragma once

#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>

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

	float target_heading;
	float target_speed;
	float target_altitude;
	std::string target_waypoint;

	float turn_rate = 0.01;
	float accel_rate = 0.001;
	float climb_rate = 0.001;

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

	void assign_callsign(int ID);

	//
	void change_heading();
	void direct_to_waypoint();
	void change_speed();
	void change_altitude();

	//
	void parse_command(std::string command, std::vector<std::string>& parsed_command);

	void listen(std::string command);

	void update_label(sf::RenderWindow& window);
	
	void draw(sf::RenderWindow& window);
};