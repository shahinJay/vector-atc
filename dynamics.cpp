#include <SFML/Graphics.hpp>

#include <iostream>
#include <math.h>

#include "airspace.h"
#include "aircraft.h"
#include "dynamics.h"

Dynamics::Dynamics(sf::RenderWindow& r_window, Airspace& airspace) {
	this->window = &r_window;
	this->airspace = &airspace;
}

float Dynamics::random_range(float lower, float upper) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(lower, upper);

	return dist(gen);
}

float Dynamics::distance(sf::Vector2f vec1, sf::Vector2f vec2) {
	return sqrt((vec2.x - vec1.x) * (vec2.x - vec1.x) + (vec2.y - vec1.y) * (vec2.y - vec1.y));
}

void Dynamics::acas() {
	for (Aircraft& curr : this->aircrafts) { //YES, I am brute forcing it (for now)
		for (Aircraft& other : this->aircrafts) {
			float alt_diff = curr.altitude - other.altitude;
			if (&curr != &other ) {
				
				float dist = distance(curr.position, other.position);
				if (dist < collision_threshold && std::abs(alt_diff) < 1000) {
					curr.collision_course = true;
				}
				else {
					curr.collision_course = false;
				}
			}
		}
	}
}

void Dynamics::spawn_departure_traffic() {

}

sf::Vector2f Dynamics::set_arrival_position() {
	// Choose region in [0,3]
	int region = static_cast<int>(std::floor(random_range(0.f, 4.f)));

	switch (region) {
		case 0: // WEST
			return sf::Vector2f(
				random_range(-200.f, 0.f),
				random_range(-200.f, this->window->getSize().y + 200.f)
			);
		case 1: // NORTH
			return sf::Vector2f(
				random_range(-200.f, this->window->getSize().x + 200.f),
				random_range(-200.f, 0.f)
			);
		case 2: // EAST
			return sf::Vector2f(
				random_range(this->window->getSize().x, this->window->getSize().x + 200.f),
				random_range(-200.f, this->window->getSize().y + 200.f)
			);
		case 3: // SOUTH
			return sf::Vector2f(
				random_range(-200.f, this->window->getSize().x + 200.f),
				random_range(this->window->getSize().y, this->window->getSize().y + 200.f)
			);
		default: // fallback to NORTH
			return sf::Vector2f(
				random_range(-200.f, this->window->getSize().x + 200.f),
				random_range(-200.f, 0.f)
			);
	}
}

void Dynamics::spawn_arrival_traffic(int amount) {
	for (int i = 0; i < amount; i++) {

		sf::Vector2f position = set_arrival_position();
		sf::Vector2f center = sf::Vector2f(this->window->getSize().x / 2.f, 
											this->window->getSize().y / 2.f);

		float dx = center.x - position.x;
		float dy = center.y - position.y;

		float rad = std::atan2(dy, dx);
		float deg_math = rad * this->rad_to_deg; 

		float heading = deg_math;

		if (heading < 0.f)
			heading += 360.f;

		float speed = random_range(3,7);
		float altitude = random_range(1000.f, 2000.f);

		Aircraft aircraft(*this->airspace, i, position, heading, speed, altitude);

		this->aircrafts.push_back(aircraft);
	}
}

void Dynamics::spawn_traffic(int amount) {
	int arrival_traffic = static_cast<int>(std::round(amount / 2));
	//Departure traffic

	spawn_arrival_traffic(arrival_traffic);
	//spawn_dep_traffic
}


void Dynamics::atc_transmit(std::string command) {
	for (Aircraft& ac : this->aircrafts) {
		ac.listen(command);
	}
}

void Dynamics::run_traffic() {
	acas();
	for (Aircraft& aircraft : this->aircrafts) {
		aircraft.position.x += aircraft.velocity.x;
		aircraft.position.y += aircraft.velocity.y;
	}
}

void Dynamics::draw_aircrafts() {
	for (Aircraft& aircraft : this->aircrafts) {
		aircraft.draw(*this->window);
	}
}

