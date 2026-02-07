#pragma once

#include <SFML/Graphics.hpp>
#include <random>




class Airspace {
public:

	struct final_approach_fix {
		std::string rw_no;
		sf::Vector2f position;
		float heading;
	};

	struct waypoint {
		std::string wp_name;
		sf::Vector2f position;
	};

	int runway_length = 50;
	int ils_range = 80;

	// COLORS
	sf::Color runway_color = sf::Color(90, 130, 130);
	sf::Color waypoint_color = sf::Color(42, 90, 90);
	sf::Color ils_line_color = sf::Color(0, 50, 50);
	
	sf::Color bg_color = sf::Color(18, 18, 18);

	sf::Color ring_color = sf::Color(42, 42, 42);

	//CONVERSION CONSTANTS
	const float deg_to_rad = 3.14159265f / 180.f;
	const float rad_to_deg = 180.f / 3.14159265f;

	//
	sf::Vector2u screen_size;
	std::string fontpath = "assets/B612-Regular.ttf";
	sf::Font font;

	//ARRAYS
	sf::VertexArray waypoints;

	std::vector<sf::Text> waypoint_labels;
	std::vector<sf::Vector2f> airport_positions;

	std::vector<final_approach_fix> final_approach_fixes;
	std::vector<waypoint> waypoints_array;


	Airspace(sf::Vector2u screen_size);

	float random_range(float lower, float upper);

	std::string random_wp_name();

	sf::Text gen_rw_number(sf::Vector2f startpos, sf::Vector2f endpos, final_approach_fix& faf);

	void spawn_waypoints(int amount);

	void gen_range_rings(sf::Vector2f center, std::vector<sf::VertexArray>& circles); 

	sf::VertexArray spawn_airport(int runways, std::vector<sf::Text>& rw_nos, sf::VertexArray& ils_lines);

	void draw(sf::RenderWindow& window);
};
