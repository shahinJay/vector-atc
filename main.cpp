#include <iostream>
#include <SFML/graphics.hpp>
#include <string>
#include <thread>
#include <mutex>

#include "airspace.h"
#include "aircraft.h"
#include "dynamics.h"

std::string global_command = " ";
std::mutex command_mutex;

void input_thread() {
	while (true) {
		std::string input;
		std::getline(std::cin, input);

		command_mutex.lock();
		global_command = input;
		command_mutex.unlock();
	}
}

float random_range(float lower, float upper) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(lower, upper);

	return dist(gen);
}
	
void generate_airspace(Airspace& arsp, sf::VertexArray& airports, std::vector<sf::Text>& rw_nos, sf::VertexArray& fafs, std::vector<sf::VertexArray>& range_rings, sf::Vector2f screen_center) {
	arsp.final_approach_fixes.clear();
	rw_nos.clear();
	fafs.clear();
	range_rings.clear();
	 
	airports = arsp.spawn_airport(3, rw_nos, fafs); //FIX: you dont need to recalculate all this 
	arsp.gen_range_rings(screen_center, range_rings);
}

void draw_airspace(sf::RenderWindow& window, Airspace& arsp, sf::VertexArray& airports, std::vector<sf::Text>& rw_nos, sf::VertexArray& fafs, std::vector<sf::VertexArray>& range_rings) {
	window.draw(airports);
	window.draw(fafs);
	window.draw(arsp.waypoints);

	for (sf::VertexArray ring : range_rings) {
		window.draw(ring);
	}

	for (sf::Text& rw : rw_nos) {
		window.draw(rw);
	}
}

int main() {
	sf::Vector2u screen_size = sf::Vector2u(1080, 720);
	sf::Vector2f screen_center = sf::Vector2f(screen_size.x / 2, screen_size.y / 2);
	sf::RenderWindow window(sf::VideoMode(screen_size), "VECTOR ATC SIM"); //WINDOW

	std::thread t(input_thread);
	t.detach();

	//AIRSPACE GENERATION---------------------------------------------------------

	Airspace arsp(screen_size); //airspace object
	sf::VertexArray airports; 
	std::vector<sf::Text> rw_nos;
	sf::VertexArray fafs(sf::PrimitiveType::Lines, 2);
	std::vector<sf::VertexArray> range_rings;

	arsp.spawn_waypoints(arsp.random_range(5, 15));

	generate_airspace(arsp, airports, rw_nos, fafs, range_rings, screen_center);


	//-----------------------------------------------------------------------------

	//DYNAMICS - and - CONTROLS-----------------------------------------------------
	Dynamics dynamics(window, arsp);

	sf::Clock delta_clock;
	float delta_time = 0;
	float time_elapsed = 0.0f;

	//AIRCRAFT SPAWNING------------------------------------------------------------
	std::vector<Aircraft> aircrafts;

	dynamics.spawn_traffic(10);
	//------------------------------------------------------------------------


	while (window.isOpen())
	{

		delta_time = delta_clock.restart().asSeconds(); //reset clock
		time_elapsed += delta_time;

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
		}
		window.clear(arsp.bg_color);


		//COMMAND TRANSFER
		command_mutex.lock();
		if (global_command != " ") {
			dynamics.atc_transmit(global_command);
			global_command = " ";
		}
		command_mutex.unlock();
		//-----------------

		draw_airspace(window, arsp, airports, rw_nos, fafs, range_rings);
		arsp.draw(window);

		if (time_elapsed >= 1.0f) {
			dynamics.run_traffic();
			time_elapsed = 0;
		}

		dynamics.draw_aircrafts();

		window.display();
		
	}
	return 0;
}

