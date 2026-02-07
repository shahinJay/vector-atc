#pragma once

#include <SFML/Graphics.hpp>
#include <math.h>

#include "airspace.h"
#include "aircraft.h"

class Dynamics {

	public:
		sf::RenderWindow* window;

		Airspace* airspace;

		std::vector<Aircraft> aircrafts;
		sf::VertexArray aircrafts_vertex_array;

		std::vector<Aircraft> departing_aircrafts;
		std::vector<Aircraft> arriving_aircrafts;

		int collision_threshold = 80;

		//CONVERSION CONSTANTS
		const float deg_to_rad = 3.14159265f / 180.f;
		const float rad_to_deg = 180.f / 3.14159265f;


		Dynamics(sf::RenderWindow& r_window, Airspace& airspace);

		float random_range(float lower, float upper);

		float distance(sf::Vector2f vec1, sf::Vector2f vec2);

		void acas();

		void spawn_departure_traffic();

		sf::Vector2f set_arrival_position();

		void spawn_arrival_traffic(int amount);
		void spawn_traffic(int amount);

		void atc_transmit(std::string command);

		void run_traffic();

		void draw_aircrafts();
};