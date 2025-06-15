#include "SpriteSheet.h"

Sprite::Sprite()
{
	image=NULL;
}
Sprite::~Sprite()
{
	al_destroy_bitmap(image);
}
void Sprite::InitSprites(int width, int height)
{
	x = 128;
	y = 20;


	maxFrame = 9;
	curFrame = 0;
	frameCount = 0;
	frameDelay = 6;
	frameWidth = 127;
	frameHeight = 128;
	animationColumns = 8;
	animationDirection = 1;

	image = al_load_bitmap("marisa.png");
	al_convert_mask_to_alpha(image, al_map_rgb(47,95,115));
}

void Sprite::UpdateSprites(int width, int height, int dir)
{
	int oldx = x;
	int oldy = y;

	//stop from moving outside which crashes
	if (y <= 1) {
		y = 1;
	}

	if (dir == 1) { //right key
		animationDirection = 1;
		x += 7;
		if (++frameCount > frameDelay)
		{
			frameCount = 0;
			if (++curFrame > 8)
				curFrame = 1;
		}
	}
	else if (dir == 0) { //left key
		animationDirection = 0;
		x -= 7;
		if (++frameCount > frameDelay)
		{
			frameCount = 0;
			if (++curFrame > 8)
				curFrame = 1;
		}
	}
	else if (dir == 3) { //up
		animationDirection = 3;
		y -= 7;
		if (++frameCount > frameDelay) {
			frameCount = 0;
			if (++curFrame > 8)
				curFrame = 1;
		}
	}
	else if (dir == 4) { //down
		animationDirection = 4;
		y += 7;
		if (++frameCount > frameDelay) {
			frameCount = 0;
			if (++curFrame > 8)
				curFrame = 1;
		}
	}
	else {//represent that they hit the space bar and that mean direction = 0
		animationDirection = dir;
	}

	//check for collided with foreground tiles
	if (animationDirection == 0)
	{
		if (collided(x + 5, y + frameHeight / 2)) { //collision detection to the left
			x = oldx;
			y = oldy;
		}

	}
	else if (animationDirection == 1)
	{
		if (collided(x + frameWidth - 5, y + frameHeight / 2)) { //collision detection to the right
			x = oldx;
			y = oldy;
		}
	}
	else if (animationDirection == 3) {
		if (collided(x + frameWidth / 2, y + 5)) { //collision detection to the top
			x = oldx;
			y = oldy;
		}
	}
	else if (animationDirection == 4) {
		if (collided(x + frameWidth / 2, y + frameHeight - 5)) { //collision detection to the bottom
			x = oldx;
			y = oldy;
		}
	}
}

bool Sprite::CollisionEndBlock()
{
	if (endValue(x + frameWidth + 1, y + frameHeight / 2))
		return true;
	else
		return false;
}
bool Sprite::CollisionDieBlock()
{
	if (dieValue(x + frameWidth / 2, y + frameHeight + 5))
		return true;
	else
		return false;
}

void Sprite::DrawSprites(int xoffset, int yoffset)
{
	int fx = (curFrame % animationColumns) * frameWidth;
	int fy = 0;

	if (animationDirection == 2) {	//idle
		fx = 0;
		fy = 0;
		al_draw_bitmap_region(image, fx, fy, frameWidth, frameHeight, x - xoffset, y - yoffset, 0);
	}
	else if (animationDirection == 0) {	//move left
		fy = frameHeight;
		al_draw_bitmap_region(image, fx, fy, frameWidth, frameHeight, x - xoffset, y - yoffset, ALLEGRO_FLIP_HORIZONTAL);
	}
	else if (animationDirection == 1) {	//move right
		fy = frameHeight;
		al_draw_bitmap_region(image, fx, fy, frameWidth, frameHeight, x - xoffset, y - yoffset, 0);
	}
	else if (animationDirection == 3) {//move up
		fy = frameHeight;

		//save sprite
		ALLEGRO_BITMAP* frame = al_create_sub_bitmap(image, fx, fy, frameWidth, frameHeight);

		//rotate that sprite up
		al_draw_rotated_bitmap(frame,frameWidth / 2.0, frameHeight / 2.0, x - xoffset + frameWidth / 2.0, y - yoffset + frameHeight / 2.0, ALLEGRO_PI / - 2, 0);
		al_destroy_bitmap(frame);
	}
	else if (animationDirection == 4) {	//move down
		fy = frameHeight;

		//save sprite
		ALLEGRO_BITMAP* frame = al_create_sub_bitmap(image, fx, fy, frameWidth, frameHeight);

		//rotate that sprite down
		al_draw_rotated_bitmap(frame, frameWidth / 2.0, frameHeight / 2.0, x - xoffset + frameWidth / 2.0, y - yoffset + frameHeight / 2.0, ALLEGRO_PI / 2, 0);
	}

}