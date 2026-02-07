#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <math.h>

#include "airspace.h"

Airspace::Airspace(sf::Vector2u screen_size) {
	this->screen_size = screen_size;

	if (!this->font.openFromFile(this->fontpath)) {
		std::cout << "ERROR loading font" << std::endl;
	}

	this->waypoints.setPrimitiveType(sf::PrimitiveType::Triangles);

}

float Airspace::random_range(float lower, float upper){
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(lower, upper);

	return dist(gen);
}

std::string Airspace::random_wp_name() {
	int n = 0;
	std::string name;

	for (int i = 0; i < 5; i++) {
		n = random_range(0, 26);

		switch (n) {
			case 0:
				name += "A";
				break;
			case 1:
				name += "B";
				break;
			case 2:
				name += "C";
				break;
			case 3:
				name += "D";
				break;
			case 4:
				name += "E";
				break;
			case 5:
				name += "F";
				break;
			case 6:
				name += "G";
				break;
			case 7:
				name += "H";
				break;
			case 8:
				name += "I";
				break;
			case 9:
				name += "J";
				break;
			case 10:
				name += "K";
				break;
			case 11:
				name += "L";
				break;
			case 12:
				name += "M";
				break;
			case 13:
				name += "N";
				break;
			case 14:
				name += "O";
				break;
			case 15:
				name += "P";
				break;
			case 16:
				name += "Q";
				break;
			case 17:
				name += "R";
				break;
			case 18:
				name += "S";
				break;
			case 19:
				name += "T";
				break;
			case 20:
				name += "U";
				break;
			case 21:
				name += "V";
				break;
			case 22:
				name += "W";
				break;
			case 23:
				name += "X";
				break;
			case 24:
				name += "Y";
				break;
			case 25:
				name += "Z";
				break;
			case 26:
				name += "A";
				break;
			default:
				break;
		}
	}
	return name;
}


sf::Text Airspace::gen_rw_number(sf::Vector2f startpos, sf::Vector2f endpos, final_approach_fix& faf) {
	
	sf::Text text(this->font);

	//math---------------------------------------------------------
	float dx = startpos.x - endpos.x;
	float dy = startpos.y - endpos.y;

	float rad = std::atan2(dy, dx);
	float degrees = rad * this->rad_to_deg - 90;

	if (degrees < 0)
		degrees += 360;

	int heading = static_cast<int>(std::round(degrees)) % 360;
	std::string rw_marker = std::to_string(heading / 10);
	//-------------------------------------------------------------

	text.setString(rw_marker);
	text.setCharacterSize(15);
	text.setFillColor(sf::Color::White);
	text.setPosition(startpos);

	//STORING FAF DATA
	faf.heading = heading;
	faf.rw_no = rw_marker;

	return text;
}

void Airspace::spawn_waypoints(int amount) {
	
	double radius = 7;
	float degToRad = this->deg_to_rad;
	this->waypoint_labels.clear();

	for (int i = 0; i < amount; i++) {
		float c_x = random_range(0, this->screen_size.x);
		float c_y = random_range(0, this->screen_size.y);

		waypoint wp; //STORING WAYPOINT
		wp.position = sf::Vector2f(c_x, c_y);
		wp.wp_name = random_wp_name();
		this->waypoints_array.push_back(wp);

		
		sf::Text waypoint_label(font); //LABELS
		waypoint_label.setString(wp.wp_name);
		waypoint_label.setCharacterSize(8);
		waypoint_label.setFillColor(sf::Color::White);
		waypoint_label.setPosition(sf::Vector2f(c_x + 5, c_y + 5));
		this->waypoint_labels.push_back(waypoint_label);

		for (int p = 0; p < 3; p++) {
			float angle = (270.f + p * 120.f) * degToRad;
			float x = c_x + radius * cos(angle);
			float y = c_y + radius * sin(angle);

			this->waypoints.append(sf::Vertex(sf::Vector2f(x, y), this->waypoint_color));
			
		}
		
	}
}

void Airspace::gen_range_rings(sf::Vector2f center, std::vector<sf::VertexArray>& circles) {
	int step_size = 36;
	int radius = 200;
	int amount = 4;

	sf::VertexArray circle(sf::PrimitiveType::LineStrip);
	sf::Vector2f point;
	for (int c = 0; c < amount; c++) {
		for (int i = 0; i <= step_size; i++) {
			point.x = center.x + radius * cos(i * (360 / step_size) * this->deg_to_rad);
			point.y = center.y + radius * sin(i * (360 / step_size) * this->deg_to_rad);
			circle.append(sf::Vertex(sf::Vector2f(point), this->ring_color));
		}
		circles.push_back(circle);
		circle.clear();
		radius += 100;
	}
}


sf::VertexArray Airspace::spawn_airport(int runways, std::vector<sf::Text>& rw_nos, sf::VertexArray& ils_lines) {
	sf::VertexArray lines(sf::PrimitiveType::Lines);
	int shrink_x = 300;
	int shrink_y = 300;

	sf::Vector2f endpos;
	sf::Vector2f startpos;
	
	final_approach_fix faf;
	sf::Vector2f faf_1;
	sf::Vector2f faf_2;

	float angle_rad = random_range(0, 360) * this->deg_to_rad;

	startpos.x = random_range(0 + shrink_x, this->screen_size.x - shrink_x);// RUNWAY start
	startpos.y = random_range(0 + shrink_y, this->screen_size.y - shrink_y);

	faf_1.x = startpos.x + this->ils_range * cos(angle_rad + (180 * deg_to_rad));// FAF 1
	faf_1.y = startpos.y + this->ils_range * sin(angle_rad + (180 * deg_to_rad));
	
	endpos.x = startpos.x + this->runway_length * cos(angle_rad);// RUNWAY other end
	endpos.y = startpos.y + this->runway_length * sin(angle_rad);

	faf_2.x = endpos.x + this->ils_range * cos(angle_rad);// FAF 2
	faf_2.y = endpos.y + this->ils_range * sin(angle_rad);

	faf.position = faf_1;
	rw_nos.push_back(gen_rw_number(startpos, endpos, faf));
	final_approach_fixes.push_back(faf);

	faf.position = faf_2;
	rw_nos.push_back(gen_rw_number(endpos, startpos, faf));
	final_approach_fixes.push_back(faf);
	
	lines.append(sf::Vertex(startpos, this->runway_color));
	lines.append(sf::Vertex(endpos, this->runway_color));

	ils_lines.append(sf::Vertex(sf::Vector2f(faf_1), this->ils_line_color));
	ils_lines.append(sf::Vertex(sf::Vector2f(startpos), this->ils_line_color));
	ils_lines.append(sf::Vertex(sf::Vector2f(faf_2), this->ils_line_color));
	ils_lines.append(sf::Vertex(sf::Vector2f(endpos), this->ils_line_color));

	return lines;
}

void Airspace::draw(sf::RenderWindow& window) {
	for (sf::Text wp_l : this->waypoint_labels) {
		window.draw(wp_l);
	}
}
