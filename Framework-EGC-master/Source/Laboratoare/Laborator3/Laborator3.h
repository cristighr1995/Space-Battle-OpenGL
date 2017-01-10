#pragma once

#include <Component/SimpleScene.h>
#include <string>
#include <Core/Engine.h>

class Laborator3 : public SimpleScene
{
	public:
		Laborator3();
		~Laborator3();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		float GetEnemyAngularStep(float ex, float ey, float tx, float ty);
		void FrameEnd() override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

	protected:
		int scale,translate;
		glm::mat3 modelMatrix;
		glm::ivec2 resolution;
		float translateX, translateY;
		float scaleX, scaleY;
		float angularStep;
		float my_ship_moving_speed;
		int enemy_ship_max_speed = 2;
		float bullet_speed;
		float starting_x = 0, starting_y = 0;
		glm::vec3 corner;
		float ship_length_side = 50;
		glm::vec3 enemy_ship_color;

		float bullet_width = 30;
		float bullet_heigth = 7;
		int interval_of_generation = 2000; // 2 seconds initially
		int interval_subtract_coef = 300;
		int MIN_INTERVAL_SECONDS = 500; // 0.5 seconds
		bool added_enemy = false;
		int how_many_enemies_added = 0;
		float circle_distance_radius;
		int player_score = 0;
		int BLUE_TYPE_SHIP = 0;
		int player_nr_lives = 3;
		int nr_enemy_change_interval = 5;
};
